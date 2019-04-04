#include "PrimeEntry.h"
#include "PrimeFactor.h"
#include "MBUtils.h"
#include <cstdint>
#include<sstream>
#include<unistd.h>

int Registered = 0;
int Calculated = 0;


PrimeEntry::PrimeEntry(uint64_t v) {

  //Initialize Variables
  m_orig = v;
  m_divisor = 2;
  m_current = v;
  m_start = MOOSTime(); 
  Registered +=1; //Index Global count of registered primes prior to assigning to obj
  m_received_index = Registered;
  m_done = 0;
  m_evens = 0;
}

PrimeEntry::~PrimeEntry()
{
}


void PrimeEntry::factor(void)
{
int max = 10000; //Only limits iterating through possible odd factors
int count = 0;

//Forces all of the 2's to be factored without counting. This will never be very many.
  while(!(m_current % 2)) {
  	m_factors.push_back(m_divisor);
  	m_current /=2;
  }

//Stages divisor to only increment odd numbers for efficiency since all factors of 2 removed.
  if(!m_evens) {
  m_divisor +=1;
  m_evens = 1;  	
  }

//Checking for odd factors
  for (int i = m_divisor; i <= sqrt(m_current); i = i+2) 
    { 
        while ((m_current%i == 0) && (count < max))
        { 
            m_factors.push_back(m_divisor);
            m_current /=i; 
            count++;
        } 
        m_divisor +=2;
    } 

//Redundant to check for factors past the square root
    if(m_divisor > sqrt(m_current)) {
    	m_factors.push_back(m_current);
    	m_current /=m_current; 
    }

//Flag completion
	if (m_current == 1) {
      m_done = 1;
    }

}

string PrimeEntry::getReport() { // Collect data and publish report

  Calculated +=1; //Increment global count
  m_calculated_index = Calculated;
  m_time = MOOSTime() - m_start; //Time to compute factors

  stringstream ss;
  ss << m_orig;
  string orig = ss.str();

  stringstream ss2;
  ss2 << m_received_index;
  string received_index = ss2.str();

  stringstream ss3;
  ss3 << m_calculated_index;
  string calculated_index = ss3.str();

  stringstream ss4;
  ss4 << m_time;
  string time = ss4.str();
  
  stringstream ss5;
  string factors;
  for (vector<uint64_t>::iterator it = m_factors.begin() ; it != m_factors.end(); ++it) {
    ss5 << *it;
    ss5 << ":";  
  }

  factors += ss5.str();
  factors.pop_back(); //All  but last iteration
  string str = "orig=" + orig + ",recived=" + received_index + ",calculated=" + calculated_index + ",solve_time=" + time + ",primes=" + factors + ",username=david\n";
  return str;
}
