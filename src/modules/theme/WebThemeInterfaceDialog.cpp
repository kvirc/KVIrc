//=============================================================================
//
//   File : WebThemeInterfaceDialog.cpp
//   Creation date : Mon Mar 14 2011 15:10:20 by Alessandro Carbone
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2011 Alessandro Carbone(elfonol at gmail dot com)
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


#ifdef COMPILE_WEBKIT_SUPPORT

#include "WebThemeInterfaceDialog.h"
#include "ThemeFunctions.h"

#include "KviApplication.h"
#include "KviFileUtils.h"
#include "KviIconManager.h"
#include "KviLocale.h"


WebThemeInterfaceDialog::WebThemeInterfaceDialog(QWidget * par)
: KviWebPackageManagementDialog(par)
{
	setWindowTitle(__tr2qs_ctx("KVIrc : Download Themes","theme"));

	// local dir for user defined themes
	g_pApp->getLocalKvircDirectory(m_szLocalThemesPath,KviApplication::Themes);
	m_szLocalThemesPath += KVI_PATH_SEPARATOR_CHAR;

	// global dir for builtin themes
	g_pApp->getGlobalKvircDirectory(m_szGlobalThemesPath,KviApplication::Themes);
	m_szGlobalThemesPath += KVI_PATH_SEPARATOR_CHAR;

	setPackagePageUrl(
			QString::fromAscii("http://www.kvirc.de/app/themes.php?version=" KVI_VERSION "&lang=%1")
					.arg(QString::fromUtf8(KviLocale::localeName().ptr()))
		);
}
WebThemeInterfaceDialog::~WebThemeInterfaceDialog()
{
}

bool WebThemeInterfaceDialog::installPackage(const QString &szPath,QString &szError)
{
	return ThemeFunctions::installThemePackage(szPath,szError,this);
}

bool WebThemeInterfaceDialog::packageIsInstalled(const QString &szName,const QString &szVersion)
{
	return \
			KviFileUtils::fileExists(m_szGlobalThemesPath+szName+"-"+szVersion) || \
			KviFileUtils::fileExists(m_szLocalThemesPath+szName+"-"+szVersion);
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
	#include "WebThemeInterfaceDialog.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES

#endif //COMPILE_WEBKIT_SUPPORT

