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


struct Electron_IsoCompForTausColl{
    double ForE_tau_SumChargedHadronPt;
    double ForE_tau_SumPhotonEt;
    double ForE_tau_SumNeutralHadronEt;

    double E_SumChargedHadronPt;
    double E_SumNeutralHadronEt;
    double E_SumPhotonEt;
    double E_rho;
    double E_ea;

    int E_counter;
    double E_matchedPt;
    double E_matchedEta;
    double E_matchedPhi;
    double E_matchedMass;
  };

  struct Muon_IsoCompForTausColl{
    double ForM_tau_SumChargedHadronPt;
    double ForM_tau_SumPhotonEt;
    double ForM_tau_SumNeutralHadronEt;

    double M_SumChargedHadronPt;
    double M_SumNeutralHadronEt;
    double M_SumPhotonEt;
    double M_sumPUPt;
    //double M_rho;
    //double M_ea;

    int M_counter;
    double M_matchedPt;
    double M_matchedEta;
    double M_matchedPhi;
    double M_matchedMass;
  };


class BoostedTauIsoCorrectionTool : public edm::stream::EDProducer<> {
   public:
      explicit BoostedTauIsoCorrectionTool(const edm::ParameterSet&);
      ~BoostedTauIsoCorrectionTool();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:

      void produce(edm::Event&, const edm::EventSetup&) override;

    Electron_IsoCompForTausColl compElectron_IsoCompForTausColl(double rho, int index, edm::Handle<std::vector<pat::Electron>> electronCollection, edm::Handle<std::vector<pat::Tau>> boostedTauCollection);
    Muon_IsoCompForTausColl compMuon_IsoCompForTausColl (int index, edm::Handle<std::vector<pat::Muon>> muonCollection, edm::Handle<std::vector<pat::Tau>> boostedTauCollection)

    edm::EDGetTokenT< std::vector<pat::Electron> > electronCollection;
    edm::EDGetTokenT< std::vector<pat::Tau> > boostedTauCollection;
    edm::EDGetTokenT< std::vector<pat::Muon> > muonCollection;

    edm::EDGetTokenT<double> rhoSrc;
    EffectiveAreas theEffectiveAreas;
    EffectiveAreas theEffectiveAreas2;

    bool verboseDebug;

};


BoostedTauIsoCorrectionTool::BoostedTauIsoCorrectionTool(const edm::ParameterSet& iConfig):
  electronCollection(consumes< std::vector<pat::Electron> >(iConfig.getParameter< edm::InputTag >("electronCollection"))),
  boostedTauCollection(consumes< std::vector<pat::Tau> >(iConfig.getParameter< edm::InputTag >("boostedTauCollection"))),
  muonCollection(consumes< std::vector<pat::Muon> >(iConfig.getParameter< edm::InputTag >("muonCollection"))),
  rhoSrc(consumes<double> (iConfig.getParameter<edm::InputTag> ("rhoSrc"))),
  theEffectiveAreas(iConfig.getParameter< edm::FileInPath>("EAConfigFile").fullPath())
  theEffectiveAreas2(iConfig.getParameter< edm::FileInPath>("EAConfigFile2").fullPath())
{
    verboseDebug = iConfig.exists("verboseDebug") ? iConfig.getParameter<bool>("verboseDebug"): false;

    // Create variables for Electron Isolation
    produces<edm::ValueMap<float>>("ForE_tau_SumChargedHadronPt");
    produces<edm::ValueMap<float>>("ForE_tau_SumPhotonEt");
    produces<edm::ValueMap<float>>("ForE_tau_SumNeutralHadronEt");

    produces<edm::ValueMap<float>>("E_SumChargedHadronPt");
    produces<edm::ValueMap<float>>("E_SumNeutralHadronEt");
    produces<edm::ValueMap<float>>("E_SumPhotonEt");
    produces<edm::ValueMap<float>>("E_rho");
    produces<edm::ValueMap<float>>("E_ea");

    produces<edm::ValueMap<int>>("E_counter");
    produces<edm::ValueMap<float>>("E_matchedPt");
    produces<edm::ValueMap<float>>("E_matchedEta");
    produces<edm::ValueMap<float>>("E_matchedPhi");
    produces<edm::ValueMap<float>>("E_matchedMass");
     
    // Create variables for Muon Isolation


    produces<edm::ValueMap<float>>("ForM_tau_SumChargedHadronPt");
    produces<edm::ValueMap<float>>("ForM_tau_SumPhotonEt");
    produces<edm::ValueMap<float>>("ForM_tau_SumNeutralHadronEt");

    produces<edm::ValueMap<float>>("M_SumChargedHadronPt");
    produces<edm::ValueMap<float>>("M_SumNeutralHadronEt");
    produces<edm::ValueMap<float>>("M_SumPhotonEt");
    produces<edm::ValueMap<float>>("M_sumPUPt");
  

    produces<edm::ValueMap<int>>("M_counter");
    produces<edm::ValueMap<float>>("M_matchedPt");
    produces<edm::ValueMap<float>>("M_matchedEta");
    produces<edm::ValueMap<float>>("M_matchedPhi");
    produces<edm::ValueMap<float>>("M_matchedMass");    

    
    //produces<edm::ValueMap<float>>("PfSumChaHad"); 
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


BoostedTauIsoCorrectionTool::~BoostedTauIsoCorrectionTool()
{
 
   // do anything here that needs to be done at destruction time
   // (e.g. close files, deallocate resources etc.)

}


void
BoostedTauIsoCorrectionTool::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   int tau_index = 0; 
   
   std::cout<<"New Event begins"<<std::endl;

   edm::Handle< std::vector<pat::Muon> > muonHandle;
   iEvent.getByToken(muonCollection, muonHandle);


   edm::Handle< std::vector<pat::Electron> > electronHandle;
   iEvent.getByToken(electronCollection, electronHandle);

   edm::Handle< std::vector<pat::Tau> > boostedTauHandle;
   iEvent.getByToken(boostedTauCollection, boostedTauHandle); 
   
   edm::Handle<double> rho;
   iEvent.getByToken(rhoSrc, rho);
   
   int nTaus = boostedTauHandle->size();
   if (verboseDebug) std::cout<<"nTaus: "<<nTaus<<std::endl;

   //define vectors for electons
   std::vector<double> ForE_tau_SumChargedHadronPtVec, ForE_tau_SumPhotonEtVec, ForE_tau_SumNeutralHadronEtVec, E_SumChargedHadronPtVec, E_SumNeutralHadronEtVec, E_SumPhotonEtVec, E_rhoVec, E_eaVec, E_matchedPtVec, E_matchedEtaVec, E_matchedPhiVec, E_matchedMassVec;
   std::vector<int> E_counterVec;

   //define vcetors for muons
   std::vector<double> ForM_tau_SumChargedHadronPtVec, ForM_tau_SumPhotonEtVec, ForM_tau_SumNeutralHadronEtVec, M_SumChargedHadronPtVec, M_SumNeutralHadronEtVec, M_SumPhotonEtVec, M_sumPUPtVec, M_matchedPtVec, M_matchedEtaVec, M_matchedPhiVec, M_matchedMassVec;    
   std::vector<int> M_counterVec;

   //define vector lengths for electrons
   ForE_tau_SumChargedHadronPtVec.reserve(nTaus);
   ForE_tau_SumPhotonEtVec.reserve(nTaus);
   ForE_tau_SumNeutralHadronEtVec.reserve(nTaus);
   E_SumChargedHadronPtVec.reserve(nTaus);
   E_SumNeutralHadronEtVec.reserve(nTaus);
   E_SumPhotonEtVec.reserve(nTaus);
   E_rhoVec.reserve(nTaus);
   E_eaVec.reserve(nTaus);
   E_matchedPtVec.reserve(nTaus);
   E_matchedEtaVec.reserve(nTaus);
   E_matchedPhiVec.reserve(nTaus);
   E_matchedMassVec.reserve(nTaus);
   E_counterVec.reserve(nTaus);

   //define vector lengths for muons

   ForM_tau_SumChargedHadronPtVec.reserve(nTaus);
   ForM_tau_SumPhotonEtVec.reserve(nTaus); 
   ForM_tau_SumNeutralHadronEtVec.reserve(nTaus); 
   M_SumChargedHadronPtVec.reserve(nTaus); 
   M_SumNeutralHadronEtVec.reserve(nTaus);
   M_SumPhotonEtVec.reserve(nTaus); 
   M_sumPUPtVec.reserve(nTaus); 
   M_matchedPtVec.reserve(nTaus); 
   M_matchedEtaVec.reserve(nTaus); 
   M_matchedPhiVec.reserve(nTaus); 
   M_matchedMassVec.reserve(nTaus);
   M_counterVec.reserve(nTaus);

   for (std::vector<pat::Tau>::const_iterator theTau = boostedTauHandle->begin();
	theTau != boostedTauHandle->end();
	++theTau)
     {

       Electron_IsoCompForTausColl PF = this->compElectron_IsoCompForTausColl(*rho, tau_index, electronHandle, boostedTauHandle);
       Muon_IsoCompForTausColl PF2 =  this->compMuon_IsoCompForTausColl(tau_index, muonHandle, boostedTauHandle);
       
       //Create Vectors for Electrons
       ForE_tau_SumChargedHadronPtVec.push_back(PF.ForE_tau_SumChargedHadronPt);
       ForE_tau_SumPhotonEtVec.push_back(PF.ForE_tau_SumPhotonEt);
       ForE_tau_SumNeutralHadronEtVec.push_back(PF.ForE_tau_SumNeutralHadronEt);
       E_SumChargedHadronPtVec.push_back(PF.E_SumChargedHadronPt);
       E_SumNeutralHadronEtVec.push_back(PF.E_SumNeutralHadronEt);
       E_SumPhotonEtVec.push_back(PF.E_SumPhotonEt);
       E_rhoVec.push_back(PF.E_rho);
       E_eaVec.push_back(PF.E_ea);
       E_counterVec.push_back(PF.E_counter);
       E_matchedPtVec.push_back(PF.E_matchedPt);
       E_matchedEtaVec.push_back(PF.E_matchedEta);
       E_matchedPhiVec.push_back(PF.E_matchedPhi);
       E_matchedMassVec.push_back(PF.E_matchedMass);

       //Create Vectors for Muons
       ForM_tau_SumChargedHadronPtVec.push_back(PF2.ForM_tau_SumChargedHadronPt);
       ForM_tau_SumPhotonEtVec.push_back(PF2.ForM_tau_SumPhotonEt);
       ForM_tau_SumNeutralHadronEtVec.push_back(PF2.ForM_tau_SumNeutralHadronEt);
       M_SumChargedHadronPtVec.push_back(PF2.M_SumChargedHadronPt);
       M_SumNeutralHadronEtVec.push_back(PF2.M_SumNeutralHadronEt);
       M_SumPhotonEtVec.push_back(PF2.M_SumPhotonEt);
       M_sumPUPtVec.push_back(PF2.M_sumPUPt);
       M_counterVec.push_back(PF2.M_counter);
       M_matchedPtVec.push_back(PF2.M_matchedPt);
       M_matchedEtaVec.push_back(PF2.M_matchedEta);
       M_matchedPhiVec.push_back(PF2.M_matchedPhi);
       M_matchedMassVec.push_back(PF2.M_matchedMass);
       

       tau_index = tau_index + 1 ;
       if(verboseDebug)
	 {
	   std::cout<<"***************************************"<<std::endl;       
	 }

     }
   
   std::cout<<"Returned after calling the function"<<std::endl;

   //value maps for electrons
   std::unique_ptr<edm::ValueMap<float>> ForE_tau_SumChargedHadronPtV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerForE_tau_SumChargedHadronPt(*ForE_tau_SumChargedHadronPtV);
   fillerForE_tau_SumChargedHadronPt.insert(boostedTauHandle, ForE_tau_SumChargedHadronPtVec.begin(), ForE_tau_SumChargedHadronPtVec.end());
   fillerForE_tau_SumChargedHadronPt.fill();

   std::unique_ptr<edm::ValueMap<float>> ForE_tau_SumPhotonEtV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerForE_tau_SumPhotonEt(*ForE_tau_SumPhotonEtV);
   fillerForE_tau_SumPhotonEt.insert(boostedTauHandle, ForE_tau_SumPhotonEtVec.begin(), ForE_tau_SumPhotonEtVec.end());
   fillerForE_tau_SumPhotonEt.fill();

   std::unique_ptr<edm::ValueMap<float>> ForE_tau_SumNeutralHadronEtV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerForE_tau_SumNeutralHadronEt(*ForE_tau_SumNeutralHadronEt);
   fillerForE_tau_SumNeutralHadronEt.insert(boostedTauHandle, ForE_tau_SumNeutralHadronEtVec.begin(), ForE_tau_SumNeutralHadronEtVec.end());
   fillerForE_tau_SumNeutralHadronEt.fill();

   std::unique_ptr<edm::ValueMap<float>> E_SumChargedHadronPtV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerE_SumChargedHadronPt(*E_SumChargedHadronPtV);
   fillerE_SumChargedHadronPt.insert(boostedTauHandle, E_SumChargedHadronPtVec.begin(), E_SumChargedHadronPtVec.end());
   fillerE_SumChargedHadronPt.fill();


   std::unique_ptr<edm::ValueMap<float>> E_SumNeutralHadronEtV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerE_SumNeutralHadronEt(*E_SumNeutralHadronEtV);
   fillerE_SumNeutralHadronEt.insert(boostedTauHandle, E_SumNeutralHadronEtVec.begin(), E_SumNeutralHadronEtVec.end());
   fillerE_SumNeutralHadronEt.fill();

   std::unique_ptr<edm::ValueMap<float>> E_SumPhotonEtV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerE_SumPhotonEt(*E_SumPhotonEtV);
   fillerE_SumPhotonEt.insert(boostedTauHandle, E_SumPhotonEtVec.begin(), E_SumPhotonEtVec.end());
   fillerE_SumPhotonEt.fill();

   std::unique_ptr<edm::ValueMap<float>> E_rhoV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerE_rho(*E_rhoV);
   fillerE_rho.insert(boostedTauHandle, E_rhoVec.begin(), E_rhoVec.end());
   fillerE_rho.fill();

   std::unique_ptr<edm::ValueMap<float>> E_eaV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerE_ea(*E_eaV);
   fillerE_ea.insert(boostedTauHandle, E_eaVec.begin(), E_eaVec.end());
   fillerE_ea.fill();

   std::unique_ptr<edm::ValueMap<float>> E_matchedPtV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerE_matchedPt(*E_matchedPtV);
   fillerE_matchedPt.insert(boostedTauHandle, E_matchedPtVec.begin(), E_matchedPtVec.end());
   fillerE_matchedPt.fill();

   std::unique_ptr<edm::ValueMap<float>> E_matchedEtaV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerE_matchedEta(*E_matchedEtaV);
   fillerE_matchedEta.insert(boostedTauHandle, E_matchedEtaVec.begin(), E_matchedEtaVec.end());
   fillerE_matchedEta.fill();

   std::unique_ptr<edm::ValueMap<float>> E_matchedPhiV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerE_matchedPhi(*E_matchedPhiV);
   fillerE_matchedPhi.insert(boostedTauHandle, E_matchedPhiVec.begin(), E_matchedPhiVec.end());
   fillerE_matchedPhi.fill();

   std::unique_ptr<edm::ValueMap<float>> E_matchedMassV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerE_matchedMass(*E_matchedMassV);
   fillerE_matchedMass.insert(boostedTauHandle, E_matchedMassVec.begin(), E_matchedMassVec.end());
   fillerE_matchedMass.fill();

   std::unique_ptr<edm::ValueMap<int>> E_counterV(new edm::ValueMap<int>());
   edm::ValueMap<int>::Filler fillerE_counter(*E_counterV);
   fillerE_counter.insert(boostedTauHandle, E_counterVec.begin(), E_counterVec.end());
   fillerE_counter.fill();


  //value maps for muons
   std::unique_ptr<edm::ValueMap<float>> ForM_tau_SumChargedHadronPtV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerForM_tau_SumChargedHadronPt(*ForM_tau_SumChargedHadronPtV);
   fillerForM_tau_SumChargedHadronPt.insert(boostedTauHandle, ForM_tau_SumChargedHadronPtVec.begin(), ForM_tau_SumChargedHadronPtVec.end());
   fillerForM_tau_SumChargedHadronPt.fill();

   std::unique_ptr<edm::ValueMap<float>> ForM_tau_SumPhotonEtV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerForM_tau_SumPhotonEt(*ForM_tau_SumPhotonEtV);
   fillerForM_tau_SumPhotonEt.insert(boostedTauHandle, ForM_tau_SumPhotonEtVec.begin(), ForM_tau_SumPhotonEtVec.end());
   fillerForM_tau_SumPhotonEt.fill();
   

   std::unique_ptr<edm::ValueMap<float>> ForM_tau_SumNeutralHadronEtV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerForM_tau_SumNeutralHadronEt(*ForM_tau_SumNeutralHadronEtV);
   fillerForM_tau_SumNeutralHadronEt.insert(boostedTauHandle, ForM_tau_SumNeutralHadronEtVec.begin(), ForM_tau_SumNeutralHadronEtVec.end());
   fillerForM_tau_SumNeutralHadronEt.fill();

   std::unique_ptr<edm::ValueMap<float>> M_SumChargedHadronPtV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerM_SumChargedHadronPt(*M_SumChargedHadronPtV);
   fillerM_SumChargedHadronPt.insert(boostedTauHandle, M_SumChargedHadronPtVec.begin(), M_SumChargedHadronPtVec.end());
   fillerM_SumChargedHadronPt.fill();

   std::unique_ptr<edm::ValueMap<float>> M_SumNeutralHadronEtV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerM_SumNeutralHadronEt(*M_SumNeutralHadronEtV);
   fillerM_SumNeutralHadronEt.insert(boostedTauHandle, M_SumNeutralHadronEtVec.begin(), M_SumNeutralHadronEtVec.end());
   fillerM_SumNeutralHadronEt.fill();

   std::unique_ptr<edm::ValueMap<float>> M_SumPhotonEtV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerM_SumPhotonEt(*M_SumPhotonEtV);
   fillerM_SumPhotonEt.insert(boostedTauHandle, M_SumPhotonEtVec.begin(), M_SumPhotonEtVec.end());
   fillerM_SumPhotonEt.fill();

   std::unique_ptr<edm::ValueMap<float>> M_sumPUPtV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerM_sumPUPt(*M_sumPUPtV);
   fillerM_sumPUPt.insert(boostedTauHandle, M_sumPUPtVec.begin(), M_sumPUPtVec.end());
   fillerM_sumPUPt.fill();

   std::unique_ptr<edm::ValueMap<float>> M_matchedPtV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerM_matchedPt(*M_matchedPtV);
   fillerM_matchedPt.insert(boostedTauHandle, M_matchedPtVec.begin(), M_matchedPtVec.end());
   fillerM_matchedPt.fill();

   std::unique_ptr<edm::ValueMap<float>> M_matchedEtaV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerM_matchedEta(*M_matchedEtaV);
   fillerM_matchedEta.insert(boostedTauHandle, M_matchedEtaVec.begin(), M_matchedEtaVec.end());
   fillerM_matchedEta.fill();

   std::unique_ptr<edm::ValueMap<float>> M_matchedPhiV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerM_matchedPhi(*M_matchedPhiV);
   fillerM_matchedPhi.insert(boostedTauHandle, M_matchedPhiVec.begin(), M_matchedPhiVec.end());
   fillerM_matchedPhi.fill();

   std::unique_ptr<edm::ValueMap<float>> M_matchedMassV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerM_matchedMass(*M_matchedMassV);
   fillerM_matchedMass.insert(boostedTauHandle, M_matchedMassVec.begin(), M_matchedMassVec.end());
   fillerM_matchedMass.fill();

   std::unique_ptr<edm::ValueMap<int>> M_counterV(new edm::ValueMap<int>());
   edm::ValueMap<int>::Filler fillerM_counter(*M_counterV);
   fillerM_counter.insert(boostedTauHandle, M_counterVec.begin(), M_counterVec.end());
   fillerM_counter.fill();




   /////////////////////////////////////////////////////////////////////////////////////////////////

   std::cout<<"No error after filler 2"<<std::endl;


   //Register for Electrons
   iEvent.put(std::move(ForE_tau_SumChargedHadronPtV), "ForE_tau_SumChargedHadronPt");
   iEvent.put(std::move(ForE_tau_SumPhotonEtV), "ForE_tau_SumPhotonEt");
   iEvent.put(std::move(ForE_tau_SumNeutralHadronEtV), "ForE_tau_SumNeutralHadronEt");
   iEvent.put(std::move(E_SumChargedHadronPtV), "E_SumChargedHadronPt");
   iEvent.put(std::move(E_SumNeutralHadronEtV), "E_SumNeutralHadronEt");
   iEvent.put(std::move(E_SumPhotonEtV), "E_SumPhotonEt");
   iEvent.put(std::move(E_rhoV), "E_rho");
   iEvent.put(std::move(E_eaV), "E_ea"); 
   iEvent.put(std::move(E_counterV), "E_counter");
   iEvent.put(std::move(E_matchedPtV), "E_matchedPt");
   iEvent.put(std::move(E_matchedEtaV), "E_matchedEta");
   iEvent.put(std::move(E_matchedPhiV), "E_matchedPhi");
   iEvent.put(std::move(E_matchedMassV), "E_matchedMass");
   
   //Register for Muons
   iEvent.put(std::move(ForM_tau_SumChargedHadronPtV), "ForM_tau_SumChargedHadronPt");
   iEvent.put(std::move(ForM_tau_SumPhotonEtV), "ForM_tau_SumPhotonEt");
   iEvent.put(std::move(ForM_tau_SumNeutralHadronEtV), "ForM_tau_SumNeutralHadronEt");
   iEvent.put(std::move(M_SumChargedHadronPtV), "M_SumChargedHadronPt");
   iEvent.put(std::move(M_SumNeutralHadronEtV), "M_SumNeutralHadronEt");
   iEvent.put(std::move(M_SumPhotonEtV), "M_SumPhotonEt");
   iEvent.put(std::move(M_sumPUPtV), "M_sumPUPt");
   iEvent.put(std::move(M_counterV), "M_counter");
   iEvent.put(std::move(M_matchedPtV), "M_matchedPt");
   iEvent.put(std::move(M_matchedEtaV), "M_matchedEta");
   iEvent.put(std::move(M_matchedPhiV), "M_matchedPhi");
   iEvent.put(std::move(M_matchedMassV), "M_matchedMass");


}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
BoostedTauIsoCorrectionTool::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

 

Electron_IsoCompForTausColl BoostedTauIsoCorrectionTool::compElectron_IsoCompForTausColl( double rho, int index,  edm::Handle<std::vector<pat::Electron>> electronCollection, edm::Handle<std::vector<pat::Tau>> boostedTauCollection)
{
  double ForE_tau_SumChargedHadronPt = 0.0;
  double ForE_tau_SumPhotonEt = 0.0;
  double ForE_tau_SumNeutralHadronEt = 0.0;

  double E_SumChargedHadronPt = 0.0;
  double E_SumNeutralHadronEt = 0.0;
  double E_SumPhotonEt = 0.0;
  double E_rho = 0.0;
  double E_ea = 0.0;

  int E_counter = 0;
  double E_matchedPt = 0.0;
  double E_matchedEta = 0.0;
  double E_matchedPhi = 0.0;
  double E_matchedMass = 0.0;
  

  
  //double tauSumNeutralHadronEt = 0.0;
  //double tauSumPhotonEt        = 0.0;
  //double dRmin = 0.4;
  //pat::Electron& matchedElectron;
  //std::cout<<"without Error so Far 1"<<std::endl;

  pat::TauRef tau(boostedTauCollection, index);
  pat::Electron matchedElectron;

  //std::cout<<"without Error so Far 2"<<std::endl; 
    

  for(auto& theEle: *electronCollection)
  //for (std::vector<pat::Electron>::const_iterator theEle = electronCollection->begin(); theEle != electronCollection->end(); ++theEle)
    {
      //pat::ElectronRef theEle(electronCollection, eleIndex);
      std::cout<<"without Error so Far 3 "<< tau->eta()<<tau->phi()<<theEle.eta()<<theEle.phi()<<std::endl;
      double deltaR = reco::deltaR(theEle.eta(), theEle.phi(), tau->eta(), tau->phi());
      std::cout<<"without Error so Far 3.5 deltaR = "<<deltaR<<std::endl;
      if (deltaR < dRmin && deltaR > 0.02)
      {
         E_counter = E_counter + 1;
         if (theEle.pt() > matchedPt)
         {
            std::cout<<"without Error so Far 4"<<std::endl;
            pat::Electron matchedElectron = theEle;
            matchedPt = theEle.pt();
         }

      }

    }

  if(matchedPt > 0.0)
   {

      std::cout<<"without Error so Far 5"<<std::endl;
      for (size_t hadrCandInd = 0;
	   hadrCandInd <tau->signalChargedHadrCands().size();
	   hadrCandInd++)
	{
    std::cout<<"without Error so Far 6"<<std::endl;
	  double dRConst = reco::deltaR(matchedElectron.eta(), matchedElectron.phi(), tau->signalChargedHadrCands()[hadrCandInd]->eta(),tau->signalChargedHadrCands()[hadrCandInd]->phi());
	  if (dRConst < 0.3) ForE_tau_SumChargedHadronPt += tau->signalChargedHadrCands()[hadrCandInd]->pt();
	}
      for(size_t neutCandInd = 0;
	  neutCandInd < tau->signalNeutrHadrCands().size();
	  ++neutCandInd)
	{
	  double dRConst = reco::deltaR(matchedElectron.eta(), matchedElectron.phi(), tau->signalNeutrHadrCands()[neutCandInd]->eta(), tau->signalNeutrHadrCands()[neutCandInd]->phi());
	  if (dRConst < 0.3) ForE_tau_SumNeutralHadronEt += tau->signalNeutrHadrCands()[neutCandInd]->pt();
	}
      for(size_t photonCandInd = 0;
	  photonCandInd < tau->signalGammaCands().size();
	  ++photonCandInd)
	{
	  double dRConst = reco::deltaR(matchedElectron.eta(),matchedElectron.phi(), tau->signalGammaCands()[photonCandInd]->eta(), tau->signalGammaCands()[photonCandInd]->phi());
	  if (dRConst < 0.3) ForE_tau_SumPhotonEt += tau->signalGammaCands()[photonCandInd]->pt();
	}

   E_SumChargedHadronPt = matchedElectron.pfIsolationVariables().sumChargedHadronPt;
   E_SumNeutralHadronEt = matchedElectron.pfIsolationVariables().sumNeutralHadronEt;
   E_SumPhotonEt = matchedElectron.pfIsolationVariables().sumPhotonEt;
   E_rho = rho;.
   E_ea = theEffectiveAreas.getEffectiveArea(fabs(matchedElectron.superCluster()->eta()));
   E_matchedPt = matchedElectron.pt();
   E_matchedEta = matchedElectron.eta();
   E_matchedPhi = matchedElectron.phi();
   E_matchedMass = matchedElectron.mass();
   }
   




  
  
  return {ForE_tau_SumChargedHadronPt,
          ForE_tau_SumPhotonEt,
          ForE_tau_SumNeutralHadronEt,
          E_SumChargedHadronPt,
          E_SumNeutralHadronEt,
          E_SumPhotonEt,
          E_rho,
          E_ea,
          E_counter,
          E_matchedPt,
          E_matchedEta,
          E_matchedPhi,
          E_matchedMass};
}


Muon_IsoCompForTausColl BoostedTauIsoCorrectionTool::compMuon_IsoCompForTausColl (int index, edm::Handle<std::vector<pat::Muon>> muonCollection, edm::Handle<std::vector<pat::Tau>> boostedTauCollection)
{
    double ForM_tau_SumChargedHadronPt = 0.0;
    double ForM_tau_SumPhotonEt = 0.0;
    double ForM_tau_SumNeutralHadronEt = 0.0;

    double M_SumChargedHadronPt = 0.0;
    double M_SumNeutralHadronEt = 0.0;
    double M_SumPhotonEt = 0.0;
    double M_sumPUPt = 0.0;
    //double M_rho = 0.0;
    //double M_ea = 0.0;

    int M_counter = 0;
    double M_matchedPt = 0.0;
    double M_matchedEta = 0.0;
    double M_matchedPhi = 0.0;
    double M_matchedMass = 0.0;

    pat::TauRef tau(boostedTauCollection, index);
    pat::Muon matchedMuon;

    for(auto& theMuo: *muonCollection)
    {
      double deltaR = reco::deltaR(theMuo.eta(), theMuo.phi(), tau->eta(), tau->phi());
      if (deltaR < dRmin && deltaR > 0.02)
      {
        M_counter = M_counter + 1;
        if (theMuo.pt() > M_matchedPt)
        {
          pat::Muon matchedMuon = theMuo;
          M_matchedPt = theMuo.pt();
        }

      }
      
    }

    if(matchedPt > 0.0)
    {
      for (size_t hadrCandInd = 0;
	   hadrCandInd <tau->signalChargedHadrCands().size();
	   hadrCandInd++)
	    {
        double dRConst = reco::deltaR(matchedMuon.eta(), matchedMuon.phi(), tau->signalChargedHadrCands()[hadrCandInd]->eta(),tau->signalChargedHadrCands()[hadrCandInd]->phi());
        if (dRConst < 0.3) ForM_tau_SumChargedHadronPt += tau->signalChargedHadrCands()[hadrCandInd]->pt();
      }
      for(size_t neutCandInd = 0;
	  neutCandInd < tau->signalNeutrHadrCands().size();
	  ++neutCandInd)
      {
        double dRConst = reco::deltaR(matchedMuon.eta(), matchedMuon.phi(), tau->signalNeutrHadrCands()[neutCandInd]->eta(), tau->signalNeutrHadrCands()[neutCandInd]->phi());
	      if (dRConst < 0.3) ForM_tau_SumNeutralHadronEt += tau->signalNeutrHadrCands()[neutCandInd]->pt();
      }
      for(size_t photonCandInd = 0;
	  photonCandInd < tau->signalGammaCands().size();
	  ++photonCandInd)
      {
        double dRConst = reco::deltaR(matchedMuon.eta(),matchedMuon.phi(), tau->signalGammaCands()[photonCandInd]->eta(), tau->signalGammaCands()[photonCandInd]->phi());
	      if (dRConst < 0.3) ForM_tau_SumPhotonEt += tau->signalGammaCands()[photonCandInd]->pt();
      }
    M_SumChargedHadronPt = matchedMuon.pfIsolationR03().sumChargedHadronPt;
    M_SumNeutralHadronEt = matchedMuon.pfIsolationR03().sumNeutralHadronEt;
    M_SumPhotonEt = matchedMuon.pfIsolationR03().sumPhotonEt;
    M_sumPUPt = matchedMuon.pfIsolationR03().sumPUPt;
    M_matchedPt = matchedMuon.pt();
    M_matchedEta = matchedMuon.eta();
    M_matchedPhi = matchedMuon.phi();
    M_matchedMass = matchedMuon.mass();  
    }
    

    return {ForE_tau_SumChargedHadronPt, 
            ForM_tau_SumPhotonEt, 
            ForM_tau_SumNeutralHadronEt, 
            M_SumChargedHadronPt, 
            M_SumNeutralHadronEt, 
            M_SumPhotonEt, 
            M_sumPUPt, 
            M_counter,
            M_matchedPt,
            M_matchedEta,
            M_matchedPhi,
            M_matchedMass,
            }






}



//define this as a plug-in
DEFINE_FWK_MODULE(BoostedTauIsoCorrectionTool);