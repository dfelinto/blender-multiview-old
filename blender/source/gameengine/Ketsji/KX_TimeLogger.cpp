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

#include "KX_TimeLogger.h"



KX_TimeLogger::KX_TimeLogger(unsigned int maxNumMeasurements)
: m_maxNumMeasurements(maxNumMeasurements), m_logging(false), m_logStart(0)
{
}


KX_TimeLogger::~KX_TimeLogger(void)
{
}


void KX_TimeLogger::SetMaxNumMeasurements(unsigned int maxNumMeasurements)
{
	if ((m_maxNumMeasurements != maxNumMeasurements) && maxNumMeasurements) {
		// Actual removing is done in NextMeasurement()
		m_maxNumMeasurements = maxNumMeasurements;
	}
}


unsigned int KX_TimeLogger::GetMaxNumMeasurements(void) const
{
	return m_maxNumMeasurements;
}


void KX_TimeLogger::StartLog(double now)
{
	if (!m_logging) {
		m_logging = true;
		m_logStart = now;
	}
}


void KX_TimeLogger::EndLog(double now)
{
	if (m_logging) {
		m_logging = false;
		double time = now - m_logStart;
		if (m_measurements.size() > 0) {
			m_measurements[0] += time;
		}
	}
}


void KX_TimeLogger::NextMeasurement(double now)
{
	// End logging to current measurement
	EndLog(now);

	// Add a new measurement at the front
	double m = 0.;
	m_measurements.push_front(m);

	// Remove measurement if we grow beyond the maximum size
	if ((m_measurements.size()) > m_maxNumMeasurements) {
		while (m_measurements.size() > m_maxNumMeasurements) {
			m_measurements.pop_back();
		}
	}
}



double KX_TimeLogger::GetAverage(void) const
{
	double avg = 0.;

	unsigned int numMeasurements = m_measurements.size();
	if (numMeasurements > 1) {
		for (int i = 1; i < numMeasurements; i++) {
			avg += m_measurements[i];
		}
		avg /= (float)numMeasurements - 1;
	}

	return avg;
}

