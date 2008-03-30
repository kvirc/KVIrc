#ifndef _KVI_TAL_FILEDIALOG_QT_H_
#define _KVI_TAL_FILEDIALOG_QT_H_

//=============================================================================
//
//   File : kvi_tal_filedialog_qt4.h
//   Creation date : Fri 19 Jan 2007 02:17:12 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2007 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"

#include <QFileDialog>

class KVILIB_API KviTalFileDialog : public QFileDialog
{
	Q_OBJECT
public:
	KviTalFileDialog(const QString &dirName,const QString &filter = QString::null,QWidget *parent = 0,const char *name = 0,bool modal = FALSE);
	~KviTalFileDialog();
public:
	enum FileMode { AnyFile, ExistingFile, ExistingFiles, Directory, DirectoryOnly };

	void setFileMode(FileMode m);
	void setDirectory(const QString &szDirectory);

	static QString getExistingDirectoryPath(const QString &dir = QString::null,const QString &caption = QString::null,QWidget *parent = 0)
		{ return getExistingDirectory(parent,caption,dir); };
};

#endif // _KVI_TAL_FILEDIALOG_QT_H_
