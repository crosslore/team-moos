// File: HazardClassification.h

#include <string> 
#include <vector> 
#include <cstdint>
#include <list>

 #ifndef HAZARD_CLASSIFICATION_HEADER
 #define HAZARD_CLASSIFICATION_HEADER

using namespace std;

 class HazardClassification
 {

 public:

   HazardClassification() {};
   ~HazardClassification() {};

//Functions
   string  getReport();


//Data
   string      m_label;
   string      m_class;
   double      m_x;
   double      m_y;
   double      m_probability; 
   int         m_priority;  

   int         m_v1_benign_count;
   int         m_v1_hazard_count;
   bool        m_update;

 };
 #endif 
