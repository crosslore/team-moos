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
#include "temps.h"

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
  void         findEstimates(double x, double y, double temp);
  void         calcAmplitude();
  void         refineTemps(double temp);
  void         findWavelength(Temps New_Temp);
  void         calculateWavelengthWest();
  void         calculateWavelengthEast();
  void         courseAdjustBoundary();
  void         reportOffsetAngle();
  void         calculatePeriodEast(Temps New_Temp);
  void         determineCoursePD(Temps New_Temp);
  IvPFunction* onRunState();
  IvPFunction* buildFunctionWithZAIC();

protected: // Local Utility functions

protected: // Configuration parameters

protected: // State variables
  double             m_osx;
  double             m_osy;
  double             m_osh;
  double             m_th;
  double             m_th_y;
  double             m_tc;
  double             m_tc_y;
  double             m_heading_desired;
  double             m_course_desired;
  double             m_curr_heading;
  bool               m_change_course;
  bool               m_top_hot;
  double             m_tave;
  bool               m_survey_start;
  std::string        m_msmnt_report; 
  double             m_course_time;
  list<double>       m_temps;
  double             m_mid_heading;
  list<double>       m_y_ave;
  list<double>       m_temps_ave;
  list<double>       m_x_ave;
  list<double>       m_xy_ave;
  list<double>       m_x_square_ave;
  double             m_angle;
  double             m_t_turn_south;
  double             m_t_turn_north;
  list<Temps>        temps_list;
  list<Temps>        ave_temps_list;
  double             a_one;
  double             a_zero;
  double             m_alpha;
  double             min_amp;
  double             max_amp;
  double             amp;
  double             max_delta;
  double             temp_last;
  bool               finding_wavelength;
  std::string        location;
  list<Temps>        wavelength_temps_west;
  list<Temps>        wavelength_temps_east;
  list<double>       wavelength_west;
  list<double>       wavelength_east;
  double             wavelength_east_guess;
  double             wavelength_west_guess;   
  double             wave_large;
  double             wave_small;

  bool               foundwave;
  Temps              Last_Temp;
  std::string        direction;
  list<Temps>        Last_Ten;


  double             min_T_N;
  double             max_T_N;
  double             min_T_S;
  double             max_T_S; 
  double             min_offset;
  double             max_offset;
  double             min_angle;
  double             max_angle;
  double             min_amplitude;
  double             max_amplitude;
  double             min_wavelength;
  double             max_wavelength;

};

#define IVP_EXPORT_FUNCTION

extern "C" {
  IVP_EXPORT_FUNCTION IvPBehavior * createBehavior(std::string name, IvPDomain domain) 
  {return new BHV_FindTempFront(domain);}
}
#endif
