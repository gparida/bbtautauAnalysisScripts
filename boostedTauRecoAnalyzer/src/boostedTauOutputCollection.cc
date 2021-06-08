#include "bbtautauAnalysisScripts/boostedTauRecoAnalyzer/interface/boostedTauOutputCollection.h"

boostedTauOutputCollection::boostedTauOutputCollection(){
  nboostedTau = 0;
  boostedTau_pt.clear();
  boostedTau_eta.clear();
  boostedTau_phi.clear();
  boostedTau_m.clear();
  boostedTau_decayModeFindingNewDMs.clear();
  boostedTau_passesVLooseID.clear();
}

void boostedTauOutputCollection::associateTreeToCollection(TTree* theTree)
{
  theTree->Branch("nboostedTau", &nboostedTau);
  theTree->Branch("boostedTau_pt", &boostedTau_pt);
  theTree->Branch("boostedTau_eta", &boostedTau_eta);
  theTree->Branch("boostedTau_phi", &boostedTau_phi);
  theTree->Branch("boostedTau_m", &boostedTau_m);
  theTree->Branch("boostedTau_decayModeFindingNewDMs", &boostedTau_decayModeFindingNewDMs);
  theTree->Branch("boostedTau_passesVLooseID", &boostedTau_passesVLooseID);
}

void boostedTauOutputCollection::clearData()
{
  nboostedTau = 0;
  boostedTau_pt.clear();
  boostedTau_eta.clear();
  boostedTau_phi.clear();
  boostedTau_m.clear();
  boostedTau_decayModeFindingNewDMs.clear();
  boostedTau_passesVLooseID.clear();
}

void boostedTauOutputCollection::fillCollection(edm::Handle< std::vector< pat::Tau > > boostedTauHandle)
{
  for(std::vector<pat::Tau>::const_iterator theBoostedTau = boostedTauHandle->begin();
      theBoostedTau != boostedTauHandle->end();
      ++theBoostedTau)
    {
      ++nboostedTau;
      boostedTau_pt.push_back(theBoostedTau->pt());
      boostedTau_eta.push_back(theBoostedTau->eta());
      boostedTau_phi.push_back(theBoostedTau->phi());
      boostedTau_m.push_back(theBoostedTau->mass());
      boostedTau_decayModeFindingNewDMs.push_back(theBoostedTau->tauID("decayModeFindingNewDMs"));
      boostedTau_passesVLooseID.push_back(theBoostedTau->tauID("byVLooseIsolationMVArun2v1DBoldDMwLT"));
    }
}
