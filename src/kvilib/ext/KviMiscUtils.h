#ifndef _KVI_MISCUTILS_H_
#define _KVI_MISCUTILS_H_
//=============================================================================
//
//   File : KviMiscUtils.h
//   Creation date : Mon 08 Jan 2007 04:07:31 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2007-2010 Szymon Stefanek <pragma at kvirc dot net>
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
* \file KviMiscUtils.h
* \author Szymon Stefanek
* \brief Helper functions
*/

#include "kvi_settings.h"

class QString;

/**
* \namespace KviMiscUtils
* \brief A namespace for helper functions
*
* This namespace contains single function stuff that doesn't really fit anywhere else
*/
namespace KviMiscUtils
{
	/**
	* \brief Compares two x.y.z... version strings
	* The function behaves like strcmp: it returns 0 when the versions are equal, -1 if
	* the szVersion1 is greater and 1 if szVersion2 is greater
	* \param szVersion1 The first version to check
	* \param szVersion2 The second version to check
	* \return int
	*/
	extern KVILIB_API int compareVersions(const QString & szVersion1, const QString & szVersion2);

	/**
	* \brief Checks if the argument string is a valid x.y.z... version string
	* \param szVersion The version to check
	* \return bool
	*/
	extern KVILIB_API bool isValidVersionString(const QString & szVersion);
}

#endif // _KVI_MISCUTILS_H_
