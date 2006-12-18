#ifndef _KVI_TIME_H_
#define _KVI_TIME_H_

//=============================================================================
//
//   File : kvi_time.h
//   Creation date : Tue Sep 25 17:28:46 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your opinion) any later version.
//
//   This program is distributed in the HOPE that it will be USEFUL,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program. If not, write to the Free Software Foundation,
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#include "kvi_settings.h"
#include <qstring.h>


#include <time.h> // for time()

#define kvi_unixTime() time(0)
#define kvi_timeSpan(_time_now,_time_before) ((_time_now) - (_time_before))
#define kvi_secondsSince(_that_time_t) kvi_timeSpan(kvi_unixTime(),_that_time_t)

#define kvi_time_t time_t

#ifdef COMPILE_ON_WINDOWS

	#include <winsock2.h> // struct timeval

	extern KVILIB_API void kvi_gettimeofday(struct timeval * tmv,struct timezone * tmz);

#else //!COMPILE_ON_WINDOWS

	#include <sys/time.h> // gettimeofday() , struct timeval

	inline void kvi_gettimeofday(struct timeval * tmv,struct timezone * tmz)
	{
		gettimeofday(tmv,tmz);
	};

#endif //!COMPILE_ON_WINDOWS

// this works for time intervals a bit longer than 24 days
class KVILIB_API KviMSecTimeInterval
{
public:
	KviMSecTimeInterval();
protected:
	unsigned long m_uReferenceSecs;
	unsigned long m_uReferenceUSecs;
public:
	// returns the number of milliseconds since
	// mark() was last called (and thus marks
	// the beginning of a new interval).
	unsigned long mark();
	// this wors ONLY in the same second that mark was called in
	// and returns the tv_sec field of the gettimeofday()
	// (remember that gettimeofday() is broken on windows)
	unsigned long secondsCounter(){ return m_uReferenceSecs; };
};

namespace KviTimeUtils
{


	// splits the time span uSecs in days, hours, minutes and seconds
	KVILIB_API void secondsToDaysHoursMinsSecs(unsigned int uSecs,
					unsigned int * uD,unsigned int * uH,unsigned int * uM,unsigned int * uS);
	// returns a string formatted like x d x h xx m xx s
	enum FormatTimeSpanFlags {
		NoLeadingEmptyIntervals = 1,    // causes the leading empty intervals to be omitted
		NoLeadingZeroes = 2,            // no leading zeroes are printed in hours and seconds
		FillWithHypens = 4              // uses only -- %d -- %h -- etc.. discards all other flags
	};
	KVILIB_API QString formatTimeInterval(unsigned int uSeconds,int iFlags = 0);
};

#endif //_KVI_TIME_H_
