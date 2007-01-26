#ifndef _KVI_IMAGEDIALOG_H_
#define _KVI_IMAGEDIALOG_H_
//
//   File : kvi_imagedialog.h
//   Creation date : Sun Dec 22 2002 19:42 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)
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




#include <qdialog.h>
#include <qcombobox.h>
#include "kvi_tal_listbox.h"
#include <qvaluelist.h>
#include <qtimer.h>
#include <qstringlist.h>

#include "kvi_dynamictooltip.h"

class KviImageDialogItem : public KviTalListBoxPixmap
{
public:
	bool m_bIsFolder;
	QString m_szImageId;
	QString m_szTipText;
public:
	KviImageDialogItem(KviTalListBox * b,const QPixmap &thumb,const QString &szFile,const QString &image_id,const QString &szTipText = QString::null,bool bIsFolder = false)
	: KviTalListBoxPixmap(b,thumb,szFile) , m_bIsFolder(bIsFolder) , m_szImageId(image_id) , m_szTipText(szTipText) {};
	~KviImageDialogItem(){};
public:
	bool isFolder(){ return m_bIsFolder; };
	const QString & imageId(){ return m_szImageId; };
	const QString & tipText(){ return m_szTipText; };
	virtual int height(const KviTalListBox *) const;
	virtual int width(const KviTalListBox *) const;
	virtual void paint(QPainter * p);
};


#define KID_TYPE_BUILTIN_IMAGES_SMALL 1
#define KID_TYPE_FULL_PATH 2

#define KID_TYPE_ALL 3

class KVIRC_API KviImageDialog : public QDialog
{
	Q_OBJECT
public:
	KviImageDialog(QWidget * par,
		const QString &szCaption = QString::null,
		int types = KID_TYPE_ALL,
		int initialType = 0,
		const QString &szInitialDir = QString::null,
		int maxPreviewFileSize = 256000, bool modal=false);
	virtual ~KviImageDialog();
protected:
	QComboBox       * m_pTypeComboBox;
	QValueList<int> * m_pTypeList;
	KviTalListBox        * m_pListBox;
	QTimer          * m_pTimer;
	int               m_iJobType;

	int               m_iMaxPreviewFileSize;

	QString           m_szJobPath;
	QStringList       m_lJobFileList;
	
	int               m_iJobIndexHelper;

	QString           m_szSelectedImage;

	QString           m_szInitialPath;

	KviDynamicToolTip * m_pTip;
public:
	const QString & selectedImage(){ return m_szSelectedImage; };
protected:
	void startJob(int type,const QString &szInitialPath = QString::null);
	void jobTerminated();
	virtual void closeEvent(QCloseEvent *e);
protected slots:
	void okClicked();
	void cancelClicked();
	void heartbeat();
	void jobTypeSelected(int index);
	void itemDoubleClicked(KviTalListBoxItem * it);
	void tipRequest(KviDynamicToolTip *,const QPoint &pnt);
};

#endif //_KVI_IMAGEDIALOG_H_
