#ifndef boostedTauOutputCollection_H
#define boostedTauOutputCollection_H

#include "DataFormats/PatCandidates/interface/Tau.h"

#include "TTree.h"

class boostedTauOutputCollection
{
public:
boostedTauOutputCollection();
~boostedTauOutputCollection() {};

void clearData();
void fillCollection(edm::Handle< std::vector< pat::Tau > > boostedTauHandle);
void associateTreeToCollection(TTree* theTree);

private:

UInt_t nboostedTau;
std::vector<float> boostedTau_pt;
std::vector<float> boostedTau_eta;
std::vector<float> boostedTau_phi;
std::vector<float> boostedTau_m;
std::vector<int> boostedTau_decayModeFindingNewDMs;
std::vector<int> boostedTau_passesVLooseID;
std::vector<int> boostedTau_passesVVLooseID;

};

#endif
