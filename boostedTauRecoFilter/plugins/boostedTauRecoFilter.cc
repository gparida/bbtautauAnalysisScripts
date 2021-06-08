// -*- C++ -*-
//
// Package:    bbtautauAnalysisScripts/boostedTauRecoFilter
// Class:      boostedTauRecoFilter
// 
/**\class boostedTauRecoFilter boostedTauRecoFilter.cc bbtautauAnalysisScripts/boostedTauRecoFilter/plugins/boostedTauRecoFilter.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Andrew Loeliger
//         Created:  Thu, 03 Jun 2021 12:12:45 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "DataFormats/PatCandidates/interface/Tau.h"


//
// class declaration
//

class boostedTauRecoFilter : public edm::stream::EDFilter<> {
   public:
      explicit boostedTauRecoFilter(const edm::ParameterSet&);
      ~boostedTauRecoFilter();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void beginStream(edm::StreamID) override;
      virtual bool filter(edm::Event&, const edm::EventSetup&) override;
      virtual void endStream() override;

      //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
      //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
  edm::EDGetTokenT< std::vector<pat::Tau> > boostedTauCollection;
  edm::EDGetTokenT< std::vector<pat::Tau> > tauCollection;

      // ----------member data ---------------------------
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
boostedTauRecoFilter::boostedTauRecoFilter(const edm::ParameterSet& iConfig):
  boostedTauCollection(consumes< std::vector<pat::Tau> >(iConfig.getParameter< edm::InputTag >("boostedTauCollection"))),
  tauCollection(consumes< std::vector<pat::Tau> >(iConfig.getParameter< edm::InputTag >("tauCollection")))
{
   //now do what ever initialization is needed

}


boostedTauRecoFilter::~boostedTauRecoFilter()
{
 
   // do anything here that needs to be done at destruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called on each new Event  ------------
//Okay, what are our filter conditions?
//let's ask for at least one boosted or non-boosted tau per event.
//
bool
boostedTauRecoFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{

  bool isGoodEvent = true;

  edm::Handle< std::vector<pat::Tau> > boostedTauHandle;
  iEvent.getByToken(boostedTauCollection, boostedTauHandle);
  
  edm::Handle< std::vector<pat::Tau> > tauHandle;
  iEvent.getByToken(tauCollection, tauHandle);

  if (boostedTauHandle->size() <= 0 && tauHandle->size() <= 0) isGoodEvent = false;


  return isGoodEvent;
  /*
  using namespace edm;
#ifdef THIS_IS_AN_EVENT_EXAMPLE
   Handle<ExampleData> pIn;
   iEvent.getByLabel("example",pIn);
#endif

#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
   ESHandle<SetupData> pSetup;
   iSetup.get<SetupRecord>().get(pSetup);
#endif
   return true;
  */
}

// ------------ method called once each stream before processing any runs, lumis or events  ------------
void
boostedTauRecoFilter::beginStream(edm::StreamID)
{
}

// ------------ method called once each stream after processing all runs, lumis and events  ------------
void
boostedTauRecoFilter::endStream() {
}

// ------------ method called when starting to processes a run  ------------
/*
void
boostedTauRecoFilter::beginRun(edm::Run const&, edm::EventSetup const&)
{ 
}
*/
 
// ------------ method called when ending the processing of a run  ------------
/*
void
boostedTauRecoFilter::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when starting to processes a luminosity block  ------------
/*
void
boostedTauRecoFilter::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when ending the processing of a luminosity block  ------------
/*
void
boostedTauRecoFilter::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
boostedTauRecoFilter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}
//define this as a plug-in
DEFINE_FWK_MODULE(boostedTauRecoFilter);
