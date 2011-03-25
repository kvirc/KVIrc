//=============================================================================
//
//   File : WebAddonInterfaceDialog.cpp
//   Creation date : Fri 25 Mar 2011 01:01:12 by Szymon Tomasz Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2011 Szymon Tomasz Stefanek <pragma at kvirc dot net>
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

#include "WebAddonInterfaceDialog.h"
#include "AddonFunctions.h"

#include "KviApplication.h"
#include "KviFileUtils.h"
#include "KviIconManager.h"
#include "KviLocale.h"
#include "KviKvsScriptAddonManager.h"
#include "KviMiscUtils.h"

WebAddonInterfaceDialog::WebAddonInterfaceDialog(QWidget * par)
: KviWebPackageManagementDialog(par)
{
	setWindowTitle(__tr2qs_ctx("KVIrc : Download Addons","theme"));

	setPackagePageUrl(
			QString::fromAscii("http://www.kvirc.de/app/addons.php?version=" KVI_VERSION "&lang=%1")
					.arg(QString::fromUtf8(KviLocale::instance()->localeName().ptr()))
		);
}
WebAddonInterfaceDialog::~WebAddonInterfaceDialog()
{
}

bool WebAddonInterfaceDialog::installPackage(const QString &szPath,QString &szError)
{
	return AddonFunctions::installAddonPackage(szPath,szError,this);
}

bool WebAddonInterfaceDialog::packageIsInstalled(const QString &szId,const QString &szVersion)
{
	KviKvsScriptAddon * pAddon = KviKvsScriptAddonManager::instance()->findAddon(szId);
	if(!pAddon)
		return false;

	if(szVersion.isEmpty())
		return true; // hum.... shouldn't happen ?

	// FIXME: If the version of the installed addon is lower then allow upgrading!
	return KviMiscUtils::compareVersions(pAddon->version(),szVersion) < 0;
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
	#include "WebAddonInterfaceDialog.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES

#endif //COMPILE_WEBKIT_SUPPORT

