#ifndef genTauOutputCollection_H
#define genTauOutputCollection_H

#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/Candidate/interface/Candidate.h"

#include "TTree.h"

class genTauOutputCollection
{
 public:
  genTauOutputCollection();
  ~genTauOutputCollection() {};

  void fillFromParticle(const reco::GenParticle* genTau);
  void clearData();
  void associateTreeToCollection(TTree* theTree);


 private:

  UInt_t ngenTau;
  std::vector<float> genTau_pt;
  std::vector<float> genTau_eta;
  std::vector<float> genTau_phi;
  std::vector<float> genTau_m;
  std::vector<int> genTau_status;
  std::vector<std::vector<int>> genTau_daughterParticles_pdgCodes;
};

#endif
