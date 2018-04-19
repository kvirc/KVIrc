//=============================================================================
//
//   File : KviApplication_filesystem.cpp
//   Creation date : Thu Dec 24 1998 04:05:26 by Szymon Stefanek
//
//   This file is part of the Kvirc irc client distribution
//   Copyright (C) 1998-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_debug.h"
#include "KviApplication.h"
#include "KviLocale.h"
#include "KviFileUtils.h"
#include "KviMediaManager.h"
#include "KviEnvironment.h"
#include "KviTimeUtils.h"
#include "KviOptions.h"
#include "kvi_defaults.h"
#include "KviIconManager.h"

#include <QDir>
#include <QFileInfo>
#include <QDateTime>

extern KviMediaManager * g_pMediaManager;

void KviApplication::getGlobalKvircDirectory(QString & szData, KvircSubdir dir, const QString & appendFile)
{
	szData = m_szGlobalKvircDir;
	KviQString::ensureLastCharIs(szData, KVI_PATH_SEPARATOR_CHAR);

	switch(dir)
	{
		case None:
			break;
		case Pics:
			szData.append("pics");
			break;
		case Modules:
#ifdef KVIRC_MODULES_DIR
			szData = KVIRC_MODULES_DIR;
#else
			szData.append("modules");
#endif
			break;
		case Plugins:
			szData.append("modules");
			break;
		case EasyPlugins:
			szData.append("easyplugins");
			break;
		case ConfigPlugins:
			KviQString::appendFormatted(szData, "config%smodules", KVI_PATH_SEPARATOR);
			break;
		case ConfigScripts:
			KviQString::appendFormatted(szData, "config%sscripts", KVI_PATH_SEPARATOR);
			break;
		case Help:
			szData.append("help");
			{
				// Localized help
				QString tmp = KviLocale::instance()->localeName();
				tmp.prepend(KVI_PATH_SEPARATOR_CHAR);
				tmp.prepend(szData);
				if(KviFileUtils::directoryExists(tmp))
					szData = tmp;
				else
				{
					tmp = szData;
					KviQString::appendFormatted(tmp, "%sen", KVI_PATH_SEPARATOR);
					if(KviFileUtils::directoryExists(tmp))
						szData = tmp;
				}
			}
			break;
		case HelpEN:
			KviQString::appendFormatted(szData, "help%sen", KVI_PATH_SEPARATOR);
			break;
		case HelpNoIntl:
			szData.append("help");
			break;
		case Log:
			qDebug("WARNING Global log directory requested!");
			break;
		case Incoming:
			qDebug("WARNING Global incoming directory requested!");
			break;
		case Trash:
			qDebug("WARNING Global trash directory requested!");
			break;
		case Config:
			szData.append("config");
			break;
		case Audio:
			szData.append("audio");
			break;
		case Scripts:
			szData.append("scripts");
			break;
		case MsgColors:
			szData.append("msgcolors");
			break;
		case Charmaps:
			szData.append("charmaps");
			break;
		case Avatars:
			szData.append("avatars");
			break;
		case DefScript:
			szData.append("defscript");
			break;
		case License:
			szData.append("license");
			break;
		case Filters:
			szData.append("filters");
			break;
		case Locale:
			szData.append("locale");
			break;
		case Tmp:
			qDebug("WARNING Global tmp directory requested!");
			break;
		case Themes:
			szData.append("themes");
			break;
		case Classes:
			szData.append("classes");
			break;
		case SmallIcons:
			szData.append("pics" KVI_PATH_SEPARATOR KVI_SMALLICONS_SUBDIRECTORY);
			break;
	}

	if(!appendFile.isEmpty())
	{
		KviQString::ensureLastCharIs(szData, KVI_PATH_SEPARATOR_CHAR);
		szData.append(appendFile);
	}

	KviFileUtils::adjustFilePath(szData);
}

void KviApplication::getLocalKvircDirectory(QString & szData, KvircSubdir dir, const QString & appendFile, bool bCreateIfNeeded)
{
	szData = m_szLocalKvircDir;
	KviQString::ensureLastCharIs(szData, KVI_PATH_SEPARATOR_CHAR);

	switch(dir)
	{
		case None:
			break;
		case Pics:
			szData.append("pics");
			break;
		case EasyPlugins:
			szData.append("easyplugins");
			break;
		case Config:
			szData.append("config");
			break;
		case ConfigPlugins:
			szData.append("config" KVI_PATH_SEPARATOR "modules");
			break;
		case ConfigScripts:
			szData.append("config" KVI_PATH_SEPARATOR "scripts");
			break;
		case Log:
			szData = KVI_OPTION_STRING(KviOption_stringLogsPath);
			// we should take special care here
			// the user is likely to mess the path behind our back
			// try to recover from most common problems
			while(szData.endsWith(KVI_PATH_SEPARATOR_CHAR))
				szData = szData.remove(szData.length() - 1, 1);
			if(szData.isEmpty())
			{
				szData = m_szLocalKvircDir;
				if(!szData.endsWith(QString(QChar(KVI_PATH_SEPARATOR_CHAR))))
					szData.append(KVI_PATH_SEPARATOR_CHAR);
				szData.append("log");
				KVI_OPTION_STRING(KviOption_stringLogsPath) = szData;
			}
			break;
		case Incoming:
			szData = KVI_OPTION_STRING(KviOption_stringIncomingPath);
			// we should take special care here
			// the user is likely to mess the path behind our back
			// try to recover from most common problems
			while(szData.endsWith(KVI_PATH_SEPARATOR_CHAR))
				szData = szData.remove(szData.length() - 1, 1);
			if(szData.isEmpty())
			{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
				if(m_bPortable)
				{
					szData = m_szLocalKvircDir;
				}
				else
				{
#endif
					szData = QDir::homePath();
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
				}
#endif
				if(!szData.endsWith(QString(QChar(KVI_PATH_SEPARATOR_CHAR))))
					szData.append(KVI_PATH_SEPARATOR_CHAR);
				szData.append(KVI_DEFAULT_INCOMING_SUBDIRECTORY_NAME);
				KVI_OPTION_STRING(KviOption_stringIncomingPath) = szData;
			}
			break;
		case Help:
			szData.append("help");
			{
				// Localized help
				QString tmp = KviLocale::instance()->localeName();
				tmp.prepend(KVI_PATH_SEPARATOR_CHAR);
				tmp.prepend(szData);
				if(KviFileUtils::directoryExists(tmp))
					szData = tmp;
				else
				{
					tmp = szData;
					tmp.append(KVI_PATH_SEPARATOR "en");
					if(KviFileUtils::directoryExists(tmp))
						szData = tmp;
				}
			}
			break;
		case HelpEN:
			szData.append("help" KVI_PATH_SEPARATOR "en");
			break;
		case HelpNoIntl:
			szData.append("help");
			break;
		case Audio:
			szData.append("audio");
			break;
		case Scripts:
			szData.append("scripts");
			break;
		case Modules:
		case Plugins:
			szData.append("modules");
			break;
		case Trash:
			szData.append("trash");
			break;
		case MsgColors:
			szData.append("msgcolors");
			break;
		case Charmaps:
			szData.append("charmaps");
			break;
		case Avatars:
			szData.append("avatars");
			break;
		case License:
			szData.append("license");
			break;
		case Filters:
			szData.append("filters");
			break;
		case Tmp:
			szData.append("tmp");
			break;
		case Locale:
			szData.append("locale");
			break;
		case Themes:
			szData.append("themes");
			break;
		case DefScript:
			szData.append("defscript"); /* should never happen! */
			break;
		case Classes:
			szData.append("classes");
			break;
		case SmallIcons:
			szData.append("pics" KVI_PATH_SEPARATOR KVI_SMALLICONS_SUBDIRECTORY);
			break;
	}
	if(bCreateIfNeeded)
		KviFileUtils::makeDir(szData);

	if(!appendFile.isEmpty())
	{
		KviQString::ensureLastCharIs(szData, KVI_PATH_SEPARATOR_CHAR);
		szData.append(appendFile);
	}

	KviFileUtils::adjustFilePath(szData);
}

void KviApplication::getTmpFileName(QString & szBuffer, const QString & szEndingFileName)
{
	QString tmp;
	getLocalKvircDirectory(tmp, Tmp);
	KviQString::ensureLastCharIs(tmp, KVI_PATH_SEPARATOR_CHAR);

	struct timeval tmv;
	kvi_gettimeofday(&tmv);

	QString szFileName = szEndingFileName.isNull() ? QString("file.tmp") : szEndingFileName;
	do
	{
		szBuffer = tmp;
		KviQString::appendFormatted(szBuffer, "kvirc_%d_%Q", tmv.tv_usec, &szFileName);
		tmv.tv_usec++;
	} while(KviFileUtils::fileExists(szBuffer));
}

void KviApplication::completeDirectory(const QString & word, std::vector<QString> & matches)
{
	QString szDir = word;
	QString szFile = word;
	KviQString::cutToLast(szFile, KVI_PATH_SEPARATOR_CHAR);
	KviQString::cutFromLast(szDir, KVI_PATH_SEPARATOR_CHAR);
	KviFileUtils::adjustFilePath(szDir);
	KviQString::ensureLastCharIs(szDir, KVI_PATH_SEPARATOR_CHAR);
	QDir d(szDir);

	// FIXME: maybe check timestamps ? (old modules)
	QStringList sl = d.entryList(QDir::Dirs);
	QStringList::Iterator it;

	for(auto & t : sl)
	{
		if(!KviQString::equalCS(t, ".") && !KviQString::equalCS(t, ".."))
		{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
			if(KviQString::equalCIN(szFile, t, szFile.length()))
#else
			if(KviQString::equalCSN(szFile, t, szFile.length()))
#endif
			{
				t.prepend(szDir);
				t.append(KVI_PATH_SEPARATOR_CHAR);
				matches.push_back(std::move(t));
			}
		}
	}

	sl = d.entryList(QDir::Files);

	for(auto & t : sl)
	{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		if(KviQString::equalCIN(szFile, t, szFile.length()))
#else
		if(KviQString::equalCSN(szFile, t, szFile.length()))
#endif
		{
			t.prepend(szDir);
			matches.push_back(std::move(t));
		}
	}
}

//
// getChannelDumpLogFileName
//
// Returns a valid filename for the channel dump log
//

void KviApplication::getChannelDumpLogFileName(QString & szStr)
{
	unsigned int uLogN = 0;
	QString szPrefix = "kick_";
	do
	{
		QString szName = QString("%1%2.log").arg(szPrefix).arg(uLogN);
		getLocalKvircDirectory(szStr, Log, szName);
		uLogN++;
		if(uLogN > 9999)
		{
			// Time to clear logs! :)
			szPrefix += "x";
			uLogN = 0;
		}
	} while(KviFileUtils::fileExists(szStr));
	KviFileUtils::adjustFilePath(szStr);
}

//
// findImage
//
// Find a file inside the default "Image" directories
// This should be thread safe (and in fact IS used by other threads)
// This function should be used only for finding the INTERNAL
// pixmaps... (so we expect them to be in the pics directory or
// Eventually in user's $HOME
//

// FIXME: #warning "Check WHEN findImage is used and when findUserFile is used...we have a mess here"

bool KviApplication::findImageInImageSearchPath(QString & szRetPath, const QString & filename)
{
	// first lookup the user defined paths
	for(auto & it : KVI_OPTION_STRINGLIST(KviOption_stringlistImageSearchPaths))
	{
		szRetPath = it;
		KviQString::ensureLastCharIs(szRetPath, KVI_PATH_SEPARATOR_CHAR);
		szRetPath.append(filename);
		//qDebug("LOOK FOR %s",szRetPath.ptr());
		if(KviFileUtils::fileExists(szRetPath))
			return true;
	}

	return false;
}

static KviApplication::KvircSubdir pics_localsubdirs[2] = { KviApplication::Pics, KviApplication::Avatars };
static KviApplication::KvircSubdir pics_globalsubdirs[2] = { KviApplication::Pics, KviApplication::Avatars };

bool KviApplication::mapImageFile(QString & szRetPath, const QString & filename)
{
	szRetPath = filename;
	// can't map non absolute paths
	if(!KviFileUtils::isAbsolutePath(filename))
		return false;
	// can't map non existing paths
	if(!KviFileUtils::fileExists(filename))
		return false;

	QFileInfo fi = QFileInfo(filename);

	unsigned int size = fi.size();

	QString szBestMatch;

	while(szRetPath.contains(KVI_PATH_SEPARATOR))
	{
		KviQString::cutToFirst(szRetPath, KVI_PATH_SEPARATOR);

		if(szRetPath.isEmpty())
			break;

		QString szBuffer;

		int i;

		for(i = 0; i < 2; i++)
		{
			getLocalKvircDirectory(szBuffer, pics_localsubdirs[i], szRetPath);
			if(KviFileUtils::fileExists(szBuffer))
			{
				QFileInfo fi2(szBuffer);
				if(size == fi2.size())
				{
					// probably the same file
					szBestMatch = szRetPath;
				}
			}
		}

		for(i = 0; i < 2; i++)
		{
			getGlobalKvircDirectory(szBuffer, pics_globalsubdirs[i], szRetPath);
			//qDebug("CHECK %s",szBuffer.ptr());
			if(KviFileUtils::fileExists(szBuffer))
			{
				QFileInfo fi2(szBuffer);
				if(size == fi2.size())
				{
					// probably the same file
					szBestMatch = szRetPath;
				}
			}
		}

		// Last resort
		szBuffer = QDir::homePath();
		KviQString::ensureLastCharIs(szBuffer, KVI_PATH_SEPARATOR_CHAR);
		szBuffer.append(szRetPath);
		if(KviFileUtils::fileExists(szBuffer))
		{
			QFileInfo fi2(szBuffer);
			if(size == fi2.size())
			{
				// prolly the same file
				szBestMatch = szRetPath;
			}
		}
	}

	if(!szBestMatch.isEmpty())
	{
		szRetPath = szBestMatch;
		return true;
	}

	// no way to map it :/
	szRetPath = filename;
	return false;
}

bool KviApplication::findImage(QString & szRetPath, const QString & filename)
{
	// Find a user file...
	if(KviFileUtils::isAbsolutePath(filename))
	{
		szRetPath = filename;
		return KviFileUtils::fileExists(filename);
	}

	// if we have a theme, look it up as first
	if(!KVI_OPTION_STRING(KviOption_stringIconThemeSubdir).isEmpty())
	{
		QString szTmp = KVI_OPTION_STRING(KviOption_stringIconThemeSubdir);
		szTmp.append(KVI_PATH_SEPARATOR_CHAR);
		szTmp.append(filename);
		getLocalKvircDirectory(szRetPath, KviApplication::Themes, szTmp);
		if(KviFileUtils::fileExists(szRetPath))
			return true;

		getGlobalKvircDirectory(szRetPath, KviApplication::Themes, szTmp);
		if(KviFileUtils::fileExists(szRetPath))
			return true;
	}

	int i;

	for(i = 0; i < 2; i++)
	{
		getLocalKvircDirectory(szRetPath, pics_localsubdirs[i], filename);
		if(KviFileUtils::fileExists(szRetPath))
			return true;
	}

	for(i = 0; i < 2; i++)
	{
		getGlobalKvircDirectory(szRetPath, pics_globalsubdirs[i], filename);
		if(KviFileUtils::fileExists(szRetPath))
			return true;
	}

	if(findImageInImageSearchPath(szRetPath, filename))
		return true;

	// Last resort
	szRetPath = QDir::homePath();
	KviQString::ensureLastCharIs(szRetPath, KVI_PATH_SEPARATOR_CHAR);
	szRetPath.append(filename);
	if(KviFileUtils::fileExists(szRetPath))
		return true;

	szRetPath = filename;

	return false;
}

bool KviApplication::findImageThemeOnlyCompat(QString & szRetPath, const QString & filename)
{
	// if we have a theme, look it up as first
	if(!KVI_OPTION_STRING(KviOption_stringIconThemeSubdir).isEmpty())
	{
		QString szTmp = KVI_OPTION_STRING(KviOption_stringIconThemeSubdir);
		szTmp.append(KVI_PATH_SEPARATOR_CHAR);
		szTmp.append(filename);

		getLocalKvircDirectory(szRetPath, KviApplication::Themes, szTmp);
		if(KviFileUtils::fileExists(szRetPath))
			return true;

		getGlobalKvircDirectory(szRetPath, KviApplication::Themes, szTmp);
		if(KviFileUtils::fileExists(szRetPath))
			return true;
	}

	getLocalKvircDirectory(szRetPath, KviApplication::Pics, filename);
	if(KviFileUtils::fileExists(szRetPath))
		return true;

	if(findImageInImageSearchPath(szRetPath, filename))
		return true;

	szRetPath = filename;

	return false;
}

bool KviApplication::findSmallIcon(QString & szRetPath, const QString & filename)
{
	// this is a bit optimized for the small builtin icons
	// looks up less places.

	// if we have a theme, look it up as first
	if(!KVI_OPTION_STRING(KviOption_stringIconThemeSubdir).isEmpty())
	{
		QString szTmp = KVI_OPTION_STRING(KviOption_stringIconThemeSubdir);
		szTmp.append(KVI_PATH_SEPARATOR_CHAR);
		szTmp.append(KVI_SMALLICONS_SUBDIRECTORY);
		szTmp.append(KVI_PATH_SEPARATOR_CHAR);
		szTmp.append(filename);

		getLocalKvircDirectory(szRetPath, KviApplication::Themes, szTmp);
		if(KviFileUtils::fileExists(szRetPath))
			return true;

		getGlobalKvircDirectory(szRetPath, KviApplication::Themes, szTmp);
		if(KviFileUtils::fileExists(szRetPath))
			return true;
	}

	// unlikely
	getLocalKvircDirectory(szRetPath, KviApplication::SmallIcons, filename);
	if(KviFileUtils::fileExists(szRetPath))
		return true;

	getLocalKvircDirectory(szRetPath, KviApplication::Pics, filename);
	if(KviFileUtils::fileExists(szRetPath))
		return true;

	// likely
	getGlobalKvircDirectory(szRetPath, KviApplication::SmallIcons, filename);
	if(KviFileUtils::fileExists(szRetPath))
		return true;

	if(findImageInImageSearchPath(szRetPath, filename))
		return true;

	// Last resort... bleah :/
	szRetPath = QDir::homePath();
	KviQString::ensureLastCharIs(szRetPath, KVI_PATH_SEPARATOR_CHAR);
	szRetPath.append(filename);
	if(KviFileUtils::fileExists(szRetPath))
		return true;

	szRetPath = filename;

	return false;
}

bool KviApplication::findAudioFile(QString & szRetPath, const QString & szFileName)
{
	if(KviFileUtils::isAbsolutePath(szFileName))
	{
		szRetPath = szFileName;
		return KviFileUtils::fileExists(szFileName);
	}

	getLocalKvircDirectory(szRetPath, KviApplication::Audio, szFileName);
	if(KviFileUtils::fileExists(szRetPath))
		return true;

	getGlobalKvircDirectory(szRetPath, KviApplication::Audio, szFileName);
	if(KviFileUtils::fileExists(szRetPath))
		return true;

	// Last resort
	szRetPath = QDir::homePath();
	KviQString::ensureLastCharIs(szRetPath, KVI_PATH_SEPARATOR_CHAR);
	szRetPath.append(szFileName);
	if(KviFileUtils::fileExists(szRetPath))
		return true;

	szRetPath = szFileName;

	return false;
}

//
// getReadOnlyConfigPath
//
// We want to READ a config file...find it
//

bool KviApplication::getReadOnlyConfigPath(QString & buffer, const char * config_name, KvircSubdir sbd, bool bNoFail)
{
	// Take a look in the local directory....
	getLocalKvircDirectory(buffer, sbd, config_name);
	//qDebug("%s %s %i |%s| %i",__FILE__,__FUNCTION__,__LINE__,buffer.ptr(),KviFileUtils::fileExists(buffer.ptr()));
	if(!KviFileUtils::fileExists(buffer))
	{
		// No saved config yet... check for defaults
		QString tmp;
		getGlobalKvircDirectory(tmp, sbd, config_name);
		if(!KviFileUtils::fileExists(tmp))
		{
			// No defaults...no such config file at all.
			if(bNoFail)
				getLocalKvircDirectory(buffer, sbd, config_name);
			return false;
		}
		else
			buffer = tmp; // get the defaults this time
	}                             // else file exists...
	return true;
}
