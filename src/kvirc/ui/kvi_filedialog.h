#ifndef _KVI_FILEDIALOG_H_
#define _KVI_FILEDIALOG_H_

//
//   File : kvi_filedialog.h
//   Creation date : Mon Nov 20 2000 03:52:12 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2000 Szymon Stefanek (pragma at kvirc dot net)
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

#ifdef Unsorted
	#undef Unsorted
#endif
#include "kvi_settings.h"
#include "kvi_string.h"


#include "kvi_tal_filedialog.h"

#include <qstringlist.h>


class KVIRC_API KviFileDialog : public KviTalFileDialog
{
	Q_OBJECT
public:
	KviFileDialog(const QString &dirName, const QString &filter=QString::null,
					QWidget *parent=0, const char *name=0, bool modal=false);
	~KviFileDialog();
public:
	static bool askForOpenFileName(QString &buffer,const QString & caption,const QString &initial = QString::null,const QString &filter = QString::null,bool showHidden = false,bool showNative = true,QWidget* parent=0);
	static bool askForSaveFileName(QString &buffer,const QString & caption,const QString &initial = QString::null,const QString &filter = QString::null,bool showHidden = false,bool bConfirmOverwrite = false,bool showNative = true,QWidget* parent=0);

	static bool askForDirectoryName(QString &buffer,const QString & caption,const QString & initial,const char * filter = 0,bool showHidden = false,bool showNative=true,QWidget* parent=0);
	static bool askForOpenFileNames(QStringList &buffer,const QString & caption,const QString & initial,const char * filter = 0,bool showHidden = false,bool showNative=true,QWidget* parent=0);

};


#endif //_KVI_FILEDIALOG_H_
