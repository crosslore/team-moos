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



string CompPath::getReport() { // Collect data and publish report

  // return m_point;
  return m_id;
}

