//=============================================================================
//
//   File : addonfunctions.cpp
//   Created on Fri 02 May 2008 17:36:07 by Elvio Basello
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2008 Elvio Basello <hellvis69 at netsons dot org>
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

#include "addonfunctions.h"

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
#include <q3mimefactory.h>
#define KviTalMimeSourceFactory Q3MimeSourceFactory

#define KVI_CURRENT_ADDONS_ENGINE_VERSION "1.0.0"

namespace KviAddonFunctions
{
	static bool notAValidAddonPackage(QString &szError)
	{
		KviQString::sprintf(szError,__tr2qs_ctx("The selected file does not seem to be a valid KVIrc addon package","addon"));
		return false;
	}

	bool installAddonPackage(const QString &szAddonPackageFileName,QString &szError,QWidget * pDialogParent)
	{
		KviPointerHashTable<QString,QString> * pInfoFields;
		QString * pValue;
		bool bInstall;
		QPixmap pix;
		QByteArray * pByteArray;
		KviHtmlDialogData hd;

		const char * check_fields[] = { "Name", "Version", "Author", "Description", "Date", "Application" };
	
		// check if it is a valid addon file
		KviPackageReader r;
		if(!r.readHeader(szAddonPackageFileName))
		{
			QString szErr = r.lastError();
			KviQString::sprintf(szError,__tr2qs_ctx("The selected file does not seem to be a valid KVIrc package: %Q","addon"),&szErr);
			return false;
		}
	
		pInfoFields = r.stringInfoFields();
	
		pValue = pInfoFields->find("PackageType");
		if(!pValue)return notAValidAddonPackage(szError);
		if(!KviQString::equalCI(*pValue,"AddonPack"))return notAValidAddonPackage(szError);
		pValue = pInfoFields->find("AddonPackVersion");
		if(!pValue)return notAValidAddonPackage(szError);
		if(!KviQString::equalCI(*pValue,"1"))return notAValidAddonPackage(szError);
		
		// make sure the default fields exist
		for(int i=0;i<6;i++)
		{
			pValue = pInfoFields->find(check_fields[i]);
			if(!pValue)return notAValidAddonPackage(szError);
		}
	
		pValue = pInfoFields->find("AddonCount");
		if(!pValue)return notAValidAddonPackage(szError);
		bool bOk;
		int iAddonCount = pValue->toInt(&bOk);
		if(!bOk)return notAValidAddonPackage(szError);
		if(iAddonCount < 1)return notAValidAddonPackage(szError);
	
		// ok.. it should be really valid at this point
		
		// load its picture
		pByteArray = r.binaryInfoFields()->find("Image");
		if(pByteArray)
			pix.loadFromData(*pByteArray,0,0);
		
		if(pix.isNull())
		{
			// load the default icon
			pix = *(g_pIconManager->getBigIcon(KVI_BIGICON_ADDONS));
		}

		QString szPackageName;
		QString szPackageVersion;
		QString szPackageAuthor;
		QString szPackageDescription;
		QString szPackageDate;
		QString szPackageAddonEngineVersion;
		QString szPackageApplication;

		QString szAuthor = __tr2qs_ctx("Author","addon");
		QString szCreatedAt = __tr2qs_ctx("Created at","addon");
		QString szCreatedOn = __tr2qs_ctx("Created with","Addon");

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
		int iValidAddonCount = iAddonCount;
		
		while(iIdx < iAddonCount)
		{
			bool bValid = true;
		
			QString szAddonName;
			QString szAddonVersion;
			QString szAddonDescription;
			QString szAddonDate;
			QString szAddonSubdirectory;
			QString szAddonAuthor;
			QString szAddonEngineVersion;
			QString szAddonApplication;

			KviQString::sprintf(szTmp,"Addon%dName",iIdx);
			r.getStringInfoField(szTmp,szAddonName);
			KviQString::sprintf(szTmp,"Addon%dVersion",iIdx);
			r.getStringInfoField(szTmp,szAddonVersion);
			KviQString::sprintf(szTmp,"Addon%dApplication",iIdx);
			r.getStringInfoField(szTmp,szAddonApplication);
			KviQString::sprintf(szTmp,"Addon%dDescription",iIdx);
			r.getStringInfoField(szTmp,szAddonDescription);
			KviQString::sprintf(szTmp,"Addon%dDate",iIdx);
			r.getStringInfoField(szTmp,szAddonDate);
			KviQString::sprintf(szTmp,"Addon%dSubdirectory",iIdx);
			r.getStringInfoField(szTmp,szAddonSubdirectory);
			KviQString::sprintf(szTmp,"Addon%dAuthor",iIdx);
			r.getStringInfoField(szTmp,szAddonAuthor);
			KviQString::sprintf(szTmp,"Addon%dAddonEngineVersion",iIdx);
			r.getStringInfoField(szTmp,szAddonEngineVersion);

			if(szAddonName.isEmpty() || szAddonVersion.isEmpty() || szAddonSubdirectory.isEmpty() || szAddonEngineVersion.isEmpty())
				bValid = false;
			if(KviMiscUtils::compareVersions(szAddonEngineVersion,KVI_CURRENT_ADDONS_ENGINE_VERSION) < 0)
				bValid = false;

			QString szDetailsBuffer;

			getAddonHtmlDescription(
				szDetailsBuffer,
				szAddonName,
				szAddonVersion,
				szAddonDescription,
				szAddonSubdirectory,
				szAddonApplication,
				szAddonAuthor,
				szAddonDate,
				szAddonEngineVersion,
				iIdx
			);

			if(iIdx > 0) szDetails += "<hr>";

			szDetails += szDetailsBuffer;

			if(!bValid)
			{
				szDetails += "<p><center><font color=\"#ff0000\"><b>";
				szDetails += __tr2qs_ctx("Warning: The addon might be incompatible with this version of KVIrc","addon");
				szDetails += "</b></font></center></p>";
				iValidAddonCount--;
			}

			iIdx++;
		}

		szDetails += "<br><p><center><a href=\"Addon_dialog_main\">";
		szDetails +=  __tr2qs_ctx("Go Back to Package Data","addon");
		szDetails += "</a></center></p>";
		szDetails += "</body></html>";

		if(iValidAddonCount < iAddonCount)
		{
			szWarnings += "<p><center><font color=\"#ff0000\"><b>";
			szWarnings += __tr2qs_ctx("Warning: Some of the Addons contained in this package might be either corrupted or incompatible with this version of KVIrc","addon");
			szWarnings += "</b></font></center></p>";
		}

		QString szShowDetails = __tr2qs_ctx("Show Details","addon");

		KviQString::sprintf(hd.szHtmlText,
			"<html bgcolor=\"#ffffff\">" \
				"<body bgcolor=\"#ffffff\">" \
					"<p><center>" \
						"<h2>%Q %Q</h2>" \
					"</center></p>" \
					"<p><center>" \
						"<img src=\"addon_dialog_pack_image\">" \
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
						"<a href=\"addon_dialog_details\">%Q</a>" \
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

		

		KviTalMimeSourceFactory::defaultFactory()->setPixmap("Addon_dialog_pack_image",pix);
		KviTalMimeSourceFactory::defaultFactory()->setText("addon_dialog_details",szDetails);
		KviTalMimeSourceFactory::defaultFactory()->setText("addon_dialog_main",hd.szHtmlText);
	
		QString beginCenter = "<center>";
		QString endCenter = "</center>";

		hd.szCaption = __tr2qs_ctx("Install Addon Pack - KVIrc","addon");
		hd.szUpperLabelText = beginCenter + __tr2qs_ctx("You're about to install the following addon package","addon") + endCenter;
		hd.szLowerLabelText = beginCenter + __tr2qs_ctx("Do you want to proceed with the installation ?","addon") + endCenter;
		hd.szButton1Text = __tr2qs_ctx("Do Not Install","addon");
		hd.szButton2Text = __tr2qs_ctx("Yes, Proceed","addon");
		hd.iDefaultButton = 2;
		hd.iCancelButton = 1;
		hd.pixIcon = *(g_pIconManager->getSmallIcon(KVI_SMALLICON_ADDONS));
		hd.iMinimumWidth = 350;
		hd.iMinimumHeight = 420;
		hd.iFlags = KviHtmlDialogData::ForceMinimumSize;

		bInstall = KviHtmlDialog::display(pDialogParent,&hd) == 2;
		
		if(bInstall)
		{
			QString szUnpackPath;
			g_pApp->getLocalKvircDirectory(szUnpackPath,KviApp::Themes);
			if(!r.unpack(szAddonPackageFileName,szUnpackPath))
			{
				QString szErr2 = r.lastError();
				KviQString::sprintf(szError,__tr2qs_ctx("Failed to unpack the selected file: %Q","addon"),&szErr2);
				return true;
			}
		}

		return true;
	}


	void getAddonHtmlDescription(
		QString &szBuffer,
		const QString &szAddonName,
		const QString &szAddonVersion,
		const QString &szAddonDescription,
		const QString &szAddonSubdirectory,
		const QString &szAddonApplication,
		const QString &szAddonAuthor,
		const QString &szAddonDate,
		const QString &szAddonAddonEngineVersion,
		int iUniqueIndexInDocument
	)
	{
		QString szAuthor = __tr2qs_ctx("Author","Addon");
		QString szCreatedAt = __tr2qs_ctx("Created at","addon");
		QString szCreatedOn = __tr2qs_ctx("Created with","addon");
		QString szAddonEngineVersion = __tr2qs_ctx("Addon Engine Version","addon");
		QString szSubdirectory = __tr2qs_ctx("Subdirectory","addon");

		KviQString::sprintf(
			szBuffer,
			"<p><center>" \
				"<h2>%Q %Q</h2>" \
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
					"%Q: %Q<br>" \
					"%Q: %Q<br>" \
				"</font>" \
			"</center></p>",
			&szAddonName,
			&szAddonVersion,
			&szAddonDescription,
			&szAuthor,
			&szAddonAuthor,
			&szCreatedAt,
			&szAddonDate,
			&szCreatedOn,
			&szAddonApplication,
			&szAddonAddonEngineVersion,
			&szAddonAddonEngineVersion,
			&szSubdirectory,
			&szAddonSubdirectory
		);
	}
};
