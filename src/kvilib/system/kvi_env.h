#ifndef _KVI_ENV_H_
#define _KVI_ENV_H_

//=============================================================================
//
//   File : kvi_env.h
//   Creation date : Sat May 05 2002 02:15:21 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002-2008 Szymon Stefanek (pragma@kvirc.net)
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
* \file kvi_env.h
* \author Szymon Stefanek
* \brief Environment function wrappers
*
* \def kvi_setenv(__name,__value) Defines kvi_setenv(__name,__value) as SetEnvironmentVariable(__name,__value) on win32
* \def kvi_unsetenv(__name) Defines kvi_unsetenv(__name) as SetEnvironmentVariable(__name,NULL) on win32
*/

#include "kvi_settings.h"

#include <stdlib.h>

/**
* \brief Gets environment variable
* \param name The name of the variable to get
* \return char *
*/
inline char * kvi_getenv(const char * name)
{
	return getenv(name);
}

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	#define kvi_setenv(__name,__value) SetEnvironmentVariable(__name,__value)
	#define kvi_unsetenv(__name) SetEnvironmentVariable(__name,NULL)
#else
	#ifdef _KVI_ENV_CPP_
		/**
		* \brief Sets environment variable
		* \param name The name of the variable to set
		* \param value The value of the variable
		* \return bool
		*/
		KVILIB_API bool kvi_setenv(const char * name, const char * value);

		/**
		* \brief Unsets environment variable
		* \param name The name of the variable to set
		* \return void
		*/
		KVILIB_API void kvi_unsetenv(const char * name);
	#else
		/**
		* \brief Sets environment variable
		* \param name The name of the variable to set
		* \param value The value of the variable
		* \return bool
		*/
		KVILIB_API extern bool kvi_setenv(const char * name, const char * value);

		/**
		* \brief Unsets environment variable
		* \param name The name of the variable to set
		* \return void
		*/
		KVILIB_API extern void kvi_unsetenv(const char * name);
	#endif
#endif


#endif //_KVI_ENV_H_
