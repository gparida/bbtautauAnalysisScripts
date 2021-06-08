#include "bbtautauAnalysisScripts/boostedTauRecoAnalyzer/interface/genTauOutputCollection.h"

genTauOutputCollection::genTauOutputCollection()
{
  ngenTau = 0; //should help us maintain proper tree filling with empty vectors
  genTau_pt.clear();
  genTau_eta.clear();
  genTau_phi.clear();
  genTau_m.clear();
  genTau_status.clear();
  genTau_daughterParticles_pdgCodes.clear();
}

void genTauOutputCollection::fillFromParticle(const reco::GenParticle* genTau)
{
  ++ngenTau;
  genTau_pt.push_back(genTau->pt());
  genTau_eta.push_back(genTau->eta());
  genTau_phi.push_back(genTau->phi());
  genTau_m.push_back(genTau->mass());
  genTau_status.push_back(genTau->status());
  std::vector<int> pdgCodes;
  for (size_t i = 0; i<genTau->numberOfDaughters(); ++i)
    {	  
      pdgCodes.push_back(genTau->daughter(i)->pdgId());
    }
  genTau_daughterParticles_pdgCodes.push_back(pdgCodes);
  pdgCodes.clear();

}

void genTauOutputCollection::clearData()
{
  ngenTau = 0; //should help us maintain proper tree filling with empty vectors
  genTau_pt.clear();
  genTau_eta.clear();
  genTau_phi.clear();
  genTau_m.clear();
  genTau_status.clear();
  genTau_daughterParticles_pdgCodes.clear();
}

void genTauOutputCollection::associateTreeToCollection(TTree* theTree)
{
  theTree->Branch("ngenTau", &ngenTau);
  theTree->Branch("genTau_pt", &genTau_pt);
  theTree->Branch("genTau_eta", &genTau_eta);
  theTree->Branch("genTau_phi", &genTau_phi);
  theTree->Branch("genTau_m", &genTau_m);
  theTree->Branch("genTau_status", &genTau_status);
  theTree->Branch("genTau_daughterParticles_pdgCodes",&genTau_daughterParticles_pdgCodes);
}
