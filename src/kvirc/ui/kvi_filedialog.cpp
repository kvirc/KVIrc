//
//   File : kvi_filedialog.cpp
//   Creation date : Mon Nov 20 2000 12:20:42 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2001 Szymon Stefanek (pragma at kvirc dot net)
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
#define __KVIRC__
#include "kvi_filedialog.h"
#include "kvi_mediatype.h"
#include "kvi_iconmanager.h"
#include "kvi_locale.h"
#include "kvi_app.h"
#include "kvi_fileutils.h"
#include "kvi_frame.h"
#include "kvi_qstring.h"

#include <qdir.h>
#include "kvi_tal_tooltip.h"
#include <qmessagebox.h>

extern KviMediaManager * g_pMediaManager;


KviFileDialog::KviFileDialog(const QString &dirName, const QString &filter,
					QWidget *parent, const char *name, bool modal)
: KviTalFileDialog(dirName,filter,parent,name,modal)
{

	setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_FOLDER)));

}

KviFileDialog::~KviFileDialog()
{
}


/*
void KviFileDialog::goKvirc()
{

	KviStr tmp;
	g_pApp->getLocalKvircDirectory(tmp,KviApp::None);
	setDir(QDir(tmp.ptr()));

}

void KviFileDialog::goHome()
{

	setDir(QDir::home());

}
*/



bool KviFileDialog::askForOpenFileName(QString &buffer,const QString &caption,const QString &initial,const QString &filter,bool showHidden, bool showNative,QWidget* parent)
{
#ifdef COMPILE_ON_WINDOWS
	if(showNative)
	{
		buffer=QFileDialog::getOpenFileName(initial,filter,parent,"open_file_name_dialog",caption);
		KviFileUtils::adjustFilePath(buffer);
		return !buffer.isEmpty();
	}
#endif
	KviFileDialog * d = new KviFileDialog(initial,filter,parent,"open_file_name_dialog",true);
	d->setCaption(caption);
	d->setMode(QFileDialog::ExistingFile);
	//d->setShowHiddenFiles(showHidden);
	if(d->exec() == QDialog::Accepted)
	{
		buffer = d->selectedFile();
		KviFileUtils::adjustFilePath(buffer);
		delete d;
		return !buffer.isEmpty();
	}
	delete d;
	return false;
}


bool KviFileDialog::askForSaveFileName(QString &buffer,const QString & caption,const QString &initial,const QString &filter,bool showHidden,bool bConfirmOverwrite,bool showNative,QWidget* parent)
{
	#ifdef COMPILE_ON_WINDOWS
	if (showNative)
	{
		while (1)
		{
			buffer=QFileDialog::getSaveFileName(initial,filter,parent,"save_file_name_dialog",caption);
			KviFileUtils::adjustFilePath(buffer);
			//return !buffer.isEmpty();
			if(buffer.isEmpty()) return false;
			if(!bConfirmOverwrite) return true;
			// Check for the file existence
			if(!KviFileUtils::fileExists(buffer)) return true;
			QString tmp;
			KviQString::sprintf(tmp,__tr2qs("The file %s already exists.<br>Do you wish to overwrite it?"),buffer.utf8().data());
			switch(QMessageBox::information(parent,__tr2qs("File Exists - KVIrc"),tmp,QMessageBox::Yes,QMessageBox::No | QMessageBox::Default,QMessageBox::Cancel | QMessageBox::Escape))
			{
					case QMessageBox::Cancel: return false; break;
					case QMessageBox::Yes:    return true;  break;
			}
			
		} 
		
		
	}
	#endif

	
	KviFileDialog * d = new KviFileDialog(initial,filter,parent,"save_file_name_dialog",true);
	d->setCaption(caption);
	d->setMode(QFileDialog::AnyFile);
	#ifdef COMPILE_KDE_SUPPORT
	d->setKeepLocation(true);
	#endif
	//d->setShowHiddenFiles(showHidden);

	while(d->exec() == QDialog::Accepted)
	{
		buffer = d->selectedFile();
		KviFileUtils::adjustFilePath(buffer);

		if(!buffer.isEmpty())
		{
			if(!bConfirmOverwrite)
			{
				delete d;
				return true;
			}
			// Check for the file existence
			if(KviFileUtils::fileExists(buffer))
			{
				QString tmp;
				KviQString::sprintf(tmp,__tr2qs("The file %s already exists.<br>Do you wish to overwrite it?"),buffer.utf8().data());
				switch(QMessageBox::information(d,__tr2qs("File Exists - KVIrc"),tmp,QMessageBox::Yes,QMessageBox::No | QMessageBox::Default,QMessageBox::Cancel | QMessageBox::Escape))
				{
					case QMessageBox::Cancel: delete d; return false; break;
					case QMessageBox::Yes:    delete d; return true;  break;
				}
			} else {
				delete d;
				return true; // ok...file not exists
			}
		} else {
			delete d;
			return false; // empty buffer
		}
	}

	delete d;
	return false;
	
}

bool KviFileDialog::askForDirectoryName(QString &buffer,const QString & caption,const QString & initial,const char * filter,bool showHidden,bool showNative,QWidget* parent)
{
#ifdef COMPILE_ON_WINDOWS
	if(showNative)
	{
		buffer = QFileDialog::getExistingDirectory(initial,parent,"open_file_name_dialog",caption);
		return !buffer.isEmpty();
	}
#else
	#ifdef COMPILE_KDE_SUPPORT
		// the KDE based dir selection dialog is now quite nice
		buffer = KFileDialog::getExistingDirectory(initial,parent,caption);
		return !buffer.isEmpty();
	#endif
#endif

	KviFileDialog * d = new KviFileDialog(initial,
		filter,parent,"directory_name_dialog",true);
	d->setCaption(caption);
	d->setMode(QFileDialog::Directory);
	//d->setShowHiddenFiles(showHidden);
	if(d->exec() == QDialog::Accepted)
	{
		buffer = d->selectedFile();
		KviFileUtils::adjustFilePath(buffer);
		delete d;
		return !buffer.isEmpty();
	}
	delete d;

	return false;
}


bool KviFileDialog::askForOpenFileNames(QStringList &buffer,const QString & caption,const QString & initial,const char * filter,bool showHidden,bool showNative,QWidget* parent)
{
	#ifdef COMPILE_ON_WINDOWS
	if (showNative)
	{
		buffer=QFileDialog::getOpenFileNames(filter,initial,parent,"open_file_name_dialog",caption);
		return (buffer.count()>0);
	}
	
	#endif
	KviFileDialog * d = new KviFileDialog(initial,filter ? QString(filter) : QString::null,parent,"open_file_names_dialog",true);
	d->setCaption(caption);
	d->setMode(QFileDialog::ExistingFiles);
	//d->setShowHiddenFiles(showHidden);	
	if(d->exec() == QDialog::Accepted)
	{
		buffer = d->selectedFiles();
		delete d;
		return (buffer.count() > 0);
	}
	delete d;
	return false;
}


#include "kvi_filedialog.moc"
