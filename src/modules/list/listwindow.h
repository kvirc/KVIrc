#ifndef _LISTWINDOW_H_
#define _LISTWINDOW_H_
//=============================================================================
//
//   File : listwindow.h
//   Creation date : Thu Oct 7 2001 13:27:55 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2008 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the lists of the GNU General Public License
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
#include "kvi_pointerlist.h"
#include "kvi_sparser.h"
#include "kvi_console.h"
#include "kvi_irccontext.h"
#include <QTreeWidget>
#include "kvi_tal_popupmenu.h"

#include <QToolButton>
#include <QLineEdit>
#include <QItemDelegate>

class KviThemedLabel;

class KviChannelTreeWidgetItemDelegate : public QItemDelegate
{
public:
	KviChannelTreeWidgetItemDelegate(QTreeWidget * pWidget = 0);
	~KviChannelTreeWidgetItemDelegate();
	void paint(QPainter * pPainter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
};

class KviChannelTreeWidgetItemData
{
	friend class KviChannelTreeWidgetItem;
	friend class KviListWindow;
	friend class KviChannelTreeWidgetItemDelegate;
public:
	KviChannelTreeWidgetItemData(const QString & szChan, const QString & szUsers, const QString & szTopic);
	~KviChannelTreeWidgetItemData();
protected:
	QString m_szChan;
	QString m_szUsers;
	QString m_szTopic;
};

class KviChannelTreeWidgetItem : public QTreeWidgetItem
{
	friend class KviListWindow;
public:
	KviChannelTreeWidgetItem(KviChannelTreeWidgetItemData * pData);
	~KviChannelTreeWidgetItem();
private:
	KviChannelTreeWidgetItemData * m_pData;
public:
	int width (const QFontMetrics & fm, const QTreeWidget * pWidget, int iColumn) const;
	bool operator<(const QTreeWidgetItem & other) const;
	inline KviChannelTreeWidgetItemData * itemData() { return m_pData; }; 
};

class KviListWindow : public KviWindow, public KviExternalServerDataParser
{
	Q_OBJECT
public:
	KviListWindow(KviFrame * lpFrm, KviConsole * lpConsole);
	~KviListWindow();
protected:
	QSplitter                                    * m_pVertSplitter;
	QSplitter                                    * m_pTopSplitter;
	QTreeWidget                                  * m_pTreeWidget;
	QLineEdit                                    * m_pParamsEdit;
	QToolButton                                  * m_pRequestButton;
	QToolButton                                  * m_pStopListDownloadButton;
	QToolButton                                  * m_pOpenButton;
	QToolButton                                  * m_pSaveButton;
	KviThemedLabel                               * m_pInfoLabel;
	QTimer                                       * m_pFlushTimer;
	KviPointerList<KviChannelTreeWidgetItemData> * m_pItemList;
public: // Methods
	virtual void control(int iMsg);
	virtual void processData(KviIrcMessage * pMsg);
	virtual void die();
	virtual QSize sizeHint() const;
protected:
	virtual QPixmap * myIconPtr();
	virtual void fillCaptionBuffers();
	virtual void applyOptions();
	virtual void resizeEvent(QResizeEvent * e);
	virtual void getBaseLogFileName(QString & szBuffer);
protected slots:
	void flush();
	void itemDoubleClicked(QTreeWidgetItem * it, int);
	void requestList();
	void stoplistdownload();
	void connectionStateChange();
	void exportList();
	void importList();
	void liveSearch(const QString & szText);
private:
	void reset();
	void endOfList();
	void startOfList();
};

#endif //_KVI_LISTWINDOW_H_
