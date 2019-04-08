/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HazardMgr.cpp                                        */
/*    DATE: Oct 26th 2012                                        */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "HazardMgr.h"
#include "XYFormatUtilsHazard.h"
#include "XYFormatUtilsPoly.h"
#include "ACTable.h"
#include "NodeMessage.h"
#include "HazardClassification.h"

using namespace std;

// struct ProbabilityComparator
// {
//   bool operator ()(const HazardClassification & hazard1, const HazardClassification & hazard2)
//   {
//     if(player1.name == player2.name)
//       return player1 < player2;
//     return player1.name < player2.name;
 
//   }
// };

//---------------------------------------------------------
// Constructor

HazardMgr::HazardMgr()
{
  // Config variables
  m_swath_width_desired = 25;
  m_pd_desired          = 0.5;

  // State Variables 
  m_sensor_config_requested = false;
  m_sensor_config_set   = false;
  m_first_four_reported = false;
  m_got_start_x = false;
  m_swath_width_granted = 0;
  m_pd_granted          = 0;

  m_sensor_config_reqs = 0;
  m_sensor_config_acks = 0;
  m_sensor_report_reqs = 0;
  m_detection_reports  = 0;

  m_summary_reports = 0;

  m_start_info = false;

}

//---------------------------------------------------------
// Procedure: OnNewMail

bool HazardMgr::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key   = msg.GetKey();
    string sval  = msg.GetString(); 

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif
    if(key == "UHZ_CONFIG_ACK") 
      handleMailSensorConfigAck(sval);

    else if(key == "UHZ_OPTIONS_SUMMARY") 
      handleMailSensorOptionsSummary(sval);
    else if(key == "GET_TIME") {
      m_start = MOOSTime();
    }

    else if(key == "UHZ_DETECTION_REPORT") 
      handleMailDetectionReport(sval);

    else if(key == "HAZARDSET_REQUEST"){ 
      handleMailReportRequest();
    }

    else if(key == "UHZ_MISSION_PARAMS") {
      if(!m_start_info) {
        handleMailMissionParams(sval);
      }
    }

    else if(key =="UHZ_HAZARD_REPORT"){
      if(m_job!="SEARCH")
        handleHazardClassification(sval);
    }

    else if(key == "NAV_X")
    //  reportEvent(sval);
      string s = "asdf";
      
    else if(key =="NEW_HAZARD_REPORT"){
     handleNewHazardReport(sval); 
    }

    else if(key == "VJOB") {
      m_job = sval;
      // if(m_job == "SEARCH") {
        // Notify("SEARCH_PATTERN",m_search_pattern);
        // Notify("CLASS_PATTERN",m_search_pattern);
        // Notify("FIRST_SEARCH_PATTERN",m_search_pattern);

      // }
      //assignVesselProbability();
    }

    else if(key == "ACK_REPORT"){
      handleAcknowledgmentReport(sval);
    }
      
    else 
      reportRunWarning("Unhandled Mail: " + key);
  }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer


bool HazardMgr::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool HazardMgr::Iterate()
{
  AppCastingMOOSApp::Iterate();

  if(!m_sensor_config_requested)
    postSensorConfigRequest();

  if(m_sensor_config_set)
    postSensorInfoRequest();

  if(m_job == "SEARCH"){
        postVesselHazards();  
    }
  double now = MOOSTime();
  double duration = now-m_start;
  if((duration>400) && (duration<800)) {
    Notify("HANG_Y","1");
  }


  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool HazardMgr::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(true);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if((param == "swath_width") && isNumber(value)) {
      m_swath_width_desired = atof(value.c_str());
      handled = true;
    }
    else if(((param == "sensor_pd") || (param == "pd")) && isNumber(value)) {
      m_pd_desired = atof(value.c_str());
      handled = true;
    }
    else if(param == "report_name") {
      value = stripQuotes(value);
      m_report_name = value;
      handled = true;
    }
    else if(param == "other_vessel") {
      value = stripQuotes(value);
      m_other_vessel = value;
      handled = true;
    }

    else if(param == "region") {
      XYPolygon poly = string2Poly(value);
      if(poly.is_convex())
    	m_search_region = poly;
      handled = true;
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);
  }
  
  m_hazard_set.setSource(m_host_community);
  m_hazard_set.setName(m_report_name);
  m_hazard_set.setRegion(m_search_region);
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void HazardMgr::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("UHZ_DETECTION_REPORT", 0);
  Register("UHZ_CONFIG_ACK", 0);
  Register("UHZ_OPTIONS_SUMMARY", 0);
  Register("UHZ_MISSION_PARAMS", 0);
  Register("UHZ_HAZARD_REPORT",0);
  Register("HAZARDSET_REQUEST", 0);
  Register("HAZARD_VESSEL_REPORT",0);
  Register("NEW_HAZARD_REPORT",0);
  Register("VJOB",0);
  Register("ACK_REPORT",0);
  Register("GET_TIME",0);
  Register("NAV_X",0);
}

//---------------------------------------------------------
// Procedure: postSensorConfigRequest

void HazardMgr::postSensorConfigRequest()
{
  string request = "vname=" + m_host_community;
  
  request += ",width=" + doubleToStringX(m_swath_width_desired,2);
  request += ",pd="    + doubleToStringX(m_pd_desired,2);

  m_sensor_config_requested = true;
  m_sensor_config_reqs++;
  Notify("UHZ_CONFIG_REQUEST", request);
}

//---------------------------------------------------------
// Procedure: postSensorInfoRequest

void HazardMgr::postSensorInfoRequest()
{
  string request = "vname=" + m_host_community;

  m_sensor_report_reqs++;
  Notify("UHZ_SENSOR_REQUEST", request);
}

//---------------------------------------------------------
// Procedure: handleMailSensorConfigAck

bool HazardMgr::handleMailSensorConfigAck(string str)
{
  // Expected ack parameters:
  string vname, width, pd, pfa, pclass;
  
  // Parse and handle ack message components
  bool   valid_msg = true;
  string original_msg = str;

  vector<string> svector = parseString(str, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];

    if(param == "vname")
      vname = value;
    else if(param == "pd")
      pd = value;
    else if(param == "width")
      width = value;
    else if(param == "pfa")
      pfa = value;
    else if(param == "pclass")
      pclass = value;
    else
      valid_msg = false;       

  }


  if((vname=="")||(width=="")||(pd=="")||(pfa=="")||(pclass==""))
    valid_msg = false;
  
  if(!valid_msg)
    reportRunWarning("Unhandled Sensor Config Ack:" + original_msg);

  
  if(valid_msg) {
    m_sensor_config_set = true;
    m_sensor_config_acks++;
    m_swath_width_granted = atof(width.c_str());
    m_pd_granted = atof(pd.c_str());
    m_pclass_granted = atof(pclass.c_str());
  }

  return(valid_msg);
}

//---------------------------------------------------------
// Procedure: handleMailDetectionReport
//      Note: The detection report should look something like:
//            UHZ_DETECTION_REPORT = vname=betty,x=51,y=11.3,label=12 


//Upon new hazard event
bool HazardMgr::handleMailDetectionReport(string str)
{
  m_detection_reports++;

  XYHazard new_hazard = string2Hazard(str);
  //new_hazard.setType("benign");

  string hazlabel = new_hazard.getLabel();
  
  if(hazlabel == "") {
    reportRunWarning("Detection report received for hazard w/out label");
    return(false);
  }

  int ix = m_hazard_set.findHazard(hazlabel);
  if(ix == -1){
    m_hazards_to_send.push_back(new_hazard);
    m_hazard_set.addHazard(new_hazard);
  }
  else {  
    m_hazard_set.setHazard(ix, new_hazard);
  }

  string event = "New Detection, label=" + new_hazard.getLabel();
  event += ", x=" + doubleToString(new_hazard.getX(),1);
  event += ", y=" + doubleToString(new_hazard.getY(),1);

  string req = "vname=" + m_host_community + ",label=" + hazlabel;
  

  Notify("UHZ_CLASSIFY_REQUEST", req);



  return(true);
}


//---------------------------------------------------------
// Procedure: handleMailReportRequest

void HazardMgr::handleMailReportRequest()
{
  m_summary_reports++; 

//  m_hazard_set.clear();
  XYHazardSet my_hazard_set;
  my_hazard_set.setSource(m_host_community);
  my_hazard_set.setName(m_report_name);
  my_hazard_set.setRegion(m_search_region);
      XYHazard update_hazard;
  list<HazardClassification>::iterator l;
  for(l=m_classification_tracker.begin(); l!=m_classification_tracker.end();) {
    HazardClassification &lobj = *l;
    XYHazard update_hazard;
    if(lobj.m_class=="hazard"){
      update_hazard.setX(lobj.m_x);
      update_hazard.setY(lobj.m_y);
      update_hazard.setType(lobj.m_class);
      update_hazard.setLabel(lobj.m_label);
    }
    my_hazard_set.addHazard(update_hazard);
    ++l;
  }
  my_hazard_set.findMinXPath(20);
  string msg = my_hazard_set.getSpec();

  // Post to the MOOSDB
  Notify("HAZARDSET_REPORT", msg);
}


//---------------------------------------------------------
// Procedure: handleMailMissionParams
//   Example: UHZ_MISSION_PARAMS = penalty_missed_hazard=100,               
//                       penalty_nonopt_hazard=55,                
//                       penalty_false_alarm=35,                  
//                       penalty_max_time_over=200,               
//                       penalty_max_time_rate=0.45,              
//                       transit_path_width=25,                           
//                       search_region = pts={-150,-75:-150,-50:40,-50:40,-75}

void HazardMgr::calculateParameters(double hazard_density)
{
  
  m_swath_width_desired = 25;
  m_pd_desired          = 0.5;
}

void HazardMgr::handleMailMissionParams(string str)
{


  vector<string> svector = parseStringZ(str, ',', "{");
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];
  }
      
  string trash = biteStringX(svector.back(), '{');
  double x1 = stod(biteStringX(svector.back(), ','));
  double y1 = stod(biteStringX(svector.back(), ':'));
  double x2 = stod(biteStringX(svector.back(), ','));
  double y2 = stod(biteStringX(svector.back(), ':'));
  double x3 = stod(biteStringX(svector.back(), ','));
  double y3 = stod(biteStringX(svector.back(), ':'));
  double x4 = stod(biteStringX(svector.back(), ','));
  double y4 = stod(biteStringX(svector.back(), '}'));

  double m_penalty_missed_hazard = stod(tokStringParse(str, "penalty_missed_hazard", ',', '='));
  double m_penalty_false_alarm = stod(tokStringParse(str, "penalty_false_alarm", ',', '='));
  
  calculateParameters(0.1);

  double m_poly_center_x = (x1 + x2 + x3 + x4) / 4;
  double m_poly_center_y = (y1 + y2 + y3 + y4) / 4;

  m_poly_w = abs(m_poly_center_x-x1)*2;
  m_poly_h = abs(m_poly_center_y-y1)*2;

  // if(m_job == "CLASS")
  //   m_poly_h = m_poly_h - 40;

  string l_width = "100";

  string tmp;
  double w_buffer = 70;
  double h_buffer = 100;
  double adjust = 0;
  h_buffer = h_buffer+2*adjust;

  m_poly_center_y = m_poly_center_y+adjust;
  tmp = "points = format=lawnmower,label=jakesearch,x=" + to_string(m_poly_center_x);
  tmp = tmp + ",y=" + to_string(m_poly_center_y) + ",height=";
  tmp = tmp + to_string(m_poly_h-h_buffer) + ",width=" + to_string(m_poly_w+w_buffer);
  tmp = tmp + ",lane_width="+l_width+",rows=east-west";//,startx=0,starty=0";

  m_search_pattern = tmp;
        Notify("SEARCH_PATTERN",m_search_pattern);
        Notify("CLASS_PATTERN",m_search_pattern);
        Notify("FIRST_SEARCH_PATTERN",m_search_pattern);

  m_start_info = true;
}


//Jake sends kasper new hazards
void HazardMgr::postVesselHazards()
{
  int size_hazards = m_hazards_to_send.size();
  int i = 1;
  string mes; 
  string x_str,y_str,l_str,t_str; 
  mes =  "src_node=" + m_report_name;
  mes = mes + ",dest_node=" + "all";
  mes = mes + ",var_name="  + "NEW_HAZARD_REPORT";  
  string updated_message;
  
  if(!m_first_four_reported && m_hazards_to_send.size()<4)
    return;

  list<XYHazard>::iterator l;
  for(l=m_hazards_to_send.begin(); l!=m_hazards_to_send.end(); l++) {
    XYHazard &lobj = *l;

    if(i<5) {
      HazardClassification new_classification;
      string msg = l->getSpec();


      x_str = tokStringParse(msg, "x", ',', '=');
      y_str = tokStringParse(msg, "y", ',', '=');
      l_str = tokStringParse(msg, "label", ',', '=');
      new_classification.m_label = l_str;
      m_classification_tracker.push_back(new_classification);
      t_str = tokStringParse(msg, "type", ',', '=');
      if(t_str=="benign")
        t_str = "b";
      if(t_str=="hazard")
        t_str = "h";
      updated_message = updated_message + "x=" + x_str + ";y=" + y_str +";l=" + l_str + ";t=" + t_str + ":";
    }

    i++;
  }

  mes = mes + ",string_val=" + updated_message;
  if(m_hazards_to_send.size()>0) {
    Notify("NODE_MESSAGE_LOCAL",mes);
  }
  if(!m_first_four_reported){
    m_first_four_reported = true;
  }
}

// Kasper Acknowledges and sends himself visit_points
void HazardMgr::handleNewHazardReport(string str)
{
  string x_str,y_str,l_str,t_str,mes; 

  int i = 0;
  string ack = "l=";
  int requests = std::count(str.begin(),str.end(),'x');
  reportEvent(to_string(requests));
  bool restart_loop;
  for(int i=0; i<requests; i++){
    restart_loop = false;
    HazardClassification new_classification;
    x_str = tokStringParse(str, "x", ';', '=');
    y_str = tokStringParse(str, "y", ';', '=');
    l_str = tokStringParse(str, "l", ';', '=');
    t_str = tokStringParse(str, "t", ';', '=');
    biteString(str, ':');
    string tmp;
    tmp = "x=" + x_str + ",y=" + y_str + ",id=" + l_str;
    new_classification.m_label = l_str;
    new_classification.m_v1_hazard_count = 0;
    new_classification.m_v1_benign_count = 0;
    new_classification.m_x = stod(x_str);
    new_classification.m_y = stod(y_str);
    ack = ack + l_str + ";";
    list<HazardClassification>::iterator l;
    for(l=m_classification_tracker.begin(); l!=m_classification_tracker.end(); ++l) {
      HazardClassification &lobj = *l;
      if(new_classification.m_label==lobj.m_label)
        restart_loop = true;
    }
    if(i==0){
      Notify("VISIT_POINT","firstpoint");
    }
    if(restart_loop){
      continue;
    }
    m_classification_tracker.push_back(new_classification);
    Notify("VISIT_POINT",tmp);
  }
  mes =  "src_node=" + m_report_name;
  mes = mes + ",dest_node=" + "all";
  mes = mes + ",var_name="  + "ACK_REPORT";  
  mes = mes + ",string_val=" + ack;
  Notify("NODE_MESSAGE_LOCAL",mes);
  
  Notify("VISIT_POINT","lastpoint");
  
}

//Jake handles kasper's acknowledgement
void HazardMgr::handleAcknowledgmentReport(string str)
{
  size_t n = std::count(str.begin(), str.end(), ';');
  biteString(str, '=');
  for( int i = 1; i<=(n); i++) {
    string next_label =  biteString(str, ';');  
    list<XYHazard>::iterator l;
    for(l=m_hazards_to_send.begin(); l!=m_hazards_to_send.end(); ++l) {
      XYHazard &lobj = *l;
      string tmp_lbl = lobj.getLabel();
      if(tmp_lbl==next_label) {
        l = m_hazards_to_send.erase(l);
      }
      else {
      }
    }
  }
}

void HazardMgr::handleHazardClassification(string str)
{
  string label_str = tokStringParse(str, "label", ',', '=');
  string type_str = tokStringParse(str, "type", ',', '=');
  double p_class = m_pclass_granted;
  double b_count, h_count;
  list<HazardClassification>::iterator l;
  for(l=m_classification_tracker.begin(); l!=m_classification_tracker.end();) {
    HazardClassification &lobj = *l;
    if(lobj.m_label == label_str){
      if(type_str=="hazard"){
        lobj.m_v1_hazard_count++;
      }
      if(type_str=="benign"){
        lobj.m_v1_benign_count++;
      }
      string msg = "hazard count = " + to_string(lobj.m_v1_hazard_count);
      msg = msg + ",benign count = " + to_string(lobj.m_v1_benign_count);
      msg = msg + ",label = " + lobj.m_label;
      reportEvent(msg);
      b_count = lobj.m_v1_benign_count;
      h_count = lobj.m_v1_hazard_count;
      if(b_count<h_count){
        lobj.m_class = "hazard";
        lobj.m_probability = pow(p_class,h_count)*pow(1-p_class,b_count);
        lobj.m_probability = lobj.m_probability / (lobj.m_probability + pow(p_class,b_count)*pow(1-p_class,h_count));
      }
      else if(h_count<b_count){
        lobj.m_class = "benign";
        lobj.m_probability = pow(p_class,b_count)*pow(1-p_class,h_count);
        lobj.m_probability = lobj.m_probability / (lobj.m_probability + pow(p_class,h_count)*pow(1-p_class,b_count));
      }

      reportEvent("type="+lobj.m_class+",probability = "+to_string(lobj.m_probability));
      Notify("UPDATE_POINT","label="+lobj.m_label+",probbability="+to_string(lobj.m_probability));
    }
    ++l;
  }
  
}


void HazardMgr::assignVesselProbability()
{
if(m_job=="SEARCH")
{
  m_pd_desired = 1;
}
if(m_job=="CLASS")
{
  m_pd_desired = 1;
}
  string request = "vname=" + m_host_community;
  request += ",pd="    + doubleToStringX(m_pd_desired,2);
  Notify("UHZ_CONFIG_REQUEST", request);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool HazardMgr::buildReport() 
{
  m_msgs << "Config Requested:"                                  << endl;
  m_msgs << "    swath_width_desired: " << m_swath_width_desired << endl;
  m_msgs << "             pd_desired: " << m_pd_desired          << endl;
  m_msgs << "   config requests sent: " << m_sensor_config_reqs  << endl;
  m_msgs << "                  acked: " << m_sensor_config_acks  << endl;
  m_msgs << "------------------------ "                          << endl;
  m_msgs << "Config Result:"                                     << endl;
  m_msgs << "       config confirmed: " << boolToString(m_sensor_config_set) << endl;
  m_msgs << "    swath_width_granted: " << m_swath_width_granted << endl;
  m_msgs << "             pd_granted: " << m_pd_granted          << endl << endl;
  m_msgs << "--------------------------------------------" << endl << endl;

  m_msgs << "               sensor requests: " << m_sensor_report_reqs << endl;
  m_msgs << "             detection reports: " << m_detection_reports  << endl << endl; 

  m_msgs << "   Hazardset Reports Requested: " << m_summary_reports << endl;
  m_msgs << "      Hazardset Reports Posted: " << m_summary_reports << endl;
  m_msgs << "                   Report Name: " << m_report_name << endl;

  return(true);
}












