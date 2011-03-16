#ifndef _KVI_THEME_H_
#define _KVI_THEME_H_

//=============================================================================
//
//   File : KviTheme.h
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

#include "kvi_settings.h"
#include "KviQString.h"
#include "KviHeapObject.h"
#include "kvi_fileextensions.h"
#include "KviApplication.h"
#include "KviFileUtils.h"

#include <QPixmap>
#define BUILTIN 1
#define KVI_THEMEINFO_FILE_NAME "themeinfo" KVI_FILEEXTENSION_CONFIG
#define KVI_THEMEDATA_FILE_NAME "themedata" KVI_FILEEXTENSION_CONFIG
#define KVI_THEMEINFO_CONFIG_GROUP "ThemeInfo"
#define KVI_THEMEDATA_CONFIG_GROUP "ThemeData"

///
/// The current theme engine version: bump up if you make INCOMPATIBLE
/// changes in the method of saving the theme.
///
#define KVI_CURRENT_THEME_ENGINE_VERSION "1.0.0"

///
/// \class KviThemeInfo
///
/// \brief A set of information about a KVIrc theme
///
/// This object contains a set of information about
/// a theme that can be read and stored to a standard
/// KVIrc configuration file (which is usually named
/// themeinfo.kvc but this is not strictly necessary).
///
class KVIRC_API KviThemeInfo : public KviHeapObject
{
public:
	KviThemeInfo();
	~KviThemeInfo();
protected:
	QString m_szName;               //< name of the theme
	QString m_szVersion;            //< version of the theme
	QString m_szSubdirectory;       //< subdirectory of $LOCALKVIRCDIR/themes where the theme is saved to (not always used)
	QString m_szDirName;	        //< directory where the theme should be saved to (or is loaded from)
	QString m_szAuthor;             //< author of the theme
	QString m_szDescription;        //< description of the theme
	QString m_szDate;               //< theme creation date
	QString m_szApplication;        //< theme creation (KVIrc) version
	QString m_szThemeEngineVersion; //< the theme engine version that saved this theme

	QString m_szLastError;          //< reported when some function fails

	QPixmap m_pixScreenshotLarge;   //< the large screenshot pixmap
	QPixmap m_pixScreenshotMedium;  //< the medium screenshot pixmap
	QPixmap m_pixScreenshotSmall;   //< the small screenshot pixmap

	bool m_bBuiltin; //< bool is a builtin theme
	QString m_szThemeFileName;
public:
	///
	/// load data from a specified theme config file
	///
	bool load(const QString &szThemeFileName,bool bBuiltin=false);
	///
	/// save the currently defined theme configuration in the specified file
	///
	bool save(const QString &szThemeFileName);
	bool isBuiltin(){return m_bBuiltin;}
	const QString & lastError(){ return m_szLastError; }
	void setLastError(const QString &szLastError){ m_szLastError = szLastError; }
	const QString & name(){ return m_szName; }
	void setName(const QString &szName){ m_szName = szName; }
	const QString & version(){ return m_szVersion; }
	void setVersion(const QString &szVersion){ m_szVersion = szVersion; }
	const QString & subdirectory(){ return m_szSubdirectory; }
	void setSubdirectory(const QString &szSubdirectory){ m_szSubdirectory = szSubdirectory; }
	void getCompleteDirPath(QString &szBuffer)
	{
	    if(m_bBuiltin) g_pApp->getGlobalKvircDirectory(szBuffer,KviApplication::Themes,m_szThemeFileName);
	    else g_pApp->getLocalKvircDirectory(szBuffer,KviApplication::Themes,m_szThemeFileName);
	    KviQString::ensureLastCharIs(szBuffer,QChar(KVI_PATH_SEPARATOR_CHAR));
	}
	const QString & dirName(){ return m_szDirName; }
	const QString & author(){ return m_szAuthor; }
	void setAuthor(const QString &szAuthor){ m_szAuthor = szAuthor; }
	const QString & description(){ return m_szDescription; }
	void setDescription(const QString &szDescription){ m_szDescription = szDescription; }
	const QString & date(){ return m_szDate; }
	void setDate(const QString &szDate){ m_szDate = szDate; }
	const QString & application(){ return m_szApplication; }
	void setApplication(const QString &szApplication){ m_szApplication = szApplication; }
	const QString & themeEngineVersion(){ return m_szThemeEngineVersion; }
	void setThemeEngineVersion(const QString &szThemeEngineVersion){ m_szThemeEngineVersion = szThemeEngineVersion; }


	///
	/// Attempt to load the theme screenshot from THEMEDIR/screenshot_*.png
	/// This function will work only if the absoluteDirectory() of the theme
	/// has been set, otherwise the returned pixmap will be null.
	///
	const QPixmap & smallScreenshot();
	///
	/// Attempt to load the theme screenshot from THEMEDIR/screenshot_*.png
	/// This function will work only if the absoluteDirectory() of the theme
	/// has been set, otherwise the returned pixmap will be null.
	///
	const QPixmap & mediumScreenshot();
	///
	/// Attempt to load the theme screenshot from THEMEDIR/screenshot_*.png
	/// This function will work only if the absoluteDirectory() of the theme
	/// has been set, otherwise the returned pixmap will be null.
	///
	const QPixmap & largeScreenshot();
	///
	/// Attempt to load the theme screenshot from THEMEDIR/screenshot_*.png
	/// This function will work only if the absoluteDirectory() of the theme
	/// has been set, otherwise the returned pixmap will be null.
	///
	QString smallScreenshotPath();
};

namespace KviTheme
{
	///
	/// Attempt to load (apply) a theme from globaldir::theme (bBuiltin=true) or from localdir::theme.
	/// Will return true on success and false on failure.
	/// On success this function will return the theme information in the buffer.
	/// On failure this function will also set buffer.lastError() to a meaningful value
	/// Note that for convenience this function is implemented in KviOptions.cpp
	///
    	bool KVIRC_API load(const QString &szThemeName,KviThemeInfo &buffer,bool bBuiltin=false);
	///
	/// Save a theme given the specified options.
	/// Will return true on success and false on failure.
	/// On failure this function will also set options.lastError() to a meaningful value
	/// This function requires both absoluteDirectory() and subdirectory() to be
	/// set to the right values. In theory this function could save a theme
	/// in a directory different than $KVIRC_LOCAL_DIR/themes/subdirectory
	/// but this feature is actually unused.
	/// Note that for convenience this function is implemented in KviOptions.cpp
	///
	bool KVIRC_API save(KviThemeInfo &options);
	///
	/// Save the theme screenshots in the given EXISTING directory and given
	/// an existing screenshot on disk (usually in the tmp directory).
	///
	bool KVIRC_API saveScreenshots(KviThemeInfo &options,const QString &szOriginalScreenshotPath);

	void KVIRC_API installedThemes(QStringList &slThemes,bool bBuiltin);
}


#endif //_KVI_THEME_H_
