#include "bbtautauAnalysisScripts/boostedTauRecoAnalyzer/interface/genTauOutputCollection.h"

genTauOutputCollection::genTauOutputCollection()
{
  ngenTau = 0; //should help us maintain proper tree filling with empty vectors
  genTau_pt.clear();
  genTau_eta.clear();
  genTau_phi.clear();
  genTau_m.clear();
  genTau_status.clear();
}

void genTauOutputCollection::fillFromParticle(const reco::GenParticle* genTau)
{
  ++ngenTau;
  genTau_pt.push_back(genTau->pt());
  genTau_eta.push_back(genTau->eta());
  genTau_phi.push_back(genTau->phi());
  genTau_m.push_back(genTau->mass());
  genTau_status.push_back(genTau->status());
}

void genTauOutputCollection::clearData()
{
  ngenTau = 0; //should help us maintain proper tree filling with empty vectors
  genTau_pt.clear();
  genTau_eta.clear();
  genTau_phi.clear();
  genTau_m.clear();
  genTau_status.clear();
}

void genTauOutputCollection::associateTreeToCollection(TTree* theTree)
{
  theTree->Branch("ngenTau", &ngenTau);
  theTree->Branch("genTau_pt", &genTau_pt);
  theTree->Branch("genTau_eta", &genTau_eta);
  theTree->Branch("genTau_phi", &genTau_phi);
  theTree->Branch("genTau_m", &genTau_m);
  theTree->Branch("genTau_status", &genTau_status);
}
