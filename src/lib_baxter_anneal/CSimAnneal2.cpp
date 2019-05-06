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


#include "CSimAnneal2.h"

using namespace std;

CSimAnneal::CSimAnneal()
{
  got_min = false;
  got_max = false;
  Energy = 0;
}

CSimAnneal::~CSimAnneal()
{
}

void CSimAnneal::updateOffset(int min, int max, int guess)
{

offset_guess = guess;
offset_guess_min = min;
offset_guess_max = max;

setMinVal(min,0);
setMaxVal(max,0);

}



void CSimAnneal::setVars( int num, double temp_fac, bool adapt)
{
  num_vars = num;
  k = temp_fac;
  adaptive = adapt;
}

bool CSimAnneal::setInitVal(vector<double> var_init)
{
  if (num_vars != var_init.size())
    {
      cout << ">>> setInitVal: Mismatch in number of variables <<<\n" << endl ;
	return(false);
    }
  variables = var_init;
  variables_good = var_init;
  variables_best = var_init;
  return(true);
}

bool CSimAnneal::setMinVal(vector<double> var)
{
  if (num_vars != var.size())
    {
      cout << ">>> setMinVal: Mismatch in number of variables <<<\n" << endl ;
	return(false);
    }
  var_min = var;
  var_min_best = var;
  got_min = true;
  return(true);
}

bool CSimAnneal::setMaxVal(vector<double> var)
{
  if (num_vars != var.size())
    {
      cout << ">>> setMaxVal: Mismatch in number of variables <<<\n" << endl ;
	return(false);
    }
  var_max = var;
  var_max_best = var;
  got_max = true;
  return(true);
}

bool CSimAnneal::setMinVal(int var, int i)
{
  var_min_best[i] = var;
  return(true);
}

bool CSimAnneal::setMaxVal(int var, int i)
{
  var_max_best[i] = var;
  return(true);
}

void CSimAnneal::getEstimate(vector<double>& var_est, bool good)
{
  if(good){
    var_est = variables_best;
  }
  else {
    var_est = variables;
  }
  
}

void CSimAnneal::clearMeas()
{
  measurements.clear();
}

void CSimAnneal::addMeas(CMeasurement new_meas)
{
  int num_meas = measurements.size(); 
  measurements.push_back(new_meas);
  // update energy
  double model = measModel(new_meas.t, new_meas.x, new_meas.y);
  Energy = sqrt((num_meas*pow(Energy,2) + pow(new_meas.temp - model,2))/(num_meas+1)); 

  model = measModelGood(new_meas.t, new_meas.x, new_meas.y);
  Energy_good = sqrt((num_meas*pow(Energy,2) + pow(new_meas.temp - model,2))/(num_meas+1)); 


  cout << ">>> Num_Meas=" << num_meas+1 << " new_meas.temp=" << new_meas.temp << endl; 
  cout << "t,x,y=" << new_meas.t << "," << new_meas.x << "," << new_meas.y 
       << " model=" << model << " Energy =" << Energy << endl;
}

CMeasurement CSimAnneal::parseMeas(string report)
{
//---------------------------------------------------------
// Procedure: parseMeas
//   Example: utc=123456789,x=100.0,y=200.5,temp=20.7

  // Part 1: Parse the measurement

  CMeasurement buf;
  string vname;
  vector<string> svector = parseString(report, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) 
    {
      string param = tolower(biteStringX(svector[i], '='));
      string value = svector[i];
      if(param == "utc")
	buf.t = atof(value.c_str());
      else if (param == "x")
	buf.x = atof(value.c_str());
      else if (param == "y")
	buf.y = atof(value.c_str());
      else if (param == "temp")
	buf.temp = atof(value.c_str());
    }
  return(buf);
}


double CSimAnneal::heatBath(double temperature)
{
  for (unsigned int i=0; i< num_vars; i++)
    {
      double old = variables[i];
      double old_good = variables_good[i];

      if (adaptive)
	{
	  // adaptive simulated annealing
	  double delta = (-1+2*Ran.rand())*temperature*(var_max[i]-var_min[i]);
	  double new_val = variables[i] + delta;
	  if (new_val > var_max[i])
	    variables[i] = var_max[i];
	  else if (new_val < var_min[i])
	    variables[i] = var_min[i];
	  else
	    variables[i]=new_val;

    delta = (-1+2*Ran.rand())*temperature*(var_max_best[i]-var_min_best[i]);
    new_val = variables_good[i] + delta;
    if (new_val > var_max_best[i])
      variables_good[i] = var_max_best[i];
    else if (new_val < var_min_best[i])
      variables_good[i] = var_min_best[i];
    else
      variables_good[i]=new_val;

	}
      else
	{
	  // traditional
    variables[i] = var_min[i] + Ran.rand()*(var_max[i]-var_min[i]);
    variables_good[i] = var_min_best[i] + Ran.rand()*(var_max_best[i]-var_min_best[i]);
	}

      double new_Energy = calcEnergy(0);
      double prob = exp(-(new_Energy-Energy)/(k*temperature));
      if (new_Energy < Energy || rand() <= prob)
	{
	  Energy = new_Energy;
	}
      else
	{
    variables[i] = old;
	}

      double new_Energy_good = calcEnergy(1);
      prob = exp(-(new_Energy_good-Energy_good)/(k*temperature));
      if (new_Energy_good < Energy_good || rand() <= prob)
  {
    Energy_good = new_Energy;
  }
      else
  {
    variables_good[i] = old_good;
  }

  if ((new_Energy < Energy) || (new_Energy_good < Energy_good)){
    if(new_Energy < new_Energy_good) {
      variables_best[i] = variables[i];
      Energy_best = new_Energy;
    }
    else {
      variables_best[i] = variables_good[i];
      Energy_best = new_Energy_good;
    }
    }
  }
  return(Energy);
}

double CSimAnneal::calcEnergy(bool good)
{
  double energy = 0;
  for (unsigned int i=0; i < measurements.size(); i++)
    {
      CMeasurement m = measurements[i];
      if(good) {
        energy += pow(m.temp - measModelGood(m.t,m.x,m.y),2);
      }
      else{
        energy += pow(m.temp - measModel(m.t,m.x,m.y),2);
      }
    }
  energy /= measurements.size();
  energy = sqrt(energy);
  return(energy);
}

double CSimAnneal::measModel(double t, double x, double y)
{
  double offset     = variables[0];
  double angle      = variables[1];
  double amplitude  = variables[2];
  double period     = variables[3];
  double wavelength = variables[4];
  double alpha      = variables[5];
  double beta       = variables[6];
  double temp_N     = variables[7];
  double temp_S     = variables[8];

  front.setVars(offset,angle,amplitude,period,wavelength,alpha,beta,temp_N,temp_S);
  // here you put your intelligent model of the temeperature field
  double temp = front.tempFunction(t,x,y);
  return(temp) ;
}

double CSimAnneal::measModelGood(double t, double x, double y)
{
  double offset     = variables_good[0];
  double angle      = variables_good[1];
  double amplitude  = variables_good[2];
  double period     = variables_good[3];
  double wavelength = variables_good[4];
  double alpha      = variables_good[5];
  double beta       = variables_good[6];
  double temp_N     = variables_good[7];
  double temp_S     = variables_good[8];

  front.setVars(offset,angle,amplitude,period,wavelength,alpha,beta,temp_N,temp_S);
  // here you put your intelligent model of the temeperature field
  double temp = front.tempFunction(t,x,y);
  return(temp) ;
}



