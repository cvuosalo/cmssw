import FWCore.ParameterSet.Config as cms

rivetAnalyzer = cms.EDAnalyzer('RivetAnalyzer',
  AnalysisNames = cms.vstring('CMS_2010_S8808686', 'MC_DIPHOTON', 'MC_JETS', 'MC_GENERIC'),
  HepMCCollection = cms.InputTag('generator:unsmeared'),
  UseExternalWeight = cms.bool(False),
  useGENweights = cms.bool(False),
  GENweightNumber = cms.int32(0),
  GenEventInfoCollection = cms.InputTag('generator'),
  genLumiInfo = cms.InputTag("generator"),
  useLHEweights = cms.bool(False),
  LHEweightNumber = cms.int32(0),
  LHECollection = cms.InputTag('source'),
  CrossSection = cms.double(-1),
  DoFinalize = cms.bool(True),
  ProduceDQMOutput = cms.bool(False),
  OutputFile = cms.string('out.aida')
)
