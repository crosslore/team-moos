/************************************************************/
/*    NAME: Brian Stanfield                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_AvoidBoundary.cpp                                    */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include <cstdlib>
#include "MBUtils.h"
#include "BuildUtils.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"
#include "BHV_AvoidBoundary.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

BHV_AvoidBoundary::BHV_AvoidBoundary(IvPDomain domain) :
  IvPBehavior(domain)
{
  // Provide a default behavior name
  IvPBehavior::setParam("name", "defaultname");

  // Declare the behavior decision space
  m_domain = subDomain(m_domain, "course,speed");

  // Add any variables this behavior needs to subscribe for
  addInfoVars("NAV_X, NAV_Y");
}

//---------------------------------------------------------------
// Procedure: setParam()

bool BHV_AvoidBoundary::setParam(string param, string val)
{
  // Convert the parameter to lower case for more general matching
  param = tolower(param);

  // Get the numerical value of the param argument for convenience once
  double double_val = atof(val.c_str());
  
  if((param == "foo") && isNumber(val)) {
    // Set local member variables here
    return(true);
  }
  else if (param == "bar") {
    // return(setBooleanOnString(m_my_bool, val));
  }

  // If not handled above, then just return false;
  return(false);
}

//---------------------------------------------------------------
// Procedure: onSetParamComplete()
//   Purpose: Invoked once after all parameters have been handled.
//            Good place to ensure all required params have are set.
//            Or any inter-param relationships like a<b.

void BHV_AvoidBoundary::onSetParamComplete()
{
}

//---------------------------------------------------------------
// Procedure: onHelmStart()
//   Purpose: Invoked once upon helm start, even if this behavior
//            is a template and not spawned at startup

void BHV_AvoidBoundary::onHelmStart()
{
}

//---------------------------------------------------------------
// Procedure: onIdleState()
//   Purpose: Invoked on each helm iteration if conditions not met.

void BHV_AvoidBoundary::onIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onCompleteState()

void BHV_AvoidBoundary::onCompleteState()
{
}

//---------------------------------------------------------------
// Procedure: postConfigStatus()
//   Purpose: Invoked each time a param is dynamically changed

void BHV_AvoidBoundary::postConfigStatus()
{
}

//---------------------------------------------------------------
// Procedure: onIdleToRunState()
//   Purpose: Invoked once upon each transition from idle to run state

void BHV_AvoidBoundary::onIdleToRunState()
{
}

//---------------------------------------------------------------
// Procedure: onRunToIdleState()
//   Purpose: Invoked once upon each transition from run to idle state

void BHV_AvoidBoundary::onRunToIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onRunState()
//   Purpose: Invoked each iteration when run conditions have been met.

IvPFunction* BHV_AvoidBoundary::onRunState()
{
  bool ok1, ok2, ok3;
  IvPFunction *ipf = 0;

//obtain O/S X,Y, and desired heading
  m_osx = getBufferDoubleVal("NAV_X", ok1);
  m_osy = getBufferDoubleVal("NAV_Y", ok2);

  if(!ok1 || !ok2) {
    postWMessage("No ownship X/Y info in info_buffer.");
    return(0);
  }

  double buffer = 20;

//logic statement to determine if a boundary is approached and turn ship
//around.. additionally it sets the general easterly or westerly search
//pattern
  bool change_course = false;
  if(m_osx > 180 - buffer){
    m_heading_desired = 270;
    change_course = true;
  }
  if((m_osx < 100) && (m_osx > -50) && (m_osy > (2.0/5.0 * m_osx - 20.0 - buffer))){
    m_heading_desired = 180;
    change_course = true;
  }
  if((m_osx < -50) && (m_osy > 7.0/10.0 * m_osx - 5.0 - buffer)){
    m_heading_desired = 90;
    change_course = true;
  }
  if((m_osx < -50) && (m_osy < -5.0/2.0 * m_osx - 325.0 + buffer)){
    m_heading_desired = 90;
    change_course = true;
   }

  m_speed_desired = 0.8;
  if(change_course)
    ipf = buildFunctionWithZAIC();
  // Part N: Prior to returning the IvP function, apply the priority wt
  // Actual weight applied may be some value different than the configured
  // m_priority_wt, depending on the behavior author's insite.
  if(ipf)
    ipf->setPWT(m_priority_wt);

  return(ipf);
}

IvPFunction *BHV_AvoidBoundary::buildFunctionWithZAIC() 
{
  ZAIC_PEAK spd_zaic(m_domain, "speed");
  spd_zaic.setSummit(m_speed_desired);
  spd_zaic.setPeakWidth(0.5);
  spd_zaic.setBaseWidth(1.0);
  spd_zaic.setSummitDelta(0.8);  
  if(spd_zaic.stateOK() == false) {
    string warnings = "Speed ZAIC problems " + spd_zaic.getWarnings();
    postWMessage(warnings);
    return(0);
  }
  ZAIC_PEAK crs_zaic(m_domain, "course");
  crs_zaic.setSummit(m_heading_desired);
  crs_zaic.setPeakWidth(0);
  crs_zaic.setBaseWidth(180.0);
  crs_zaic.setSummitDelta(0);  
  crs_zaic.setValueWrap(true);
  if(crs_zaic.stateOK() == false) {
    string warnings = "Course ZAIC problems " + crs_zaic.getWarnings();
    postWMessage(warnings);
    return(0);
  }
  IvPFunction *spd_ipf = spd_zaic.extractIvPFunction();
  IvPFunction *crs_ipf = crs_zaic.extractIvPFunction();
  OF_Coupler coupler;
  IvPFunction *ivp_function = coupler.couple(crs_ipf, spd_ipf, 50, 50);
  return(ivp_function);
}