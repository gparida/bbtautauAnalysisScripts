#ifndef tauOutputCollection_H
#define tauOutputCollection_H

#include "DataFormats/PatCandidates/interface/Tau.h"

#include "TTree.h"

class tauOutputCollection
{
public:
tauOutputCollection();
~tauOutputCollection() {};

void clearData();
void fillCollection(edm::Handle< std::vector< pat::Tau > > tauHandle);
void associateTreeToCollection(TTree* theTree);

private:

UInt_t ntau;
std::vector<float> tau_pt;
std::vector<float> tau_eta;
std::vector<float> tau_phi;
std::vector<float> tau_m;
std::vector<int> tau_decayModeFindingNewDMs;

};

#endif
