import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

process = cms.Process('IsoCorrection')

process.load('FWCore.MessageService.MessageLogger_cfi')
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.source = cms.Source('PoolSource',fileNames=cms.untracked.vstring('/store/mc/RunIISummer20UL16MiniAOD/RadionTohhTohtatahbb_narrow_M-4500_TuneCP5_13TeV-madgraph-pythia8/MINIAODSIM/106X_mcRun2_asymptotic_v13-v2/270000/2F58DACF-8A1D-A244-A50D-FA9B05849E66.root'))

#load standard configurations used by the nanoAOD makeratiohistograms
process.load("Configuration.Geometry.GeometryRecoDB_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '106X_mcRun2_asymptotic_v13', '')

##analyzer path
process.isoCorrectionTool = cms.EDAnalyzer('electronBoostedTopologyIsoCorrectionTool',
                                           boostedTauCollection = cms.InputTag('slimmedTausBoosted'),
                                           electronCollection = cms.InputTag('slimmedElectrons'),
                                           muonCollection = cms.InputTag('slimmedMuons'),
                                           tauCollection = cms.InputTag('slimmedTaus'),
                                           pfCands = cms.InputTag("packedPFCandidates"),
                                           rhoSrc = cms.InputTag('fixedGridRhoFastjetAll'),
                                           EAConfigFile = cms.FileInPath("RecoEgamma/ElectronIdentification/data/Fall17/effAreaElectrons_cone03_pfNeuHadronsAndPhotons_94X.txt"))

#embed the electron effective area into collection

process.p = cms.Path(process.isoCorrectionTool)
