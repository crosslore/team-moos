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
   double      m_v1_pc;
   double      m_v2_pc;
   string      m_class;
      

   int         m_v1_benign_count;
   int         m_v1_hazard_count;

 };
 #endif 
