/************************************************************/
/*    NAME: Brian Stanfield                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_FindTempFront.cpp                                    */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include <list>
#include <cstdlib>
#include "MBUtils.h"
#include "BuildUtils.h"
#include <cmath>
#include "OF_Coupler.h"
#include "BHV_FindTempFront.h"
#include "ZAIC_PEAK.h"


using namespace std;

//---------------------------------------------------------------
// Constructor

  IvPFunction *ipf = 0;
  bool temp_report = false;

BHV_FindTempFront::BHV_FindTempFront(IvPDomain domain) :
  IvPBehavior(domain)
{
  m_th = 0;
  m_tc = 9999;
  m_heading_desired = 0;
  m_change_course = false;
  m_top_hot = false;
  m_mid_heading = 90;
  m_survey_start = false;


  // Provide a default behavior name
  IvPBehavior::setParam("name", "defaultname");

  // Declare the behavior decision space
  m_domain = subDomain(m_domain, "course,speed");

  // Add any variables this behavior needs to subscribe for
  addInfoVars("NAV_X, NAV_Y, DESIRED_HEADING");
  addInfoVars("UCTD_MSMNT_REPORT","no_warning");

}

//---------------------------------------------------------------
// Procedure: setParam()

bool BHV_FindTempFront::setParam(string param, string val)
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

void BHV_FindTempFront::onSetParamComplete()
{
}

//---------------------------------------------------------------
// Procedure: onHelmStart()
//   Purpose: Invoked once upon helm start, even if this behavior
//            is a template and not spawned at startup

void BHV_FindTempFront::onHelmStart()
{
}

//---------------------------------------------------------------
// Procedure: onIdleState()
//   Purpose: Invoked on each helm iteration if conditions not met.

void BHV_FindTempFront::onIdleState()
{
  bool ok1, ok2, ok3, ok4;
  m_osx = getBufferDoubleVal("NAV_X", ok1);
  if(m_osx > 160)
    m_mid_heading = 270;
  if(m_osx < -30)
    m_mid_heading = 90;;
  m_osy = getBufferDoubleVal("NAV_Y", ok2);
  m_curr_heading = getBufferDoubleVal("DESIRED_HEADING",ok4);
  if(!ok1 || !ok2) {
    postWMessage("No ownship X/Y info in info_buffer.");
  }
  m_msmnt_report = getBufferStringVal("UCTD_MSMNT_REPORT", ok3);
  if(!ok3) {
    postWMessage("No temperature info in info_buffer.");
  }
  double new_temp = stod(tokStringParse(m_msmnt_report,"temp",',','='));
  double temp_x = stod(tokStringParse(m_msmnt_report,"x",',','='));
  double temp_y = stod(tokStringParse(m_msmnt_report,"y",',','='));
  m_temps.push_back(new_temp);

  list<double>::iterator it;
 
  for(it = m_temps.begin(); it!= m_temps.end(); ++it){
  double CurrTemp = *it;
    if(CurrTemp > m_th){
      m_th = CurrTemp;
      m_th_y = temp_y;
    }
    if(CurrTemp < m_tc){
      m_tc = CurrTemp;
      m_tc_y = temp_y; 
    }
  }
}

//---------------------------------------------------------------
// Procedure: onCompleteState()

void BHV_FindTempFront::onCompleteState()
{
}

//---------------------------------------------------------------
// Procedure: postConfigStatus()
//   Purpose: Invoked each time a param is dynamically changed

void BHV_FindTempFront::postConfigStatus()
{
}

//---------------------------------------------------------------
// Procedure: onIdleToRunState()
//   Purpose: Invoked once upon each transition from idle to run state

void BHV_FindTempFront::onIdleToRunState()
{
}

//---------------------------------------------------------------
// Procedure: onRunToIdleState()
//   Purpose: Invoked once upon each transition from run to idle state

void BHV_FindTempFront::onRunToIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onRunState()
//   Purpose: Invoked each iteration when run conditions have been met.

IvPFunction* BHV_FindTempFront::onRunState()
{
  bool ok1, ok2, ok3, ok4;
  

//Publishes North and South Temperatures to Front Estimate to reduce bounds for annealing.
if(!temp_report){
  postMessage("TEMP_NORTH",to_string(m_tc));
  postMessage("TEMP_SOUTH",to_string(m_th));
  temp_report = true;
}


  m_osx = getBufferDoubleVal("NAV_X", ok1);
  // if(m_osx > 160)
  //   m_mid_heading = 270;
  // if(m_osx < -50)
  //   m_mid_heading = 90;
  m_osy = getBufferDoubleVal("NAV_Y", ok2);
  m_curr_heading = getBufferDoubleVal("DESIRED_HEADING",ok4);
  if(!ok1 || !ok2) {
    postWMessage("No ownship X/Y info in info_buffer.");
    return(0);
  }
  m_msmnt_report = getBufferStringVal("UCTD_MSMNT_REPORT", ok3);
  if(!ok3) {
    postWMessage("No temperature info in info_buffer.");
    return(0);
  }
  double new_temp = stod(tokStringParse(m_msmnt_report,"temp",',','='));
  double temp_x = stod(tokStringParse(m_msmnt_report,"x",',','='));
  double temp_y = stod(tokStringParse(m_msmnt_report,"y",',','='));
  m_temps.push_back(new_temp);

  list<double>::iterator it;
 
  for(it = m_temps.begin(); it!= m_temps.end(); ++it){
  double CurrTemp = *it;
    if(CurrTemp > m_th){
      m_th = CurrTemp;
      m_th_y = temp_y;
    }
    if(CurrTemp < m_tc){
      m_tc = CurrTemp;
      m_tc_y = temp_y; 
    }
  }

  if (m_th_y > m_tc_y)
    m_top_hot = true;
  else
    m_top_hot = false;
  
  m_top_hot = false;
 // m_th = 28;
 // m_tc = 16;

  double t_ave = (m_th - m_tc)/2;
  double t_turn = (m_th - m_tc) * 0.70;
  double m_curr_time = getBufferCurrTime();

  if(m_change_course && m_curr_time - m_course_time > 0.5){
    m_change_course = false;
  }


  if (!m_change_course && m_curr_heading == 0){
    // if(m_top_hot && new_temp > m_th - t_turn){
    //   m_heading_desired = m_mid_heading;
    //   m_change_course = true;
    //   m_course_time = getBufferCurrTime();
    // }
    if(!m_top_hot && new_temp < m_tc + t_turn){
      m_heading_desired = m_mid_heading;
      m_change_course = true;
      m_course_time = getBufferCurrTime();
    }
  }   
  if (!m_change_course && m_curr_heading == 180){
    if(!m_top_hot && new_temp > m_th - t_turn){
      m_heading_desired = m_mid_heading;;
      m_change_course = true;
      m_course_time = getBufferCurrTime();
    }
    // if(m_top_hot && new_temp < m_tc + t_turn){
    //   m_heading_desired = m_mid_heading;;
    //   m_change_course = true;
    //   m_course_time = getBufferCurrTime();
    // }
  }
  if (!m_change_course && (m_curr_heading == 90 || m_curr_heading == 270)){
    // if(m_top_hot && new_temp > m_th - t_turn){
    //   m_heading_desired = 180;
    //   m_change_course = true;
    //   m_course_time = getBufferCurrTime();
    // }
    if(!m_top_hot && new_temp < m_tc + t_turn){
      m_heading_desired = 180;
      m_change_course = true;
      m_course_time = getBufferCurrTime();
    }
    if(!m_top_hot && new_temp > m_th - t_turn){
      m_heading_desired = 0;
      m_change_course = true;
      m_course_time = getBufferCurrTime();
    }
  }

  if (m_change_course && !m_survey_start){
    m_survey_start = true;
    postMessage("SURVEY","true");
  }

  int buffer = 25;

  if(m_osx > 180 - buffer){
    m_heading_desired = 270;
     m_mid_heading = 270;
    m_change_course = true;
  }
  if(m_osx < 100 && m_osx > -50 && m_osy > 2/5 * m_osx - 20 - buffer){
    m_heading_desired = 180;
    //m_mid_heading = 90;
    m_change_course = true;
  }
  if(m_osx < -50 && m_osy > 7/10*m_osx - 5 - buffer){
    m_heading_desired = 90;
    m_mid_heading = 90;
    m_change_course = true;
  }
  if(m_osx < -50 && m_osy > -5/2 * m_osx - 325 + buffer){
    m_change_course = true;
    m_mid_heading = 90;
    m_heading_desired = 90;
  }

      ipf = buildFunctionWithZAIC();
  
  // Part 1: Build the IvP function

  // Part N: Prior to returning the IvP function, apply the priority wt
  // Actual weight applied may be some value different than the configured
  // m_priority_wt, depending on the behavior author's insite.
  if(ipf)
    ipf->setPWT(m_priority_wt);
  return(ipf);
}

IvPFunction *BHV_FindTempFront::buildFunctionWithZAIC() 
{
  ZAIC_PEAK spd_zaic(m_domain, "speed");
  spd_zaic.setSummit(2.0);
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

