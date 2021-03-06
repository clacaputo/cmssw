/**
 * Author: Shahram Rahatlou, University of Rome & INFN
 * Created: 22 Feb 2006
 * $Id: EcalADCToGeVConstant.cc,v 1.3 2006/05/15 12:43:57 meridian Exp $
 **/

#include "CondFormats/EcalObjects/interface/EcalADCToGeVConstant.h"

// why don't put default values simply? Shervin
EcalADCToGeVConstant::EcalADCToGeVConstant() 
{
  EBvalue_=0.;
  EEvalue_=0.;
  //EKvalue_=0.;
}

// EcalADCToGeVConstant::EcalADCToGeVConstant(const float & EBvalue,const float & EEvalue, const float& EKvalue) {
//   EBvalue_ = EBvalue;
//   EEvalue_ = EEvalue;
//   EKvalue_ = EKvalue;
// }

EcalADCToGeVConstant::EcalADCToGeVConstant(const float & EBvalue,const float & EEvalue) {
  EBvalue_ = EBvalue;
  EEvalue_ = EEvalue;
  //  EKvalue_ = EEvalue;
}

EcalADCToGeVConstant::~EcalADCToGeVConstant() {

}
