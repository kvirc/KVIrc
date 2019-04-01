#ifndef _LINKSWINDOW_H_
#define _LINKSWINDOW_H_
//=============================================================================
//
//   File : LinksWindow.h
//   Creation date : Thu Dec 21 2000 12:42:55 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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
#include "KviIrcContext.h"
#include "KviThemedTreeWidget.h"

#include <QToolButton>
#include <QMenu>

#include <memory>
#include <vector>

class KviThemedLabel;

struct KviLink
{
	KviCString host;
	KviCString parent;
	int hops;
	KviCString description;
};

class LinksListView : public KviThemedTreeWidget
{
	Q_OBJECT
public:
	LinksListView(QWidget * par, KviWindow * wnd, const char * txt);
	~LinksListView(){};

protected:
	void mousePressEvent(QMouseEvent * e) override;
signals:
	void rightButtonPressed(QTreeWidgetItem *, QPoint);
};

class LinksWindow : public KviWindow, public KviExternalServerDataParser
{
	Q_OBJECT
public:
	LinksWindow(KviConsoleWindow * lpConsole);
	~LinksWindow();

protected:
	QSplitter * m_pVertSplitter;
	QSplitter * m_pTopSplitter;
	LinksListView * m_pListView;
	std::vector<std::unique_ptr<KviLink>> m_pLinkList;
	QMenu * m_pHostPopup;
	QString m_szRootServer;
	QToolButton * m_pRequestButton;
	KviThemedLabel * m_pInfoLabel;

public: // Methods
	void control(int msg) override;
	void processData(KviIrcMessage * msg) override;
	void die() override;

protected:
	QPixmap * myIconPtr() override;
	void fillCaptionBuffers() override;
	void applyOptions() override;
	void resizeEvent(QResizeEvent * e) override;
	void getBaseLogFileName(QString & buffer) override;
protected slots:
	void showHostPopup(QTreeWidgetItem * i, const QPoint & p);
	void hostPopupClicked(QAction * pAction);
	void requestLinks();
	void connectionStateChange();

public:
	QSize sizeHint() const override;

private:
	void reset();
	void endOfLinks();
	QTreeWidgetItem * insertLink(KviLink * l);
	QTreeWidgetItem * getItemByHost(const char * host, QTreeWidgetItem * par);
};

#endif //_KVI_LINKSWINDOW_H_
