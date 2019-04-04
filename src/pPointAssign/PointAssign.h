/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: PointAssign.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef PointAssign_HEADER
#define PointAssign_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include <list>
#include "PointParse.h"



class PointAssign : public AppCastingMOOSApp
{
 public:
   PointAssign();
   ~PointAssign();

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
  void    evenSort();
  void    regionSort();
  list<PointParse> m_list; //List of objects that are then parsed
  list<string> m_list1; //List of VISIT_POINTs
  list<string> m_list2; //List of VISIT_POINTs
  string      m_name1;// = "1";
  string      m_name2;// = "2";
  string      m_out_name1;
  string      m_out_x1;
  string      m_out_name2;
  string      m_out_x2;
  bool        m_got_all_points;// = false;
  bool        m_sent_all_points;// = false;
  bool		  m_even;// = true;



 private: // Configuration variables

 private: // State variables
};

#endif 
