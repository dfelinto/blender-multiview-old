/**
 * $Id$
 *
 * ***** BEGIN GPL/BL DUAL LICENSE BLOCK *****
 *
 * The contents of this file may be used under the terms of either the GNU
 * General Public License Version 2 or later (the "GPL", see
 * http://www.gnu.org/licenses/gpl.html ), or the Blender License 1.0 or
 * later (the "BL", see http://www.blender.org/BL/ ) which has to be
 * bought from the Blender Foundation to become active, in which case the
 * above mentioned GPL option does not apply.
 *
 * The Original Code is Copyright (C) 2002 by NaN Holding BV.
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): none yet.
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 */

#include "KX_TimeCategoryLogger.h"


KX_TimeCategoryLogger::KX_TimeCategoryLogger(unsigned int maxNumMeasurements)
: m_maxNumMeasurements(maxNumMeasurements)
{
}


KX_TimeCategoryLogger::~KX_TimeCategoryLogger(void)
{
	DisposeLoggers();
}


void KX_TimeCategoryLogger::SetMaxNumMeasurements(unsigned int maxNumMeasurements)
{
	KX_TimeLoggerMap::iterator it;
	for (it = m_loggers.begin(); it != m_loggers.end(); it++) {
		it->second->SetMaxNumMeasurements(maxNumMeasurements);
	}
	m_maxNumMeasurements = maxNumMeasurements;
}


unsigned int KX_TimeCategoryLogger::GetMaxNumMeasurements(void) const
{
	return m_maxNumMeasurements;
}


void KX_TimeCategoryLogger::AddCategory(TimeCategory tc)
{
	// Only add if not already present
	if (m_loggers.find(tc) == m_loggers.end()) {
		KX_TimeLogger* logger = new KX_TimeLogger(m_maxNumMeasurements);
		//assert(logger);
		m_loggers.insert(KX_TimeLoggerMap::value_type(tc, logger));
	}
}


void KX_TimeCategoryLogger::StartLog(TimeCategory tc, double now, bool endOtherCategories)
{
	if (endOtherCategories) {
		KX_TimeLoggerMap::iterator it;
		for (it = m_loggers.begin(); it != m_loggers.end(); it++) {
			if (it->first != tc) {
				it->second->EndLog(now);
			}
		}
	}
	//assert(m_loggers[tc] != m_loggers.end());
	m_loggers[tc]->StartLog(now);
}


void KX_TimeCategoryLogger::EndLog(TimeCategory tc, double now)
{
	//assert(m_loggers[tc] != m_loggers.end());
	m_loggers[tc]->EndLog(now);
}


void KX_TimeCategoryLogger::EndLog(double now)
{
	KX_TimeLoggerMap::iterator it;
	for (it = m_loggers.begin(); it != m_loggers.end(); it++) {
		it->second->EndLog(now);
	}
}


void KX_TimeCategoryLogger::NextMeasurement(double now)
{
	KX_TimeLoggerMap::iterator it;
	for (it = m_loggers.begin(); it != m_loggers.end(); it++) {
		it->second->NextMeasurement(now);
	}
}


double KX_TimeCategoryLogger::GetAverage(TimeCategory tc)
{
	//assert(m_loggers[tc] != m_loggers.end());
	return m_loggers[tc]->GetAverage();
}


double KX_TimeCategoryLogger::GetAverage(void)
{
	double time = 0.;

	KX_TimeLoggerMap::iterator it;
	for (it = m_loggers.begin(); it != m_loggers.end(); it++) {
		time += it->second->GetAverage();
	}

	return time;
}


void KX_TimeCategoryLogger::DisposeLoggers(void)
{
	KX_TimeLoggerMap::iterator it;
	for (it = m_loggers.begin(); it != m_loggers.end(); it++) {
		delete it->second;
	}
}

