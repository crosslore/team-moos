/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: GenPath.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "GenPath.h"
#include "CompPath.h"
#include <cstdint>
#include<sstream>
#include<unistd.h>
#include"XYPoint.h"
#include"XYSegList.h"
#include<math.h>

using namespace std;


int visit_radius = 5;

//---------------------------------------------------------
// Constructor

GenPath::GenPath()
{
m_got_all_points = false;
m_sent_all_points = false;
m_register_start = false;
}

//---------------------------------------------------------
// Destructor

GenPath::~GenPath()
{
}

void GenPath::setStart(XYPoint point)
{

m_start_point = point;
m_register_start = true;

}

void GenPath::testComp()
{

  list<CompPath>::iterator l;
  for(l=m_list.begin(); l!=m_list.end();) {
    CompPath &lobj = *l;

    double x_pt,y_pt;

    x_pt = atof(lobj.m_x.c_str());
    y_pt = atof(lobj.m_y.c_str());

    double hyp = hypot(x_pt-m_x_curr,y_pt-m_y_curr);

    if((hyp < visit_radius) && (lobj.m_id!="firstpoint") && (lobj.m_id!="lastpoint")) {
      l = m_list.erase(l);
    }
    else {
      ++l;
    }
  }
}

void GenPath::sendPoints()
{
  my_seglist.clear();
  double x,y;
  string label,color;

  my_seglist.set_param("vertex_size", "3");
  string update_str = "points = ";


  list<CompPath>::iterator l;
  for(l=m_list.begin(); l!=m_list.end(); l++) {
    CompPath &lobj = *l;
    // int tmp_id = atoi(lobj.m_id.c_str());

    x = atof(lobj.m_x.c_str());
    y = atof(lobj.m_y.c_str());
    
    XYPoint point(x,y);
    point.set_param("vertex_size", "3");
    point.set_label(lobj.m_id);

    // Only Matters After first total completion. Gaurentees the boats come home
    if(m_set_start) {
      my_seglist.add_vertex(m_start_point);//.x(),m_start_point.y());
    }

    if((lobj.m_id=="firstpoint") ||(lobj.m_id=="lastpoint")){
      my_seglist.add_vertex(m_start_point);//.x(),m_start_point.y());
    }
    else {      
      my_seglist.insert_vertex(point.x(),point.y());
    }
  }

  update_str       += my_seglist.get_spec();

  Notify("LOITER_UPDATE",update_str);

  m_sent_all_points = true;

}

//---------------------------------------------------------
// Procedure: OnNewMail

bool GenPath::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    string key = msg.GetKey();

    // if(key=="WPT_STAT") {
    //   string value = msg.GetString();
    //   reportEvent("WPT_STAT = "+value);
    // }

    if(key=="GENPATH_REGENERATE") {
      string value = msg.GetString();
      if(value =="true") {
        reportEvent("DONE !!!!!");
        Notify("LOITER","false");

        if ((m_list.size() > 2) && (m_sent_all_points)) {
          sendPoints();
        }
        else if (m_list.size()==2)
        {
          m_set_start = true;
          sendPoints();
          m_set_start = false;    
          Notify("NO_WPT","true");
  
        }

        Notify("LOITER","true");


        Notify("GENPATH_REGENERATE","false");
      }
    }



  //Every new VISIT_POINT instantiates a CompPath object which is pushed to a list
    if(key=="VISIT_POINT"){
      string value = msg.GetString();      
      CompPath b(value);
      m_list.push_front(b);
      // Notify("GOT_IT",b.getReport());
    }

    if(key=="NODE_REPORT_LOCAL"){
      string value = msg.GetString();  
      string x_str,y_str,ans; 
      double x_now,y_now;

      x_str = tokStringParse(value, "X", ',', '=');
      y_str = tokStringParse(value, "Y", ',', '=');
      x_now = atof(x_str.c_str());
      y_now = atof(y_str.c_str());
      m_x_curr = x_now;
      m_y_curr = y_now;

      XYPoint point(x_now,y_now);

      if(!m_register_start){
        setStart(point);
      }

    }


#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool GenPath::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool GenPath::Iterate()
{
  AppCastingMOOSApp::Iterate();
  // Do your thing here!

    if(!m_got_all_points) {
      list<CompPath>::iterator p;
      for(p=m_list.begin(); p!=m_list.end(); p++) {
        CompPath &pointobj = *p;

        if((pointobj.m_id=="lastpoint")) {
          m_got_all_points = true;
        }
      }
    }


    if((m_got_all_points) && (!m_sent_all_points)) {
      sendPoints();
    }

    if(m_sent_all_points) {
      testComp();
    }



  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool GenPath::OnStartUp()
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

void GenPath::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("VISIT_POINT", 0);
  Register("LOITER_UPDATE",0);
  Register("NODE_REPORT_LOCAL",0);
  Register("GENPATH_REGENERATE",0);
  Register("WPT_STAT",0);
  Notify("PAUSE_TIME","false");

}


//------------------------------------------------------------
// Procedure: buildReport()

bool GenPath::buildReport() 
{
  m_msgs << "============================================ \n";
  m_msgs << "File:                                        \n";
  m_msgs << "============================================ \n";

m_msgs << "SIZE" << m_list.size() << endl;


  // ACTable actab(4);
  // actab << "Alpha | Bravo | Charlie | Delta";
  // actab.addHeaderLines();
  // actab << "one" << "two" << "three" << "four";
  // m_msgs << actab.getFormattedString();

  return(true);
}




