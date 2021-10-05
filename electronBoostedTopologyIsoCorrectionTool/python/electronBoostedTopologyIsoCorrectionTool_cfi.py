import FWCore.ParameterSet.Config as cms

electronIsoCorrectionTool = cms.EDProducer('electronBoostedTopologyIsoCorrectionTool',
                                           boostedTauCollection = cms.InputTag('slimmedTausBoosted'),
                                           electronCollection = cms.InputTag('slimmedElectrons'),
                                           rhoSrc = cms.InputTag('fixedGridRhoFastjetAll'),
                                           EAConfigFile = cms.FileInPath("RecoEgamma/ElectronIdentification/data/Fall17/effAreaElectrons_cone03_pfNeuHadronsAndPhotons_94X.txt"))
