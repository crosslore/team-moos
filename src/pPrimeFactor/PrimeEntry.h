// File: PrimeEntry.h

#include <string> 
#include <vector> 
#include <cstdint>

 #ifndef PRIME_ENTRY_HEADER
 #define PRIME_ENTRY_HEADER

using namespace std;

 class PrimeEntry
 {

 public:

   friend class PrimeFactor;
   PrimeEntry(uint64_t v);
   ~PrimeEntry();

 protected:
//Functions
   bool   done() {return(m_done);};
   void factor(void);
   string  getReport();

//Data
   uint64_t      m_start_index;
   uint64_t      m_orig;
   uint64_t      m_current;
   uint64_t      m_divisor;
   double        m_start;
   double        m_time;
   bool          m_done;
   bool          m_evens;
   unsigned int  m_received_index;
   unsigned int  m_calculated_index;
   vector<uint64_t> m_factors;
 };
 #endif 
