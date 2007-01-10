///////////////////////////////////////////////////////////////////////////////
//
//   File : libkvitheme.cpp
//   Created on Sat 30 Dec 2006 14:54:56 by Szymon Stefanek
//
//   This toolbar is part of the KVirc irc client distribution
//   Copyright (C) 2006 Szymon Stefanek (pragma at kvirc dot net)
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
///////////////////////////////////////////////////////////////////////////////

#include "kvi_module.h"
#include "kvi_locale.h"
#include "kvi_qstring.h"
#include "kvi_parameterlist.h"
#include "kvi_cmdformatter.h"
#include "kvi_qstring.h"
#include "kvi_error.h"
#include "kvi_out.h"
#include "kvi_iconmanager.h"
#include "kvi_mirccntrl.h"
#include "kvi_config.h"
#include "kvi_sourcesdate.h"

#include "managementdialog.h"
#include "themefunctions.h"

QRect g_rectManagementDialogGeometry(0,0,0,0);


/*
	@doc: theme.install
	@type:
		command
	@title:
		theme.install
	@short:
		Shows the theme theme management editor
	@syntax:
		theme.install <package_path:string>
	@description:
		Attempts to install the themes in the package specified by <package_path>.
*/

static bool theme_kvs_cmd_install(KviKvsModuleCommandCall * c)
{
	QString szThemePackFile; 
	
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("package_path",KVS_PT_STRING,0,szThemePackFile)
	KVSM_PARAMETERS_END(c)

	QString szError;
	if(!KviThemeFunctions::installThemePackage(szThemePackFile,szError))
	{
		c->error(__tr2qs_ctx("Error installing theme package: %Q","theme"),&szError);
		return false;
	}

	return true;
}

/*
	@doc: theme.dialog
	@type:
		command
	@title:
		theme.dialog
	@short:
		Shows the theme theme management editor
	@syntax:
		theme.dialog
	@description:
		Shows the theme theme management editor
*/

static bool theme_kvs_cmd_dialog(KviKvsModuleCommandCall * c)
{
	KviThemeManagementDialog::display();
	return true;
}

static bool theme_module_init(KviModule *m)
{
	KVSM_REGISTER_SIMPLE_COMMAND(m,"dialog",theme_kvs_cmd_dialog);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"install",theme_kvs_cmd_install);

	QString szBuf;
	m->getDefaultConfigFileName(szBuf);
	KviConfig cfg(szBuf,KviConfig::Read);
	g_rectManagementDialogGeometry = cfg.readRectEntry("EditorGeometry",QRect(10,10,390,440));

	return true;
}

static bool theme_module_cleanup(KviModule *m)
{
	KviThemeManagementDialog::cleanup();

	QString szBuf;
	m->getDefaultConfigFileName(szBuf);
	KviConfig cfg(szBuf,KviConfig::Write);
	cfg.writeEntry("EditorGeometry",g_rectManagementDialogGeometry);

	return true;
}

static bool theme_module_can_unload(KviModule * m)
{
	return (!KviThemeManagementDialog::instance());
}


KVIRC_MODULE(
	"theme",                                                      // module name
	"1.0.0",                                                        // module version
	"Copyright (C) 2006 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
	"Theme management functions",
	theme_module_init,
	theme_module_can_unload,
	0,
	theme_module_cleanup
)
