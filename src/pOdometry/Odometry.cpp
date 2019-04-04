/************************************************************/
/*    NAME: David Baxter                                              */
/*    ORGN: MIT                                             */
/*    FILE: Odometry.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "Odometry.h"


using namespace std;

//---------------------------------------------------------
// Constructor

Odometry::Odometry()
{
  //Initialize Variables
  bool m_fitst_reading = 1; //Used to correct previous x and y values for case where origin not at (0,0). Flips when First NAV_X and NAV_Y received
  bool m_NAV_X_first_reading = 1; //Flips when first NAV_X received
  bool m_NAV_Y_first_reading = 1; //Flips when first NAV_Y received
  double m_current_x = 0;
  double m_current_y = 0;
  double m_previous_x = 0;
  double m_previous_y = 0;
  double m_total_distance = 0;

}

//---------------------------------------------------------
// Destructor

Odometry::~Odometry()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool Odometry::OnNewMail(MOOSMSG_LIST &NewMail)
{

  // Allows the AppCastingMOOSApp::OnNewMail() function to remove handled messages before returning to the mail handling implemented in YourMOOSApp::OnNewMail().
  AppCastingMOOSApp::OnNewMail(NewMail);  

  MOOSMSG_LIST::iterator p;
   
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    string key = msg.GetKey();

    if(key=="NAV_X"){
      m_current_x = msg.GetDouble();
      if(m_NAV_X_first_reading){
	m_NAV_X_first_reading = 0; //Flips when first NAV_X received
	m_previous_x = m_current_x;
      }
    }

    if(key=="NAV_Y"){
      m_current_y = msg.GetDouble();
      if(m_NAV_Y_first_reading){
	m_NAV_Y_first_reading = 0; //Flips when first NAV_Y received
	m_previous_y = m_current_y;
      }
    }

    //Toggles bool after first NAV_X and NAV_Y received
    if(!(m_NAV_X_first_reading+m_NAV_Y_first_reading)){
	m_first_reading = 0;
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

bool Odometry::OnConnectToServer()
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

bool Odometry::Iterate()
{

  //invoking subclass method that does certain common bookkeeping such as incrementing the counter representing the number of application iterations, and updating a variable holding the present MOOS time
  AppCastingMOOSApp::Iterate();

  if(m_first_reading){ //May never be invoked. Only would occur if first message had NAV_X but not NAV_Y or vise versa
      m_previous_y = m_current_y;
      m_previous_x = m_current_x;
      return(true);
      }

  //Calculate Hypotenuse of distance traveled in update
  m_total_distance += sqrt((m_current_x-m_previous_x)*(m_current_x-m_previous_x)+(m_current_y-m_previous_y)*(m_current_y-m_previous_y));

  //Reset previous
    m_previous_y = m_current_y;
    m_previous_x = m_current_x;

    //Publish Odometry
    Notify("ODOMETRY_DIST",m_total_distance);

    //Invokes the on-demand appcasting logic. If an appcast is deemed warranted, it will invoke the buildReport() function.
    AppCastingMOOSApp::PostReport();  

  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool Odometry::OnStartUp()
{
  //Invokes a superclass function to perform startup steps needed by the AppCastingMOOSApp superclass.
  AppCastingMOOSApp::OnStartUp(); 

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
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void Odometry::RegisterVariables()
{

  AppCastingMOOSApp::RegisterVariables();

  Register("NAV_X", 0);
  Register("NAV_Y", 0);
}


//Appcast Made here
bool Odometry::buildReport(){
  m_msgs << "Total Dist: " << m_total_distance << endl;
  return(true);
}
