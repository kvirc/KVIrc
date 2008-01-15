#ifndef _LISTWINDOW_H_
#define _LISTWINDOW_H_
//=============================================================================
//
//   File : listwindow.h
//   Creation date : Thu Oct 7 2001 13:27:55 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2004 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#include "kvi_window.h"
#include "kvi_string.h"
#include "kvi_ircsocket.h"
#include "kvi_pointerlist.h"
#include "kvi_sparser.h"
#include "kvi_console.h"
#include "kvi_irccontext.h"

#include "kvi_tal_listview.h"
#include "kvi_tal_popupmenu.h"
#include <qtoolbutton.h>
#include <qlineedit.h>

class KviThemedLabel;

class KviChannelListViewItemData
{
	friend class KviChannelListViewItem;
	friend class KviListWindow;
public:
	KviChannelListViewItemData(const QString &szChan,const QString &szUsers,const QString &szTopic);
	~KviChannelListViewItemData();
protected:
	QString m_szChan;
	QString m_szUsers;
	QString m_szTopic;
	QString m_szUsersKey;
};

class KviChannelListViewItem : public KviTalListViewItem
{
	friend class KviListWindow;
public:
	KviChannelListViewItem(KviTalListView * v,KviChannelListViewItemData * pData);
	~KviChannelListViewItem();
protected:
	KviChannelListViewItemData * m_pData;
public:
	const QString & channel(){ return m_pData->m_szChan; };
#ifdef COMPILE_USE_QT4
	int width ( const QFontMetrics & fm, const KviTalListView * lv, int column ) const;
#else
	int width ( const QFontMetrics & fm, const QListView * lv, int column ) const;
#endif
protected:
	virtual void paintCell(QPainter * p,const QColorGroup &cg,int col,int wdth,int align);
	virtual QString key(int col,bool) const;
};



class KviListWindow : public KviWindow, public KviExternalServerDataParser
{
	Q_OBJECT
public:
	KviListWindow(KviFrame * lpFrm,KviConsole * lpConsole);
	~KviListWindow();
protected:
	QSplitter                              * m_pVertSplitter;
	QSplitter                              * m_pTopSplitter;
	KviTalListView                         * m_pListView;
	QLineEdit                              * m_pParamsEdit;
	QToolButton                            * m_pRequestButton;
	QToolButton							   * m_pStopListDownloadButton;
	QToolButton							   * m_pOpenButton;
	QToolButton							   * m_pSaveButton;
	KviThemedLabel                         * m_pInfoLabel;
	QTimer                                 * m_pFlushTimer;
	KviPointerList<KviChannelListViewItemData> * m_pItemList;
public: // Methods
	virtual void control(int msg);
	virtual void processData(KviIrcMessage * msg);
	virtual void die();
protected:
	virtual QPixmap * myIconPtr();
	virtual void fillCaptionBuffers();
	virtual void applyOptions();
	virtual void resizeEvent(QResizeEvent *e);
	virtual void getBaseLogFileName(KviStr &buffer);
protected slots:
	void flush();
	void itemDoubleClicked(KviTalListViewItem *it);
	void requestList();
	void stoplistdownload();
	void connectionStateChange();
	void exportList();
	void importList();
public:
	virtual QSize sizeHint() const;
private:
	void reset();
	void endOfList();
	void startOfList();
};

#endif //_KVI_LISTWINDOW_H_
