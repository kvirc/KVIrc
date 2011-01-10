//=============================================================================
//
//   File : kvi_themeinfo.cpp
//   Creation date : Mon Jan 08 2007 03:23:00 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2007-2010 Szymon Stefanek (pragma at kvirc dot net)
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


#include "KviTheme.h"
#include "KviFileUtils.h"
#include "KviConfigurationFile.h"
#include "KviLocale.h"
#include "KviMiscUtils.h"
#include "kvi_sourcesdate.h"

#include <QImage>

#define KVI_THEME_SMALL_SCREENSHOT_NAME "screenshot_small.png"
#define KVI_THEME_MEDIUM_SCREENSHOT_NAME "screenshot_medium.png"
#define KVI_THEME_LARGE_SCREENSHOT_NAME "screenshot.png"

#define QIMAGE_SCALE_MIN Qt::KeepAspectRatio


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

	KviConfigurationFile cfg(szThemeFileName,KviConfigurationFile::Read);

	cfg.setGroup(KVI_THEMEINFO_CONFIG_GROUP);

	m_szThemeEngineVersion = cfg.readEntry("ThemeEngineVersion","1.0.0");
	if(KviMiscUtils::compareVersions(m_szThemeEngineVersion,KVI_CURRENT_THEME_ENGINE_VERSION) < 0)
	{
		m_szLastError = QString(__tr2qs("This KVIrc executable is too old for this theme (minimum theme engine version required is %1 while this theme engine has version %2)")).arg(m_szThemeEngineVersion,KVI_CURRENT_THEME_ENGINE_VERSION);
		return false; // incompatible theme engine (will not work)
	}

	// mandatory fields
	m_szName = cfg.readEntry("Name","");

	if(m_szName.isEmpty())
	{
		m_szLastError = __tr2qs("Theme information file is not valid");
		return false;
	}

	// optional fields
	m_szVersion = cfg.readEntry("Version","");
	if(m_szVersion.isEmpty())
		m_szVersion = "?.?.?";
	m_szAuthor = cfg.readEntry("Author","");
	QString szUnknown = __tr2qs("Unknown");
	if(m_szAuthor.isEmpty())
		m_szAuthor = szUnknown;
	m_szDescription = cfg.readEntry("Description","");
	m_szDate = cfg.readEntry("Date","");
	if(m_szDate.isEmpty())
		m_szDate = szUnknown;
	m_szApplication = cfg.readEntry("Application","");
	if(m_szApplication.isEmpty())
		m_szApplication = szUnknown;

	return true;
}

bool KviThemeInfo::save(const QString &szThemeFileName)
{
	KviConfigurationFile inf(szThemeFileName,KviConfigurationFile::Write);

	inf.clear();

	inf.setGroup(KVI_THEMEINFO_CONFIG_GROUP);

	inf.writeEntry("Name",m_szName);
	inf.writeEntry("Version",m_szVersion);
	inf.writeEntry("Author",m_szAuthor);
	inf.writeEntry("Description",m_szDescription);
	inf.writeEntry("Date",m_szDate);
	inf.writeEntry("ThemeEngineVersion",KVI_CURRENT_THEME_ENGINE_VERSION);
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

QString KviThemeInfo::smallScreenshotPath()
{
	QString ret;
	if(!m_szAbsoluteDirectory.isEmpty())
	{
		ret = m_szAbsoluteDirectory;
		KviQString::ensureLastCharIs(ret,KVI_PATH_SEPARATOR_CHAR);
		ret.append(KVI_THEME_SMALL_SCREENSHOT_NAME);
	}
	return ret;
}

const QPixmap & KviThemeInfo::smallScreenshot()
{
	if(!m_pixScreenshotSmall.isNull())return m_pixScreenshotSmall;

	if(!m_szAbsoluteDirectory.isEmpty())
	{
		QString szFileName = m_szAbsoluteDirectory;
		KviQString::ensureLastCharIs(szFileName,KVI_PATH_SEPARATOR_CHAR);
		szFileName.append(KVI_THEME_SMALL_SCREENSHOT_NAME);
		QPixmap pix(szFileName);
		if(!pix.isNull())
		{
			m_pixScreenshotSmall = pix;
			return m_pixScreenshotSmall;
		}
		// try to scale it from the large one (and save it by the way)
		pix = mediumScreenshot();
		if(pix.isNull())return m_pixScreenshotSmall;

		if(pix.width() > 300 || pix.height() > 225)
		{
			QImage sbri = pix.toImage();
			pix = QPixmap::fromImage(sbri.scaled(300,225,QIMAGE_SCALE_MIN,Qt::SmoothTransformation));
		}
		pix.save(szFileName,"PNG");

		m_pixScreenshotSmall = pix;
		return m_pixScreenshotSmall;
	}

	return m_pixScreenshotSmall;
}

const QPixmap & KviThemeInfo::mediumScreenshot()
{
	if(!m_pixScreenshotMedium.isNull())return m_pixScreenshotMedium;

	if(!m_szAbsoluteDirectory.isEmpty())
	{
		QString szFileName = m_szAbsoluteDirectory;
		KviQString::ensureLastCharIs(szFileName,KVI_PATH_SEPARATOR_CHAR);
		szFileName.append(KVI_THEME_MEDIUM_SCREENSHOT_NAME);
		QPixmap pix(szFileName);
		if(!pix.isNull())
		{
			m_pixScreenshotMedium = pix;
			return m_pixScreenshotMedium;
		}
		// try to scale it from the large one (and save it by the way)
		pix = largeScreenshot();
		if(pix.isNull())return m_pixScreenshotMedium;

		if(pix.width() > 600 || pix.height() > 450)
		{
			QImage sbri = pix.toImage();
			pix.fromImage(sbri.scaled(640,450,QIMAGE_SCALE_MIN,Qt::SmoothTransformation));
		}

		pix.save(szFileName,"PNG");

		m_pixScreenshotMedium = pix;
		return m_pixScreenshotMedium;
	}

	return m_pixScreenshotMedium;
}

const QPixmap & KviThemeInfo::largeScreenshot()
{
	if(!m_pixScreenshotLarge.isNull())return m_pixScreenshotLarge;

	if(!m_szAbsoluteDirectory.isEmpty())
	{
		QString szFileName = m_szAbsoluteDirectory;
		KviQString::ensureLastCharIs(szFileName,KVI_PATH_SEPARATOR_CHAR);
		szFileName.append(KVI_THEME_LARGE_SCREENSHOT_NAME);
		QPixmap pix(szFileName);
		if(pix.isNull())return m_pixScreenshotLarge;
		m_pixScreenshotLarge = pix;
	}
	return m_pixScreenshotLarge;
}


namespace KviTheme
{
	bool saveScreenshots(KviThemeInfo &options,const QString &szOriginalScreenshotPath)
	{
		QImage pix(szOriginalScreenshotPath);
		if(pix.isNull())
		{
			options.setLastError(__tr2qs("Failed to load the specified screenshot image"));
			return false;
		}

		QImage out;

		QString szScreenshotFileName = options.absoluteDirectory();
		if(szScreenshotFileName.isEmpty())
		{
			options.setLastError(__tr2qs("Invalid option"));
			return false;
		}

		KviQString::ensureLastCharIs(szScreenshotFileName,KVI_PATH_SEPARATOR_CHAR);
		szScreenshotFileName.append(KVI_THEME_LARGE_SCREENSHOT_NAME);
		if(!pix.save(szScreenshotFileName,"PNG"))
		{
			options.setLastError(__tr2qs("Failed to save the screenshot image"));
			return false;
		}

		if(pix.width() > 600 || pix.height() > 450)
			out = pix.scaled(640,450,QIMAGE_SCALE_MIN,Qt::SmoothTransformation);
		else
			out = pix;

		szScreenshotFileName = options.absoluteDirectory();
		KviQString::ensureLastCharIs(szScreenshotFileName,KVI_PATH_SEPARATOR_CHAR);
		szScreenshotFileName.append(KVI_THEME_MEDIUM_SCREENSHOT_NAME);
		if(!out.save(szScreenshotFileName,"PNG"))
		{
			options.setLastError(__tr2qs("Failed to save the screenshot image"));
			return false;
		}

		if(pix.width() > 300 || pix.height() > 225)
			out = pix.scaled(300,225,QIMAGE_SCALE_MIN,Qt::SmoothTransformation);
		else
			out = pix;

		szScreenshotFileName = options.absoluteDirectory();
		KviQString::ensureLastCharIs(szScreenshotFileName,KVI_PATH_SEPARATOR_CHAR);
		szScreenshotFileName.append(KVI_THEME_SMALL_SCREENSHOT_NAME);
		if(!out.save(szScreenshotFileName,"PNG"))
		{
			options.setLastError(__tr2qs("Failed to save the screenshot image"));
			return false;
		}

		return true;
	}
};
