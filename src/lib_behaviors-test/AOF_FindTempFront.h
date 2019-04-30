/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_SimpleWaypoint.h                                 */
/*    DATE: Feb 22th 2009                                        */
/*****************************************************************/
 
#ifndef AOF_FindTempFront_HEADER
#define AOF_FindTempFront_HEADER

#include "AOF.h"
#include "IvPDomain.h"

class AOF_FindTempFront: public AOF {
 public:
  AOF_FindTempFront(IvPDomain);
  ~AOF_FindTempFront() {};

public: // virtuals defined
  bool   initialize();
  // Initialization parameters
  double m_temp;
  double m_x;
  double m_y;
  double m_utc;


};

#endif

