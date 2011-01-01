#ifndef _KVI_ENV_H_
#define _KVI_ENV_H_

//=============================================================================
//
//   File : KviEnvironment.h
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

/**
* \file KviEnvironment.h
* \author Szymon Stefanek
* \brief Environment function wrappers
*/

#include "kvi_settings.h"

#include <stdlib.h>

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	#include <windows.h>
#endif

namespace KviEnvironment
{

	/**
	* \brief Gets environment variable
	* \param name The name of the variable to get
	* \return char *
	*/
	inline char * getVariable(const char * name)
	{
		return getenv(name);
	}

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)

	inline void setVariable(const char * szName,const char * szValue)
	{
		SetEnvironmentVariable(szName,szValue);
	}
	
	inline void unsetVariable(const char * name)
	{
		SetEnvironmentVariable(name,NULL);
	}
#else
	/**
	* \brief Sets environment variable
	* \param name The name of the variable to set
	* \param value The value of the variable
	* \return bool
	*/
	KVILIB_API bool setVariable(const char * name, const char * value);

	/**
	* \brief Unsets environment variable
	* \param name The name of the variable to set
	* \return void
	*/
	KVILIB_API void unsetVariable(const char * name);
#endif

}

#endif //_KVI_ENV_H_
