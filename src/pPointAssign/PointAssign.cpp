/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: PointAssign.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "PointAssign.h"
#include "PointParse.h"
#include <cstdint>
#include<sstream>
#include<unistd.h>
#include"XYPoint.h"
#include <cctype>

using namespace std;

//---------------------------------------------------------
// Constructor

PointAssign::PointAssign()
{
m_name1 = "1";
m_name2 = "2";
m_got_all_points = false;
m_sent_all_points = false;
m_even = true;


}

//---------------------------------------------------------
// Destructor

PointAssign::~PointAssign()
{
}


void PointAssign::sendPoints()
{

    regionSort();


  // if(m_even) {
  //   evenSort();
  // }
  // else {
  //   regionSort();
  // }

}




void PointAssign::evenSort() {

  double x,y;
  string label,color;

  Notify(m_out_name1,"firstpoint");
  Notify(m_out_name2,"firstpoint");

  list<PointParse>::iterator l;
  for(l=m_list.begin(); l!=m_list.end(); l++) {
    PointParse &lobj = *l;
    int tmp_id = atoi(lobj.m_id.c_str());

    x = atof(lobj.m_x.c_str());
    y = atof(lobj.m_y.c_str());
    XYPoint point(x,y);
    point.set_param("vertex_size", "3");
    point.set_label(lobj.m_id);


    // Notify("ASSIGN","OK");

    if((lobj.m_id=="firstpoint") ||(lobj.m_id=="lastpoint")){

    }
    else if(tmp_id%2) {
      point.set_color("vertex","red");
      string spec = point.get_spec();
      Notify("VIEW_POINT",spec);
      Notify(m_out_name1,"x="+lobj.m_x+", y="+lobj.m_y+", id="+lobj.m_id);
    }
    else {
      point.set_color("vertex","yellow");
      string spec = point.get_spec();
      Notify("VIEW_POINT",spec);

      Notify(m_out_name2,"x="+lobj.m_x+", y="+lobj.m_y+", id="+lobj.m_id);
    }

  }

 Notify(m_out_name1,"lastpoint");
 Notify(m_out_name2,"lastpoint");

  m_sent_all_points = true;

  // int tmp_id = stoi(m_id);

}

void PointAssign::regionSort() {

  m_list.sort();
  int tmp_id = 1;

string LOOK;
string ID;
  list<PointParse>::iterator l2;
  for(l2=m_list.begin(); l2!=m_list.end(); l2++) {
    PointParse &lobj = *l2;

    LOOK += lobj.m_x +"  ";
    lobj.set_m_id(to_string(tmp_id));

    ID +=lobj.m_id;
    tmp_id ++;

  }

  // reportEvent("Good msg received: " + LOOK);
  // reportEvent("ID: " + ID);


  double x,y;
  string label,color;
  double size = m_list.size()/2;

  Notify(m_out_name1,"firstpoint");
  Notify(m_out_name2,"firstpoint");

  list<PointParse>::iterator l;
  for(l=m_list.begin(); l!=m_list.end(); l++) {
    PointParse &lobj = *l;

    x = atof(lobj.m_x.c_str());
    y = atof(lobj.m_y.c_str());

    XYPoint point(x,y);
    point.set_param("vertex_size", "3");
    point.set_label(lobj.m_id);

  // reportEvent("ID2: " + lobj.m_id);


    if((lobj.m_id=="firstpoint") ||(lobj.m_id=="lastpoint")){

    }
    else if(atof(lobj.m_id.c_str())<size) {
      point.set_color("vertex","red");
      string spec = point.get_spec();
      Notify("VIEW_POINT",spec);
      Notify(m_out_name1,"x="+lobj.m_x+", y="+lobj.m_y+", id="+lobj.m_id);
    }
    else {
      point.set_color("vertex","yellow");
      string spec = point.get_spec();
      Notify("VIEW_POINT",spec);

      Notify(m_out_name2,"x="+lobj.m_x+", y="+lobj.m_y+", id="+lobj.m_id);
    }

  }

 Notify(m_out_name1,"lastpoint");
 Notify(m_out_name2,"lastpoint");

  m_sent_all_points = true;


}


//---------------------------------------------------------
// Procedure: OnNewMail

bool PointAssign::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

  //Every new VISIT_POINT instantiates a PointParse object which is pushed to a list
    string key = msg.GetKey();
    if(key=="VISIT_POINT"){
      string value = msg.GetString();      
      PointParse b(value);
      m_list.push_front(b);
      
    }

    if(key=="NODE_REPORT"){
      string value = msg.GetString();   
      string name = tokStringParse(value, "NAME",',','=');
      name = toupper(name);   
      Notify("READ_NAME",name);
      if(m_name1 == "1") {
        m_name1 = name;
        m_out_name1 = "VISIT_POINT_"+m_name1;
        m_out_x1 = tokStringParse(value, "X",',','=');

      }
      else if((m_name2 == "2") && (name!=m_name1)) {
        m_name2 = name;
        m_out_name2 = "VISIT_POINT_"+m_name2;
        m_out_x2 = tokStringParse(value, "X",',','=');

        if(atof(m_out_x2.c_str()) < atof(m_out_x1.c_str())) {

          m_out_x2 = m_out_x1;
          m_out_x1 = tokStringParse(value, "X",',','=');

          m_out_name2 = m_out_name1;
          m_out_name1= "VISIT_POINT_"+m_name2;
        }

        reportEvent("X_POS: " + tokStringParse(value, "X",',','='));
      }
    }

    // if(key=="NODE_REPORT"){
    //   string value = msg.GetString();   
    //   Notify("GOT_IT",value);
    // }




    



#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

     if(key == "FOO") 
       cout << "great!";

     // else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
     //   reportRunWarning("Unhandled Mail: " + key);
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool PointAssign::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool PointAssign::Iterate()
{
  AppCastingMOOSApp::Iterate();
  // Do your thing here!

  // list<PointParse>::iterator p;
  // for(p=m_list.begin(); p!=m_list.end(); p++) {
  //   PointParse &pointobj = *p;
  //   // Notify("ASSIGN",pointobj.getReport());
    Notify("NAME1",m_name1);
    Notify("NAME2",m_name2);

    if(!m_got_all_points) {
      list<PointParse>::iterator p;
      for(p=m_list.begin(); p!=m_list.end(); p++) {
        PointParse &pointobj = *p;

        if((pointobj.m_id=="lastpoint") && (m_name2 != "2")) {
          m_got_all_points = true;
        }
      }
    }


    if((m_got_all_points) && (!m_sent_all_points)) {
      sendPoints();
    }





  //   if(pointobj.m_id()=="lastpoint") {
  //     m_got_all_points = true;
  //   }
  //   if(m_got_all_points) {
  //     sendPoints();
  //     p = m_list.erase(p);

  //   }
  // }

  // Notify("ASSIGN",m_list2)

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool PointAssign::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();


  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if(param == "foo") {
      handled = true;
    }
    else if(param == "bar") {
      handled = true;
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void PointAssign::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();

  Register("VISIT_POINT", 0);
  Register("NODE_REPORT", 0);

  // Notify("PAUSE_TIME","false");

}


//------------------------------------------------------------
// Procedure: buildReport()

bool PointAssign::buildReport() 
{
  m_msgs << "============================================ \n";
  m_msgs << "File:                                        \n";
  m_msgs << "============================================ \n";

  ACTable actab(4);
  actab << "Alpha | Bravo | Charlie | Delta";
  actab.addHeaderLines();
  actab << "one" << "two" << "three" << "four";
  m_msgs << actab.getFormattedString();

  return(true);
}




