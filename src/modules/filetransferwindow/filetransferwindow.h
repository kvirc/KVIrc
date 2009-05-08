#ifndef _FILETRANSFERWINDOW_H_
#define _FILETRANSFERWINDOW_H_
//=============================================================================
//
//   File : filetransferwindow.h
//   Creation date : Mon Apr 21 2003 23:14:12 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2003-2008 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the linkss of the GNU General Public License
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

#include "kvi_window.h"
#include "kvi_string.h"
#include "kvi_ircsocket.h"
#include "kvi_sparser.h"
#include "kvi_console.h"
#include "kvi_moduleextension.h"
#include "kvi_filetransfer.h"
#include "kvi_dynamictooltip.h"
#include "kvi_tal_tablewidget.h"
#include "kvi_tal_popupmenu.h"
#include "kvi_tal_itemdelegates.h"

#include <QTimer>
#include <QPixmap>

class KviFileTransferItem;

class KviFileTransferWidget : public KviTalTableWidget
{
	friend class KviFileTransferItemDelegate;
	Q_OBJECT
	Q_PROPERTY(int TransparencyCapable READ dummyRead)
public:
	KviFileTransferWidget(QWidget * pParent);
	~KviFileTransferWidget() {};
	void paintEvent(QPaintEvent * event);
	int dummyRead() const { return 0; };
protected:
	void mousePressEvent (QMouseEvent *e);
	void mouseDoubleClickEvent (QMouseEvent *e);
signals:
	void rightButtonPressed(KviFileTransferItem *,QPoint pnt);
	void doubleClicked(KviFileTransferItem *,QPoint pnt);
};

class KviFileTransferItem : public KviTalTableWidgetItemEx
{
public:
	KviFileTransferItem(KviFileTransferWidget * v,KviFileTransfer * t);
	~KviFileTransferItem();
protected:
	KviFileTransfer * m_pTransfer;
	KviTalTableWidgetItem * col1Item; //item on the second column
	KviTalTableWidgetItem * col2Item; //item on the third column
public:
	KviFileTransfer * transfer(){ return m_pTransfer; };
	virtual QString key(int column,bool bAcending) const;
	virtual void displayUpdate();
};

class KviFileTransferItemDelegate : public KviTalIconAndRichTextItemDelegate
{
	Q_OBJECT
public:
	KviFileTransferItemDelegate(QAbstractItemView * pWidget=0)
		: KviTalIconAndRichTextItemDelegate(pWidget) {};
	~KviFileTransferItemDelegate(){};
	 QSize sizeHint(const QStyleOptionViewItem &option,const QModelIndex &index) const;
	void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
};

class KviFileTransferWindow : public KviWindow , public KviModuleExtension
{
	friend class KviFileTransferItem;
	friend class KviFileTransferItemDelegate;
	Q_OBJECT
public:
	KviFileTransferWindow(KviModuleExtensionDescriptor * d,KviFrame * lpFrm);
	~KviFileTransferWindow();
protected:
	QSplitter		* m_pVertSplitter;
	KviFileTransferWidget	* m_pTableWidget;
	QAbstractItemDelegate	* m_pItemDelegate;
	KviTalPopupMenu		* m_pContextPopup;
	KviTalPopupMenu		* m_pLocalFilePopup;
	KviTalPopupMenu		* m_pOpenFilePopup;
	QTimer			* m_pTimer;
	int			m_iLineSpacing; // cached fm value
public: // Methods
	virtual void die();
protected:
	virtual QPixmap * myIconPtr();
	virtual void fillCaptionBuffers();
	virtual void applyOptions();
	virtual void resizeEvent(QResizeEvent *e);
	virtual void getBaseLogFileName(QString &buffer);
	KviFileTransferItem * findItem(KviFileTransfer * t);
	void fillTransferView();
	KviFileTransfer * selectedTransfer();
	virtual void fontChange(const QFont &oldFont);
	bool eventFilter( QObject *obj, QEvent *ev );
public:
	virtual QSize sizeHint() const;
	int lineSpacing(){ return m_iLineSpacing; };
protected slots:
	void transferRegistered(KviFileTransfer *t);
	void transferUnregistering(KviFileTransfer *t);
	void rightButtonPressed(KviFileTransferItem *it,const QPoint &pnt);
	void doubleClicked(KviFileTransferItem *it,const QPoint &pnt);
	void heartbeat();
	void clearTerminated();
	void clearAll();
	void tipRequest(KviDynamicToolTip * tip,const QPoint &pnt);
	void openLocalFile();
	void openLocalFileWith();
	void copyLocalFileToClipboard();
	void openLocalFileFolder();
	void openFilePopupActivated(int id);
	void openLocalFileTerminal();
	void deleteLocalFile();
};

#endif //_FILETRANSFERWINDOW_H_
