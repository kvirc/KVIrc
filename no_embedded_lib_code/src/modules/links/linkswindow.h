#ifndef _LINKSWINDOW_H_
#define _LINKSWINDOW_H_
//=============================================================================
//
//   File : linkswindow.h
//   Creation date : Thu Dec 21 2000 12:42:55 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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
#include "kvi_irccontext.h"
#include <QTreeWidget>
#include "kvi_tal_popupmenu.h"

#include <QToolButton>

class KviThemedLabel;


typedef struct _KviLink
{
	KviStr host;
	KviStr parent;
	int hops;
	KviStr description;
} KviLink;

class KviLinksListView : public QTreeWidget
{
	Q_OBJECT
public:
	KviLinksListView(QWidget*);
	~KviLinksListView(){};
protected:
	void mousePressEvent (QMouseEvent *e);
signals:
	void rightButtonPressed(QTreeWidgetItem *,QPoint);
};

class KviLinksWindow : public KviWindow, public KviExternalServerDataParser
{
	Q_OBJECT
public:
	KviLinksWindow(KviFrame * lpFrm,KviConsole * lpConsole);
	~KviLinksWindow();
protected:
	QSplitter      * m_pVertSplitter;
	QSplitter      * m_pTopSplitter;
	KviLinksListView      * m_pListView;
	KviPointerList<KviLink> * m_pLinkList;
	KviTalPopupMenu     * m_pHostPopup;
	QString          m_szRootServer;
	QToolButton    * m_pRequestButton;
	KviThemedLabel * m_pInfoLabel;
public: // Methods
	virtual void control(int msg);
	virtual void processData(KviIrcMessage * msg);
	virtual void die();
protected:
	virtual QPixmap * myIconPtr();
	virtual void fillCaptionBuffers();
	virtual void applyOptions();
	virtual void resizeEvent(QResizeEvent *e);
	virtual void getBaseLogFileName(QString &buffer);
protected slots:
	void showHostPopup(QTreeWidgetItem *i,const QPoint &p);
	void hostPopupClicked(int id);
	void requestLinks();
	void connectionStateChange();
public:
	virtual QSize sizeHint() const;
private:
	void reset();
	void endOfLinks();
	QTreeWidgetItem * insertLink(KviLink * l);
	QTreeWidgetItem * getItemByHost(const char *host,QTreeWidgetItem * par);
};

#endif //_KVI_LINKSWINDOW_H_
