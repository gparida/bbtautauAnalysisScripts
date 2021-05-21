import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

##initial setup
options = VarParsing.VarParsing ('analysis')
options.parseArguments()

process = cms.Process('Plots')

process.load('FWCore.MessageService.MessageLogger_cfi')
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.source = cms.Source('PoolSource',
                            fileNames = cms.untracked.vstring(tuple(options.inputFiles)),
                            duplicateCheckMode = cms.untracked.string("noDuplicateCheck"))

# import of standard configurations
process.load("Configuration.Geometry.GeometryRecoDB_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '106X_mcRun2_asymptotic_v13', '')

##analyzer path
process.plots = cms.EDAnalyzer('boostedTauRecoAnalyzer',
                               prunedGenParticleCollection = cms.InputTag("prunedGenParticles"),
                               boostedTauCollection = cms.InputTag("slimmedTausBoosted"),
                               tauCollection = cms.InputTag("slimmedTaus"))

#output name
process.TFileService = cms.Service('TFileService',
                                   fileName = cms.string('ntuple.root'))

process.p = cms.Path(process.plots)

