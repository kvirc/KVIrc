//=============================================================================
//
//   File : themefunctions.cpp
//   Creation date : Wed 03 Jan 2007 03:14:07 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2007-2008 Szymon Stefanek <pragma at kvirc dot net>
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

#include "themefunctions.h"

#include "kvi_packagefile.h"
#include "kvi_locale.h"
#include "kvi_msgbox.h"
#include "kvi_app.h"
#include "kvi_htmldialog.h"
#include "kvi_iconmanager.h"
#include "kvi_miscutils.h"
#include "kvi_sourcesdate.h"
#include "kvi_theme.h"
#include "kvi_frame.h"


//#include <qmime.h>


namespace KviThemeFunctions
{

	static bool notAValidThemePackage(QString &szError)
	{
		szError = __tr2qs_ctx("The selected file does not seem to be a valid KVIrc theme package","theme");
		return false;
	}

	bool installThemePackage(const QString &szThemePackageFileName,QString &szError,QWidget * pDialogParent)
	{
		KviPointerHashTable<QString,QString> * pInfoFields;
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
			QString szErr = r.lastError();
			KviQString::sprintf(szError,__tr2qs_ctx("The selected file does not seem to be a valid KVIrc package: %Q","theme"),&szErr);
			return false;
		}

		pInfoFields = r.stringInfoFields();

		pValue = pInfoFields->find("PackageType");
		if(!pValue)
			return notAValidThemePackage(szError);

		if(!KviQString::equalCI(*pValue,"ThemePack"))
			return notAValidThemePackage(szError);

		pValue = pInfoFields->find("ThemePackVersion");

		if(!pValue)
			return notAValidThemePackage(szError);

		// make sure the default fields exist
		for(int i=0;i<6;i++)
		{
			pValue = pInfoFields->find(check_fields[i]);
			if(!pValue)return notAValidThemePackage(szError);
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
			pix.loadFromData(*pByteArray,0,0);

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

		QString szAuthor = __tr2qs_ctx("Author","theme");
		QString szCreatedAt = __tr2qs_ctx("Created at","theme");
		QString szCreatedOn = __tr2qs_ctx("Created with","theme");

		r.getStringInfoField("Name",szPackageName);
		r.getStringInfoField("Version",szPackageVersion);
		r.getStringInfoField("Author",szPackageAuthor);
		r.getStringInfoField("Description",szPackageDescription);
		r.getStringInfoField("Application",szPackageApplication);
		r.getStringInfoField("Date",szPackageDate);

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

			KviQString::sprintf(szTmp,"Theme%dName",iIdx);
			r.getStringInfoField(szTmp,szThemeName);
			KviQString::sprintf(szTmp,"Theme%dVersion",iIdx);
			r.getStringInfoField(szTmp,szThemeVersion);
			KviQString::sprintf(szTmp,"Theme%dApplication",iIdx);
			r.getStringInfoField(szTmp,szThemeApplication);
			KviQString::sprintf(szTmp,"Theme%dDescription",iIdx);
			r.getStringInfoField(szTmp,szThemeDescription);
			KviQString::sprintf(szTmp,"Theme%dDate",iIdx);
			r.getStringInfoField(szTmp,szThemeDate);
			KviQString::sprintf(szTmp,"Theme%dSubdirectory",iIdx);
			r.getStringInfoField(szTmp,szThemeSubdirectory);
			KviQString::sprintf(szTmp,"Theme%dAuthor",iIdx);
			r.getStringInfoField(szTmp,szThemeAuthor);
			KviQString::sprintf(szTmp,"Theme%dThemeEngineVersion",iIdx);
			r.getStringInfoField(szTmp,szThemeEngineVersion);
			KviQString::sprintf(szTmp,"Theme%dScreenshot",iIdx);
			QPixmap pixScreenshot;
			pByteArray = r.binaryInfoFields()->find(szTmp);
			if(pByteArray)
				pixScreenshot.loadFromData(*pByteArray,0,0);

			if(szThemeName.isEmpty() || szThemeVersion.isEmpty() || szThemeSubdirectory.isEmpty() || szThemeEngineVersion.isEmpty())
				bValid = false;
			if(KviMiscUtils::compareVersions(szThemeEngineVersion,KVI_CURRENT_THEME_ENGINE_VERSION) < 0)
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
				iIdx,&hd
			);

			if(iIdx > 0)
				szDetails += "<hr>";

			szDetails += szDetailsBuffer;

			if(!bValid)
			{
				szDetails += "<p><center><font color=\"#ff0000\"><b>";
				szDetails += __tr2qs_ctx("Warning: The theme might be incompatible with this version of KVIrc","theme");
				szDetails += "</b></font></center></p>";
				iValidThemeCount--;
			}

			iIdx++;
		}

		szDetails += "<br><p><center><a href=\"theme_dialog_main\">";
		szDetails +=  __tr2qs_ctx("Go Back to Package Data","theme");
		szDetails += "</a></center></p>";
		szDetails += "</body></html>";

		if(iValidThemeCount < iThemeCount)
		{
			szWarnings += "<p><center><font color=\"#ff0000\"><b>";
			szWarnings += __tr2qs_ctx("Warning: Some of the theme contained in this package might be either corrupted or incompatible with this version of KVIrc","theme");
			szWarnings += "</b></font></center></p>";
		}

		QString szShowDetails = __tr2qs_ctx("Show Details","theme");

		KviQString::sprintf(hd.szHtmlText,
			"<html bgcolor=\"#ffffff\">" \
				"<body bgcolor=\"#ffffff\">" \
					"<p><center>" \
						"<h2>%Q %Q</h2>" \
					"</center></p>" \
					"<p><center>" \
						"<img src=\"theme_dialog_pack_image\">" \
					"</center></p>" \
					"<p><center>" \
						"<i>%Q</i>" \
					"</center></p>" \
					"<p><center>" \
						"%Q: <b>%Q</b><br>" \
						"%Q: <b>%Q</b><br>" \
					"</center></p>" \
					"<p><center>" \
						"<font color=\"#808080\">" \
							"%Q: %Q<br>" \
						"</font>" \
					"</center></p>" \
					"%Q" \
					"<br>" \
					"<p><center>" \
						"<a href=\"theme_dialog_details\">%Q</a>" \
					"</center></p>" \
				"</body>" \
			"</html>",
			&szPackageName,
			&szPackageVersion,
			&szPackageDescription,
			&szAuthor,
			&szPackageAuthor,
			&szCreatedAt,
			&szPackageDate,
			&szCreatedOn,
			&szPackageApplication,
			&szWarnings,
			&szShowDetails
		);

		hd.addImageResource("theme_dialog_pack_image",pix);
		hd.addHtmlResource("theme_dialog_details",szDetails);
		hd.addHtmlResource("theme_dialog_main",hd.szHtmlText);

		QString beginCenter = "<center>";
		QString endCenter = "</center>";

		hd.szCaption = __tr2qs_ctx("Install Theme Pack - KVIrc","theme");
		hd.szUpperLabelText = beginCenter + __tr2qs_ctx("You're about to install the following theme package","theme") + endCenter;
		hd.szLowerLabelText = beginCenter + __tr2qs_ctx("Do you want to proceed with the installation ?","theme") + endCenter;
		hd.szButton1Text = __tr2qs_ctx("Do Not Install","theme");
		hd.szButton2Text = __tr2qs_ctx("Yes, Proceed","theme");
		hd.iDefaultButton = 2;
		hd.iCancelButton = 1;
		hd.pixIcon = *(g_pIconManager->getSmallIcon(KVI_SMALLICON_THEME));
		hd.iMinimumWidth = 350;
		hd.iMinimumHeight = 420;
		hd.iFlags = KviHtmlDialogData::ForceMinimumSize;

		bInstall = KviHtmlDialog::display(pDialogParent,&hd) == 2;
		if(bInstall)
		{
			QString szUnpackPath;
			g_pApp->getLocalKvircDirectory(szUnpackPath,KviApp::Themes);
			if(!r.unpack(szThemePackageFileName,szUnpackPath))
			{
				QString szErr2 = r.lastError();
				KviQString::sprintf(szError,__tr2qs_ctx("Failed to unpack the selected file: %Q","theme"),&szErr2);
				return false;
			}
		}

		return true;
	}


	void getThemeHtmlDescription(
		QString &szBuffer,
		const QString &szThemeName,
		const QString &szThemeVersion,
		const QString &szThemeDescription,
		const QString &szThemeSubdirectory,
		const QString &szThemeApplication,
		const QString &szThemeAuthor,
		const QString &szThemeDate,
		const QString &szThemeThemeEngineVersion,
		const QPixmap &pixScreenshot,
		int iUniqueIndexInDocument,
		KviHtmlDialogData *hd
	)
	{
		QString szAuthor = __tr2qs_ctx("Author","theme");
		QString szCreatedAt = __tr2qs_ctx("Created at","theme");
		QString szCreatedOn = __tr2qs_ctx("Created with","theme");
		QString szThemeEngineVersion = __tr2qs_ctx("Theme Engine Version","theme");
		QString szSubdirectory = __tr2qs_ctx("Subdirectory","theme");

		QString szScreenshot;
		if(!pixScreenshot.isNull())
		{
			KviQString::sprintf(szScreenshot,"<p><center><img src=\"theme_shot%d\"></center></p>",iUniqueIndexInDocument);
			QString szTmp;
			KviQString::sprintf(szTmp,"theme_shot%d",iUniqueIndexInDocument);
			//FIXME in tooltip
			if (hd)
				hd->addImageResource(szTmp,pixScreenshot);
			else szScreenshot = "";
		} else {
			szScreenshot = "";
		}

		KviQString::sprintf(
			szBuffer,
			"<p><center>" \
				"<h2>%Q %Q</h2>" \
			"</center></p>" \
			"%Q" \
			"<p><center>" \
				"<i>%Q</i>" \
			"</center></p>" \
			"<p><center>" \
				"%Q: <b>%Q</b><br>" \
				"%Q: <b>%Q</b><br>" \
			"</center></p>" \
			"<p><center>" \
				"<font color=\"#808080\">" \
					"%Q: %Q<br>" \
					"%Q: %Q<br>" \
					"%Q: %Q<br>" \
				"</font>" \
			"</center></p>",
			&szThemeName,
			&szThemeVersion,
			&szScreenshot,
			&szThemeDescription,
			&szAuthor,
			&szThemeAuthor,
			&szCreatedAt,
			&szThemeDate,
			&szCreatedOn,
			&szThemeApplication,
			&szThemeEngineVersion,
			&szThemeThemeEngineVersion,
			&szSubdirectory,
			&szThemeSubdirectory
		);
	}

	bool makeKVIrcScreenshot(const QString &szSavePngFilePath,bool bMaximizeFrame)
	{
		if(bMaximizeFrame)
		{
			if(g_pFrame->isMaximized())
				bMaximizeFrame = false;
		}

		if(bMaximizeFrame)
			g_pFrame->showMaximized();

		QPixmap pix = QPixmap::grabWidget(g_pFrame);
		bool bResult = true;

		if(pix.isNull())
			bResult = false;
		else {
			if(!pix.save(szSavePngFilePath,"PNG"))
				bResult = false;
		}

		if(bMaximizeFrame)
			g_pFrame->showNormal();
		return bResult;
	}
}

