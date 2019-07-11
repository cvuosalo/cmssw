/** \class VolumeBasedMagneticFieldESProducer
 *
 *  Producer for the VolumeBasedMagneticField.
 *
 */

#include "FWCore/Framework/interface/ESTransientHandle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/EventSetupRecordIntervalFinder.h"
#include "FWCore/Framework/interface/ESProducer.h"
#include "FWCore/Framework/interface/ModuleFactory.h"

#include "MagneticField/VolumeBasedEngine/interface/VolumeBasedMagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "MagneticField/Engine/interface/MagneticField.h"

#include "MagneticField/GeomBuilder/plugins/dd4hep/MagGeoBuilder.h"
#include "CondFormats/MFObjects/interface/MagFieldConfig.h"
#include "Geometry/Records/interface/DDSpecParRegistryRcd.h"
#include "Geometry/Records/interface/GeometryFileRcd.h"
#include "DetectorDescription/DDCMS/interface/BenchmarkGrd.h"
#include "DetectorDescription/DDCMS/interface/DDCompactView.h"
#include "DetectorDescription/DDCMS/interface/DDSpecParRegistry.h"
#include "DetectorDescription/DDCMS/interface/DDDetector.h"
#include "DetectorDescription/DDCMS/interface/DDFilteredView.h"

#include <string>
#include <vector>


using namespace cms;
using namespace edm;
using namespace std;
using namespace magneticfield;

namespace magneticfield {
  class VolBasedMagFieldESProducerDD4hep : public edm::ESProducer {
  public:
    VolBasedMagFieldESProducerDD4hep(const edm::ParameterSet& iConfig);

    std::unique_ptr<MagneticField> produce(const IdealMagneticFieldRecord& iRecord);

  private:
    // forbid copy ctor and assignment op.
    VolBasedMagFieldESProducerDD4hep(const VolBasedMagFieldESProducerDD4hep&) = delete;
    const VolBasedMagFieldESProducerDD4hep& operator=(const VolBasedMagFieldESProducerDD4hep&) = delete;

    edm::ParameterSet pset_;
    const bool debug_;
    const bool useParametrizedTrackerField_;
    const MagFieldConfig conf_;
    const std::string version_;
    edm::ESGetToken<MagneticField, IdealMagneticFieldRecord> paramFieldToken_;
    edm::ESGetToken<DDCompactView, IdealMagneticFieldRecord> cpvToken_;
    // edm::ESGetToken<cms::DDDetector, GeometryFileRcd> cpvToken_;
    edm::ESGetToken<cms::DDSpecParRegistry, DDSpecParRegistryRcd> registryToken_;
    const edm::ESInputTag tag_;
  };
}  // namespace magneticfield



VolBasedMagFieldESProducerDD4hep::VolBasedMagFieldESProducerDD4hep(const edm::ParameterSet& iConfig)
    : pset_{iConfig}, 
      debug_{iConfig.getUntrackedParameter<bool>("debugBuilder", false)},
      useParametrizedTrackerField_{iConfig.getParameter<bool>("useParametrizedTrackerField")},
      conf_{iConfig, debug_},
      version_{iConfig.getParameter<std::string>("version")},
      tag_{iConfig.getParameter<ESInputTag>("DDDetector")}
{
  auto cc = setWhatProduced(this, iConfig.getUntrackedParameter<std::string>("label", ""));
  cc.setConsumes(cpvToken_);
  registryToken_ =  cc.consumesFrom<DDSpecParRegistry, DDSpecParRegistryRcd>(tag_);
  if (useParametrizedTrackerField_) {
    cc.setConsumes(paramFieldToken_);
  }
}

// ------------ method called to produce the data  ------------
std::unique_ptr<MagneticField> VolBasedMagFieldESProducerDD4hep::produce(const IdealMagneticFieldRecord& iRecord) {
  if (debug_) {
    edm::LogPrint("VolBasedMagFieldESProducerDD4hep") << "VolBasedMagFieldESProducerDD4hep::produce() " << version_;
  }

  MagGeoBuilder builder(conf_.version, conf_.geometryVersion, debug_);

  // Set scaling factors
  if (!conf_.keys.empty()) {
    builder.setScaling(conf_.keys, conf_.values);
  }

  // Set specification for the grid tables to be used.
  if (!conf_.gridFiles.empty()) {
    builder.setGridFiles(conf_.gridFiles);
  }

  auto cpv = iRecord.getTransientHandle(cpvToken_);
  const DDCompactView* cpvPtr = cpv.product();
  const DDDetector* det = cpvPtr->detector();
  ESTransientHandle<DDSpecParRegistry> registry = iRecord.getTransientHandle(registryToken_);
  DDSpecParRefs myReg;
  {
    BenchmarkGrd b1("VolBasedMagFieldESProducerDD4hep Filter Registry");
    const string attribute{pset_.getParameter<string>("attribute")};
    const string value{pset_.getParameter<string>("value")};
    registry->filter(myReg, attribute, value);
  }
  builder.build(det, myReg);

  // Get slave field (from ES)
  const MagneticField* paramField = nullptr;
  if (useParametrizedTrackerField_) {
    paramField = &iRecord.get(paramFieldToken_);
  }
  return std::make_unique<VolumeBasedMagneticField>(conf_.geometryVersion,
                                                    builder.barrelLayers(),
                                                    builder.endcapSectors(),
                                                    builder.barrelVolumes(),
                                                    builder.endcapVolumes(),
                                                    builder.maxR(),
                                                    builder.maxZ(),
                                                    paramField,
                                                    false);
}

DEFINE_FWK_EVENTSETUP_MODULE(VolBasedMagFieldESProducerDD4hep);
