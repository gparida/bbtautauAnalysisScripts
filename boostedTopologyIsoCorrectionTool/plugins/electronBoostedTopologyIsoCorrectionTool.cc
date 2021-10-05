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
//         Created:  Fri, 24 Sep 2021 19:32:33 GMT
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
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

#include "RecoEgamma/EgammaTools/interface/EffectiveAreas.h"


#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Common/interface/RefToPtr.h"

#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<>
// This will improve performance in multithreaded jobs.


using reco::TrackCollection;

class electronBoostedTopologyIsoCorrectionTool : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
   public:
      explicit electronBoostedTopologyIsoCorrectionTool(const edm::ParameterSet&);
      ~electronBoostedTopologyIsoCorrectionTool();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;
  virtual double electronCorrectIso(pat::Electron ele, double rho, double ea, edm::Handle<std::vector<pat::Tau>> boostedTauCollectionHandle);
  virtual double electronCorrectPFIso(pat::Electron ele, double rho, double ea, edm::Handle<std::vector<pat::Tau>> boostedTauCollectionHandle);
  virtual double muonCorrectIso(pat::Muon muon, edm::Handle<std::vector<pat::Tau>> boostedTauCollectionHandle);
  virtual double muonCorrectPFIso(pat::Muon muon, edm::Handle<std::vector<pat::Tau>> boostedTauCollectionHandle);
  // ----------member data ---------------------------
  edm::EDGetTokenT< std::vector<pat::Muon> > muonCollection;
  edm::EDGetTokenT< std::vector<pat::Electron> > electronCollection;
  edm::EDGetTokenT< std::vector<pat::Tau> > boostedTauCollection;
  edm::EDGetTokenT< std::vector<pat::Tau> > tauCollection;
  edm::EDGetTokenT< std::vector<pat::PackedCandidate> >  pfCandCollection;
  edm::EDGetTokenT<double> rhoSrc;
  edm::EDGetTokenT<bool> verboseDebug = false;
  EffectiveAreas theEffectiveAreas;
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
electronBoostedTopologyIsoCorrectionTool::electronBoostedTopologyIsoCorrectionTool(const edm::ParameterSet& iConfig)
 :
  muonCollection(consumes< std::vector<pat::Muon> > (iConfig.getParameter< edm::InputTag > ("muonCollection"))),
  electronCollection(consumes< std::vector<pat::Electron> >(iConfig.getParameter< edm::InputTag >("electronCollection"))),
  boostedTauCollection(consumes< std::vector<pat::Tau> >(iConfig.getParameter< edm::InputTag >("boostedTauCollection"))),
  tauCollection(consumes< std::vector<pat::Tau> >(iConfig.getParameter< edm::InputTag >("tauCollection"))),
  pfCandCollection(consumes< pat::PackedCandidateCollection >(iConfig.getParameter< edm::InputTag >("pfCands"))),
  rhoSrc(consumes<double> (iConfig.getParameter<edm::InputTag> ("rhoSrc"))),
  verboseDebug(iConfig.exists("verboseDebug") ? 
	       iConfig.getParameter<edm::InputTag>("verboseDebug") : 
	       false)
  theEffectiveAreas(iConfig.getParameter< edm::FileInPath>("EAConfigFile").fullPath())
{
   //now do what ever initialization is needed
}


electronBoostedTopologyIsoCorrectionTool::~electronBoostedTopologyIsoCorrectionTool()
{

   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
electronBoostedTopologyIsoCorrectionTool::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

   edm::Handle< std::vector<pat::Electron> > electronHandle;
   iEvent.getByToken(electronCollection, electronHandle);

   edm::Handle< std::vector<pat::Muon> > muonHandle;
   iEvent.getByToken(muonCollection, muonHandle);

   edm::Handle< std::vector<pat::Tau> > boostedTauHandle;
   iEvent.getByToken(boostedTauCollection, boostedTauHandle); 

   edm::Handle< std::vector<pat::PackedCandidate> > pfCandHandle;
   iEvent.getByToken(pfCandCollection, pfCandHandle);
   
   edm::Handle<double> rho;
   iEvent.getByToken(rhoSrc, rho);

   
   int nElectrons = electronHandle->size();
   if (verboseDebug) std::cout<<"nElectrons: "<<nElectrons<<std::endl;


   for (std::vector<pat::Electron>::const_iterator theElectron = electronHandle->begin();
	theElectron != electronHandle->end();
	++theElectron)
     {
       ++nElectrons;
      float effectiveArea;
      effectiveArea = theEffectiveAreas.getEffectiveArea(fabs(theElectron->superCluster()->eta()));


       double nano0p3ConeCalculation = theElectron->pfIsolationVariables().sumChargedHadronPt+std::max(0.0, theElectron->pfIsolationVariables().sumNeutralHadronEt+theElectron->pfIsolationVariables().sumPhotonEt-(*rho)*effectiveArea);
       double altNano0p3ConeCalculation = theElectron->chargedHadronIso()+std::max(0.0, theElectron->neutralHadronIso()+theElectron->photonIso()-(*rho)*effectiveArea);
       double nano0p4ConeCalculation = theElectron->chargedHadronIso()+std::max(0.0, theElectron->neutralHadronIso()+theElectron->photonIso()-(*rho)*effectiveArea*16.0/9.0); // 16/9 for 0.4 vs 0.3 cone?
       
       double theCorrIso = this->electronCorrectIso(*theElectron, *rho, effectiveArea, boostedTauHandle);
       double theCorrPFIso = this->electronCorrectPFIso(*theElectron, *rho, effectiveArea, boostedTauHandle);

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

	   std::cout<<"\nCorrected Nano calculation 0.3 cone: "<<theCorrPFIso<<std::endl;
	   std::cout<<"Corrected Alt Nano calculation 0.3 cone: "<<theCorrIso<<std::endl;

	   std::cout<<"***************************************"<<std::endl;
	 }

     }



#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
   ESHandle<SetupData> pSetup;
   iSetup.get<SetupRecord>().get(pSetup);
#endif
}


// ------------ method called once each job just before starting event loop  ------------
void
electronBoostedTopologyIsoCorrectionTool::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void
electronBoostedTopologyIsoCorrectionTool::endJob()
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
electronBoostedTopologyIsoCorrectionTool::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
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
//camilla's formula for the corrected isolation
double electronBoostedTopologyIsoCorrectionTool::electronCorrectIso(pat::Electron ele, double rho, double ea, edm::Handle<std::vector<pat::Tau>> boostedTauCollectionHandle)
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

  return correctedIso;
}

double electronBoostedTopologyIsoCorrectionTool::electronCorrectPFIso(pat::Electron ele, double rho, double ea, edm::Handle<std::vector<pat::Tau>> boostedTauCollectionHandle)
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
  double correctedIso = sumChargedHadronPt + std::max(sumNeutralHadronPt - rho * ea, 0.0);

  return correctedIso;
}


double electronBoostedTopologyIsoCorrectionTool::muonCorrectIso(pat::Muon muon, edm::Handle<std::vector<pat::Tau>> boostedTauCollectionHandle)
{
  return 0.0;
}
double electronBoostedTopologyIsoCorrectionTool::muonCorrectPFIso(pat::Muon muon, edm::Handle<std::vector<pat::Tau>> boostedTauCollectionHandle)
{
  return 0.0;
}

//define this as a plug-in
DEFINE_FWK_MODULE(electronBoostedTopologyIsoCorrectionTool);
