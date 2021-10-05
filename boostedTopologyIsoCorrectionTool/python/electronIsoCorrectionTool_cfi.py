import FWCore.ParameterSet.Config as cms
electronIsoCorrectionTool = cms.EDProducer('electronBoostedTopologyIsoCorrectionTool',
                                           boostedTauCollection = cms.InputTag('slimmedTausBoosted'),
                                           electronCollection = cms.InputTag('slimmedElectrons'),
                                           muonCollection = cms.InputTag('slimmedMuons'),
                                           tauCollection = cms.InputTag('slimmedTaus'),
                                           pfCands = cms.InputTag("packedPFCandidates"),
                                           rhoSrc = cms.InputTag('fixedGridRhoFastjetAll'),
                                           EAConfigFile = cms.FileInPath("RecoEgamma/ElectronIdentification/data/Fall17/effAreaElectrons_cone03_pfNeuHadronsAndPhotons_94X.txt"))
