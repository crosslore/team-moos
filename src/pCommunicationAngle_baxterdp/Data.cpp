//CPP

#include "Data.h"
#include "CommunicationAngle_baxterdp.h"
#include "MBUtils.h"
#include <cstdint>
#include<sstream>
#include<unistd.h>
#include<cmath>

using namespace std;

Data::Data() {

  //Initialize Variables

}

Data::~Data()
{
}

void Data::updateOutput()
{
	m_R0 = 0;
	m_Z0 = m_depth0;
	getRange();
	m_Za = m_deptha;
	m_Zc = -m_c0/m_g;
	circCent();
	findRadius();
}

void Data::getRange()
{
	m_Ra = sqrt(pow(m_x0-m_xa,2)+pow(m_y0-m_ya,2));
}

void Data::circCent()
{
if (m_Z0 == m_Za){
	m_Rc = (m_R0+m_Ra)/2;
}
else {
	double b = (pow(m_Ra,2)+pow(m_Za,2)-pow(m_R0,2)-pow(m_Z0,2))/(2*(m_Za-m_Z0));
	double m = (m_R0-m_Ra)/(m_Za-m_Z0);
	m_Rc = (m_Zc-b)/m;
}
}

void Data::findRadius()
{
	m_Radius = sqrt(pow(m_Rc-m_Ra,2)+pow(m_Zc-m_Za,2));
	m_Rmax = abs(m_Zc)+m_depthw;
}

void Data::findAngle()
{
if(m_Za >= m_Z0) { //Guarantee that m_Rc >0
	double theta = atan((abs(m_Zc)+m_Z0)/m_Rc);
	double alpha = atan(abs(m_Z0-m_Za)/abs(m_R0-m_Ra));
	double beta = M_PI/2-theta-alpha;
	double theta_alpha = theta+2*beta;
	m_Angle = -(M_PI/2-theta)*180/M_PI;
	m_Arc = (m_Radius*(2*beta));
	m_Grazing = m_Angle+2*beta*180/M_PI;
	// m_test = m_Grazing;
}
else { 
	double rca = m_Ra-m_Rc;
	double theta = atan((abs(m_Zc)+m_Za)/rca);
	double alpha = atan(abs(m_Z0-m_Za)/abs(m_R0-m_Ra));
	double beta = M_PI/2-theta-alpha;
	double theta_0 = theta+2*beta;
	m_Angle = m_Grazing-2*beta*180/M_PI;
	m_Arc = (m_Radius*(2*beta));
	m_Grazing = (M_PI/2-theta)*180/M_PI;//+2*beta*180/M_PI;
	// m_test = m_Angle;
	}
}


void Data::transmissionLoss()
{
m_Loss = 20*log10(m_Arc/2);
// double j = pow(m_Arc,2);
// double p = sqrt(abs((m_c0+m_g*m_deptha)/((m_c0+m_g*m_depth0)*j)));
// double p1 = 1;
// m_Loss = -20*log10(p/p1);
}


string Data::makeReport()
{
if((m_Radius > m_Rmax) && (m_Ra > m_Radius)) {
	travel();
	m_AngleOut = "NaN";
}
else {

	stringstream ssx, ssy, ssd;
	ssx << m_xa;
	ssy << m_ya;
	ssd << m_deptha;
	string x, y, d;
	x = ssx.str();
	y = ssy.str();
	d = ssd.str();
	m_LocationOut = "x=" + x + ",y=" + y + ",depth=" + d +",id=baxterdp@mit.edu.";
	stringstream ss;
  	ss << -m_Grazing;
  	m_AngleOut = ss.str();
}

	stringstream ssl;
	ssl << m_Loss;
	string l;
	l = ssl.str();

return("elev_angle=" + m_AngleOut + ", transmission_loss=" + l);

}

void Data::travel()
{
	double Rcm = sqrt(pow(m_Rmax,2)+pow((abs(m_Zc)+m_Z0),2));
	double rca = m_Ra-Rcm;
	double theta = atan((abs(m_Zc)+m_Za)/rca);
	m_int_R_2D = Rcm+m_Rmax*cos(theta);
	m_int_Z_2D = m_Zc+m_Rmax*sin(theta);

	if(m_int_Z_2D < 0) { //Not UAV, Cant Fly
		m_int_Z_2D = 0;
		m_int_R_2D = Rcm+sqrt(pow(m_Rmax,2)-pow(m_Zc,2));
	}

	double Xn = m_x0 + m_int_R_2D*(m_xa-m_x0)/m_Ra; //m_Ra is range in x y plane
	double Yn = m_y0 + m_int_R_2D*(m_ya-m_y0)/m_Ra;

	stringstream ssx, ssy, ssd;
	ssx << Xn;
	ssy << Yn;
	ssd << m_int_Z_2D;
	string x, y, d;
	x = ssx.str();
	y = ssy.str();
	d = ssd.str();
	m_LocationOut = "x=" + x + ",y=" + y + ",depth=" + d +",id=baxterdp@mit.edu.";
}