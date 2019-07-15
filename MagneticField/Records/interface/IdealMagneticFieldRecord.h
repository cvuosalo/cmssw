#ifndef MagneticField_IdealMagneticFieldRecord_h
#define MagneticField_IdealMagneticFieldRecord_h

#include "FWCore/Framework/interface/EventSetupRecordImplementation.h"
#include "FWCore/Framework/interface/DependentRecordImplementation.h"
#include "CondFormats/DataRecord/interface/RunSummaryRcd.h"
#include "CondFormats/DataRecord/interface/MagFieldConfigRcd.h"
#include "CondFormats/DataRecord/interface/MFGeometryFileRcd.h"
#include "Geometry/Records/interface/DDSpecParRegistryRcd.h"
#include "Geometry/Records/interface/GeometryFileRcd.h"
#include "boost/mpl/vector.hpp"

class IdealMagneticFieldRecord : public edm::eventsetup::DependentRecordImplementation<
                                     IdealMagneticFieldRecord,
                                     boost::mpl::vector<MFGeometryFileRcd, RunInfoRcd, DDSpecParRegistryRcd,
                                     GeometryFileRcd, MagFieldConfigRcd> > {};

#endif
