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
#include "DataFormats/MuonReco/interface/MuonSelectors.h"


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


class TauIsoCorrectionTool : public edm::stream::EDProducer<> {
   public:
      explicit TauIsoCorrectionTool(const edm::ParameterSet&);
      ~TauIsoCorrectionTool();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:

      void produce(edm::Event&, const edm::EventSetup&) override;

    Electron_IsoCompForTausColl compElectron_IsoCompForTausColl(double rho, int index, edm::Handle<std::vector<pat::Electron>> electronCollection, edm::Handle<std::vector<pat::Tau>> TauCollection);
    Muon_IsoCompForTausColl compMuon_IsoCompForTausColl (int index, edm::Handle<std::vector<pat::Muon>> muonCollection, edm::Handle<std::vector<pat::Tau>> TauCollection);

    edm::EDGetTokenT< std::vector<pat::Electron> > electronCollection;
    edm::EDGetTokenT< std::vector<pat::Tau> > TauCollection;
    edm::EDGetTokenT< std::vector<pat::Muon> > muonCollection;

    edm::EDGetTokenT<double> rhoSrc;
    EffectiveAreas theEffectiveAreas;
    EffectiveAreas theEffectiveAreas2;

    bool verboseDebug;

};


TauIsoCorrectionTool::TauIsoCorrectionTool(const edm::ParameterSet& iConfig):
  electronCollection(consumes< std::vector<pat::Electron> >(iConfig.getParameter< edm::InputTag >("electronCollection"))),
  TauCollection(consumes< std::vector<pat::Tau> >(iConfig.getParameter< edm::InputTag >("TauCollection"))),
  muonCollection(consumes< std::vector<pat::Muon> >(iConfig.getParameter< edm::InputTag >("muonCollection"))),
  rhoSrc(consumes<double> (iConfig.getParameter<edm::InputTag> ("rhoSrc"))),
  theEffectiveAreas(iConfig.getParameter< edm::FileInPath>("EAConfigFile").fullPath()),
  theEffectiveAreas2(iConfig.getParameter< edm::FileInPath>("EAConfigFile2").fullPath())
{
    verboseDebug = iConfig.exists("verboseDebug") ? iConfig.getParameter<bool>("verboseDebug"): false;

    // Create variables for Electron Isolation
    produces<edm::ValueMap<float>>("ForEtauSumChargedHadronPt");
    produces<edm::ValueMap<float>>("ForEtauSumPhotonEt");
    produces<edm::ValueMap<float>>("ForEtauSumNeutralHadronEt");

    produces<edm::ValueMap<float>>("ESumChargedHadronPt");
    produces<edm::ValueMap<float>>("ESumNeutralHadronEt");
    produces<edm::ValueMap<float>>("ESumPhotonEt");
    produces<edm::ValueMap<float>>("Erho");
    produces<edm::ValueMap<float>>("Eea");

    produces<edm::ValueMap<int>>("Ecounter");
    produces<edm::ValueMap<float>>("EmatchedPt");
    produces<edm::ValueMap<float>>("EmatchedEta");
    produces<edm::ValueMap<float>>("EmatchedPhi");
    produces<edm::ValueMap<float>>("EmatchedMass");
     
    // Create variables for Muon Isolation


    produces<edm::ValueMap<float>>("ForMtauSumChargedHadronPt");
    produces<edm::ValueMap<float>>("ForMtauSumPhotonEt");
    produces<edm::ValueMap<float>>("ForMtauSumNeutralHadronEt");

    produces<edm::ValueMap<float>>("MSumChargedHadronPt");
    produces<edm::ValueMap<float>>("MSumNeutralHadronEt");
    produces<edm::ValueMap<float>>("MSumPhotonEt");
    produces<edm::ValueMap<float>>("MsumPUPt");
  

    produces<edm::ValueMap<int>>("Mcounter");
    produces<edm::ValueMap<float>>("MmatchedPt");
    produces<edm::ValueMap<float>>("MmatchedEta");
    produces<edm::ValueMap<float>>("MmatchedPhi");
    produces<edm::ValueMap<float>>("MmatchedMass");    

    
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


TauIsoCorrectionTool::~TauIsoCorrectionTool()
{
 
   // do anything here that needs to be done at destruction time
   // (e.g. close files, deallocate resources etc.)

}


void
TauIsoCorrectionTool::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   int tau_index = 0; 

   std::cout<<""<<std::endl;
   std::cout<<""<<std::endl;
   std::cout<<"New Event begins"<<std::endl;
   
   //std::cout<<"New Event begins"<<std::endl;

   edm::Handle< std::vector<pat::Muon> > muonHandle;
   iEvent.getByToken(muonCollection, muonHandle);


   edm::Handle< std::vector<pat::Electron> > electronHandle;
   iEvent.getByToken(electronCollection, electronHandle);

   edm::Handle< std::vector<pat::Tau> > TauHandle;
   iEvent.getByToken(TauCollection, TauHandle); 
   
   edm::Handle<double> rho;
   iEvent.getByToken(rhoSrc, rho);
   
   int nTaus = TauHandle->size();
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

   for (std::vector<pat::Tau>::const_iterator theTau = TauHandle->begin();
	theTau != TauHandle->end();
	++theTau)
     {
       std::cout<<""<<std::endl;
       std::cout<<""<<std::endl;
       std::cout << "Next Tau Begins, index = "<<tau_index<<std::endl;

       Electron_IsoCompForTausColl PF = this->compElectron_IsoCompForTausColl(*rho, tau_index, electronHandle, TauHandle);
       Muon_IsoCompForTausColl PF2 =  this->compMuon_IsoCompForTausColl(tau_index, muonHandle, TauHandle);
       
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
   
   //std::cout<<"Returned after calling the function"<<std::endl;

   //value maps for electrons
   std::unique_ptr<edm::ValueMap<float>> ForE_tau_SumChargedHadronPtV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerForE_tau_SumChargedHadronPt(*ForE_tau_SumChargedHadronPtV);
   fillerForE_tau_SumChargedHadronPt.insert(TauHandle, ForE_tau_SumChargedHadronPtVec.begin(), ForE_tau_SumChargedHadronPtVec.end());
   fillerForE_tau_SumChargedHadronPt.fill();

   std::unique_ptr<edm::ValueMap<float>> ForE_tau_SumPhotonEtV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerForE_tau_SumPhotonEt(*ForE_tau_SumPhotonEtV);
   fillerForE_tau_SumPhotonEt.insert(TauHandle, ForE_tau_SumPhotonEtVec.begin(), ForE_tau_SumPhotonEtVec.end());
   fillerForE_tau_SumPhotonEt.fill();

   std::unique_ptr<edm::ValueMap<float>> ForE_tau_SumNeutralHadronEtV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerForE_tau_SumNeutralHadronEt(*ForE_tau_SumNeutralHadronEtV);
   fillerForE_tau_SumNeutralHadronEt.insert(TauHandle, ForE_tau_SumNeutralHadronEtVec.begin(), ForE_tau_SumNeutralHadronEtVec.end());
   fillerForE_tau_SumNeutralHadronEt.fill();

   std::unique_ptr<edm::ValueMap<float>> E_SumChargedHadronPtV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerE_SumChargedHadronPt(*E_SumChargedHadronPtV);
   fillerE_SumChargedHadronPt.insert(TauHandle, E_SumChargedHadronPtVec.begin(), E_SumChargedHadronPtVec.end());
   fillerE_SumChargedHadronPt.fill();


   std::unique_ptr<edm::ValueMap<float>> E_SumNeutralHadronEtV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerE_SumNeutralHadronEt(*E_SumNeutralHadronEtV);
   fillerE_SumNeutralHadronEt.insert(TauHandle, E_SumNeutralHadronEtVec.begin(), E_SumNeutralHadronEtVec.end());
   fillerE_SumNeutralHadronEt.fill();

   std::unique_ptr<edm::ValueMap<float>> E_SumPhotonEtV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerE_SumPhotonEt(*E_SumPhotonEtV);
   fillerE_SumPhotonEt.insert(TauHandle, E_SumPhotonEtVec.begin(), E_SumPhotonEtVec.end());
   fillerE_SumPhotonEt.fill();

   std::unique_ptr<edm::ValueMap<float>> E_rhoV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerE_rho(*E_rhoV);
   fillerE_rho.insert(TauHandle, E_rhoVec.begin(), E_rhoVec.end());
   fillerE_rho.fill();

   std::unique_ptr<edm::ValueMap<float>> E_eaV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerE_ea(*E_eaV);
   fillerE_ea.insert(TauHandle, E_eaVec.begin(), E_eaVec.end());
   fillerE_ea.fill();

   std::unique_ptr<edm::ValueMap<float>> E_matchedPtV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerE_matchedPt(*E_matchedPtV);
   fillerE_matchedPt.insert(TauHandle, E_matchedPtVec.begin(), E_matchedPtVec.end());
   fillerE_matchedPt.fill();

   std::unique_ptr<edm::ValueMap<float>> E_matchedEtaV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerE_matchedEta(*E_matchedEtaV);
   fillerE_matchedEta.insert(TauHandle, E_matchedEtaVec.begin(), E_matchedEtaVec.end());
   fillerE_matchedEta.fill();

   std::unique_ptr<edm::ValueMap<float>> E_matchedPhiV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerE_matchedPhi(*E_matchedPhiV);
   fillerE_matchedPhi.insert(TauHandle, E_matchedPhiVec.begin(), E_matchedPhiVec.end());
   fillerE_matchedPhi.fill();

   std::unique_ptr<edm::ValueMap<float>> E_matchedMassV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerE_matchedMass(*E_matchedMassV);
   fillerE_matchedMass.insert(TauHandle, E_matchedMassVec.begin(), E_matchedMassVec.end());
   fillerE_matchedMass.fill();

   std::unique_ptr<edm::ValueMap<int>> E_counterV(new edm::ValueMap<int>());
   edm::ValueMap<int>::Filler fillerE_counter(*E_counterV);
   fillerE_counter.insert(TauHandle, E_counterVec.begin(), E_counterVec.end());
   fillerE_counter.fill();


  //value maps for muons
   std::unique_ptr<edm::ValueMap<float>> ForM_tau_SumChargedHadronPtV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerForM_tau_SumChargedHadronPt(*ForM_tau_SumChargedHadronPtV);
   fillerForM_tau_SumChargedHadronPt.insert(TauHandle, ForM_tau_SumChargedHadronPtVec.begin(), ForM_tau_SumChargedHadronPtVec.end());
   fillerForM_tau_SumChargedHadronPt.fill();

   std::unique_ptr<edm::ValueMap<float>> ForM_tau_SumPhotonEtV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerForM_tau_SumPhotonEt(*ForM_tau_SumPhotonEtV);
   fillerForM_tau_SumPhotonEt.insert(TauHandle, ForM_tau_SumPhotonEtVec.begin(), ForM_tau_SumPhotonEtVec.end());
   fillerForM_tau_SumPhotonEt.fill();
   

   std::unique_ptr<edm::ValueMap<float>> ForM_tau_SumNeutralHadronEtV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerForM_tau_SumNeutralHadronEt(*ForM_tau_SumNeutralHadronEtV);
   fillerForM_tau_SumNeutralHadronEt.insert(TauHandle, ForM_tau_SumNeutralHadronEtVec.begin(), ForM_tau_SumNeutralHadronEtVec.end());
   fillerForM_tau_SumNeutralHadronEt.fill();

   std::unique_ptr<edm::ValueMap<float>> M_SumChargedHadronPtV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerM_SumChargedHadronPt(*M_SumChargedHadronPtV);
   fillerM_SumChargedHadronPt.insert(TauHandle, M_SumChargedHadronPtVec.begin(), M_SumChargedHadronPtVec.end());
   fillerM_SumChargedHadronPt.fill();

   std::unique_ptr<edm::ValueMap<float>> M_SumNeutralHadronEtV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerM_SumNeutralHadronEt(*M_SumNeutralHadronEtV);
   fillerM_SumNeutralHadronEt.insert(TauHandle, M_SumNeutralHadronEtVec.begin(), M_SumNeutralHadronEtVec.end());
   fillerM_SumNeutralHadronEt.fill();

   std::unique_ptr<edm::ValueMap<float>> M_SumPhotonEtV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerM_SumPhotonEt(*M_SumPhotonEtV);
   fillerM_SumPhotonEt.insert(TauHandle, M_SumPhotonEtVec.begin(), M_SumPhotonEtVec.end());
   fillerM_SumPhotonEt.fill();

   std::unique_ptr<edm::ValueMap<float>> M_sumPUPtV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerM_sumPUPt(*M_sumPUPtV);
   fillerM_sumPUPt.insert(TauHandle, M_sumPUPtVec.begin(), M_sumPUPtVec.end());
   fillerM_sumPUPt.fill();

   std::unique_ptr<edm::ValueMap<float>> M_matchedPtV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerM_matchedPt(*M_matchedPtV);
   fillerM_matchedPt.insert(TauHandle, M_matchedPtVec.begin(), M_matchedPtVec.end());
   fillerM_matchedPt.fill();

   std::unique_ptr<edm::ValueMap<float>> M_matchedEtaV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerM_matchedEta(*M_matchedEtaV);
   fillerM_matchedEta.insert(TauHandle, M_matchedEtaVec.begin(), M_matchedEtaVec.end());
   fillerM_matchedEta.fill();

   std::unique_ptr<edm::ValueMap<float>> M_matchedPhiV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerM_matchedPhi(*M_matchedPhiV);
   fillerM_matchedPhi.insert(TauHandle, M_matchedPhiVec.begin(), M_matchedPhiVec.end());
   fillerM_matchedPhi.fill();

   std::unique_ptr<edm::ValueMap<float>> M_matchedMassV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerM_matchedMass(*M_matchedMassV);
   fillerM_matchedMass.insert(TauHandle, M_matchedMassVec.begin(), M_matchedMassVec.end());
   fillerM_matchedMass.fill();

   std::unique_ptr<edm::ValueMap<int>> M_counterV(new edm::ValueMap<int>());
   edm::ValueMap<int>::Filler fillerM_counter(*M_counterV);
   fillerM_counter.insert(TauHandle, M_counterVec.begin(), M_counterVec.end());
   fillerM_counter.fill();




   /////////////////////////////////////////////////////////////////////////////////////////////////

   //std::cout<<"No error after filler 2"<<std::endl;


   //Register for Electrons
   iEvent.put(std::move(ForE_tau_SumChargedHadronPtV), "ForEtauSumChargedHadronPt");
   iEvent.put(std::move(ForE_tau_SumPhotonEtV), "ForEtauSumPhotonEt");
   iEvent.put(std::move(ForE_tau_SumNeutralHadronEtV), "ForEtauSumNeutralHadronEt");
   iEvent.put(std::move(E_SumChargedHadronPtV), "ESumChargedHadronPt");
   iEvent.put(std::move(E_SumNeutralHadronEtV), "ESumNeutralHadronEt");
   iEvent.put(std::move(E_SumPhotonEtV), "ESumPhotonEt");
   iEvent.put(std::move(E_rhoV), "Erho");
   iEvent.put(std::move(E_eaV), "Eea"); 
   iEvent.put(std::move(E_counterV), "Ecounter");
   iEvent.put(std::move(E_matchedPtV), "EmatchedPt");
   iEvent.put(std::move(E_matchedEtaV), "EmatchedEta");
   iEvent.put(std::move(E_matchedPhiV), "EmatchedPhi");
   iEvent.put(std::move(E_matchedMassV), "EmatchedMass");
   
   //Register for Muons
   iEvent.put(std::move(ForM_tau_SumChargedHadronPtV), "ForMtauSumChargedHadronPt");
   iEvent.put(std::move(ForM_tau_SumPhotonEtV), "ForMtauSumPhotonEt");
   iEvent.put(std::move(ForM_tau_SumNeutralHadronEtV), "ForMtauSumNeutralHadronEt");
   iEvent.put(std::move(M_SumChargedHadronPtV), "MSumChargedHadronPt");
   iEvent.put(std::move(M_SumNeutralHadronEtV), "MSumNeutralHadronEt");
   iEvent.put(std::move(M_SumPhotonEtV), "MSumPhotonEt");
   iEvent.put(std::move(M_sumPUPtV), "MsumPUPt");
   iEvent.put(std::move(M_counterV), "Mcounter");
   iEvent.put(std::move(M_matchedPtV), "MmatchedPt");
   iEvent.put(std::move(M_matchedEtaV), "MmatchedEta");
   iEvent.put(std::move(M_matchedPhiV), "MmatchedPhi");
   iEvent.put(std::move(M_matchedMassV), "MmatchedMass");


}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
TauIsoCorrectionTool::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

 

Electron_IsoCompForTausColl TauIsoCorrectionTool::compElectron_IsoCompForTausColl( double rho, int index,  edm::Handle<std::vector<pat::Electron>> electronCollection, edm::Handle<std::vector<pat::Tau>> TauCollection)
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
  double dRmin = 0.4;
  //pat::Electron& matchedElectron;
  //std::cout<<"without Error so Far 1"<<std::endl;

  pat::TauRef tau(TauCollection, index);
  pat::Electron matchedElectron;
  std::vector<pat::Electron>::const_iterator matIteEle;

  //std::cout<<"without Error so Far 2"<<std::endl; 
    

  //for(auto& theEle: *electronCollection)
  for (std::vector<pat::Electron>::const_iterator IteEle = electronCollection->begin(); IteEle != electronCollection->end(); ++IteEle)
    {

      pat::Electron theEle = *IteEle;
      
      //pat::ElectronRef theEle(electronCollection, eleIndex);
      //std::cout<<"without Error so Far 3 "<< tau->eta()<<tau->phi()<<theEle.eta()<<theEle.phi()<<std::endl;
      double deltaR = reco::deltaR(theEle.eta(), theEle.phi(), tau->eta(), tau->phi());
      //std::cout<<"without Error so Far 3.5 deltaR = "<<deltaR<<std::endl;
      //std::cout<<"Before Matching Pt = "<<theEle.pt()<<" mass = "<<theEle.mass()<<std::endl;
      //std::cout<<"ID = "<<theEle.electronID("cutBasedElectronID-Fall17-94X-V2-loose")<<" if it is 0 go to next Ele"<<std::endl;
      std::cout<<"Electron Pt = "<<theEle.pt()<<" mass = "<<theEle.mass()<<" deltaR ="<<deltaR<<" ID = "<<theEle.electronID("cutBasedElectronID-Fall17-94X-V2-loose")<<std::endl;
      if (deltaR < dRmin && deltaR > 0.02 && theEle.electronID("cutBasedElectronID-Fall17-94X-V2-loose"))
      //if (deltaR < dRmin && deltaR > 0.02 && theEle.electronID("cutBasedElectronID-Fall17-94X-V2-loose"))
      {
         E_counter = E_counter + 1;
         std::cout<<"Matched Pt = "<<theEle.pt()<<" mass = "<<theEle.mass()<<" deltaR ="<<deltaR<<" ID = "<<theEle.electronID("cutBasedElectronID-Fall17-94X-V2-loose")<<" sumChargedHadronPt= "<<theEle.pfIsolationVariables().sumChargedHadronPt<<" SumNeutralHadronEt= "<<theEle.pfIsolationVariables().sumNeutralHadronEt<<" sumPhotonEt= "<<theEle.pfIsolationVariables().sumPhotonEt<<std::endl;
         //std::cout<<"After Matching Pt = "<<theEle.pt()<<" mass = "<<theEle.mass()<<" ELectron Counter = "<< E_counter<<std::endl;
         if (theEle.pt() > E_matchedPt)
         {
            std::cout<<"Pt greater than 0 or grater than previous match"<<std::endl;
           // std::cout<<"Pt is greater than previous match = "<<theEle.pt()<<" mass = "<<theEle.mass()<<std::endl;
            //std::cout<<"without Error so Far 4"<<std::endl;
            matchedElectron = theEle;
            matIteEle = IteEle;
            E_matchedPt = theEle.pt();
         }

      }

    }

  if(E_matchedPt > 0.0)
   {

      //std::cout<<"without Error so Far 5"<<std::endl;
      for (size_t hadrCandInd = 0;
	   hadrCandInd <tau->signalChargedHadrCands().size();
	   hadrCandInd++)
	{
    //std::cout<<"without Error so Far 6"<<std::endl;
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
   E_rho = rho;
   //std::cout <<"no error 7 "<<std::endl;//"<<matchedElectron.mass() <<" EA = "<<theEffectiveAreas.getEffectiveArea(fabs(matchedElectron.superCluster()->eta()));
   E_ea = theEffectiveAreas.getEffectiveArea(fabs(matIteEle->superCluster()->eta()));
   //std::cout <<"no error 7.5 and the effective area is "<<E_ea<<std::endl;
   E_matchedPt = matchedElectron.pt();
   E_matchedEta = matchedElectron.eta();
   E_matchedPhi = matchedElectron.phi();
   //std::cout <<"no error 8 "<<std::endl;
   E_matchedMass = matchedElectron.mass();
   //std::cout <<"no error 9, mass = "<<matchedElectron.mass()<<"Phi = "<<matchedElectron.phi()<<std::endl;
   }
   //std::cout<<"ForE_tau_SumChargedHadronPt = "<<ForE_tau_SumChargedHadronPt<<" ForE_tau_SumPhotonEt = "<<ForE_tau_SumPhotonEt<<" ForE_tau_SumNeutralHadronEt = "<<ForE_tau_SumNeutralHadronEt<<" E_SumChargedHadronPt = "<<E_SumChargedHadronPt<<" E_SumNeutralHadronEt = "<<E_SumNeutralHadronEt<<" E_SumPhotonEt = "<<E_SumPhotonEt<<std::endl;
   std::cout<<"Electron ForE_tau_SumChargedHadronPt = "<<ForE_tau_SumChargedHadronPt<<" ForE_tau_SumPhotonEt = "<<ForE_tau_SumPhotonEt<<" ForE_tau_SumNeutralHadronEt = "<<ForE_tau_SumNeutralHadronEt<<" E_SumChargedHadronPt = "<<E_SumChargedHadronPt<<" E_SumNeutralHadronEt = "<<E_SumNeutralHadronEt<<" E_SumPhotonEt = "<<E_SumPhotonEt<<std::endl;



  
  
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


Muon_IsoCompForTausColl TauIsoCorrectionTool::compMuon_IsoCompForTausColl (int index, edm::Handle<std::vector<pat::Muon>> muonCollection, edm::Handle<std::vector<pat::Tau>> TauCollection)
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

    double dRmin = 0.4;

    pat::TauRef tau(TauCollection, index);
    pat::Muon matchedMuon;

    for(auto& theMuo: *muonCollection)
    {
      double deltaR = reco::deltaR(theMuo.eta(), theMuo.phi(), tau->eta(), tau->phi());
      std::cout<<"Muon Pt = "<<theMuo.pt()<<" mass = "<<theMuo.mass()<<" deltaR ="<<deltaR<<" ID = "<<theMuo.passed(reco::Muon::CutBasedIdLoose)<<std::endl;
      if (deltaR < dRmin && deltaR > 0.02 && theMuo.passed(reco::Muon::CutBasedIdLoose))
      {
        M_counter = M_counter + 1;
        std::cout<<"Matched Pt = "<<theMuo.pt()<<" mass = "<<theMuo.mass()<<" deltaR ="<<deltaR<<" ID = "<<theMuo.passed(reco::Muon::CutBasedIdLoose)<<" sumChargedHadronPt= "<<theMuo.pfIsolationR03().sumChargedHadronPt<<" SumNeutralHadronEt= "<<theMuo.pfIsolationR03().sumNeutralHadronEt<<" sumPhotonEt= "<<theMuo.pfIsolationR03().sumPhotonEt<<" M_sumPUPt= "<<theMuo.pfIsolationR03().sumPUPt<<std::endl;
        if (theMuo.pt() > M_matchedPt)
        {
          std::cout<<"Pt greater than 0 or grater than previous match"<<std::endl;
          matchedMuon = theMuo;
          M_matchedPt = theMuo.pt();
        }

      }
      
    }

    if(M_matchedPt > 0.0)
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
    
    std::cout<<"Muon ForM_tau_SumChargedHadronPt = "<<ForM_tau_SumChargedHadronPt<<" ForM_tau_SumPhotonEt = "<<ForM_tau_SumPhotonEt<<" ForM_tau_SumNeutralHadronEt = "<<ForM_tau_SumPhotonEt<<" M_SumChargedHadronPt = "<<M_SumChargedHadronPt<<" M_SumNeutralHadronEt = "<<M_SumNeutralHadronEt<<" M_SumPhotonEt = "<<M_SumPhotonEt<<" M_sumPUpt= "<<M_sumPUPt<<std::endl;
    return {ForM_tau_SumChargedHadronPt, 
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
            };






}



//define this as a plug-in
DEFINE_FWK_MODULE(TauIsoCorrectionTool);