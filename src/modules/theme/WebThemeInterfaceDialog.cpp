//=============================================================================
//
//   File : WebThemeInterfaceDialog.cpp
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

#include "WebThemeInterfaceDialog.h"
#include "ThemeFunctions.h"

#include "KviApplication.h"
#include "KviFileUtils.h"
#include "KviIconManager.h"
#include "KviLocale.h"

WebThemeInterfaceDialog::WebThemeInterfaceDialog(QWidget * par)
    : KviWebPackageManagementDialog(par)
{
	setWindowTitle(__tr2qs_ctx("Download Themes - KVIrc", "theme"));

	// local dir for user defined themes
	g_pApp->getLocalKvircDirectory(m_szLocalThemesPath, KviApplication::Themes);
	m_szLocalThemesPath += KVI_PATH_SEPARATOR_CHAR;

	// global dir for builtin themes
	g_pApp->getGlobalKvircDirectory(m_szGlobalThemesPath, KviApplication::Themes);
	m_szGlobalThemesPath += KVI_PATH_SEPARATOR_CHAR;

	setPackagePageUrl(
	    QString::fromLatin1("http://www.kvirc.de/app/themes.php?version=" KVI_VERSION "&lang=%1")
	        .arg(KviLocale::instance()->localeName()));
}
WebThemeInterfaceDialog::~WebThemeInterfaceDialog()
    = default;

bool WebThemeInterfaceDialog::installPackage(const QString & szPath, QString & szError)
{
	return ThemeFunctions::installThemePackage(szPath, szError, this);
}

bool WebThemeInterfaceDialog::packageIsInstalled(const QString & szId, const QString & szVersion)
{
	QString szSubdir = szId + QString("-") + szVersion;
	szSubdir.replace(QRegExp("[^a-zA-Z0-9_\\-.][^a-zA-Z0-9_\\-.]*"), "_");

	return KviFileUtils::fileExists(m_szGlobalThemesPath + szSubdir) || KviFileUtils::fileExists(m_szLocalThemesPath + szSubdir);
}

#endif //COMPILE_WEBKIT_SUPPORT
