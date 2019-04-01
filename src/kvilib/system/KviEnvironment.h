#ifndef _KVI_ENV_H_
#define _KVI_ENV_H_
//=============================================================================
//
//   File : KviEnvironment.h
//   Creation date : Sat May 05 2002 02:15:21 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma@kvirc.net)
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
* \file KviEnvironment.h
* \author Szymon Stefanek
* \brief Environment function wrappers
*/

#include "kvi_settings.h"

#include <QString>

#include <stdlib.h>

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
#include <windows.h>
#define MAX_ENV_SIZE 32767
#endif

namespace KviEnvironment
{

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	inline QString getVariable(const QString & szName)
	{
		LPTSTR szRet = (LPTSTR)::malloc(MAX_ENV_SIZE * sizeof(TCHAR));
		QString szValue;
		if (GetEnvironmentVariable(szName.toStdWString().c_str(), szRet, MAX_ENV_SIZE))
			szValue = QString::fromStdWString(szRet);
		::free(szRet);
		return szValue;
	}

	inline void setVariable(const QString & szName, const QString & szValue)
	{
		SetEnvironmentVariable(szName.toStdWString().c_str(),
			szValue.toStdWString().c_str());
	}

	inline void unsetVariable(const QString & szName)
	{
		SetEnvironmentVariable(szName.toStdWString().c_str(), nullptr);
	}
#else
	/**
	* \brief Gets environment variable
	* \param name The name of the variable to get
	* \return char *
	*/
	inline QString getVariable(const QString & szName)
	{
		auto name = szName.toLocal8Bit();
		return QString::fromLocal8Bit(getenv(name.data()));
	}

	/**
	* \brief Sets environment variable
	* \param name The name of the variable to set
	* \param value The value of the variable
	* \return bool
	*/
	KVILIB_API bool setVariable(const QString & szName, const QString & szValue);

	/**
	* \brief Unsets environment variable
	* \param name The name of the variable to set
	* \return void
	*/
	KVILIB_API void unsetVariable(const QString & szName);
#endif
}

#endif //_KVI_ENV_H_
