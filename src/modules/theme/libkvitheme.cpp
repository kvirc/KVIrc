//=============================================================================
//
//   File : libkvitheme.cpp
//   Creation date : Sat 30 Dec 2006 14:54:56 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2006-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "ThemeManagementDialog.h"
#include "ThemeFunctions.h"

#include "KviKvsArrayCast.h"
#include "KviMessageBox.h"
#include "KviModule.h"
#include "KviLocale.h"
#include "KviCommandFormatter.h"
#include "KviError.h"
#include "kvi_out.h"
#include "KviMainWindow.h"
#include "KviIconManager.h"
#include "KviConfigurationFile.h"
#include "kvi_sourcesdate.h"
#include "KviFileUtils.h"
#include "KviFileDialog.h"
#include "KviTheme.h"

#include <QFileInfo>
#include <QMessageBox>

QRect g_rectManagementDialogGeometry(0, 0, 0, 0);

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
	KVSM_PARAMETER("package_path", KVS_PT_STRING, 0, szThemePackFile)
	KVSM_PARAMETERS_END(c)

	QString szError;
	if(!ThemeFunctions::installThemePackage(szThemePackFile, szError))
	{
		c->error(__tr2qs_ctx("Error installing theme package: %Q", "theme"), &szError);
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
		theme.apply [-b] [-e] [-u] <theme:string>
	@switches:
		!sw: -e | --external
		<theme> is an absolute directory containing a theme
		!sw: -b | --builtin
		<theme> is the name of a builtin theme (a subdirectory of the KVIrc global themes directory)
		!sw: -u | --user
		<theme> is the name of a user theme (a subdirectory of the KVIrc local themes directory)
	@description:
		Attempts to apply the global theme specified by <theme>.
		If the -b switch is present then <theme> is assumed to be a name of
		a builtin installed theme (a subdirectory of the KVIrc global themes directory).
		If the -l switch is present then <theme> is assumed to be a name of
		a user installed theme (a subdirectory of the KVIrc local themes directory).
		If the -e switch is present then <theme> is assumed to be an absolute
		directory containing the theme data.
		If no switch is present then KVIrc tries to determine automatically the
		type of theme (user, builtin or external).
*/

static bool theme_kvs_cmd_apply(KviKvsModuleCommandCall * c)
{
	QString szTheme;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("theme", KVS_PT_STRING, 0, szTheme)
	KVSM_PARAMETERS_END(c)

	KviThemeInfo out;
	KviThemeInfo::Location eLocation = KviThemeInfo::Auto;

	if(c->switches()->find('b', "builtin"))
		eLocation = KviThemeInfo::Builtin;
	else if(c->switches()->find('e', "external"))
		eLocation = KviThemeInfo::External;
	else if(c->switches()->find('u', "user"))
		eLocation = KviThemeInfo::User;

	if(!KviTheme::apply(szTheme, eLocation, out))
	{
		QString szErr = out.lastError();
		c->error(__tr2qs_ctx("Failed to apply theme: %Q", "theme"), &szErr);
		return false;
	}

	return true;
}

/*
	@doc: theme.info
	@type:
		function
	@title:
		$theme.info
	@short:
		Return info about a user defined theme.
	@description:
		Returns a hash with information about the global theme specified by <theme>.
*/

static bool theme_kvs_fnc_info(KviKvsModuleFunctionCall * c)
{
	QString szTheme;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("theme", KVS_PT_STRING, 0, szTheme)
	KVSM_PARAMETERS_END(c)

	KviKvsHash * pHash = new KviKvsHash();
	c->returnValue()->setHash(pHash);

	KviThemeInfo theme;
	if(!theme.load(szTheme, KviThemeInfo::Auto))
	{
		c->warning(__tr2qs_ctx("The theme package '%Q' doesn't exist", "theme"), &szTheme);
		return true;
	}

	pHash->set("name", new KviKvsVariant(theme.name()));
	pHash->set("version", new KviKvsVariant(theme.version()));
	pHash->set("author", new KviKvsVariant(theme.author()));
	pHash->set("description", new KviKvsVariant(theme.description()));
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
	KVSM_PARAMETER("file_name_path", KVS_PT_STRING, KVS_PF_OPTIONAL, szFileName)
	KVSM_PARAMETERS_END(c)

	KviFileUtils::adjustFilePath(szFileName);

	QString szTmp;
	c->enterBlockingSection();

	bool bResult = KviFileDialog::askForSaveFileName(
	    szTmp,
	    __tr2qs_ctx("Enter a Filename - KVIrc", "theme"), //dialog header title
	    szFileName,
	    "*.png",
	    false,
	    false,
	    true,
	    g_pMainWindow);

	if(!c->leaveBlockingSection())
		return false; // need to stop immediately

	if(!bResult)
		return true;

	szFileName = szTmp;

	if(szFileName.isEmpty())
		return true; // done

	KviFileUtils::adjustFilePath(szFileName);
	if(QFileInfo(szFileName).suffix() != "png")
		szFileName += ".png";

	QString szError;
	if(!ThemeFunctions::makeKVIrcScreenshot(szFileName))
	{
		c->error(__tr2qs_ctx("Error capturing and saving screenshot!", "theme"));
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

static bool theme_kvs_cmd_dialog(KviKvsModuleCommandCall * c)
{
	ThemeManagementDialog::display(c->hasSwitch('t', "toplevel"));
	return true;
}

/*
	@doc: theme.pack
	@type:
		command
	@title:
		theme.pack
	@short:
		Creates a kvt package containing a set of themes
	@syntax:
		theme.pack <package_path> <package_name> <package_version> <package_description> <package_author> <package_image> <themes>
	@description:
		Creates a *.kvt package containing a set of KVIrc themes.[br]
		<package_path> is the absolute path and file name of the package that should be saved.[br]
		<package_name> is the visible name of the package (something like "My Theme Set").[br]
		<package_version> is the version of the package in the form X.Y.Z.[br]
		<package_description> is a textual description of the package.
		<package_author> is the name of the person that is creating the package (NOT necessarily the themes contained within).
		<package_image> is the path of an image to be used as package representative image. If the package is going
		to contain a single theme you may specify the theme's screenshot here. Pass an empty string if you
		don't want an image to be stored in the package.
		<theme> is a either a single path to a directory containing a theme as it's exported by KVIrc,
		or an array of such paths.
*/

static bool theme_kvs_cmd_pack(KviKvsModuleCommandCall * c)
{
	QString szPath, szName, szVersion, szDescription, szAuthor, szImage;

	KviKvsArrayCast aCast;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("package_path", KVS_PT_NONEMPTYSTRING, 0, szPath)
	KVSM_PARAMETER("package_name", KVS_PT_NONEMPTYSTRING, 0, szName)
	KVSM_PARAMETER("package_version", KVS_PT_NONEMPTYSTRING, 0, szVersion)
	KVSM_PARAMETER("package_description", KVS_PT_STRING, 0, szDescription)
	KVSM_PARAMETER("package_author", KVS_PT_NONEMPTYSTRING, 0, szAuthor)
	KVSM_PARAMETER("package_image", KVS_PT_STRING, 0, szImage)
	KVSM_PARAMETER("theme", KVS_PT_ARRAYCAST, 0, aCast)
	KVSM_PARAMETERS_END(c)

	KviKvsArray * pArray = aCast.array();
	if((!pArray) || (pArray->size() < 1))
	{
		c->error(__tr2qs_ctx("No themes specified", "theme"));
		return false;
	}

	kvs_uint_t s = pArray->size();
	QStringList lThemeList;

	for(kvs_uint_t i = 0; i < s; i++)
	{
		KviKvsVariant * v = pArray->at(i);
		if(!v)
			continue; // ?
		QString szVal;
		v->asString(szVal);
		if(szVal.isEmpty())
			continue;
		lThemeList.append(szVal);
	}

	KviPointerList<KviThemeInfo> lThemeInfoList;
	lThemeInfoList.setAutoDelete(true);

	for(const auto & szTheme : lThemeList)
	{
		KviThemeInfo * pInfo = new KviThemeInfo();
		if(!pInfo->load(szTheme, KviThemeInfo::External))
		{
			QString szErr = pInfo->lastError();
			c->error(__tr2qs_ctx("Failed to load theme from directory %Q: %Q", "theme"), &szTheme, &szErr);
			delete pInfo;
			return false;
		}

		lThemeInfoList.append(pInfo);
	}

	if(lThemeInfoList.isEmpty())
	{
		c->error(__tr2qs_ctx("No themes specified: refusing to create an empty theme package", "theme"));
		return false;
	}

	QString szError;

	if(
	    ThemeFunctions::packageThemes(
	        szPath,
	        szName,
	        szVersion,
	        szDescription,
	        szAuthor,
	        szImage,
	        lThemeInfoList,
	        szError))
		return true;

	c->error(szError);
	return false;
}

static bool theme_module_init(KviModule * m)
{
	KVSM_REGISTER_SIMPLE_COMMAND(m, "dialog", theme_kvs_cmd_dialog);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "install", theme_kvs_cmd_install);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "apply", theme_kvs_cmd_apply);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "screenshot", theme_kvs_cmd_screenshot);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "pack", theme_kvs_cmd_pack);

	KVSM_REGISTER_FUNCTION(m, "info", theme_kvs_fnc_info);

	QString szBuf;
	m->getDefaultConfigFileName(szBuf);
	KviConfigurationFile cfg(szBuf, KviConfigurationFile::Read);
	g_rectManagementDialogGeometry = cfg.readRectEntry("EditorGeometry", QRect(10, 10, 390, 440));

	return true;
}

static bool theme_module_cleanup(KviModule * m)
{
	ThemeManagementDialog::cleanup();

	QString szBuf;
	m->getDefaultConfigFileName(szBuf);
	KviConfigurationFile cfg(szBuf, KviConfigurationFile::Write);
	cfg.writeEntry("EditorGeometry", g_rectManagementDialogGeometry);

	return true;
}

static bool theme_module_can_unload(KviModule *)
{
	return (!ThemeManagementDialog::instance());
}

KVIRC_MODULE(
    "Theme",                                                        // module name
    "4.0.0",                                                        // module version
    "Copyright (C) 2006 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
    "Theme management functions",
    theme_module_init,
    theme_module_can_unload,
    0,
    theme_module_cleanup,
    "theme")
