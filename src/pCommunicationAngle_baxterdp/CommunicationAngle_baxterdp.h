/************************************************************/
/*    NAME: David Baxter                                              */
/*    ORGN: MIT                                             */
/*    FILE: CommunicationAngle_baxterdp.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef CommunicationAngle_baxterdp_HEADER
#define CommunicationAngle_baxterdp_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "Data.h"

using namespace std;


class CommunicationAngle_baxterdp : public CMOOSApp
{
 public:
   CommunicationAngle_baxterdp();
   ~CommunicationAngle_baxterdp();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected:
   void RegisterVariables();
   void RegisterCollabVariables(string);
   void set_collab_nav_x(string v){m_collaborator_NAV_X = v;};
   void set_collab_nav_y(string v){m_collaborator_NAV_Y = v;};
   void set_collab_nav_depth(string v){m_collaborator_NAV_DEPTH = v;};

   bool m_collab;
   Data m_current;
   string m_collaborator_NAV_X;
   string m_collaborator_NAV_Y;
   string m_collaborator_NAV_DEPTH;

 private: // Configuration variables

 private: // State variables
};

#endif 
