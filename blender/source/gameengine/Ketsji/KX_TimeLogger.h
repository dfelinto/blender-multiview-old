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

#ifndef __KX_TIME_LOGGER_H
#define __KX_TIME_LOGGER_H

#ifdef WIN32
#pragma warning (disable:4786) // suppress stl-MSVC debug info warning
#endif


#include <deque>

/**
 * Stores and manages time measurements.
 */
class KX_TimeLogger {
public:
	/**
	 * Constructor.
	 * @param maxNumMesasurements Maximum number of measurements stored (>1).
	 */
	KX_TimeLogger(unsigned int maxNumMeasurements = 10);

	/**
	 * Destructor.
	 */
	virtual ~KX_TimeLogger(void);

	/**
	 * Changes the maximum number of measurements that can be stored.
	 */
	virtual void SetMaxNumMeasurements(unsigned int maxNumMeasurements);

	/**
	 * Changes the maximum number of measurements that can be stored.
	 */
	virtual unsigned int GetMaxNumMeasurements(void) const;

	/**
	 * Starts logging in current measurement.
	 * @param now	The current time.
	 */
	virtual void StartLog(double now);

	/**
	 * End logging in current measurement.
	 * @param now	The current time.
	 */
	virtual void EndLog(double now);

	/**
	 * Logs time in next measurement.
	 * @param now	The current time.
	 */
	virtual void NextMeasurement(double now);

	/**
	 * Returns average of all but the current measurement.
	 * @return The average of all but the current measurement.
	 */
	virtual double GetAverage(void) const;

protected:
	/** Storage for the measurements. */
	std::deque<double> m_measurements;

	/** Maximum number of measurements. */
	unsigned int m_maxNumMeasurements;

	/** Time at start of logging. */
	double m_logStart;

	/** State of logging. */
	bool m_logging;
};

#endif // __KX_TIME_LOGGER_H
