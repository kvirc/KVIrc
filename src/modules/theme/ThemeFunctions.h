#ifndef _THEMEFUNCTIONS_H_
#define _THEMEFUNCTIONS_H_
//=============================================================================
//
//   File : ThemeFunctions.h
//   Creation date : Wed 03 Jan 2007 03:14:07 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2007-2010 Szymon Stefanek <pragma at kvirc dot net>
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
#include "KviQString.h"
#include "KviHtmlDialog.h"
#include "KviPointerList.h"
#include "KviTheme.h"

#include <QWidget>
#include <QPixmap>

namespace ThemeFunctions
{
	bool installThemePackage(const QString & szThemePackageFileName, QString & szError, QWidget * pDialogParent = nullptr);

	void getThemeHtmlDescription(
	    QString & szBuffer,
	    const QString & szThemeName,
	    const QString & szThemeVersion,
	    const QString & szThemeDescription,
	    const QString & szThemeSubdirectory,
	    const QString & szThemeApplication,
	    const QString & szThemeAuthor,
	    const QString & szThemeDate,
	    const QString & szThemeThemeEngineVersion,
	    const QPixmap & pixScreenshot,
	    int iUniqueIndexInDocument = 0,
	    KviHtmlDialogData * hd = nullptr);

	bool makeKVIrcScreenshot(const QString & szSavePngFilePath, bool bMaximizeFrame = false);

	bool packageThemes(
	    const QString & szPackagePath,
	    const QString & szPackageName,
	    const QString & szPackageVersion,
	    const QString & szPackageDescription,
	    const QString & szPackageAuthor,
	    const QString & szPackageImagePath,
	    KviPointerList<KviThemeInfo> & lThemeInfo,
	    QString & szError);
}

#endif //!_THEMEFUNCTIONS_H_
