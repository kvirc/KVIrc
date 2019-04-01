#ifndef _FILETRANSFERWINDOW_H_
#define _FILETRANSFERWINDOW_H_
//=============================================================================
//
//   File : FileTransferWindow.h
//   Creation date : Mon Apr 21 2003 23:14:12 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviWindow.h"
#include "KviCString.h"
#include "KviIrcSocket.h"
#include "KviIrcServerParser.h"
#include "KviConsoleWindow.h"
#if 0
#include "KviModuleExtension.h"
#endif
#include "KviFileTransfer.h"
#include "KviDynamicToolTip.h"
#include "KviTalTableWidget.h"
#include "KviTalIconAndRichTextItemDelegate.h"

#include <QTimer>
#include <QPixmap>
#include <QMenu>

class FileTransferItem;

class FileTransferWidget : public KviTalTableWidget
{
	friend class FileTransferItemDelegate;
	Q_OBJECT
	Q_PROPERTY(int TransparencyCapable READ dummyRead)
public:
	FileTransferWidget(QWidget * pParent);
	~FileTransferWidget(){};
	void paintEvent(QPaintEvent * event) override;
	int dummyRead() const { return 0; };
protected:
	void mousePressEvent(QMouseEvent * e) override;
	void mouseDoubleClickEvent(QMouseEvent * e) override;
signals:
	void rightButtonPressed(FileTransferItem *, QPoint pnt);
	void doubleClicked(FileTransferItem *, QPoint pnt);
};

class FileTransferItem : public KviTalTableWidgetItemEx
{
public:
	FileTransferItem(FileTransferWidget * v, KviFileTransfer * t);
	~FileTransferItem();

protected:
	KviFileTransfer * m_pTransfer;
	KviTalTableWidgetItem * col1Item; //item on the second column
	KviTalTableWidgetItem * col2Item; //item on the third column
public:
	KviFileTransfer * transfer() { return m_pTransfer; };
	virtual QString key(int column, bool bAcending) const;
	virtual void displayUpdate();
};

class FileTransferItemDelegate : public KviTalIconAndRichTextItemDelegate
{
	Q_OBJECT
public:
	FileTransferItemDelegate(QAbstractItemView * pWidget = nullptr)
	    : KviTalIconAndRichTextItemDelegate(pWidget){};
	~FileTransferItemDelegate(){};
	QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const;
	void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
};

class FileTransferWindow : public KviWindow
#if 0
	,public KviModuleExtension
#endif
{
	friend class FileTransferItem;
	friend class FileTransferItemDelegate;
	Q_OBJECT
public:
	FileTransferWindow(
#if 0
			KviModuleExtensionDescriptor * d
#endif
	    );
	~FileTransferWindow();

protected:
	QSplitter * m_pVertSplitter;
	FileTransferWidget * m_pTableWidget;
	QAbstractItemDelegate * m_pItemDelegate;
	QMenu * m_pContextPopup;
	QMenu * m_pLocalFilePopup;
	QMenu * m_pOpenFilePopup;
	QTimer * m_pTimer;
	int m_iLineSpacing; // cached fm value
public:                 // Methods
	virtual void die();

protected:
	QPixmap * myIconPtr() override;
	void fillCaptionBuffers() override;
	void applyOptions() override;
	void resizeEvent(QResizeEvent * e) override;
	void getBaseLogFileName(QString & buffer) override;
	FileTransferItem * findItem(KviFileTransfer * t);
	void fillTransferView();
	KviFileTransfer * selectedTransfer();
	bool eventFilter(QObject * obj, QEvent * ev) override;

public:
	QSize sizeHint() const override;
	int lineSpacing() { return m_iLineSpacing; };
protected slots:
	void transferRegistered(KviFileTransfer * t);
	void transferUnregistering(KviFileTransfer * t);
	void rightButtonPressed(FileTransferItem * it, const QPoint & pnt);
	void doubleClicked(FileTransferItem * it, const QPoint & pnt);
	void heartbeat();
	void clearTerminated();
	void clearAll();
	void tipRequest(KviDynamicToolTip * tip, const QPoint & pnt);
	void openLocalFile();
	void openLocalFileWith();
	void copyLocalFileToClipboard();
	void openLocalFileFolder();
	void openFilePopupActivated(QAction * pAction);
	void openLocalFileTerminal();
	void deleteLocalFile();
};

#endif //_FILETRANSFERWINDOW_H_
