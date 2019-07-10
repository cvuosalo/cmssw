#ifndef VolBasedMagFieldESProducerDD4hep_h
#define VolBasedMagFieldESProducerDD4hep_h

/** \class VolBasedMagFieldESProducerDD4hep
 *
 *  Producer for the VolumeBasedMagneticField.
 *
 */

#include "FWCore/Framework/interface/EventSetupRecordIntervalFinder.h"
#include "FWCore/Framework/interface/ESProducer.h"

#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "Geometry/Records/interface/DDSpecParRegistryRcd.h"
#include "Geometry/Records/interface/GeometryFileRcd.h"
#include "DetectorDescription/DDCMS/interface/DDSpecParRegistry.h"
#include "DetectorDescription/DDCMS/interface/DDDetector.h"
#include "DetectorDescription/DDCMS/interface/DDFilteredView.h"

#include <string>
#include <vector>

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
    edm::ESGetToken<cms::DDDetector, GeometryFileRcd> cpvToken_;
    edm::ESGetToken<cms::DDSpecParRegistry, DDSpecParRegistryRcd> registryToken_;
    const edm::ESInputTag tag_;
  };
}  // namespace magneticfield

#endif
