#ifndef _LISTWINDOW_H_
#define _LISTWINDOW_H_
//=============================================================================
//
//   File : ListWindow.h
//   Creation date : Thu Oct 7 2001 13:27:55 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the lists of the GNU General Public License
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
#include "KviPointerList.h"
#include "KviIrcServerParser.h"
#include "KviConsoleWindow.h"
#include "KviIrcContext.h"
#include "KviThemedTreeWidget.h"

#include <QToolButton>
#include <QLineEdit>
#include <QItemDelegate>
#include <QMenu>

class KviThemedLabel;
class KviThemedLineEdit;

class ChannelTreeWidgetItemDelegate : public QItemDelegate
{
public:
	ChannelTreeWidgetItemDelegate(QTreeWidget * pWidget = nullptr);
	~ChannelTreeWidgetItemDelegate();
	void paint(QPainter * pPainter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
	QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const;
};

class ChannelTreeWidgetItemData
{
	friend class ChannelTreeWidgetItem;
	friend class ListWindow;
	friend class ChannelTreeWidgetItemDelegate;

public:
	ChannelTreeWidgetItemData(const QString & szChan, const QString & szUsers, const QString & szTopic);
	~ChannelTreeWidgetItemData();

protected:
	QString m_szChan;
	QString m_szUsers;
	QString m_szTopic;
	QString m_szStrippedTopic;
};

class ChannelTreeWidgetItem : public QTreeWidgetItem
{
	friend class ListWindow;

public:
	ChannelTreeWidgetItem(ChannelTreeWidgetItemData * pData);
	~ChannelTreeWidgetItem();

private:
	ChannelTreeWidgetItemData * m_pData;

public:
	bool operator<(const QTreeWidgetItem & other) const;
	ChannelTreeWidgetItemData * itemData() const { return m_pData; }
};

class ChannelTreeWidget : public KviThemedTreeWidget
{
	friend class ChannelTreeWidgetItem;
	friend class ChannelTreeWidgetItemData;
	Q_OBJECT
public:
	ChannelTreeWidget(QWidget * par, KviWindow * pWindow, const char * name) : KviThemedTreeWidget(par, pWindow, name){};
	~ChannelTreeWidget(){};
	ChannelTreeWidgetItem * itemFromIndex(const QModelIndex & index) const { return (ChannelTreeWidgetItem *)KviThemedTreeWidget::itemFromIndex(index); };
};

class ListWindow : public KviWindow, public KviExternalServerDataParser
{
	Q_OBJECT
public:
	ListWindow(KviConsoleWindow * lpConsole);
	~ListWindow();

protected:
	QSplitter * m_pVertSplitter;
	QSplitter * m_pTopSplitter;
	ChannelTreeWidget * m_pTreeWidget;
	KviThemedLineEdit * m_pParamsEdit;
	QToolButton * m_pRequestButton;
	QToolButton * m_pStopListDownloadButton;
	QToolButton * m_pOpenButton;
	QToolButton * m_pSaveButton;
	KviThemedLabel * m_pInfoLabel;
	QTimer * m_pFlushTimer;
	KviPointerList<ChannelTreeWidgetItemData> * m_pItemList;

public: // Methods
	void control(int iMsg) override;
	void processData(KviIrcMessage * pMsg) override;
	void die() override;
	QSize sizeHint() const override;

protected:
	QPixmap * myIconPtr() override;
	void fillCaptionBuffers() override;
	void applyOptions() override;
	void resizeEvent(QResizeEvent * e) override;
	void getBaseLogFileName(QString & szBuffer) override;
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
