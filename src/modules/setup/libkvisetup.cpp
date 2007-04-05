//=============================================================================
//
//   File : libkvisetup.cpp
//   Creation date : Sat Oct  6 02:08:24 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2004 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#include "kvi_module.h"

#include "setupwizard.h"

#include <qstring.h>
#include <qfile.h>

#include "kvi_options.h"
#include "kvi_defaults.h"
#include "kvi_app.h"
#include "kvi_kvs_script.h"
#include "kvi_kvs_variantlist.h"
#include "kvi_window.h"
#include "kvi_theme.h"
#include "kvi_ircserverdb.h"

// this will be choosen during the setup process
QString g_szChoosenIncomingDirectory;
//int g_iThemeToApply = THEME_APPLY_NONE;
bool bNeedToApplyDefaults;
unsigned int uPort;
QString szHost;
QString szUrl;
QString szMircServers;
QString szMircIni;




// this is called by KviApp when no local kvirc directory has been found
// we have nothing to rely on yet... (settings have not been loaded yet)
KVIMODULEEXPORTFUNC bool setup_begin()
{
	// FIXME: try to guess the user's .kvirc directory
	//        and the preferred nickname from a previous installation

	KviSetupWizard * w = new KviSetupWizard();
	int retcode = w->exec();
	delete w;
	return (retcode == QDialog::Accepted);
}


// this is called just after the first startup
// all the subsystems are up and running (we even have a nice console
// to write on if needed).
// all the options are loaded.
KVIMODULEEXPORTFUNC void setup_finish()
{
	if(bNeedToApplyDefaults)
	{
		KVI_OPTION_STRING(KviOption_stringIncomingPath) = g_szChoosenIncomingDirectory;
		// Reset the quit message and the real name... if they contain the KVIrc version
		// then probably the user hasn't even edited them.
		if(KVI_OPTION_STRING(KviOption_stringQuitMessage).find("KVIrc",0,false) != -1)
			KVI_OPTION_STRING(KviOption_stringQuitMessage) = KVI_DEFAULT_QUIT_MESSAGE;
		// We deserve to change the user's part message in something nice :)
		KVI_OPTION_STRING(KviOption_stringPartMessage) = KVI_DEFAULT_PART_MESSAGE;
	
		// FIXME: Should we also change these ?
		// #define KVI_DEFAULT_CTCP_USERINFO_REPLY "I'm too lazy to edit this field."
		// #define KVI_DEFAULT_AWAY_MESSAGE "I'm off to see the wizard."
	
		// Apply the choosen default theme
		QString szDir;
/*		switch(g_iThemeToApply)
		{
			case THEME_APPLY_HIRES:
				g_pApp->getGlobalKvircDirectory(szDir,KviApp::Themes,"default.hires");
				g_pApp->loadTheme(szDir);
			break;
			case THEME_APPLY_LORES:
				g_pApp->getGlobalKvircDirectory(szDir,KviApp::Themes,"default.lores");
				g_pApp->loadTheme(szDir);
			break;
			// default: no theme
		}*/
		
		g_pApp->getGlobalKvircDirectory(szDir,KviApp::Themes,"silverirc");
		KviThemeInfo out;
		KviTheme::load(szDir,out);
		
		if(!szMircServers.isEmpty())
		{
			g_pIrcServerDataBase->loadFromMircIni(szMircServers,szMircIni,
				KVI_OPTION_STRINGLIST(KviOption_stringlistRecentIrcUrls));
			g_pApp->emitRecentUrlsChanged();
			g_pApp->saveIrcServerDataBase();
		}

		g_pApp->loadDefaultScript();
		if(!szUrl.isEmpty())
		{
			KviKvsVariantList * pParams = new KviKvsVariantList();
			pParams->append(szUrl);
			KviKvsScript::run("openurl $0",g_pActiveWindow,pParams);
			delete pParams;
			KVI_OPTION_BOOL(KviOption_boolShowServersConnectDialogOnStart) = TRUE;
		} else if(!szHost.isEmpty()) {
			KviKvsVariantList * pParams = new KviKvsVariantList();
			pParams->append(szHost);
			pParams->append((kvs_int_t)uPort);
			KviKvsScript::run("server $0 $1",g_pActiveWindow,pParams);
			delete pParams;
			KVI_OPTION_BOOL(KviOption_boolShowServersConnectDialogOnStart) = TRUE;
		}
	}
}

static bool setup_module_init(KviModule *m)
{
	bNeedToApplyDefaults=true;
	return true;
}

static bool setup_module_cleanup(KviModule *m)
{
	return true;
}

static bool setup_module_can_unload(KviModule *m)
{
	return true; // can be always unloaded...this module does basically nothing
}

KVIRC_MODULE(
    "Setup wizard",
	"1.0.0",
	"Szymon Stefanek <pragma at kvirc dot net>" ,
	"First-time-run setup wizard",
    setup_module_init ,
    setup_module_can_unload,
    0,
	setup_module_cleanup
)
