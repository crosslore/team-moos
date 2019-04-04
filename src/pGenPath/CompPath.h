// File: CompPath.h

#include <string> 
#include <vector> 
#include <cstdint>

 #ifndef COMP_PATH_HEADER
 #define COMP_PATH_HEADER

using namespace std;

 class CompPath
 {

 public:

   friend class GenPath;
   CompPath(string v);

   ~CompPath();

 protected:
//Functions
   string  getReport();


//Data
   string      m_point;
   string      m_x;
   string      m_y;
   string      m_id;
   string	   m_start;
 };
 #endif 
