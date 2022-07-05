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


struct IsoCompForTausColl{
    //Non PF variables
    double tauSumChargedHadronPt;
    double sumChargedHadronPt;
  };


class electronBoostedTopologyIsoCorrectionTool : public edm::stream::EDProducer<> {
   public:
      explicit electronBoostedTopologyIsoCorrectionTool(const edm::ParameterSet&);
      ~electronBoostedTopologyIsoCorrectionTool();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:

      void produce(edm::Event&, const edm::EventSetup&) override;

    IsoCompForTausColl compIsoCompForTausColl(double rho, int index, edm::Handle<std::vector<pat::Electron>> electronCollection, edm::Handle<std::vector<pat::Tau>> boostedTauCollection);

    edm::EDGetTokenT< std::vector<pat::Electron> > electronCollection;
    edm::EDGetTokenT< std::vector<pat::Tau> > boostedTauCollection;
    edm::EDGetTokenT<double> rhoSrc;
    EffectiveAreas theEffectiveAreas;
    bool verboseDebug;

};


electronBoostedTopologyIsoCorrectionTool::electronBoostedTopologyIsoCorrectionTool(const edm::ParameterSet& iConfig):
  electronCollection(consumes< std::vector<pat::Electron> >(iConfig.getParameter< edm::InputTag >("electronCollection"))),
  boostedTauCollection(consumes< std::vector<pat::Tau> >(iConfig.getParameter< edm::InputTag >("boostedTauCollection"))),
  rhoSrc(consumes<double> (iConfig.getParameter<edm::InputTag> ("rhoSrc"))),
  theEffectiveAreas(iConfig.getParameter< edm::FileInPath>("EAConfigFile").fullPath())
{
    verboseDebug = iConfig.exists("verboseDebug") ? iConfig.getParameter<bool>("verboseDebug"): false;
    produces<edm::ValueMap<float>>("tauSumChargedHadronPt");
    produces<edm::ValueMap<float>>("SumChargedHadronPt");
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


electronBoostedTopologyIsoCorrectionTool::~electronBoostedTopologyIsoCorrectionTool()
{
 
   // do anything here that needs to be done at destruction time
   // (e.g. close files, deallocate resources etc.)

}


void
electronBoostedTopologyIsoCorrectionTool::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   int tau_index = 0; 
   
   std::cout<<"New Event begins"<<std::endl;


   edm::Handle< std::vector<pat::Electron> > electronHandle;
   iEvent.getByToken(electronCollection, electronHandle);

   edm::Handle< std::vector<pat::Tau> > boostedTauHandle;
   iEvent.getByToken(boostedTauCollection, boostedTauHandle); 
   
   edm::Handle<double> rho;
   iEvent.getByToken(rhoSrc, rho);
   
   int nTaus = boostedTauHandle->size();
   if (verboseDebug) std::cout<<"nTaus: "<<nTaus<<std::endl;


   std::vector<double> tauSumChargedHadronPtVec, SumChargedHadronPtVec;
   tauSumChargedHadronPtVec.reserve(nTaus);
   SumChargedHadronPtVec.reserve(nTaus);

   for (std::vector<pat::Tau>::const_iterator theTau = boostedTauHandle->begin();
	theTau != boostedTauHandle->end();
	++theTau)
     {

       IsoCompForTausColl PF = this->compIsoCompForTausColl(*rho, tau_index, electronHandle, boostedTauHandle);
       
       tauSumChargedHadronPtVec.push_back(PF.tauSumChargedHadronPt);
       SumChargedHadronPtVec.push_back(PF.sumChargedHadronPt);
       

       tau_index = tau_index + 1 ;
       if(verboseDebug)
	 {
	   std::cout<<"***************************************"<<std::endl;       
	 }

     }
   
   std::cout<<"Returned after calling the function"<<std::endl;


   std::unique_ptr<edm::ValueMap<float>> tauSumChargedHadronPtV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillertauSumChargedHadronPt(*tauSumChargedHadronPtV);
   fillertauSumChargedHadronPt.insert(boostedTauHandle, tauSumChargedHadronPtVec.begin(), tauSumChargedHadronPtVec.end());
   fillertauSumChargedHadronPt.fill();

   std::cout<<"No error after filler 1"<<std::endl;

   std::unique_ptr<edm::ValueMap<float>> SumChargedHadronPtV(new edm::ValueMap<float>());
   edm::ValueMap<float>::Filler fillerSumChargedHadronPt(*SumChargedHadronPtV);
   fillerSumChargedHadronPt.insert(boostedTauHandle, SumChargedHadronPtVec.begin(), SumChargedHadronPtVec.end());
   fillerSumChargedHadronPt.fill();

   std::cout<<"No error after filler 2"<<std::endl;


   iEvent.put(std::move(tauSumChargedHadronPtV), "tauSumChargedHadronPt");
   iEvent.put(std::move(SumChargedHadronPtV), "SumChargedHadronPt"); 
 
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
electronBoostedTopologyIsoCorrectionTool::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}



IsoCompForTausColl electronBoostedTopologyIsoCorrectionTool::compIsoCompForTausColl( double rho, int index,  edm::Handle<std::vector<pat::Electron>> electronCollection, edm::Handle<std::vector<pat::Tau>> boostedTauCollection)
{
  double tauSumChargedHadronPt = 0.0;
  double SumChargedHadronPt = 0.0;
  double matchedPt = 0.0;
  int eleCounter = 0;
  double dRmin = 0.4;

  double tauSumNeutralHadronEt = 0.0;
  double tauSumPhotonEt        = 0.0;
  
  
  //double tauSumNeutralHadronEt = 0.0;
  //double tauSumPhotonEt        = 0.0;
  //double dRmin = 0.4;
  //pat::Electron& matchedElectron;
  std::cout<<"without Error so Far 1"<<std::endl;

  pat::TauRef tau(boostedTauCollection, index);
  pat::Electron matchedElectron;

  std::cout<<"without Error so Far 2"<<std::endl; 
    

  for(auto& theEle: *electronCollection)
  //for (std::vector<pat::Electron>::const_iterator theEle = electronCollection->begin(); theEle != electronCollection->end(); ++theEle)
    {
      //pat::ElectronRef theEle(electronCollection, eleIndex);
      std::cout<<"without Error so Far 3 "<< tau->eta()<<tau->phi()<<theEle.eta()<<theEle.phi()<<std::endl;
      double deltaR = reco::deltaR(theEle.eta(), theEle.phi(), tau->eta(), tau->phi());
      std::cout<<"without Error so Far 3.5 deltaR = "<<deltaR<<std::endl;
      if (deltaR < dRmin && deltaR > 0.02)
      {
         eleCounter = eleCounter + 1;
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
      SumChargedHadronPt = matchedElectron.pfIsolationVariables().sumChargedHadronPt;
      std::cout<<"without Error so Far 5"<<std::endl;
      for (size_t hadrCandInd = 0;
	   hadrCandInd <tau->signalChargedHadrCands().size();
	   hadrCandInd++)
	{
    std::cout<<"without Error so Far 6"<<std::endl;
	  double dRConst = reco::deltaR(matchedElectron.eta(), matchedElectron.phi(), tau->signalChargedHadrCands()[hadrCandInd]->eta(),tau->signalChargedHadrCands()[hadrCandInd]->phi());
	  if (dRConst < 0.3) tauSumChargedHadronPt += tau->signalChargedHadrCands()[hadrCandInd]->pt();
	}
      for(size_t neutCandInd = 0;
	  neutCandInd < tau->signalNeutrHadrCands().size();
	  ++neutCandInd)
	{
	  double dRConst = reco::deltaR(matchedElectron.eta(), matchedElectron.phi(), tau->signalNeutrHadrCands()[neutCandInd]->eta(), tau->signalNeutrHadrCands()[neutCandInd]->phi());
	  if (dRConst < 0.3) tauSumNeutralHadronEt += tau->signalNeutrHadrCands()[neutCandInd]->pt();
	}
      for(size_t photonCandInd = 0;
	  photonCandInd < tau->signalGammaCands().size();
	  ++photonCandInd)
	{
	  double dRConst = reco::deltaR(matchedElectron.eta(),matchedElectron.phi(), tau->signalGammaCands()[photonCandInd]->eta(), tau->signalGammaCands()[photonCandInd]->phi());
	  if (dRConst < 0.3) tauSumPhotonEt += tau->signalGammaCands()[photonCandInd]->pt();
	}
   }
  
  std::cout<<"Returning values "<<tauSumChargedHadronPt<<matchedElectron.pfIsolationVariables().sumChargedHadronPt<<std::endl;
  return {tauSumChargedHadronPt,SumChargedHadronPt};
}


//define this as a plug-in
DEFINE_FWK_MODULE(electronBoostedTopologyIsoCorrectionTool);