#ifndef _KviDockExtension_h_
#define _KviDockExtension_h_
//=============================================================================
//
//   File : KviDockExtension.h
//   Creation date : Sun Jun 18 2000 17:59:02 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2000-2011 Szymon Stefanek (pragma at kvirc dot net)
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

#include <QWidget>

///
/// \class KviDockExtension
/// \brief The KviDockExtension class
///
/// Base class for the dock extension applets..
///
class KVIRC_API KviDockExtension
{
private:
	Qt::WindowStates m_oStoredWindowStates;

public:

	///
	/// Creates an instance of KviDockExtension
	///
	KviDockExtension()
		: m_oStoredWindowStates(0)
	{
	}

	///
	/// Destroys the instance of KviDockExtension
	/// and frees all the relevant resources
	///
	virtual ~KviDockExtension()
	{
	}


public:
	void setPrevWindowState(Qt::WindowStates state)
	{
		m_oStoredWindowStates = state;
	}

	Qt::WindowStates getPrevWindowState()
	{
		return m_oStoredWindowStates;
	}

	virtual void refresh()
	{
	}

	virtual void die()
	{
		delete this;
	}

}; // class KviDockExtension



#endif //!_KviDockExtension_h_
