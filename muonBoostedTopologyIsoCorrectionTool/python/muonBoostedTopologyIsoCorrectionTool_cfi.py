import FWCore.ParameterSet.Config as cms

muonIsoCorrectionTool = cms.EDProducer('muonBoostedTopologyIsoCorrectionTool',
                                       boostedTauCollection = cms.InputTag('slimmedTausBoosted'),
                                       muonCollection = cms.InputTag('slimmedMuons'),
                                       verboseDebug=cms.bool(False),
                                       rhoSrc = cms.InputTag("fixedGridRhoFastjetAll"),
                                       EAConfigFile = cms.FileInPath("PhysicsTools/NanoAOD/data/effAreaMuons_cone03_pfNeuHadronsAndPhotons_94X.txt"),
)
