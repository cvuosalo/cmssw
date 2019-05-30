#include "MagneticField/GeomBuilder/plugins/dd4hep/VolumeBasedMagneticFieldESProducer.h"
#include "MagneticField/VolumeBasedEngine/interface/VolumeBasedMagneticField.h"

#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"

#include "FWCore/Framework/interface/ESTransientHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Framework/interface/ModuleFactory.h"
#include "MagneticField/GeomBuilder/plugs/dd4hep/MagGeoBuilder.h"
#include "CondFormats/MFObjects/interface/MagFieldConfig.h"

#include <string>
#include <vector>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace magneticfield;


VolumeBasedMagneticFieldESProducer::VolumeBasedMagneticFieldESProducer(const edm::ParameterSet& iConfig) :
  pset_(iConfig),
  tag_(iConfig.getParameter<ESInputTag>("DDDetector"))
{
  setWhatProduced(this, pset_.getUntrackedParameter<std::string>("label",""));
}

// ------------ method called to produce the data  ------------
std::unique_ptr<MagneticField> VolumeBasedMagneticFieldESProducer::produce(const IdealMagneticFieldRecord & iRecord)
{
  bool debug = pset_.getUntrackedParameter<bool>("debugBuilder", false);
  if (debug) {
    LogDebug("VolumeBasedMagneticFieldESProducer") << "produce()  version " << pset_.getParameter<std::string>("version") << endl;
  }
  
  edm::ESTransientHandle<DDDetector> cpv;
  record.getRecord<GeometryFileRcd>().get(tag_.module(), cpv);

  ESTransientHandle<DDSpecParRegistry> registry;
  record.getRecord<DDSpecParRegistryRcd>().get(tag_.module(), registry);
  DDSpecParRefs myReg;
  {
    BenchmarkGrd b1("DTGeometryESProducer Filter Registry");
    const string attribute{pset_.getParameter<string>("attribute")};
    const string value{pset_.getParameter<string>("value")};
    registry->filter(myReg, attribute, value);
  }
  MagGeoBuilder builder;
  MagFieldConfig conf(pset_, debug);
  // Set scaling factors
  if (!conf.keys.empty()) {
    builder.setScaling(conf.keys, conf.values);
  }
  // Set specification for the grid tables to be used.
  if (!conf.gridFiles.empty()) {
    builder.setGridFiles(conf.gridFiles);
  }
  builder.build(&(*cpv), myReg);

  // Get subordinate field (from ES)
  edm::ESHandle<MagneticField> paramField;
  if (pset_.getParameter<bool>("useParametrizedTrackerField")) {;
    iRecord.get(pset_.getParameter<string>("paramLabel"),paramField);
  }
  return std::make_unique<VolumeBasedMagneticField>(conf.geometryVersion,builder.barrelLayers(),
    builder.endcapSectors(), builder.barrelVolumes(), builder.endcapVolumes(), builder.maxR(),
    builder.maxZ(), paramField.product(), false);
}



DEFINE_FWK_EVENTSETUP_MODULE(VolumeBasedMagneticFieldESProducer);
