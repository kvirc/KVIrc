//=============================================================================
//
//   File : KviTimeUtils.cpp
//   Creation date : Tue Sep 25 17:35:13 2001 GMT by Szymon Stefanek
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

#include "KviTimeUtils.h"
#include "KviLocale.h"

#include <QString>

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)

// Call SystemTimeToFileTime to copy the system time to a FILETIME structure.
// Call GetSystemTime to get the current system time to pass to SystemTimeToFileTime.
// Copy the contents of the FILETIME structure to a ULARGE_INTEGER structure.
// Initialize a SYSTEMTIME structure with the date and time of the first second of January 1, 1970.
// Call SystemTimeToFileTime, passing the SYSTEMTIME structure initialized in Step 3 to the call.
// Copy the contents of the FILETIME structure returned by SystemTimeToFileTime in Step 4 to
// a second ULARGE_INTEGER. The copied value should be greater than or equal to the value copied
// in Step 2. Subtract the 64-bit value in the ULARGE_INTEGER structure initialized in Step 2
// from the 64-bit value of the ULARGE_INTEGER structure initialized in Step 5.
// This produces a value in 100-nanosecond intervals since January 1, 1970.
// To convert this value to seconds, divide by 10,000,000.

// buah buah buahhhh lol ghgh :DDDDDDDDD

void kvi_gettimeofday(struct timeval * tmv)
{
	SYSTEMTIME st;
	GetSystemTime(&st);

	// this is simply fucked up..
	// to minimize the possibility of wrapping we use also the day field.
	// we actually give something that is near the number of seconds from the beginning
	// of the current month...
	// We cannot use the wMonth field since the months have variable length :/
	tmv->tv_sec = (st.wDay * 86400) + (st.wHour * 3600) + (st.wMinute * 60) + (st.wSecond);
	tmv->tv_usec = st.wMilliseconds * 1000;
}
#endif

KviMSecTimeInterval::KviMSecTimeInterval()
{
	m_uReferenceSecs = 0;
	m_uReferenceUSecs = 0;
}

unsigned long KviMSecTimeInterval::mark()
{
	struct timeval tmv;
	kvi_gettimeofday(&tmv);
	unsigned long uDiff = ((((unsigned long)(tmv.tv_sec)) - m_uReferenceSecs) * 1000);
	if(((unsigned long)(tmv.tv_usec)) > m_uReferenceUSecs)
		uDiff += (((unsigned long)(tmv.tv_usec) - m_uReferenceUSecs) / 1000);
	else
		uDiff -= ((m_uReferenceUSecs - (unsigned long)(tmv.tv_usec)) / 1000);
	m_uReferenceSecs = (unsigned long)tmv.tv_sec;
	m_uReferenceUSecs = (unsigned long)tmv.tv_usec;
	return uDiff;
}

namespace KviTimeUtils
{
	long long getCurrentTimeMills()
	{
		struct timeval tmv;
		kvi_gettimeofday(&tmv);
		long long result = tmv.tv_sec * 1000 + tmv.tv_usec / 1000;
		return result;
	}

	void secondsToDaysHoursMinsSecs(unsigned int uSecs,
	    unsigned int * uD, unsigned int * uH, unsigned int * uM, unsigned int * uS)
	{
		*uD = uSecs / 86400;
		uSecs = uSecs % 86400;
		*uH = uSecs / 3600;
		uSecs = uSecs % 3600;
		*uM = uSecs / 60;
		*uS = uSecs % 60;
	}

	QString formatTimeInterval(unsigned int uSeconds, int iFlags)
	{
		unsigned int d, h, m, s;
		secondsToDaysHoursMinsSecs(uSeconds, &d, &h, &m, &s);
		QString szRet;
		// the following tricks maybe will help translators a bit...
		if(iFlags & FillWithHypens)
		{
			szRet = __tr2qs("- d -- h -- m -- s");
		}
		else
		{
			if((iFlags & NoLeadingEmptyIntervals) && (d == 0))
			{
				if(h > 0)
				{
					if(iFlags & NoLeadingZeroes)
						szRet = QString(__tr2qs("%1 h %2 m %3 s")).arg(h).arg(m).arg(s);
					else
						szRet = QString(__tr2qs("%1 h %2%3 m %4%5 s")).arg(h).arg(m / 10).arg(m % 10).arg(s / 10).arg(s % 10);
				}
				else
				{
					if(m > 0)
					{
						if(iFlags & NoLeadingZeroes)
							szRet = QString(__tr2qs("%1 m %2 s")).arg(m).arg(s);
						else
							szRet = QString(__tr2qs("%1 m %2%3 s")).arg(m).arg(s / 10).arg(s % 10);
					}
					else
					{
						szRet = QString(__tr2qs("%1 s")).arg(s);
					}
				}
			}
			else
			{
				if(iFlags & NoLeadingZeroes)
					szRet = QString(__tr2qs("%1 d %2 h %3 m %4 s")).arg(d).arg(h).arg(m).arg(s);
				else
					szRet = QString(__tr2qs("%1 d %2%3 h %4%5 m %6%7 s")).arg(d).arg(h / 10).arg(h % 10).arg(m / 10).arg(m % 10).arg(s / 10).arg(s % 10);
			}
		}
		return szRet;
	}
}
