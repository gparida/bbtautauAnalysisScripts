// -*- C++ -*-
//
// Package:    bbtautauAnalysisScripts/electronBoostedTopologyIsoCorrectionTool
// Class:      electronBoostedTopologyIsoCorrectionTool
// 
/**\class electronBoostedTopologyIsoCorrectionTool electronBoostedTopologyIsoCorrectionTool.cc bbtautauAnalysisScripts/electronBoostedTopologyIsoCorrectionTool/plugins/electronBoostedTopologyIsoCorrectionTool.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Andrew Loeliger
//         Created:  Tue, 05 Oct 2021 21:42:27 GMT
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
#include "DataFormats/PatCandidates/interface/Electron.h"

//
// class declaration
//

class electronBoostedTopologyIsoCorrectionTool : public edm::stream::EDProducer<> {
   public:
      explicit electronBoostedTopologyIsoCorrectionTool(const edm::ParameterSet&);
      ~electronBoostedTopologyIsoCorrectionTool();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void beginStream(edm::StreamID) override;
      virtual void produce(edm::Event&, const edm::EventSetup&) override;
      virtual void endStream() override;

  virtual std::vector<double> electronCorrectIso(pat::Electron ele, double rho, double ea, edm::Handle<std::vector<pat::Tau>> boostedTauCollectionHandle);
  virtual std::vector<double> electronCorrectPFIso(pat::Electron ele, double rho, double ea, edm::Handle<std::vector<pat::Tau>> boostedTauCollectionHandle);

      //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
      //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

      // ----------member data ---------------------------
  edm::EDGetTokenT< std::vector<pat::Electron> > electronCollection;
  edm::EDGetTokenT< std::vector<pat::Tau> > boostedTauCollection;
  edm::EDGetTokenT<double> rhoSrc;
  EffectiveAreas theEffectiveAreas;
  bool verboseDebug;

  struct isolationVariables{
    //Non PF variables
    double correctedIso_Andrew;
    double tauSumChargedHadronPt;
    double tauSumNeutralHadronEt;
    double tauSumPhotonEt;

    //commmon variables
    double rho;
    double ea;

  };

  struct PFisolationVariables{
    //Non PF variables
    double correctedIso_Andrew;
    double tauSumChargedHadronPt;
    double tauSumNeutralHadronEt;
    double tauSumPhotonEt;

    //commmon variables
    double rho;
    double ea;

  };

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
electronBoostedTopologyIsoCorrectionTool::electronBoostedTopologyIsoCorrectionTool(const edm::ParameterSet& iConfig):
    electronCollection(consumes< std::vector<pat::Electron> >(iConfig.getParameter< edm::InputTag >("electronCollection"))),
  boostedTauCollection(consumes< std::vector<pat::Tau> >(iConfig.getParameter< edm::InputTag >("boostedTauCollection"))),
  rhoSrc(consumes<double> (iConfig.getParameter<edm::InputTag> ("rhoSrc"))),
  theEffectiveAreas(iConfig.getParameter< edm::FileInPath>("EAConfigFile").fullPath())
{
    verboseDebug = iConfig.exists("verboseDebug") ? iConfig.getParameter<bool>("verboseDebug"): false;
    produces<edm::ValueMap<float>>("TauCorrIso");
    produces<edm::ValueMap<float>>("TauCorrPfIso");
    produces<edm::ValueMap<float>>("PfSumChaHadPt"); 
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


electronBoostedTopologyIsoCorrectionTool::~electronBoostedTopologyIsoCorrectionTool()
{
 
   // do anything here that needs to be done at destruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to produce the data  ------------
void
electronBoostedTopologyIsoCorrectionTool::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
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

   edm::Handle< std::vector<pat::Electron> > electronHandle;
   iEvent.getByToken(electronCollection, electronHandle);

   edm::Handle< std::vector<pat::Tau> > boostedTauHandle;
   iEvent.getByToken(boostedTauCollection, boostedTauHandle); 
   
   edm::Handle<double> rho;
   iEvent.getByToken(rhoSrc, rho);
   
   int nElectrons = electronHandle->size();
   if (verboseDebug) std::cout<<"nElectrons: "<<nElectrons<<std::endl;


   std::vector<float> theCorrIsoVector, theCorrPFIsoVector,thePFSumHadPtVector;
   theCorrIsoVector.reserve(nElectrons);
   theCorrPFIsoVector.reserve(nElectrons);
   thePFSumHadPtVector.reserve(nElectrons);

   for (std::vector<pat::Electron>::const_iterator theElectron = electronHandle->begin();
	theElectron != electronHandle->end();
	++theElectron)
     {
      float effectiveArea;
      effectiveArea = theEffectiveAreas.getEffectiveArea(fabs(theElectron->superCluster()->eta()));


       double nano0p3ConeCalculation = theElectron->pfIsolationVariables().sumChargedHadronPt+std::max(0.0, theElectron->pfIsolationVariables().sumNeutralHadronEt+theElectron->pfIsolationVariables().sumPhotonEt-(*rho)*effectiveArea);
       double altNano0p3ConeCalculation = theElectron->chargedHadronIso()+std::max(0.0, theElectron->neutralHadronIso()+theElectron->photonIso()-(*rho)*effectiveArea);
       double nano0p4ConeCalculation = theElectron->chargedHadronIso()+std::max(0.0, theElectron->neutralHadronIso()+theElectron->photonIso()-(*rho)*effectiveArea*16.0/9.0); // 16/9 for 0.4 vs 0.3 cone?
       
       //double theCorrIso = this->electronCorrectIso(*theElectron, *rho, effectiveArea, boostedTauHandle);
       //double theCorrPFIso = this->electronCorrectPFIso(*theElectron, *rho, effectiveArea, boostedTauHandle);
       isolationVariables theCorrIso;
       theCorrIso.correctedIso_Andrew = this->electronCorrectIso(*theElectron, *rho, effectiveArea, boostedTauHandle)[0];
       PFisolationVariables theCorrPFIso;
       theCorrPFIso.correctedIso_Andrew = this->electronCorrectPFIso(*theElectron, *rho, effectiveArea, boostedTauHandle)[0];

       theCorrIsoVector.push_back(theCorrIso.correctedIso_Andrew);

       theCorrPFIsoVector.push_back(theCorrPFIso.correctedIso_Andrew);
       thePFSumHadPtVector.push_back(theCorrPFIso.tauSumChargedHadronPt);

       if(verboseDebug)
	 {
	   std::cout<<"***************************************"<<std::endl;       
	   std::cout<<"Electron pt: "<<theElectron->pt()<<std::endl;

	   std::cout<<"\nNorm: theElectron->pfIsolationVariables().sumChargedHadronPt: "<<theElectron->pfIsolationVariables().sumChargedHadronPt<<std::endl;
	   std::cout<<"Norm: theElectron->pfIsolationVariables().sumNeutralHadronEt: "<<theElectron->pfIsolationVariables().sumNeutralHadronEt<<std::endl;
	   std::cout<<"Norm: theElectron->pfIsolationVariables().sumPhotonEt: "<<theElectron->pfIsolationVariables().sumPhotonEt<<std::endl;
	   std::cout<<"Norm: std::max(0.0, theElectron->pfIsolationVariables().sumNeutralHadronEt+theElectron->pfIsolationVariables().sumPhotonEt-(*rho)*effectiveArea)"<<std::max(0.0, theElectron->pfIsolationVariables().sumNeutralHadronEt+theElectron->pfIsolationVariables().sumPhotonEt-(*rho)*effectiveArea)<<std::endl;
       
	   std::cout<<"\nAlt: theElectron->chargedHadronIso(): "<<theElectron->chargedHadronIso()<<std::endl;
	   std::cout<<"Alt: theElectron->neutralHadronIso(): "<<theElectron->neutralHadronIso()<<std::endl;
	   std::cout<<"Alt: theElectron->photonIso(): "<<theElectron->photonIso()<<std::endl;
	   std::cout<<"Alt: std::max(0.0, theElectron->neutralHadronIso()+theElectron->photonIso()-(*rho)*effectiveArea): "<<std::max(0.0, theElectron->neutralHadronIso()+theElectron->photonIso()-(*rho)*effectiveArea)<<std::endl;
       
	   std::cout<<"\nRho: "<<*rho<<std::endl;
	   std::cout<<"effectiveArea: "<<effectiveArea<<std::endl;

	   std::cout<<"\nNano calculation 0.3 cone: "<<nano0p3ConeCalculation<<std::endl;
	   std::cout<<"Alt-Nano calculation 0.3 cone: "<<altNano0p3ConeCalculation<<std::endl;
	   std::cout<<"Nano calculation 0.4 cone: "<<nano0p4ConeCalculation<<std::endl;

	   std::cout<<"\nCorrected Nano calculation 0.3 cone: "<<theCorrPFIso.correctedIso_Andrew<<std::endl;
	   std::cout<<"Corrected Alt Nano calculation 0.3 cone: "<<theCorrIso.correctedIso_Andrew<<std::endl;

	   std::cout<<"***************************************"<<std::endl;
	 }

     }

   std::unique_ptr<edm::ValueMap<float>> corrIsoV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerCorrIso(*corrIsoV);
   fillerCorrIso.insert(electronHandle, theCorrIsoVector.begin(), theCorrIsoVector.end());
   fillerCorrIso.fill();

   std::unique_ptr<edm::ValueMap<float>> corrPFIsoV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerCorrPFIso(*corrPFIsoV);
   fillerCorrPFIso.insert(electronHandle, theCorrPFIsoVector.begin(), theCorrPFIsoVector.end());
   fillerCorrPFIso.fill();

   std::unique_ptr<edm::ValueMap<float>> PFsumHadChaV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerPFsumHadCha(*PFsumHadChaV);
   fillerPFsumHadCha.insert(electronHandle, thePFSumHadPtVector.begin(), thePFSumHadPtVector.end());
   fillerPFsumHadCha.fill();


   iEvent.put(std::move(corrIsoV), "TauCorrIso");
   iEvent.put(std::move(corrPFIsoV), "TauCorrPfIso");
   iEvent.put(std::move(PFsumHadChaV), "PfSumChaHad");
   
 
}

// ------------ method called once each stream before processing any runs, lumis or events  ------------
void
electronBoostedTopologyIsoCorrectionTool::beginStream(edm::StreamID)
{
}

// ------------ method called once each stream after processing all runs, lumis and events  ------------
void
electronBoostedTopologyIsoCorrectionTool::endStream() {
}

// ------------ method called when starting to processes a run  ------------
/*
void
electronBoostedTopologyIsoCorrectionTool::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when ending the processing of a run  ------------
/*
void
electronBoostedTopologyIsoCorrectionTool::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when starting to processes a luminosity block  ------------
/*
void
electronBoostedTopologyIsoCorrectionTool::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when ending the processing of a luminosity block  ------------
/*
void
electronBoostedTopologyIsoCorrectionTool::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
electronBoostedTopologyIsoCorrectionTool::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

std::vector<double> electronBoostedTopologyIsoCorrectionTool::electronCorrectIso(pat::Electron ele, double rho, double ea, edm::Handle<std::vector<pat::Tau>> boostedTauCollectionHandle)
{
  double tauSumChargedHadronPt = 0.0;
  double tauSumNeutralHadronEt = 0.0;
  double tauSumPhotonEt        = 0.0;
  double dRmin = 0.4;
  pat::TauRef matchedTau;

  for(size_t tauIndex = 0; tauIndex < boostedTauCollectionHandle->size(); tauIndex++)
    {
      pat::TauRef theTau(boostedTauCollectionHandle, tauIndex);
      double deltaR = reco::deltaR(ele.eta(), ele.phi(), theTau->eta(), theTau->phi());
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
	  double dRConst = reco::deltaR(ele.eta(), ele.phi(), matchedTau->signalChargedHadrCands()[hadrCandInd]->eta(), matchedTau->signalChargedHadrCands()[hadrCandInd]->phi());
	  if (dRConst < 0.3) tauSumChargedHadronPt += matchedTau->signalChargedHadrCands()[hadrCandInd]->pt();
	}
      for(size_t neutCandInd = 0;
	  neutCandInd < matchedTau->signalNeutrHadrCands().size();
	  ++neutCandInd)
	{
	  double dRConst = reco::deltaR(ele.eta(), ele.phi(), matchedTau->signalNeutrHadrCands()[neutCandInd]->eta(), matchedTau->signalNeutrHadrCands()[neutCandInd]->phi());
	  if (dRConst < 0.3) tauSumNeutralHadronEt += matchedTau->signalNeutrHadrCands()[neutCandInd]->pt();
	}
      for(size_t photonCandInd = 0;
	  photonCandInd < matchedTau->signalGammaCands().size();
	  ++photonCandInd)
	{
	  double dRConst = reco::deltaR(ele.eta(), ele.phi(), matchedTau->signalGammaCands()[photonCandInd]->eta(), matchedTau->signalGammaCands()[photonCandInd]->phi());
	  if (dRConst < 0.3) tauSumPhotonEt += matchedTau->signalGammaCands()[photonCandInd]->pt();
	}
    }

  double sumChargedHadronPt = std::max(0.0, ele.chargedHadronIso()-tauSumChargedHadronPt);
  double sumNeutralHadronPt = std::max(0.0, ele.neutralHadronIso()-tauSumNeutralHadronEt+ele.photonIso()-tauSumPhotonEt);
  double correctedIso = sumChargedHadronPt + std::max(sumNeutralHadronPt - rho * ea, 0.0);

  return {correctedIso, sumChargedHadronPt, sumNeutralHadronPt, tauSumPhotonEt, rho, ea};
}

std::vector<double> electronBoostedTopologyIsoCorrectionTool::electronCorrectPFIso(pat::Electron ele, double rho, double ea, edm::Handle<std::vector<pat::Tau>> boostedTauCollectionHandle)
{
  double tauSumChargedHadronPt = 0.0;
  double tauSumNeutralHadronEt = 0.0;
  double tauSumPhotonEt        = 0.0;
  double dRmin = 0.4;
  pat::TauRef matchedTau;

  for(size_t tauIndex = 0; tauIndex < boostedTauCollectionHandle->size(); tauIndex++)
    {
      pat::TauRef theTau(boostedTauCollectionHandle, tauIndex);
      double deltaR = reco::deltaR(ele.eta(), ele.phi(), theTau->eta(), theTau->phi());
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
	  double dRConst = reco::deltaR(ele.eta(), ele.phi(), matchedTau->signalChargedHadrCands()[hadrCandInd]->eta(), matchedTau->signalChargedHadrCands()[hadrCandInd]->phi());
	  if (dRConst < 0.3) tauSumChargedHadronPt += matchedTau->signalChargedHadrCands()[hadrCandInd]->pt();
	}
      for(size_t neutCandInd = 0;
	  neutCandInd < matchedTau->signalNeutrHadrCands().size();
	  ++neutCandInd)
	{
	  double dRConst = reco::deltaR(ele.eta(), ele.phi(), matchedTau->signalNeutrHadrCands()[neutCandInd]->eta(), matchedTau->signalNeutrHadrCands()[neutCandInd]->phi());
	  if (dRConst < 0.3) tauSumNeutralHadronEt += matchedTau->signalNeutrHadrCands()[neutCandInd]->pt();
	}
      for(size_t photonCandInd = 0;
	  photonCandInd < matchedTau->signalGammaCands().size();
	  ++photonCandInd)
	{
	  double dRConst = reco::deltaR(ele.eta(), ele.phi(), matchedTau->signalGammaCands()[photonCandInd]->eta(), matchedTau->signalGammaCands()[photonCandInd]->phi());
	  if (dRConst < 0.3) tauSumPhotonEt += matchedTau->signalGammaCands()[photonCandInd]->pt();
	}
    }

  double sumChargedHadronPt = std::max(0.0, ele.pfIsolationVariables().sumChargedHadronPt-tauSumChargedHadronPt);
  double sumNeutralHadronPt = std::max(0.0, ele.pfIsolationVariables().sumNeutralHadronEt-tauSumNeutralHadronEt+ele.pfIsolationVariables().sumPhotonEt-tauSumPhotonEt);
  double correctedIso = sumChargedHadronPt + std::max(sumNeutralHadronPt - rho * ea
  , 0.0);

  return {correctedIso, sumChargedHadronPt, sumNeutralHadronPt, tauSumPhotonEt, rho, ea};
}


//define this as a plug-in
DEFINE_FWK_MODULE(electronBoostedTopologyIsoCorrectionTool);
