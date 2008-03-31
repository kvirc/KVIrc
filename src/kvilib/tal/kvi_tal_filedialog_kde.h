#ifndef _KVI_TAL_FILEDIALOG_KDE_H_
#define _KVI_TAL_FILEDIALOG_KDE_H_

//
//   File : kvi_tal_filedialog_kde.h
//   Creation date : Thu Aug 11 2003 04:43:58 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2003 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"

#include <KFileDialog>

class KVILIB_API KviTalFileDialog : public KFileDialog
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
	{
		// QFileDialog allows making new directories...kfiledialog not :/
		return KFileDialog::getExistingDirectory(KUrl(dir),parent,caption);
		//return getExistingDirectory(dir,parent,caption);
	};
};

#endif // _KVI_TAL_FILEDIALOG_KDE_H_
