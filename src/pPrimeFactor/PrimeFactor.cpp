/************************************************************/
/*    NAME: David Baxter                                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeFactor.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "PrimeFactor.h"
#include "PrimeEntry.h"
#include <cstdint>
#include<sstream>
#include<unistd.h>

using namespace std;

//---------------------------------------------------------
// Constructor
PrimeFactor::PrimeFactor() {
}

//---------------------------------------------------------
// Destructor
PrimeFactor::~PrimeFactor() {
}

//---------------------------------------------------------
// Procedure: OnNewMail
bool PrimeFactor::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
   
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

  //Every new NUM_VALUE instantiates a PrimeEntry object which is pushed to a list
    string key = msg.GetKey();
    if(key=="NUM_VALUE"){
      string value = msg.GetString();      
      uint64_t a = strtoul(value.c_str(),NULL,0);
      PrimeEntry b(a);
      m_list2.push_front(b); 
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

bool PrimeFactor::OnConnectToServer()
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

bool PrimeFactor::Iterate() //Compute factors of objects in list and publish results
{
  list<PrimeEntry>::iterator p;
  for(p=m_list2.begin(); p!=m_list2.end(); p++) {
    PrimeEntry &primeobj = *p;
    primeobj.factor();
    if(primeobj.done()) {
      Notify("PRIME_RESULT",primeobj.getReport());
      p = m_list2.erase(p);
    }
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool PrimeFactor::OnStartUp()
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
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void PrimeFactor::RegisterVariables()
{
  Register("NUM_VALUE", 0);
}
