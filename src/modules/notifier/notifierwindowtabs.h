#ifndef _KVINOTIFIERWINDOWTABS_H_
#define _KVINOTIFIERWINDOWTABS_H_
//=============================================================================
//
//   File : kvinotifierwindowtabs.h
//   Created on dom 02 gen 2005 15:30:50 by Iacopo Palazzi
//
//   This file is part of the KVIrc distribution
//   Copyright (C) 2005 Iacopo Palazzi < iakko(at)siena.linux.it >
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
//   Inc. ,51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include <qfont.h>
#include <qmap.h>
#include <qobject.h>
#include <qpixmap.h>
#include <qpoint.h>
#include <qrect.h>
#include "kvi_settings.h"
#include "kvi_pointerlist.h"
#include "notifiersettings.h"
#include "notifierwindow.h"

#include "notifiermessage.h"


class QPainter;
class KviWindow;
class KviNotifierWindowTab;

typedef struct wndTabPair_s {
	KviNotifierWindowTab * pTab;
	KviWindow * pWnd;
} wndTabPair;

class KviNotifierWindowTab : public QObject // this class defines an object for every single tab about the tabs area
{
	Q_OBJECT
public:
	KviNotifierWindowTab(KviWindow *, QString);
	~KviNotifierWindowTab();

// ================================
// Put members declaration below...
// ================================
private:
	bool m_bFocused;
	
	QColor m_cLabel;
	QColor m_clrHighlightedLabel;
	QColor m_clrNormalLabel;
	QColor m_clrChangedLabel;

	QRect m_rect;

	QString	m_label;

	KviPointerList<KviNotifierMessage> * m_pMessageList;
	KviWindow * m_pWnd;
	KviNotifierMessage	* m_pCurrentMessage;
		
	TabState m_eState;
	
public:

protected:
	

// ================================
// Put methods declaration below...
// ================================
private:

private slots:
	void labelChanged();
	void closeMe();

public:
	bool focused() { return m_bFocused; };

	int countMessage() { return m_pMessageList->count(); };
	int width(bool b = false);

	void appendMessage(KviNotifierMessage * m);
	void setCurrentMessage(KviNotifierMessage * m) { m_pCurrentMessage = m; };
	void setFocused(bool b = true);
	void setLastMessageAsCurrent();
	void setNextMessageAsCurrent();
	void setPrevMessageAsCurrent();
	void setState(TabState state);
	void setRect(int x, int y, int w, int h) { m_rect.setX(x); m_rect.setY(y); m_rect.setWidth(w); m_rect.setHeight(h); };
	
	QColor labelColor() {return m_cLabel; }
	QRect rect() { return m_rect; };
	QString label() { return m_label; };
	
	KviNotifierMessage * currentMessage() { return m_pCurrentMessage; };
	KviPointerList<KviNotifierMessage> * messageList() { return m_pMessageList; };
	KviWindow * window() { return m_pWnd; };
	KviWindow * wnd() {return m_pWnd; };
	
	TabState state() { return m_eState; };
	
protected:



};


//####################################################################################################
//## Tabs Manager
//####################################################################################################


class KviNotifierWindowTabs // this class defines the tabs area
{
public:
	KviNotifierWindowTabs(QRect r);
	~KviNotifierWindowTabs();

// ================================
// Put members declaration below...
// ================================
private:
	QRect 	m_rct;
	QPoint	m_pnt;

	QMap<KviWindow *, KviNotifierWindowTab *> m_tabMap;
	KviPointerList<KviNotifierWindowTab> m_tabPtrList;
	KviPointerList<KviNotifierWindowTab> m_lastVisitedTabPtrList;
	
	QFont * m_pFocusedFont;
	QFont * m_pUnfocusedFont;
	
	KviNotifierWindowTab * m_pTabFocused;
	KviWindow * m_pWndTabFocused;

	QRect	m_rctNextIcon;
	QRect	m_rctPrevIcon;
	QRect	m_rctCloseTabIcon;
	QRect	m_rctCloseTabIconHotArea;
	QRect	m_rctTabs;

	QPainter * m_pPainter;
	QPixmap * m_pPixmap;
	
	QPixmap	m_pixDX;
	QPixmap	m_pixSX;
	QPixmap	m_pixBKG;
	QPixmap m_pixSXFocused;
	QPixmap m_pixDXFocused;
	QPixmap m_pixBKGFocused;
	QPixmap m_pixSXUnfocused;
	QPixmap m_pixDXUnfocused;
	QPixmap m_pixBKGUnfocused;

	QPixmap m_pixIconTabPrev;
	QPixmap m_pixIconTabPrev_out;
	QPixmap m_pixIconTabPrev_over;
	QPixmap m_pixIconTabPrev_clicked;
	
	QPixmap m_pixIconTabNext;
	QPixmap m_pixIconTabNext_out;
	QPixmap m_pixIconTabNext_over;
	QPixmap m_pixIconTabNext_clicked;

	QPixmap m_pixIconCloseTab;
	QPixmap m_pixIconCloseTab_off;
	QPixmap m_pixIconCloseTab_on;
	QPixmap m_pixIconCloseTab_clicked;

	int m_closeTabIconState;
	int m_iTabToStartFrom;

	bool m_bIsOverLeftBound;
	bool m_bIsOverRightBound;
	bool m_bNeedToRedraw;

public:

protected:


// ================================
// Put methods declaration below...
// ================================
private:
	void initConfig();
	void loadImages();

public:
	void addMessage(KviWindow *, KviNotifierMessage *);
	void closeCurrentTab();
	void closeTab(KviWindow * pWnd); // referes to void closeTab(KviWindow * pWnd, KviNotifierWindowTab * pTab);
	void closeTab(KviNotifierWindowTab * pTab); // referes to void closeTab(KviWindow * pWnd, KviNotifierWindowTab * pTab);
	void closeTab(KviWindow * pWnd, KviNotifierWindowTab * pTab);
	void draw(QPainter *);
	void needToRedraw() { m_bNeedToRedraw = true; };
	void markAllMessagesAsHistoric();
	void mouseMoveEvent(QMouseEvent * e);
	void mousePressEvent(QMouseEvent * e);
	void mouseReleaseEvent(QMouseEvent * e);
	void next();
	void prev();
	void recalculatePositions();
	void resetIcons();
	void resize(QRect);
	void scrollTabsLeft();
	void scrollTabsRight();
	void setCloseTabIcon(int state);
	void setCurrentMessage(KviNotifierMessage * m) { if(m_pTabFocused)m_pTabFocused->setCurrentMessage(m); };
	void setFocusOn(KviNotifierWindowTab *);
	void setPoint(int x, int y) { m_pnt.setX(x); m_pnt.setY(y); };
	void setWidth(int w);
	void touch() { m_bNeedToRedraw = true; };

	int baseLine() { return (y()+height()); };
	int closeTabIconState() { return m_closeTabIconState; };
	int count() { return (int)m_tabMap.count(); };
	int height() { return m_rct.height(); };
	int width() { return m_rct.width(); };
	int x() { return m_pnt.x(); };
	int y() { return m_pnt.y(); };

	QFont * fontFocused() { return m_pFocusedFont; };
	QFont * fontUnfocused() { return m_pUnfocusedFont; };

	QPixmap * tabFocusedPixSx() {return &m_pixSXFocused; };
	QPixmap * tabFocusedPixDx() {return &m_pixDXFocused; };
	QPixmap * tabUnfocusedPixSx() {return &m_pixSXUnfocused; };
	QPixmap * tabUnfocusedPixDx() {return &m_pixDXUnfocused; };

	QRect rect() {return m_rct;};
	QRect rctCloseTabIconHotArea() {return m_rctCloseTabIconHotArea; };

	KviNotifierWindowTab * currentTab() { return m_pTabFocused; };

	KviNotifierMessage * currentMessage() { return m_pTabFocused ? m_pTabFocused->currentMessage() : 0; };
	
	KviWindow * window() { return m_pTabFocused->wnd(); };



protected:

};

#endif //!_KVINOTIFIERWINDOWTABS_H_
