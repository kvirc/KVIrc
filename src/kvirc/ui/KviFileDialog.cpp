//=============================================================================
//
//   File : KviFileDialog.cpp
//   Creation date : Mon Nov 20 2000 12:20:42 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviFileDialog.h"
#include "KviMediaManager.h"
#include "KviIconManager.h"
#include "KviLocale.h"
#include "KviApplication.h"
#include "KviFileUtils.h"
#include "KviMainWindow.h"
#include "KviTalToolTip.h"

#include <QDir>
#include <QIcon>
#include <QMessageBox>
#include <QFileDialog>

extern KviMediaManager * g_pMediaManager;

KviFileDialog::KviFileDialog(const QString & szDirName, const QString & szFilter, QWidget * pParent, const char * name, bool bModal)
    : KviTalFileDialog(szDirName, szFilter, pParent, name, bModal)
{
	setWindowIcon(QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Folder))));
}

KviFileDialog::~KviFileDialog()
    = default;

/*
void KviFileDialog::goKvirc()
{
	KviCString tmp;
	g_pApp->getLocalKvircDirectory(tmp,KviApplication::None);
	setDir(QDir(tmp.ptr()));
}

void KviFileDialog::goHome()
{
	setDir(QDir::home());
}
*/

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
bool KviFileDialog::askForOpenFileName(QString & szBuffer, const QString & szCaption, const QString & szInitial, const QString & szFilter, bool, bool bShowNative, QWidget * pParent)
{
	if(bShowNative)
	{
		szBuffer = QFileDialog::getOpenFileName(pParent, szCaption, szInitial, szFilter);
		KviFileUtils::adjustFilePath(szBuffer);
		return !szBuffer.isEmpty();
	}
#else
bool KviFileDialog::askForOpenFileName(QString & szBuffer, const QString & szCaption, const QString & szInitial, const QString & szFilter, bool, bool, QWidget * pParent)
{
#endif
	KviFileDialog * pDialog = new KviFileDialog(szInitial, szFilter, pParent, "open_file_name_dialog", true);
	pDialog->setWindowTitle(szCaption);
	// i was ignorant
	pDialog->setFileMode(KviTalFileDialog::ExistingFile);
	//pDialog->setShowHiddenFiles(showHidden);

	if(pDialog->exec() == QDialog::Accepted)
	{
		QStringList szFiles = pDialog->selectedFiles();
		if(!szFiles.isEmpty())
			szBuffer = szFiles[0];
		KviFileUtils::adjustFilePath(szBuffer);
		delete pDialog;
		return !szBuffer.isEmpty();
	}
	delete pDialog;
	return false;
}

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
bool KviFileDialog::askForSaveFileName(QString & szBuffer, const QString & szCaption, const QString & szInitial, const QString & szFilter, bool, bool bConfirmOverwrite, bool bShowNative, QWidget * pParent)
{
	if(bShowNative)
	{
		while(1)
		{
			szBuffer = QFileDialog::getSaveFileName(pParent, szCaption, szInitial, szFilter, 0, QFileDialog::DontConfirmOverwrite);
			KviFileUtils::adjustFilePath(szBuffer);
			//return !buffer.isEmpty();
			if(szBuffer.isEmpty())
				return false;

			if(!bConfirmOverwrite)
				return true;

			// Check for the file existence
			if(!KviFileUtils::fileExists(szBuffer))
				return true;

			QString szTmp = QString(__tr2qs("The file %1 already exists.<br>Do you wish to overwrite it?")).arg(szBuffer);

			switch(QMessageBox::information(pParent, __tr2qs("File Already Exists - KVIrc"), szTmp, QMessageBox::Yes, QMessageBox::No | QMessageBox::Default, QMessageBox::Cancel | QMessageBox::Escape))
			{
				case QMessageBox::Cancel:
					return false;
					break;
				case QMessageBox::Yes:
					return true;
					break;
			}
		}
	}
#else
bool KviFileDialog::askForSaveFileName(QString & szBuffer, const QString & szCaption, const QString & szInitial, const QString & szFilter, bool, bool bConfirmOverwrite, bool, QWidget * pParent)
{
#endif

	KviFileDialog * pDialog = new KviFileDialog(szInitial, szFilter, pParent, "save_file_name_dialog", true);
	pDialog->setWindowTitle(szCaption);
	// 190
	pDialog->setFileMode(KviTalFileDialog::AnyFile);
//pDialog->setShowHiddenFiles(showHidden);
#ifdef COMPILE_KDE4_SUPPORT
	pDialog->setKeepLocation(true);
#endif
	while(pDialog->exec() == QDialog::Accepted)
	{
		QStringList szFiles = pDialog->selectedFiles();
		if(!szFiles.isEmpty())
			szBuffer = szFiles[0];
		KviFileUtils::adjustFilePath(szBuffer);

		if(!szBuffer.isEmpty())
		{
			if(!bConfirmOverwrite)
			{
				delete pDialog;
				return true;
			}

			// Check for the file existence
			if(KviFileUtils::fileExists(szBuffer))
			{
				QString szTmp = QString(__tr2qs("The file %1 already exists.<br>Do you wish to overwrite it?")).arg(szBuffer);

				switch(QMessageBox::information(pDialog, __tr2qs("File Already Exists - KVIrc"), szTmp, QMessageBox::Yes, QMessageBox::No | QMessageBox::Default, QMessageBox::Cancel | QMessageBox::Escape))
				{
					case QMessageBox::Cancel:
						delete pDialog;
						return false;
						break;
					case QMessageBox::Yes:
						delete pDialog;
						return true;
						break;
				}
			}
			else
			{
				delete pDialog;
				return true; // ok...file not exists
			}
		}
		else
		{
			delete pDialog;
			return false; // empty buffer
		}
	}

	delete pDialog;
	return false;
}

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
bool KviFileDialog::askForDirectoryName(QString & szBuffer, const QString & szCaption, const QString & szInitial, const QString & szFilter, bool, bool bShowNative, QWidget * pParent)
{
	if(bShowNative)
	{
		szBuffer = QFileDialog::getExistingDirectory(pParent, szCaption, szInitial);
		KviFileUtils::adjustFilePath(szBuffer);
		return !szBuffer.isEmpty();
	}
#else
bool KviFileDialog::askForDirectoryName(QString & szBuffer, const QString & szCaption, const QString & szInitial, const QString & szFilter, bool, bool, QWidget * pParent)
{
#ifdef COMPILE_KDE4_SUPPORT
	// the KDE based dir selection dialog is now quite nice
	szBuffer = KFileDialog::getExistingDirectory(szInitial, pParent, szCaption);
	return !szBuffer.isEmpty();
#endif
#endif

	KviFileDialog * pDialog = new KviFileDialog(szInitial,
	    szFilter, pParent, "directory_name_dialog", true);
	pDialog->setWindowTitle(szCaption);
	// Move to tal and settle matters there?
	pDialog->setFileMode(KviTalFileDialog::Directory);
	//pDialog->setShowHiddenFiles(showHidden);

	if(pDialog->exec() == QDialog::Accepted)
	{
		QStringList szFiles = pDialog->selectedFiles();
		if(!szFiles.isEmpty())
			szBuffer = szFiles[0];
		KviFileUtils::adjustFilePath(szBuffer);
		delete pDialog;
		return !szBuffer.isEmpty();
	}
	delete pDialog;

	return false;
}

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
bool KviFileDialog::askForOpenFileNames(QStringList & szBuffer, const QString & szCaption, const QString & szInitial, const QString & szFilter, bool, bool bShowNative, QWidget * pParent)
{
	if(bShowNative)
	{
		szBuffer = QFileDialog::getOpenFileNames(pParent, szCaption, szInitial, szFilter);
		return (szBuffer.count() > 0);
	}
#else
bool KviFileDialog::askForOpenFileNames(QStringList & szBuffer, const QString & szCaption, const QString & szInitial, const QString & szFilter, bool, bool, QWidget * pParent)
{
#endif

	KviFileDialog * pDialog = new KviFileDialog(szInitial, szFilter, pParent, "open_file_names_dialog", true);
	pDialog->setWindowTitle(szCaption);
	// See line 190
	pDialog->setFileMode(KviTalFileDialog::ExistingFiles);
	//pDialog->setShowHiddenFiles(showHidden);

	if(pDialog->exec() == QDialog::Accepted)
	{
		szBuffer = pDialog->selectedFiles();
		delete pDialog;
		return (szBuffer.count() > 0);
	}
	delete pDialog;
	return false;
}
