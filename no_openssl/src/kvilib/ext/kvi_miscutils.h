#ifndef _KVI_MISCUTILS_H_
#define _KVI_MISCUTILS_H_
//=============================================================================
//
//   File : kvi_miscutils.h
//   Creation date : Mon 08 Jan 2007 04:07:31 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2007-2008 Szymon Stefanek <pragma at kvirc dot net>
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

#include "kvi_settings.h"
#include "kvi_qstring.h"

// this is the namespace for single function stuff that doesn't really fit anywhere else

namespace KviMiscUtils
{
	// Compare two x.y.z... version strings.
	// The function behaves like strcmp: it returns 0 when the versions
	// are equal, -1 if the szVersion1 is greater and 1 if szVersion2 is greater
	extern KVILIB_API int compareVersions(const QString &szVersion1,const QString &szVersion2);

	// Check if the argument string is a valid x.y.z.... version string
	extern KVILIB_API bool isValidVersionString(const QString &szVersion);

}

#endif //!_KVI_MISCUTILS_H_
