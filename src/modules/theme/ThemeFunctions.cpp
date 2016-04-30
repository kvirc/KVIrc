//=============================================================================
//
//   File : ThemeFunctions.cpp
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

#include "ThemeFunctions.h"

#include "KviPackageReader.h"
#include "KviPackageWriter.h"
#include "KviLocale.h"
#include "KviMessageBox.h"
#include "KviApplication.h"
#include "KviHtmlDialog.h"
#include "KviIconManager.h"
#include "KviMiscUtils.h"
#include "kvi_sourcesdate.h"
#include "KviTheme.h"
#include "KviMainWindow.h"

#include <QImage>
#include <QPixmap>
#include <QByteArray>
#include <QBuffer>
#include <QDateTime>
#include <QLocale>

//#include <qmime.h>

namespace ThemeFunctions
{

	static bool notAValidThemePackage(QString & szError)
	{
		szError = __tr2qs_ctx("The selected file doesn't seem to be a valid KVIrc theme package", "theme");
		return false;
	}

	bool installThemePackage(const QString & szThemePackageFileName, QString & szError, QWidget * pDialogParent)
	{
		KviPointerHashTable<QString, QString> * pInfoFields;
		QString * pValue;
		bool bInstall;
		QPixmap pix;
		QByteArray * pByteArray;
		KviHtmlDialogData hd;

		const char * check_fields[] = { "Name", "Version", "Author", "Description", "Date", "Application" };

		// check if it is a valid theme file
		KviPackageReader r;

		if(!r.readHeader(szThemePackageFileName))
		{
			qDebug("The selected file doesn't seem to be a valid KVIrc package");
			QString szErr = r.lastError();
			szError = QString(__tr2qs_ctx("The selected file doesn't seem to be a valid KVIrc package: %1", "theme")).arg(szErr);
			return false;
		}

		pInfoFields = r.stringInfoFields();

		pValue = pInfoFields->find("PackageType");
		if(!pValue)
			return notAValidThemePackage(szError);

		if(!KviQString::equalCI(*pValue, "ThemePack"))
			return notAValidThemePackage(szError);

		pValue = pInfoFields->find("ThemePackVersion");

		if(!pValue)
			return notAValidThemePackage(szError);

		// make sure the default fields exist
		for(auto & check_field : check_fields)
		{
			pValue = pInfoFields->find(check_field);
			if(!pValue)
				return notAValidThemePackage(szError);
		}

		pValue = pInfoFields->find("ThemeCount");
		if(!pValue)
			return notAValidThemePackage(szError);

		bool bOk;
		int iThemeCount = pValue->toInt(&bOk);
		if(!bOk)
			return notAValidThemePackage(szError);

		if(iThemeCount < 1)
			return notAValidThemePackage(szError);

		// ok.. it should be really valid at this point

		// load its picture
		pByteArray = r.binaryInfoFields()->find("Image");
		if(pByteArray)
			pix.loadFromData(*pByteArray, nullptr, nullptr);

		if(pix.isNull())
		{
			// load the default icon
			pix = *(g_pIconManager->getBigIcon(KVI_BIGICON_THEME));
		}

		QString szPackageName;
		QString szPackageVersion;
		QString szPackageAuthor;
		QString szPackageDescription;
		QString szPackageDate;
		QString szPackageThemeEngineVersion;
		QString szPackageApplication;

		QString szAuthor = __tr2qs_ctx("Author", "theme");
		QString szCreatedAt = __tr2qs_ctx("Created at", "theme");
		QString szCreatedOn = __tr2qs_ctx("Created with", "theme");

		r.getStringInfoField("Name", szPackageName);
		r.getStringInfoField("Version", szPackageVersion);
		r.getStringInfoField("Author", szPackageAuthor);
		r.getStringInfoField("Description", szPackageDescription);
		r.getStringInfoField("Application", szPackageApplication);
		r.getStringInfoField("Date", szPackageDate);

		QString szWarnings;
		QString szDetails = "<html><body bgcolor=\"#ffffff\">";
		QString szTmp;

		int iIdx = 0;
		int iValidThemeCount = iThemeCount;

		while(iIdx < iThemeCount)
		{
			bool bValid = true;

			QString szThemeName;
			QString szThemeVersion;
			QString szThemeDescription;
			QString szThemeDate;
			QString szThemeSubdirectory;
			QString szThemeAuthor;
			QString szThemeEngineVersion;
			QString szThemeApplication;

			szTmp = QString("Theme%1Name").arg(iIdx);
			r.getStringInfoField(szTmp, szThemeName);
			szTmp = QString("Theme%1Version").arg(iIdx);
			r.getStringInfoField(szTmp, szThemeVersion);
			szTmp = QString("Theme%1Application").arg(iIdx);
			r.getStringInfoField(szTmp, szThemeApplication);
			szTmp = QString("Theme%1Description").arg(iIdx);
			r.getStringInfoField(szTmp, szThemeDescription);
			szTmp = QString("Theme%1Date").arg(iIdx);
			r.getStringInfoField(szTmp, szThemeDate);
			szTmp = QString("Theme%1Subdirectory").arg(iIdx);
			r.getStringInfoField(szTmp, szThemeSubdirectory);
			szTmp = QString("Theme%1Author").arg(iIdx);
			r.getStringInfoField(szTmp, szThemeAuthor);
			szTmp = QString("Theme%1ThemeEngineVersion").arg(iIdx);
			r.getStringInfoField(szTmp, szThemeEngineVersion);
			szTmp = QString("Theme%1Screenshot").arg(iIdx);
			QPixmap pixScreenshot;
			pByteArray = r.binaryInfoFields()->find(szTmp);
			if(pByteArray)
				pixScreenshot.loadFromData(*pByteArray, nullptr, nullptr);

			if(szThemeName.isEmpty() || szThemeVersion.isEmpty() || szThemeSubdirectory.isEmpty() || szThemeEngineVersion.isEmpty())
				bValid = false;
			if(KviMiscUtils::compareVersions(szThemeEngineVersion, KVI_CURRENT_THEME_ENGINE_VERSION) < 0)
				bValid = false;

			QString szDetailsBuffer;

			getThemeHtmlDescription(
			    szDetailsBuffer,
			    szThemeName,
			    szThemeVersion,
			    szThemeDescription,
			    szThemeSubdirectory,
			    szThemeApplication,
			    szThemeAuthor,
			    szThemeDate,
			    szThemeEngineVersion,
			    pixScreenshot,
			    iIdx, &hd);

			if(iIdx > 0)
				szDetails += "<hr>";

			szDetails += szDetailsBuffer;

			if(!bValid)
			{
				szDetails += "<p><center><font color=\"#ff0000\"><b>";
				szDetails += __tr2qs_ctx("Warning: this theme might be incompatible with this version of KVIrc", "theme");
				szDetails += "</b></font></center></p>";
				iValidThemeCount--;
			}

			iIdx++;
		}

		szDetails += "<br><p><center><a href=\"theme_dialog_main\">";
		szDetails += __tr2qs_ctx("Go Back to Package Data", "theme");
		szDetails += "</a></center></p>";
		szDetails += "</body></html>";

		if(iValidThemeCount < iThemeCount)
		{
			szWarnings += "<p><center><font color=\"#ff0000\"><b>";
			szWarnings += __tr2qs_ctx("Warning: the theme contained in this package might be either corrupted or incompatible with this version of KVIrc", "theme");
			szWarnings += "</b></font></center></p>";
		}

		QString szShowDetails = __tr2qs_ctx("Show Details", "theme");

		// clang-format off
		hd.szHtmlText = QString(
			"<html bgcolor=\"#ffffff\">" \
				"<body bgcolor=\"#ffffff\">" \
					"<p><center>" \
						"<h2>%1 %2</h2>" \
					"</center></p>" \
					"<p><center>" \
						"<img src=\"theme_dialog_pack_image\">" \
					"</center></p>" \
					"<p><center>" \
						"<i>%3</i>" \
					"</center></p>" \
					"<p><center>" \
						"%4: <b>%5</b><br>" \
						"%6: <b>%7</b><br>" \
					"</center></p>" \
					"<p><center>" \
						"<font color=\"#808080\">" \
							"%8: %9<br>" \
						"</font>" \
					"</center></p>" \
					"%10" \
					"<br>" \
					"<p><center>" \
						"<a href=\"theme_dialog_details\">%11</a>" \
					"</center></p>" \
				"</body>" \
			"</html>").arg(szPackageName,szPackageVersion,szPackageDescription,szAuthor,szPackageAuthor,szCreatedAt,szPackageDate,szCreatedOn,szPackageApplication).arg(szWarnings,szShowDetails);
		// clang-format on

		hd.addImageResource("theme_dialog_pack_image", pix);
		hd.addHtmlResource("theme_dialog_details", szDetails);
		hd.addHtmlResource("theme_dialog_main", hd.szHtmlText);

		QString beginCenter = "<center>";
		QString endCenter = "</center>";

		hd.szCaption = __tr2qs_ctx("Install Theme Pack - KVIrc", "theme");
		hd.szUpperLabelText = beginCenter + __tr2qs_ctx("You're about to install the following theme package", "theme") + endCenter;
		hd.szLowerLabelText = beginCenter + __tr2qs_ctx("Do you want to proceed with the installation?", "theme") + endCenter;
		hd.szButton1Text = __tr2qs_ctx("Do Not Install", "theme");
		hd.szButton2Text = __tr2qs_ctx("Yes, Proceed", "theme");
		hd.iDefaultButton = 2;
		hd.iCancelButton = 1;
		hd.pixIcon = *(g_pIconManager->getSmallIcon(KviIconManager::Theme));
		hd.iMinimumWidth = 350;
		hd.iMinimumHeight = 420;
		hd.iFlags = KviHtmlDialogData::ForceMinimumSize;

		bInstall = KviHtmlDialog::display(pDialogParent, &hd) == 2;
		if(bInstall)
		{
			QString szUnpackPath;
			g_pApp->getLocalKvircDirectory(szUnpackPath, KviApplication::Themes);
			if(!r.unpack(szThemePackageFileName, szUnpackPath))
			{
				QString szErr2 = r.lastError();
				szError = QString(__tr2qs_ctx("Failed to unpack the selected file: %1", "theme")).arg(szErr2);
				return false;
			}
		}

		return true;
	}

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
	    int iUniqueIndexInDocument,
	    KviHtmlDialogData * hd)
	{
		QString szAuthor = __tr2qs_ctx("Author", "theme");
		QString szCreatedAt = __tr2qs_ctx("Created at", "theme");
		QString szCreatedOn = __tr2qs_ctx("Created with", "theme");
		QString szThemeEngineVersion = __tr2qs_ctx("Theme engine version", "theme");
		QString szSubdirectory = __tr2qs_ctx("Subdirectory", "theme");

		QString szScreenshot;
		if(!pixScreenshot.isNull())
		{
			szScreenshot = QString("<p><center><img src=\"theme_shot%1\"></center></p>").arg(iUniqueIndexInDocument);
			QString szTmp;
			szTmp = QString("theme_shot%1").arg(iUniqueIndexInDocument);
			//FIXME in tooltip
			if(hd)
				hd->addImageResource(szTmp, pixScreenshot);
			else
				szScreenshot = "";
		}
		else
		{
			szScreenshot = "";
		}

		// clang-format off
		szBuffer = QString(
			"<p><center>" \
				"<h2>%1 %2</h2>" \
			"</center></p>" \
			"%3" \
			"<p><center>" \
				"<i>%4</i>" \
			"</center></p>" \
			"<p><center>" \
				"%5: <b>%6</b><br>" \
				"%7: <b>%8</b><br>" \
			"</center></p>" \
			"<p><center>" \
				"<font color=\"#808080\">" \
					"%9: %10<br>" \
					"%11: %12<br>" \
					"%13: %14<br>" \
				"</font>" \
			"</center></p>").arg(szThemeName,szThemeVersion,szScreenshot,szThemeDescription,szAuthor,szThemeAuthor,szCreatedAt,szThemeDate,szCreatedOn).arg(szThemeApplication,szThemeEngineVersion,szThemeThemeEngineVersion,szSubdirectory,szThemeSubdirectory);
		// clang-format on
	}

	bool makeKVIrcScreenshot(const QString & szSavePngFilePath, bool bMaximizeFrame)
	{
		if(bMaximizeFrame)
		{
			if(g_pMainWindow->isMaximized())
				bMaximizeFrame = false;
		}

		if(bMaximizeFrame)
			g_pMainWindow->showMaximized();

		QPixmap pix = g_pMainWindow->grab();
		bool bResult = true;

		if(pix.isNull())
			bResult = false;
		else
		{
			if(!pix.save(szSavePngFilePath, "PNG"))
				bResult = false;
		}

		if(bMaximizeFrame)
			g_pMainWindow->showNormal();
		return bResult;
	}

	bool packageThemes(
	    const QString & szPackagePath,
	    const QString & szPackageName,
	    const QString & szPackageVersion,
	    const QString & szPackageDescription,
	    const QString & szPackageAuthor,
	    const QString & szPackageImagePath,
	    KviPointerList<KviThemeInfo> & lThemeInfoList,
	    QString & szError)
	{
		if(szPackagePath.isEmpty())
		{
			szError = __tr2qs_ctx("Invalid empty package path", "theme");
			return false;
		}

		if(szPackageName.isEmpty())
		{
			szError = __tr2qs_ctx("Invalid empty package name", "theme");
			return false;
		}

		QPixmap out;

		if(!szPackageImagePath.isEmpty())
		{
			QImage pix(szPackageImagePath);
			if(pix.isNull())
			{
				szError = __tr2qs_ctx("Failed to load the selected image: please fix it", "theme");
				return false;
			}

			if((pix.width() > 300) || (pix.height() > 225))
				out = out.fromImage(pix.scaled(300, 225, Qt::KeepAspectRatio));
			else
				out = out.fromImage(pix);
		}

		KviPackageWriter f;

		f.addInfoField("PackageType", "ThemePack");
		f.addInfoField("ThemePackVersion", KVI_CURRENT_THEME_ENGINE_VERSION);
		f.addInfoField("Name", szPackageName);
		f.addInfoField("Version", szPackageVersion.isEmpty() ? "1.0.0" : szPackageVersion);
		f.addInfoField("Author", szPackageAuthor);
		f.addInfoField("Description", szPackageDescription);
		// this is the equivalent to an empty date.toString() call, but it's needed
		// to ensure qt4 will use the default() locale and not the system() one
		f.addInfoField("Date", QDateTime::currentDateTime().toString(Qt::ISODate));
		f.addInfoField("Application", "KVIrc " KVI_VERSION "." KVI_SOURCES_DATE);

		if(!out.isNull())
		{
			QByteArray * pba = new QByteArray();
			QBuffer buffer(pba, nullptr);
			buffer.open(QIODevice::WriteOnly);
			out.save(&buffer, "PNG");
			buffer.close();
			f.addInfoField("Image", pba); // cool :) [no disk access needed]
		}

		QString szTmp;

		szTmp.setNum(lThemeInfoList.count());
		f.addInfoField("ThemeCount", szTmp);

		int iIdx = 0;
		for(KviThemeInfo * pInfo = lThemeInfoList.first(); pInfo; pInfo = lThemeInfoList.next())
		{
			if(pInfo->name().isEmpty())
			{
				szError = __tr2qs_ctx("Invalid theme name", "theme");
				return false;
			}
			if(pInfo->version().isEmpty())
			{
				szError = __tr2qs_ctx("Invalid theme version", "theme");
				return false;
			}

			QString szSubdir = pInfo->name() + QString("-") + pInfo->version();
			szSubdir.replace(QRegExp("[^a-zA-Z0-9_\\-.][^a-zA-Z0-9_\\-.]*"), "_");

			szTmp = QString("Theme%1Name").arg(iIdx);
			f.addInfoField(szTmp, pInfo->name());
			szTmp = QString("Theme%1Version").arg(iIdx);
			f.addInfoField(szTmp, pInfo->version());
			szTmp = QString("Theme%1Description").arg(iIdx);
			f.addInfoField(szTmp, pInfo->description());
			szTmp = QString("Theme%1Date").arg(iIdx);
			f.addInfoField(szTmp, pInfo->date());
			szTmp = QString("Theme%1Subdirectory").arg(iIdx);
			f.addInfoField(szTmp, szSubdir);
			szTmp = QString("Theme%1Author").arg(iIdx);
			f.addInfoField(szTmp, pInfo->author());
			szTmp = QString("Theme%1Application").arg(iIdx);
			f.addInfoField(szTmp, pInfo->application());
			szTmp = QString("Theme%1ThemeEngineVersion").arg(iIdx);
			f.addInfoField(szTmp, pInfo->themeEngineVersion());
			QPixmap pixScreenshot = pInfo->smallScreenshot();
			if(!pixScreenshot.isNull())
			{
				szTmp = QString("Theme%1Screenshot").arg(iIdx);
				QByteArray * pba = new QByteArray();

				QBuffer bufferz(pba, nullptr);
				bufferz.open(QIODevice::WriteOnly);
				pixScreenshot.save(&bufferz, "PNG");
				bufferz.close();
				f.addInfoField(szTmp, pba);
			}

			if(!f.addDirectory(pInfo->directory(), szSubdir))
			{
				szError = __tr2qs_ctx("Packaging failed", "theme");
				szError += ": ";
				szError += f.lastError();
				return false;
			}

			iIdx++;
		}

		if(!f.pack(szPackagePath))
		{
			szError = __tr2qs_ctx("Packaging failed", "theme");
			szError += ": ";
			szError += f.lastError();
			return false;
		}

		return true;
	}
}
