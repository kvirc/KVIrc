//=============================================================================
//
//   File : addonfunctions.cpp
//   Creation date : Fri 02 May 2008 17:36:07 by Elvio Basello
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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include "addonfunctions.h"

#include "kvi_packagefile.h"
#include "kvi_locale.h"
#include "kvi_msgbox.h"
#include "kvi_app.h"
#include "kvi_htmldialog.h"
#include "kvi_iconmanager.h"
#include "kvi_fileutils.h"
#include "kvi_miscutils.h"
#include "kvi_sourcesdate.h"
#include "kvi_frame.h"
#include "kvi_kvs_script.h"

#include <QDir>
#include <QMessageBox>

#include <stdlib.h>

namespace KviAddonFunctions
{
	bool notAValidAddonPackage(QString &szError)
	{
		szError = __tr2qs_ctx("The selected file does not seem to be a valid KVIrc addon package","addon");
		return false;
	}

	bool installAddonPackage(const QString &szAddonPackageFileName,QString &szError,QWidget * pDialogParent)
	{
		KviPointerHashTable<QString,QString> * pInfoFields;
		QString * pValue;
		QString szErr;
		QPixmap pix;
		QByteArray * pByteArray;
		bool bInstall;
		KviHtmlDialogData hd;

		const char * check_fields[] = { "Name", "Version", "Author", "Description", "Date", "Application" };

		// check if it is a valid addon file
		KviPackageReader r;
		if(!r.readHeader(szAddonPackageFileName))
		{
			szErr = r.lastError();
			KviQString::sprintf(szError,__tr2qs_ctx("The selected file does not seem to be a valid KVIrc package: %Q","addon"),&szErr);
			return false;
		}

		pInfoFields = r.stringInfoFields();

		pValue = pInfoFields->find("PackageType");

		if(!pValue)
			return notAValidAddonPackage(szError);

		if(!KviQString::equalCI(*pValue,"AddonPack"))
			return notAValidAddonPackage(szError);

		pValue = pInfoFields->find("AddonPackVersion");

		if(!pValue)
			return notAValidAddonPackage(szError);

		// make sure the default fields exist
		for(int i=0;i<6;i++)
		{
			pValue = pInfoFields->find(check_fields[i]);
			if(!pValue)
				return notAValidAddonPackage(szError);
		}

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
		QString szAddonPackVersion;
		QString szPackageApplication;

		QString szAuthor = __tr2qs_ctx("Author","addon");
		QString szCreatedAt = __tr2qs_ctx("Created at","addon");
		QString szCreatedWith = __tr2qs_ctx("Created with","addon");

		r.getStringInfoField("Name",szPackageName);
		r.getStringInfoField("Version",szPackageVersion);
		r.getStringInfoField("Author",szPackageAuthor);
		r.getStringInfoField("Description",szPackageDescription);
		r.getStringInfoField("Application",szPackageApplication);
		r.getStringInfoField("Date",szPackageDate);
		r.getStringInfoField("AddonPackVersion",szAddonPackVersion);

		QString szWarnings;
		QString szTmp;

		bool bValid = true;

		if(szPackageName.isEmpty() || szPackageVersion.isEmpty() || szAddonPackVersion.isEmpty())
			bValid = false;

		if(KviMiscUtils::compareVersions(szAddonPackVersion,KVI_CURRENT_ADDONS_ENGINE_VERSION) < 0)
			bValid = false;


		if(!bValid)
		{
			szWarnings += "<p><center><font color=\"#ff0000\"><b>";
			szWarnings += __tr2qs_ctx("Warning: The addon might be incompatible with this version of KVIrc","addon");
			szWarnings += "</b></font></center></p>";
		}

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
				"</body>" \
			"</html>",
			&szPackageName,
			&szPackageVersion,
			&szPackageDescription,
			&szAuthor,
			&szPackageAuthor,
			&szCreatedAt,
			&szPackageDate,
			&szCreatedWith,
			&szPackageApplication,
			&szWarnings
		);

		hd.addImageResource("addon_dialog_pack_image",pix);
		hd.addHtmlResource("addon_dialog_main",hd.szHtmlText);

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

		if(!bInstall)
			return true;

		// Create a random extraction dir
		QString szTmpPath, szUnpackPath;
		QString szRandomDir = createRandomDir();

		g_pApp->getLocalKvircDirectory(szTmpPath,KviApp::Tmp);
		KviQString::ensureLastCharIs(szTmpPath,QChar(KVI_PATH_SEPARATOR_CHAR));
		szUnpackPath = szTmpPath + szRandomDir;
		QDir szTmpDir(szUnpackPath);

		// Check for dir existence
		while(szTmpDir.exists())
		{
			szRandomDir = createRandomDir();
			szUnpackPath = szTmpPath + szRandomDir;
			szTmpDir = QDir(szUnpackPath);
		}

		// Unpack addon package into the random tmp dir
		if(!r.unpack(szAddonPackageFileName,szUnpackPath))
		{
			szErr = r.lastError();
			KviQString::sprintf(szError,__tr2qs_ctx("Failed to unpack the selected file: %Q","addon"),&szErr);
			return false;
		}

		// Now we have all stuff in ~/.config/KVIrc/tmp/$rand
		if(!
				KviKvsScript::run(
						QString::fromAscii("parse \"%1/install.kvs\"")
								.arg(
										szUnpackPath
											.replace("\\","\\\\")
											.replace("\"","\\\"")
									),
						g_pActiveWindow
					)
				)
		{
			// Parsing the script failed
			// However, the user should already be notified via normal script output.
		}

		if(!KviFileUtils::deleteDir(szUnpackPath))
			QMessageBox::warning(NULL,__tr2qs_ctx("One or more files can't be deleted","addon"),__tr2qs_ctx("Failed to delete the directory '%1'","addon").arg(szUnpackPath));

		return true;
	}

	QString createRandomDir()
	{
		QString szDirName;
		char chars[] = {
			'A','B','C','D','E','F','G','H',
			'I','J','K','L','M','N','O','P',
			'Q','R','S','T','U','V','W','X',
			'Y','Z','a','b','c','d','e','f',
			'g','h','i','j','k','l','m','n',
			'o','p','q','r','s','t','u','v',
			'w','x','y','z','-','_','.'
		};

		// Generate dir name
		for(int i=0;i<10;i++)
		{
			int n = rand() % sizeof(chars);
			szDirName.append(chars[n]);
		}

		return szDirName;
	}
}
