//=============================================================================
//
//   File : KviEnvironment.cpp
//   Creation date : Sat May 05 2002 02:15:21 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma@kvirc.net)
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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#define _KVI_ENV_CPP_

#include "KviEnvironment.h"
#include "KviCString.h"
#include "KviMemory.h"

namespace KviEnvironment
{

#if !defined(COMPILE_ON_WINDOWS) && ! defined(COMPILE_ON_MINGW)

#ifdef HAVE_SETENV
bool setVariable(const char * name,const char * value)
{
	return (setenv(name,value,1) == 0);
#else
	#ifdef HAVE_PUTENV
bool setVariable(const char * name,const char * value)
{
		int iLen1 = kvi_strLen(name);
		int iLen2 = kvi_strLen(value);
		char * buf = (char *)KviMemory::allocate(iLen1 + iLen2 + 2);
		KviMemory::move(buf,name,iLen1);
		*(buf + iLen1) = '=';
		KviMemory::move(buf + iLen1 + 1,value,iLen2);
		*(buf + iLen1 + iLen2 + 1) = '\0';
		int iRet = putenv(buf);
		if(iRet != 0)
		{
			KviMemory::free(buf);
			return false;
		}
		return true;
	#else
bool setVariable(const char *,const char *)
{
		// no setenv, no putenv.. what the hell of system is this ?
		return false;
	#endif
#endif
}

#ifdef HAVE_UNSETENV
void unsetVariable(const char * name)
{
	unsetenv(name);
#else
	#ifdef HAVE_PUTENV
void unsetVariable(const char * name)
{
		int iLen1 = kvi_strLen(name);
		char * buf = (char *)KviMemory::allocate(iLen1 + 1);
		KviMemory::move(buf,name,iLen1);
		*(buf + iLen1) = '\0';
		int iRet = putenv(buf);
		if(iRet != 0)
		{
			KviMemory::free(buf);
		} else {
			// hmmm
			if(getVariable(name) == 0)
			{
				// ok, the string is not in the environment
				// we can free it
				KviMemory::free(buf);
			} // else this system sux
		}
	#else
void unsetVariable(const char *)
{
		// no setenv, no putenv.. what the hell of system is this ?
	#endif
#endif
}

#endif //COMPILE_ON_WINDOWS

}

