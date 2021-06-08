//quick class for taking gen higgs vectors and reading them out to a tree friendly structure
#ifndef genHiggsOutputCollection_H
#define genHiggsOutputCollection_H


#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/Candidate/interface/Candidate.h"

#include "TTree.h"

class genHiggsOutputCollection
{
 public:
  genHiggsOutputCollection();
  ~genHiggsOutputCollection() {};

  void fillFromParticle(const reco::GenParticle* genHiggs);
  void clearData();
  void associateTreeToCollection(TTree* theTree);


 private:

  UInt_t ngenHiggs;
  std::vector<float> genHiggs_pt;
  std::vector<float> genHiggs_eta;
  std::vector<float> genHiggs_phi;
  std::vector<float> genHiggs_m;
  std::vector<int> genHiggs_status;
  std::vector<UInt_t> genHiggs_ndaughterParticles;
  std::vector<std::vector<int>> genHiggs_daughterParticles_pdgCodes;
};

#endif
