/************************************************************/
/*    NAME: David Baxter                                              */
/*    ORGN: MIT                                             */
/*    FILE: CommunicationAngle_baxterdp.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "CommunicationAngle_baxterdp.h"
#include "Data.h"
#include <cstdint>
#include<sstream>
#include<unistd.h>

using namespace std;

//---------------------------------------------------------
// Constructor
CommunicationAngle_baxterdp::CommunicationAngle_baxterdp()
{
  m_collab = 0;
}

//---------------------------------------------------------
// Destructor

CommunicationAngle_baxterdp::~CommunicationAngle_baxterdp()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool CommunicationAngle_baxterdp::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
   
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

  //Every new NUM_VALUE instantiates a PrimeEntry object which is pushed to a list
    string key = msg.GetKey();

    if(!m_collab) {
      if(key=="COLLABORATOR_NAME"){
        string collab = msg.GetString();      
        RegisterCollabVariables(collab);
        m_collab = 1;
      }
    }

    if(m_collab) {
      if(key==m_collaborator_NAV_X){
        double nav_x0 = msg.GetDouble();      
        m_current.set_x0(nav_x0);
      }
      
      if(key==m_collaborator_NAV_Y){
        double nav_y0 = msg.GetDouble();      
        m_current.set_y0(nav_y0);
      }

    if(key==m_collaborator_NAV_DEPTH){
      double nav_depth0 = msg.GetDouble();      
      m_current.set_depth0(nav_depth0);
    }

    }

    if(key=="NAV_DEPTH"){
      double nav_deptha = msg.GetDouble();      
      m_current.set_deptha(nav_deptha);
    }


    if(key=="NAV_X"){
      double nav_xa = msg.GetDouble();      
      m_current.set_xa(nav_xa);
    }

    if(key=="NAV_Y"){
      double nav_ya = msg.GetDouble();      
      m_current.set_ya(nav_ya);
    }


#if 0 // Keep these around just for template
    string key   = msg.GetKey();
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

bool CommunicationAngle_baxterdp::OnConnectToServer()
{
   // register for variables here
   // possibly look at the mission file?
   // m_MissionReader.GetConfigurationParam("Name", <string>);
   // m_Comms.Register("VARNAME", 0);
	
   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool CommunicationAngle_baxterdp::Iterate()
{

  m_current.updateOutput();
  m_current.findAngle();
  m_current.transmissionLoss();
  Notify("TEST_VAR",m_current.m_Loss);
  Notify("TEST_STR",m_current.makeReport());
  Notify("ACOUSTIC_PATH",m_current.m_AngleOut);
  Notify("CONNECTIVITY_LOCATION",m_current.m_LocationOut);
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool CommunicationAngle_baxterdp::OnStartUp()
{
  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string line  = *p;
      string param = tolower(biteStringX(line, '='));
      string value = line;
      
      if(param == "foo") {
        //handled
      }
      else if(param == "bar") {
        //handled
      }
    }
  }
  
  RegisterVariables();	

  m_current.set_c0(1480);
  m_current.set_g(0.016);
  m_current.set_depthw(6000);

  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void CommunicationAngle_baxterdp::RegisterVariables()
{
  Register("VEHICLE_NAME", 0);
  Register("COLLABORATOR_NAME", 0);
  Register("NAV_X", 0);
  Register("NAV_Y", 0);
  Register("NAV_DEPTH", 0);
  Register("NAV_HEADING", 0);
  Register("NAV_SPEED", 0);
  Register("ACOUSTIC_PATH_REF", 0);
  Register("CONNECTIVITY_LOCATION_REF", 0);
}

void CommunicationAngle_baxterdp::RegisterCollabVariables(string name)
{
  set_collab_nav_x(name + "_NAV_X");
  Register(m_collaborator_NAV_X, 0);
  set_collab_nav_y(name + "_NAV_Y");
  Register(m_collaborator_NAV_Y, 0);
  set_collab_nav_depth(name + "_NAV_DEPTH");
  Register(m_collaborator_NAV_DEPTH, 0);
  // string collaborator_NAV_HEADING = name + "_NAV_HEADING";
  // Register(collaborator_NAV_HEADING, 0);
  // string collaborator_NAV_SPEED = name + "_NAV_SPEED";
  // Register(collaborator_NAV_SPEED, 0);
}

