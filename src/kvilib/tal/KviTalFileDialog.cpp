//=============================================================================
//
//   File : KviTalFileDialog.cpp
//   Creation date : Thu Sep 11 2003 04:09:24 by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2003-2010 Szymon Stefanek (pragma at kvirc dot net)
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



#include "KviTalFileDialog.h"

#ifdef COMPILE_KDE_SUPPORT

KviTalFileDialog::KviTalFileDialog(const QString & szDirName, const QString & szFilter, QWidget * pParent, const char *, bool)
: KFileDialog(KUrl(szDirName),szFilter,pParent)
{
	//clearWFlags(WDestructiveClose);
}

#else // COMPILE_KDE_SUPPORT

KviTalFileDialog::KviTalFileDialog(const QString & szDirName, const QString & szFilter, QWidget * pParent, const char *, bool bModal)
: QFileDialog(pParent,"",szDirName,szFilter)
{
	setModal(bModal);
}

#endif // COMPILE_KDE_SUPPORT

KviTalFileDialog::~KviTalFileDialog()
{
}

void KviTalFileDialog::setFileMode(FileMode m)
{
	switch(m)
	{
#ifdef COMPILE_KDE_SUPPORT
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
#else // COMPILE_KDE_SUPPORT
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
#endif // COMPILE_KDE_SUPPORT
	}
}

void KviTalFileDialog::setDirectory(const QString & szDirectory)
{
#ifdef COMPILE_KDE_SUPPORT
	setUrl(KUrl(szDirectory));
#else
	QFileDialog::setDirectory(szDirectory);
#endif
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
	#include "KviTalFileDialog.moc"
#endif //COMPILE_USE_STANDALONE_MOC_SOURCES
