#ifndef _KVI_TIME_H_
#define _KVI_TIME_H_
//=============================================================================
//
//   File : KviTimeUtils.h
//   Creation date : Tue Sep 25 17:28:46 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your option) any later version.
//
//   This program is distributed in the HOPE that it will be USEFUL,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program. If not, write to the Free Software Foundation,
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

/**
* \file KviTimeUtils.h
* \author Szymon Stefanek
* \brief Time management
*/

#include "kvi_settings.h"

#include <time.h> // for time()

class QString;

#define kvi_unixTime() time(0)
#define kvi_timeSpan(_time_now, _time_before) ((_time_now) - (_time_before))
#define kvi_secondsSince(_that_time_t) kvi_timeSpan(kvi_unixTime(), _that_time_t)

#define kvi_time_t time_t

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
#include <winsock2.h> // struct timeval

/**
	* \brief Gets the time of the day
	* \param tmv The time value
	* \param tmz The timezone
	* \return void
	*/
extern KVILIB_API void kvi_gettimeofday(struct timeval * tmv);
#else
#include <sys/time.h> // gettimeofday(), struct timeval

/**
	* \brief Gets the time of the day
	* \param tmv The time value
	* \param tmz The timezone
	* \return void
	*/
inline void kvi_gettimeofday(struct timeval * tmv)
{
	gettimeofday(tmv, nullptr);
}
#endif

/**
* \class KviMSecTimeInterval
* \brief A class to retrieve the time interval
* This works for time intervals a bit longer than 24 days
*/
class KVILIB_API KviMSecTimeInterval
{
public:
	KviMSecTimeInterval();

protected:
	unsigned long m_uReferenceSecs;
	unsigned long m_uReferenceUSecs;

public:
	/**
	* \brief Returns the number of milliseconds.
	* The amount is calculated since mark() was last called (and thus marks
	* the beginning of a new interval).
	* \return unsigned long
	*/
	unsigned long mark();

	/**
	* \brief Returns the number of seconds
	* This wors ONLY in the same second that mark was called in and returns the
	* tv_sec field of the gettimeofday() (remember that gettimeofday() is broken
	* on windows)
	* \return unsigned long
	*/
	unsigned long secondsCounter() { return m_uReferenceSecs; };
};

/**
* \namespace KviTimeUtils
* \brief Time utilities
*/
namespace KviTimeUtils
{
	/**
	* \brief Returns the current time mills
	* \return long long
	*/
	KVILIB_API long long getCurrentTimeMills();

	/**
	* \brief Splits the time span uSecs in days, hours, minutes and seconds
	* \param uSecs The amount of seconds to span
	* \param uD Where to store days data
	* \param uH Where to store hours data
	* \param uM Where to store minutes data
	* \param uS Where to store seconds data
	* \return void
	*/
	KVILIB_API void secondsToDaysHoursMinsSecs(unsigned int uSecs,
	    unsigned int * uD, unsigned int * uH, unsigned int * uM, unsigned int * uS);

	/** \enum FormatTimeSpanFlags */
	enum FormatTimeSpanFlags
	{
		NoLeadingEmptyIntervals = 1, /**< Causes the leading empty intervals to be omitted */
		NoLeadingZeroes = 2,         /**< No leading zeroes are printed in hours and seconds */
		FillWithHypens = 4           /**< Uses only -- %d -- %h -- etc.. discards all other flags */
	};

	/**
	* \brief Returns a string formatted like x d x h xx m xx s
	* \param uSeconds The number of seconds to format
	* \param iFlags The flags to format the time string
	* \return QString
	*/
	KVILIB_API QString formatTimeInterval(unsigned int uSeconds, int iFlags = 0);
}

#endif //_KVI_TIME_H_
