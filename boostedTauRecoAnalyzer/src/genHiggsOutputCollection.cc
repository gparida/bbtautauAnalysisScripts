//functions for the gen higgs collection
#include "bbtautauAnalysisScripts/boostedTauRecoAnalyzer/interface/genHiggsOutputCollection.h"

genHiggsOutputCollection::genHiggsOutputCollection()
{
  ngenHiggs = 0; //should help us maintain proper tree filling with empty vectors
  genHiggs_pt.clear();
  genHiggs_eta.clear();
  genHiggs_phi.clear();
  genHiggs_m.clear();
  genHiggs_status.clear();
  genHiggs_ndaughterParticles.clear();
  genHiggs_daughterParticles_pdgCodes.clear();
}

void genHiggsOutputCollection::fillFromParticle(const reco::GenParticle* genHiggs)
{
  ++ngenHiggs;
  genHiggs_pt.push_back(genHiggs->pt());
  genHiggs_eta.push_back(genHiggs->eta());
  genHiggs_phi.push_back(genHiggs->phi());
  genHiggs_m.push_back(genHiggs->mass());
  genHiggs_status.push_back(genHiggs->status());
  genHiggs_ndaughterParticles.push_back(genHiggs->numberOfDaughters());
  std::vector<int> pdgCodes;
  for (size_t i = 0; i<genHiggs->numberOfDaughters(); ++i)
    {	  
      pdgCodes.push_back(genHiggs->daughter(i)->pdgId());
    }
  genHiggs_daughterParticles_pdgCodes.push_back(pdgCodes);
  pdgCodes.clear();
}

void genHiggsOutputCollection::clearData()
{
  ngenHiggs = 0; //should help us maintain proper tree filling with empty vectors
  genHiggs_pt.clear();
  genHiggs_eta.clear();
  genHiggs_phi.clear();
  genHiggs_m.clear();
  genHiggs_status.clear();
  genHiggs_ndaughterParticles.clear();
  genHiggs_daughterParticles_pdgCodes.clear();
}

void genHiggsOutputCollection::associateTreeToCollection(TTree* theTree)
{
  theTree->Branch("ngenHiggs", &ngenHiggs);
  theTree->Branch("genHiggs_pt", &genHiggs_pt);
  theTree->Branch("genHiggs_eta", &genHiggs_eta);
  theTree->Branch("genHiggs_phi", &genHiggs_phi);
  theTree->Branch("genHiggs_m", &genHiggs_m);
  theTree->Branch("genHiggs_status", &genHiggs_status);
  theTree->Branch("genHiggs_ndaughterParticles", &genHiggs_ndaughterParticles);
  theTree->Branch("genHiggs_daughterparticles_pdgCodes", &genHiggs_daughterParticles_pdgCodes);
}
