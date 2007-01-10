//=============================================================================
//
//   File : themefunctions.cpp
//   Created on Wed 03 Jan 2007 03:14:07 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2007 Szymon Stefanek <pragma at kvirc dot net>
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

#include "themefunctions.h"

#include "kvi_packagefile.h"
#include "kvi_locale.h"
#include "kvi_msgbox.h"
#include "kvi_app.h"
#include "kvi_htmldialog.h"
#include "kvi_iconmanager.h"
#include "kvi_miscutils.h"
#include "kvi_sourcesdate.h"

#include <qmime.h>

namespace KviThemeFunctions
{

	static bool notAValidThemePackage(QString &szError)
	{
		KviQString::sprintf(szError,__tr2qs_ctx("The selected file does not seem to be a valid KVIrc theme package","theme"));
		return false;
	}

	bool installThemePackage(const QString &szThemePackageFileName,QString &szError,QWidget * pDialogParent)
	{
		QDict<QString> * pInfoFields;
		QString * pValue;
		bool bInstall;
		QPixmap pix;
		QByteArray * pByteArray;
		KviHtmlDialogData hd;

		const char * check_fields[] = { "Name", "Version", "Author", "Description", "Date", "MinimumKVIrcVersion", "Application" };
	
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
		if(!pValue)return notAValidThemePackage(szError);
		if(!KviQString::equalCI(*pValue,"ThemePack"))return notAValidThemePackage(szError);
		pValue = pInfoFields->find("ThemePackVersion");
		if(!pValue)return notAValidThemePackage(szError);
		if(!KviQString::equalCI(*pValue,"1"))return notAValidThemePackage(szError);
		
		// make sure the default fields exist
		for(int i=0;i<6;i++)
		{
			pValue = pInfoFields->find(check_fields[i]);
			if(!pValue)return notAValidThemePackage(szError);
		}
	
		pValue = pInfoFields->find("ThemeCount");
		if(!pValue)return notAValidThemePackage(szError);
		bool bOk;
		int iThemeCount = pValue->toInt(&bOk);
		if(!bOk)return notAValidThemePackage(szError);
		if(iThemeCount < 1)return notAValidThemePackage(szError);
	
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

		///////////////////////////////////
		QString szPackageName;
		QString szPackageVersion;
		QString szPackageAuthor;
		QString szPackageDescription;
		QString szPackageDate;
		QString szPackageMinimumKVIrcVersion;
		QString szPackageApplication;

		QString szAuthor = __tr2qs_ctx("Author","theme");
		QString szCreatedAt = __tr2qs_ctx("Created at","theme");
		QString szCreatedOn = __tr2qs_ctx("Created with","theme");

		r.getStringInfoField("Name",szPackageName);
		r.getStringInfoField("Version",szPackageVersion);
		r.getStringInfoField("Author",szPackageAuthor);
		r.getStringInfoField("Description",szPackageDescription);
		r.getStringInfoField("MinimumKVIrcVersion",szPackageMinimumKVIrcVersion);
		r.getStringInfoField("Application",szPackageApplication);
		r.getStringInfoField("Date",szPackageDate);

		if(KviMiscUtils::compareVersions(KVI_VERSION "." KVI_SOURCES_DATE,szPackageMinimumKVIrcVersion) > 0)
		{
			KviQString::sprintf(szError,__tr2qs_ctx("The selected theme package requires at least KVIrc %Q","theme"),&szPackageMinimumKVIrcVersion);
			return false;
		}

		/*
		int iIdx = 0;
		for(KviThemeInfo * pInfo = m_pThemeInfoList->first();pInfo;pInfo = m_pThemeInfoList->next())
		{
			KviQString::sprintf(szTmp,"Theme%dName",iIdx);
			f.addInfoField(szTmp,pInfo->szName);
			KviQString::sprintf(szTmp,"Theme%dVersion",iIdx);
			f.addInfoField(szTmp,pInfo->szVersion);
			KviQString::sprintf(szTmp,"Theme%dDescription",iIdx);
			f.addInfoField(szTmp,pInfo->szDescription);
			KviQString::sprintf(szTmp,"Theme%dDate",iIdx);
			f.addInfoField(szTmp,pInfo->szDate);
			KviQString::sprintf(szTmp,"Theme%dSubdirectory",iIdx);
			f.addInfoField(szTmp,pInfo->szSubdirectory);
			KviQString::sprintf(szTmp,"Theme%dAuthor",iIdx);
			f.addInfoField(szTmp,pInfo->szAuthor);
			KviQString::sprintf(szTmp,"Theme%dKVIrcVersion",iIdx);
			f.addInfoField(szTmp,pInfo->szKvircVersion);
			if(!f.addDirectory(pInfo->szAbsoluteDirectory,pInfo->szSubdirectory))
			{
				szTmp = __tr2qs_ctx("Packaging failed","theme");
				szTmp += ": ";
				szTmp += f.lastError();
				QMessageBox::critical(this,__tr2qs_ctx("Export Theme - KVIrc","theme"),szTmp,
						QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
			}
			iIdx++;
		}
		*/

		KviQString::sprintf(hd.szHtmlText,
			"<html bgcolor=\"#ffffff\">" \
				"<body bgcolor=\"#ffffff\">" \
					"<br>" \
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
					"<br>" \
					"<p><center>" \
						"<a href=\"theme_dialog_details\">Show Details</a>" \
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
			&szPackageApplication
		);

		QString szDetails = "Doh.. da details";
		

		QMimeSourceFactory::defaultFactory()->setPixmap("theme_dialog_pack_image",pix);
		QMimeSourceFactory::defaultFactory()->setText("theme_dialog_details",szDetails);
		QMimeSourceFactory::defaultFactory()->setText("theme_dialog_main",hd.szHtmlText);
	
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
				return true;
			}
		}

		return true;
	}


};

