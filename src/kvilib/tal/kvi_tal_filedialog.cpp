//=============================================================================
//
//   File : kvi_tal_filedialog.coo
//   Creation date : Thu Sep 11 2003 04:09:24 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2003-2007 Szymon Stefanek (pragma at kvirc dot net)
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

#define __KVILIB__


#include "kvi_tal_filedialog.h"

#ifdef COMPILE_KDE_SUPPORT

	KviTalFileDialog::KviTalFileDialog(const QString &dirName,const QString &filter,QWidget *parent,const char *name,bool modal)
	: KFileDialog(KUrl(dirName),filter,parent)
	{
	//	clearWFlags(WDestructiveClose);
	}

	KviTalFileDialog::~KviTalFileDialog()
	{
	}

	void KviTalFileDialog::setFileMode(FileMode m)
	{
		switch(m)
		{
			case AnyFile:
				setMode(KFile::File | KFile::LocalOnly);
				setOperationMode(Saving);
			break;
			case ExistingFile:
				setMode(KFile::File | KFile::ExistingOnly | KFile::LocalOnly);
				setOperationMode(Opening);
			break;
			case ExistingFiles:
				setMode(KFile::Files | KFile::ExistingOnly | KFile::LocalOnly);
				setOperationMode(Opening);
			break;
			case Directory:
				setMode(KFile::Directory);
			break;
			case DirectoryOnly:
				setMode(KFile::Directory);
			break;
			default:
				setMode(KFile::File | KFile::LocalOnly);
				setOperationMode(Saving);
			break;
		}
	}

	void KviTalFileDialog::setDirectory(const QString &szDirectory)
	{
		setUrl(KUrl(szDirectory));
	}


#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
	#include "kvi_tal_filedialog_kde.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES

#else

	#include <QDir>

	KviTalFileDialog::KviTalFileDialog(const QString &dirName,const QString &filter,QWidget *parent,const char *name,bool modal)
	: QFileDialog(parent,"",dirName,filter)
	{
		setModal(modal);
	}

	KviTalFileDialog::~KviTalFileDialog()
	{
	}

	void KviTalFileDialog::setFileMode(FileMode m)
	{
		switch(m)
		{
			case AnyFile:
				QFileDialog::setFileMode(QFileDialog::AnyFile);
			break;
			case ExistingFile:
				QFileDialog::setFileMode(QFileDialog::ExistingFile);
			break;
			case ExistingFiles:
				QFileDialog::setFileMode(QFileDialog::ExistingFiles);
			break;
			case Directory:
				QFileDialog::setFileMode(QFileDialog::Directory);
			break;
			case DirectoryOnly:
				QFileDialog::setFileMode(QFileDialog::DirectoryOnly);
			break;
			default:
				QFileDialog::setFileMode(QFileDialog::AnyFile);
			break;
		}
	}

	void KviTalFileDialog::setDirectory(const QString &szDirectory)
	{
		QFileDialog::setDirectory(szDirectory);
	}


#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
	#include "kvi_tal_filedialog_qt.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES

#endif
