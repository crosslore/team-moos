#include "CompPath.h"
#include "GenPath.h"
#include "MBUtils.h"
#include <cstdint>
#include<sstream>
#include<unistd.h>
#include <stdlib.h>


CompPath::CompPath(string v) {

  //Initialize Variables
  m_point = v;
  if((v!="firstpoint") && (v!="lastpoint")) {
    m_x = tokStringParse(v, "x", ',', '=');
    m_y = tokStringParse(v, "y", ',', '=');
    m_id = tokStringParse(v, "id", ',', '=');
    m_prob = 0.5;
  }
  else if(v=="firstpoint"){
    m_x = "firstpoint";
    m_y = "firstpoint";
    m_id = "firstpoint";
  }
  else {
    m_x = "lastpoint";
    m_y = "lastpoint";
    m_id = "lastpoint";

  }


}

CompPath::~CompPath()
{
}

bool CompPath::operator<(const CompPath& somePoint) const
{
  double d1,d2;


 d1 = m_dist;
 d2 = somePoint.m_dist;

// cout << x1 << endl;
// cout << x2 << endl;

 if(d1 < d2)
    return(true);

 return(false);
}



void CompPath::setProb(double prob) {
  m_prob = prob;

}


string CompPath::getReport() { // Collect data and publish report

  // return m_point;
  return m_id;
}

