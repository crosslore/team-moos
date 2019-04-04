//Header
// File: Data.h

#include <string> 
#include <vector> 
#include <cstdint>

 #ifndef DATA_HEADER
 #define DATA_HEADER

using namespace std;

 class Data
 {

 public:
   friend class CommunicationAngle_baxterdp;
   Data();
   ~Data();

 protected:
//Functions
   // bool   done() {return(m_done);};
   // void factor(void);
   // string  getReport();

 	//Updating Data
   bool   set_x0(double v) {m_x0 = v; return(true);};
   bool   set_y0(double v) {m_y0 = v; return(true);};
   bool   set_depth0(double v) {m_depth0 = v; return(true);};
   bool   set_deptha(double v) {m_deptha = v; return(true);};
   bool   set_xa(double v) {m_xa = v; return(true);};
   bool   set_ya(double v) {m_ya = v; return(true);};
   bool   set_c0(double v) {m_c0 = v; return(true);};
   bool   set_g(double v) {m_g = v; return(true);};
   bool   set_depthw(double v) {m_depthw = v; return(true);};

   //Calculating
	void updateOutput(); //Updates Output Data members
	void getRange(); //Updates UAV range horizontal from transmitter
	void circCent(); //Gets center of sound transmission arc wrt ocean surface directly above the transmitter 
	void findRadius(); //Gets Radius of unique sound arc connecting transmitter to UAV and max Radius before bottom interference
	void findAngle(); //Gets arc length and dip angle at transmitter for arc connecting transmitter to UAV
	string makeReport(); //Calculate transmission angle or new location required
	void travel();
	void midpoint();
	void transmissionLoss();

//Data
	//Input Data
   double      	 m_x0; //Transmitter
   double        m_y0;
   double        m_xa; //UAV
   double        m_ya;
   double        m_depth0;
   double        m_deptha;
   double        m_c0;
   double        m_g;
   double        m_depthw;

   //Output Data
   double 		 m_Rc; //
   double 		 m_Zc;
   double 		 m_R0; //Transmitter Range = 0 since it is reference
   double 		 m_Z0; //Transmitter Depth
   double 		 m_Ra; //UAV Range to transmitter
   double 		 m_Za; //UAV Depth
   double 		 m_Radius;
   double		 m_Rmax; //Max radius before bottom interference
   double		 m_Angle; //Grazing angle at transmitter
   double 		 m_Arc; //Arclength of sound profile
   double		 m_Grazing;
   double		 m_Loss;
   double		 m_int_R_2D;
   double		 m_int_Z_2D;
   double		 m_int_X_3D;
   double		 m_int_Y_3D;
   string		 m_AngleOut;
   string		 m_LocationOut;

   double 		 m_test;



   // bool          m_evens;
   // unsigned int  m_received_index;
   // unsigned int  m_calculated_index;
   // vector<uint64_t> m_factors;
 };
 #endif 
