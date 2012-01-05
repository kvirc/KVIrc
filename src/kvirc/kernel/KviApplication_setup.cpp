//=============================================================================
//
//   File : KviApplication_setup.cpp
//   Creation date : Fri Apr 2 1999 02:38:05 by Szymon Stefanek
//
//   This file is part of the Kvirc irc client distribution
//   Copyright (C) 1999-2010 Szymon Stefanek (pragma at kvirc dot net)
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



//#define _KVI_DEBUG_CHECK_RANGE_
//#include "kvi_debug.h"

#include "kvi_settings.h"
#include "kvi_defaults.h"
#include "KviWindow.h"
#include "KviMainWindow.h"
#include "KviApplication.h"
#include "KviFileUtils.h"
#include "KviLocale.h"
#include "KviMessageBox.h"
#include "kvi_sourcesdate.h"
#include "KviIconManager.h"
#include "KviConfigurationFile.h"

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	#include <shlwapi.h>
	#include <windows.h> // at least for GetModuleFileName and *PrivateProfileString
#else
	#include <stdlib.h> // for getenv()
	#include <unistd.h> // for symlink() <-- unused ?

	#ifdef COMPILE_KDE_SUPPORT
		#include <kconfig.h>
		#include <KConfigGroup>
//		#include <kstddirs.h>
	#endif
#endif

#include <QTextCodec>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QLibrary>

static QLibrary * g_pSetupLibrary = 0;

//
// Things launched at startup:
// - Attempt to find the global Kvirc directory
// - Attempt to find the local Kvirc directory
//   and if it is not found, ask the user to choose one
//

bool KviApplication::checkGlobalKvircDirectory(const QString szDir)
{
	//First check if the help subdir exists
	QString szHelpDir = szDir;
	szHelpDir += KVI_PATH_SEPARATOR"help";
	if(!KviFileUtils::directoryExists(szHelpDir))
		return false;
	//Then check if the pics subdir exists
	QString szPicsDir = szDir;
	szPicsDir += KVI_PATH_SEPARATOR"pics";
	if(!KviFileUtils::directoryExists(szPicsDir))
		return false;
	//Now make sure that it is the dir that we're looking for.
	//Check for an image file that we need.
	szPicsDir.append(KVI_PATH_SEPARATOR);
	szPicsDir.append(KVI_ACTIVITYMETER_IMAGE_NAME);
	return KviFileUtils::isReadable(szPicsDir);
}

bool KviApplication::checkLocalKvircDirectory(const QString &szSpecificPath)
{
	QString szSavedLocalKvircDir = m_szLocalKvircDir;
	m_szLocalKvircDir = szSpecificPath;
	bool bRet = checkLocalKvircDirectory();
	m_szLocalKvircDir = szSavedLocalKvircDir;
	return bRet;
}

bool KviApplication::checkLocalKvircDirectory()
{
	//First check if the dir exists
	if(!KviFileUtils::directoryExists(m_szLocalKvircDir))
		return false;
	if(!QFileInfo(m_szLocalKvircDir).isWritable())
		return false;

	QString szBuff;
	getLocalKvircDirectory(szBuff,Config);
	if(!KviFileUtils::directoryExists(szBuff))
		return false;

	return true;
}

bool KviApplication::checkFileAssociations()
{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
#define QUERY_BUFFER 2048
	char * pcBuffer;
	DWORD len = QUERY_BUFFER;
	DWORD err;
	pcBuffer = (char*)malloc(len*sizeof(char));
	HKEY hKey;

	if(RegOpenKeyEx(HKEY_CLASSES_ROOT,".kvs",0,KEY_READ,&hKey) != ERROR_SUCCESS)
	{
		free(pcBuffer);
		return false;
	}

	if((err=RegQueryValueEx(hKey,0,0,0,(LPBYTE)pcBuffer,&len)) != ERROR_SUCCESS)
	{
		free(pcBuffer);
		return false;
	} else {
		if(!kvi_strEqualCIN("KVIrcScript",pcBuffer,11))
		{
			free(pcBuffer);
			return false;
		}
	}

	len = QUERY_BUFFER;
	if(RegOpenKeyEx(HKEY_CLASSES_ROOT,"KVIrcScript",0,KEY_READ,&hKey) != ERROR_SUCCESS)
	{
		free(pcBuffer);
		return false;
	}

	if((err=RegQueryValueEx( hKey,0,0,0,(LPBYTE)pcBuffer,&len)) != ERROR_SUCCESS)
	{
		free(pcBuffer);
		return false;
	} else {
		if(!kvi_strEqualCI(__tr2qs("KVIrc KVS Script").toLocal8Bit().data(),pcBuffer))
		{
			free(pcBuffer);
			return false;
		}
	}

	len = QUERY_BUFFER;
	if(RegOpenKeyEx(HKEY_CLASSES_ROOT,"KVIrcScript\\DefaultIcon",0,KEY_READ,&hKey) != ERROR_SUCCESS)
	{
		free(pcBuffer);
		return false;
	}

	if(RegQueryValueEx(hKey,0,0,0,(LPBYTE)pcBuffer,&len) != ERROR_SUCCESS)
	{
		free(pcBuffer);
		return false;
	} else {
		QString szIcon = applicationFilePath()+",1";
		szIcon.replace('/',"\\");
		if(!kvi_strEqualCI(szIcon.toLocal8Bit().data(),pcBuffer))
		{
			free(pcBuffer);
			return false;
		}
	}

	len = QUERY_BUFFER;
	if(RegOpenKeyEx(HKEY_CLASSES_ROOT,"KVIrcScript\\Shell\\Parse",0,KEY_READ,&hKey) != ERROR_SUCCESS)
	{
		free(pcBuffer);
		return false;
	}

	if(RegQueryValueEx( hKey,0,0,0,(LPBYTE)pcBuffer,&len) != ERROR_SUCCESS)
	{
		free(pcBuffer);
		return false;
	} else {
		if(!kvi_strEqualCI(__tr2qs("Run KVS Script").toLocal8Bit().data(),pcBuffer))
		{
			free(pcBuffer);
			return false;
		}
	}

	len = QUERY_BUFFER;
	if(RegOpenKeyEx(HKEY_CLASSES_ROOT,"KVIrcScript\\Shell\\Parse\\command",0,KEY_READ,&hKey) != ERROR_SUCCESS)
	{
		free(pcBuffer);
		return false;
	}

	if(RegQueryValueEx( hKey,0,0,0,(LPBYTE)pcBuffer,&len) != ERROR_SUCCESS)
	{
		free(pcBuffer);
		return false;
	} else {
		QString szCmd = applicationFilePath()+" \"%1\"";
		szCmd.replace('/',"\\");
		if(!kvi_strEqualCI(szCmd.toLocal8Bit().data(),pcBuffer))
		{
			free(pcBuffer);
			return false;
		}
	}

	//Config
	if(RegOpenKeyEx(HKEY_CLASSES_ROOT,".kvc",0,KEY_READ,&hKey) != ERROR_SUCCESS)
	{
		free(pcBuffer);
		return false;
	}

	if( (err=RegQueryValueEx( hKey,0,0,0,(LPBYTE)pcBuffer,&len)) != ERROR_SUCCESS)
	{
		free(pcBuffer);
		return false;
	} else {
		if(!kvi_strEqualCIN("KVIrcConfig",pcBuffer,11))
		{
			free(pcBuffer);
			return false;
		}
	}

	len = QUERY_BUFFER;
	if(RegOpenKeyEx(HKEY_CLASSES_ROOT,"KVIrcConfig",0,KEY_READ,&hKey) != ERROR_SUCCESS)
	{
		free(pcBuffer);
		return false;
	}

	//Addon
	if(RegOpenKeyEx(HKEY_CLASSES_ROOT,".kva",0,KEY_READ,&hKey) != ERROR_SUCCESS)
	{
		free(pcBuffer);
		return false;
	}

	if((err=RegQueryValueEx( hKey,0,0,0,(LPBYTE)pcBuffer,&len)) != ERROR_SUCCESS)
	{
		free(pcBuffer);
		return false;
	} else {
		if(!kvi_strEqualCIN("KVIrcAddon",pcBuffer,11))
		{
			free(pcBuffer);
			return false;
		}
	}

	len = QUERY_BUFFER;
	if(RegOpenKeyEx(HKEY_CLASSES_ROOT,"KVIrcAddon",0,KEY_READ,&hKey) != ERROR_SUCCESS)
	{
		free(pcBuffer);
		return false;
	}

	//Theme
	if(RegOpenKeyEx(HKEY_CLASSES_ROOT,".kvt",0,KEY_READ,&hKey) != ERROR_SUCCESS)
	{
		free(pcBuffer);
		return false;
	}

	if((err=RegQueryValueEx( hKey,0,0,0,(LPBYTE)pcBuffer,&len)) != ERROR_SUCCESS)
	{
		free(pcBuffer);
		return false;
	} else {
		if(!kvi_strEqualCIN("KVIrcTheme",pcBuffer,11))
		{
			free(pcBuffer);
			return false;
		}
	}

	len = QUERY_BUFFER;
	if(RegOpenKeyEx(HKEY_CLASSES_ROOT,"KVIrcTheme",0,KEY_READ,&hKey) != ERROR_SUCCESS)
	{
		free(pcBuffer);
		return false;
	}

	free(pcBuffer);
#endif
	return true;

}


#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
bool KviApplication::checkUriAssociations(const char * pcProto)
{
#define QUERY_BUFFER 2048
	char * pcBuffer;
	DWORD len = QUERY_BUFFER;
	DWORD err;
	pcBuffer = (char*)malloc(len*sizeof(char));
	HKEY hKey;

	KviCString szStoredKey = pcProto;
	KviCString szKey = szStoredKey;

	len = QUERY_BUFFER;
	if(RegOpenKeyEx(HKEY_CLASSES_ROOT,szKey,0,KEY_READ,&hKey) != ERROR_SUCCESS)
	{
		free(pcBuffer);
		return false;
	}

	if((err=RegQueryValueEx(hKey,0,0,0,(LPBYTE)pcBuffer,&len)) != ERROR_SUCCESS)
	{
		free(pcBuffer);
		return false;
	} else {
		if(!kvi_strEqualCI(__tr2qs("URL:IRC Protocol").toLocal8Bit().data(),pcBuffer))
		{
			free(pcBuffer);
			return false;
		}
	}

	len = QUERY_BUFFER;
	if((err=RegQueryValueEx(hKey,"URL Protocol",0,0,(LPBYTE)pcBuffer,&len)) != ERROR_SUCCESS)
	{
		free(pcBuffer);
		return false;
	}

	szKey = szStoredKey + "\\DefaultIcon";
	len = QUERY_BUFFER;
	if(RegOpenKeyEx(HKEY_CLASSES_ROOT,szKey,0,KEY_READ,&hKey) != ERROR_SUCCESS)
	{
		free(pcBuffer);
		return false;
	}

	if(RegQueryValueEx(hKey,0,0,0,(LPBYTE)pcBuffer,&len) != ERROR_SUCCESS)
	{
		free(pcBuffer);
		return false;
	} else {
		QString szIcon = applicationFilePath() + ",0";
		szIcon.replace('/',"\\");
		if(!kvi_strEqualCI(szIcon.toLocal8Bit().data(),pcBuffer))
		{
			free(pcBuffer);
			return false;
		}
	}

	len = QUERY_BUFFER;
	szKey = szStoredKey + "\\Shell\\open";
	if(RegOpenKeyEx(HKEY_CLASSES_ROOT,szKey,0,KEY_READ,&hKey) != ERROR_SUCCESS)
	{
		free(pcBuffer);
		return false;
	}

	if(RegQueryValueEx(hKey,0,0,0,(LPBYTE)pcBuffer,&len) != ERROR_SUCCESS)
	{
		free(pcBuffer);
		return false;
	} else {
		if(!kvi_strEqualCI(__tr2qs("Open with KVIrc").toLocal8Bit().data(),pcBuffer))
		{
			free(pcBuffer);
			return false;
		}
	}

	len = QUERY_BUFFER;
	szKey = szStoredKey + "\\Shell\\open\\command";
	if(RegOpenKeyEx(HKEY_CLASSES_ROOT,szKey,0,KEY_READ,&hKey) != ERROR_SUCCESS)
	{
		free(pcBuffer);
		return false;
	}

	if(RegQueryValueEx(hKey,0,0,0,(LPBYTE)pcBuffer,&len) != ERROR_SUCCESS)
	{
		free(pcBuffer);
		return false;
	} else {
		QString szCmd = applicationFilePath() + " \"%1\"";
		szCmd.replace('/',"\\");
		if(!kvi_strEqualCI(szCmd.toLocal8Bit().data(),pcBuffer))
		{
			free(pcBuffer);
			return false;
		}
	}

	free(pcBuffer);
#else
bool KviApplication::checkUriAssociations(const char *)
{
#endif
	return true;
}


#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
void KviApplication::setupUriAssociations(const char * pcProto)
{
	HKEY hKey;
	DWORD err;

	KviCString szStoredKey = pcProto;
	KviCString szKey = szStoredKey;

	QByteArray tmp;
	QString szAppPath = applicationFilePath();
	szAppPath.replace('/',"\\");

	SHDeleteKey(HKEY_CLASSES_ROOT,szKey);

	err = RegCreateKeyEx(HKEY_CLASSES_ROOT,szKey,0,0,0,KEY_WRITE,0,&hKey,0);
	RegSetValueEx(hKey,0,0,REG_SZ,(LPBYTE)"URL:IRC Protocol",16);
	RegSetValueEx(hKey,"URL Protocol",0,REG_SZ,(LPBYTE)"",0);

	szKey = szStoredKey + "\\DefaultIcon";
	RegCreateKeyEx(HKEY_CLASSES_ROOT,szKey,0,0,0,KEY_WRITE,0,&hKey,0);
	tmp = QString(szAppPath + ",0").toLocal8Bit();
	RegSetValueEx(hKey,0,0,REG_SZ,(LPBYTE)tmp.data(),tmp.length());

	szKey = szStoredKey + "\\Shell\\open";
	RegCreateKeyEx(HKEY_CLASSES_ROOT,szKey,0,0,0,KEY_WRITE,0,&hKey,0);
	tmp = __tr2qs("Open with KVIrc").toLocal8Bit();
	RegSetValueEx(hKey,0,0,REG_SZ,(LPBYTE)tmp.data(),tmp.length());

	szKey = szStoredKey + "\\Shell\\open\\command";
	RegCreateKeyEx(HKEY_CLASSES_ROOT,szKey,0,0,0,KEY_WRITE,0,&hKey,0);
	tmp = QString(szAppPath + " --external \"%1\"").toLocal8Bit();
	RegSetValueEx(hKey,0,0,REG_SZ,(LPBYTE)tmp.data(),tmp.length());
#else
void KviApplication::setupUriAssociations(const char *)
{
#endif
}

void KviApplication::setupFileAssociations()
{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	HKEY hKey;
	DWORD err;

	QByteArray tmp;
	QString szAppPath = applicationFilePath();
	szAppPath.replace('/',"\\");

	SHDeleteKey(HKEY_CLASSES_ROOT,".kvs");

	err = RegCreateKeyEx(HKEY_CLASSES_ROOT,".kvs",0,0,0,KEY_WRITE,0,&hKey,0);
	RegSetValueEx(hKey,0,0,REG_SZ,(LPBYTE)"KVIrcScript",11);

	SHDeleteKey(HKEY_CLASSES_ROOT,"KVIrcScript");
	RegCreateKeyEx(HKEY_CLASSES_ROOT,"KVIrcScript",0,0,0,KEY_WRITE,0,&hKey,0);
	tmp = __tr2qs("KVIrc KVS Script").toLocal8Bit();
	RegSetValueEx(hKey,0,0,REG_SZ,(LPBYTE)tmp.data(),tmp.length());

	RegCreateKeyEx(HKEY_CLASSES_ROOT,"KVIrcScript\\DefaultIcon",0,0,0,KEY_WRITE,0,&hKey,0);
	tmp = QString(szAppPath + ",1").toLocal8Bit();
	RegSetValueEx(hKey,0,0,REG_SZ,(LPBYTE)tmp.data(),tmp.length());

	RegCreateKeyEx(HKEY_CLASSES_ROOT,"KVIrcScript\\Shell\\Parse",0,0,0,KEY_WRITE,0,&hKey,0);
	tmp = __tr2qs("Run KVS Script").toLocal8Bit();
	RegSetValueEx(hKey,0,0,REG_SZ,(LPBYTE)tmp.data(),tmp.length());

	RegCreateKeyEx(HKEY_CLASSES_ROOT,"KVIrcScript\\Shell\\Parse\\command",0,0,0,KEY_WRITE,0,&hKey,0);
	tmp = QString(szAppPath + " \"%1\"").toLocal8Bit();
	RegSetValueEx(hKey,0,0,REG_SZ,(LPBYTE)tmp.data(),tmp.length());

	//Configs
	SHDeleteKey(HKEY_CLASSES_ROOT,".kvc");

	err = RegCreateKeyEx(HKEY_CLASSES_ROOT,".kvc",0,0,0,KEY_WRITE,0,&hKey,0);
	RegSetValueEx(hKey,0,0,REG_SZ,(LPBYTE)"KVIrcConfig",11);

	SHDeleteKey(HKEY_CLASSES_ROOT,"KVIrcConfig");
	RegCreateKeyEx(HKEY_CLASSES_ROOT,"KVIrcConfig",0,0,0,KEY_WRITE,0,&hKey,0);
	tmp = __tr2qs("KVIrc Configuration File").toLocal8Bit();
	RegSetValueEx(hKey,0,0,REG_SZ,(LPBYTE)tmp.data(),tmp.length());

	RegCreateKeyEx(HKEY_CLASSES_ROOT,"KVIrcConfig\\DefaultIcon",0,0,0,KEY_WRITE,0,&hKey,0);
	tmp = QString(szAppPath + ",2").toLocal8Bit();
	RegSetValueEx(hKey,0,0,REG_SZ,(LPBYTE)tmp.data(),tmp.length());

	// Themes
	SHDeleteKey(HKEY_CLASSES_ROOT,".kvt");

	err = RegCreateKeyEx(HKEY_CLASSES_ROOT,".kvt",0,0,0,KEY_WRITE,0,&hKey,0);
	RegSetValueEx(hKey,0,0,REG_SZ,(LPBYTE)"KVIrcTheme",11);

	SHDeleteKey(HKEY_CLASSES_ROOT,"KVIrcTheme");
	RegCreateKeyEx(HKEY_CLASSES_ROOT,"KVIrcTheme",0,0,0,KEY_WRITE,0,&hKey,0);
	tmp = __tr2qs("KVIrc Theme Package").toLocal8Bit();
	RegSetValueEx(hKey,0,0,REG_SZ,(LPBYTE)tmp.data(),tmp.length());

	RegCreateKeyEx(HKEY_CLASSES_ROOT,"KVIrcTheme\\DefaultIcon",0,0,0,KEY_WRITE,0,&hKey,0);
	tmp = QString(szAppPath + ",3").toLocal8Bit();
	RegSetValueEx(hKey,0,0,REG_SZ,(LPBYTE)tmp.data(),tmp.length());

	RegCreateKeyEx(HKEY_CLASSES_ROOT,"KVIrcTheme\\Shell\\Install",0,0,0,KEY_WRITE,0,&hKey,0);
	tmp = __tr2qs("Install Theme Package").toLocal8Bit();
	RegSetValueEx(hKey,0,0,REG_SZ,(LPBYTE)tmp.data(),tmp.length());

	RegCreateKeyEx(HKEY_CLASSES_ROOT,"KVIrcTheme\\Shell\\Install\\command",0,0,0,KEY_WRITE,0,&hKey,0);
	tmp = QString(szAppPath + " \"%1\"").toLocal8Bit();
	RegSetValueEx(hKey,0,0,REG_SZ,(LPBYTE)tmp.data(),tmp.length());

	//Addons
	SHDeleteKey(HKEY_CLASSES_ROOT,".kva");

	err = RegCreateKeyEx(HKEY_CLASSES_ROOT,".kva",0,0,0,KEY_WRITE,0,&hKey,0);
	RegSetValueEx(hKey,0,0,REG_SZ,(LPBYTE)"KVIrcAddon",11);

	SHDeleteKey(HKEY_CLASSES_ROOT,"KVIrcAddon");
	RegCreateKeyEx(HKEY_CLASSES_ROOT,"KVIrcAddon",0,0,0,KEY_WRITE,0,&hKey,0);
	tmp = __tr2qs("KVIrc Addon Package").toLocal8Bit();
	RegSetValueEx(hKey,0,0,REG_SZ,(LPBYTE)tmp.data(),tmp.length());

	RegCreateKeyEx(HKEY_CLASSES_ROOT,"KVIrcAddon\\DefaultIcon",0,0,0,KEY_WRITE,0,&hKey,0);
	tmp = QString(szAppPath + ",4").toLocal8Bit();
	RegSetValueEx(hKey,0,0,REG_SZ,(LPBYTE)tmp.data(),tmp.length());

	RegCreateKeyEx(HKEY_CLASSES_ROOT,"KVIrcAddon\\Shell\\Install",0,0,0,KEY_WRITE,0,&hKey,0);
	tmp = __tr2qs("Install Package").toLocal8Bit();
	RegSetValueEx(hKey,0,0,REG_SZ,(LPBYTE)tmp.data(),tmp.length());

	RegCreateKeyEx(HKEY_CLASSES_ROOT,"KVIrcAddon\\Shell\\Install\\command",0,0,0,KEY_WRITE,0,&hKey,0);
	tmp = QString(szAppPath + " \"%1\"").toLocal8Bit();
	RegSetValueEx(hKey,0,0,REG_SZ,(LPBYTE)tmp.data(),tmp.length());

#endif
}

void KviApplication::findGlobalKvircDirectory()
{
	// Look for the installation directory
	// with the image and help files inside
#ifdef GLOBAL_KVIRC_DIR
	m_szGlobalKvircDir = GLOBAL_KVIRC_DIR;
	KviFileUtils::adjustFilePath(m_szGlobalKvircDir);
	if(checkGlobalKvircDirectory(m_szGlobalKvircDir))
		return;
#endif //GLOBAL_KVIRC_DIR

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)

	m_szGlobalKvircDir = applicationDirPath();
	KviFileUtils::adjustFilePath(m_szGlobalKvircDir);
	if(checkGlobalKvircDirectory(m_szGlobalKvircDir))
		return;

	KviMessageBox::warning("Unable to find the shared Kvirc directory.\n"\
		"I have tried %Q, but it seemed to fail\n" \
		"Trying to run anyway...\n",&m_szGlobalKvircDir);
#else

	// Check for MacOS X Bundle compilation
	#ifdef COMPILE_ON_MAC
		m_szGlobalKvircDir = applicationDirPath();
		m_szGlobalKvircDir += "/../Resources/";
	#else
		m_szGlobalKvircDir = KVIRC_RESOURCES_DIR;
	#endif //COMPILE_ON_MAC

	if(checkGlobalKvircDirectory(m_szGlobalKvircDir))
		return;

	m_szGlobalKvircDir = "";

	// DO NOT TRANSLATE THIS
	// THE TRANSLATION DIRECTORY WAS NOT FOUND YET
	// AND THE LOCALE IS NOT INITIALIZED AT ALL

	#ifdef COMPILE_KDE_SUPPORT
		KviMessageBox::warning("Unable to find the shared Kvirc directory.\n"\
			"The usual path for this directory is $KDEDIR/share/apps/kvirc.\n"\
			"Are you sure that 'make install' worked correctly ?\n"\
			"Please make sure that you have the read permission to that directory\n"\
			"and you have set KDEDIR correctly. You may also try to rerun 'make install'.\n"\
			"A detailed explaination of the Kvirc directory system is in the INSTALL document\n"\
			"shipped with the kvirc source dirstribution.\n"\
			"Trying to run anyway...");
	#elif defined(COMPILE_ON_MAC)
		KviMessageBox::warning("Unable to find the shared KVIrc directory.\n"\
			"The usual path for this directory is ./Contents/Resources/kvirc within your application bundle.\n"\
			"Something went wrong during the bundle creation.\n"\
			"Please read the documentation and make sure to set proper paths for --prefix, -bindir, -libdir and --datadir during the configure run.\n"\
			"Trying to run anyway...\n");
	#else //!defined(COMPILE_KDE_SUPPORT) && !defined(COMPILE_ON_MAC)
		KviMessageBox::warning("Unable to find the shared KVIrc directory.\n"\
			"The usual path for this directory is /usr/local/share/kvirc.\n"\
			"Are you sure that 'make install' worked correctly ?\n"\
			"Please make sure that you have the read permission to that directory.\n"\
			"You may also need to rerun 'make install'.\n"\
			"A detailed explaination of the KVIrc directory system is in the INSTALL document\n"\
			"shipped with the kvirc source dirstribution.\n"\
			"Trying to run anyway...\n");
	#endif //!COMPILE_ON_MAC
#endif
}


bool KviApplication::findLocalKvircDirectory()
{
	// Here we check if we already did the setup
	// and we have the kvirc local directory saved somewhere
#ifdef COMPILE_KDE_SUPPORT
	if(m_szConfigFile.isEmpty())
	{
		// don't do that if user supplied a config file :)
		KConfig oKCfg("kvirc");
		KConfigGroup oKCfgMainGroup(&oKCfg, "Main");

		m_szLocalKvircDir = oKCfgMainGroup.readEntry("LocalKvircDirectory");

		unsigned int uSourcesDate = oKCfgMainGroup.readEntry("SourcesDate").toInt();
		if(uSourcesDate < KVI_SOURCES_DATE_NUMERIC_FORCE_SETUP)
			return false; // we force a setup anyway

		// If we have it, ok...done
		if(checkLocalKvircDirectory())
			return true;
	}
#endif //COMPILE_KDE_SUPPORT

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	if(m_bPortable)
	{
		m_szLocalKvircDir = g_pApp->applicationDirPath() + KVI_PATH_SEPARATOR_CHAR + "Settings";
		if(checkLocalKvircDirectory())
			return true;
	}
#endif
	//Check if we have a special .kvirc.rc in $HOME
	QString szF;

	if(!m_szConfigFile.isEmpty())
	{
		QString szConfig = m_szConfigFile;
		if(QDir::isRelativePath(m_szConfigFile))
		{
			szF = QDir::homePath();
			szF += KVI_PATH_SEPARATOR;
			szF += m_szConfigFile;
		} else {
			szF = szConfig;
		}
	} else {
		szF = QDir::homePath();
		szF += KVI_PATH_SEPARATOR;
		szF += KVI_HOME_CONFIG_FILE_NAME;
	}

	//If the file exists, read the first non empty line.
	KviConfigurationFile cfgx(szF,KviConfigurationFile::Read);

	cfgx.setGroup("Main");
	m_szLocalKvircDir = cfgx.readEntry("LocalKvircDirectory","");

	unsigned int uSourcesDate = cfgx.readUIntEntry("SourcesDate",0);
	if(uSourcesDate < KVI_SOURCES_DATE_NUMERIC_FORCE_SETUP)
		return false; // we force a setup anyway

	// If we have it, ok...done
	return checkLocalKvircDirectory();
}

void KviApplication::loadDirectories()
{
	// First find the global (readable) one...
	findGlobalKvircDirectory();
	// Init locale
	QString szLocalePath = m_szGlobalKvircDir;
	szLocalePath.append(KVI_PATH_SEPARATOR);
	szLocalePath.append("locale");
	szLocalePath.append(KVI_PATH_SEPARATOR);

	//__debug_1arg("Global Kvirc directory is %s",m_szGlobalKvircDir.ptr());
	// Now look for the local (writable) one
	m_bFirstTimeRun = !findLocalKvircDirectory();

	KviLocale::init(this,szLocalePath, m_szLocalKvircDir);


	if(m_bFirstTimeRun)
		setupBegin();

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	if(m_bPortable) return;
#endif
	if(!checkFileAssociations()) setupFileAssociations();
	if(!checkUriAssociations("irc"))   setupUriAssociations("irc");
	if(!checkUriAssociations("ircs"))  setupUriAssociations("ircs");
	if(!checkUriAssociations("irc6"))  setupUriAssociations("irc6");
	if(!checkUriAssociations("ircs6")) setupUriAssociations("ircs6");
}

void KviApplication::setupBegin()
{
	//We must do the setup...ask the user..
	QString szSetupLib;

#ifdef KVIRC_MODULES_DIR
	szSetupLib = KVIRC_MODULES_DIR;
#else
	getGlobalKvircDirectory(szSetupLib,KviApplication::Modules);
#endif

	KviQString::ensureLastCharIs(szSetupLib, KVI_PATH_SEPARATOR_CHAR);

#if defined(COMPILE_ON_WINDOWS)
	szSetupLib.append("kvisetup.dll");
#elif defined(COMPILE_ON_MINGW)
	szSetupLib.append("libkvisetup.dll");
#else
	szSetupLib.append("libkvisetup.so");
#endif
	g_pSetupLibrary = new QLibrary(szSetupLib);
	if(!g_pSetupLibrary->load())
	{
		KviMessageBox::warning(__tr2qs("Ops...it looks like I can't load modules on this sytem.\n" \
			"I have been looking for the %s library but I haven't been able to load it\n" \
			"due to the following error: \"%s\"\nAborting."),szSetupLib.toUtf8().data(),g_pSetupLibrary->errorString().toUtf8().data());
#if defined(COMPILE_ON_WINDOWS)
		ExitProcess(-1);
#elif defined(COMPILE_ON_MINGW)
                ExitProcess(1);
#else
		::exit(-1);
#endif
	}

	bool (*sfunc)() = (bool(*)())g_pSetupLibrary->resolve("setup_begin");
	if(!sfunc)
	{
		KviMessageBox::warning(__tr2qs("Ops...it looks like you have a broken distribution.\n" \
			"The setup module does not export the \"setup_begin\" function.\n" \
			"Aborting!"));
#if defined(COMPILE_ON_WINDOWS)
		ExitProcess(-1);
#elif defined(COMPILE_ON_MINGW)
                ExitProcess(1);
#else
		::exit(-1);
#endif
	}

	bool bRet = sfunc();

	if(!bRet)
	{
		KviMessageBox::warning(__tr2qs("Setup aborted"));
#if defined(COMPILE_ON_WINDOWS)
		ExitProcess(-1);
#elif defined(COMPILE_ON_MINGW)
                ExitProcess(1);
#else
		::exit(-1);
#endif
	}

	// Now save it
	//Let it be done by setup function
//	saveKvircDirectory();
}

void KviApplication::setupFinish()
{
	if(!g_pSetupLibrary)
	{
		qDebug("Oops... lost the setup library ?");
		return;
	}

	void (*sfunc)() = (void(*)())g_pSetupLibrary->resolve("setup_finish");
	if(!sfunc)
	{
		KviMessageBox::warning(__tr2qs("Ops...it looks like you have a broken distribution.\n" \
			"The setup module does not export the \"setup_finish\" function.\n" \
			"Trying to continue anyway..."));
	}

	sfunc();

	g_pSetupLibrary->unload();
	delete g_pSetupLibrary;
	g_pSetupLibrary = 0;
}

void KviApplication::saveKvircDirectory()
{
	// Here we save the local directory path
#ifdef COMPILE_KDE_SUPPORT
	// In KDE we use the application config file
	if(m_szConfigFile.isEmpty())
	{
		// not if user supplied a config file
		KConfig * pCfg = new KConfig("kvirc");
		KConfigGroup * pCfgMainGroup = new KConfigGroup(pCfg,"Main");
		if(pCfg)
		{
			if(pCfg->accessMode() == KConfig::ReadWrite)
			{
				pCfgMainGroup->writeEntry("LocalKvircDirectory",m_szLocalKvircDir);
				pCfgMainGroup->writeEntry("SourcesDate",KVI_SOURCES_DATE_NUMERIC);
				pCfg->sync();
				delete pCfgMainGroup;
				pCfgMainGroup = 0;
				return;
			}
		}
	}
#endif //COMPILE_KDE_SUPPORT
	// In NON-KDE we use $HOME/.kvirc.rc or $HOME/kvirc.ini
	QString szF = QDir::homePath();
	if(!m_szConfigFile.isEmpty())
	{
		//Must be changed from QString::fromLocal8Bit to QTextCodec::codecForLocale()
		QString szConfig = m_szConfigFile;
		if(QDir::isRelativePath(szConfig))
		{
			szF += KVI_PATH_SEPARATOR;
			szF += szConfig;
		} else {
			szF = szConfig;
		}
	} else {
		szF += KVI_PATH_SEPARATOR;
		szF += KVI_HOME_CONFIG_FILE_NAME;
	}
	//FIXME: LOCALE BROKEN!!!

	KviConfigurationFile cfgx(szF,KviConfigurationFile::Write);

	cfgx.setGroup("Main");
	cfgx.writeEntry("LocalKvircDirectory",m_szLocalKvircDir);
	cfgx.writeEntry("SourcesDate",KVI_SOURCES_DATE_NUMERIC);
}
