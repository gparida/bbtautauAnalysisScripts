// -*- C++ -*-
//
// Package:    bbtautauAnalysisScripts/boostedTauLeadingLeptonIso
// Class:      boostedTauLeadingLeptonIso
// 
/**\class boostedTauLeadingLeptonIso boostedTauLeadingLeptonIso.cc bbtautauAnalysisScripts/boostedTauLeadingLeptonIso/plugins/boostedTauLeadingLeptonIso.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Andrew Loeliger
//         Created:  Wed, 20 Jul 2022 18:32:01 GMT
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
#include "DataFormats/PatCandidates/interface/Muon.h"


struct leptonInfo
{
float pt = -1.0;
float eta = 0.0;
float phi = 0.0;
float m = 0.0;

float sumPFChargedHadronPt = 0.0;
float sumPFNeutralHadronPt = 0.0;
float sumPFPhotonPt = 0.0;

float sumPUPt = 0.0;

float rho = 0.0;
float ea = 0.0;

float correctedSumPFChargedHadronPt = 0.0;
float correctedSumPFNeutralHadronPt = 0.0;

float correctedIso = 0.0;
};

//
// class declaration
//

class boostedTauLeadingLeptonIso : public edm::stream::EDProducer<> {
   public:
      explicit boostedTauLeadingLeptonIso(const edm::ParameterSet&);
      ~boostedTauLeadingLeptonIso();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void beginStream(edm::StreamID) override;
      virtual void produce(edm::Event&, const edm::EventSetup&) override;
      virtual void endStream() override;
      virtual void calculateCorrectedMuonIsoInformation(pat::Tau theTau, leptonInfo &theMuonInfo);
      virtual void calculateCorrectedElectronIsoInformation(pat::Tau theTau, leptonInfo &theElectronInfo);

      //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
      //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

      // ----------member data ---------------------------
  edm::EDGetTokenT< std::vector<pat::Muon> > muonCollection;
  edm::EDGetTokenT< std::vector<pat::Electron> > electronCollection;
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
boostedTauLeadingLeptonIso::boostedTauLeadingLeptonIso(const edm::ParameterSet& iConfig):
  muonCollection(consumes< std::vector<pat::Muon> >(iConfig.getParameter< edm::InputTag >("muonCollection"))),
  electronCollection(consumes< std::vector<pat::Electron> >(iConfig.getParameter< edm::InputTag >("electronCollection"))),
  boostedTauCollection(consumes< std::vector<pat::Tau> >(iConfig.getParameter< edm::InputTag >("boostedTauCollection"))),
  rhoSrc(consumes<double > (iConfig.getParameter< edm::InputTag >("rhoSrc"))),
  theEffectiveAreas(iConfig.getParameter< edm::FileInPath>("EAConfigFile").fullPath())
{
  verboseDebug = iConfig.exists("verboseDebug") ? iConfig.getParameter<bool>("verboseDebug"): false; 
  //Muon isolation products
  //Stores pt eta phi m
  //and implied isolation of leading, sub-leading, and sub-subleading muons
  //if this boosted tau is used
  produces<edm::ValueMap<float>>("LeadingMuonPt");
  produces<edm::ValueMap<float>>("LeadingMuonEta");
  produces<edm::ValueMap<float>>("LeadingMuonPhi");
  produces<edm::ValueMap<float>>("LeadingMuonM");
  produces<edm::ValueMap<float>>("LeadingMuonCorrIso");

  produces<edm::ValueMap<float>>("SubLeadingMuonPt");
  produces<edm::ValueMap<float>>("SubLeadingMuonEta");
  produces<edm::ValueMap<float>>("SubLeadingMuonPhi");
  produces<edm::ValueMap<float>>("SubLeadingMuonM");
  produces<edm::ValueMap<float>>("SubLeadingMuonCorrIso");

  produces<edm::ValueMap<float>>("SubSubLeadingMuonPt");
  produces<edm::ValueMap<float>>("SubSubLeadingMuonEta");
  produces<edm::ValueMap<float>>("SubSubLeadingMuonPhi");
  produces<edm::ValueMap<float>>("SubSubLeadingMuonM");
  produces<edm::ValueMap<float>>("SubSubLeadingMuonCorrIso");

  produces<edm::ValueMap<int>>("Mcounter");

  //Electron isolation products
  //Stores pt eta phi m
  //and implied isolation of leading, sub-leading, and sub-subleading electons
  //if this boosted tau is used 

  produces<edm::ValueMap<float>>("LeadingElectronPt");
  produces<edm::ValueMap<float>>("LeadingElectronEta");
  produces<edm::ValueMap<float>>("LeadingElectronPhi");
  produces<edm::ValueMap<float>>("LeadingElectronM");
  produces<edm::ValueMap<float>>("LeadingElectronCorrIso");

  produces<edm::ValueMap<float>>("SubLeadingElectronPt");
  produces<edm::ValueMap<float>>("SubLeadingElectronEta");
  produces<edm::ValueMap<float>>("SubLeadingElectronPhi");
  produces<edm::ValueMap<float>>("SubLeadingElectronM");
  produces<edm::ValueMap<float>>("SubLeadingElectronCorrIso");

  produces<edm::ValueMap<float>>("SubSubLeadingElectronPt");
  produces<edm::ValueMap<float>>("SubSubLeadingElectronEta");
  produces<edm::ValueMap<float>>("SubSubLeadingElectronPhi");
  produces<edm::ValueMap<float>>("SubSubLeadingElectronM");
  produces<edm::ValueMap<float>>("SubSubLeadingElectronCorrIso");

  produces<edm::ValueMap<int>>("Ecounter");
}


boostedTauLeadingLeptonIso::~boostedTauLeadingLeptonIso()
{
 
   // do anything here that needs to be done at destruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to produce the data  ------------
void
boostedTauLeadingLeptonIso::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

   edm::Handle< std::vector<pat::Muon> > muonHandle;
   iEvent.getByToken(muonCollection, muonHandle);

   edm::Handle< std::vector<pat::Electron> > electronHandle;
   iEvent.getByToken(electronCollection, electronHandle);
   
   edm::Handle< std::vector<pat::Tau> > boostedTauHandle;
   iEvent.getByToken(boostedTauCollection, boostedTauHandle);

   edm::Handle<double> rho;
   iEvent.getByToken(rhoSrc, rho);
   
   int nMuons = muonHandle->size();
   int nElectrons = electronHandle->size();
   if (verboseDebug) std::cout<<"nMuons: "<<nMuons<<std::endl;
   if (verboseDebug) std::cout<<"nElectrons: "<<nElectrons<<std::endl;

   double dRmin = 0.4;
   double deltaR = 0.0;

   int eleCounter = 0;
   int mCounter = 0;


   //These vectors will store the information about corrected leptons that we make later
   std::vector<float> leadingMuonVector_pt, leadingMuonVector_eta, leadingMuonVector_phi, leadingMuonVector_m, leadingMuonVector_corrIso;
   std::vector<float> subleadingMuonVector_pt, subleadingMuonVector_eta, subleadingMuonVector_phi, subleadingMuonVector_m, subleadingMuonVector_corrIso;
   std::vector<float> subsubleadingMuonVector_pt, subsubleadingMuonVector_eta, subsubleadingMuonVector_phi, subsubleadingMuonVector_m, subsubleadingMuonVector_corrIso;
   std::vector<int> Mcounter;

   std::vector<float> leadingElectronVector_pt, leadingElectronVector_eta, leadingElectronVector_phi, leadingElectronVector_m, leadingElectronVector_corrIso;
   std::vector<float> subleadingElectronVector_pt, subleadingElectronVector_eta, subleadingElectronVector_phi, subleadingElectronVector_m, subleadingElectronVector_corrIso;
   std::vector<float> subsubleadingElectronVector_pt, subsubleadingElectronVector_eta, subsubleadingElectronVector_phi, subsubleadingElectronVector_m, subsubleadingElectronVector_corrIso;
   std::vector<int> Ecounter;

   //Okay, the idea here is that for each boosted tau we have,
   //we go through and check each lepton
   //we first make a structure that contains it's 4 vector info,
   //and then the corrected isolation values that would be implied from that boosted tau
   //we store this for the leading, subleading, and sub-sub-leading electrons, and muons

   leptonInfo nullInfo;

   for(std::vector<pat::Tau>::const_iterator theBoostedTau = boostedTauHandle->begin();
       theBoostedTau != boostedTauHandle->end();
       ++theBoostedTau)
     {
       eleCounter = 0;
       mCounter = 0;

       std::vector< leptonInfo > electronInformation;
       std::vector< leptonInfo > muonInformation;

       //For each boosted tau we now go through the list of muons and electrons, and select 
       // the three leading candidates of each, and store their information in our vectors of
       // lepton information

       //once we have their information, we ca go through and calculate the corrected isolation
       //of each of the supplied leptons, with respect to the given boosted tau
       for(std::vector<pat::Muon>::const_iterator theMuon = muonHandle->begin();
	   theMuon != muonHandle->end();
	   ++theMuon)
	 {
	   leptonInfo currentMuonInfo;
	   currentMuonInfo.pt = theMuon->pt();
	   currentMuonInfo.eta = theMuon->eta();
	   currentMuonInfo.phi = theMuon->phi();
	   currentMuonInfo.m = theMuon->mass();
	   currentMuonInfo.sumPFChargedHadronPt = theMuon->pfIsolationR03().sumChargedHadronPt;
	   currentMuonInfo.sumPFNeutralHadronPt = theMuon->pfIsolationR03().sumNeutralHadronEt;
	   currentMuonInfo.sumPFPhotonPt = theMuon->pfIsolationR03().sumPhotonEt;
	   currentMuonInfo.sumPUPt = theMuon->pfIsolationR03().sumPUPt;

	   //loop through our information collection
	   //If we have higher pt than the current entry, we insert this lepton's information before

     deltaR = reco::deltaR(currentMuonInfo.eta, currentMuonInfo.phi, theBoostedTau->eta(), theBoostedTau->phi());

     if (deltaR < dRmin && deltaR > 0.02 && theMuon->passed(reco::Muon::CutBasedIdLoose))
     {
        bool insertAtEnd = true;
        mCounter++;
        for (std::vector< leptonInfo >::const_iterator muonInfoIt = muonInformation.begin();
		muonInfoIt != muonInformation.end();
		++muonInfoIt)
	     {
	       if (currentMuonInfo.pt > (*muonInfoIt).pt) 
		 {
		   insertAtEnd = false;
		   muonInformation.insert(muonInfoIt, currentMuonInfo);
		   break;
		 }
	     }
	   //If we are at the end, insert the information at the end
	   if (insertAtEnd) muonInformation.insert(muonInformation.end(), currentMuonInfo);
	   //then if we have more than 3 entries in the list of information, get rid of the 
	   //last entry
	   if (muonInformation.size() > 3) muonInformation.pop_back();

     }
	   
	 }
       //Now that we have all of the leading muons and their information, let's go through
       //and calculated a rectified muon isolation for each of them
       for (std::vector< leptonInfo >::iterator muonInfoIt = muonInformation.begin();
	    muonInfoIt != muonInformation.end();
	    ++muonInfoIt) this->calculateCorrectedMuonIsoInformation(*theBoostedTau, *muonInfoIt);
       //if any slots in the information vector are empty, let's create null information
       //to fill them
       int nullMuonEntriesNeeded = (int)(3-muonInformation.size());
       for (int i=0; i< nullMuonEntriesNeeded;++i)
	 {
	   //leptonInfo nullInfo;
	   muonInformation.push_back(nullInfo);
	 }

       //Now we do something similar for the electrons
       for(std::vector<pat::Electron>::const_iterator theElectron = electronHandle->begin();
	   theElectron != electronHandle->end();
	   ++theElectron)
	 {
	   leptonInfo currentElectronInfo;
	   currentElectronInfo.pt = theElectron->pt();
	   currentElectronInfo.eta = theElectron->eta();
	   currentElectronInfo.phi = theElectron->phi();
	   currentElectronInfo.m = theElectron->mass();
	   currentElectronInfo.sumPFChargedHadronPt = theElectron->pfIsolationVariables().sumChargedHadronPt;
	   currentElectronInfo.sumPFNeutralHadronPt = theElectron->pfIsolationVariables().sumNeutralHadronEt;
	   currentElectronInfo.sumPFPhotonPt = theElectron->pfIsolationVariables().sumPhotonEt;
	   
	   currentElectronInfo.rho = *rho;
	   currentElectronInfo.ea = theEffectiveAreas.getEffectiveArea(fabs(theElectron->superCluster()->eta()));

	   //loop through our information collection
	   //if we have a higher pt than the current entry, we insert this lepton's information before
     deltaR = reco::deltaR(currentElectronInfo.eta, currentElectronInfo.phi, theBoostedTau->eta(), theBoostedTau->phi());

     if (deltaR < dRmin && deltaR > 0.02 && theElectron->electronID("cutBasedElectronID-Fall17-94X-V2-loose"))
     {
     eleCounter++;
	   bool insertAtEnd = true;
	   for(std::vector< leptonInfo >::const_iterator electronInfoIt = electronInformation.begin();
	       electronInfoIt != electronInformation.end();
	       ++electronInfoIt)
	     {
	       if(currentElectronInfo.pt > (*electronInfoIt).pt)
		 {
		   insertAtEnd = false;
		   electronInformation.insert(electronInfoIt, currentElectronInfo);
		   break;
		 }
	     }
	   //if we are at the end, insert the information at the end
	   if (insertAtEnd) electronInformation.insert(electronInformation.end(), currentElectronInfo);
	   //now, if we have more than 3 entries, get rid of the last entry in the list
	   if (electronInformation.size() > 3) electronInformation.pop_back();
     }
	 }
       //Now that we have all of the leading electrons and their information, let's go through
       //and calculated a rectified electron isolation for each of them
       for (std::vector< leptonInfo >::iterator electronInfoIt = electronInformation.begin();
	    electronInfoIt != electronInformation.end();
	    ++electronInfoIt) this->calculateCorrectedMuonIsoInformation(*theBoostedTau, *electronInfoIt);
       //if any slots in the information vector are empty, let's create null information
       //to fill them
       int nullElectronEntriesNeeded = (int)(3-electronInformation.size());
       for (int i=0; i<nullElectronEntriesNeeded;++i)
	 {
	   leptonInfo nullInfo;
	   electronInformation.push_back(nullInfo);
	 }
       //Now that we have all the correct information for this tau, we can read out all the information
       //to a series of vectors that we will store later
       leadingMuonVector_pt.push_back(muonInformation[0].pt); 
       leadingMuonVector_eta.push_back(muonInformation[0].eta); 
       leadingMuonVector_phi.push_back(muonInformation[0].phi);
       leadingMuonVector_m.push_back(muonInformation[0].m);
       leadingMuonVector_corrIso.push_back(muonInformation[0].correctedIso);
       subleadingMuonVector_pt.push_back(muonInformation[1].pt); 
       subleadingMuonVector_eta.push_back(muonInformation[1].eta); 
       subleadingMuonVector_phi.push_back(muonInformation[1].phi);
       subleadingMuonVector_m.push_back(muonInformation[1].m);
       subleadingMuonVector_corrIso.push_back(muonInformation[1].correctedIso);
       subsubleadingMuonVector_pt.push_back(muonInformation[2].pt); 
       subsubleadingMuonVector_eta.push_back(muonInformation[2].eta); 
       subsubleadingMuonVector_phi.push_back(muonInformation[2].phi);
       subsubleadingMuonVector_m.push_back(muonInformation[2].m);
       subsubleadingMuonVector_corrIso.push_back(muonInformation[2].correctedIso);
       Mcounter.push_back(mCounter);


       leadingElectronVector_pt.push_back(electronInformation[0].pt); 
       leadingElectronVector_eta.push_back(electronInformation[0].eta); 
       leadingElectronVector_phi.push_back(electronInformation[0].phi);
       leadingElectronVector_m.push_back(electronInformation[0].m);
       leadingElectronVector_corrIso.push_back(electronInformation[0].correctedIso);
       subleadingElectronVector_pt.push_back(electronInformation[1].pt); 
       subleadingElectronVector_eta.push_back(electronInformation[1].eta); 
       subleadingElectronVector_phi.push_back(electronInformation[1].phi);
       subleadingElectronVector_m.push_back(electronInformation[1].m);
       subleadingElectronVector_corrIso.push_back(electronInformation[1].correctedIso);
       subsubleadingElectronVector_pt.push_back(electronInformation[2].pt); 
       subsubleadingElectronVector_eta.push_back(electronInformation[2].eta); 
       subsubleadingElectronVector_phi.push_back(electronInformation[2].phi);
       subsubleadingElectronVector_m.push_back(electronInformation[2].m);
       subsubleadingElectronVector_corrIso.push_back(electronInformation[2].correctedIso);
       Ecounter.push_back(eleCounter);
       
     }

   //we have all of the information for the taus in this event. We read this out to the 
   //edm format, and we're done.

   std::unique_ptr< edm::ValueMap < int > > Mcounter_valueMap(new edm::ValueMap < int >());
   edm::ValueMap< int >::Filler filler_Mcounter_valueMap(*Mcounter_valueMap);
   filler_Mcounter_valueMap.insert(boostedTauHandle, Mcounter.begin(), Mcounter.end());
   filler_Mcounter_valueMap.fill();   

   std::unique_ptr< edm::ValueMap < int > > Ecounter_valueMap(new edm::ValueMap < int >());
   edm::ValueMap< int >::Filler filler_Ecounter_valueMap(*Ecounter_valueMap);
   filler_Ecounter_valueMap.insert(boostedTauHandle, Ecounter.begin(), Ecounter.end());
   filler_Ecounter_valueMap.fill();

   std::unique_ptr< edm::ValueMap < float > > leadingMuonVector_pt_valueMap(new edm::ValueMap < float >());
   edm::ValueMap< float >::Filler filler_leadingMuonVector_pt_valueMap(*leadingMuonVector_pt_valueMap);
   filler_leadingMuonVector_pt_valueMap.insert(boostedTauHandle, leadingMuonVector_pt.begin(), leadingMuonVector_pt.end());
   filler_leadingMuonVector_pt_valueMap.fill();

   std::unique_ptr< edm::ValueMap < float > > leadingMuonVector_eta_valueMap(new edm::ValueMap < float >());
   edm::ValueMap< float >::Filler filler_leadingMuonVector_eta_valueMap(*leadingMuonVector_eta_valueMap);
   filler_leadingMuonVector_eta_valueMap.insert(boostedTauHandle, leadingMuonVector_eta.begin(), leadingMuonVector_eta.end());
   filler_leadingMuonVector_eta_valueMap.fill();

   std::unique_ptr< edm::ValueMap < float > > leadingMuonVector_phi_valueMap(new edm::ValueMap < float >());
   edm::ValueMap< float >::Filler filler_leadingMuonVector_phi_valueMap(*leadingMuonVector_phi_valueMap);
   filler_leadingMuonVector_phi_valueMap.insert(boostedTauHandle, leadingMuonVector_phi.begin(), leadingMuonVector_phi.end());
   filler_leadingMuonVector_phi_valueMap.fill();

   std::unique_ptr< edm::ValueMap < float > > leadingMuonVector_m_valueMap(new edm::ValueMap < float >());
   edm::ValueMap< float >::Filler filler_leadingMuonVector_m_valueMap(*leadingMuonVector_m_valueMap);
   filler_leadingMuonVector_m_valueMap.insert(boostedTauHandle, leadingMuonVector_m.begin(), leadingMuonVector_m.end());
   filler_leadingMuonVector_m_valueMap.fill();

   std::unique_ptr< edm::ValueMap < float > > leadingMuonVector_corrIso_valueMap(new edm::ValueMap < float >());
   edm::ValueMap< float >::Filler filler_leadingMuonVector_corrIso_valueMap(*leadingMuonVector_corrIso_valueMap);
   filler_leadingMuonVector_corrIso_valueMap.insert(boostedTauHandle, leadingMuonVector_corrIso.begin(), leadingMuonVector_corrIso.end());
   filler_leadingMuonVector_corrIso_valueMap.fill();

   std::unique_ptr< edm::ValueMap < float > > subleadingMuonVector_pt_valueMap(new edm::ValueMap < float >());
   edm::ValueMap< float >::Filler filler_subleadingMuonVector_pt_valueMap(*subleadingMuonVector_pt_valueMap);
   filler_subleadingMuonVector_pt_valueMap.insert(boostedTauHandle, subleadingMuonVector_pt.begin(), subleadingMuonVector_pt.end());
   filler_subleadingMuonVector_pt_valueMap.fill();

   std::unique_ptr< edm::ValueMap < float > > subleadingMuonVector_eta_valueMap(new edm::ValueMap < float >());
   edm::ValueMap< float >::Filler filler_subleadingMuonVector_eta_valueMap(*subleadingMuonVector_eta_valueMap);
   filler_subleadingMuonVector_eta_valueMap.insert(boostedTauHandle, subleadingMuonVector_eta.begin(), subleadingMuonVector_eta.end());
   filler_subleadingMuonVector_eta_valueMap.fill();

   std::unique_ptr< edm::ValueMap < float > > subleadingMuonVector_phi_valueMap(new edm::ValueMap < float >());
   edm::ValueMap< float >::Filler filler_subleadingMuonVector_phi_valueMap(*subleadingMuonVector_phi_valueMap);
   filler_subleadingMuonVector_phi_valueMap.insert(boostedTauHandle, subleadingMuonVector_phi.begin(), subleadingMuonVector_phi.end());
   filler_subleadingMuonVector_phi_valueMap.fill();

   std::unique_ptr< edm::ValueMap < float > > subleadingMuonVector_m_valueMap(new edm::ValueMap < float >());
   edm::ValueMap< float >::Filler filler_subleadingMuonVector_m_valueMap(*subleadingMuonVector_m_valueMap);
   filler_subleadingMuonVector_m_valueMap.insert(boostedTauHandle, subleadingMuonVector_m.begin(), subleadingMuonVector_m.end());
   filler_subleadingMuonVector_m_valueMap.fill();

   std::unique_ptr< edm::ValueMap < float > > subleadingMuonVector_corrIso_valueMap(new edm::ValueMap < float >());
   edm::ValueMap< float >::Filler filler_subleadingMuonVector_corrIso_valueMap(*subleadingMuonVector_corrIso_valueMap);
   filler_subleadingMuonVector_corrIso_valueMap.insert(boostedTauHandle, subleadingMuonVector_corrIso.begin(), subleadingMuonVector_corrIso.end());
   filler_subleadingMuonVector_corrIso_valueMap.fill();

   std::unique_ptr< edm::ValueMap < float > > subsubleadingMuonVector_pt_valueMap(new edm::ValueMap < float >());
   edm::ValueMap< float >::Filler filler_subsubleadingMuonVector_pt_valueMap(*subsubleadingMuonVector_pt_valueMap);
   filler_subsubleadingMuonVector_pt_valueMap.insert(boostedTauHandle, subsubleadingMuonVector_pt.begin(), subsubleadingMuonVector_pt.end());
   filler_subsubleadingMuonVector_pt_valueMap.fill();

   std::unique_ptr< edm::ValueMap < float > > subsubleadingMuonVector_eta_valueMap(new edm::ValueMap < float >());
   edm::ValueMap< float >::Filler filler_subsubleadingMuonVector_eta_valueMap(*subsubleadingMuonVector_eta_valueMap);
   filler_subsubleadingMuonVector_eta_valueMap.insert(boostedTauHandle, subsubleadingMuonVector_eta.begin(), subsubleadingMuonVector_eta.end());
   filler_subsubleadingMuonVector_eta_valueMap.fill();

   std::unique_ptr< edm::ValueMap < float > > subsubleadingMuonVector_phi_valueMap(new edm::ValueMap < float >());
   edm::ValueMap< float >::Filler filler_subsubleadingMuonVector_phi_valueMap(*subsubleadingMuonVector_phi_valueMap);
   filler_subsubleadingMuonVector_phi_valueMap.insert(boostedTauHandle, subsubleadingMuonVector_phi.begin(), subsubleadingMuonVector_phi.end());
   filler_subsubleadingMuonVector_phi_valueMap.fill();

   std::unique_ptr< edm::ValueMap < float > > subsubleadingMuonVector_m_valueMap(new edm::ValueMap < float >());
   edm::ValueMap< float >::Filler filler_subsubleadingMuonVector_m_valueMap(*subsubleadingMuonVector_m_valueMap);
   filler_subsubleadingMuonVector_m_valueMap.insert(boostedTauHandle, subsubleadingMuonVector_m.begin(), subsubleadingMuonVector_m.end());
   filler_subsubleadingMuonVector_m_valueMap.fill();

   std::unique_ptr< edm::ValueMap < float > > subsubleadingMuonVector_corrIso_valueMap(new edm::ValueMap < float >());
   edm::ValueMap< float >::Filler filler_subsubleadingMuonVector_corrIso_valueMap(*subsubleadingMuonVector_corrIso_valueMap);
   filler_subsubleadingMuonVector_corrIso_valueMap.insert(boostedTauHandle, subsubleadingMuonVector_corrIso.begin(), subsubleadingMuonVector_corrIso.end());
   filler_subsubleadingMuonVector_corrIso_valueMap.fill();

   std::unique_ptr< edm::ValueMap < float > > leadingElectronVector_pt_valueMap(new edm::ValueMap < float >());
   edm::ValueMap< float >::Filler filler_leadingElectronVector_pt_valueMap(*leadingElectronVector_pt_valueMap);
   filler_leadingElectronVector_pt_valueMap.insert(boostedTauHandle, leadingElectronVector_pt.begin(), leadingElectronVector_pt.end());
   filler_leadingElectronVector_pt_valueMap.fill();

   std::unique_ptr< edm::ValueMap < float > > leadingElectronVector_eta_valueMap(new edm::ValueMap < float >());
   edm::ValueMap< float >::Filler filler_leadingElectronVector_eta_valueMap(*leadingElectronVector_eta_valueMap);
   filler_leadingElectronVector_eta_valueMap.insert(boostedTauHandle, leadingElectronVector_eta.begin(), leadingElectronVector_eta.end());
   filler_leadingElectronVector_eta_valueMap.fill();

   std::unique_ptr< edm::ValueMap < float > > leadingElectronVector_phi_valueMap(new edm::ValueMap < float >());
   edm::ValueMap< float >::Filler filler_leadingElectronVector_phi_valueMap(*leadingElectronVector_phi_valueMap);
   filler_leadingElectronVector_phi_valueMap.insert(boostedTauHandle, leadingElectronVector_phi.begin(), leadingElectronVector_phi.end());
   filler_leadingElectronVector_phi_valueMap.fill();

   std::unique_ptr< edm::ValueMap < float > > leadingElectronVector_m_valueMap(new edm::ValueMap < float >());
   edm::ValueMap< float >::Filler filler_leadingElectronVector_m_valueMap(*leadingElectronVector_m_valueMap);
   filler_leadingElectronVector_m_valueMap.insert(boostedTauHandle, leadingElectronVector_m.begin(), leadingElectronVector_m.end());
   filler_leadingElectronVector_m_valueMap.fill();

   std::unique_ptr< edm::ValueMap < float > > leadingElectronVector_corrIso_valueMap(new edm::ValueMap < float >());
   edm::ValueMap< float >::Filler filler_leadingElectronVector_corrIso_valueMap(*leadingElectronVector_corrIso_valueMap);
   filler_leadingElectronVector_corrIso_valueMap.insert(boostedTauHandle, leadingElectronVector_corrIso.begin(), leadingElectronVector_corrIso.end());
   filler_leadingElectronVector_corrIso_valueMap.fill();

   std::unique_ptr< edm::ValueMap < float > > subleadingElectronVector_pt_valueMap(new edm::ValueMap < float >());
   edm::ValueMap< float >::Filler filler_subleadingElectronVector_pt_valueMap(*subleadingElectronVector_pt_valueMap);
   filler_subleadingElectronVector_pt_valueMap.insert(boostedTauHandle, subleadingElectronVector_pt.begin(), subleadingElectronVector_pt.end());
   filler_subleadingElectronVector_pt_valueMap.fill();

   std::unique_ptr< edm::ValueMap < float > > subleadingElectronVector_eta_valueMap(new edm::ValueMap < float >());
   edm::ValueMap< float >::Filler filler_subleadingElectronVector_eta_valueMap(*subleadingElectronVector_eta_valueMap);
   filler_subleadingElectronVector_eta_valueMap.insert(boostedTauHandle, subleadingElectronVector_eta.begin(), subleadingElectronVector_eta.end());
   filler_subleadingElectronVector_eta_valueMap.fill();

   std::unique_ptr< edm::ValueMap < float > > subleadingElectronVector_phi_valueMap(new edm::ValueMap < float >());
   edm::ValueMap< float >::Filler filler_subleadingElectronVector_phi_valueMap(*subleadingElectronVector_phi_valueMap);
   filler_subleadingElectronVector_phi_valueMap.insert(boostedTauHandle, subleadingElectronVector_phi.begin(), subleadingElectronVector_phi.end());
   filler_subleadingElectronVector_phi_valueMap.fill();

   std::unique_ptr< edm::ValueMap < float > > subleadingElectronVector_m_valueMap(new edm::ValueMap < float >());
   edm::ValueMap< float >::Filler filler_subleadingElectronVector_m_valueMap(*subleadingElectronVector_m_valueMap);
   filler_subleadingElectronVector_m_valueMap.insert(boostedTauHandle, subleadingElectronVector_m.begin(), subleadingElectronVector_m.end());
   filler_subleadingElectronVector_m_valueMap.fill();

   std::unique_ptr< edm::ValueMap < float > > subleadingElectronVector_corrIso_valueMap(new edm::ValueMap < float >());
   edm::ValueMap< float >::Filler filler_subleadingElectronVector_corrIso_valueMap(*subleadingElectronVector_corrIso_valueMap);
   filler_subleadingElectronVector_corrIso_valueMap.insert(boostedTauHandle, subleadingElectronVector_corrIso.begin(), subleadingElectronVector_corrIso.end());
   filler_subleadingElectronVector_corrIso_valueMap.fill();

   std::unique_ptr< edm::ValueMap < float > > subsubleadingElectronVector_pt_valueMap(new edm::ValueMap < float >());
   edm::ValueMap< float >::Filler filler_subsubleadingElectronVector_pt_valueMap(*subsubleadingElectronVector_pt_valueMap);
   filler_subsubleadingElectronVector_pt_valueMap.insert(boostedTauHandle, subsubleadingElectronVector_pt.begin(), subsubleadingElectronVector_pt.end());
   filler_subsubleadingElectronVector_pt_valueMap.fill();

   std::unique_ptr< edm::ValueMap < float > > subsubleadingElectronVector_eta_valueMap(new edm::ValueMap < float >());
   edm::ValueMap< float >::Filler filler_subsubleadingElectronVector_eta_valueMap(*subsubleadingElectronVector_eta_valueMap);
   filler_subsubleadingElectronVector_eta_valueMap.insert(boostedTauHandle, subsubleadingElectronVector_eta.begin(), subsubleadingElectronVector_eta.end());
   filler_subsubleadingElectronVector_eta_valueMap.fill();

   std::unique_ptr< edm::ValueMap < float > > subsubleadingElectronVector_phi_valueMap(new edm::ValueMap < float >());
   edm::ValueMap< float >::Filler filler_subsubleadingElectronVector_phi_valueMap(*subsubleadingElectronVector_phi_valueMap);
   filler_subsubleadingElectronVector_phi_valueMap.insert(boostedTauHandle, subsubleadingElectronVector_phi.begin(), subsubleadingElectronVector_phi.end());
   filler_subsubleadingElectronVector_phi_valueMap.fill();

   std::unique_ptr< edm::ValueMap < float > > subsubleadingElectronVector_m_valueMap(new edm::ValueMap < float >());
   edm::ValueMap< float >::Filler filler_subsubleadingElectronVector_m_valueMap(*subsubleadingElectronVector_m_valueMap);
   filler_subsubleadingElectronVector_m_valueMap.insert(boostedTauHandle, subsubleadingElectronVector_m.begin(), subsubleadingElectronVector_m.end());
   filler_subsubleadingElectronVector_m_valueMap.fill();

   std::unique_ptr< edm::ValueMap < float > > subsubleadingElectronVector_corrIso_valueMap(new edm::ValueMap < float >());
   edm::ValueMap< float >::Filler filler_subsubleadingElectronVector_corrIso_valueMap(*subsubleadingElectronVector_corrIso_valueMap);
   filler_subsubleadingElectronVector_corrIso_valueMap.insert(boostedTauHandle, subsubleadingElectronVector_corrIso.begin(), subsubleadingElectronVector_corrIso.end());
   filler_subsubleadingElectronVector_corrIso_valueMap.fill();

   iEvent.put(std::move(Mcounter_valueMap), "Mcounter");
   iEvent.put(std::move(Ecounter_valueMap), "Ecounter");

   iEvent.put(std::move(leadingMuonVector_pt_valueMap), "LeadingMuonPt");
   iEvent.put(std::move(leadingMuonVector_eta_valueMap), "LeadingMuonEta");
   iEvent.put(std::move(leadingMuonVector_phi_valueMap), "LeadingMuonPhi");
   iEvent.put(std::move(leadingMuonVector_m_valueMap), "LeadingMuonM");
   iEvent.put(std::move(leadingMuonVector_corrIso_valueMap), "LeadingMuonCorrIso");
   iEvent.put(std::move(subleadingMuonVector_pt_valueMap), "SubLeadingMuonPt");
   iEvent.put(std::move(subleadingMuonVector_eta_valueMap), "SubLeadingMuonEta");
   iEvent.put(std::move(subleadingMuonVector_phi_valueMap), "SubLeadingMuonPhi");
   iEvent.put(std::move(subleadingMuonVector_m_valueMap), "SubLeadingMuonM");
   iEvent.put(std::move(subleadingMuonVector_corrIso_valueMap), "SubLeadingMuonCorrIso");
   iEvent.put(std::move(subsubleadingMuonVector_pt_valueMap), "SubSubLeadingMuonPt");
   iEvent.put(std::move(subsubleadingMuonVector_eta_valueMap), "SubSubLeadingMuonEta");
   iEvent.put(std::move(subsubleadingMuonVector_phi_valueMap), "SubSubLeadingMuonPhi");
   iEvent.put(std::move(subsubleadingMuonVector_m_valueMap), "SubSubLeadingMuonM");
   iEvent.put(std::move(subsubleadingMuonVector_corrIso_valueMap), "SubSubLeadingMuonCorrIso");

   iEvent.put(std::move(leadingElectronVector_pt_valueMap), "LeadingElectronPt");
   iEvent.put(std::move(leadingElectronVector_eta_valueMap), "LeadingElectronEta");
   iEvent.put(std::move(leadingElectronVector_phi_valueMap), "LeadingElectronPhi");
   iEvent.put(std::move(leadingElectronVector_m_valueMap), "LeadingElectronM");
   iEvent.put(std::move(leadingElectronVector_corrIso_valueMap), "LeadingElectronCorrIso");
   iEvent.put(std::move(subleadingElectronVector_pt_valueMap), "SubLeadingElectronPt");
   iEvent.put(std::move(subleadingElectronVector_eta_valueMap), "SubLeadingElectronEta");
   iEvent.put(std::move(subleadingElectronVector_phi_valueMap), "SubLeadingElectronPhi");
   iEvent.put(std::move(subleadingElectronVector_m_valueMap), "SubLeadingElectronM");
   iEvent.put(std::move(subleadingElectronVector_corrIso_valueMap), "SubLeadingElectronCorrIso");
   iEvent.put(std::move(subsubleadingElectronVector_pt_valueMap), "SubSubLeadingElectronPt");
   iEvent.put(std::move(subsubleadingElectronVector_eta_valueMap), "SubSubLeadingElectronEta");
   iEvent.put(std::move(subsubleadingElectronVector_phi_valueMap), "SubSubLeadingElectronPhi");
   iEvent.put(std::move(subsubleadingElectronVector_m_valueMap), "SubSubLeadingElectronM");
   iEvent.put(std::move(subsubleadingElectronVector_corrIso_valueMap), "SubSubLeadingElectronCorrIso");
}

// ------------ method called once each stream before processing any runs, lumis or events  ------------
void
boostedTauLeadingLeptonIso::beginStream(edm::StreamID)
{
}

// ------------ method called once each stream after processing all runs, lumis and events  ------------
void
boostedTauLeadingLeptonIso::endStream() {
}

// ------------ method called when starting to processes a run  ------------
/*
void
boostedTauLeadingLeptonIso::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when ending the processing of a run  ------------
/*
void
boostedTauLeadingLeptonIso::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when starting to processes a luminosity block  ------------
/*
void
boostedTauLeadingLeptonIso::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when ending the processing of a luminosity block  ------------
/*
void
boostedTauLeadingLeptonIso::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
boostedTauLeadingLeptonIso::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

void boostedTauLeadingLeptonIso::calculateCorrectedMuonIsoInformation(pat::Tau theTau, leptonInfo &theMuonInfo)
{
  double tauSumChargedHadronPt = 0.0;
  double tauSumNeutralHadronEt = 0.0;
  double tauSumPhotonEt        = 0.0;
  //Okay, let's go through and check for any charged hadrons of the tau in the cone of the muon
  //If we find any, we add it to the sum and correct the isolation for it
  
  for(size_t hadrCandInd = 0;
      hadrCandInd < theTau.signalChargedHadrCands().size();
      ++hadrCandInd)
    {
      double dRConst = reco::deltaR(theMuonInfo.eta, theMuonInfo.phi, theTau.signalChargedHadrCands()[hadrCandInd]->eta(), theTau.signalChargedHadrCands()[hadrCandInd]->phi());
      if (dRConst < 0.3) tauSumChargedHadronPt += theTau.signalChargedHadrCands()[hadrCandInd]->pt();
    }

  for(size_t neutCandInd = 0;
      neutCandInd < theTau.signalNeutrHadrCands().size();
      ++neutCandInd)
    {
      double dRConst = reco::deltaR(theMuonInfo.eta, theMuonInfo.phi, theTau.signalNeutrHadrCands()[neutCandInd]->eta(), theTau.signalNeutrHadrCands()[neutCandInd]->phi());
      if (dRConst < 0.3) tauSumNeutralHadronEt += theTau.signalNeutrHadrCands()[neutCandInd]->pt();
    }

  for(size_t photonCandInd = 0;
      photonCandInd < theTau.signalGammaCands().size();
      ++photonCandInd)
    {
      double dRConst = reco::deltaR(theMuonInfo.eta, theMuonInfo.phi, theTau.signalGammaCands()[photonCandInd]->eta(), theTau.signalGammaCands()[photonCandInd]->phi());
      if (dRConst < 0.3) tauSumPhotonEt += theTau.signalGammaCands()[photonCandInd]->pt();
    }
  
  theMuonInfo.correctedSumPFChargedHadronPt = std::max(0.0, theMuonInfo.sumPFChargedHadronPt - tauSumChargedHadronPt);
  theMuonInfo.correctedSumPFNeutralHadronPt = std::max(0.0, theMuonInfo.sumPFNeutralHadronPt - tauSumNeutralHadronEt + theMuonInfo.sumPFPhotonPt - tauSumPhotonEt);

  theMuonInfo.correctedIso = theMuonInfo.correctedSumPFChargedHadronPt + std::max(theMuonInfo.correctedSumPFNeutralHadronPt - theMuonInfo.sumPUPt/2.0, 0.0);
}

void boostedTauLeadingLeptonIso::calculateCorrectedElectronIsoInformation(pat::Tau theTau, leptonInfo &theElectronInfo)
{
  double tauSumChargedHadronPt = 0.0;
  double tauSumNeutralHadronEt = 0.0;
  double tauSumPhotonEt        = 0.0;
  //Okay, let's go through and check for any charged hadrons of the tau in the cone of the muon
  //If we find any, we add it to the sum and correct the isolation for it
  
  for(size_t hadrCandInd = 0;
      hadrCandInd < theTau.signalChargedHadrCands().size();
      ++hadrCandInd)
    {
      double dRConst = reco::deltaR(theElectronInfo.eta, theElectronInfo.phi, theTau.signalChargedHadrCands()[hadrCandInd]->eta(), theTau.signalChargedHadrCands()[hadrCandInd]->phi());
      if (dRConst < 0.3) tauSumChargedHadronPt += theTau.signalChargedHadrCands()[hadrCandInd]->pt();
    }

  for(size_t neutCandInd = 0;
      neutCandInd < theTau.signalNeutrHadrCands().size();
      ++neutCandInd)
    {
      double dRConst = reco::deltaR(theElectronInfo.eta, theElectronInfo.phi, theTau.signalNeutrHadrCands()[neutCandInd]->eta(), theTau.signalNeutrHadrCands()[neutCandInd]->phi());
      if (dRConst < 0.3) tauSumNeutralHadronEt += theTau.signalNeutrHadrCands()[neutCandInd]->pt();
    }

  for(size_t photonCandInd = 0;
      photonCandInd < theTau.signalGammaCands().size();
      ++photonCandInd)
    {
      double dRConst = reco::deltaR(theElectronInfo.eta, theElectronInfo.phi, theTau.signalGammaCands()[photonCandInd]->eta(), theTau.signalGammaCands()[photonCandInd]->phi());
      if (dRConst < 0.3) tauSumPhotonEt += theTau.signalGammaCands()[photonCandInd]->pt();
    }
  
  theElectronInfo.correctedSumPFChargedHadronPt = std::max(0.0, theElectronInfo.sumPFChargedHadronPt - tauSumChargedHadronPt);
  theElectronInfo.correctedSumPFNeutralHadronPt = std::max(0.0, theElectronInfo.sumPFNeutralHadronPt - tauSumNeutralHadronEt + theElectronInfo.sumPFPhotonPt - tauSumPhotonEt);
  
  theElectronInfo.correctedIso = theElectronInfo.correctedSumPFChargedHadronPt +std::max((double)(theElectronInfo.correctedSumPFNeutralHadronPt - theElectronInfo.rho * theElectronInfo.ea), 0.0);
}

//define this as a plug-in
DEFINE_FWK_MODULE(boostedTauLeadingLeptonIso);
