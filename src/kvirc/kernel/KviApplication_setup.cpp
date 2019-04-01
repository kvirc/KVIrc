//=============================================================================
//
//   File : KviApplication_setup.cpp
//   Creation date : Fri Apr 2 1999 02:38:05 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 1999-2010 Szymon Stefanek (pragma at kvirc dot net)
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
#else
#include <cstdlib> // for getenv()
#include <unistd.h> // for symlink() <-- unused?

#ifdef COMPILE_KDE_SUPPORT
#include <kconfig.h>
#include <KConfigGroup>
#endif
#endif

#include <QTextCodec>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QLibrary>

static QLibrary * g_pSetupLibrary = nullptr;

//
// Things launched at startup:
// - Attempt to find the global KVIrc directory
// - Attempt to find the local KVIrc directory
//   and if it is not found, ask the user to choose one
//

bool KviApplication::checkGlobalKvircDirectory(const QString szDir)
{
	//First check if the help subdir exists
	QString szHelpDir = szDir;
	szHelpDir += KVI_PATH_SEPARATOR "help";
	if(!KviFileUtils::directoryExists(szHelpDir))
		return false;
	//Then check if the pics subdir exists
	QString szPicsDir = szDir;
	szPicsDir += KVI_PATH_SEPARATOR "pics";
	if(!KviFileUtils::directoryExists(szPicsDir))
		return false;
	//Now make sure that it is the dir that we're looking for.
	//Check for an image file that we need.
	szPicsDir.append(KVI_PATH_SEPARATOR);
	szPicsDir.append(KVI_ACTIVITYMETER_IMAGE_NAME);
	return KviFileUtils::isReadable(szPicsDir);
}

bool KviApplication::checkLocalKvircDirectory(const QString & szSpecificPath)
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
	getLocalKvircDirectory(szBuff, Config);
	if(!KviFileUtils::directoryExists(szBuff))
		return false;

	return true;
}

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
void KviApplication::setupUriAssociations(const QString & szProto)
{
	HKEY hKey;
	std::wstring tmp;

	QString szAppPath = applicationFilePath().replace('/', "\\");

	tmp = QString("Software\\Classes\\" + szProto).toStdWString();
	SHDeleteKey(HKEY_CURRENT_USER, (LPCWSTR)tmp.c_str());
	RegCreateKeyEx(HKEY_CURRENT_USER, (LPCWSTR)tmp.c_str(), 0, nullptr, 0, KEY_WRITE, nullptr, &hKey, nullptr);
	RegSetValueEx(hKey, 0, 0, REG_SZ, (LPBYTE)TEXT("URL:IRC Protocol"), 16 * 2 + 1);
	RegSetValueEx(hKey, TEXT("URL Protocol"), 0, REG_SZ, (LPBYTE)"", 0);

	tmp = QString("Software\\Classes\\" + szProto + "\\DefaultIcon").toStdWString();
	RegCreateKeyEx(HKEY_CURRENT_USER, (LPCWSTR)tmp.c_str(), 0, nullptr, 0, KEY_WRITE, nullptr, &hKey, nullptr);
	tmp = QString(szAppPath + ",0").toStdWString();
	RegSetValueEx(hKey, 0, 0, REG_SZ, (LPBYTE)tmp.c_str(), tmp.length() * 2 + 1);

	tmp = QString("Software\\Classes\\" + szProto + "\\Shell\\open").toStdWString();
	RegCreateKeyEx(HKEY_CURRENT_USER, (LPCWSTR)tmp.c_str(), 0, nullptr, 0, KEY_WRITE, nullptr, &hKey, nullptr);
	tmp = __tr2qs("Open with KVIrc").toStdWString();
	RegSetValueEx(hKey, 0, 0, REG_SZ, (LPBYTE)tmp.c_str(), tmp.length() * 2 + 1);

	tmp = QString("Software\\Classes\\" + szProto + "\\Shell\\open\\command").toStdWString();
	RegCreateKeyEx(HKEY_CURRENT_USER, (LPCWSTR)tmp.c_str(), 0, nullptr, 0, KEY_WRITE, nullptr, &hKey, nullptr);
	tmp = QString(szAppPath + " --external \"%1\"").toStdWString();
	RegSetValueEx(hKey, 0, 0, REG_SZ, (LPBYTE)tmp.c_str(), tmp.length() * 2 + 1);

#else
void KviApplication::setupUriAssociations(const QString &)
{
#endif
}

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
void KviApplication::setFileAssociation(const QString & szExtension, const QString & szClassName, const QString & szDescription, const int iIconIndex, const QString & szActionName)
{
	HKEY hKey;
	std::wstring tmp;

	QString szAppPath = applicationFilePath().replace('/', "\\");

	tmp = QString("Software\\Classes\\." + szExtension).toStdWString();
	SHDeleteKey(HKEY_CURRENT_USER, (LPCWSTR)tmp.c_str());
	RegCreateKeyEx(HKEY_CURRENT_USER, (LPCWSTR)tmp.c_str(), 0, nullptr, 0, KEY_WRITE, nullptr, &hKey, nullptr);
	tmp = szClassName.toStdWString();
	RegSetValueEx(hKey, 0, 0, REG_SZ, (LPBYTE)tmp.c_str(), tmp.length() * 2 + 1);

	tmp = QString("Software\\Classes\\" + szClassName).toStdWString();
	SHDeleteKey(HKEY_CURRENT_USER, (LPCWSTR)tmp.c_str());
	RegCreateKeyEx(HKEY_CURRENT_USER, (LPCWSTR)tmp.c_str(), 0, 0, 0, KEY_WRITE, 0, &hKey, 0);
	tmp = szDescription.toStdWString();
	RegSetValueEx(hKey, 0, 0, REG_SZ, (LPBYTE)tmp.c_str(), tmp.length() * 2 + 1);

	tmp = QString("Software\\Classes\\" + szClassName + "\\DefaultIcon").toStdWString();
	RegCreateKeyEx(HKEY_CURRENT_USER, (LPCWSTR)tmp.c_str(), 0, 0, 0, KEY_WRITE, 0, &hKey, 0);
	tmp = QString(szAppPath + ",%1").arg(iIconIndex).toStdWString();
	RegSetValueEx(hKey, 0, 0, REG_SZ, (LPBYTE)tmp.c_str(), tmp.length() * 2 + 1);

	if (!szActionName.isEmpty())
	{
		tmp = QString("Software\\Classes\\" + szClassName + "\\Shell\\Parse").toStdWString();
		RegCreateKeyEx(HKEY_CURRENT_USER, (LPCWSTR)tmp.c_str(), 0, 0, 0, KEY_WRITE, 0, &hKey, 0);
		tmp = szActionName.toStdWString();
		RegSetValueEx(hKey, 0, 0, REG_SZ, (LPBYTE)tmp.c_str(), tmp.length() * 2 + 1);

		tmp = QString("Software\\Classes\\" + szClassName + "\\Shell\\Parse\\command").toStdWString();
		RegCreateKeyEx(HKEY_CURRENT_USER, (LPCWSTR)tmp.c_str(), 0, 0, 0, KEY_WRITE, 0, &hKey, 0);
		tmp = QString(szAppPath + " \"%1\"").toStdWString();
		RegSetValueEx(hKey, 0, 0, REG_SZ, (LPBYTE)tmp.c_str(), tmp.length() * 2 + 1);
	}
}
#endif

void KviApplication::setupFileAssociations()
{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	setFileAssociation("kvs", "KVIrcScript", __tr2qs("KVIrc KVS Script"), 1, __tr2qs("Run KVS Script"));
	setFileAssociation("kvc", "KVIrcConfig", __tr2qs("KVIrc Configuration File"), 2, QString());
	setFileAssociation("kvt", "KVIrcTheme", __tr2qs("KVIrc Theme Package"), 3, __tr2qs("Install Theme Package"));
	setFileAssociation("kva", "KVIrcAddon", __tr2qs("KVIrc Addon Package"), 4, __tr2qs("Install Package"));
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

	KviMessageBox::warning("Unable to find the shared KVIrc directory.\n"
	                       "I have tried %Q, but it seemed to fail\n"
	                       "Trying to run anyway...\n",
	    &m_szGlobalKvircDir);
#else

// Check for macOS Bundle compilation
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
	KviMessageBox::warning("Unable to find the shared KVIrc directory.\n"
	                       "The usual path for this directory is $KDEDIR/share/apps/kvirc.\n"
	                       "Are you sure that 'make install' worked correctly?\n"
	                       "Please make sure that you have the read permission to that directory\n"
	                       "and you have set KDEDIR correctly. You may also try to rerun 'make install'.\n"
	                       "A detailed explanation of the KVIrc directory system is in the INSTALL document\n"
	                       "shipped with the KVIrc source distribution.\n"
	                       "Trying to run anyway...");
#elif defined(COMPILE_ON_MAC)
	KviMessageBox::warning("Unable to find the shared KVIrc directory.\n"
	                       "The usual path for this directory is ./Contents/Resources/kvirc within your application bundle.\n"
	                       "Something went wrong during the bundle creation.\n"
	                       "Please read the documentation and make sure to set proper paths for --prefix, -bindir, -libdir and --datadir during the configure run.\n"
	                       "Trying to run anyway...\n");
#else  //!defined(COMPILE_KDE_SUPPORT) && !defined(COMPILE_ON_MAC)
	KviMessageBox::warning("Unable to find the shared KVIrc directory.\n"
	                       "The usual path for this directory is /usr/local/share/kvirc.\n"
	                       "Are you sure that 'make install' worked correctly?\n"
	                       "Please make sure that you have the read permission to that directory.\n"
	                       "You may also need to rerun 'make install'.\n"
	                       "A detailed explanation of the KVIrc directory system is in the INSTALL document\n"
	                       "shipped with the KVIrc source distribution.\n"
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
		}
		else
		{
			szF = szConfig;
		}
	}
	else
	{
		szF = QDir::homePath();
		szF += KVI_PATH_SEPARATOR;
		szF += KVI_HOME_CONFIG_FILE_NAME;
	}

	//If the file exists, read the first non empty line.
	KviConfigurationFile cfgx(szF, KviConfigurationFile::Read);

	cfgx.setGroup("Main");
	m_szLocalKvircDir = cfgx.readEntry("LocalKvircDirectory", "");

	unsigned int uSourcesDate = cfgx.readUIntEntry("SourcesDate", 0);
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

	//__debug_1arg("Global KVIrc directory is %s",m_szGlobalKvircDir.ptr());
	// Now look for the local (writable) one
	m_bFirstTimeRun = !findLocalKvircDirectory();

	KviLocale::init(this, szLocalePath, m_szLocalKvircDir);

	if(m_bFirstTimeRun)
		setupBegin();

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	if(m_bPortable)
		return;
#endif
	setupFileAssociations();
	setupUriAssociations("irc");
	setupUriAssociations("ircs");
	setupUriAssociations("irc6");
	setupUriAssociations("ircs6");
}

void KviApplication::setupBegin()
{
	//We must do the setup...ask the user..
	QString szSetupLib;

#ifdef KVIRC_MODULES_DIR
	szSetupLib = KVIRC_MODULES_DIR;
#else
	getGlobalKvircDirectory(szSetupLib, KviApplication::Modules);
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
		KviMessageBox::warning(__tr2qs("Oops! It looks like I can't load modules on this system.\n"
		                               "I have been looking for the %s library but I haven't been able to load it\n"
		                               "due to the following error: \"%s\"\nAborting."),
		    szSetupLib.toUtf8().data(), g_pSetupLibrary->errorString().toUtf8().data());
#if defined(COMPILE_ON_WINDOWS)
		ExitProcess(-1);
#elif defined(COMPILE_ON_MINGW)
		ExitProcess(1);
#else
		::exit(-1);
#endif
	}

	bool (*sfunc)() = (bool (*)())g_pSetupLibrary->resolve("setup_begin");
	if(!sfunc)
	{
		KviMessageBox::warning(__tr2qs("Oops! It looks like you have a broken distribution.\n"
		                               "The setup module does not export the \"setup_begin\" function.\n"
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
#if defined(COMPILE_ON_WINDOWS)
		ExitProcess(-1);
#elif defined(COMPILE_ON_MINGW)
		ExitProcess(1);
#else
		::exit(-1);
#endif
	}

}

void KviApplication::setupFinish()
{
	if(!g_pSetupLibrary)
	{
		qDebug("Oops! Lost the setup library?");
		return;
	}

	void (*sfunc)() = (void (*)())g_pSetupLibrary->resolve("setup_finish");
	if(!sfunc)
	{
		KviMessageBox::warning(__tr2qs("Oops! It looks like you have a broken distribution.\n"
		                               "The setup module does not export the \"setup_finish\" function.\n"
		                               "Trying to continue anyway..."));
	}

	sfunc();

	g_pSetupLibrary->unload();
	delete g_pSetupLibrary;
	g_pSetupLibrary = nullptr;
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
		KConfigGroup * pCfgMainGroup = new KConfigGroup(pCfg, "Main");
		if(pCfg)
		{
			if(pCfg->accessMode() == KConfig::ReadWrite)
			{
				pCfgMainGroup->writeEntry("LocalKvircDirectory", m_szLocalKvircDir);
				pCfgMainGroup->writeEntry("SourcesDate", KVI_SOURCES_DATE_NUMERIC);
				pCfg->sync();
				delete pCfgMainGroup;
				pCfgMainGroup = nullptr;
				return;
			}
		}
	}
#endif //COMPILE_KDE_SUPPORT
	// In NON-KDE we use $HOME/.kvirc4.rc or $HOME/kvirc4.ini
	QString szF = QDir::homePath();
	if(!m_szConfigFile.isEmpty())
	{
		//Must be changed from QString::fromLocal8Bit to QTextCodec::codecForLocale()
		QString szConfig = m_szConfigFile;
		if(QDir::isRelativePath(szConfig))
		{
			szF += KVI_PATH_SEPARATOR;
			szF += szConfig;
		}
		else
		{
			szF = szConfig;
		}
	}
	else
	{
		szF += KVI_PATH_SEPARATOR;
		szF += KVI_HOME_CONFIG_FILE_NAME;
	}
	//FIXME: LOCALE BROKEN!!!

	KviConfigurationFile cfgx(szF, KviConfigurationFile::Write);

	cfgx.setGroup("Main");
	cfgx.writeEntry("LocalKvircDirectory", m_szLocalKvircDir);
	cfgx.writeEntry("SourcesDate", KVI_SOURCES_DATE_NUMERIC);
}
