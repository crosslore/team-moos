/************************************************************/
/*    NAME: Brian Stanfield                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_AvoidBoundary.h                                      */
/*    DATE:                                                 */
/************************************************************/

#ifndef AvoidBoundary_HEADER
#define AvoidBoundary_HEADER

#include <string>
#include "IvPBehavior.h"

class BHV_AvoidBoundary : public IvPBehavior {
public:
  BHV_AvoidBoundary(IvPDomain);
  ~BHV_AvoidBoundary() {};
  
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
  double             m_osx;
  double             m_osy;
  double             m_heading_desired;
  double             m_speed_desired;

protected: // State variables
};

#define IVP_EXPORT_FUNCTION

extern "C" {
  IVP_EXPORT_FUNCTION IvPBehavior * createBehavior(std::string name, IvPDomain domain) 
  {return new BHV_AvoidBoundary(domain);}
}
#endif
