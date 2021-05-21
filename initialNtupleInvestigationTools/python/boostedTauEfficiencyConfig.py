import FWCore.ParameterSet.Config as cms


##initial setup

process = cms.Process('Plots')

process.load('FWCore.MessageService.MessageLogger_cfi')
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.source = cms.Source('PoolSource',
                            fileNames = cms.untracked.vstring('file:/data/aloeliger/preliminarybbtautauNtuples/miniAOD/step1_MiniAOD_0.root',
                                                              'file:/data/aloeliger/preliminarybbtautauNtuples/miniAOD/step1_MiniAOD_1.root'))

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('PhysicsTools.PatAlgos.slimming.metFilterPaths_cff')
process.load('Configuration.StandardSequences.PATMC_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '106X_mcRun2_asymptotic_v13', '')

##analyzer path
process.plots = cms.EDAnalyzer()

##output name
process.TFileService = cms.Service('TFileService',
                                   fileName = cms.string('ntuple.root'))

process.p = cms.Path()

