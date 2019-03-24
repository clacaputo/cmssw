/**
 *  Class: DynamicTruncation
 *
 *  Description:
 *  class for the dynamical stop of the KF according to the
 *  compatibility degree between the extrapolated track
 *  state and the reconstructed segment in the muon chambers
 *
 *  Authors :
 *  D. Pagano & G. Bruno - UCL Louvain
 *
 **/

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "RecoMuon/GlobalTrackingTools/interface/DynamicTruncation.h"
#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "TrackingTools/TrackFitters/interface/RecHitLessByDet.h"
#include "TrackingTools/Records/interface/TransientRecHitRecord.h"
#include "RecoMuon/DetLayers/interface/MuonDetLayerGeometry.h"
#include "RecoMuon/Records/interface/MuonRecoGeometryRecord.h"
#include "RecoMuon/Navigation/interface/MuonNavigationSchool.h"
#include "RecoMuon/Navigation/interface/MuonNavigationPrinter.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"
#include "RecoTracker/Record/interface/NavigationSchoolRecord.h"
#include "RecoMuon/TrackingTools/interface/MuonPatternRecoDumper.h"
#include "TrackingTools/KalmanUpdators/interface/Chi2MeasurementEstimator.h"
#include "TrackPropagation/SteppingHelixPropagator/interface/SteppingHelixPropagator.h"
#include "SimTracker/Records/interface/TrackAssociatorRecord.h"

#define MAX_THR 1e7

using namespace edm;
using namespace std;
using namespace reco;



DynamicTruncation::DynamicTruncation(const edm::Event& event, const MuonServiceProxy& theService) {
  propagator = theService.propagator("SmartPropagatorAny");
  propagatorPF = theService.propagator("SmartPropagatorAny");
  propagatorCompatibleDet = theService.propagator("SmartPropagatorAny");
  theG = theService.trackingGeometry();
  theService.eventSetup().get<TransientRecHitRecord>().get("MuonRecHitBuilder",theMuonRecHitBuilder);
  theService.eventSetup().get<TrackingComponentsRecord>().get("KFUpdator",updatorHandle);
  theService.eventSetup().get<MuonGeometryRecord>().get(cscGeom);
  theService.eventSetup().get<MuonRecoGeometryRecord>().get(navMuon);
  theService.eventSetup().get<IdealMagneticFieldRecord>().get(magfield);
  navigation = new DirectMuonNavigation(theService.detLayerGeometry());
  getSegs = new ChamberSegmentUtility();
  thrManager = new ThrParameters(&theService.eventSetup());
  useDBforThr = thrManager->isValidThdDB();
  if (useDBforThr) dytThresholds = thrManager->getInitialThresholds();
  doUpdateOfKFStates = true;
}

DynamicTruncation::~DynamicTruncation() {
  delete navigation;
  delete thrManager;
  delete getSegs;
}

void DynamicTruncation::update(TrajectoryStateOnSurface& tsos, ConstRecHitPointer rechit) {
  TrajectoryStateOnSurface temp = updatorHandle->update(tsos, *rechit);
  if (temp.isValid()) tsos = updatorHandle->update(tsos, *rechit);
}

void DynamicTruncation::updateWithDThits(TrajectoryStateOnSurface& tsos, DTRecSegment4D const &bestDTSeg) {
  ConstRecHitContainer tmprecHits;
  vector<const TrackingRecHit*> DTrh = bestDTSeg.recHits();
  for (vector<const TrackingRecHit*>::iterator it = DTrh.begin(); it != DTrh.end(); it++) {
    tmprecHits.push_back(theMuonRecHitBuilder->build(*it)); 
  }
  sort(tmprecHits);
  for (ConstRecHitContainer::const_iterator it = tmprecHits.begin(); it != tmprecHits.end(); ++it) {
    DTLayerId layid((*it)->det()->geographicalId());
    TrajectoryStateOnSurface temp = propagator->propagate(tsos, theG->idToDet(layid)->surface());
    if (temp.isValid()) {
        TrajectoryStateOnSurface tempTsos = updatorHandle->update(temp, **it);
        if (tempTsos.isValid() ) tsos = tempTsos;
    }
  }
}

void DynamicTruncation::updateWithCSChits(TrajectoryStateOnSurface& tsos, CSCSegment const &bestCSCSeg) {
  ConstRecHitContainer tmprecHits;
  vector<CSCRecHit2D> CSCrh = bestCSCSeg.specificRecHits();
  for (vector<CSCRecHit2D>::iterator it = CSCrh.begin(); it != CSCrh.end(); ++it) {
    tmprecHits.push_back(theMuonRecHitBuilder->build(&*it));
  }
  sort(tmprecHits);
  for (ConstRecHitContainer::const_iterator it = tmprecHits.begin(); it != tmprecHits.end(); ++it) {
    const CSCLayer* cscLayer = cscGeom->layer((*it)->det()->geographicalId());
    TrajectoryStateOnSurface temp = propagator->propagate(tsos, cscLayer->surface());  
    if (temp.isValid()) {
      TrajectoryStateOnSurface tempTsos = updatorHandle->update(temp, **it);
      if (tempTsos.isValid() ) tsos = tempTsos;
    }
  }
}


/////////////////////////////////
///// Configuration methods /////
/////////////////////////////////
void DynamicTruncation::setSelector(int selector) {
  if (selector < 0 || selector > 2) throw cms::Exception("NotAvailable") << "DYT selector: wrong option!" << endl;
  //if (selector == 0) cout << "[DYT disabled]\n";
  //if (selector == 1) cout << "[use all compatible stations]\n";
  //if (selector == 2) cout << "[stop at second consecutive incompatible station]\n";
  DYTselector = selector;

}

void DynamicTruncation::setUseAPE(bool useAPE_) {
  useAPE = useAPE_;
}

void DynamicTruncation::setUpdateState(bool upState) {
  doUpdateOfKFStates = upState;
}

void DynamicTruncation::setThr(const vector<int>& thr) {
  if (thr.size() == 2) {
    for (unsigned int i = 0; i < thr.size(); i++)
      if (thr[i] >= 0) Thrs.push_back(thr[i]);
      else Thrs.push_back(MAX_THR);
    return;
  }
  throw cms::Exception("NotAvailable") << "WARNING: wrong size for the threshold vector!\nExpected size: 2\n   Found size: " << thr.size();
}
/////////////////////////////////
/////////////////////////////////
/////////////////////////////////



//===> filter
TransientTrackingRecHit::ConstRecHitContainer DynamicTruncation::filter(const Trajectory& traj, double globalMomentum, double globalEta) {
  result.clear();
  prelFitMeas.clear();
  
  // Get APE maps 
  dtApeMap = thrManager->GetDTApeMap();
  cscApeMap = thrManager->GetCSCApeMap();

  double momentum = globalMomentum;
  double eta = globalEta;

  // Get Last tracker TSOS (updated)
  vector<TrajectoryMeasurement> muonMeasurements = traj.measurements();
  TrajectoryMeasurement lastTKm = muonMeasurements.front();
  for (vector<TrajectoryMeasurement>::const_iterator imT = muonMeasurements.begin(); imT != muonMeasurements.end(); imT++ ) {
    if ( !(*imT).recHit()->isValid() ) continue;
    const TransientTrackingRecHit* hit = &(*(*imT).recHit());
    if (hit->geographicalId().det() == DetId::Tracker) {
      result.push_back((*imT).recHit());
      if (!(*imT).forwardPredictedState().isValid()) continue;
      if ((*imT).forwardPredictedState().globalPosition().mag() >
	  lastTKm.forwardPredictedState().globalPosition().mag()) lastTKm = *imT;
    }
  }
  currentState = lastTKm.forwardPredictedState();
  update(currentState, lastTKm.recHit());

  prelFitState = lastTKm.forwardPredictedState();
  update(prelFitState, lastTKm.recHit());
  prelFitMeas = result;

  // Run the DYT
  filteringAlgo(momentum,eta); //filteringAlgo();
  
  return result;
}


//===> filteringAlgo
void DynamicTruncation::filteringAlgo(double momentum,double eta) {
  map<int, vector<DetId> > compatibleIds;
  map<int, vector<DTRecSegment4D> > dtSegMap;
  map<int, vector<CSCSegment> > cscSegMap;
  int incompConLay = 0;
  nStationsUsed = 0;

  p_reco = momentum;
  eta_reco = eta;

  // Get list of compatible layers
  compatibleDets(currentState, compatibleIds);

  // Fill segment maps
  fillSegmentMaps(compatibleIds, dtSegMap, cscSegMap); 

  // Do a preliminary fit
  if (useDBforThr) preliminaryFit(compatibleIds, dtSegMap, cscSegMap);

  // Loop on compatible layers
  for (map<int, vector<DetId> >::iterator it=compatibleIds.begin(); it!=compatibleIds.end(); ++it) {
    int stLayer = stationfromDet(it->second.front());
    DTRecSegment4D bestDTSeg;
    CSCSegment bestCSCSeg;
    double bestDTEstimator  = MAX_THR;
    double bestCSCEstimator = MAX_THR;
    vector<DTRecSegment4D> dtSegs   = dtSegMap[it->first];
    vector<CSCSegment> cscSegs      = cscSegMap[it->first];

    // DT case: find the most compatible segment 
    TrajectoryStateOnSurface tsosDTlayer;
    testDTstation(currentState, dtSegs, bestDTEstimator, bestDTSeg, tsosDTlayer);

    // CSC case: find the most compatible segment
    TrajectoryStateOnSurface tsosCSClayer;
    testCSCstation(currentState, cscSegs, bestCSCEstimator, bestCSCSeg, tsosCSClayer);

    // Decide whether to keep the layer or not
    bool chosenLayer = chooseLayers(incompConLay, bestDTEstimator, bestDTSeg, tsosDTlayer, bestCSCEstimator, bestCSCSeg, tsosCSClayer);
    fillDYTInfos(stLayer, chosenLayer, incompConLay, bestDTEstimator, bestCSCEstimator, bestDTSeg, bestCSCSeg); 
  }
  //cout << "Number of used stations = " << nStationsUsed << endl;
}


//===> stationfromDet
int DynamicTruncation::stationfromDet(DetId const& det) {
  if (det.subdetId() == MuonSubdetId::CSC) {
    CSCDetId ch(det);
    return ch.station();
  }
  if (det.subdetId() == MuonSubdetId::DT) {
    DTChamberId ch(det);
    return ch.station();
  }
  return 0;
}


//===> fillDYTInfos
void DynamicTruncation::fillDYTInfos(int const &st, bool const &chosenLayer, int &incompConLay, 
				     double const &bestDTEstimator, double const &bestCSCEstimator,
				     DTRecSegment4D const &bestDTSeg, CSCSegment const &bestCSCSeg) {
  if (chosenLayer) {
    nStationsUsed++;
    incompConLay = 0;
    if (bestDTEstimator <= bestCSCEstimator) {
      estimatorMap[st] = bestDTEstimator;
      DetId id(bestDTSeg.chamberId()); 
      idChamberMap[st] = id;
    } else {
      DetId id(bestCSCSeg.cscDetId());
      idChamberMap[st] = id;
      estimatorMap[st] = bestCSCEstimator;
    }
    usedStationMap[st] = true;
  } else {
    incompConLay++;
    estimatorMap[st] = -1;
    usedStationMap[st] = false;
  }
}


//===> compatibleDets
void DynamicTruncation::compatibleDets(TrajectoryStateOnSurface &tsos, map<int, vector<DetId> > &detMap) {
  MuonPatternRecoDumper dumper;
  MeasurementEstimator *theEstimator = new Chi2MeasurementEstimator(1000, 1000);
  vector<const DetLayer *> navLayers;
  navLayers = navigation->compatibleLayers(*(currentState.freeState()), alongMomentum);
  unsigned int ilayerCorrected = 0;
  for ( unsigned int ilayer=0; ilayer<navLayers.size(); ilayer++ ) {
    // Skip RPC layers
    if (navLayers[ilayer]->subDetector() != GeomDetEnumerators::DT &&
	navLayers[ilayer]->subDetector() != GeomDetEnumerators::CSC) continue;
    ilayerCorrected++;
    vector<DetLayer::DetWithState> comps = navLayers[ilayer]->compatibleDets(currentState, *propagatorCompatibleDet, *theEstimator);
    //cout << comps.size() << " compatible Dets with " << navLayers[ilayer]->subDetector() << " Layer " << ilayer << " " 
    //<< dumper.dumpLayer(navLayers[ilayer]);
    if (!comps.empty()) {
      for ( unsigned int icomp=0; icomp<comps.size(); icomp++ ) {
		DetId id(comps[icomp].first->geographicalId().rawId());
		detMap[ilayerCorrected].push_back(id);
      }
    }
  }
  if (theEstimator) delete theEstimator;
}


//===> fillSegmentMaps
void DynamicTruncation::fillSegmentMaps( map<int, vector<DetId> > &compatibleIds,
                                         map<int, vector<DTRecSegment4D> > &dtSegMap,
                                         map<int, vector<CSCSegment> > &cscSegMap) {
  for (map<int, vector<DetId> >::iterator it=compatibleIds.begin(); it!=compatibleIds.end(); ++it) {
    vector<DetId> ids = compatibleIds[it->first];
    for (unsigned j = 0; j < ids.size(); j++) {
      if (ids[j].subdetId() == MuonSubdetId::CSC) {
        CSCDetId ch(ids[j]);
        vector<CSCSegment> tmp = getSegs->getCSCSegmentsInChamber(ch);
        for (unsigned int k = 0; k < tmp.size(); k++) cscSegMap[it->first].push_back(tmp[k]);
      }
      if (ids[j].subdetId() == MuonSubdetId::DT) {
        DTChamberId ch(ids[j]);
        vector<DTRecSegment4D> tmp = getSegs->getDTSegmentsInChamber(ch);
        for (unsigned int k = 0; k < tmp.size(); k++) dtSegMap[it->first].push_back(tmp[k]);
      }
    }
  }
}


//===> testDTstation
void DynamicTruncation::testDTstation(TrajectoryStateOnSurface &startingState, vector<DTRecSegment4D> const &segments, 
				      double &bestEstimator, DTRecSegment4D &bestSeg, TrajectoryStateOnSurface &tsosdt) {
  if (segments.empty()) return;
  for (unsigned int iSeg = 0; iSeg < segments.size(); iSeg++) {
    DTChamberId chamber(segments[iSeg].chamberId());
    if (!propagator->propagate(startingState, theG->idToDet(chamber)->surface()).isValid()) continue;
    tsosdt = propagator->propagate(startingState, theG->idToDet(chamber)->surface());
    //if (!tsosdt.isValid()) continue;
    LocalError apeLoc;
    if (useAPE) apeLoc = ErrorFrameTransformer().transform(dtApeMap.find(chamber)->second, theG->idToDet(chamber)->surface());
    StateSegmentMatcher estim(tsosdt, segments[iSeg], apeLoc);
    double estimator = estim.value();      
    //cout << "estimator DT = " << estimator << endl;
    if (estimator >= bestEstimator) continue; 
    bestEstimator = estimator;
    bestSeg = segments[iSeg];
  }
}


//===> testCSCstation
void DynamicTruncation::testCSCstation(TrajectoryStateOnSurface &startingState, vector<CSCSegment> const &segments, 
				       double &bestEstimator, CSCSegment &bestSeg, TrajectoryStateOnSurface &tsoscsc) {
  if (segments.empty()) return;
  for (unsigned int iSeg = 0; iSeg < segments.size(); iSeg++) {
    CSCDetId chamber(segments[iSeg].cscDetId());
    if (!propagator->propagate(startingState, theG->idToDet(chamber)->surface()).isValid()) continue;
    tsoscsc = propagator->propagate(startingState, theG->idToDet(chamber)->surface());
    //if (!tsoscsc.isValid()) continue;
    LocalError apeLoc;
    if (useAPE) apeLoc = ErrorFrameTransformer().transform(cscApeMap.find(chamber)->second, theG->idToDet(chamber)->surface());
    StateSegmentMatcher estim(tsoscsc, segments[iSeg], apeLoc);
    double estimator = estim.value();
    //cout << "estimator CSC = " << estimator << endl;
    if (estimator >= bestEstimator) continue;
    bestEstimator = estimator;
    bestSeg = segments[iSeg]; 
  }
}


//===> useSegment
void DynamicTruncation::useSegment(DTRecSegment4D const &bestDTSeg, TrajectoryStateOnSurface const &tsosDT) {
  result.push_back(theMuonRecHitBuilder->build(&bestDTSeg));
  if (doUpdateOfKFStates) updateWithDThits(currentState, bestDTSeg);
  else currentState = tsosDT;
}


//===> useSegment 
void DynamicTruncation::useSegment(CSCSegment const &bestCSCSeg, TrajectoryStateOnSurface const &tsosCSC) {
  result.push_back(theMuonRecHitBuilder->build(&bestCSCSeg));
  if (doUpdateOfKFStates) updateWithCSChits(currentState, bestCSCSeg);
  else currentState = tsosCSC;
}


//===> preliminaryFit
void DynamicTruncation::preliminaryFit(map<int, vector<DetId> > compatibleIds, map<int, vector<DTRecSegment4D> > dtSegMap,
				       map<int, vector<CSCSegment> > cscSegMap) {
  for (map<int, vector<DetId> >::iterator it=compatibleIds.begin(); it!=compatibleIds.end(); ++it) {
    DTRecSegment4D bestDTSeg;
    CSCSegment bestCSCSeg;
    double bestDTEstimator  = MAX_THR;
    double bestCSCEstimator = MAX_THR;
    double initThr = MAX_THR;
    vector<DTRecSegment4D> dtSegs = dtSegMap[it->first];
    vector<CSCSegment> cscSegs    = cscSegMap[it->first];

    // DT case: find the most compatible segment
    TrajectoryStateOnSurface tsosDTlayer;
    testDTstation(prelFitState, dtSegs, bestDTEstimator, bestDTSeg, tsosDTlayer);

    // CSC case: find the most compatible segment
    TrajectoryStateOnSurface tsosCSClayer;
    testCSCstation(prelFitState, cscSegs, bestCSCEstimator, bestCSCSeg, tsosCSClayer);

    // Decide whether to keep the layer or not
    if (bestDTEstimator == MAX_THR && bestCSCEstimator == MAX_THR) continue;
    if (bestDTEstimator <= bestCSCEstimator) {
      getThresholdFromCFG(initThr, DetId(bestDTSeg.chamberId()));
      if (bestDTEstimator >= initThr) continue;
      prelFitMeas.push_back(theMuonRecHitBuilder->build(&bestDTSeg));
      auto aSegRH = prelFitMeas.back();
      auto uRes = updatorHandle->update(tsosDTlayer, *aSegRH);
      if (uRes.isValid()){
	prelFitState = uRes;
      } else {
	prelFitMeas.pop_back();
      }
    } else {
      getThresholdFromCFG(initThr, DetId(bestCSCSeg.cscDetId()));
      if (bestCSCEstimator >= initThr) continue;
      prelFitMeas.push_back(theMuonRecHitBuilder->build(&bestCSCSeg));
      auto aSegRH = prelFitMeas.back();
      auto uRes = updatorHandle->update(tsosCSClayer, *aSegRH);
      if (uRes.isValid()){
	prelFitState = uRes;
      } else {
	prelFitMeas.pop_back();
      }
    }
  }
  if (!prelFitMeas.empty()) prelFitMeas.pop_back();
  for (auto imrh = prelFitMeas.rbegin(); imrh != prelFitMeas.rend(); ++imrh) {
    DetId id = (*imrh)->geographicalId(); 
    TrajectoryStateOnSurface tmp = propagatorPF->propagate(prelFitState, theG->idToDet(id)->surface());
    if (tmp.isValid()) prelFitState = tmp; 
  }
  muonPTest  = prelFitState.globalMomentum().perp();
  muonETAest = prelFitState.globalMomentum().eta();
}


//===> chooseLayers
bool DynamicTruncation::chooseLayers(int &incompLayers, double const &bestDTEstimator, DTRecSegment4D const &bestDTSeg, TrajectoryStateOnSurface const &tsosDT, 
				     double const &bestCSCEstimator, CSCSegment const &bestCSCSeg, TrajectoryStateOnSurface const &tsosCSC) {
  double initThr = MAX_THR;
  if (bestDTEstimator == MAX_THR && bestCSCEstimator == MAX_THR) return false;
  if (bestDTEstimator <= bestCSCEstimator) {
    // Get threshold for the chamber
    if (useDBforThr) getThresholdFromDB(initThr, DetId(bestDTSeg.chamberId()));
    else getThresholdFromCFG(initThr, DetId(bestDTSeg.chamberId())); 
    if (DYTselector == 0 || (DYTselector == 1 && bestDTEstimator < initThr) ||
	(DYTselector == 2 && incompLayers < 2 && bestDTEstimator < initThr)) {
      useSegment(bestDTSeg, tsosDT); 
      return true;
    }
  } else {
    // Get threshold for the chamber
    if (useDBforThr) getThresholdFromDB(initThr, DetId(bestCSCSeg.cscDetId()));
    else getThresholdFromCFG(initThr, DetId(bestCSCSeg.cscDetId()));
    if (DYTselector == 0 || (DYTselector == 1 && bestCSCEstimator < initThr) ||
	(DYTselector == 2 && incompLayers < 2 && bestCSCEstimator < initThr)) {
      useSegment(bestCSCSeg, tsosCSC);
      return true;
    }
  }
  return false;
}


//===> getThresholdFromDB
void DynamicTruncation::getThresholdFromDB(double& thr, DetId const& id) {
  vector<DYTThrObject::DytThrStruct> thrvector = dytThresholds->thrsVec;
  for (vector<DYTThrObject::DytThrStruct>::const_iterator it = thrvector.begin(); it != thrvector.end(); it++) {
    DYTThrObject::DytThrStruct obj = (*it);
    if (obj.id == id) {
      thr = obj.thr;
      break;
    }
  }
  correctThrByPtAndEta(thr);
}


//===> correctThrByPtAndEta
void DynamicTruncation::correctThrByPtAndEta(double& thr) {

	//double p = p_reco;
	//double   p08[2] = { -0.919853, 0.990742};
	//double   p12[4] = { -0.897354, 0.987738};
	//double   p20[3] = { -0.986855, 0.998516};
	//double   p22[2] = { -0.940342, 0.992955};
	//double   p24[2] = { -0.947633, 0.993762};
	//double thr50[5] = { 1, 1, 4, 1, 1};

	if( TMath::Abs(eta_reco) > 0 && TMath::Abs(eta_reco) <= 0.8 ){
		thr = 24; //3000-> 26; //400-> 11; //1000-> 19; //2000-> 25;
		//thr = thr50[0] * ( 1 + p08[0]*p + TMath::Power( p, p08[1])); 	
	} else if( TMath::Abs(eta_reco) > 0.8 && TMath::Abs(eta_reco) <= 1.2 ){
		thr = 38; //3000-> 29; //400-> 14; //1000-> 22; //2000-> 27;
                //thr = thr50[1] * ( 1 + p12[0]*p + TMath::Power( p, p12[1])); 
        } else if( TMath::Abs(eta_reco) > 1.2 && TMath::Abs(eta_reco) <= 2.0 ){
		thr = 19; //3000-> 20; //400-> 10; //1000-> 16; //2000-> 19;
        	//thr = thr50[2] * ( 1 + p20[0]*p + TMath::Power( p, p20[1])); 
        } else if( TMath::Abs(eta_reco) > 2.0 && TMath::Abs(eta_reco) <= 2.2 ){
		thr = 16; //3000-> 16; //400-> 8; //1000-> 13; //2000-> 15;
        	//thr = thr50[3] * ( 1 + p22[0]*p + TMath::Power( p, p22[1]));
        } else if( TMath::Abs(eta_reco) > 2.2 && TMath::Abs(eta_reco) <= 2.4 ){
		thr = 14; //3000-> 12; //400-> 8; //1000-> 11; //2000-> 13;
        	//thr = thr50[4] * ( 1 + p24[0]*p + TMath::Power( p, p24[1]));
	}
	//thr = 10;

}


//===> getThresholdFromCFG
void DynamicTruncation::getThresholdFromCFG(double& thr, DetId const& id) {
  if (id.subdetId() == MuonSubdetId::DT) {
    thr = Thrs[0];
  }
  if (id.subdetId() == MuonSubdetId::CSC) {
    thr = Thrs[1];
  }
}


//===> sort
void DynamicTruncation::sort(ConstRecHitContainer& recHits) {
  unsigned int i=0;
  unsigned int j=0;
  ConstRecHitContainer::size_type n = recHits.size();
  for(i=1; i<n; ++i)
    for(j=n-1; j>=i; --j)
      if(recHits[j-1]->globalPosition().mag() > recHits[j]->globalPosition().mag()) swap (recHits[j-1],recHits[j]);
}
