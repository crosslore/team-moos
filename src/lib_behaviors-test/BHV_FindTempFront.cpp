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
#include <math.h>
#include <numeric>

using namespace std;

//---------------------------------------------------------------
// Constructor

// IvPFunction *ipf = 0;
bool temp_report = false;

BHV_FindTempFront::BHV_FindTempFront(IvPDomain domain) :
  IvPBehavior(domain)
{
  m_tave = 0;
  m_th = 0;
  m_tc = 9999;
  m_heading_desired = 0;
  m_change_course = false;
  m_top_hot = false;
  m_mid_heading = 90;
  m_survey_start = false;
  m_alpha = 0;
  max_amp = 0;
  min_amp = 0;
  amp = 0;
  m_alpha = 350;
  temp_last = 0;


  // Provide a default behavior name
  IvPBehavior::setParam("name", "defaultname");

  // Declare the behavior decision space
  m_domain = subDomain(m_domain, "course,speed");

  // Add any variables this behavior needs to subscribe for
  addInfoVars("NAV_X, NAV_Y, DESIRED_HEADING");
  addInfoVars("UCTD_MSMNT_REPORT","no_warning");
  addInfoVars("OPREG_ABSOLUTE_PERIM_DIST","no_warning");

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

void BHV_FindTempFront::refineTemps(double t_ave_new)
{
  if(t_ave_new < m_tave)
    return;
  m_tave = t_ave_new;
  m_th = round(m_tave + (m_tave - m_tc));
  postMessage("TEMP_SOUTH_NEW",to_string(m_th));
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

  //determine if a new max or min temperature exists.
  Temps Temp_New;
  Temp_New.m_temps = stod(tokStringParse(m_msmnt_report,"temp",',','='));
  if(temp_last == 0)
    temp_last = Temp_New.m_temps;
  if(Temp_New.m_temps - temp_last > max_delta){
    max_delta = Temp_New.m_temps - temp_last;
    double p_t_ave = (Temp_New.m_temps + temp_last)/2;
    refineTemps(p_t_ave);
  }
  Temp_New.m_x = stod(tokStringParse(m_msmnt_report,"x",',','='));
  Temp_New.m_y = stod(tokStringParse(m_msmnt_report,"y",',','='));
  
  temps_list.push_back(Temp_New);

  if(Temp_New.m_temps > m_th){
    m_th = ceil(Temp_New.m_temps);
  }
  if(Temp_New.m_temps < m_tc){
    m_tc = floor(Temp_New.m_temps);
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

void BHV_FindTempFront::calcAmplitude()
{
  std::list<Temps>::iterator it;
  for (it = ave_temps_list.begin(); it != ave_temps_list.end(); ++it){
    Temps Curr_Temp = *it;
    if(Curr_Temp.m_temps > m_tave + 0.05 * (m_th - m_tc))
      continue;
    if(Curr_Temp.m_temps < m_tave - 0.05 * (m_th - m_tc))
      continue;
    if(Curr_Temp.m_x < 35 && Curr_Temp.m_x > -15){
    double A = Curr_Temp.m_x - 0;
    double B = Curr_Temp.m_y - (a_one * A + a_zero);
    double C = 40;
    double D = a_one * C + a_zero - (a_one * A + a_zero);

    double dot = A * C + B * D; 
    double len_sq = C * C + D * D;
    double param = -1;

    if(len_sq !=0){
      param = dot / len_sq;
    }
    double xx, yy;
    
    xx = param * C;
    yy = a_one * A + a_zero + param * D;


    double dx = Curr_Temp.m_x - xx;
    double dy = Curr_Temp.m_y - yy;

    postMessage("A_DX",to_string(dx));
    postMessage("A_DY",to_string(dy));

    double uncorrected_amplitude = pow(dx * dx + dy * dy,0.5);

    if(Curr_Temp.m_x + Curr_Temp.m_y < xx + yy){
      uncorrected_amplitude = -uncorrected_amplitude;
    }

    double x_prime = sqrt((yy - a_zero) * (yy - a_zero) + xx * xx);
    if(xx<0)
      x_prime = -x_prime;

    Curr_Temp.m_amplitude = uncorrected_amplitude / exp(-x_prime/m_alpha);
    if(Curr_Temp.m_amplitude > max_amp){
      max_amp = Curr_Temp.m_amplitude;
      amp = (max_amp + abs(min_amp))/2;
    }
    if(Curr_Temp.m_amplitude < min_amp){
      min_amp = Curr_Temp.m_amplitude;
      amp = (max_amp + abs(min_amp))/2;
    }
  }
  }
}

void BHV_FindTempFront::findEstimates(double x, double y, double temp)
{

//give bounds for values used for linear regression
  double t_ave_high = m_tave  + 0.2 * (m_th - m_tc);
  double t_ave_low = m_tave - 0.2 * (m_th - m_tc);

//if temperature within allowance band, perform a linear regression
  if ((temp > t_ave_low) && (temp < t_ave_high)){
    Temps New_Temp;
    New_Temp.m_x = x;
    New_Temp.m_y = y;
    New_Temp.m_temps = temp;
    ave_temps_list.push_back(New_Temp);
    calcAmplitude();

    m_x_ave.push_back(x);
    m_y_ave.push_back(y);
    m_xy_ave.push_back(x*y);
    m_x_square_ave.push_back(x*x);

    //summations used for linear regression calculation
    double S_x = std::accumulate(std::begin(m_x_ave), std::end(m_x_ave), 0.0);
    double S_y = std::accumulate(std::begin(m_y_ave), std::end(m_y_ave), 0.0);
    double S_xy = std::accumulate(std::begin(m_xy_ave), std::end(m_xy_ave), 0.0);
    double S_xx = std::accumulate(std::begin(m_x_square_ave), std::end(m_x_square_ave), 0.0);

    double n = m_x_ave.size();

    // solution in form of y = a1 * x + a0
    a_one = (n * S_xy - S_x * S_y) / (n * S_xx - S_x * S_x);
    a_zero = (S_xx * S_y - S_xy * S_x) / (n * S_xx - S_x * S_x);

    // convert a1 from radians to degrees
    m_angle = atan(a_one) * 180/M_PI;

    // rotate angle as needed for graphing on pMarine Viewer
    double angle_drawn = 90 - m_angle;

    string s = "x=0,y="+to_string(a_zero)+",mag=100,ang="+to_string(angle_drawn)+",label=one,edge_color=red";  
    string s2 = "x=0,y="+to_string(a_zero)+",mag="+to_string(amp) + ",ang="+to_string(-m_angle)+",label=amp,edge_color=red";
    //draw vector of guess and solution
    postMessage("VIEW_VECTOR",s);
    postMessage("VIEW_VECTOR","x=0,y=-78,mag=100,ang=105,label=truth");
    postMessage("VIEW_VECTOR",s2);
    postMessage("VIEW_VECTOR","x=0,y=-78,mag=34,ang=15,label=true_amplitude");


 
  }
}


//---------------------------------------------------------------
// Procedure: onRunState()
//   Purpose: Invoked each iteration when run conditions have been met.

IvPFunction* BHV_FindTempFront::onRunState()
{
  bool ok1, ok2, ok3, ok4;
  IvPFunction *ipf = 0;

//Publishes North and South Temperatures to Front Estimate to reduce bounds 
//for annealing once FindTempFront is in the run state.
if(!temp_report){
  postMessage("TEMP_NORTH",to_string(m_tc));
  postMessage("TEMP_SOUTH",to_string(m_th));
  temp_report = true;
}

//obtain O/S X,Y, and desired heading
  m_osx = getBufferDoubleVal("NAV_X", ok1);
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

  //determine temperature at location
  Temps Temp_New;
  Temp_New.m_temps = stod(tokStringParse(m_msmnt_report,"temp",',','='));
  Temp_New.m_x = stod(tokStringParse(m_msmnt_report,"x",',','='));
  Temp_New.m_y = stod(tokStringParse(m_msmnt_report,"y",',','='));

//checks temperature gradient. refine t_ave, t_h, t_c
  if(temp_last ==0)
    temp_last = Temp_New.m_temps;
  if(Temp_New.m_temps - temp_last > max_delta){
    max_delta = Temp_New.m_temps - temp_last;
    double p_t_ave = (temp_last + Temp_New.m_temps)/ 2;
    refineTemps(p_t_ave);
  }

//only start linear regression solver if survey has started
  if(m_survey_start){
   findEstimates(Temp_New.m_x,Temp_New.m_y,Temp_New.m_temps); 
  }

  //determine if new temp is max or min and store
  if(Temp_New.m_temps > m_th){
    m_th = ceil(Temp_New.m_temps);
  }
  if(Temp_New.m_temps < m_tc){
    m_tc = floor(Temp_New.m_temps);
  }

  m_tave = (m_th + m_tc)/2;
  double t_turn = (m_th - m_tc) * 0.70;
  double m_curr_time = getBufferCurrTime();

//adjust how often a course change can be applied
  if(m_change_course && m_curr_time - m_course_time > 0.5){
    m_change_course = false;
  }
//logic statements used to determine if a new desired heading
//is desired and apply that course change
  if (!m_change_course && m_curr_heading == 0){
    if(Temp_New.m_temps < m_tc + t_turn){
      m_heading_desired = m_mid_heading;
      m_change_course = true;
      m_course_time = getBufferCurrTime();
    }
  }   
  if (!m_change_course && m_curr_heading == 180){
    if(Temp_New.m_temps > m_th - t_turn){
      m_heading_desired = m_mid_heading;;
      m_change_course = true;
      m_course_time = getBufferCurrTime();
    }
  }
  if (!m_change_course && (m_curr_heading == 90 || m_curr_heading == 270)){
    if(Temp_New.m_temps < m_tc + t_turn){
      m_heading_desired = 180;
      m_change_course = true;
      m_course_time = getBufferCurrTime();
    }
    if(Temp_New.m_temps > m_th - t_turn){
      m_heading_desired = 0;
      m_change_course = true;
      m_course_time = getBufferCurrTime();
    }
  }

  if (m_change_course && !m_survey_start){
    m_survey_start = true;
    postMessage("SURVEY","true");
  }

//buffer for approaching boundary  
  double buffer = 15;

//logic statement to determine if a boundary is approached and turn ship
//around.. additionally it sets the general easterly or westerly search
//pattern
  if(m_osx > 180 - buffer){
    m_heading_desired = 270;
    m_mid_heading = 270;
    m_change_course = true;
    m_course_time = getBufferCurrTime();
  }
  if((m_osx < 100) && (m_osx > -50) && (m_osy > (2.0/5.0 * m_osx - 20.0 - buffer))){
    m_heading_desired = 180;
    //m_mid_heading = 90;
    m_change_course = true;
    m_course_time = getBufferCurrTime();
  }
  if((m_osx < -50) && (m_osy > 7.0/10.0 * m_osx - 5.0 - buffer)){
    m_heading_desired = 90;
    m_mid_heading = 90;
    m_change_course = true;
    m_course_time = getBufferCurrTime();




//determining wavelength
    double y_one = a_one * (-40) + a_zero;
    double y_two = a_one * (160) + a_zero;
    postMessage("WAVE_UPDATES","points=pts={-40,"+to_string(y_one)+":160,"+to_string(y_two) +"}");
    postMessage("FIND_WL","true");
  }
  if((m_osx < -50) && (m_osy < -5.0/2.0 * m_osx - 325.0 + buffer)){
    m_change_course = true;
    m_mid_heading = 90;
    m_heading_desired = 90;
    m_course_time = getBufferCurrTime();
    double y_one = a_one * (-40) + a_zero;
    double y_two = a_one * (160) + a_zero;
    postMessage("WAVE_UPDATES","points=pts={-40,"+to_string(y_one)+":160,"+to_string(y_two) +"}");
    postMessage("FIND_WL","true");
   }

  //build the IvP function
  ipf = buildFunctionWithZAIC();
  
  // Prior to returning the IvP function, apply the priority wt
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

