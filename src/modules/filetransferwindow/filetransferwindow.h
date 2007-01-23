#ifndef _FILETRANSFERWINDOW_H_
#define _FILETRANSFERWINDOW_H_
//==================================================================================
//
//   File : filetransferwindow.h
//   Creation date : Mon Apr 21 2003 23:14:12 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2001 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//==================================================================================

#include "kvi_window.h"
#include "kvi_string.h"
#include "kvi_ircsocket.h"
#include "kvi_sparser.h"
#include "kvi_console.h"
#include "kvi_moduleextension.h"
#include "kvi_filetransfer.h"
#include "kvi_dynamictooltip.h"

#include <qlistview.h>
#include "kvi_tal_popupmenu.h"
#include <qtoolbutton.h>
#include <qtimer.h>
#include <qpixmap.h>



class KviFileTransferItem : public QListViewItem
{
public:
	KviFileTransferItem(QListView * v,KviFileTransfer * t);
	~KviFileTransferItem();
protected:
	KviFileTransfer * m_pTransfer;
public:
	KviFileTransfer * transfer(){ return m_pTransfer; };
	virtual void paintCell(QPainter * p,const QColorGroup &cg,int column,int width,int align);
	virtual void setHeight(int h);
	virtual QString key(int column,bool bAcending) const;
};


class KviFileTransferWindow : public KviWindow , public KviModuleExtension
{
	friend class KviFileTransferItem;
	Q_OBJECT
public:
	KviFileTransferWindow(KviModuleExtensionDescriptor * d,KviFrame * lpFrm);
	~KviFileTransferWindow();
protected:
	QSplitter           * m_pVertSplitter;
	QListView           * m_pListView;
	KviTalPopupMenu          * m_pContextPopup;
	KviTalPopupMenu          * m_pLocalFilePopup;
	KviTalPopupMenu          * m_pOpenFilePopup;
	QTimer              * m_pTimer;
	QPixmap             * m_pMemPixmap;
	int                   m_iLineSpacing; // cached fm value
public: // Methods
	virtual void die();
protected:
	QPixmap * memPixmap(){ return m_pMemPixmap; };
	virtual QPixmap * myIconPtr();
	virtual void fillCaptionBuffers();
	virtual void applyOptions();
	virtual void resizeEvent(QResizeEvent *e);
	virtual void getBaseLogFileName(KviStr &buffer);
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
	void rightButtonPressed(QListViewItem *it,const QPoint &pnt,int col);
	void doubleClicked(QListViewItem *it,const QPoint &pnt,int col);
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
