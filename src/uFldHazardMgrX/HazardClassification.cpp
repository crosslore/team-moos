#include "HazardClassification.h"
#include "uFldHazardMgrX.h"
#include "MBUtils.h"
#include <cstdint>
#include <sstream>
#include <unistd.h>
#include <stdlib.h>
#include <list>


HazardClassification::HazardClassification()
{
	m_v1_benign_count = 0;
	m_v1_hazard_count = 0;
	m_v2_benign_count = 0;
	m_v2_hazard_count = 0;
	m_probability = 0;
	m_priority = 100000;
	m_update = false;

}

HazardClassification::~HazardClassification()
{
}


