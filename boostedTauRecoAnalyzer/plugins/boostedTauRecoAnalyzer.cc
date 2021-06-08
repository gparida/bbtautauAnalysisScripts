// -*- C++ -*-
//
// Package:    bbtautauAnalysisScripts/boostedTauRecoAnalyzer
// Class:      boostedTauRecoAnalyzer
//
/**\class boostedTauRecoAnalyzer boostedTauRecoAnalyzer.cc bbtautauAnalysisScripts/boostedTauRecoAnalyzer/plugins/boostedTauRecoAnalyzer.cc

   Description: [one line class summary]

   Implementation:
   [Notes on implementation]
*/
//
// Original Author:  Andrew Loeliger
//         Created:  Thu, 20 May 2021 10:28:50 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/Candidate/interface/Candidate.h"

#include "DataFormats/PatCandidates/interface/Tau.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "TTree.h"

#include "bbtautauAnalysisScripts/boostedTauRecoAnalyzer/interface/genHiggsOutputCollection.h"
#include "bbtautauAnalysisScripts/boostedTauRecoAnalyzer/interface/genTauOutputCollection.h"
#include "bbtautauAnalysisScripts/boostedTauRecoAnalyzer/interface/boostedTauOutputCollection.h"
#include "bbtautauAnalysisScripts/boostedTauRecoAnalyzer/interface/tauOutputCollection.h"
//#include "DataFormats/HepMCCandidate/interface/GenStatusFlags.h"
//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<>
// This will improve performance in multithreaded jobs.


using reco::TrackCollection;

class boostedTauRecoAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
public:
explicit boostedTauRecoAnalyzer(const edm::ParameterSet&);
~boostedTauRecoAnalyzer();

static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
virtual void beginJob() override;
virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
virtual void endJob() override;
  
TTree* finalTree;
genHiggsOutputCollection theGenHiggsOutputCollection;
genTauOutputCollection theGenTauOutputCollection;
boostedTauOutputCollection theBoostedTauOutputCollection;
tauOutputCollection theTauOutputCollection;

// ----------member data ---------------------------
  
edm::EDGetTokenT< edm::View<reco::GenParticle> > prunedGenCollection;
edm::EDGetTokenT< std::vector<pat::Tau> > boostedTauCollection;
edm::EDGetTokenT< std::vector<pat::Tau> > tauCollection;
//edm::EDGetTokenT<TrackCollection> tracksToken_;  //used to select what tracks to read from configuration file
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
boostedTauRecoAnalyzer::boostedTauRecoAnalyzer(const edm::ParameterSet& iConfig):
  prunedGenCollection(consumes< edm::View<reco::GenParticle> >(iConfig.getParameter<edm::InputTag>("prunedGenParticleCollection"))),
  boostedTauCollection(consumes< std::vector<pat::Tau> >(iConfig.getParameter< edm::InputTag >("boostedTauCollection"))),
  tauCollection(consumes< std::vector<pat::Tau> >(iConfig.getParameter< edm::InputTag >("tauCollection")))
{
  //now do what ever initialization is needed
  edm::Service<TFileService> fs;
  finalTree = fs->make<TTree>("Events","Event data");
  
  theGenHiggsOutputCollection = genHiggsOutputCollection();
  theGenHiggsOutputCollection.associateTreeToCollection(finalTree);
  
  theGenTauOutputCollection = genTauOutputCollection();
  theGenTauOutputCollection.associateTreeToCollection(finalTree);

  theBoostedTauOutputCollection = boostedTauOutputCollection();
  theBoostedTauOutputCollection.associateTreeToCollection(finalTree);

  theTauOutputCollection = tauOutputCollection();
  theTauOutputCollection.associateTreeToCollection(finalTree);

}

  
boostedTauRecoAnalyzer::~boostedTauRecoAnalyzer()
{

// do anything here that needs to be done at desctruction time
// (e.g. close files, deallocate resources etc.)

}


    //
    // member functions
    //

    // ------------ method called for each event  ------------
void boostedTauRecoAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  
  using namespace edm;
  
  edm::Handle< edm::View<reco::GenParticle> > prunedGenHandle;
  iEvent.getByToken(prunedGenCollection, prunedGenHandle);
  
  edm::Handle< std::vector<pat::Tau> > boostedTauHandle;
  iEvent.getByToken(boostedTauCollection, boostedTauHandle);
  
  edm::Handle< std::vector<pat::Tau> > tauHandle;
  iEvent.getByToken(tauCollection, tauHandle);
  

  for(size_t iGen = 0; iGen < prunedGenHandle->size(); iGen++)
    {
      const auto genParticle = (*prunedGenHandle)[iGen];
      if (genParticle.pdgId() == 25) theGenHiggsOutputCollection.fillFromParticle(&genParticle);
      //std::cout<<"Found gen higgs: "<<genParticle.pt()<<" "<<genParticle.eta()<<" "<<genParticle.phi()<<" "<<genParticle.mass()<<" Status: "<<genParticle.status()<<std::endl;
      //std::cout<<"Number of daughters: "<<genParticle.numberOfDaughters()<<": ";
      //for (size_t j = 0; j<genParticle.numberOfDaughters(); ++j) std::cout<<genParticle.daughter(j)->pdgId()<<" ";
      //std::cout<<std::endl;
      if (abs(genParticle.pdgId()) == 15 && genParticle.status() == 2) theGenTauOutputCollection.fillFromParticle(&genParticle);
      //std::cout<<"Found status 2 tau: "<<genParticle.pt()<<" "<<genParticle.eta()<<" "<<genParticle.phi()<<" "<<genParticle.mass()<<std::endl;
    }

  theBoostedTauOutputCollection.fillCollection(boostedTauHandle);
  theTauOutputCollection.fillCollection(tauHandle);
  
  //fill the tree and clear the output collections
  finalTree->Fill();
  theGenHiggsOutputCollection.clearData();
  theGenTauOutputCollection.clearData();
  theBoostedTauOutputCollection.clearData();
  theTauOutputCollection.clearData();
  
  //okay, the first higgs in the vector is our tau parent
  //and we have all gen taus.
  //the first higgs is the parent of the taus in all cases.
  
  /*
    for(const auto& track : iEvent.get(tracksToken_) ) {
    // do something with track parameters, e.g, plot the charge.
    // int charge = track.charge();
    }
    
    #ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
    ESHandle<SetupData> pSetup;
    iSetup.get<SetupRecord>().get(pSetup);
    #endif
  */
}


// ------------ method called once each job just before starting event loop  ------------
void
boostedTauRecoAnalyzer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void
boostedTauRecoAnalyzer::endJob()
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
boostedTauRecoAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);

  //Specify that only 'tracks' is allowed
  //To use, remove the default given above and uncomment below
  //ParameterSetDescription desc;
  //desc.addUntracked<edm::InputTag>("tracks","ctfWithMaterialTracks");
  //descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(boostedTauRecoAnalyzer);
