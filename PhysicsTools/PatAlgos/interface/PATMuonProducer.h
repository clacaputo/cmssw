//
// $Id$
//

#ifndef PhysicsTools_PatAlgos_PATMuonProducer_h
#define PhysicsTools_PatAlgos_PATMuonProducer_h

/**
  \class    PATMuonProducer PATMuonProducer.h "PhysicsTools/PatAlgos/interface/PATMuonProducer.h"
  \brief    Produces pat::Muon's

   The PATMuonProducer produces analysis-level pat::Muon's starting from
   a collection of objects of MuonType.

  \author   Steven Lowette, Roger Wolf
  \version  $Id$
*/


#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/InputTag.h"

#include "PhysicsTools/Utilities/interface/PtComparator.h"

#include "DataFormats/PatCandidates/interface/Muon.h"

#include <string>


namespace pat {


  class ObjectResolutionCalc;
  class LeptonLRCalc;


  class PATMuonProducer : public edm::EDProducer {

    public:

      explicit PATMuonProducer(const edm::ParameterSet & iConfig);
      ~PATMuonProducer();

      virtual void produce(edm::Event & iEvent, const edm::EventSetup & iSetup);

    private:

      reco::GenParticleCandidate findTruth(const reco::CandidateCollection & parts, const MuonType & muon);  
      void matchTruth(const reco::CandidateCollection & parts, std::vector<MuonType> & muons);

    private:

      // configurables
      edm::InputTag muonSrc_;
      bool          addGenMatch_;
      edm::InputTag genPartSrc_;
      float         maxDeltaR_;
      float         minRecoOnGenEt_;
      float         maxRecoOnGenEt_;
      bool          addResolutions_;
      bool          useNNReso_;
      std::string   muonResoFile_;
      bool          doTrkIso_;
      bool          doCalIso_;
      edm::InputTag trackIsoSrc_;
      edm::InputTag ecalIsoSrc_;
      edm::InputTag hcalIsoSrc_;
      edm::InputTag hocalIsoSrc_;
      bool          addMuonID_;
      bool          addLRValues_;
      edm::InputTag tracksSrc_;
      std::string   muonLRFile_;
      // tools
      ObjectResolutionCalc * theResoCalc_;
      LeptonLRCalc         * theLeptonLRCalc_;
      GreaterByPt<Muon>      pTComparator_;
      // other
// FIXME!!!!! this should not be persistent, even not a member!!!
      std::vector<std::pair<const reco::Candidate *, MuonType *> > pairGenRecoMuonsVector_;

  };


}

#endif
