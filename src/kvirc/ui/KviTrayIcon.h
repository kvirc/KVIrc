#ifndef _KviTrayIcon_h_
#define _KviTrayIcon_h_
//=============================================================================
//
//   File : KviTrayIcon.h
//   Creation date : Sun Jun 18 2000 17:59:02 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2011 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"

///
/// \class KviTrayIcon
/// \brief The KviTrayIcon class
///
/// Base class for the dock extension applets..
///
class KVIRC_API KviTrayIcon
{
public:
	///
	/// Destroys the instance of KviTrayIcon
	/// and frees all the relevant resources
	///
	virtual ~KviTrayIcon()
	{
	}

	virtual void refresh()
	{
	}

}; // class KviTrayIcon

#endif //!_KviTrayIcon_h_
