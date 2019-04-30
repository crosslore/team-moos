/************************************************************/
/*    NAME: Brian Stanfield                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_FindTempFront.h                                      */
/*    DATE:                                                 */
/************************************************************/

#ifndef FindTempFront_HEADER
#define FindTempFront_HEADER

#include <string>
#include "IvPBehavior.h"

#include <list>
#include "ZAIC_PEAK.h"

class BHV_FindTempFront : public IvPBehavior {
public:
  BHV_FindTempFront(IvPDomain);
  ~BHV_FindTempFront() {};
  
  bool         setParam(std::string, std::string);
  void         onSetParamComplete();
  void         onCompleteState();
  void         onIdleState();
  void         onHelmStart();
  void         postConfigStatus();
  void         onRunToIdleState();
  void         onIdleToRunState();
  IvPFunction* onRunState();
  IvPFunction* buildFunctionWithZAIC();

protected: // Local Utility functions

protected: // Configuration parameters

protected: // State variables
  double             m_osx;
  double             m_osy;
  double             m_th;
  double             m_th_y;
  double             m_tc;
  double             m_tc_y;
  double             m_heading_desired;
  double             m_curr_heading;
  bool               m_change_course;
  bool               m_top_hot;
  std::string        m_msmnt_report; 
  double             m_course_time;
  list<double>       m_temps;
  double             m_mid_heading;

};

#define IVP_EXPORT_FUNCTION

extern "C" {
  IVP_EXPORT_FUNCTION IvPBehavior * createBehavior(std::string name, IvPDomain domain) 
  {return new BHV_FindTempFront(domain);}
}
#endif
