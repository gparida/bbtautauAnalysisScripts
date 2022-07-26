import FWCore.ParameterSet.Config as cms

boostedTauLeadingLeptonIso = cms.EDProducer('boostedTauLeadingLeptonIso',
                                            boostedTauCollection = cms.InputTag('slimmedTausBoosted'),
                                            electronCollection = cms.InputTag('slimmedElectrons'),
                                            muonCollection = cms.InputTag('slimmedMuons'),
                                            verboseDebug=cms.bool(False),
                                            rhoSrc = cms.InputTag("fixedGridRhoFastjetAll"),
                                            EAConfigFile = cms.FileInPath("PhysicsTools/NanoAOD/data/effAreaMuons_cone03_pfNeuHadronsAndPhotons_94X.txt"),
)
