//=============================================================================
//
//   File : kvi_themeinfo.cpp
//   Creation date : Mon Jan 08 2007 03:23:00 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2007 Szymon Stefanek (pragma at kvirc dot net)
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

#define __KVIRC__
#include "kvi_theme.h"
#include "kvi_fileutils.h"
#include "kvi_config.h"
#include "kvi_locale.h"
#include "kvi_miscutils.h"
#include "kvi_sourcesdate.h"

// increment only when there are INCOMPATIBLE changes in the file format (should never happen)
#define CURRENT_THEMEINFO_FILE_VERSION "1.0.0"


KviThemeInfo::KviThemeInfo()
: KviHeapObject()
{
}

KviThemeInfo::~KviThemeInfo()
{
}



bool KviThemeInfo::load(const QString &szThemeFileName)
{
	if(!KviFileUtils::fileExists(szThemeFileName))
	{
		m_szLastError = __tr2qs("The theme information file does not exist");
		return false;
	}

	KviConfig cfg(szThemeFileName,KviConfig::Read);

	cfg.setGroup(KVI_THEMEINFO_CONFIG_GROUP);

	QString tmp = cfg.readQStringEntry("ThemeInfoFileVersion",CURRENT_THEMEINFO_FILE_VERSION);
	if(KviMiscUtils::compareVersions(tmp,CURRENT_THEMEINFO_FILE_VERSION) < 0)
		return false; // incompatible ThemeInfoFileVersion

	// mandatory fields
	m_szName = cfg.readQStringEntry("Name","");

	if(m_szName.isEmpty())
	{
		m_szLastError = __tr2qs("Theme information file is not valid");
		return false;
	}

	// optional fields
	m_szVersion = cfg.readQStringEntry("Version","");
	if(m_szVersion.isEmpty())
		m_szVersion = "?.?.?";
	m_szAuthor = cfg.readQStringEntry("Author","");
	QString szUnknown = __tr2qs("Unknown");
	if(m_szAuthor.isEmpty())
		m_szAuthor = szUnknown;
	m_szDescription = cfg.readQStringEntry("Description","");
	m_szDate = cfg.readQStringEntry("Date","");
	if(m_szDate.isEmpty())
		m_szDate = szUnknown;
	m_szApplication = cfg.readQStringEntry("Application","");
	if(m_szApplication.isEmpty())
		m_szApplication = szUnknown;

	m_szMinimumKVIrcVersion = cfg.readQStringEntry("MinimumKVIrcVersion","3.0.0");
	// if present, must be verified

	if(!m_szMinimumKVIrcVersion.isEmpty())
	{
		if(KviMiscUtils::compareVersions(m_szMinimumKVIrcVersion,KVI_VERSION "." KVI_SOURCES_DATE) < 0)
		{
			KviQString::sprintf(m_szLastError,__tr2qs("This KVIrc executable is too old for this theme (minimum version required is %Q)"),&m_szMinimumKVIrcVersion);
			return false;
		}
	}

	return true;
}

bool KviThemeInfo::save(const QString &szThemeFileName)
{
	KviConfig inf(szThemeFileName,KviConfig::Write);

	inf.clear();

	inf.setGroup(KVI_THEMEINFO_CONFIG_GROUP);

	inf.writeEntry("Name",m_szName);
	inf.writeEntry("Version",m_szVersion);
	inf.writeEntry("Author",m_szAuthor);
	inf.writeEntry("Description",m_szDescription);
	inf.writeEntry("Date",m_szDate);
	
	if(!m_szMinimumKVIrcVersion.isEmpty())
	{
		if(
			(!KviMiscUtils::isValidVersionString(m_szMinimumKVIrcVersion))
			||
			(KviMiscUtils::compareVersions(m_szMinimumKVIrcVersion,KVI_VERSION "." KVI_SOURCES_DATE) < 0)
		)
		{
			m_szLastError = __tr2qs("The specified minimum KVIrc version is not valid");
			return false;
		}
	}

	inf.writeEntry("MinimumKVIrcVersion",m_szMinimumKVIrcVersion);
	inf.writeEntry("ThemeInfoFileVersion",CURRENT_THEMEINFO_FILE_VERSION);
	inf.writeEntry("Application","KVIrc " KVI_VERSION "." KVI_SOURCES_DATE);

	return true;
}

bool KviThemeInfo::loadFromDirectory(const QString &szThemeDirectory,bool bIgnoreThemeData)
{
	QString szD = szThemeDirectory;
	szD.append(KVI_PATH_SEPARATOR_CHAR);
	szD.append(KVI_THEMEINFO_FILE_NAME);
	
	if(!load(szD))
		return false; // loading failed for some reason

	m_szAbsoluteDirectory = szThemeDirectory;

	if(bIgnoreThemeData)
		return true; // assume success

	// check if themedata file exists
	szD = szThemeDirectory;
	szD.append(KVI_PATH_SEPARATOR_CHAR);
	szD.append(KVI_THEMEDATA_FILE_NAME);

	return KviFileUtils::fileExists(szD);
}

