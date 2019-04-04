/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: GenPath.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef GenPath_HEADER
#define GenPath_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include <list>
#include "CompPath.h"
#include"XYPoint.h"
#include"XYSegList.h"



class GenPath : public AppCastingMOOSApp
{
 public:
   GenPath();
   ~GenPath();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();

 protected:
   void registerVariables();
   void sendPoints();
   void setStart(XYPoint);
   void testComp();
   list<CompPath> m_list; //List of objects that are then parsed
   bool        m_got_all_points;// = false;
   bool        m_sent_all_points;// = false;
   XYPoint     m_start_point;
   bool        m_register_start;// = false;
   bool        m_set_start;// = false;
   XYSegList   my_seglist;
   double	   m_x_curr;
   double	   m_y_curr;




 private: // Configuration variables

 private: // State variables
};

#endif 
