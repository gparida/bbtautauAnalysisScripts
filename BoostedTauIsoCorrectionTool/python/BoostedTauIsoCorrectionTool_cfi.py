import FWCore.ParameterSet.Config as cms

BoostedTauIsoCorrectionTool = cms.EDProducer('BoostedTauIsoCorrectionTool',
                                           boostedTauCollection = cms.InputTag('slimmedTausBoosted'),
                                           electronCollection = cms.InputTag('slimmedElectrons'),
                                           muonCollection = cms.InputTag('slimmedMuons'),
                                           rhoSrc = cms.InputTag('fixedGridRhoFastjetAll'),
                                           EAConfigFile2 = cms.FileInPath("PhysicsTools/NanoAOD/data/effAreaMuons_cone03_pfNeuHadronsAndPhotons_94X.txt"),
                                           EAConfigFile = cms.FileInPath("RecoEgamma/ElectronIdentification/data/Fall17/effAreaElectrons_cone03_pfNeuHadronsAndPhotons_94X.txt"))
