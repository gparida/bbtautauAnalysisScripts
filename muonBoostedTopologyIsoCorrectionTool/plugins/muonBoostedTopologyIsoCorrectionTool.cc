// -*- C++ -*-
//
// Package:    bbtautauAnalysisScripts/muonBoostedTopologyIsoCorrectionTool
// Class:      muonBoostedTopologyIsoCorrectionTool
// 
/**\class muonBoostedTopologyIsoCorrectionTool muonBoostedTopologyIsoCorrectionTool.cc bbtautauAnalysisScripts/muonBoostedTopologyIsoCorrectionTool/plugins/muonBoostedTopologyIsoCorrectionTool.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Andrew Loeliger
//         Created:  Tue, 05 Oct 2021 21:42:37 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "RecoEgamma/EgammaTools/interface/EffectiveAreas.h"

#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Common/interface/RefToPtr.h"

#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/Muon.h"


//
// class declaration
//

class muonBoostedTopologyIsoCorrectionTool : public edm::stream::EDProducer<> {
   public:
      explicit muonBoostedTopologyIsoCorrectionTool(const edm::ParameterSet&);
      ~muonBoostedTopologyIsoCorrectionTool();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void beginStream(edm::StreamID) override;
      virtual void produce(edm::Event&, const edm::EventSetup&) override;
      virtual void endStream() override;

  virtual double muonCorrectPFIso(pat::Muon muo, edm::Handle<std::vector<pat::Tau>> boostedTauCollectionHandle);
      //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
      //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

      // ----------member data ---------------------------
  edm::EDGetTokenT< std::vector<pat::Muon> > muonCollection;
  edm::EDGetTokenT< std::vector<pat::Tau> > boostedTauCollection;
  edm::EDGetTokenT<double> rhoSrc;
  EffectiveAreas theEffectiveAreas;
  bool verboseDebug;
  
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
muonBoostedTopologyIsoCorrectionTool::muonBoostedTopologyIsoCorrectionTool(const edm::ParameterSet& iConfig):
  muonCollection(consumes< std::vector<pat::Muon> >(iConfig.getParameter< edm::InputTag >("muonCollection"))),
  boostedTauCollection(consumes< std::vector<pat::Tau> >(iConfig.getParameter< edm::InputTag >("boostedTauCollection"))),
  rhoSrc(consumes<double > (iConfig.getParameter< edm::InputTag >("rhoSrc"))),
  theEffectiveAreas(iConfig.getParameter< edm::FileInPath>("EAConfigFile").fullPath())
{
  verboseDebug = iConfig.exists("verboseDebug") ? iConfig.getParameter<bool>("verboseDebug"): false;
  //produces<edm::ValueMap<float>>("TauCorrIso");
  produces<edm::ValueMap<float>>("TauCorrPfIso");
   //register your products
/* Examples
   produces<ExampleData2>();

   //if do put with a label
   produces<ExampleData2>("label");
 
   //if you want to put into the Run
   produces<ExampleData2,InRun>();
*/
   //now do what ever other initialization is needed
  
}


muonBoostedTopologyIsoCorrectionTool::~muonBoostedTopologyIsoCorrectionTool()
{
 
   // do anything here that needs to be done at destruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to produce the data  ------------
void
muonBoostedTopologyIsoCorrectionTool::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

   edm::Handle< std::vector<pat::Muon> > muonHandle;
   iEvent.getByToken(muonCollection, muonHandle);
   
   edm::Handle< std::vector<pat::Tau> > boostedTauHandle;
   iEvent.getByToken(boostedTauCollection, boostedTauHandle);

   edm::Handle<double> rho;
   iEvent.getByToken(rhoSrc, rho);
   
   int nMuons = muonHandle->size();
   if (verboseDebug) std::cout<<"nMuons: "<<nMuons<<std::endl;

   std::vector<float> theCorrPFIsoVector;
   theCorrPFIsoVector.reserve(nMuons);

   for(std::vector<pat::Muon>::const_iterator theMuon = muonHandle->begin();
       theMuon != muonHandle->end();
       ++theMuon)
     {
       double muon0p3ConePFCalculation = theMuon->pfIsolationR03().sumChargedHadronPt + std::max(theMuon->pfIsolationR03().sumNeutralHadronEt + theMuon->pfIsolationR03().sumPhotonEt - theMuon->pfIsolationR03().sumPUPt/2.0, 0.0); //pf isolation, delta beta corrections

       double theCorrPFIso = this->muonCorrectPFIso(*theMuon, boostedTauHandle);

       theCorrPFIsoVector.push_back(theCorrPFIso);

       if(verboseDebug)
	 {
	   std::cout<<"***************************************"<<std::endl;
	   std::cout<<"Muon pt: "<<theMuon->pt()<<std::endl;

	   std::cout<<"\ntheMuon->pfIsolationR03().sumChargedHadronPt: "<<theMuon->pfIsolationR03().sumChargedHadronPt<<std::endl;
	   std::cout<<"theMuon->pfIsolationR03().sumNeutralHadronEt: "<<theMuon->pfIsolationR03().sumNeutralHadronEt<<std::endl;
	   std::cout<<"theMuon->pfIsolationR03().sumPhotonEt: "<<theMuon->pfIsolationR03().sumPhotonEt<<std::endl;
	   std::cout<<"theMuon->pfIsolationR03().sumPUPt: "<<theMuon->pfIsolationR03().sumPUPt<<std::endl;
	   
	   std::cout<<"Muon calculation: "<<muon0p3ConePFCalculation<<std::endl;
	   std::cout<<"Corrected muon calculation: "<<theCorrPFIso<<std::endl;
	   std::cout<<"***************************************"<<std::endl;
	 }
     }

   std::unique_ptr<edm::ValueMap<float>> corrPFIsoV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerCorrPFIso(*corrPFIsoV);
   fillerCorrPFIso.insert(muonHandle, theCorrPFIsoVector.begin(), theCorrPFIsoVector.end());
   fillerCorrPFIso.fill();

   iEvent.put(std::move(corrPFIsoV), "TauCorrPfIso");

/* This is an event example
   //Read 'ExampleData' from the Event
   Handle<ExampleData> pIn;
   iEvent.getByLabel("example",pIn);

   //Use the ExampleData to create an ExampleData2 which 
   // is put into the Event
   iEvent.put(std::make_unique<ExampleData2>(*pIn));
*/

/* this is an EventSetup example
   //Read SetupData from the SetupRecord in the EventSetup
   ESHandle<SetupData> pSetup;
   iSetup.get<SetupRecord>().get(pSetup);
*/
 
}

// ------------ method called once each stream before processing any runs, lumis or events  ------------
void
muonBoostedTopologyIsoCorrectionTool::beginStream(edm::StreamID)
{
}

// ------------ method called once each stream after processing all runs, lumis and events  ------------
void
muonBoostedTopologyIsoCorrectionTool::endStream() {
}

// ------------ method called when starting to processes a run  ------------
/*
void
muonBoostedTopologyIsoCorrectionTool::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when ending the processing of a run  ------------
/*
void
muonBoostedTopologyIsoCorrectionTool::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when starting to processes a luminosity block  ------------
/*
void
muonBoostedTopologyIsoCorrectionTool::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when ending the processing of a luminosity block  ------------
/*
void
muonBoostedTopologyIsoCorrectionTool::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
muonBoostedTopologyIsoCorrectionTool::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

double muonBoostedTopologyIsoCorrectionTool::muonCorrectPFIso(pat::Muon muo, edm::Handle<std::vector<pat::Tau>> boostedTauCollectionHandle)
{
  double tauSumChargedHadronPt = 0.0;
  double tauSumNeutralHadronEt = 0.0;
  double tauSumPhotonEt        = 0.0;
  double dRmin = 0.4;
  pat::TauRef matchedTau;

  for(size_t tauIndex = 0; tauIndex < boostedTauCollectionHandle->size(); tauIndex++)
    {
      pat::TauRef theTau(boostedTauCollectionHandle, tauIndex);
      double deltaR = reco::deltaR(muo.eta(), muo.phi(), theTau->eta(), theTau->phi());
      if (deltaR < dRmin && deltaR > 0.02 &&
	  theTau->pt() > 20 &&
	  fabs(theTau->eta())<2.4 &&
	  theTau->tauID("decayModeFindingNewDMs") > 0.5 &&
	  theTau->tauID("byVVLooseIsolationMVArun2v1DBnewDMwLT") > 0.5)
	{
	  matchedTau = theTau;
	  dRmin = deltaR;
	}
    }

  if(matchedTau.isNonnull())
    {
      for (size_t hadrCandInd = 0;
	   hadrCandInd < matchedTau->signalChargedHadrCands().size();
	   hadrCandInd++)
	{
	  double dRConst = reco::deltaR(muo.eta(), muo.phi(), matchedTau->signalChargedHadrCands()[hadrCandInd]->eta(), matchedTau->signalChargedHadrCands()[hadrCandInd]->phi());
	  if (dRConst < 0.3) tauSumChargedHadronPt += matchedTau->signalChargedHadrCands()[hadrCandInd]->pt();
	}
      for(size_t neutCandInd = 0;
	  neutCandInd < matchedTau->signalNeutrHadrCands().size();
	  ++neutCandInd)
	{
	  double dRConst = reco::deltaR(muo.eta(), muo.phi(), matchedTau->signalNeutrHadrCands()[neutCandInd]->eta(), matchedTau->signalNeutrHadrCands()[neutCandInd]->phi());
	  if (dRConst < 0.3) tauSumNeutralHadronEt += matchedTau->signalNeutrHadrCands()[neutCandInd]->pt();
	}
      for(size_t photonCandInd = 0;
	  photonCandInd < matchedTau->signalGammaCands().size();
	  ++photonCandInd)
	{
	  double dRConst = reco::deltaR(muo.eta(), muo.phi(), matchedTau->signalGammaCands()[photonCandInd]->eta(), matchedTau->signalGammaCands()[photonCandInd]->phi());
	  if (dRConst < 0.3) tauSumPhotonEt += matchedTau->signalGammaCands()[photonCandInd]->pt();
	}
    }

  double sumChargedHadronPt = std::max(0.0, muo.pfIsolationR03().sumChargedHadronPt-tauSumChargedHadronPt);
  double sumNeutralHadronPt = std::max(0.0, muo.pfIsolationR03().sumNeutralHadronEt-tauSumNeutralHadronEt+muo.pfIsolationR03().sumPhotonEt-tauSumPhotonEt);
  double correctedIso = sumChargedHadronPt + std::max(sumNeutralHadronPt - muo.pfIsolationR03().sumPUPt/2.0, 0.0);

  return correctedIso;
}

//define this as a plug-in
DEFINE_FWK_MODULE(muonBoostedTopologyIsoCorrectionTool);
