#ifndef _KVI_IMAGEDIALOG_H_
#define _KVI_IMAGEDIALOG_H_
//=============================================================================
//
//   File : KviImageDialog.h
//   Creation date : Sun Dec 22 2002 19:42 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"
#include "KviDynamicToolTip.h"
#include "KviTalListWidget.h"

#include <QDialog>
#include <QComboBox>
#include <QTimer>
#include <QStringList>
#include <QList>

class KviImageDialogItem : public KviTalListWidgetPixmap
{
public:
	bool m_bIsFolder;
	QString m_szImageId;
	QString m_szTipText;

public:
	KviImageDialogItem(KviTalListWidget * b, const QPixmap & thumb, const QString & szFile, const QString & image_id, const QString & szTipText = QString(), bool bIsFolder = false)
	    : KviTalListWidgetPixmap(b, thumb, szFile), m_bIsFolder(bIsFolder), m_szImageId(image_id), m_szTipText(szTipText){};
	~KviImageDialogItem(){};

public:
	bool isFolder() { return m_bIsFolder; };
	const QString & imageId() { return m_szImageId; };
	const QString & tipText() { return m_szTipText; };
	virtual int height(const KviTalListWidget *) const;
	virtual int width(const KviTalListWidget *) const;
	void paint(QPainter * p) override;
};

#define KID_TYPE_BUILTIN_IMAGES_SMALL 1
#define KID_TYPE_FULL_PATH 2

#define KID_TYPE_ALL 3

class KVIRC_API KviImageDialog : public QDialog
{
	Q_OBJECT
public:
	KviImageDialog(QWidget * par,
	    const QString & szCaption = QString(),
	    int types = KID_TYPE_ALL,
	    int initialType = 0,
	    const QString & szInitialDir = QString(),
	    int maxPreviewFileSize = 256000, bool modal = false);
	~KviImageDialog();

protected:
	QComboBox * m_pTypeComboBox;
	QList<int> * m_pTypeList;
	KviTalListWidget * m_pListBox;
	QTimer * m_pTimer;
	int m_iJobType;

	int m_iMaxPreviewFileSize;

	QString m_szJobPath;
	QStringList m_lJobFileList;

	int m_iJobIndexHelper;

	QString m_szSelectedImage;

	QString m_szInitialPath;

	KviDynamicToolTip * m_pTip;

public:
	const QString & selectedImage() { return m_szSelectedImage; };
protected:
	void startJob(int type, const QString & szInitialPath = QString());
	void jobTerminated();
	void closeEvent(QCloseEvent * e) override;
protected slots:
	void okClicked();
	void cancelClicked();
	void heartbeat();
	void jobTypeSelected(int index);
	void itemDoubleClicked(QListWidgetItem * it);
	void tipRequest(KviDynamicToolTip *, const QPoint & pnt);
};

#endif //_KVI_IMAGEDIALOG_H_
