import FWCore.ParameterSet.Config as cms

TauIsoCorrectionTool = cms.EDProducer('TauIsoCorrectionTool',
                                           #boostedTauCollection = cms.InputTag('slimmedTausBoosted'),
                                           TauCollection = cms.InputTag('slimmedTaus'),
                                           electronCollection = cms.InputTag('slimmedElectrons'),
                                           muonCollection = cms.InputTag('slimmedMuons'),
                                           rhoSrc = cms.InputTag('fixedGridRhoFastjetAll'),
                                           EAConfigFile2 = cms.FileInPath("PhysicsTools/NanoAOD/data/effAreaMuons_cone03_pfNeuHadronsAndPhotons_94X.txt"),
                                           EAConfigFile = cms.FileInPath("RecoEgamma/ElectronIdentification/data/Fall17/effAreaElectrons_cone03_pfNeuHadronsAndPhotons_94X.txt"))
