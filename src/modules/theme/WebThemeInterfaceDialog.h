#ifndef _WEBTHEMEINTERFACEDIALOG_H_
#define _WEBTHEMEINTERFACEDIALOG_H_
//=============================================================================
//
//   File : WebThemeInterfaceDialog.h
//   Creation date : Mon Mar 14 2011 15:10:20 by Alessandro Carbone
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2011 Alessandro Carbone(elfonol at gmail dot com)
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

class WebThemeInterfaceDialog : public KviWebPackageManagementDialog
{
	Q_OBJECT
public:
	WebThemeInterfaceDialog(QWidget * par = nullptr);
	~WebThemeInterfaceDialog();

private:
	QString m_szLocalThemesPath;
	QString m_szGlobalThemesPath;

protected:
	bool packageIsInstalled(const QString & szId, const QString & szVersion) override;
	bool installPackage(const QString & szPath, QString & szError) override;
};

#endif //COMPILE_WEBKIT_SUPPORT

#endif //_WEBTHEMEINTERFACEDIALOG_H_
