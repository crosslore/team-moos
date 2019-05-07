/*****************************************************************/
/*    NAME: Henrik Schmidt                                       */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE:                                                      */
/*    DATE:                                                      */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifndef __GENETIC_h__
#define __GENETIC_h__

//#include "MOOSLIB/MOOSLib.h"
#include "MOOS/libMOOS/MOOSLib.h"

#include "Random.h"
#include "CFrontSim2.h"
#include <math.h>
#include <vector>
#include <iostream>
#include "MBUtils.h"

class CMeasurement
{
 public:
  double t;
  double x;
  double y;
  double temp;
};
 
class CSimAnneal 
{
 public:

  CSimAnneal();
  virtual ~CSimAnneal();

  void setVars(int num, double temp_fac, bool adapt);
  bool setInitVal(std::vector<double> val);
  bool setMinVal(std::vector<double> val);
  bool setMaxVal(std::vector<double> val);
  void getEstimate(std::vector<double>& est, bool good);
  void clearMeas();
  void addMeas(CMeasurement new_meas);
  CMeasurement parseMeas(std::string report);
  double calcEnergy(bool good);
  double heatBath(double temperature);
  double measModel(double t, double x, double y);
  double measModelGood(double t, double x, double y);
  
  void updateOffset(int min, int max, int guess);
  bool setMinVal(int val, int i);
  bool setMaxVal(int val, int i);

 protected:

  CRandom Ran;
  unsigned int num_vars;
  double k;
  bool adaptive;

  std::vector<double> variables;
  std::vector<double> var_min;
  std::vector<double> var_max;
  std::vector<double> var_norm;
  std::vector<double> variables_best;
  std::vector<double> variables_good;
  std::vector<double> var_min_best;
  std::vector<double> var_max_best;

  bool got_min;
  bool got_max;
  
  double Energy;
  double Energy_good;
  double Energy_best;

  std::vector<CMeasurement> measurements;
  std::vector<CMeasurement> model;

  CFrontSim front;

  int offset_guess;
  int offset_guess_min;
  int offset_guess_max;

  // void updateAmplitude(int min, int max, int guess);
  // void updateAmplitude(int min, int max, int guess);


};
  
#endif /* __CSimAnneal_h__ */



