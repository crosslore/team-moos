#include "PointParse.h"
#include "PointAssign.h"
#include "MBUtils.h"
#include <cstdint>
#include<sstream>
#include<unistd.h>
#include <stdlib.h>


PointParse::PointParse(string v) {

  //Initialize Variables
  m_point = v;
  if((v!="firstpoint") && (v!="lastpoint")) {
    m_x = tokStringParse(v, "x", ',', '=');
    m_y = tokStringParse(v, "y", ',', '=');
    m_id = tokStringParse(v, "id", ',', '=');
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

PointParse::~PointParse()
{
}

void PointParse::set_m_id(string tmp) {
  m_id = tmp;

}

bool PointParse::operator<(const PointParse& somePoint) const
{
  double x1,x2;


 x1 = atof(m_x.c_str());
 x2 = atof(somePoint.m_x.c_str());

// cout << x1 << endl;
// cout << x2 << endl;

 if(x1 < x2)
    return(true);

 return(false);
}


// struct PointParse::Comparator() {

//   bool operator()(const PointParse& first, const PointParse& second) const {
//     double x1 = atof(first.m_x.c_str());
//     double x2 = atof(second.m_x.c_str());

//     // Return true if first should go before second

//     if(x1<x2) {
//       return true;
//     }
//     return false;
//   }

// }


string PointParse::getReport() { // Collect data and publish report

  // return m_point;
  return m_id;
}

