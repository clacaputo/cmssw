// -*- C++ -*-
//
// Package:    MuonGEMRecHits
// Class:      MuonGEMRecHits
// 
/**\class MuonGEMRecHits MuonGEMRecHits.cc 

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Claudio Caputo
//         Created:  Thu, 06 Feb 2014 13:57:03 GMT
// $Id$
//
//


// system include files
#include <memory>
#include <fstream>
#include <sys/time.h>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include <FWCore/Framework/interface/EventSetup.h>
#include "FWCore/Framework/interface/Event.h"
#include <FWCore/Framework/interface/ESHandle.h>
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/MonitorElement.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "Geometry/Records/interface/GlobalTrackingGeometryRecord.h"
#include "Geometry/CommonDetUnit/interface/GlobalTrackingGeometry.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"

#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "DataFormats/TrajectorySeed/interface/TrajectorySeedCollection.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "RecoMuon/TrackingTools/interface/MuonPatternRecoDumper.h"

#include "SimDataFormats/Track/interface/SimTrackContainer.h"

#include <DataFormats/GEMRecHit/interface/GEMRecHit.h>
#include "DataFormats/GEMRecHit/interface/GEMRecHitCollection.h"
#include "SimDataFormats/TrackingHit/interface/PSimHitContainer.h"

#include "Geometry/GEMGeometry/interface/GEMGeometry.h"
#include <Geometry/GEMGeometry/interface/GEMEtaPartition.h>
#include "Geometry/GEMGeometry/interface/GEMEtaPartitionSpecs.h"
#include <Geometry/Records/interface/MuonGeometryRecord.h>
#include <Geometry/CommonDetUnit/interface/GeomDet.h>
#include <Geometry/CommonTopologies/interface/RectangularStripTopology.h>
#include <Geometry/CommonTopologies/interface/TrapezoidalStripTopology.h>

#include "DataFormats/Provenance/interface/Timestamp.h"

#include <DataFormats/MuonDetId/interface/GEMDetId.h>

#include "RecoMuon/DetLayers/interface/MuonDetLayerGeometry.h"
#include "RecoMuon/Records/interface/MuonRecoGeometryRecord.h"

#include "RecoMuon/DetLayers/interface/MuRodBarrelLayer.h"
#include "RecoMuon/DetLayers/interface/MuDetRod.h"
#include "RecoMuon/DetLayers/interface/MuRingForwardDoubleLayer.h"
#include "RecoMuon/DetLayers/interface/MuDetRing.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "DataFormats/MuonDetId/interface/MuonSubdetId.h"

#include "DataFormats/TrackingRecHit/interface/TrackingRecHitFwd.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "TrackingTools/GeomPropagators/interface/Propagator.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"
#include "DataFormats/GeometrySurface/interface/Plane.h"
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h"

//Structures used for filling ME
#include "Validation/MuonGEMRecHits/interface/SimRecStructures.h"
//Tracks Matching
#include "Validation/MuonGEMRecHits/interface/SimTrackMatchManager.h"
#include "Validation/MuonGEMRecHits/interface/DigiMatcher.h"
#include "Validation/MuonGEMRecHits/interface/GEMRecHitMatcher.h"

using namespace std;
using namespace edm;
using namespace matching;



//
// class declaration
//

class MuonGEMRecHits : public edm::EDAnalyzer {
   public:
      explicit MuonGEMRecHits(const edm::ParameterSet&);
      ~MuonGEMRecHits();
    
      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    
   private:
    
    bool isGEMRecHitMatched(MyGEMRecHit gem_recHit_, MyGEMSimHit gem_sh);
    bool isSimTrackGood(const SimTrack &t);
    void bookingME(const GEMGeometry* gem_geometry_);
    void buildLUT();
    std::pair<int,int> getClosestChambers(int region, float phi);
    
    virtual void beginJob() override;
    virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
    virtual void endJob() override;

    virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
    virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
      //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

      // ----------member data ---------------------------
    
    bool debug_;   
    bool checkTrack_;
    std::string folderPath_;
    bool EffSaveRootFile_;
    std::string EffRootFileName_;

    
    DQMStore * dbe;
    std::map<std::string, MonitorElement*> meCollection;
    
    edm::Handle<GEMRecHitCollection> gemRecHits_;
    edm::Handle<edm::PSimHitContainer> GEMHits;
    edm::Handle<edm::SimTrackContainer> sim_tracks;
    edm::Handle<edm::SimVertexContainer> sim_vertices;
    edm::ESHandle<GEMGeometry> gem_geom_;
    
    const GEMGeometry* gem_geometry_;

    
    /*-------------------------------------------------*/
    //This structures are defined in "SimRecStructures.h"
    MyGEMRecHit gem_recHit_;
    MyGEMSimHit gem_sh;
    MySimTrack track_;
    /*-------------------------------------------------*/

    edm::ParameterSet cfg_;
    edm::InputTag gemRecHitInput_;
    edm::InputTag gemSimHitInput_;
    edm::InputTag simTrackInput_;    
    double simTrackMinPt_;
    double simTrackMaxPt_;
    double simTrackMinEta_;
    double simTrackMaxEta_;
    double simTrackOnlyMuon_;
    float radiusCenter_;
    float chamberHeight_;
    
    std::pair<std::vector<float>,std::vector<int> > positiveLUT_;
    std::pair<std::vector<float>,std::vector<int> > negativeLUT_;
    
    bool hasGEMGeometry_;
   
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
MuonGEMRecHits::MuonGEMRecHits(const edm::ParameterSet& iConfig):
debug_(iConfig.getUntrackedParameter<bool>("debug")),
checkTrack_(iConfig.getUntrackedParameter<bool>("AnalyzeTracks")),
folderPath_(iConfig.getUntrackedParameter<std::string>("folderPath")),
EffSaveRootFile_(iConfig.getUntrackedParameter<bool>("EffSaveRootFile")),
EffRootFileName_(iConfig.getUntrackedParameter<std::string>("EffRootFileName"))
{
    
    auto cfg_ = iConfig.getParameter<edm::ParameterSet>("simTrackMatching");
    auto simTrack = cfg_.getParameter<edm::ParameterSet>("simTrack");
    simTrackInput_ = simTrack.getParameter<edm::InputTag>("input");
    simTrackMinPt_ = simTrack.getParameter<double>("minPt");
    simTrackMaxPt_ = simTrack.getParameter<double>("maxPt");
    simTrackMinEta_ = simTrack.getParameter<double>("minEta");
    simTrackMaxEta_ = simTrack.getParameter<double>("maxEta");
    simTrackOnlyMuon_ = simTrack.getParameter<bool>("onlyMuon");
    
    auto gemSimHit = cfg_.getParameter<edm::ParameterSet>("gemSimHit");
    gemSimHitInput_ = gemSimHit.getParameter<edm::InputTag>("input");
    
    auto gemRecHit = cfg_.getParameter<edm::ParameterSet>("gemRecHit");
    gemRecHitInput_ = gemRecHit.getParameter<edm::InputTag>("input");
    
    dbe = edm::Service<DQMStore>().operator->();
    
    if(debug_) std::cout<<"booking Global histograms with "<<folderPath_<<std::endl;
    std::string folder;
    folder = folderPath_;
    dbe->setCurrentFolder(folder);
    
    hasGEMGeometry_=false;
}


MuonGEMRecHits::~MuonGEMRecHits()
{
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//RecHit Matching
bool MuonGEMRecHits::isGEMRecHitMatched(MyGEMRecHit gem_recHit_, MyGEMSimHit gem_sh)
{
    
    Int_t gem_region = gem_recHit_.region;
    Int_t gem_layer = gem_recHit_.layer;
    Int_t gem_station = gem_recHit_.station;
    Int_t gem_chamber = gem_recHit_.chamber;
    Int_t gem_roll = gem_recHit_.roll;
    Int_t gem_firstStrip = gem_recHit_.firstClusterStrip;
    Int_t gem_cls = gem_recHit_.clusterSize;
    
    Int_t gem_sh_region = gem_sh.region;
    Int_t gem_sh_layer = gem_sh.layer;
    Int_t gem_sh_station = gem_sh.station;
    Int_t gem_sh_chamber = gem_sh.chamber;
    Int_t gem_sh_roll = gem_sh.roll;
    Int_t gem_sh_strip = gem_sh.strip;
    
    std::vector<int> stripsFired;
    for(int i = gem_firstStrip; i < (gem_firstStrip + gem_cls); i++){
        
        stripsFired.push_back(i);
        
    }
    
    bool cond1, cond2, cond3;
    
    if(gem_sh_region == gem_region && gem_sh_layer == gem_layer && gem_sh_station == gem_station) cond1 = true;
    else cond1 = false;
    if(gem_sh_chamber == gem_chamber && gem_sh_roll == gem_roll) cond2 = true;
    else cond2 = false;
    if(std::find(stripsFired.begin(), stripsFired.end(), (gem_sh_strip + 1)) != stripsFired.end()) cond3 = true;
    else cond3 = false;
    
    return (cond1 & cond2 & cond3);
    
}




// ------------ method called for each event  ------------
void
MuonGEMRecHits::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    using namespace edm;
    
    iEvent.getByLabel(gemRecHitInput_, gemRecHits_);
    iEvent.getByLabel(gemSimHitInput_, GEMHits);
    iEvent.getByLabel(simTrackInput_, sim_tracks);
   //-----
    std::vector<int> trackIds;
    std::vector<int> trackType;
    const edm::SimTrackContainer & sim_trks = *sim_tracks.product();
    
    for (auto& t: sim_trks)
    {
        
        if (!isSimTrackGood(t)) continue;
        trackType.push_back(t.type());
        trackIds.push_back(t.trackId());
        
    }
   
    for (edm::PSimHitContainer::const_iterator itHit = GEMHits->begin(); itHit!=GEMHits->end(); ++itHit) {
  
        if(abs(itHit->particleType()) != 13) continue;
        if(std::find(trackIds.begin(), trackIds.end(), itHit->trackId()) == trackIds.end()) continue;
        
        
        gem_sh.eventNumber = iEvent.id().event();
        gem_sh.detUnitId = itHit->detUnitId();
        gem_sh.particleType = itHit->particleType();
        gem_sh.x = itHit->localPosition().x();
        gem_sh.y = itHit->localPosition().y();
        gem_sh.energyLoss = itHit->energyLoss();
        gem_sh.pabs = itHit->pabs();
        gem_sh.timeOfFlight = itHit->timeOfFlight();

        const GEMDetId id(itHit->detUnitId());
        
        gem_sh.region = id.region();
        gem_sh.ring = id.ring();
        gem_sh.station = id.station();
        gem_sh.layer = id.layer();
        gem_sh.chamber = id.chamber();
        gem_sh.roll = id.roll();
        
        const LocalPoint p0(0., 0., 0.);
        const GlobalPoint Gp0(gem_geometry_->idToDet(itHit->detUnitId())->surface().toGlobal(p0));
        
        gem_sh.Phi_0 = Gp0.phi();
        gem_sh.R_0 = Gp0.perp();
        gem_sh.DeltaPhi = atan(-1*id.region()*pow(-1,id.chamber())*itHit->localPosition().x()/(Gp0.perp() + itHit->localPosition().y()));
        
        const LocalPoint hitLP(itHit->localPosition());
        const GlobalPoint hitGP(gem_geometry_->idToDet(itHit->detUnitId())->surface().toGlobal(hitLP));
        gem_sh.globalR = hitGP.perp();
        gem_sh.globalEta = hitGP.eta();
        gem_sh.globalPhi = hitGP.phi();
        gem_sh.globalX = hitGP.x();
        gem_sh.globalY = hitGP.y();
        gem_sh.globalZ = hitGP.z();
        
        //  Now filling strip info using entry point rather than local position to be
        //  consistent with digi strips. To change back, just switch the comments - WHF
        //  gem_sh.strip=gem_geometry_->etaPartition(itHit->detUnitId())->strip(hitLP);
        const LocalPoint hitEP(itHit->entryPoint());
        gem_sh.strip = gem_geometry_->etaPartition(itHit->detUnitId())->strip(hitEP);
        
        int count = 0;
        //std::cout<<"SimHit: region "<<gem_sh.region<<" station "<<gem_sh.station<<" layer "<<gem_sh.layer<<" chamber "<<gem_sh.chamber<<" roll "<<gem_sh.roll<<" strip "<<gem_sh.strip<<" type "<<itHit->particleType()<<" id "<<itHit->trackId()<<std::endl;
        for (GEMRecHitCollection::const_iterator recHit = gemRecHits_->begin(); recHit != gemRecHits_->end(); ++recHit)
        {
            gem_recHit_.x = recHit->localPosition().x();
            gem_recHit_.xErr = recHit->localPositionError().xx();
            gem_recHit_.y = recHit->localPosition().y();
            gem_recHit_.detId = (Short_t) (*recHit).gemId();
            gem_recHit_.bx = recHit->BunchX();
            gem_recHit_.clusterSize = recHit->clusterSize();
            gem_recHit_.firstClusterStrip = recHit->firstClusterStrip();
            
            GEMDetId id((*recHit).gemId());
            
            gem_recHit_.region = (Short_t) id.region();
            gem_recHit_.ring = (Short_t) id.ring();
            gem_recHit_.station = (Short_t) id.station();
            gem_recHit_.layer = (Short_t) id.layer();
            gem_recHit_.chamber = (Short_t) id.chamber();
            gem_recHit_.roll = (Short_t) id.roll();
            
            LocalPoint hitLP = recHit->localPosition();
            GlobalPoint hitGP = gem_geometry_->idToDet((*recHit).gemId())->surface().toGlobal(hitLP);
            
            gem_recHit_.globalR = hitGP.perp();
            gem_recHit_.globalEta = hitGP.eta();
            gem_recHit_.globalPhi = hitGP.phi();
            gem_recHit_.globalX = hitGP.x();
            gem_recHit_.globalY = hitGP.y();
            gem_recHit_.globalZ = hitGP.z();
            
            gem_recHit_.x_sim = gem_sh.x;
            gem_recHit_.y_sim = gem_sh.y;
            gem_recHit_.globalEta_sim = gem_sh.globalEta;
            gem_recHit_.globalPhi_sim = gem_sh.globalPhi;
            gem_recHit_.globalX_sim = gem_sh.globalX;
            gem_recHit_.globalY_sim = gem_sh.globalY;
            gem_recHit_.globalZ_sim = gem_sh.globalZ;
            gem_recHit_.pull = (gem_sh.x - gem_recHit_.x) / gem_recHit_.xErr;
           
        /*-----------BunchCrossing----------------*/
            meCollection["bxDistribution"]->Fill(gem_recHit_.bx);
            if(gem_recHit_.station==1) meCollection["bxDistribution_st1"]->Fill(gem_recHit_.bx);
            if(gem_recHit_.station==2) meCollection["bxDistribution_st2"]->Fill(gem_recHit_.bx);
            if(gem_recHit_.station==3) meCollection["bxDistribution_st3"]->Fill(gem_recHit_.bx);

            if(gem_recHit_.bx != 0) continue;
            if(isGEMRecHitMatched(gem_recHit_, gem_sh))
            {
                bool verbose(false);
                if (verbose)
                    std::cout<<"RecHit: region "<<gem_recHit_.region<<" station "<<gem_recHit_.station
                    <<" layer "<<gem_recHit_.layer<<" chamber "<<gem_recHit_.chamber
                    <<" roll "<<gem_recHit_.roll<<" firstStrip "<<gem_recHit_.firstClusterStrip
                    <<" cls "<<gem_recHit_.clusterSize<<" bx "<<gem_recHit_.bx<<std::endl;
                
        /*----------------ClustersSize--------------------------*/
                meCollection["clsDistribution"]->Fill(gem_recHit_.clusterSize);
                if(gem_recHit_.region==-1 && gem_recHit_.layer==1) meCollection["clsDistribution_rm1_l1"]->Fill(gem_recHit_.clusterSize);
                if(gem_recHit_.region==-1 && gem_recHit_.layer==2) meCollection["clsDistribution_rm1_l2"]->Fill(gem_recHit_.clusterSize);

                if(gem_recHit_.region==1 && gem_recHit_.layer==1) meCollection["clsDistribution_rp1_l1"]->Fill(gem_recHit_.clusterSize);
                if(gem_recHit_.region==1 && gem_recHit_.layer==2) meCollection["clsDistribution_rp1_l2"]->Fill(gem_recHit_.clusterSize);
                
        /*-----------------------X pull--------------------------*/
                meCollection["recHitPullX"]->Fill(gem_recHit_.pull);
                
                //Station 1
                if(gem_recHit_.station==1 && gem_recHit_.region==-1 && gem_recHit_.layer==1) meCollection["recHitPullX_rm1_st1_l1"]->Fill(gem_recHit_.pull);
                if(gem_recHit_.station==1 && gem_recHit_.region==-1 && gem_recHit_.layer==2) meCollection["recHitPullX_rm1_st1_l2"]->Fill(gem_recHit_.pull);
                if(gem_recHit_.station==1 && gem_recHit_.region==1 && gem_recHit_.layer==1) meCollection["recHitPullX_rp1_st1_l1"]->Fill(gem_recHit_.pull);
                if(gem_recHit_.station==1 && gem_recHit_.region==1 && gem_recHit_.layer==2) meCollection["recHitPullX_rp1_st1_l2"]->Fill(gem_recHit_.pull);
                
                //Station 2
                if(gem_recHit_.station==2 && gem_recHit_.region==-1 && gem_recHit_.layer==1) meCollection["recHitPullX_rm1_st2_l1"]->Fill(gem_recHit_.pull);
                if(gem_recHit_.station==2 && gem_recHit_.region==-1 && gem_recHit_.layer==2) meCollection["recHitPullX_rm1_st2_l2"]->Fill(gem_recHit_.pull);
                if(gem_recHit_.station==2 && gem_recHit_.region==1 && gem_recHit_.layer==1) meCollection["recHitPullX_rp1_st2_l1"]->Fill(gem_recHit_.pull);
                if(gem_recHit_.station==2 && gem_recHit_.region==1 && gem_recHit_.layer==2) meCollection["recHitPullX_rp1_st2_l2"]->Fill(gem_recHit_.pull);
                
                //Station 3
                if(gem_recHit_.station==3 && gem_recHit_.region==-1 && gem_recHit_.layer==1) meCollection["recHitPullX_rm1_st3_l1"]->Fill(gem_recHit_.pull);
                if(gem_recHit_.station==3 && gem_recHit_.region==-1 && gem_recHit_.layer==2) meCollection["recHitPullX_rm1_st3_l2"]->Fill(gem_recHit_.pull);
                if(gem_recHit_.station==3 && gem_recHit_.region==1 && gem_recHit_.layer==1) meCollection["recHitPullX_rp1_st3_l1"]->Fill(gem_recHit_.pull);
                if(gem_recHit_.station==3 && gem_recHit_.region==1 && gem_recHit_.layer==2) meCollection["recHitPullX_rp1_st3_l2"]->Fill(gem_recHit_.pull);
                
        /*---------------------DeltaPhi-------------------------------*/
                double deltaPhi = gem_recHit_.globalPhi-gem_sh.globalPhi;
                
                meCollection["recHitDPhi"]->Fill(deltaPhi);
                if(gem_recHit_.station==1) meCollection["recHitDPhi_st1"]->Fill(deltaPhi);
                if(gem_recHit_.station==2) meCollection["recHitDPhi_st2"]->Fill(deltaPhi);
                if(gem_recHit_.station==3) meCollection["recHitDPhi_st3"]->Fill(deltaPhi);
                
                if(gem_recHit_.station==1 && gem_recHit_.clusterSize==1)meCollection["recHitDPhi_st1_cls1"]->Fill(deltaPhi);
                if(gem_recHit_.station==2 && gem_recHit_.clusterSize==1)meCollection["recHitDPhi_st2_cls1"]->Fill(deltaPhi);
                if(gem_recHit_.station==3 && gem_recHit_.clusterSize==1)meCollection["recHitDPhi_st3_cls1"]->Fill(deltaPhi);
                
                if(gem_recHit_.station==1 && gem_recHit_.clusterSize==2)meCollection["recHitDPhi_st1_cls2"]->Fill(deltaPhi);
                if(gem_recHit_.station==2 && gem_recHit_.clusterSize==2)meCollection["recHitDPhi_st2_cls2"]->Fill(deltaPhi);
                if(gem_recHit_.station==3 && gem_recHit_.clusterSize==2)meCollection["recHitDPhi_st3_cls2"]->Fill(deltaPhi);
                
                if(gem_recHit_.station==1 && gem_recHit_.clusterSize==3)meCollection["recHitDPhi_st1_cls3"]->Fill(deltaPhi);
                if(gem_recHit_.station==2 && gem_recHit_.clusterSize==3)meCollection["recHitDPhi_st2_cls3"]->Fill(deltaPhi);
                if(gem_recHit_.station==3 && gem_recHit_.clusterSize==3)meCollection["recHitDPhi_st3_cls3"]->Fill(deltaPhi);
                
        /*---------------------Occupancy XY---------------------------*/
                //Station 1
                if(gem_recHit_.station==1 && gem_recHit_.region==-1 && gem_recHit_.layer==1) meCollection["localrh_xy_rm1_st1_l1"]->Fill(gem_recHit_.globalX,gem_recHit_.globalY);
                if(gem_recHit_.station==1 && gem_recHit_.region==-1 && gem_recHit_.layer==2) meCollection["localrh_xy_rm1_st1_l2"]->Fill(gem_recHit_.globalX,gem_recHit_.globalY);
                if(gem_recHit_.station==1 && gem_recHit_.region==1 && gem_recHit_.layer==1) meCollection["localrh_xy_rp1_st1_l1"]->Fill(gem_recHit_.globalX,gem_recHit_.globalY);
                if(gem_recHit_.station==1 && gem_recHit_.region==1 && gem_recHit_.layer==2) meCollection["localrh_xy_rp1_st1_l2"]->Fill(gem_recHit_.globalX,gem_recHit_.globalY);
                 
                
                //Station 2
                if(gem_recHit_.station==2 && gem_recHit_.region==-1 && gem_recHit_.layer==1) meCollection["localrh_xy_rm1_st2_l1"]->Fill(gem_recHit_.globalX,gem_recHit_.globalY);
                if(gem_recHit_.station==2 && gem_recHit_.region==-1 && gem_recHit_.layer==2) meCollection["localrh_xy_rm1_st2_l2"]->Fill(gem_recHit_.globalX,gem_recHit_.globalY);
                if(gem_recHit_.station==2 && gem_recHit_.region==1 && gem_recHit_.region==1) meCollection["localrh_xy_rp1_st2_l1"]->Fill(gem_recHit_.globalX,gem_recHit_.globalY);
                if(gem_recHit_.station==2 && gem_recHit_.region==1 && gem_recHit_.layer==2) meCollection["localrh_xy_rp1_st2_l2"]->Fill(gem_recHit_.globalX,gem_recHit_.globalY);
                
                //Station 3
                if(gem_recHit_.station==3 && gem_recHit_.region==-1 && gem_recHit_.layer==1) meCollection["localrh_xy_rm1_st3_l1"]->Fill(gem_recHit_.globalX,gem_recHit_.globalY);
                if(gem_recHit_.station==3 && gem_recHit_.region==-1 && gem_recHit_.layer==2) meCollection["localrh_xy_rm1_st3_l2"]->Fill(gem_recHit_.globalX,gem_recHit_.globalY);
                if(gem_recHit_.station==3 && gem_recHit_.region==1 && gem_recHit_.region==1) meCollection["localrh_xy_rp1_st3_l1"]->Fill(gem_recHit_.globalX,gem_recHit_.globalY);
                if(gem_recHit_.station==3 && gem_recHit_.region==1 && gem_recHit_.layer==2) meCollection["localrh_xy_rp1_st3_l2"]->Fill(gem_recHit_.globalX,gem_recHit_.globalY);

                
                double glb_R=sqrt(gem_recHit_.globalX*gem_recHit_.globalX+gem_recHit_.globalY*gem_recHit_.globalY);
                if(gem_recHit_.station==1 && gem_recHit_.region==-1) meCollection["localrh_zr_rm1_st1"]->Fill(gem_recHit_.globalZ,glb_R);
                if(gem_recHit_.station==1 && gem_recHit_.region==1) meCollection["localrh_zr_rp1_st1"]->Fill(gem_recHit_.globalZ,glb_R);
                
        /*-------------------Strips--------------------------------------*/
                
                meCollection["strip_rh_tot"]->Fill(gem_recHit_.firstClusterStrip);
                
                //Station 1
                if(gem_recHit_.station==1 && gem_recHit_.region==-1 && gem_recHit_.layer==1) meCollection["strip_rh_rm1_st1_l1_tot"]->Fill(gem_recHit_.firstClusterStrip);
                if(gem_recHit_.station==1 && gem_recHit_.region==-1 && gem_recHit_.layer==2) meCollection["strip_rh_rm1_st1_l2_tot"]->Fill(gem_recHit_.firstClusterStrip);
                if(gem_recHit_.station==1 && gem_recHit_.region==1 && gem_recHit_.layer==1) meCollection["strip_rh_rp1_st1_l1_tot"]->Fill(gem_recHit_.firstClusterStrip);
                if(gem_recHit_.station==1 && gem_recHit_.region==1 && gem_recHit_.layer==2) meCollection["strip_rh_rp1_st1_l2_tot"]->Fill(gem_recHit_.firstClusterStrip);
                
                //Station 2
                if(gem_recHit_.station==2 && gem_recHit_.region==-1 && gem_recHit_.layer==1) meCollection["strip_rh_rm1_st2_l1_tot"]->Fill(gem_recHit_.firstClusterStrip);
                if(gem_recHit_.station==2 && gem_recHit_.region==-1 && gem_recHit_.layer==2) meCollection["strip_rh_rm1_st2_l2_tot"]->Fill(gem_recHit_.firstClusterStrip);
                if(gem_recHit_.station==2 && gem_recHit_.region==1 && gem_recHit_.layer==1) meCollection["strip_rh_rp1_st2_l1_tot"]->Fill(gem_recHit_.firstClusterStrip);
                if(gem_recHit_.station==2 && gem_recHit_.region==1 && gem_recHit_.layer==2) meCollection["strip_rh_rp1_st2_l2_tot"]->Fill(gem_recHit_.firstClusterStrip);
                
                //Station 3
                if(gem_recHit_.station==3 && gem_recHit_.region==-1 && gem_recHit_.layer==1) meCollection["strip_rh_rm1_st3_l1_tot"]->Fill(gem_recHit_.firstClusterStrip);
                if(gem_recHit_.station==3 && gem_recHit_.region==-1 && gem_recHit_.layer==2) meCollection["strip_rh_rm1_st3_l2_tot"]->Fill(gem_recHit_.firstClusterStrip);
                if(gem_recHit_.station==3 && gem_recHit_.region==1 && gem_recHit_.layer==1) meCollection["strip_rh_rp1_st3_l1_tot"]->Fill(gem_recHit_.firstClusterStrip);
                if(gem_recHit_.station==3 && gem_recHit_.region==1 && gem_recHit_.layer==2) meCollection["strip_rh_rp1_st3_l2_tot"]->Fill(gem_recHit_.firstClusterStrip);
                
        /*--------------------------StripsVsRolls------------------------*/
                meCollection["roll_vs_strip_rh"]->Fill(gem_recHit_.firstClusterStrip,gem_recHit_.roll);
                
                //Station 1
                if(gem_recHit_.station==1 && gem_recHit_.region==-1 && gem_recHit_.layer==1) meCollection["roll_vs_strip_rh_rm1_st1_l1"]->Fill(gem_recHit_.firstClusterStrip,gem_recHit_.roll);
                if(gem_recHit_.station==1 && gem_recHit_.region==-1 && gem_recHit_.layer==2) meCollection["roll_vs_strip_rh_rm1_st1_l2"]->Fill(gem_recHit_.firstClusterStrip,gem_recHit_.roll);
                if(gem_recHit_.station==1 && gem_recHit_.region==1 && gem_recHit_.layer==1) meCollection["roll_vs_strip_rh_rp1_st1_l1"]->Fill(gem_recHit_.firstClusterStrip,gem_recHit_.roll);
                if(gem_recHit_.station==1 && gem_recHit_.region==1 && gem_recHit_.layer==2) meCollection["roll_vs_strip_rh_rp1_st1_l2"]->Fill(gem_recHit_.firstClusterStrip,gem_recHit_.roll);
                
                //Station 2
                if(gem_recHit_.station==2 && gem_recHit_.region==-1 && gem_recHit_.layer==1) meCollection["roll_vs_strip_rh_rm1_st2_l1"]->Fill(gem_recHit_.firstClusterStrip,gem_recHit_.roll);
                if(gem_recHit_.station==2 && gem_recHit_.region==-1 && gem_recHit_.layer==2) meCollection["roll_vs_strip_rh_rm1_st2_l2"]->Fill(gem_recHit_.firstClusterStrip,gem_recHit_.roll);
                if(gem_recHit_.station==2 && gem_recHit_.region==1 && gem_recHit_.layer==1) meCollection["roll_vs_strip_rh_rp1_st2_l1"]->Fill(gem_recHit_.firstClusterStrip,gem_recHit_.roll);
                if(gem_recHit_.station==2 && gem_recHit_.region==1 && gem_recHit_.layer==2) meCollection["roll_vs_strip_rh_rp1_st2_l2"]->Fill(gem_recHit_.firstClusterStrip,gem_recHit_.roll);
                
                //Station 1
                if(gem_recHit_.station==3 && gem_recHit_.region==-1 && gem_recHit_.layer==1) meCollection["roll_vs_strip_rh_rm1_st3_l1"]->Fill(gem_recHit_.firstClusterStrip,gem_recHit_.roll);
                if(gem_recHit_.station==3 && gem_recHit_.region==-1 && gem_recHit_.layer==2) meCollection["roll_vs_strip_rh_rm1_st3_l2"]->Fill(gem_recHit_.firstClusterStrip,gem_recHit_.roll);
                if(gem_recHit_.station==3 && gem_recHit_.region==1 && gem_recHit_.layer==1) meCollection["roll_vs_strip_rh_rp1_st3_l1"]->Fill(gem_recHit_.firstClusterStrip,gem_recHit_.roll);
                if(gem_recHit_.station==3 && gem_recHit_.region==1 && gem_recHit_.layer==2) meCollection["roll_vs_strip_rh_rp1_st3_l2"]->Fill(gem_recHit_.firstClusterStrip,gem_recHit_.roll);
                
                count++;
            }
        }
        gem_sh.countMatching = count;
        
    }
    
    ///////////////////////////////////
    ///    Tracks
    
    const edm::SimVertexContainer & sim_vert = *sim_vertices.product();
    if(checkTrack_){
        for (auto& t: sim_trks)
            {
        if (!isSimTrackGood(t)) continue;
        // match hits and digis to this SimTrack
                cout<<"Check>>>>>>>>>>>>>>"<<endl;
        SimTrackMatchManager match(t, sim_vert[t.vertIndex()], cfg_, iEvent, iSetup);
                cout<<"Check>>>>>>>>>>>>>>"<<endl;
        const SimHitMatcher& match_sh = match.simhits();
        const GEMRecHitMatcher& match_rh = match.gemRecHits();
        
        track_.pt = t.momentum().pt();
        track_.phi = t.momentum().phi();
        track_.eta = t.momentum().eta();
        track_.charge = t.charge();
        track_.endcap = (track_.eta > 0.) ? 1 : -1;
        track_.gem_sh_layer1 = 0;
        track_.gem_sh_layer2 = 0;
        track_.gem_rh_layer1 = 0;
        track_.gem_rh_layer2 = 0;
        track_.gem_sh_eta = -9.;
        track_.gem_sh_phi = -9.;
        track_.gem_sh_x = -999;
        track_.gem_sh_y = -999;
        track_.gem_rh_eta = -9.;
        track_.gem_rh_phi = -9.;
        track_.gem_trk_rho = -999.;
        track_.gem_lx_even = -999.;
        track_.gem_ly_even = -999.;
        track_.gem_lx_odd = -999.;
        track_.gem_ly_odd = -999.;
        track_.has_gem_sh_l1 = 0;
        track_.has_gem_sh_l2 = 0;
        track_.has_gem_rh_l1 = 0;
        track_.has_gem_rh_l2 = 0;
        
        // ** GEM SimHits ** //
        auto gem_sh_ids_sch = match_sh.superChamberIdsGEM();
        for(auto d: gem_sh_ids_sch)
        {
            auto gem_simhits = match_sh.hitsInSuperChamber(d);
            auto gem_simhits_gp = match_sh.simHitsMeanPosition(gem_simhits);

            track_.gem_sh_eta = gem_simhits_gp.eta();
            track_.gem_sh_phi = gem_simhits_gp.phi();
            track_.gem_sh_x = gem_simhits_gp.x();
            track_.gem_sh_y = gem_simhits_gp.y();
        }
        
        // Calculation of the localXY efficiency
        GlobalPoint gp_track(match_sh.propagatedPositionGEM());
        track_.gem_trk_eta = gp_track.eta();
        track_.gem_trk_phi = gp_track.phi();
        track_.gem_trk_rho = gp_track.perp();
        //std::cout << "track eta phi rho = " << track_.gem_trk_eta << " " << track_.gem_trk_phi << " " << track_.gem_trk_rho << std::endl;
        
        float track_angle = gp_track.phi().degrees();
        if (track_angle < 0.) track_angle += 360.;
        //std::cout << "track angle = " << track_angle << std::endl;
        const int track_region = (gp_track.z() > 0 ? 1 : -1);
        
        // closest chambers in phi
        const auto mypair = getClosestChambers(track_region, track_angle);
        
        // chambers
        GEMDetId detId_first(mypair.first);
        GEMDetId detId_second(mypair.second);
        
        // assignment of local even and odd chambers (there is always an even and an odd chamber)
        bool firstIsOdd = detId_first.chamber() & 1;
        
        GEMDetId detId_even_L1(firstIsOdd ? detId_second : detId_first);
        GEMDetId detId_odd_L1(firstIsOdd ? detId_first : detId_second);
        
        auto even_partition = gem_geometry_->idToDetUnit(detId_even_L1)->surface();
        auto odd_partition = gem_geometry_->idToDetUnit(detId_odd_L1)->surface();
        
        // global positions of partitions' centers
        LocalPoint p0(0.,0.,0.);
        GlobalPoint gp_even_partition = even_partition.toGlobal(p0);
        GlobalPoint gp_odd_partition = odd_partition.toGlobal(p0);
        
        LocalPoint lp_track_even_partition = even_partition.toLocal(gp_track);
        LocalPoint lp_track_odd_partition = odd_partition.toLocal(gp_track);
        
        // track chamber local x is the same as track partition local x
        track_.gem_lx_even = lp_track_even_partition.x();
        track_.gem_lx_odd = lp_track_odd_partition.x();
        
        // track chamber local y is the same as track partition local y
        // corrected for partition's local y WRT chamber
        track_.gem_ly_even = lp_track_even_partition.y() + (gp_even_partition.perp() - radiusCenter_);
        track_.gem_ly_odd = lp_track_odd_partition.y() + (gp_odd_partition.perp() - radiusCenter_);
        
        //std::cout << track_.gem_lx_even << " " << track_.gem_ly_even << std::endl;
        //std::cout << track_.gem_lx_odd << " " << track_.gem_ly_odd << std::endl;
        
        
        auto gem_sh_ids_ch = match_sh.chamberIdsGEM();
        for(auto d: gem_sh_ids_ch)
        {
            GEMDetId id(d);
            bool odd(id.chamber() & 1);
            
            if (id.layer() == 1)
            {
                if (odd) track_.gem_sh_layer1 |= 1;
                else track_.gem_sh_layer1 |= 2;
            }
            else if (id.layer() == 2)
            {
                if (odd) track_.gem_sh_layer2 |= 1;
                else track_.gem_sh_layer2 |= 2;
            }
        }
        
        // ** GEM RecHits ** //
        auto gem_rh_ids_sch = match_rh.superChamberIds();
        for(auto d: gem_rh_ids_sch)
        {
            auto gem_recHits = match_rh.recHitsInSuperChamber(d);
            auto gem_rh_gp = match_rh.recHitMeanPosition(gem_recHits);
            
            track_.gem_rh_eta = gem_rh_gp.eta();
            track_.gem_rh_phi = gem_rh_gp.phi();
            
        }
        
        auto gem_rh_ids_ch = match_rh.chamberIds();
        for(auto d: gem_rh_ids_ch)
        {
            GEMDetId id(d);
            bool odd(id.chamber() & 1);
            
            if (id.layer() == 1)
            {
                if (odd)
                {
                    track_.gem_rh_layer1 |= 1;
                }
                else
                {
                    track_.gem_rh_layer1 |= 2;
                }
            }
            else if (id.layer() == 2)
            {
                if (odd)
                {
                    track_.gem_rh_layer2 |= 1;
                }
                else
                {
                    track_.gem_rh_layer2 |= 2;
                }
            }
        }
        
        // Construct Chamber DetIds from the "projected" ids:
        GEMDetId id_ch_even_L1(detId_even_L1.region(), detId_even_L1.ring(), detId_even_L1.station(), 1, detId_even_L1.chamber(), 0);
        GEMDetId id_ch_odd_L1(detId_odd_L1.region(), detId_odd_L1.ring(), detId_odd_L1.station(), 1, detId_odd_L1.chamber(), 0);
        GEMDetId id_ch_even_L2(detId_even_L1.region(), detId_even_L1.ring(), detId_even_L1.station(), 2, detId_even_L1.chamber(), 0);
        GEMDetId id_ch_odd_L2(detId_odd_L1.region(), detId_odd_L1.ring(), detId_odd_L1.station(), 2, detId_odd_L1.chamber(), 0);
        
        // check if track has sh
        if(gem_sh_ids_ch.count(id_ch_even_L1)!=0) track_.has_gem_sh_l1 |= 2;
        if(gem_sh_ids_ch.count(id_ch_odd_L1)!=0) track_.has_gem_sh_l1 |= 1;
        if(gem_sh_ids_ch.count(id_ch_even_L2)!=0) track_.has_gem_sh_l2 |= 2;
        if(gem_sh_ids_ch.count(id_ch_odd_L2)!=0) track_.has_gem_sh_l2 |= 1;
        
        // check if track has rh
        if(gem_rh_ids_ch.count(id_ch_even_L1)!=0){
            track_.has_gem_rh_l1 |= 2;
        }
        if(gem_rh_ids_ch.count(id_ch_odd_L1)!=0){
            track_.has_gem_rh_l1 |= 1;
        }
        if(gem_rh_ids_ch.count(id_ch_even_L2)!=0){
            track_.has_gem_rh_l2 |= 2;
        }
        if(gem_rh_ids_ch.count(id_ch_odd_L2)!=0){
            track_.has_gem_rh_l2 |= 1;
        }
        
    }
    }
}
    

bool MuonGEMRecHits::isSimTrackGood(const SimTrack &t)
{
    // SimTrack selection
    if (t.noVertex()) return false;
    if (t.noGenpart()) return false;
    // only muons
    if (std::abs(t.type()) != 13) return false;
    // pt selection
    //if (t.momentum().pt() < simTrackMinPt_) return false;
    // eta selection
    const float eta(std::abs(t.momentum().eta()));
//    if (eta > simTrackMaxEta_ || eta < simTrackMinEta_) return false;
    if (eta > 2.2 || eta < 1.5) return false;
    return true;
}

void MuonGEMRecHits::bookingME(const GEMGeometry* gem_geometry_){
    std::cout<<"Print ============================>     "<<gem_geometry_->regions().size()<<std::endl;//[0]->stations()[0]->superChambers()[0]->chambers()[0]->etaPartitions()[0]->specs()->parameters()[3]<<std::endl; 
    std::cout<<"Print ============================>     "<<gem_geometry_->regions()[0]->stations().size()<<std::endl;	
    int num_region=gem_geometry_->regions().size();
    int num_station=gem_geometry_->regions()[0]->stations().size();
    float nStrips=0;
    
    std::string region[2] ={"m1", "p1"};
    std::string station[3]={ "_st1", "_st2", "_st3" };

    //---------------------------------//
    meCollection["denominatore_eta"] = dbe->book1D("denominatore_eta","Tracks matched SimHit",30,-3,3);
    //--------------------------------//
    
    meCollection["clsDistribution"] = dbe->book1D("clsDistribution","ClusterSizeDistribution",11,-0.5,10.5);

    meCollection["bxDistribution"] = dbe->book1D("bxDistribution","BunchCrossingDistribution",11,-5.5,5.5);
    meCollection["recHitPullX"] = dbe->book1D("recHitPullX","recHitPullX",100,-50,+50);
    meCollection["recHitDPhi"] = dbe->book1D("recHitDPhi","DeltaPhi RecHit",100,-0.001,+0.001);
    meCollection["localrh_zr_rm1_st1"] = dbe->book2D("localrh_zr_rm1_st1","GEM RecHit occupancy: region-1",200,-573,-564,110,130,240);
    meCollection["localrh_zr_rp1_st1"] = dbe->book2D("localrh_zr_rp1_st1","GEM RecHit occupancy: region1",200,573,564,110,130,240);
    meCollection["strip_rh_tot"] = dbe->book1D("strip_rh_tot","GEM RecHit occupancy per strip number",384,0.5,384.5);
    meCollection["roll_vs_strip_rh"] = dbe->book2D("roll_vs_strip_rh","GEM RecHit occupancy per roll and strip number",768,0.5,768.5,12,0.5,12.5);
    
    for (int k=0; k<num_station; k++){
        //-----------------------BunchX--------------------------------------//
        meCollection["bxDistribution"+station[k]] = dbe->book1D("bxDistribution"+station[k],"BunchCrossingDistribution, Station="+std::to_string(k+1),11,-5.5,5.5);
        //-----------------------Delta Phi--------------------------------------//
        meCollection["recHitDPhi"+station[k]] = dbe->book1D("recHitDPhi"+station[k],"DeltaPhi RecHit, Station="+std::to_string(k+1),100,-0.001,+0.001);
        meCollection["recHitDPhi"+station[k]+"_cls1"] = dbe->book1D("recHitDPhi"+station[k]+"_cls1","DeltaPhi RecHit, Station="+std::to_string(k+1)+", CLS=1",100,-0.001,+0.001);
        meCollection["recHitDPhi"+station[k]+"_cls2"] = dbe->book1D("recHitDPhi"+station[k]+"_cls2","DeltaPhi RecHit, Station="+std::to_string(k+1)+", CLS=2",100,-0.001,+0.001);
        meCollection["recHitDPhi"+station[k]+"_cls3"] = dbe->book1D("recHitDPhi"+station[k]+"_cls3","DeltaPhi RecHit, Station="+std::to_string(k+1)+", CLS=3",100,-0.001,+0.001);
        
        
    }
    
    for (int j=0; j<num_region; j++){
        
        meCollection["clsDistribution_r"+region[j]+"_l1"] = dbe->book1D("clsDistribution_r"+region[j]+"_l1","ClusterSizeDistribution, region "+region[j]+", Layer=1",11,-0.5,10.5);
        meCollection["clsDistribution_r"+region[j]+"_l2"] = dbe->book1D("clsDistribution_r"+region[j]+"_l2","ClusterSizeDistribution, region "+region[j]+", Layer=2",11,-0.5,10.5);
        
        for (int i=0; i<num_station; i++) {
        
        //-------------------------(x_rec-x_sim)/x_sim-----------------------------------//
            meCollection["recHitPullX_r"+region[j]+station[i]+"_l1"] = dbe->book1D("recHitPullX_r"+region[j]+station[i]+"_l1","recHitPullX, region "+region[j]+", station"+std::to_string(i+1)+", layer1",100,-50,+50);
            meCollection["recHitPullX_r"+region[j]+station[i]+"_l2"] = dbe->book1D("recHitPullX_r"+region[j]+station[i]+"_l2","recHitPullX, region "+region[j]+", station"+std::to_string(i+1)+", layer2",100,-50,+50);
        
        //----------------Occupancy XY-------------------------------//
            meCollection["localrh_xy_r"+region[j]+station[i]+"_l1"] = dbe->book2D("localrh_xy_r"+region[j]+station[i]+"_l1","GEM RecHit occupancy: region "+region[j]+", station"+std::to_string(i+1)+", layer1",200,-360,360,200,-360,360);
            meCollection["localrh_xy_r"+region[j]+station[i]+"_l2"] = dbe->book2D("localrh_xy_r"+region[j]+station[i]+"_l2","GEM RecHit occupancy: region"+region[j]+", station"+std::to_string(i+1)+", layer2",200,-360,360,200,-360,360);
        
        //---------------------Strips Occupancy------------------//
        
            if(i==0) nStrips=384.;  /*Station1*/
            if(i>0)  nStrips=768.;  /*Station2 & 3*/
            meCollection["strip_rh_r"+region[j]+station[i]+"_l1_tot"] = dbe->book1D("strip_rh_r"+region[j]+station[i]+"_l1_tot","GEM RecHit occupancy per strip number, region "+region[j]+" layer1 station"+std::to_string(i+1),nStrips,0.5,nStrips+0.5);
            meCollection["strip_rh_r"+region[j]+station[i]+"_l2_tot"] = dbe->book1D("strip_rh_r"+region[j]+station[i]+"_l2_tot","GEM RecHit occupancy per strip number, region "+region[j]+" layer2 station"+std::to_string(i+1),nStrips,0.5,nStrips+0.5);
        
            meCollection["roll_vs_strip_rh_r"+region[j]+station[i]+"_l1"] = dbe->book2D("roll_vs_strip_rh_r"+region[j]+station[i]+"_l1","GEM RecHit occupancy per roll and strip number, region "+region[j]+" layer1 station"+std::to_string(i+1),nStrips,0.5,nStrips+0.5,12,0.5,12.5);
            meCollection["roll_vs_strip_rh_r"+region[j]+station[i]+"_l2"] = dbe->book2D("roll_vs_strip_rh_r"+region[j]+station[i]+"_l2","GEM RecHit occupancy per roll and strip number, region "+region[j]+" layer2 station"+std::to_string(i+1),nStrips,0.5,nStrips+0.5,12,0.5,12.5);
 
        }
    }
}
// ------------ method called once each job just before starting event loop  ------------
void MuonGEMRecHits::beginJob(){
    
}

// ------------ method called once each job just after ending the event loop  ------------
void MuonGEMRecHits::endJob(){
    
    dbe = 0;
    
}

// ------------ method called when starting to processes a run  ------------

void 
MuonGEMRecHits::beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup)
{
   try {
       iSetup.get<MuonGeometryRecord>().get(gem_geom_);
       gem_geometry_ = &*gem_geom_;
       hasGEMGeometry_=true;
    } catch (edm::eventsetup::NoProxyException<GEMGeometry>& e) {
        hasGEMGeometry_ = false;
        edm::LogWarning("MuonGEMRecHits") << "+++ Info: GEM geometry is unavailable. +++\n";
    }
    
    if(hasGEMGeometry_) {
        
        bookingME(gem_geometry_);
    
    }
}

// ------------ method called when ending the processing of a run  ------------

void 
MuonGEMRecHits::endRun(edm::Run const&, edm::EventSetup const&)
{
    
    if (EffSaveRootFile_) dbe->save(EffRootFileName_);
    
}


void MuonGEMRecHits::buildLUT()
{
    std::vector<int> pos_ids;
    pos_ids.push_back(GEMDetId(1,1,1,1,36,1).rawId());
    
    std::vector<int> neg_ids;
    neg_ids.push_back(GEMDetId(-1,1,1,1,36,1).rawId());
    
    // VK: I would really suggest getting phis from GEMGeometry
    
    std::vector<float> phis;
    phis.push_back(0.);
    for(int i=1; i<37; ++i)
    {
        pos_ids.push_back(GEMDetId(1,1,1,1,i,1).rawId());
        neg_ids.push_back(GEMDetId(-1,1,1,1,i,1).rawId());
        phis.push_back(i*10.);
    }
    positiveLUT_ = std::make_pair(phis,pos_ids);
    negativeLUT_ = std::make_pair(phis,neg_ids);
}

std::pair<int,int> MuonGEMRecHits::getClosestChambers(int region, float phi)
{
    auto& phis(positiveLUT_.first);
    auto upper = std::upper_bound(phis.begin(), phis.end(), phi);
    //std::cout << "lower = " << upper - phis.begin() << std::endl;
    //std::cout << "upper = " << upper - phis.begin() + 1 << std::endl;
    auto& LUT = (region == 1 ? positiveLUT_.second : negativeLUT_.second);
    return std::make_pair(LUT.at(upper - phis.begin()), (LUT.at((upper - phis.begin() + 1)%36)));
}

// ------------ method called when starting to processes a luminosity block  ------------
/*
void 
MuonGEMRecHits::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a luminosity block  ------------
/*
void 
MuonGEMRecHits::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
MuonGEMRecHits::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(MuonGEMRecHits);
