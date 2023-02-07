import FWCore.ParameterSet.Config as cms
pfParticleNetAK8LastJetTagInfos = cms.EDProducer("ParticleNetFeatureEvaluator",
        vertices = cms.InputTag("offlineSlimmedPrimaryVertices"),
        secondary_vertices = cms.InputTag("slimmedSecondaryVertices"),
        pf_candidates = cms.InputTag("packedPFCandidates"),
        #jets = cms.InputTag("slimmedJetsAK8Calibrated" if options.applyJECs else "slimmedJetsAK8"), # should I use the JECs or slimmedJetsAK8
        jets = cms.InputTag("slimmedJetsAK8"),
        muons = cms.InputTag("slimmedMuons"),
        electrons = cms.InputTag("slimmedElectrons"),
        taus = cms.InputTag("slimmedTaus"),
        jet_radius = cms.double(0.8),
        #min_jet_pt = cms.double(options.jetPtMin),
        min_jet_pt = cms.double(180.0),
        #max_jet_eta = cms.double(options.jetEtaMax),
        max_jet_eta = cms.double(2.5),
        #min_pt_for_pfcandidates = cms.double(options.jetPFCandidatePtMin), ## arbitrary
        min_pt_for_pfcandidates = cms.double(0.1),                                                                                                            
        use_puppiP4 = cms.bool(False),
        min_puppi_wgt = cms.double(-1),
)
