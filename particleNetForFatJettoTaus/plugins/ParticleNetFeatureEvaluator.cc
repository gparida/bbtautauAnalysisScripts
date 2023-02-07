#include <TVector3.h>
#include <TTree.h>

#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"
#include "FWCore/Utilities/interface/ESGetToken.h"

#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"

#include "RecoBTag/FeatureTools/interface/TrackInfoBuilder.h"
#include "RecoBTag/FeatureTools/interface/deep_helpers.h"
#include "RecoBTag/FeatureTools/interface/sorting_modules.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"

#include "DataFormats/Candidate/interface/VertexCompositePtrCandidate.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/BTauReco/interface/DeepBoostedJetTagInfo.h"

using namespace btagbtvdeep;

class ParticleNetFeatureEvaluator : public edm::stream::EDProducer<> {
public:
  explicit ParticleNetFeatureEvaluator(const edm::ParameterSet &);
  ~ParticleNetFeatureEvaluator() override;

  static void fillDescriptions(edm::ConfigurationDescriptions &descriptions);

private:

  void beginStream(edm::StreamID) override {}
  void produce(edm::Event &, const edm::EventSetup &) override;
  void endStream() override {}
  void fillParticleFeatures(DeepBoostedJetFeatures & fts, 
			    const reco::Jet & jet, 
			    const std::vector<math::XYZTLorentzVector> & tau_pfcandidates,
			    const pat::MuonCollection & muons, 
			    const pat::ElectronCollection & electrons,
			    const pat::PhotonCollection & photons
			    );
  void fillSVFeatures(DeepBoostedJetFeatures & fts, 
		      const reco::Jet & jet);
  void fillLostTrackFeatures(DeepBoostedJetFeatures & fts, 
			     const reco::Jet & jet);
  bool useTrackProperties(const pat::PackedCandidate* cand);

  const double jet_radius_;
  const double min_jet_pt_;
  const double max_jet_eta_;
  const double min_jet_eta_;
  const double min_pt_for_track_properties_;
  const double min_pt_for_pfcandidates_;
  const double min_pt_for_losttrack_;
  const double max_dr_for_losttrack_;
  const double min_pt_for_taus_;
  const double max_eta_for_taus_;
  const bool   use_puppiP4_;
  const double min_puppi_wgt_;
  const bool   include_neutrals_;
  const bool   dump_feature_tree_;

  edm::EDGetTokenT<pat::MuonCollection > muon_token_;
  edm::EDGetTokenT<pat::ElectronCollection > electron_token_;
  edm::EDGetTokenT<pat::PhotonCollection > photon_token_;
  edm::EDGetTokenT<pat::TauCollection > tau_token_;
  edm::EDGetTokenT<edm::View<reco::Jet> > jet_token_;
  edm::EDGetTokenT<pat::PackedCandidateCollection > losttrack_token_;
  edm::EDGetTokenT<reco::VertexCollection> vtx_token_;
  edm::EDGetTokenT<reco::VertexCompositePtrCandidateCollection> sv_token_;
  edm::EDGetTokenT<edm::View<reco::Candidate> > pfcand_token_;
  edm::ESGetToken<TransientTrackBuilder, TransientTrackRecord> track_builder_token_;
  
  edm::Handle<reco::VertexCollection> vtxs_;
  edm::Handle<reco::VertexCompositePtrCandidateCollection> svs_;
  edm::Handle<edm::View<reco::Candidate> > pfcands_;
  edm::Handle<pat::PackedCandidateCollection > losttracks_;
  edm::ESHandle<TransientTrackBuilder> track_builder_;

  const static std::vector<std::string> particle_features_;
  const static std::vector<std::string> sv_features_;
  const static std::vector<std::string> losttrack_features_;
  const reco::Vertex *pv_ = nullptr;

  TTree* tree;
  unsigned int event;
  float jet_pt;
  float jet_pt_raw;
  float jet_eta;
  float jet_phi;
  float jet_mass;
  unsigned int ijet;
  std::vector<float> jet_pfcand_pt_log;
  std::vector<float> jet_pfcand_energy_log;
  std::vector<float> jet_pfcand_deta;
  std::vector<float> jet_pfcand_dphi;
  std::vector<float> jet_pfcand_eta ;
  std::vector<float> jet_pfcand_charge;
  std::vector<float> jet_pfcand_frompv;
  std::vector<float> jet_pfcand_nlostinnerhits;
  std::vector<float> jet_pfcand_track_chi2;
  std::vector<float> jet_pfcand_track_qual;
  std::vector<float> jet_pfcand_dz;
  std::vector<float> jet_pfcand_dzsig;
  std::vector<float> jet_pfcand_dxy;
  std::vector<float> jet_pfcand_dxysig;
  std::vector<float> jet_pfcand_etarel;
  std::vector<float> jet_pfcand_pperp_ratio;
  std::vector<float> jet_pfcand_ppara_ratio;
  std::vector<float> jet_pfcand_trackjet_d3d;
  std::vector<float> jet_pfcand_trackjet_d3dsig;
  std::vector<float> jet_pfcand_trackjet_dist;
  std::vector<float> jet_pfcand_npixhits;
  std::vector<float> jet_pfcand_nstriphits;
  std::vector<float> jet_pfcand_trackjet_decayL;
  std::vector<float> jet_pfcand_id;
  std::vector<float> jet_pfcand_calofraction;
  std::vector<float> jet_pfcand_hcalfraction;
  std::vector<float> jet_pfcand_puppiw;
  std::vector<float> jet_pfcand_muon_id;
  std::vector<float> jet_pfcand_muon_isglobal;
  std::vector<float> jet_pfcand_muon_chi2;
  std::vector<float> jet_pfcand_muon_segcomp;
  std::vector<float> jet_pfcand_muon_nvalidhit;
  std::vector<float> jet_pfcand_muon_nstation;
  std::vector<float> jet_pfcand_electron_detaIn;
  std::vector<float> jet_pfcand_electron_dphiIn;
  std::vector<float> jet_pfcand_electron_sigIetaIeta;
  std::vector<float> jet_pfcand_electron_sigIphiIphi;
  std::vector<float> jet_pfcand_electron_r9;
  std::vector<float> jet_pfcand_electron_convProb;
  std::vector<float> jet_pfcand_photon_sigIetaIeta;
  std::vector<float> jet_pfcand_photon_r9;
  std::vector<float> jet_pfcand_photon_eVeto;
  std::vector<float> jet_pfcand_tau_signal;
  std::vector<float> jet_sv_pt_log;
  std::vector<float> jet_sv_mass;
  std::vector<float> jet_sv_deta;
  std::vector<float> jet_sv_dphi;
  std::vector<float> jet_sv_eta;
  std::vector<float> jet_sv_ntrack;
  std::vector<float> jet_sv_chi2;
  std::vector<float> jet_sv_dxy;
  std::vector<float> jet_sv_dxysig;
  std::vector<float> jet_sv_d3d;
  std::vector<float> jet_sv_d3dsig;
  std::vector<float> jet_losttrack_pt_log;
  std::vector<float> jet_losttrack_eta;
  std::vector<float> jet_losttrack_deta;
  std::vector<float> jet_losttrack_dphi;
  std::vector<float> jet_losttrack_charge;
  std::vector<float> jet_losttrack_frompv;
  std::vector<float> jet_losttrack_track_chi2;
  std::vector<float> jet_losttrack_track_qual;
  std::vector<float> jet_losttrack_dz;
  std::vector<float> jet_losttrack_dxy;
  std::vector<float> jet_losttrack_dzsig;
  std::vector<float> jet_losttrack_dxysig;
  std::vector<float> jet_losttrack_etarel;
  std::vector<float> jet_losttrack_trackjet_d3d;
  std::vector<float> jet_losttrack_trackjet_d3dsig;
  std::vector<float> jet_losttrack_trackjet_dist;
  std::vector<float> jet_losttrack_trackjet_decayL;
  std::vector<float> jet_losttrack_npixhits;
  std::vector<float> jet_losttrack_nstriphits;  

};

const std::vector<std::string> ParticleNetFeatureEvaluator::particle_features_{ "jet_pfcand_pt_log", "jet_pfcand_energy_log", "jet_pfcand_deta", "jet_pfcand_dphi", "jet_pfcand_eta", "jet_pfcand_charge", "jet_pfcand_frompv", "jet_pfcand_nlostinnerhits", "jet_pfcand_track_chi2", "jet_pfcand_track_qual", "jet_pfcand_dz", "jet_pfcand_dzsig", "jet_pfcand_dxy", "jet_pfcand_dxysig", "jet_pfcand_etarel", "jet_pfcand_pperp_ratio", "jet_pfcand_ppara_ratio", "jet_pfcand_trackjet_d3d", "jet_pfcand_trackjet_d3dsig", "jet_pfcand_trackjet_dist", "jet_pfcand_nhits", "jet_pfcand_npixhits", "jet_pfcand_nstriphits", "jet_pfcand_trackjet_decayL", "jet_pfcand_id", "jet_pfcand_calofraction", "jet_pfcand_hcalfraction", "jet_pfcand_puppiw", "jet_pfcand_muon_id", "jet_pfcand_muon_isglobal", "jet_pfcand_muon_segcomp", "jet_pfcand_muon_chi2", "jet_pfcand_muon_nvalidhit", "jet_pfcand_muon_nstation", "jet_pfcand_electron_detaIn", "jet_pfcand_electron_dphiIn", "jet_pfcand_electron_sigIetaIeta", "jet_pfcand_electron_sigIphiIphi", "jet_pfcand_electron_r9", "jet_pfcand_electron_convProb", "jet_pfcand_photon_sigIetaIeta", "jet_pfcand_photon_r9", "jet_pfcand_photon_eVeto", "jet_pfcand_tau_signal", "pfcand_mask"};

const std::vector<std::string> ParticleNetFeatureEvaluator::sv_features_{"jet_sv_pt_log","jet_sv_mass","jet_sv_deta","jet_sv_dphi", "jet_sv_eta", "jet_sv_ntrack", "jet_sv_chi2", "jet_sv_dxy", "jet_sv_dxysig", "jet_sv_d3d", "jet_sv_d3dsig", "sv_mask"};

const std::vector<std::string> ParticleNetFeatureEvaluator::losttrack_features_{"jet_losttrack_pt_log","jet_losttrack_eta","jet_losttrack_deta","jet_losttrack_dphi","jet_losttrack_charge","jet_losttrack_frompv","jet_losttrack_track_chi2","jet_losttrack_track_qual","jet_losttrack_dz","jet_losttrack_dxy","jet_losttrack_dzsig","jet_losttrack_dxysig","jet_losttrack_etarel","jet_losttrack_trackjet_d3d","jet_losttrack_trackjet_d3dsig","jet_losttrack_trackjet_dist","jet_losttrack_trackjet_decayL","jet_losttrack_npixhits","jet_losttrack_nstriphits","lt_mask"};

ParticleNetFeatureEvaluator::ParticleNetFeatureEvaluator(const edm::ParameterSet &iConfig):
  jet_radius_(iConfig.getParameter<double>("jet_radius")),
  min_jet_pt_(iConfig.getParameter<double>("min_jet_pt")),
  max_jet_eta_(iConfig.getParameter<double>("max_jet_eta")),
  min_jet_eta_(iConfig.getParameter<double>("min_jet_eta")),
  min_pt_for_track_properties_(iConfig.getParameter<double>("min_pt_for_track_properties")),
  min_pt_for_pfcandidates_(iConfig.getParameter<double>("min_pt_for_pfcandidates")),
  min_pt_for_losttrack_(iConfig.getParameter<double>("min_pt_for_losttrack")),
  max_dr_for_losttrack_(iConfig.getParameter<double>("max_dr_for_losttrack")),
  min_pt_for_taus_(iConfig.getParameter<double>("min_pt_for_taus")),
  max_eta_for_taus_(iConfig.getParameter<double>("max_eta_for_taus")),
  use_puppiP4_(iConfig.getParameter<bool>("use_puppiP4")),
  min_puppi_wgt_(iConfig.getParameter<double>("min_puppi_wgt")),
  include_neutrals_(iConfig.getParameter<bool>("include_neutrals")),
  dump_feature_tree_(iConfig.getParameter<bool>("dump_feature_tree")),
  muon_token_(consumes<pat::MuonCollection >(iConfig.getParameter<edm::InputTag>("muons"))),
  electron_token_(consumes<pat::ElectronCollection >(iConfig.getParameter<edm::InputTag>("electrons"))),
  photon_token_(consumes<pat::PhotonCollection >(iConfig.getParameter<edm::InputTag>("photons"))),
  tau_token_(consumes<pat::TauCollection>(iConfig.getParameter<edm::InputTag>("taus"))),
  jet_token_(consumes<edm::View<reco::Jet> >(iConfig.getParameter<edm::InputTag>("jets"))),
  losttrack_token_(consumes<pat::PackedCandidateCollection>(iConfig.getParameter<edm::InputTag>("losttracks"))),
  vtx_token_(consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertices"))),
  sv_token_(consumes<reco::VertexCompositePtrCandidateCollection>(iConfig.getParameter<edm::InputTag>("secondary_vertices"))),
  pfcand_token_(consumes<edm::View<reco::Candidate> >(iConfig.getParameter<edm::InputTag>("pf_candidates"))),
  track_builder_token_(esConsumes<TransientTrackBuilder, TransientTrackRecord>(edm::ESInputTag("", "TransientTrackBuilder"))) {

  produces<std::vector<reco::DeepBoostedJetTagInfo>>();

  if(dump_feature_tree_){

    edm::Service<TFileService> fs;
    tree = fs->make<TTree>("tree","tree");

    tree->Branch("event", &event, "event/i");
    tree->Branch("jet_pt", &jet_pt, "jet_pt/F");
    tree->Branch("jet_pt_raw", &jet_pt_raw, "jet_pt_raw/F");
    tree->Branch("jet_eta", &jet_eta, "jet_eta/F");
    tree->Branch("jet_phi", &jet_eta, "jet_phi/F");
    tree->Branch("jet_mass", &jet_eta, "jet_mass/F");
    tree->Branch("ijet", &ijet, "ijet/i");

    tree->Branch("jet_pfcand_pt_log","std::vector<float>", &jet_pfcand_pt_log);
    tree->Branch("jet_pfcand_energy_log","std::vector<float>", &jet_pfcand_energy_log);
    tree->Branch("jet_pfcand_deta","std::vector<float>", &jet_pfcand_deta);
    tree->Branch("jet_pfcand_dphi","std::vector<float>", &jet_pfcand_dphi);
    tree->Branch("jet_pfcand_eta","std::vector<float>", &jet_pfcand_eta);
    tree->Branch("jet_pfcand_charge","std::vector<float>", &jet_pfcand_charge);
    tree->Branch("jet_pfcand_frompv","std::vector<float>", &jet_pfcand_frompv);
    tree->Branch("jet_pfcand_dz","std::vector<float>", &jet_pfcand_dz);
    tree->Branch("jet_pfcand_dzsig","std::vector<float>", &jet_pfcand_dzsig);
    tree->Branch("jet_pfcand_dxy","std::vector<float>", &jet_pfcand_dxy);
    tree->Branch("jet_pfcand_dxysig","std::vector<float>", &jet_pfcand_dxysig);
    tree->Branch("jet_pfcand_nlostinnerhits","std::vector<float>", &jet_pfcand_nlostinnerhits);
    tree->Branch("jet_pfcand_track_chi2","std::vector<float>", &jet_pfcand_track_chi2);
    tree->Branch("jet_pfcand_track_qual","std::vector<float>", &jet_pfcand_track_qual);
    tree->Branch("jet_pfcand_etarel","std::vector<float>", &jet_pfcand_etarel);
    tree->Branch("jet_pfcand_pperp_ratio","std::vector<float>", &jet_pfcand_pperp_ratio);
    tree->Branch("jet_pfcand_ppara_ratio","std::vector<float>", &jet_pfcand_ppara_ratio);
    tree->Branch("jet_pfcand_trackjet_d3d","std::vector<float>", &jet_pfcand_trackjet_d3d);
    tree->Branch("jet_pfcand_trackjet_d3dsig","std::vector<float>", &jet_pfcand_trackjet_d3dsig);
    tree->Branch("jet_pfcand_trackjet_dist","std::vector<float>", &jet_pfcand_trackjet_dist);
    tree->Branch("jet_pfcand_trackjet_decayL","std::vector<float>", &jet_pfcand_trackjet_decayL);
    tree->Branch("jet_pfcand_npixhits","std::vector<float>", &jet_pfcand_npixhits);
    tree->Branch("jet_pfcand_nstriphits","std::vector<float>", &jet_pfcand_nstriphits);
    tree->Branch("jet_pfcand_id","std::vector<float>", &jet_pfcand_id);
    tree->Branch("jet_pfcand_calofraction","std::vector<float>", &jet_pfcand_calofraction);
    tree->Branch("jet_pfcand_hcalfraction","std::vector<float>", &jet_pfcand_hcalfraction);
    tree->Branch("jet_pfcand_muon_id","std::vector<float>", &jet_pfcand_muon_id);
    tree->Branch("jet_pfcand_muon_isglobal","std::vector<float>", &jet_pfcand_muon_isglobal);
    tree->Branch("jet_pfcand_muon_chi2","std::vector<float>", &jet_pfcand_muon_chi2);
    tree->Branch("jet_pfcand_muon_segcomp","std::vector<float>", &jet_pfcand_muon_segcomp);
    tree->Branch("jet_pfcand_muon_nvalidhit","std::vector<float>", &jet_pfcand_muon_nvalidhit);
    tree->Branch("jet_pfcand_muon_nstation","std::vector<float>", &jet_pfcand_muon_nstation);
    tree->Branch("jet_pfcand_electron_detaIn","std::vector<float>", &jet_pfcand_electron_detaIn);
    tree->Branch("jet_pfcand_electron_dphiIn","std::vector<float>", &jet_pfcand_electron_dphiIn);
    tree->Branch("jet_pfcand_electron_sigIetaIeta","std::vector<float>", &jet_pfcand_electron_sigIetaIeta);
    tree->Branch("jet_pfcand_electron_sigIphiIphi","std::vector<float>", &jet_pfcand_electron_sigIphiIphi);
    tree->Branch("jet_pfcand_electron_r9","std::vector<float>", &jet_pfcand_electron_r9);
    tree->Branch("jet_pfcand_electron_convProb","std::vector<float>", &jet_pfcand_electron_convProb);
    tree->Branch("jet_pfcand_photon_sigIetaIeta","std::vector<float>", &jet_pfcand_photon_sigIetaIeta);
    tree->Branch("jet_pfcand_photon_r9","std::vector<float>", &jet_pfcand_photon_r9);
    tree->Branch("jet_pfcand_photon_eVeto","std::vector<float>", &jet_pfcand_photon_eVeto);
    tree->Branch("jet_pfcand_tau_signal","std::vector<float>", &jet_pfcand_tau_signal);
    tree->Branch("jet_pfcand_puppiw","std::vector<float>", &jet_pfcand_puppiw);
    
    tree->Branch("jet_sv_pt_log","std::vector<float>", &jet_sv_pt_log);
    tree->Branch("jet_sv_mass","std::vector<float>", &jet_sv_mass);
    tree->Branch("jet_sv_deta","std::vector<float>", &jet_sv_deta);
    tree->Branch("jet_sv_dphi","std::vector<float>", &jet_sv_dphi);
    tree->Branch("jet_sv_eta","std::vector<float>", &jet_sv_eta);
    tree->Branch("jet_sv_ntrack","std::vector<float>", &jet_sv_ntrack);
    tree->Branch("jet_sv_chi2","std::vector<float>", &jet_sv_chi2);
    tree->Branch("jet_sv_dxy","std::vector<float>", &jet_sv_dxy);
    tree->Branch("jet_sv_dxysig","std::vector<float>", &jet_sv_dxysig);
    tree->Branch("jet_sv_d3d","std::vector<float>", &jet_sv_d3d);
    tree->Branch("jet_sv_d3dsig","std::vector<float>", &jet_sv_d3dsig);

    tree->Branch("jet_losttrack_pt_log","std::vector<float>", &jet_losttrack_pt_log);
    tree->Branch("jet_losttrack_eta","std::vector<float>", &jet_losttrack_eta);
    tree->Branch("jet_losttrack_deta","std::vector<float>", &jet_losttrack_deta);
    tree->Branch("jet_losttrack_dphi","std::vector<float>", &jet_losttrack_dphi);
    tree->Branch("jet_losttrack_charge","std::vector<float>", &jet_losttrack_charge);
    tree->Branch("jet_losttrack_frompv","std::vector<float>", &jet_losttrack_frompv);
    tree->Branch("jet_losttrack_dz","std::vector<float>", &jet_losttrack_dz);
    tree->Branch("jet_losttrack_dxy","std::vector<float>", &jet_losttrack_dxy);
    tree->Branch("jet_losttrack_dzsig","std::vector<float>", &jet_losttrack_dzsig);
    tree->Branch("jet_losttrack_dxysig","std::vector<float>", &jet_losttrack_dxysig);
    tree->Branch("jet_losttrack_etarel","std::vector<float>", &jet_losttrack_etarel);
    tree->Branch("jet_losttrack_npixhits","std::vector<float>", &jet_losttrack_npixhits);
    tree->Branch("jet_losttrack_nstriphits","std::vector<float>", &jet_losttrack_nstriphits);
    tree->Branch("jet_losttrack_track_chi2","std::vector<float>", &jet_losttrack_track_chi2);
    tree->Branch("jet_losttrack_track_qual","std::vector<float>", &jet_losttrack_track_qual);
    tree->Branch("jet_losttrack_trackjet_d3d","std::vector<float>", &jet_losttrack_trackjet_d3d);
    tree->Branch("jet_losttrack_trackjet_d3dsig","std::vector<float>", &jet_losttrack_trackjet_d3dsig);
    tree->Branch("jet_losttrack_trackjet_decayL","std::vector<float>", &jet_losttrack_trackjet_decayL);
  }
}

ParticleNetFeatureEvaluator::~ParticleNetFeatureEvaluator() {}

void ParticleNetFeatureEvaluator::fillDescriptions(edm::ConfigurationDescriptions &descriptions) {
  // pfDeepBoostedJetTagInfos
  edm::ParameterSetDescription desc;
  desc.add<double>("jet_radius", 0.8);
  desc.add<double>("min_jet_pt", 150);
  desc.add<double>("max_jet_eta", 99);
  desc.add<double>("min_jet_eta", 0.0);
  desc.add<double>("min_pt_for_track_properties", -1);
  desc.add<double>("min_pt_for_pfcandidates", -1);
  desc.add<double>("min_pt_for_losttrack", 1);
  desc.add<double>("max_dr_for_losttrack", 0.4);
  desc.add<double>("min_pt_for_taus",20.);
  desc.add<double>("max_eta_for_taus",2.4);
  desc.add<bool>("use_puppiP4", false);
  desc.add<bool>("include_neutrals", true);
  desc.add<double>("min_puppi_wgt", -1.);
  desc.add<bool>("dump_feature_tree", false);
  desc.add<edm::InputTag>("vertices", edm::InputTag("offlineSlimmedPrimaryVertices"));
  desc.add<edm::InputTag>("secondary_vertices", edm::InputTag("slimmedSecondaryVertices"));
  desc.add<edm::InputTag>("pf_candidates", edm::InputTag("packedPFCandidates"));
  desc.add<edm::InputTag>("losttracks", edm::InputTag("lostTracks"));
  desc.add<edm::InputTag>("jets", edm::InputTag("slimmedJetsAK8"));
  desc.add<edm::InputTag>("muons", edm::InputTag("slimmedMuons"));
  desc.add<edm::InputTag>("taus", edm::InputTag("slimmedTaus"));
  desc.add<edm::InputTag>("electrons", edm::InputTag("slimmedElectrons"));
  desc.add<edm::InputTag>("photons", edm::InputTag("slimmedPhotons"));
  descriptions.add("ParticleNetFeatureEvaluator", desc);
}

void ParticleNetFeatureEvaluator::produce(edm::Event &iEvent, const edm::EventSetup &iSetup) {

  if(dump_feature_tree_){
    event = iEvent.id().event();
    jet_pt = 0;
    jet_pt_raw = 0;
    jet_eta = 0;
    jet_phi = 0;
    jet_mass = 0;
    ijet = 0;
  }
   std::cout<<"pfNET plugin inserted by MEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE"<<std::endl;
  // output collection
  auto output_tag_infos = std::make_unique<std::vector<reco::DeepBoostedJetTagInfo>>();
  // Input jets
  auto jets = iEvent.getHandle(jet_token_);
  // Input muons
  auto muons = iEvent.getHandle(muon_token_);  
  // Input taus
  auto taus = iEvent.getHandle(tau_token_);  
  // Input electrons
  auto electrons = iEvent.getHandle(electron_token_);
  // Input photons
  auto photons = iEvent.getHandle(photon_token_);
  // Input lost tracks
  iEvent.getByToken(losttrack_token_, losttracks_);
  // Primary vertexes
  iEvent.getByToken(vtx_token_, vtxs_);
  if (vtxs_->empty()) {
    // produce empty TagInfos in case no primary vertex
    iEvent.put(std::move(output_tag_infos));
    return;  // exit event
  }
  // Leading vertex
  pv_ = &vtxs_->at(0);
  // Secondary vertexs
  iEvent.getByToken(sv_token_, svs_);
  // PF candidates
  iEvent.getByToken(pfcand_token_, pfcands_);
  // Track builder
  track_builder_ = iSetup.getHandle(track_builder_token_);

  // tau signal candidates
  std::vector<math::XYZTLorentzVector> tau_pfcandidates;
  for (size_t itau = 0; itau < taus->size(); itau++) {
    if(taus->at(itau).pt() < min_pt_for_taus_) continue;
    if(fabs(taus->at(itau).eta()) > max_eta_for_taus_) continue;
    for(unsigned ipart = 0; ipart < taus->at(itau).signalCands().size(); ipart++){
      const pat::PackedCandidate* pfcand = dynamic_cast<const pat::PackedCandidate*> (taus->at(itau).signalCands()[ipart].get());
      tau_pfcandidates.push_back(pfcand->p4());
    }
  }
  
  // Loop over jet
  for (std::size_t jet_n = 0; jet_n < jets->size(); jet_n++) {
    if(dump_feature_tree_){
      jet_pfcand_pt_log.clear();
      jet_pfcand_energy_log.clear();
      jet_pfcand_deta.clear();
      jet_pfcand_dphi.clear();
      jet_pfcand_eta .clear();
      jet_pfcand_charge.clear();
      jet_pfcand_frompv.clear();
      jet_pfcand_nlostinnerhits.clear();
      jet_pfcand_track_chi2.clear();
      jet_pfcand_track_qual.clear();
      jet_pfcand_dz.clear();
      jet_pfcand_dzsig.clear();
      jet_pfcand_dxy.clear();
      jet_pfcand_dxysig.clear();
      jet_pfcand_etarel.clear();
      jet_pfcand_pperp_ratio.clear();
      jet_pfcand_ppara_ratio.clear();
      jet_pfcand_trackjet_d3d.clear();
      jet_pfcand_trackjet_d3dsig.clear();
      jet_pfcand_trackjet_dist.clear();
      jet_pfcand_npixhits.clear();
      jet_pfcand_nstriphits.clear();
      jet_pfcand_trackjet_decayL.clear();
      jet_pfcand_id.clear();
      jet_pfcand_puppiw.clear();
      jet_pfcand_calofraction.clear();
      jet_pfcand_hcalfraction.clear();
      jet_pfcand_muon_id.clear();
      jet_pfcand_muon_isglobal.clear();
      jet_pfcand_muon_chi2.clear();
      jet_pfcand_muon_segcomp.clear();
      jet_pfcand_muon_nvalidhit.clear();
      jet_pfcand_muon_nstation.clear();
      jet_pfcand_electron_detaIn.clear();
      jet_pfcand_electron_dphiIn.clear();
      jet_pfcand_electron_sigIetaIeta.clear();
      jet_pfcand_electron_sigIphiIphi.clear();
      jet_pfcand_electron_r9.clear();
      jet_pfcand_electron_convProb.clear();
      jet_pfcand_tau_signal.clear();
      jet_pfcand_photon_sigIetaIeta.clear();
      jet_pfcand_photon_r9.clear();
      jet_pfcand_photon_eVeto.clear();
      jet_sv_pt_log.clear();
      jet_sv_mass.clear();
      jet_sv_deta.clear();
      jet_sv_dphi.clear();
      jet_sv_eta.clear();
      jet_sv_ntrack.clear();
      jet_sv_chi2.clear();
      jet_sv_dxy.clear();
      jet_sv_dxysig.clear();
      jet_sv_d3d.clear();
      jet_sv_d3dsig.clear();
      jet_losttrack_pt_log.clear();
      jet_losttrack_eta.clear();
      jet_losttrack_deta.clear();
      jet_losttrack_dphi.clear();
      jet_losttrack_charge.clear();
      jet_losttrack_frompv.clear();
      jet_losttrack_track_chi2.clear();
      jet_losttrack_track_qual.clear();
      jet_losttrack_dz.clear();
      jet_losttrack_dxy.clear();
      jet_losttrack_dzsig.clear();
      jet_losttrack_dxysig.clear();
      jet_losttrack_etarel.clear();
      jet_losttrack_trackjet_d3d.clear();
      jet_losttrack_trackjet_d3dsig.clear();
      jet_losttrack_trackjet_decayL.clear();
      jet_losttrack_npixhits.clear();
      jet_losttrack_nstriphits.clear();  
    }
    
    const auto & jet = (*jets)[jet_n];        
    edm::RefToBase<reco::Jet> jet_ref(jets, jet_n);

    // create jet features
    DeepBoostedJetFeatures features;
    for (const auto &name : particle_features_) 
      features.add(name);
    for (const auto &name : sv_features_)
      features.add(name);

    // fill values only if above pt threshold and has daughters, otherwise left
    bool fill_vars = true;
    if ((jet.pt() < min_jet_pt_ and dynamic_cast<const pat::Jet *>(&jet)->correctedJet("Uncorrected").pt() < min_jet_pt_) or 
	std::abs(jet.eta()) > max_jet_eta_ or 
	std::abs(jet.eta()) < min_jet_eta_ )
      fill_vars = false;
    if (jet.numberOfDaughters() == 0)
      fill_vars = false;

    // fill features
    if (fill_vars) {
      fillParticleFeatures(features, jet, tau_pfcandidates, *muons, *electrons, *photons);
      fillSVFeatures(features, jet);
      fillLostTrackFeatures(features, jet);
      features.check_consistency(particle_features_);
      features.check_consistency(sv_features_);
      features.check_consistency(losttrack_features_);
    }

    if(dump_feature_tree_ and fill_vars){
      if(dynamic_cast<const pat::Jet *>(&jet)->genJet()){
	jet_pt = jet.pt();
	jet_pt_raw = dynamic_cast<const pat::Jet *>(&jet)->correctedJet("Uncorrected").pt();
	jet_eta = jet.eta();
	jet_phi = jet.phi();
	jet_mass = jet.mass();
	tree->Fill();
	ijet++;
      }
    }

    // this should always be done even if features are not filled
    output_tag_infos->emplace_back(features, jet_ref);

  }
  // move output collection
  iEvent.put(std::move(output_tag_infos));
}

bool ParticleNetFeatureEvaluator::useTrackProperties(const pat::PackedCandidate* cand) {
  const auto *track = cand->bestTrack();
  return track != nullptr and track->pt() > min_pt_for_track_properties_;
};

void ParticleNetFeatureEvaluator::fillParticleFeatures(DeepBoostedJetFeatures & fts, 
						       const reco::Jet & jet,
						       const std::vector<math::XYZTLorentzVector> & tau_pfcandidates,
						       const pat::MuonCollection & muons, 
						       const pat::ElectronCollection & electrons,
						       const pat::PhotonCollection & photons
						       ) {
  // some jet properties
  math::XYZVector jet_dir = jet.momentum().Unit();
  TVector3 jet_direction (jet.momentum().Unit().x(), jet.momentum().Unit().y(), jet.momentum().Unit().z());
  GlobalVector jet_ref_track_dir (jet.px(), jet.py(), jet.pz());
  // vertexes
  reco::VertexRefProd PVRefProd(vtxs_);
  // track builder
  TrackInfoBuilder trackinfo (track_builder_);

  // make list of pf-candidates to be considered
  std::vector<const pat::PackedCandidate*> daughters;
  for (const auto &dau : jet.daughterPtrVector()) {
    // remove particles w/ extremely low puppi weights    
    const pat::PackedCandidate* cand = dynamic_cast<const pat::PackedCandidate*>(&(*dau));
    if(not cand)
      throw edm::Exception(edm::errors::InvalidReference)
	<< "Cannot convert to either pat::PackedCandidate";
    if(cand->puppiWeight() < min_puppi_wgt_) continue;
    // base requirements on PF candidates
    if (cand->pt() < min_pt_for_pfcandidates_) continue;
    // charged candidate selection (for Higgs Interaction Net)
    if (!include_neutrals_ and (cand->charge() == 0 or cand->pt() < min_pt_for_track_properties_)) continue;
    // filling daughters
    daughters.push_back(cand);
  }

  // sort by Puppi-weighted pt
  if (use_puppiP4_)
    std::sort(daughters.begin(), daughters.end(), [&](const pat::PackedCandidate* a, const pat::PackedCandidate* b) {
	return  a->puppiWeight() * a->pt() > b->puppiWeight() * b->pt();
      });
  // sort by original pt (not Puppi-weighted)
  else
    std::sort(daughters.begin(), daughters.end(), [](const auto &a, const auto &b) { return a->pt() > b->pt();});
  
  // reserve space
  for (const auto &name : particle_features_)
    fts.reserve(name, daughters.size());

  // Build observables
  for (const auto &cand : daughters) {
    
    if (!include_neutrals_ and !useTrackProperties(cand)) continue;
    
    // input particle is a packed PF candidate
    auto candP4 = use_puppiP4_ ? cand->puppiWeight() * cand->p4() : cand->p4();
    auto candP3 = use_puppiP4_ ? cand->puppiWeight() * cand->momentum() : cand->momentum();
    
    // candidate track
    const reco::Track *track = nullptr;
    if(useTrackProperties(cand))
      track = cand->bestTrack();

    // reco-vertex association
    reco::VertexRef pv_ass = reco::VertexRef(vtxs_, 0);
    math::XYZPoint  pv_ass_pos = pv_ass->position();

    TVector3 cand_direction(candP3.x(), candP3.y(), candP3.z());
    
    fts.fill("jet_pfcand_pt_log", std::isnan(std::log(candP4.pt())) ? 0 : std::log(candP4.pt()));
    fts.fill("jet_pfcand_energy_log", std::isnan(std::log(candP4.energy())) ? 0 : std::log(candP4.energy()));
    fts.fill("jet_pfcand_eta", candP4.eta());
    fts.fill("jet_pfcand_deta", jet_direction.Eta() - cand_direction.Eta());
    fts.fill("jet_pfcand_dphi", jet_direction.DeltaPhi(cand_direction));
    fts.fill("jet_pfcand_charge", cand->charge());
    fts.fill("jet_pfcand_etarel", std::isnan(reco::btau::etaRel(jet_dir, candP3)) ? 0 : reco::btau::etaRel(jet_dir, candP3));
    fts.fill("jet_pfcand_pperp_ratio", std::isnan(jet_direction.Perp(cand_direction) / cand_direction.Mag()) ? 0 : jet_direction.Perp(cand_direction) / cand_direction.Mag());
    fts.fill("jet_pfcand_ppara_ratio", std::isnan(jet_direction.Dot(cand_direction) / cand_direction.Mag()) ? 0 : jet_direction.Dot(cand_direction) / cand_direction.Mag());
    fts.fill("jet_pfcand_frompv", cand->fromPV());
    fts.fill("jet_pfcand_dz", std::isnan(cand->dz(pv_ass_pos)) ? 0 : cand->dz(pv_ass_pos));
    fts.fill("jet_pfcand_dxy", std::isnan(cand->dxy(pv_ass_pos)) ? 0 : cand->dxy(pv_ass_pos));
    fts.fill("jet_pfcand_puppiw", cand->puppiWeight());
    fts.fill("jet_pfcand_nlostinnerhits", cand->lostInnerHits());
    fts.fill("jet_pfcand_nhits", cand->numberOfHits());
    fts.fill("jet_pfcand_npixhits", cand->numberOfPixelHits());
    fts.fill("jet_pfcand_nstriphits", cand->stripLayersWithMeasurement());

    if(abs(cand->pdgId()) == 11 and cand->charge() != 0)
      fts.fill("jet_pfcand_id",0);
    else if(abs(cand->pdgId()) == 13 and cand->charge() != 0)
      fts.fill("jet_pfcand_id",1);
    else if(abs(cand->pdgId()) == 22 and cand->charge() == 0)
      fts.fill("jet_pfcand_id",2);
    else if(abs(cand->pdgId()) != 22 and cand->charge() == 0 and abs(cand->pdgId()) != 1 and abs(cand->pdgId()) != 2)
      fts.fill("jet_pfcand_id",3);
    else if(abs(cand->pdgId()) != 11 and abs(cand->pdgId()) != 13 and cand->charge() != 0)
      fts.fill("jet_pfcand_id",4);
    else if(cand->charge() == 0 and  abs(cand->pdgId()) == 1)
      fts.fill("jet_pfcand_id",5);
    else if(cand->charge() == 0 and  abs(cand->pdgId()) == 2)
      fts.fill("jet_pfcand_id",6);
    else
      fts.fill("jet_pfcand_id",-1);

    fts.fill("jet_pfcand_hcalfraction",std::isnan(cand->hcalFraction()) ? 0 : cand->hcalFraction());
    fts.fill("jet_pfcand_calofraction",std::isnan(cand->caloFraction()) ? 0 : cand->caloFraction());
    fts.fill("pfcand_mask", 1);

    if (track) {
      fts.fill("jet_pfcand_dzsig", std::isnan(fabs(cand->dz(pv_ass_pos)) / cand->dzError()) ? 0 : fabs(cand->dz(pv_ass_pos)) / cand->dzError());
      fts.fill("jet_pfcand_dxysig", std::isnan(fabs(cand->dxy(pv_ass_pos)) / cand->dxyError()) ? 0 : fabs(cand->dxy(pv_ass_pos)) / cand->dxyError());
      fts.fill("jet_pfcand_track_chi2", track->normalizedChi2());
      fts.fill("jet_pfcand_track_qual", track->qualityMask());

      reco::TransientTrack transientTrack = track_builder_->build(*track);
      Measurement1D meas_ip2d    = IPTools::signedTransverseImpactParameter(transientTrack, jet_ref_track_dir, *pv_).second;
      Measurement1D meas_ip3d    = IPTools::signedImpactParameter3D(transientTrack, jet_ref_track_dir, *pv_).second;
      Measurement1D meas_jetdist = IPTools::jetTrackDistance(transientTrack, jet_ref_track_dir, *pv_).second;
      Measurement1D meas_decayl  = IPTools::signedDecayLength3D(transientTrack, jet_ref_track_dir, *pv_).second;

      fts.fill("jet_pfcand_trackjet_d3d", std::isnan(meas_ip3d.value()) ? 0 : meas_ip3d.value());
      fts.fill("jet_pfcand_trackjet_d3dsig", std::isnan(fabs(meas_ip3d.significance())) ? 0 : fabs(meas_ip3d.significance()));
      fts.fill("jet_pfcand_trackjet_dist", std::isnan(-meas_jetdist.value()) ? 0 : -meas_jetdist.value());
      fts.fill("jet_pfcand_trackjet_decayL", std::isnan(meas_decayl.value()) ? 0 : meas_decayl.value());      
    } 
    else {
      fts.fill("jet_pfcand_dzsig", 0);
      fts.fill("jet_pfcand_dxysig", 0);
      fts.fill("jet_pfcand_track_chi2", 0);
      fts.fill("jet_pfcand_track_qual", 0);
      fts.fill("jet_pfcand_trackjet_d3d", 0);
      fts.fill("jet_pfcand_trackjet_d3dsig", 0);
      fts.fill("jet_pfcand_trackjet_dist", 0);
      fts.fill("jet_pfcand_trackjet_decayL", 0);
    }

    // muons specific
    if(abs(cand->pdgId()) == 13){
      std::vector<unsigned int> muonsToSkip;
      int ipos = -1;
      float minDR = 1000;
      for (size_t i = 0; i < muons.size(); i++) {
	if(not muons[i].isPFMuon()) continue;
	if(std::find(muonsToSkip.begin(),muonsToSkip.end(),i) != muonsToSkip.end()) continue;
	float dR = reco::deltaR(muons[i].p4(),candP4);
	if(dR < jet_radius_ and dR < minDR){
	  minDR = dR;
	  ipos = i;
	  muonsToSkip.push_back(i);
	}
      }
      if(ipos >= 0){
	int muonId = 0;
	if(muons[ipos].passed(reco::Muon::CutBasedIdLoose)) muonId++;
	if(muons[ipos].passed(reco::Muon::CutBasedIdMedium)) muonId++;
	if(muons[ipos].passed(reco::Muon::CutBasedIdTight)) muonId++;
	if(muons[ipos].passed(reco::Muon::CutBasedIdGlobalHighPt)) muonId++;
	if(muons[ipos].passed(reco::Muon::CutBasedIdTrkHighPt)) muonId++;
	fts.fill("jet_pfcand_muon_id",muonId);
	fts.fill("jet_pfcand_muon_isglobal",muons[ipos].isGlobalMuon());
	fts.fill("jet_pfcand_muon_chi2",(muons[ipos].isGlobalMuon()) ? muons[ipos].globalTrack()->normalizedChi2(): 0);
	fts.fill("jet_pfcand_muon_nvalidhit",(muons[ipos].isGlobalMuon()) ? muons[ipos].globalTrack()->hitPattern().numberOfValidMuonHits() : 0);
	fts.fill("jet_pfcand_muon_nstation",muons[ipos].numberOfMatchedStations());
	fts.fill("jet_pfcand_muon_segcomp",muon::segmentCompatibility(muons[ipos]));
      }
      else{
	fts.fill("jet_pfcand_muon_id",0);
	fts.fill("jet_pfcand_muon_isglobal",0);
	fts.fill("jet_pfcand_muon_chi2",0);
	fts.fill("jet_pfcand_muon_nvalidhit",0);
	fts.fill("jet_pfcand_muon_nstation",0);
	fts.fill("jet_pfcand_muon_segcomp",0);
      }
    }
    else{
	fts.fill("jet_pfcand_muon_id",0);
	fts.fill("jet_pfcand_muon_isglobal",0);
	fts.fill("jet_pfcand_muon_chi2",0);
	fts.fill("jet_pfcand_muon_nvalidhit",0);
	fts.fill("jet_pfcand_muon_nstation",0);
	fts.fill("jet_pfcand_muon_segcomp",0);
    }
    
    // electrons specific
    if(abs(cand->pdgId()) == 11){      
      int ipos    = -1;
      for (size_t i = 0; i < electrons.size(); i++) {
	if(electrons[i].isPF()){
	  for(const auto & element : electrons[i].associatedPackedPFCandidates()){
	    if(abs(element->pdgId()) == 11 and element->p4() == candP4)
	      ipos = i;
	  }
	}  
      }
      if(ipos >= 0){
	fts.fill("jet_pfcand_electron_detaIn",std::isnan(electrons[ipos].deltaEtaSuperClusterTrackAtVtx()) ? 0 : electrons[ipos].deltaEtaSuperClusterTrackAtVtx());
	fts.fill("jet_pfcand_electron_dphiIn",std::isnan(electrons[ipos].deltaPhiSuperClusterTrackAtVtx()) ? 0 : electrons[ipos].deltaPhiSuperClusterTrackAtVtx());
	fts.fill("jet_pfcand_electron_sigIetaIeta",std::isnan(electrons[ipos].full5x5_sigmaIetaIeta()) ? 0 : electrons[ipos].full5x5_sigmaIetaIeta());
	fts.fill("jet_pfcand_electron_sigIphiIphi",std::isnan(electrons[ipos].full5x5_sigmaIphiIphi()) ? 0 : electrons[ipos].full5x5_sigmaIphiIphi());
	fts.fill("jet_pfcand_electron_r9",std::isnan(electrons[ipos].full5x5_r9()) ? 0 : electrons[ipos].full5x5_r9());
	fts.fill("jet_pfcand_electron_convProb",std::isnan(electrons[ipos].convVtxFitProb()) ? 0 : electrons[ipos].convVtxFitProb());
      }
      else{
	fts.fill("jet_pfcand_electron_detaIn",0);
	fts.fill("jet_pfcand_electron_dphiIn",0);
	fts.fill("jet_pfcand_electron_sigIetaIeta",0);
	fts.fill("jet_pfcand_electron_sigIphiIphi",0);
	fts.fill("jet_pfcand_electron_r9",0);
	fts.fill("jet_pfcand_electron_convProb",0);
      }
    }
    else{
      fts.fill("jet_pfcand_electron_detaIn",0);
      fts.fill("jet_pfcand_electron_dphiIn",0);
      fts.fill("jet_pfcand_electron_sigIetaIeta",0);
      fts.fill("jet_pfcand_electron_sigIphiIphi",0);
      fts.fill("jet_pfcand_electron_r9",0);
      fts.fill("jet_pfcand_electron_convProb",0);
    }

    // photons specific
    if(abs(cand->pdgId()) == 22){      
      int ipos    = -1;
      for (size_t i = 0; i < photons.size(); i++) {
	for(const auto & element : photons[i].associatedPackedPFCandidates()){
	  if(abs(element->pdgId()) == 22 and element->p4() == candP4)
	    ipos = i;	
	}  
      }
      if(ipos >= 0){
	fts.fill("jet_pfcand_photon_sigIetaIeta",std::isnan(photons[ipos].full5x5_sigmaIetaIeta()) ? 0 : photons[ipos].full5x5_sigmaIetaIeta());
	fts.fill("jet_pfcand_photon_r9",std::isnan(photons[ipos].full5x5_r9()) ? 0 : photons[ipos].full5x5_r9());
	fts.fill("jet_pfcand_photon_eVeto",photons[ipos].passElectronVeto());
      }
      else{
	fts.fill("jet_pfcand_photon_sigIetaIeta",0);
	fts.fill("jet_pfcand_photon_r9",0);
	fts.fill("jet_pfcand_photon_eVeto",0);
      }
    }
    else{
      fts.fill("jet_pfcand_photon_sigIetaIeta",0);
      fts.fill("jet_pfcand_photon_r9",0);
      fts.fill("jet_pfcand_photon_eVeto",0);
    }
    
    // tau specific prior to any puppi weight application                                                                                                                                          
    if(std::find(tau_pfcandidates.begin(),tau_pfcandidates.end(),cand->p4()) != tau_pfcandidates.end())
      fts.fill("jet_pfcand_tau_signal",1);
    else
      fts.fill("jet_pfcand_tau_signal",0);

    if(dump_feature_tree_){
      jet_pfcand_pt_log.push_back(fts.get("jet_pfcand_pt_log").back());
      jet_pfcand_energy_log.push_back(fts.get("jet_pfcand_energy_log").back());
      jet_pfcand_deta.push_back(fts.get("jet_pfcand_deta").back());
      jet_pfcand_dphi.push_back(fts.get("jet_pfcand_dphi").back());
      jet_pfcand_eta.push_back(fts.get("jet_pfcand_eta").back());
      jet_pfcand_charge.push_back(fts.get("jet_pfcand_charge").back());
      jet_pfcand_frompv.push_back(fts.get("jet_pfcand_frompv").back());
      jet_pfcand_nlostinnerhits.push_back(fts.get("jet_pfcand_nlostinnerhits").back());
      jet_pfcand_track_chi2.push_back(fts.get("jet_pfcand_track_chi2").back());
      jet_pfcand_track_qual.push_back(fts.get("jet_pfcand_track_qual").back());
      jet_pfcand_dz.push_back(fts.get("jet_pfcand_dz").back());
      jet_pfcand_dxy.push_back(fts.get("jet_pfcand_dxy").back());
      jet_pfcand_dxysig.push_back(fts.get("jet_pfcand_dxysig").back());
      jet_pfcand_dzsig.push_back(fts.get("jet_pfcand_dzsig").back());
      jet_pfcand_etarel.push_back(fts.get("jet_pfcand_etarel").back());
      jet_pfcand_pperp_ratio.push_back(fts.get("jet_pfcand_pperp_ratio").back());
      jet_pfcand_ppara_ratio.push_back(fts.get("jet_pfcand_ppara_ratio").back());
      jet_pfcand_trackjet_d3d.push_back(fts.get("jet_pfcand_trackjet_d3d").back());
      jet_pfcand_trackjet_d3dsig.push_back(fts.get("jet_pfcand_trackjet_d3dsig").back());
      jet_pfcand_trackjet_dist.push_back(fts.get("jet_pfcand_trackjet_dist").back());
      jet_pfcand_npixhits.push_back(fts.get("jet_pfcand_npixhits").back());
      jet_pfcand_nstriphits.push_back(fts.get("jet_pfcand_nstriphits").back());
      jet_pfcand_trackjet_decayL.push_back(fts.get("jet_pfcand_trackjet_decayL").back());
      jet_pfcand_id.push_back(fts.get("jet_pfcand_id").back());
      jet_pfcand_puppiw.push_back(fts.get("jet_pfcand_puppiw").back());
      jet_pfcand_calofraction.push_back(fts.get("jet_pfcand_calofraction").back());
      jet_pfcand_hcalfraction.push_back(fts.get("jet_pfcand_hcalfraction").back());
      jet_pfcand_muon_id.push_back(fts.get("jet_pfcand_muon_id").back());
      jet_pfcand_muon_segcomp.push_back(fts.get("jet_pfcand_muon_segcomp").back());
      jet_pfcand_muon_isglobal.push_back(fts.get("jet_pfcand_muon_isglobal").back());
      jet_pfcand_muon_chi2.push_back(fts.get("jet_pfcand_muon_chi2").back());
      jet_pfcand_muon_nvalidhit.push_back(fts.get("jet_pfcand_muon_nvalidhit").back());
      jet_pfcand_muon_nstation.push_back(fts.get("jet_pfcand_muon_nstation").back());
      jet_pfcand_electron_detaIn.push_back(fts.get("jet_pfcand_electron_detaIn").back());
      jet_pfcand_electron_dphiIn.push_back(fts.get("jet_pfcand_electron_dphiIn").back());
      jet_pfcand_electron_sigIetaIeta.push_back(fts.get("jet_pfcand_electron_sigIetaIeta").back());
      jet_pfcand_electron_sigIphiIphi.push_back(fts.get("jet_pfcand_electron_sigIphiIphi").back());
      jet_pfcand_electron_r9.push_back(fts.get("jet_pfcand_electron_r9").back());
      jet_pfcand_electron_convProb.push_back(fts.get("jet_pfcand_electron_convProb").back());
      jet_pfcand_photon_sigIetaIeta.push_back(fts.get("jet_pfcand_photon_sigIetaIeta").back());
      jet_pfcand_photon_r9.push_back(fts.get("jet_pfcand_photon_r9").back());
      jet_pfcand_photon_eVeto.push_back(fts.get("jet_pfcand_photon_eVeto").back());
      jet_pfcand_tau_signal.push_back(fts.get("jet_pfcand_tau_signal").back());
    }
  }
}

void ParticleNetFeatureEvaluator::fillSVFeatures(DeepBoostedJetFeatures &fts, 
						 const reco::Jet & jet) {

  // secondary vertexes matching jet
  std::vector<const reco::VertexCompositePtrCandidate*> jetSVs;
  for (const auto &sv : *svs_) {
    if (reco::deltaR2(sv, jet) < jet_radius_ * jet_radius_) {
      jetSVs.push_back(&sv);
    }
  }

  // sort by dxy significance
  std::sort(jetSVs.begin(),
            jetSVs.end(),
            [&](const reco::VertexCompositePtrCandidate *sva, 
		const reco::VertexCompositePtrCandidate *svb) {
              return sv_vertex_comparator(*sva, *svb, *pv_);
            });

  // reserve space
  for (const auto &name : sv_features_)
    fts.reserve(name, jetSVs.size());
  
  GlobalVector jet_global_vec(jet.px(), jet.py(), jet.pz());
  
  for (const auto *sv : jetSVs) {
    fts.fill("sv_mask", 1);
    fts.fill("jet_sv_pt_log", std::isnan(std::log(sv->pt())) ? 0 : std::log(sv->pt()));
    fts.fill("jet_sv_eta", sv->eta());
    fts.fill("jet_sv_mass", sv->mass());
    fts.fill("jet_sv_deta", sv->eta() - jet.eta());
    fts.fill("jet_sv_dphi", sv->phi() - jet.phi());
    fts.fill("jet_sv_ntrack", sv->numberOfDaughters());
    fts.fill("jet_sv_chi2", sv->vertexNormalizedChi2());
    
    reco::Vertex::CovarianceMatrix csv;
    sv->fillVertexCovariance(csv);
    reco::Vertex svtx(sv->vertex(), csv);
    
    VertexDistanceXY dxy;
    auto valxy = dxy.signedDistance(svtx, *pv_, jet_global_vec);
    fts.fill("jet_sv_dxy", std::isnan(valxy.value()) ? 0 : valxy.value());
    fts.fill("jet_sv_dxysig", std::isnan(fabs(valxy.significance())) ? 0 : fabs(valxy.significance()));
    
    VertexDistance3D d3d;
    auto val3d = d3d.signedDistance(svtx, *pv_, jet_global_vec);
    fts.fill("jet_sv_d3d", std::isnan(val3d.value()) ? 0 : val3d.value());
    fts.fill("jet_sv_d3dsig", std::isnan(fabs(val3d.significance())) ? 0 : fabs(val3d.significance()));

    if(dump_feature_tree_){
      jet_sv_pt_log.push_back(fts.get("jet_sv_pt_log").back());
      jet_sv_mass.push_back(fts.get("jet_sv_mass").back());
      jet_sv_deta.push_back(fts.get("jet_sv_deta").back());
      jet_sv_dphi.push_back(fts.get("jet_sv_dphi").back());
      jet_sv_eta.push_back(fts.get("jet_sv_eta").back());
      jet_sv_ntrack.push_back(fts.get("jet_sv_ntrack").back());
      jet_sv_chi2.push_back(fts.get("jet_sv_chi2").back());
      jet_sv_dxy.push_back(fts.get("jet_sv_dxy").back());
      jet_sv_dxysig.push_back(fts.get("jet_sv_dxysig").back());
      jet_sv_d3d.push_back(fts.get("jet_sv_d3d").back());
      jet_sv_d3dsig.push_back(fts.get("jet_sv_d3dsig").back());
    }
  }
}

void ParticleNetFeatureEvaluator::fillLostTrackFeatures(DeepBoostedJetFeatures &fts, 
							const reco::Jet & jet
							) {

  // some jet properties
  TVector3        jet_direction (jet.momentum().Unit().x(), jet.momentum().Unit().y(), jet.momentum().Unit().z());
  GlobalVector    jet_ref_track_dir (jet.px(), jet.py(), jet.pz());
  math::XYZVector jet_dir = jet.momentum().Unit();

  std::vector<pat::PackedCandidate> jet_lost_tracks;
  for(size_t itrk = 0; itrk < losttracks_->size(); itrk++){
    if(reco::deltaR(losttracks_->at(itrk).p4(),jet.p4()) < max_dr_for_losttrack_ and
       losttracks_->at(itrk).pt() > min_pt_for_losttrack_ ){
      jet_lost_tracks.push_back(losttracks_->at(itrk));
    }
  }
  std::sort(jet_lost_tracks.begin(), jet_lost_tracks.end(), [](const auto &a, const auto &b) { return a.pt() > b.pt();});

  // reserve space
  for (const auto &name : losttrack_features_)
    fts.reserve(name, jet_lost_tracks.size());

  reco::VertexRef pv_ass = reco::VertexRef(vtxs_, 0);
  math::XYZPoint  pv_ass_pos = pv_ass->position();

  for(auto const & ltrack : jet_lost_tracks){

    fts.fill("jet_losttrack_pt_log",std::isnan(std::log(ltrack.pt())) ? 0 : std::log(ltrack.pt()));
    fts.fill("jet_losttrack_eta",ltrack.eta());
    fts.fill("jet_losttrack_charge",ltrack.charge());
    fts.fill("jet_losttrack_frompv",ltrack.fromPV());
    fts.fill("jet_losttrack_dz",std::isnan(ltrack.dz(pv_ass_pos)) ? 0 : ltrack.dz(pv_ass_pos));
    fts.fill("jet_losttrack_dxy",std::isnan(ltrack.dxy(pv_ass_pos)) ? 0 : ltrack.dxy(pv_ass_pos));
    fts.fill("jet_losttrack_npixhits",ltrack.numberOfPixelHits());
    fts.fill("jet_losttrack_nstriphits",ltrack.stripLayersWithMeasurement());

    TVector3 ltrack_momentum (ltrack.momentum().x(),ltrack.momentum().y(),ltrack.momentum().z());
    fts.fill("jet_losttrack_deta",jet_direction.Eta()-ltrack_momentum.Eta());
    fts.fill("jet_losttrack_dphi",jet_direction.DeltaPhi(ltrack_momentum));
    fts.fill("jet_losttrack_etarel",std::isnan(reco::btau::etaRel(jet_dir,ltrack.momentum())) ? 0 : reco::btau::etaRel(jet_dir,ltrack.momentum()));

    const reco::Track* track = ltrack.bestTrack();
    if(track){
      fts.fill("jet_losttrack_track_chi2",track->normalizedChi2());
      fts.fill("jet_losttrack_track_qual",track->qualityMask());
      fts.fill("jet_losttrack_dxysig",std::isnan(fabs(ltrack.dxy(pv_ass_pos))/ltrack.dxyError()) ? 0 : fabs(ltrack.dxy(pv_ass_pos))/ltrack.dxyError());
      fts.fill("jet_losttrack_dzsig",std::isnan(fabs(ltrack.dz(pv_ass_pos))/ltrack.dzError()) ? 0 : fabs(ltrack.dz(pv_ass_pos))/ltrack.dzError());

      reco::TransientTrack transientTrack = track_builder_->build(*track);
      Measurement1D meas_ip3d    = IPTools::signedImpactParameter3D(transientTrack, jet_ref_track_dir, *pv_).second;
      Measurement1D meas_jetdist = IPTools::jetTrackDistance(transientTrack, jet_ref_track_dir, *pv_).second;
      Measurement1D meas_decayl  = IPTools::signedDecayLength3D(transientTrack, jet_ref_track_dir, *pv_).second;

      fts.fill("jet_losttrack_trackjet_d3d", std::isnan(meas_ip3d.value()) ? 0 : meas_ip3d.value());
      fts.fill("jet_losttrack_trackjet_d3dsig", std::isnan(fabs(meas_ip3d.significance())) ? 0 : fabs(meas_ip3d.significance()));
      fts.fill("jet_losttrack_trackjet_dist", std::isnan(-meas_jetdist.value()) ? 0 : -meas_jetdist.value());
      fts.fill("jet_losttrack_trackjet_decayL", std::isnan(meas_decayl.value()) ? 0 : meas_decayl.value());      
    }
    else{
      fts.fill("jet_losttrack_track_chi2",0);
      fts.fill("jet_losttrack_track_qual",0);
      fts.fill("jet_losttrack_dxysig",0);
      fts.fill("jet_losttrack_dzsig",0);
      fts.fill("jet_losttrack_trackjet_d3d", 0);
      fts.fill("jet_losttrack_trackjet_d3dsig", 0);
      fts.fill("jet_losttrack_trackjet_dist", 0);
      fts.fill("jet_losttrack_trackjet_decayL", 0);
    }

    fts.fill("lt_mask",1);
    
    if(dump_feature_tree_){
      jet_losttrack_pt_log.push_back(fts.get("jet_losttrack_pt_log").back());
      jet_losttrack_deta.push_back(fts.get("jet_losttrack_deta").back());
      jet_losttrack_dphi.push_back(fts.get("jet_losttrack_dphi").back());
      jet_losttrack_eta.push_back(fts.get("jet_losttrack_eta").back());
      jet_losttrack_charge.push_back(fts.get("jet_losttrack_charge").back());
      jet_losttrack_frompv.push_back(fts.get("jet_losttrack_frompv").back());
      jet_losttrack_track_chi2.push_back(fts.get("jet_losttrack_track_chi2").back());
      jet_losttrack_track_qual.push_back(fts.get("jet_losttrack_track_qual").back());
      jet_losttrack_dz.push_back(fts.get("jet_losttrack_dz").back());
      jet_losttrack_dxy.push_back(fts.get("jet_losttrack_dxy").back());
      jet_losttrack_dxysig.push_back(fts.get("jet_losttrack_dxysig").back());
      jet_losttrack_dzsig.push_back(fts.get("jet_losttrack_dzsig").back());
      jet_losttrack_etarel.push_back(fts.get("jet_losttrack_etarel").back());
      jet_losttrack_npixhits.push_back(fts.get("jet_losttrack_npixhits").back());
      jet_losttrack_nstriphits.push_back(fts.get("jet_losttrack_nstriphits").back());
      jet_losttrack_trackjet_d3d.push_back(fts.get("jet_losttrack_trackjet_d3d").back());
      jet_losttrack_trackjet_d3dsig.push_back(fts.get("jet_losttrack_trackjet_d3dsig").back());
      jet_losttrack_trackjet_dist.push_back(fts.get("jet_losttrack_trackjet_dist").back());
      jet_losttrack_trackjet_decayL.push_back(fts.get("jet_losttrack_trackjet_decayL").back());
    }
  }
}

// define this as a plug-in
DEFINE_FWK_MODULE(ParticleNetFeatureEvaluator);
