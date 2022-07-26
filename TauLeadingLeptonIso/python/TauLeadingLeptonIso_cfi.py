import FWCore.ParameterSet.Config as cms

TauLeadingLeptonIso = cms.EDProducer('TauLeadingLeptonIso',
                                            TauCollection = cms.InputTag('slimmedTaus'),
                                            electronCollection = cms.InputTag('slimmedElectrons'),
                                            muonCollection = cms.InputTag('slimmedMuons'),
                                            verboseDebug=cms.bool(False),
                                            rhoSrc = cms.InputTag("fixedGridRhoFastjetAll"),
                                            EAConfigFile = cms.FileInPath("PhysicsTools/NanoAOD/data/effAreaMuons_cone03_pfNeuHadronsAndPhotons_94X.txt"),
)
