import FWCore.ParameterSet.Config as cms

boostedTauRecoFilter = cms.EDFilter('boostedTauRecoFilter',
                                    boostedTauCollection = cms.InputTag("slimmedTausBoosted"),
                                    tauCollection = cms.InputTag("slimmedTaus")
)
