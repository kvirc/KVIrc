//=============================================================================
//
//   File : libkvitheme.cpp
//   Creation date : Sat 30 Dec 2006 14:54:56 by Szymon Stefanek
//
//   This toolbar is part of the KVIrc irc client distribution
//   Copyright (C) 2006-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "ThemeManagementDialog.h"
#include "ThemeFunctions.h"

#include "KviMessageBox.h"
#include "KviModule.h"
#include "KviLocale.h"
#include "KviParameterList.h"
#include "KviCommandFormatter.h"
#include "KviError.h"
#include "kvi_out.h"
#include "KviIconManager.h"
#include "KviConfigurationFile.h"
#include "kvi_sourcesdate.h"
#include "KviFileUtils.h"
#include "KviFileDialog.h"
#include "KviTheme.h"

#include <QFileInfo>
#include <QMessageBox>

QRect g_rectManagementDialogGeometry(0,0,0,0);


/*
	@doc: theme.install
	@type:
		command
	@title:
		theme.install
	@short:
		Shows the theme management editor
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
	if(!ThemeFunctions::installThemePackage(szThemePackFile,szError))
	{
		c->error(__tr2qs_ctx("Error installing theme package: %Q","theme"),&szError);
		return false;
	}

	return true;
}
/*
	@doc: theme.apply
	@type:
		command
	@title:
		theme.apply
	@short:
		Apply a theme.
	@syntax:
		theme.apply <package_name:string>
	@description:
		Attempts to apply the theme specified by <package_name>.
*/
static bool theme_kvs_cmd_apply(KviKvsModuleCommandCall * c)
{
	QString szThemePackFile;

	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("package_name",KVS_PT_STRING,0,szThemePackFile)
	KVSM_PARAMETERS_END(c)
	QString szDir;
	g_pApp->getLocalKvircDirectory(szDir,KviApplication::Themes);
	szDir += KVI_PATH_SEPARATOR_CHAR;
	szDir += szThemePackFile;
	KviThemeInfo * themeInfo = new KviThemeInfo();
	if(themeInfo->loadFromDirectory(szDir))
	{
		themeInfo->setSubdirectory(szThemePackFile);
		if(KviMessageBox::yesNo(__tr2qs_ctx("Apply theme - KVIrc","theme"),
		__tr2qs_ctx("Do you wish to apply theme \"%Q\" (version %Q)?","theme"),
		&(themeInfo->name()),&(themeInfo->version())))
		{
			QString szPath = themeInfo->absoluteDirectory();
			if(szPath.isEmpty())return true;

			KviThemeInfo out;

			if(!KviTheme::load(szPath,out))
			{
				QString szErr = out.lastError();
				QString szMsg = QString(__tr2qs_ctx("Failed to apply the specified theme: %1","theme")).arg(szErr);
				QMessageBox::critical(0,__tr2qs_ctx("Apply theme - KVIrc","theme"),szMsg,
				QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
			}
		}
	}
	c->warning(__tr2qs_ctx("The theme package '%Q' does not exist","theme"),&szThemePackFile);
	return true;
}
/*
	@doc: theme.info
	@type:
		function
	@title:
		theme.info
	@short:
		Return info about a theme.
	@syntax:
		<themes_list:string> $theme.info(<theme_name:string>)
	@description:
		Returns as hash the info about a theme.
		Hash keys are: name, version, author, description.
*/
static bool theme_kvs_fnc_info(KviKvsModuleFunctionCall * c)
{
	QString szThemePackFile;

	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("package_name",KVS_PT_STRING,0,szThemePackFile)
	KVSM_PARAMETERS_END(c)
	QString szDir;
	g_pApp->getLocalKvircDirectory(szDir,KviApplication::Themes);
	szDir += KVI_PATH_SEPARATOR_CHAR;
	szDir += szThemePackFile;
	KviThemeInfo * themeInfo = new KviThemeInfo();
	if(themeInfo->loadFromDirectory(szDir))
	{
		KviKvsHash *pHash=new KviKvsHash();
		KviKvsVariant *name=new KviKvsVariant(themeInfo->name());
		pHash->set("name",name);
		KviKvsVariant *version=new KviKvsVariant(themeInfo->version());
		pHash->set("version",version);
		KviKvsVariant *author=new KviKvsVariant(themeInfo->author());
		pHash->set("author",author);
		KviKvsVariant *description=new KviKvsVariant(themeInfo->description());
		pHash->set("description",description);
		c->returnValue()->setHash(pHash);
		return true;
	}
	c->warning(__tr2qs_ctx("The theme package '%Q' does not exist","theme"),&szThemePackFile);
	return true;
}

/*
	@doc: theme.list
	@type:
		function
	@title:
		theme.list
	@short:
		Return a list of the installed themes.
	@syntax:
		<themes_list:string> $theme.list
	@description:
		Returns a comma separated string list of the KVIrc installed themes.
*/
static bool theme_kvs_fnc_list(KviKvsModuleFunctionCall * c)
{
	QString szDir;
	g_pApp->getLocalKvircDirectory(szDir,KviApplication::Themes);
	QDir d(szDir);
	QStringList sl = d.entryList(QDir::Dirs);
	QStringList szThemes;
	for(QStringList::Iterator it = sl.begin();it != sl.end();++it)
	{
		if(*it == ".")continue;
		if(*it == "..")continue;
		szThemes.append(*it);
	}
	c->returnValue()->setString(szThemes.join(","));
	return true;
}

/*
	@doc: theme.screenshot
	@type:
		command
	@title:
		theme.screenshot
	@short:
		Makes a screenshot of the KVIrc window
	@syntax:
		theme.screenshot [file_name_path:string]
	@description:
		Makes a screenshot of the KVIrc main window
		and saves it in the specified file. If [file_name_path]
		is not specified then a save file dialog is shown.
*/

static bool theme_kvs_cmd_screenshot(KviKvsModuleCommandCall * c)
{
	QString szFileName;

	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("file_name_path",KVS_PT_STRING,KVS_PF_OPTIONAL,szFileName)
	KVSM_PARAMETERS_END(c)


	KviFileUtils::adjustFilePath(szFileName);

	QString szTmp;
	c->enterBlockingSection();

	bool bResult = KviFileDialog::askForSaveFileName(szTmp,__tr2qs_ctx("Choose a file to save the screenshot to","theme"),szFileName,"*.png");

	if(!c->leaveBlockingSection())return false; // need to stop immediately
	if(!bResult)return true;

	szFileName = szTmp;

	if(szFileName.isEmpty())return true; // done
	KviFileUtils::adjustFilePath(szFileName);
	if(QFileInfo(szFileName).suffix()!="png")
		szFileName+=".png";

	QString szError;
	if(!ThemeFunctions::makeKVIrcScreenshot(szFileName))
	{
		c->error(__tr2qs_ctx("Error making screenshot","theme")); // FIXME: a nicer error ?
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
		theme.dialog [-t]
	@description:
		Shows the theme theme management editor[br]
		If the [-t] switch is used, the dialog is opened as toplevel window,
		otherwise it is opened as part of the current frame window.[br]
*/

static bool theme_kvs_cmd_dialog(KviKvsModuleCommandCall *c)
{

	ThemeManagementDialog::display(c->hasSwitch('t',"toplevel"));
	return true;
}

static bool theme_module_init(KviModule *m)
{
	KVSM_REGISTER_SIMPLE_COMMAND(m,"dialog",theme_kvs_cmd_dialog);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"install",theme_kvs_cmd_install);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"apply",theme_kvs_cmd_apply);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"screenshot",theme_kvs_cmd_screenshot);

	KVSM_REGISTER_FUNCTION(m,"list",theme_kvs_fnc_list);
	KVSM_REGISTER_FUNCTION(m,"info",theme_kvs_fnc_info);

	QString szBuf;
	m->getDefaultConfigFileName(szBuf);
	KviConfigurationFile cfg(szBuf,KviConfigurationFile::Read);
	g_rectManagementDialogGeometry = cfg.readRectEntry("EditorGeometry",QRect(10,10,390,440));

	return true;
}

static bool theme_module_cleanup(KviModule *m)
{
	ThemeManagementDialog::cleanup();

	QString szBuf;
	m->getDefaultConfigFileName(szBuf);
	KviConfigurationFile cfg(szBuf,KviConfigurationFile::Write);
	cfg.writeEntry("EditorGeometry",g_rectManagementDialogGeometry);

	return true;
}

static bool theme_module_can_unload(KviModule *)
{
	return (!ThemeManagementDialog::instance());
}


KVIRC_MODULE(
	"Theme",                                                      // module name
	"4.0.0",                                                        // module version
	"Copyright (C) 2006 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
	"Theme management functions",
	theme_module_init,
	theme_module_can_unload,
	0,
	theme_module_cleanup,
	"theme"
)
