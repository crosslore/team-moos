/*****************************************************************/
/*    NAME: David Baxter (Adapted from Henrik Schmidt)           */
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

class Measurement
{
 public:
  double t;
  double x;
  double y;
  double temp;
};

class Chromosome
{
 public:
  bool operator< (const Chromosome&) const;

  std::vector<double> variables;
  double cost;

};
 
class Genetic 
{
 public:

  Genetic();
  virtual ~Genetic();

  void setVars(int num, double temp_fac, bool adapt);
  bool setInitVal(std::vector<double> val);
  bool setMinVal(std::vector<double> val);
  bool setMaxVal(std::vector<double> val);
  void getEstimate(std::vector<double>& est, bool good);
  void clearMeas();
  void addMeas(Measurement new_meas);
  Measurement parseMeas(std::string report);
  double calcEnergy(int chrom);
  std::string run();
  double measModel(double t, double x, double y, int chrom);

  void updateParam(int param, int min, int max, int guess);
  void calcCost();
  void mate();
  void mutate();

  bool setMinVal(int val, int i);
  bool setMaxVal(int val, int i);

  int cumlsum(int n);


 protected:

  CRandom Ran;
  unsigned int num_vars;
  double k;
  bool adaptive;

  //Genetic Specific
  double max_it;      //Max Iterations
  double popsize;     //population size
  double mutrate;     //mutation rate
  double cselection;  //fraction of population kept
  double selection;   //fraction of population kept
  double Nt;          //continuous parameter GA Nt=#variables
  double keep;        //population members that survive
  double nmut;        //mutation number
  double M;           //number of matings
  double iga;         //generation counter
  std::vector<std::vector<int> > par;
  std::vector<double> pop_cost;
  std::vector<double> prob;
  std::vector<double> odds;
  std::vector<double> pick1;
  std::vector<double> pick2;
  std::vector<int> ma;
  std::vector<int> pa;
  std::vector<int> xp; //crossoverpoint
  std::vector<int> ix; //index Mate 1
  std::vector<int> mut_row; //index Mute
  std::vector<int> mut_col; //index Mute
  std::vector<double> mix;
  // std::vector<double> spread;

  std::vector<Chromosome> population;


  std::vector<double> variables;
  std::vector<double> var_min;
  std::vector<double> var_max;
  std::vector<double> var_norm;
  std::vector<double> variables_best;
  std::vector<double> var_min_best;
  std::vector<double> var_max_best;

  bool got_min;
  bool got_max;
  bool got_init;
  bool check;
  int count;

  double Energy;
  double Energy_good;
  double Energy_best;

  std::vector<Measurement> measurements;
  std::vector<Measurement> model;

  CFrontSim front;



};
  
#endif /* __Genetic_h__ */



