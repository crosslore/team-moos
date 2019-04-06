// File: HazardClassification.h

#include <string> 
#include <vector> 
#include <cstdint>

 #ifndef HAZARD_CLASSIFICATION_HEADER
 #define HAZARD_CLASSIFICATION_HEADER

using namespace std;

 class HazardClassification
 {

 public:

   friend class HazardMgr;
   HazardClassification(string str);
   ~HazardClassification();

 protected:
//Functions
   string  getReport();


//Data
   string      m_label;
   string      m_v1_pc;
   string      m_v2_pc;
   int         m_v1_benign_count;
   int         m_v1_hazard_count;
   int         m_v2_benign_count;
   int         m_v2_hazard_count;
 };
 #endif 
