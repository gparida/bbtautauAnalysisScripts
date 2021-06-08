#include "bbtautauAnalysisScripts/boostedTauRecoAnalyzer/interface/tauOutputCollection.h"

tauOutputCollection::tauOutputCollection(){
  ntau = 0;
  tau_pt.clear();
  tau_eta.clear();
  tau_phi.clear();
  tau_m.clear();
  tau_decayModeFindingNewDMs.clear();
  tau_passesVLooseID.clear();
}

void tauOutputCollection::associateTreeToCollection(TTree* theTree)
{
  theTree->Branch("ntau", &ntau);
  theTree->Branch("tau_pt", &tau_pt);
  theTree->Branch("tau_eta", &tau_eta);
  theTree->Branch("tau_phi", &tau_phi);
  theTree->Branch("tau_m", &tau_m);
  theTree->Branch("tau_decayModeFindingNewDMs", &tau_decayModeFindingNewDMs);
  theTree->Branch("tau_passesVLooseID", &tau_passesVLooseID);
}

void tauOutputCollection::clearData()
{
  ntau = 0;
  tau_pt.clear();
  tau_eta.clear();
  tau_phi.clear();
  tau_m.clear();
  tau_decayModeFindingNewDMs.clear();
  tau_passesVLooseID.clear();
}

void tauOutputCollection::fillCollection(edm::Handle< std::vector< pat::Tau > > tauHandle)
{
  for(std::vector<pat::Tau>::const_iterator theTau = tauHandle->begin();
      theTau != tauHandle->end();
      ++theTau)
    {
      ++ntau;
      tau_pt.push_back(theTau->pt());
      tau_eta.push_back(theTau->eta());
      tau_phi.push_back(theTau->phi());
      tau_m.push_back(theTau->mass());
      tau_decayModeFindingNewDMs.push_back(theTau->tauID("decayModeFindingNewDMs"));
      tau_passesVLooseID.push_back(theTau->tauID("byVLooseIsolationMVArun2v1DBoldDMwLT"));
    }
}
