#ifndef _WebAddonInterfaceDialog_h_
#define _WebAddonInterfaceDialog_h_
//=============================================================================
//
//   File : WebAddonInterfaceDialog.h
//   Creation date : Fri 25 Mar 2011 01:01:12 by Szymon Tomasz Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2011 Szymon Tomasz Stefanek <pragma at kvirc dot net>
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

#ifdef COMPILE_WEBKIT_SUPPORT

#include "KviWebPackageManagementDialog.h"

///
/// \class WebAddonInterfaceDialog
/// \brief The WebAddonInterfaceDialog class
///
/// This class...
///
class WebAddonInterfaceDialog : public KviWebPackageManagementDialog
{
	Q_OBJECT

public:
	///
	/// Creates an instance of WebAddonInterfaceDialog
	///
	WebAddonInterfaceDialog(QWidget * pParent = nullptr);

	///
	/// Destroys the instance of WebAddonInterfaceDialog
	/// and frees all the relevant resources
	///
	virtual ~WebAddonInterfaceDialog();

protected:
	virtual bool packageIsInstalled(const QString & szId, const QString & szVersion);
	virtual bool installPackage(const QString & szPath, QString & szError);

}; // class WebAddonInterfaceDialog

#endif //COMPILE_WEBKIT_SUPPORT

#endif //!_WebAddonInterfaceDialog_h_
