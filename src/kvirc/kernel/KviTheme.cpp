//=============================================================================
//
//   File : KviTheme.cpp
//   Creation date : Mon Jan 08 2007 03:23:00 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2007-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviTheme.h"
#include "KviFileUtils.h"
#include "KviConfigurationFile.h"
#include "KviLocale.h"
#include "KviMiscUtils.h"
#include "kvi_sourcesdate.h"

#include <QImage>
#include <QDir>
#include <QStringList>

#define KVI_THEME_SMALL_SCREENSHOT_NAME "screenshot_small.png"
#define KVI_THEME_MEDIUM_SCREENSHOT_NAME "screenshot_medium.png"
#define KVI_THEME_LARGE_SCREENSHOT_NAME "screenshot.png"

#define QIMAGE_SCALE_MIN Qt::KeepAspectRatio

KviThemeInfo::KviThemeInfo()
    : KviHeapObject()
{
}

KviThemeInfo::~KviThemeInfo()
    = default;

void KviThemeInfo::setDirectoryAndLocation(const QString & szDirectory, Location eLocation)
{
	switch(eLocation)
	{
		case Auto:
			g_pApp->getLocalKvircDirectory(m_szDirectory, KviApplication::Themes, szDirectory);
			if(KviFileUtils::directoryExists(m_szDirectory))
			{
				m_eLocation = User;
			}
			else
			{
				g_pApp->getGlobalKvircDirectory(m_szDirectory, KviApplication::Themes, szDirectory);
				if(KviFileUtils::directoryExists(m_szDirectory))
				{
					m_eLocation = Builtin;
				}
				else
				{
					m_eLocation = External;
					m_szDirectory = szDirectory;
					m_szSubdirectory = KviFileUtils::extractFileName(szDirectory, false);
				}
			}
			break;
		case Builtin:
			m_eLocation = eLocation;
			m_szSubdirectory = szDirectory;
			g_pApp->getGlobalKvircDirectory(m_szDirectory, KviApplication::Themes, m_szSubdirectory);
			break;
		case User:
			m_eLocation = eLocation;
			m_szSubdirectory = szDirectory;
			g_pApp->getLocalKvircDirectory(m_szDirectory, KviApplication::Themes, m_szSubdirectory);
			break;
		default: // assume external
		{
			m_eLocation = External;
			m_szDirectory = szDirectory;
			m_szSubdirectory = KviFileUtils::extractFileName(szDirectory, false);
		}
		break;
	}

	KviQString::ensureLastCharIs(m_szDirectory, QChar(KVI_PATH_SEPARATOR_CHAR));

	//qDebug(
	//		"[KviTheme::setDirectoryAndLocation] Directory(%s), Subdirectory(%s), Location(%d)",
	//		m_szDirectory.toUtf8().data(),
	//		m_szSubdirectory.toUtf8().data(),
	//		m_eLocation
	//	);
}

bool KviThemeInfo::load(const QString & szDirectory, Location eLocation)
{
	setDirectoryAndLocation(szDirectory, eLocation);

	QString szThemeInfoFileName = m_szDirectory + KVI_THEMEINFO_FILE_NAME;
	QString szThemeDataFileName = m_szDirectory + KVI_THEMEDATA_FILE_NAME;

	if(!KviFileUtils::fileExists(szThemeInfoFileName))
	{
		m_szLastError = __tr2qs("The theme information file does not exist");
		return false;
	}

	if(!KviFileUtils::fileExists(szThemeDataFileName))
	{
		m_szLastError = __tr2qs("The theme data file does not exist");
		return false;
	}

	KviConfigurationFile cfg(szThemeInfoFileName, KviConfigurationFile::Read);

	cfg.setGroup(KVI_THEMEINFO_CONFIG_GROUP);

	m_szThemeEngineVersion = cfg.readEntry("ThemeEngineVersion", "1.0.0");
	if(KviMiscUtils::compareVersions(m_szThemeEngineVersion, KVI_CURRENT_THEME_ENGINE_VERSION) < 0)
	{
		m_szLastError = QString(__tr2qs("This KVIrc executable is too old for this theme (minimum theme engine version required is %1 while this theme engine has version %2)")).arg(m_szThemeEngineVersion, KVI_CURRENT_THEME_ENGINE_VERSION);
		return false; // incompatible theme engine (will not work)
	}

	// mandatory fields
	m_szName = cfg.readEntry("Name", "");

	if(m_szName.isEmpty())
	{
		m_szLastError = __tr2qs("Theme information file is not valid");
		return false;
	}

	// optional fields
	m_szVersion = cfg.readEntry("Version", "");
	if(m_szVersion.isEmpty())
		m_szVersion = "?.?.?";
	m_szAuthor = cfg.readEntry("Author", "");
	QString szUnknown = __tr2qs("Unknown");
	if(m_szAuthor.isEmpty())
		m_szAuthor = szUnknown;
	m_szDescription = cfg.readEntry("Description", "");
	m_szDate = cfg.readEntry("Date", "");
	if(m_szDate.isEmpty())
		m_szDate = szUnknown;
	m_szApplication = cfg.readEntry("Application", "");
	if(m_szApplication.isEmpty())
		m_szApplication = szUnknown;

	//qDebug(
	//		"[KviTheme::load] Directory(%s), Subdirectory(%s), Location(%d)",
	//		m_szDirectory.toUtf8().data(),
	//		m_szSubdirectory.toUtf8().data(),
	//		m_eLocation
	//	);

	return true;
}

bool KviThemeInfo::save(const QString & szThemeFileName)
{
	KviConfigurationFile inf(szThemeFileName, KviConfigurationFile::Write);

	inf.clear();

	inf.setGroup(KVI_THEMEINFO_CONFIG_GROUP);

	inf.writeEntry("Name", m_szName);
	inf.writeEntry("Version", m_szVersion);
	inf.writeEntry("Author", m_szAuthor);
	inf.writeEntry("Description", m_szDescription);
	inf.writeEntry("Date", m_szDate);
	inf.writeEntry("ThemeEngineVersion", KVI_CURRENT_THEME_ENGINE_VERSION);
	inf.writeEntry("Application", "KVIrc " KVI_VERSION "." KVI_SOURCES_DATE);

	return true;
}

QString KviThemeInfo::smallScreenshotPath()
{
	QString szRet = m_szDirectory;
	szRet.append(KVI_THEME_SMALL_SCREENSHOT_NAME);
	return szRet;
}

const QPixmap & KviThemeInfo::smallScreenshot()
{
	if(!m_pixScreenshotSmall.isNull())
		return m_pixScreenshotSmall;

	QString szFileName = m_szDirectory;
	szFileName.append(KVI_THEME_SMALL_SCREENSHOT_NAME);
	QPixmap pix(szFileName);
	if(!pix.isNull())
	{
		m_pixScreenshotSmall = pix;
		return m_pixScreenshotSmall;
	}
	// try to scale it from the large one (and save it by the way)
	pix = mediumScreenshot();
	if(pix.isNull())
		return m_pixScreenshotSmall;

	if(pix.width() > 300 || pix.height() > 225)
	{
		QImage sbri = pix.toImage();
		pix = QPixmap::fromImage(sbri.scaled(300, 225, QIMAGE_SCALE_MIN, Qt::SmoothTransformation));
	}
	pix.save(szFileName, "PNG");

	m_pixScreenshotSmall = pix;
	return m_pixScreenshotSmall;
}

const QPixmap & KviThemeInfo::mediumScreenshot()
{
	if(!m_pixScreenshotMedium.isNull())
		return m_pixScreenshotMedium;

	QString szFileName = m_szDirectory;
	szFileName.append(KVI_THEME_MEDIUM_SCREENSHOT_NAME);
	QPixmap pix(szFileName);
	if(!pix.isNull())
	{
		m_pixScreenshotMedium = pix;
		return m_pixScreenshotMedium;
	}
	// try to scale it from the large one (and save it by the way)
	pix = largeScreenshot();
	if(pix.isNull())
		return m_pixScreenshotMedium;
	if(pix.width() > 600 || pix.height() > 450)
	{
		QImage sbri = pix.toImage();
		pix.fromImage(sbri.scaled(640, 450, QIMAGE_SCALE_MIN, Qt::SmoothTransformation));
	}
	pix.save(szFileName, "PNG");

	m_pixScreenshotMedium = pix;
	return m_pixScreenshotMedium;
}

const QPixmap & KviThemeInfo::largeScreenshot()
{
	if(!m_pixScreenshotLarge.isNull())
		return m_pixScreenshotLarge;

	QString szFileName = m_szDirectory;
	szFileName.append(KVI_THEME_LARGE_SCREENSHOT_NAME);
	QPixmap pix(szFileName);
	if(pix.isNull())
		return m_pixScreenshotLarge;
	m_pixScreenshotLarge = pix;
	return m_pixScreenshotLarge;
}

namespace KviTheme
{
	bool saveScreenshots(KviThemeInfo & options, const QString & szOriginalScreenshotPath)
	{
		QImage pix(szOriginalScreenshotPath);
		if(pix.isNull())
		{
			options.setLastError(__tr2qs("Failed to load the specified screenshot image"));
			return false;
		}

		QImage out;

		QString szScreenshotPath = options.directory();

		if(szScreenshotPath.isEmpty())
		{
			options.setLastError(__tr2qs("Invalid option"));
			return false;
		}

		if(!pix.save(szScreenshotPath + KVI_THEME_LARGE_SCREENSHOT_NAME, "PNG"))
		{
			options.setLastError(__tr2qs("Failed to save the screenshot image"));
			return false;
		}

		if(pix.width() > 600 || pix.height() > 450)
			out = pix.scaled(640, 450, QIMAGE_SCALE_MIN, Qt::SmoothTransformation);
		else
			out = pix;

		if(!out.save(szScreenshotPath + KVI_THEME_MEDIUM_SCREENSHOT_NAME, "PNG"))
		{
			options.setLastError(__tr2qs("Failed to save the screenshot image"));
			return false;
		}

		if(pix.width() > 300 || pix.height() > 225)
			out = pix.scaled(300, 225, QIMAGE_SCALE_MIN, Qt::SmoothTransformation);
		else
			out = pix;

		if(!out.save(szScreenshotPath + KVI_THEME_SMALL_SCREENSHOT_NAME, "PNG"))
		{
			options.setLastError(__tr2qs("Failed to save the screenshot image"));
			return false;
		}

		return true;
	}

	void installedThemeDirectories(QStringList & slThemes, KviThemeInfo::Location eLocation)
	{
		QString szThemePath;
		switch(eLocation)
		{
			case KviThemeInfo::Builtin:
				g_pApp->getGlobalKvircDirectory(szThemePath, KviApplication::Themes);
				break;
			case KviThemeInfo::User:
				g_pApp->getLocalKvircDirectory(szThemePath, KviApplication::Themes);
				break;
			default:
				return;
				break;
		}

		QDir d(szThemePath);
		QStringList sl = d.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

		for(auto & it : sl)
			slThemes.append(it);
	}
};
