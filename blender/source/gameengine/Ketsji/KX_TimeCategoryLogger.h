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

#ifndef __KX_TIME_CATEGORY_LOGGER_H
#define __KX_TIME_CATEGORY_LOGGER_H

#ifdef WIN32
#pragma warning (disable:4786) // suppress stl-MSVC debug info warning
#endif

#include <map>

#include "KX_TimeLogger.h"

/**
 * Stores and manages time measurements by category.
 * Categories can be added dynamically.
 * Average measurements can be established for each separate category
 * or for all categories together.
 */
class KX_TimeCategoryLogger {
public:
	typedef int TimeCategory;

	/**
	 * Constructor.
	 * @param maxNumMesasurements Maximum number of measurements stored (> 1).
	 */
	KX_TimeCategoryLogger(unsigned int maxNumMeasurements = 10);

	/**
	 * Destructor.
	 */
	virtual ~KX_TimeCategoryLogger(void);

	/**
	 * Changes the maximum number of measurements that can be stored.
	 */
	virtual void SetMaxNumMeasurements(unsigned int maxNumMeasurements);

	/**
	 * Changes the maximum number of measurements that can be stored.
	 */
	virtual unsigned int GetMaxNumMeasurements(void) const;

	/**
	 * Adds a category.
	 * @param category	The new category.
	 */
	virtual void AddCategory(TimeCategory tc);

	/**
	 * Starts logging in current measurement for the given category.
	 * @param tc					The category to log to.
	 * @param now					The current time.
	 * @param endOtherCategories	Whether to stop logging to other categories.
	 */
	virtual void StartLog(TimeCategory tc, double now, bool endOtherCategories = true);

	/**
	 * End logging in current measurement for the given category.
	 * @param tc	The category to log to.
	 * @param now	The current time.
	 */
	virtual void EndLog(TimeCategory tc, double now);

	/**
	 * End logging in current measurement for all categories.
	 * @param now	The current time.
	 */
	virtual void EndLog(double now);

	/**
	 * Logs time in next measurement.
	 * @param now	The current time.
	 */
	virtual void NextMeasurement(double now);

	/**
	 * Returns average of all but the current measurement time.
	 * @return The average of all but the current measurement.
	 */
	virtual double GetAverage(TimeCategory tc);

	/**
	 * Returns average for grand total.
	 */
	virtual double GetAverage(void);

protected:
	/**  
	 * Disposes loggers.
	 */  
	virtual void DisposeLoggers(void);

	/** Storage for the loggers. */
	typedef std::map<TimeCategory, KX_TimeLogger*> KX_TimeLoggerMap;
	KX_TimeLoggerMap m_loggers;
	/** Maximum number of measurements. */
	unsigned int m_maxNumMeasurements;

};

#endif // __KX_TIME_CATEGORY_LOGGER_H
