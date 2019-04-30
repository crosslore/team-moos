/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_SimpleWaypoint.cpp                               */
/*    DATE: Feb 22th 2009                                        */
/*****************************************************************/

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <math.h> 
#include "AOF_FindTempFront.h"
#include "AngleUtils.h"
#include "GeomUtils.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor

AOF_FindTempFront::AOF_FindTempFront(IvPDomain domain) : AOF(domain)
{
  m_temp = 0;
  m_x = 0;
  m_y = 0;
  m_utc = 0;

}


//----------------------------------------------------------------
// Procedure: setParam

bool AOF_FindTempFront::initialize()
{
  return(true);
}
