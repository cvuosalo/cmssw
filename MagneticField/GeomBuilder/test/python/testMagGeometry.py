import FWCore.ParameterSet.Config as cms

process = cms.Process("MagneticFieldTest")

process.source = cms.Source("EmptySource")
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
    )

process.MessageLogger = cms.Service(
    "MessageLogger",
    statistics = cms.untracked.vstring('cout', 'magGeometry'),
    categories = cms.untracked.vstring('Geometry'),
    cout = cms.untracked.PSet(
        threshold = cms.untracked.string('WARNING'),
        noLineBreaks = cms.untracked.bool(True)
        ),
    magGeometry = cms.untracked.PSet(
        INFO = cms.untracked.PSet(
            limit = cms.untracked.int32(0)
            ),
        noLineBreaks = cms.untracked.bool(True),
        DEBUG = cms.untracked.PSet(
            limit = cms.untracked.int32(0)
            ),
        WARNING = cms.untracked.PSet(
            limit = cms.untracked.int32(0)
            ),
        ERROR = cms.untracked.PSet(
            limit = cms.untracked.int32(0)
            ),
        threshold = cms.untracked.string('INFO'),
        Geometry = cms.untracked.PSet(
            limit = cms.untracked.int32(-1)
            )
        ),
    destinations = cms.untracked.vstring('cout',
                                         'magGeometry')
    )

process.DDDetectorESProducer = cms.ESSource("DDDetectorESProducer",
                                            confGeomXMLFiles = cms.FileInPath('DetectorDescription/DDCMS/data/cms-mf-geometry.xml'),
                                            appendToDataLabel = cms.string('CMS')
                                            )

process.MagneticFieldESProducer = cms.ESProducer("VolumeBasedMagneticFieldESProducer",
                                              DDDetector = cms.ESInputTag('CMS'),
                                              appendToDataLabel = cms.string(''),
                                              useParametrizedTrackerField = cms.bool(True),
                                              label = cms.untracked.string(''),
                                              paramLabel = cms.string('parametrizedField'),
                                              version = cms.string('grid_160812_3_8t'),
                                              geometryVersion = cms.int32(160812),
                                              debugBuilder = cms.untracked.bool(False),
                                              cacheLastVolume = cms.untracked.bool(True),
                                              scalingVolumes = cms.vint32(),
                                              scalingFactors = cms.vdouble(),

                                              gridFiles = cms.VPSet(
                                                          cms.PSet(
                                                               volumes   = cms.string('1001-1010,1012-1027,1030-1033,1036-1041,1044-1049,1052-1057,1060-1063,1066-1071,1074-1077,1080-1097,1102-1129,1138-1402,1415-1416,' + 
                                                                                      '2001-2010,2012-2027,2030-2033,2036-2041,2044-2049,2052-2057,2060-2063,2066-2071,2074-2077,2080-2097,2102-2129,2138-2402,2415-2416'),
                                                               sectors   = cms.string('0') ,
                                                               master    = cms.int32(0),
                                                               path      = cms.string('s[s]/grid.[v].bin'),
                                                          ),
                                              # Replicate sector 1 for volumes outside any detector
                                                          cms.PSet(
                                                               volumes   = cms.string('1011,1028-1029,1034-1035,1042-1043,1050-1051,1058-1059,1064-1065,1072-1073,1078-1079,'+ # volumes extending from R~7.6 m to to R=9 m,
                                                                                      '1098-1101,1130-1137,' + # Forward volumes, ouside CASTOR/HF
                                                                                      '1403-1414,1417-1464,' # Volumes beyond |Z|>17.74
                                                                                      '2011,2028-2029,2034-2035,2042-2043,2050-2051,2058-2059,2064-2065,2072-2073,2078-2079,'+
                                                                                      '2098-2101,2130-2137,'+
                                                                                      '2403-2414,2417-2464'),
                                                               sectors   = cms.string('0'),
                                                               master    = cms.int32(1),
                                                               path      = cms.string('s01/grid.[v].bin'),
                                                          ),
                                                     )
                                               )

process.DDSpecParRegistryESProducer = cms.ESProducer("DDSpecParRegistryESProducer",
                                                     appendToDataLabel = cms.string('CMS')
                                                     )

process.test = cms.EDAnalyzer("testMagGeometryAnalyzer",
                              DDDetector = cms.ESInputTag('MUON')
                              )

process.p = cms.Path(process.test)
