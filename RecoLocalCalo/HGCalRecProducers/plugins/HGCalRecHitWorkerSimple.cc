#include "RecoLocalCalo/HGCalRecProducers/plugins/HGCalRecHitWorkerSimple.h"
#include "DataFormats/ForwardDetId/interface/HGCEEDetId.h"
#include "DataFormats/ForwardDetId/interface/HGCHEDetId.h"
#include "DataFormats/ForwardDetId/interface/HGCalDetId.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "CommonTools/Utils/interface/StringToEnumValue.h"

HGCalRecHitWorkerSimple::HGCalRecHitWorkerSimple(const edm::ParameterSet&ps) :
        HGCalRecHitWorkerBaseClass(ps)
{
        rechitMaker_ = new HGCalRecHitSimpleAlgo();
	//        v_chstatus_ = ps.getParameter<std::vector<int> >("ChannelStatusToBeExcluded");
	//	v_DB_reco_flags_ = ps.getParameter<std::vector<int> >("flagsMapDBReco");
	//        killDeadChannels_ = ps.getParameter<bool>("killDeadChannels");
	// uncomment at more advanced simulation or data

}



void HGCalRecHitWorkerSimple::set(const edm::EventSetup& es)
{
  //        es.get<HGCalIntercalibConstantsRcd>().get(ical);
  //        es.get<HGCalTimeCalibConstantsRcd>().get(itime);
  //        es.get<HGCalTimeOffsetConstantRcd>().get(offtime);
  //        es.get<HGCalADCToGeVConstantRcd>().get(agc);
  //        es.get<HGCalChannelStatusRcd>().get(chStatus);
	// uncomment at more advanced simulation or data
}


bool
HGCalRecHitWorkerSimple::run( const edm::Event & evt,
                const HGCUncalibratedRecHit& uncalibRH,
                HGCRecHitCollection & result )
{
        DetId detid=uncalibRH.id();

        uint32_t recoFlag = 0;


	//	float offsetTime = 0; // the global time phase

        if ( detid.subdetId() == HGCEE ) {
	  //                rechitMaker_->setADCToGeVConstant( float(agc->getEEValue()) );
	  //		offsetTime = offtime->getEEValue();
        } else if ( detid.subdetId() == HGCHEF ) {
	  //                rechitMaker_->setADCToGeVConstant( float(agc->getHEFValue()) );
	  //		offsetTime = offtime->getHEFValue();
	}else {
          //      rechitMaker_->setADCToGeVConstant( float(agc->getHEBValue()) );
	  //	offsetTime = offtime->getHEBValue();
        }
	 
        // make the rechit and put in the output collection
	if (recoFlag == 0) {
          HGCRecHit myrechit( rechitMaker_->makeRecHit(uncalibRH, /*recoflags_*/ 0) );	

	  result.push_back(myrechit);
	}

        return true;
}

HGCalRecHitWorkerSimple::~HGCalRecHitWorkerSimple(){

  delete rechitMaker_;
}


#include "FWCore/Framework/interface/MakerMacros.h"
#include "RecoLocalCalo/HGCalRecProducers/interface/HGCalRecHitWorkerFactory.h"
DEFINE_EDM_PLUGIN( HGCalRecHitWorkerFactory, HGCalRecHitWorkerSimple, "HGCalRecHitWorkerSimple" );
