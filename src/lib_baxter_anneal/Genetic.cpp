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
/*                                                              */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/


#include "Genetic.h"

using namespace std;

Genetic::Genetic()
{
  got_min = false;
  got_max = false;
  got_init = false;
  check = true;
  count = 0;
  Energy = 0;

  max_it = 10000;
  popsize = 8;             //set population size
  mutrate = 0.1;            //set mutation rate
  cselection = 0.5;         //fraction of population kept
}

Genetic::~Genetic()
{
}

bool Chromosome::operator<(const Chromosome& someChrom) const
{
  double d1,d2;


 d1 = cost;
 d2 = someChrom.cost;

 if(d1 < d2)
    return(true);

 return(false);
}

void Genetic::updateParam(int param, int min, int max, int guess)
{

setMinVal(min,param);
setMaxVal(max,param);

}


void Genetic::setVars( int num, double temp_fac, bool adapt)
{
  num_vars = num;
  k = temp_fac;
  adaptive = adapt;

  selection = cselection;               //fraction of population kept
  Nt = num_vars;                        //continuous parameter GA Nt=#variables
  keep = floor(selection*popsize);;     //population members that survive
  nmut = ceil((popsize-1)*Nt*mutrate);; //set mutation number
  M = ceil((popsize-keep)/2);           //number of matings
  iga = 0;                              //generation counter initialized
  
  //Initialize population empty shell
  for (int i = 0; i < popsize; i++) { 
    Chromosome chrom;
    population.push_back(chrom);
  } 

  //Calc Reproduction Priority Weights
  int prob_den = cumlsum(keep);
  double cumprob;
  double pos;
    for (int i = 1; i <= keep; ++i)
  {
    pos = i;
    prob.push_back(pos/prob_den);
  }
  reverse(prob.begin(), prob.end()); // Most chance of top rank
  odds.push_back(0);
  for (int i = 0; i <keep; ++i)
  {
    cumprob += prob[i];
    odds.push_back(cumprob);
  }


  //Initialize Mating vector empty shells
  for (int i = 0; i < M; ++i)
  {
    pick1.push_back(0);
    pick2.push_back(0);
    ma.push_back(0);
    pa.push_back(0);
    xp.push_back(0);
    mix.push_back(0);
    // spread.push_back(0);
  }

  //Initialize Mate 1 indicies
  for (int i = 0; i < keep; i+=2)
  {
    ix.push_back(i+keep);
  }
for (int i = 0; i < nmut; ++i)
{
  mut_row.push_back(ceil(Ran.rand()*(popsize-1)));
  mut_col.push_back(floor(Ran.rand()*(Nt)));
}
}

bool Genetic::setInitVal(vector<double> var_init)
{
  if (num_vars != var_init.size())
    {
      cout << ">>> setInitVal: Mismatch in number of variables <<<\n" << endl ;
	return(false);
    }
  variables = var_init;
  variables_best = var_init;
  got_init = true;

  // Initialize a random population
  for (int i = 0; i < popsize; i++) { 
    for (int j = 0; j < var_init.size(); j++) { 
      double m = var_min[j]+Ran.rand()*(var_max[j]-var_min[j]);
      population[i].variables.push_back(m);
    }
  }

  return(true);
}

bool Genetic::setMinVal(vector<double> var)
{
  if (num_vars != var.size())
    {
      cout << ">>> setMinVal: Mismatch in number of variables <<<\n" << endl ;
	return(false);
    }
  var_min = var;
  got_min = true;
  return(true);
}

bool Genetic::setMaxVal(vector<double> var)
{
  if (num_vars != var.size())
    {
      cout << ">>> setMaxVal: Mismatch in number of variables <<<\n" << endl ;
	return(false);
    }
  var_max = var;
  got_max = true;
  return(true);
}

bool Genetic::setMinVal(int var, int i)
{
  var_min[i] = var;
  return(true);
}

bool Genetic::setMaxVal(int var, int i)
{
  var_max[i] = var;
  return(true);
}

int Genetic::cumlsum(int n)
{
  return (n == 1) ? 1 : cumlsum(n - 1) + n;
}

void Genetic::getEstimate(vector<double>& var_est, bool good)
{
    var_est = population[0].variables; 
}

void Genetic::clearMeas()
{
  measurements.clear();
}

void Genetic::addMeas(Measurement new_meas)
{
  int num_meas = measurements.size(); 
  measurements.push_back(new_meas);

  cout << ">>> Num_Meas=" << num_meas+1 << " new_meas.temp=" << new_meas.temp << endl; 
  cout << "t,x,y=" << new_meas.t << "," << new_meas.x << "," << new_meas.y 
       << endl;
}

Measurement Genetic::parseMeas(string report)
{
//---------------------------------------------------------
// Procedure: parseMeas
//   Example: utc=123456789,x=100.0,y=200.5,temp=20.7

  // Part 1: Parse the measurement

  Measurement buf;
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


string Genetic::run()
{


  count +=1;
  string tmp;
  // for (int i = 0; i < Nt; ++i)
  // {
  //   tmp += to_string(population[0].variables[i]);
  //   tmp += "   ";  
  // }

  // if((check) && (count>400)) {
  //   calcCost();
  //   sort(population.begin(),population.end());
  //   check = false;

  //   //Pair and Mate
  //   mate();
  //   mutate();

  // }

    if((check) && (count>1400)) {
    check = false;


  calcCost();
  sort(population.begin(),population.end());
for (int i = 0; i < max_it; ++i)
{
  
  mate();
  mutate();
  calcCost();
  sort(population.begin(),population.end());

}


  //   for (int i = 0; i < popsize; ++i)
  // {
  //   tmp += to_string(population[i].cost);
  //   tmp += "   ";  
  // }
  }

    for (int i = 0; i < Nt; ++i)
  {
    tmp += to_string(population[0].variables[i]);
    // tmp += to_string(mix[i]);
    tmp += "   ";  
  }

  tmp += to_string(population[0].cost);

    return(tmp);
  
  // else {
  //   return(10.3);
  // }
}

double Genetic::calcEnergy(int chrom)
{
  double energy = 0;
  for (unsigned int i=0; i < measurements.size(); i++)
    {
      Measurement m = measurements[i];
      energy += pow(m.temp - measModel(m.t,m.x,m.y,chrom),2);
    }
  energy /= measurements.size();
  energy = sqrt(energy);
  return(energy);
}

void Genetic::calcCost()
{
  for (int i = 0; i < popsize; i++) { 
    population[i].cost = calcEnergy(i);
  }

}

void Genetic::mate()
{

for (int i = 0; i < M; ++i)
{
  pick1[i] = Ran.rand();
  pick2[i] = Ran.rand();
}
// ma and pa contain the indicies of the chromosomes that will mate
for (int ic = 0; ic < M; ++ic)
{
  for (int id = 1; id <= keep; ++id)
  {
    if((pick1[ic]<=odds[id]) && (pick1[ic]>odds[id-1])) {
      ma[ic]=id-1;
    }
    if((pick2[ic]<=odds[id]) && (pick2[ic]>odds[id-1])) {
      pa[ic]=id-1;
    }
  }
}

// Performs mating using blended crossover
for (int i = 0; i < M; ++i)
{
  xp[i] = floor(Ran.rand()*Nt); //crossover point
  mix[i] = Ran.rand();          //mixing parameter
}

double xy;
for (int i = 0; i < M; ++i)
{
  // xy = population[ma[i]].variables[xp[i]]-population[pa[i]].variables[xp[i]];
  //MATE
  for (int j = 0; j < Nt; ++j)
  {
    population[ix[i]].variables[j] = population[ma[i]].variables[j]*mix[i]+population[pa[i]].variables[j]*(1-mix[i]);
    population[ix[i]+1].variables[j] = population[pa[i]].variables[j]*mix[i]+population[ma[i]].variables[j]*(1-mix[i]);
  }
}

}

void Genetic::mutate()
{
  for (int i = 0; i < nmut; ++i)
  {
  mut_row[i] = ceil(Ran.rand()*(popsize-1));
  mut_col[i] = floor(Ran.rand()*(Nt));
  }

  for (int i = 0; i < nmut; ++i)
  {
    population[mut_row[i]].variables[mut_col[i]] = var_min[mut_col[i]]+Ran.rand()*(var_max[mut_col[i]]-var_min[mut_col[i]]);
  }
}

double Genetic::measModel(double t, double x, double y, int chrom)
{
  double offset     = population[chrom].variables[0];
  double angle      = population[chrom].variables[1];
  double amplitude  = population[chrom].variables[2];
  double period     = population[chrom].variables[3];
  double wavelength = population[chrom].variables[4];
  double alpha      = population[chrom].variables[5];
  double beta       = population[chrom].variables[6];
  double temp_N     = population[chrom].variables[7];
  double temp_S     = population[chrom].variables[8];

  front.setVars(offset,angle,amplitude,period,wavelength,alpha,beta,temp_N,temp_S);
  // here you put your intelligent model of the temeperature field
  double temp = front.tempFunction(t,x,y);
  return(temp) ;
}





