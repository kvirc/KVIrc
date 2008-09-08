//=============================================================================
//
//   File : kvinotifierwindowtabs.cpp
//   Created on dom 02 gen 2005 15:30:50 by Iacopo Palazzi
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2005 Iacopo Palazzi < iakko(at)siena(dot)linux(dot)it >
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#include "notifiermessage.h"
#include "notifierwindowtabs.h"

#include "kvi_app.h"
#include "kvi_config.h"
#include "kvi_qstring.h"
#include "kvi_iconmanager.h"
#include "kvi_locale.h"
#include "kvi_options.h"
#include "kvi_window.h"

#include <QPainter>
#include <QBrush>
#include <QColor>
#include <QFontMetrics>
#include <QEvent>
#include <QMouseEvent>

extern KviNotifierWindow * g_pNotifierWindow;
KviNotifierWindowTabs * g_pTabs;


// --- tab object definition class

KviNotifierWindowTab::KviNotifierWindowTab(KviWindow * pWnd, QString label)
{
	m_pWnd = pWnd;
	m_label = label;
	m_pMessageList = new KviPointerList<KviNotifierMessage>;
	m_pMessageList->setAutoDelete(true);
	m_bFocused = false;
	m_pCurrentMessage = 0;

	QString buffer;
	g_pApp->getReadOnlyConfigPath(buffer,"libkvinotifier.kvc",KviApp::ConfigPlugins,true);
	KviConfig cfg(buffer,KviConfig::Read);
	cfg.setGroup("NotifierSkin");
		m_clrHighlightedLabel = cfg.readColorEntry("HighlightedTabLablerColor",QColor(200,0,0));
		m_clrNormalLabel = cfg.readColorEntry("NormalTabLablerColor",QColor(0,0,0));
		m_clrChangedLabel = cfg.readColorEntry("ChangedTabLablerColor",QColor(0,0,100));

	if(pWnd)
	{
		connect(pWnd,SIGNAL(windowNameChanged()),this,SLOT(labelChanged()));
		connect(pWnd,SIGNAL(destroyed()),this,SLOT(closeMe()));
	}
}

KviNotifierWindowTab::~KviNotifierWindowTab()
{
	delete m_pMessageList;
}

void KviNotifierWindowTab::setNextMessageAsCurrent()
{
	if(!m_pCurrentMessage)return;
	if(m_pMessageList->findRef(m_pCurrentMessage) == -1)return;
	m_pCurrentMessage = m_pMessageList->next();
	if(!m_pCurrentMessage)m_pCurrentMessage = m_pMessageList->last();
}

void KviNotifierWindowTab::setPrevMessageAsCurrent()
{
	if(!m_pCurrentMessage)return;
	if(!m_pMessageList->findRef(m_pCurrentMessage) == -1)return;
	m_pCurrentMessage = m_pMessageList->prev();
	if(!m_pCurrentMessage)m_pCurrentMessage = m_pMessageList->first();
}

void KviNotifierWindowTab::setLastMessageAsCurrent()
{
	m_pCurrentMessage = m_pMessageList->last();
}

void KviNotifierWindowTab::appendMessage(KviNotifierMessage * m)
{
	if(m_pCurrentMessage == m_pMessageList->last())m_pCurrentMessage = m;
	m_pMessageList->append(m);
	// avoid having too much messages floating around
	while(m_pMessageList->count() > MAX_MESSAGES_IN_WINDOW)
	{
		m = m_pMessageList->first();
		m_pMessageList->removeFirst();
		if(m == m_pCurrentMessage)
			m_pCurrentMessage = m_pMessageList->first();
	}
	if(focused())setState(Normal);
	else setState(Highlighted);
}

void KviNotifierWindowTab::labelChanged()
{
	if(!m_pWnd)return;
	m_label = m_pWnd->windowName();
	setState(Changed);
	g_pTabs->needToRedraw();
	g_pNotifierWindow->update();
}

void KviNotifierWindowTab::closeMe()
{
	g_pTabs->closeTab(this);
}

void KviNotifierWindowTab::setFocused(bool b)
{
	m_bFocused = b;
	if(m_bFocused)setState(Normal);
}

void KviNotifierWindowTab::setState(TabState state)
{
	m_eState=state;
	switch (m_eState) {
		case Normal:
			m_cLabel = m_clrNormalLabel;
			break;
		case Highlighted:
			m_cLabel = m_clrHighlightedLabel;
			break;
		case Changed:
			m_cLabel = m_clrChangedLabel;
			break;
	}
};

int KviNotifierWindowTab::width(bool b) {
	int width = 0;
	QFont * font;

	QPixmap * sx;
	QPixmap * dx;

	if (focused()) {
		font = g_pTabs->fontFocused();
		sx = g_pTabs->tabFocusedPixSx();
		dx = g_pTabs->tabFocusedPixSx();
	} else {
		sx = g_pTabs->tabUnfocusedPixSx();
		dx = g_pTabs->tabUnfocusedPixSx();
		font = g_pTabs->fontUnfocused();
	}

	QFontMetrics fm(*font);
	width = fm.width(label())+2;
	if (!b) width += sx->width() + dx->width();

	return width;
}

// #################################################################################
// ### end of tab object definition class
// #################################################################################


KviNotifierWindowTabs::KviNotifierWindowTabs(QRect r)
{
	g_pTabs = this;
	m_pPixmap = new QPixmap();
	m_pPainter = new QPainter();
	m_pTabFocused = 0;
	m_pWndTabFocused = 0;
	m_iTabToStartFrom = 0;

	m_tabPtrList.setAutoDelete(false); // FIXME: should take a closer look at this
	m_lastVisitedTabPtrList.setAutoDelete(false);

	loadImages();
	initConfig();
	resize(r);
}

KviNotifierWindowTabs::~KviNotifierWindowTabs()
{
	QMap<KviWindow *, KviNotifierWindowTab *>::Iterator tab;
	for(tab = m_tabMap.begin(); tab != m_tabMap.end(); tab++ )
		delete tab.value();
	m_tabMap.clear();
	delete m_pFocusedFont;
	delete m_pUnfocusedFont;
	delete m_pPainter;
	delete m_pPixmap;
}

void KviNotifierWindowTabs::initConfig()
{
	QString buffer;
	g_pApp->getReadOnlyConfigPath(buffer,"libkvinotifier.kvc",KviApp::ConfigPlugins,true);

	KviConfig cfg(buffer,KviConfig::Read);

	cfg.setGroup("NotifierSkin");

	QString szFamily = cfg.readEntry("TextFontFocusedTab","Arial");
	m_pFocusedFont = new QFont(szFamily,cfg.readIntEntry("TextFocusedFontSize",10));
	m_pFocusedFont->setBold(true);
	szFamily = cfg.readEntry("TextFontUnfocusedTab","Arial");
	m_pUnfocusedFont = new QFont(szFamily,cfg.readIntEntry("TextUnfocusedFontSize",9));
}

void KviNotifierWindowTabs::loadImages()
{
	QPixmap * p;
	if((p = g_pIconManager->getPixmap("notifier_pix_tab_dx.png")))
		m_pixDX = *p;
	if((p = g_pIconManager->getPixmap("notifier_pix_tab_sx.png")))
		m_pixSX = *p;
	if((p = g_pIconManager->getPixmap("notifier_pix_tab_bkg.png")))
		m_pixBKG = *p;
	if((p = g_pIconManager->getPixmap("notifier_pix_tab_focused_sx.png")))
		m_pixSXFocused = *p;
	if((p = g_pIconManager->getPixmap("notifier_pix_tab_focused_dx.png")))
		m_pixDXFocused = *p;
	if((p = g_pIconManager->getPixmap("notifier_pix_tab_focused_bkg.png")))
		m_pixBKGFocused = *p;
	if((p = g_pIconManager->getPixmap("notifier_pix_tab_unfocused_sx.png")))
		m_pixSXUnfocused = *p;
	if((p = g_pIconManager->getPixmap("notifier_pix_tab_unfocused_dx.png")))
		m_pixDXUnfocused = *p;
	if((p = g_pIconManager->getPixmap("notifier_pix_tab_unfocused_bkg.png")))
		m_pixBKGUnfocused = *p;

	if((p = g_pIconManager->getPixmap("notifier_icon_tab_next_out.png")))
		m_pixIconTabNext_out = *p;
	if((p = g_pIconManager->getPixmap("notifier_icon_tab_next_over.png")))
		m_pixIconTabNext_over = *p;
	if((p = g_pIconManager->getPixmap("notifier_icon_tab_next_clicked.png")))
		m_pixIconTabNext_clicked = *p;

	m_pixIconTabNext = m_pixIconTabNext_out;

	if((p = g_pIconManager->getPixmap("notifier_icon_tab_prev_out.png")))
		m_pixIconTabPrev_out = *p;
	if((p = g_pIconManager->getPixmap("notifier_icon_tab_prev_over.png")))
		m_pixIconTabPrev_over = *p;
	if((p = g_pIconManager->getPixmap("notifier_icon_tab_prev_clicked.png")))
		m_pixIconTabPrev_clicked = *p;

	m_pixIconTabPrev = m_pixIconTabPrev_out;

	if((p = g_pIconManager->getPixmap("notifier_icon_tab_close_off.png")))
		m_pixIconCloseTab_off = *p;
	if((p = g_pIconManager->getPixmap("notifier_icon_tab_close_on.png")))
		m_pixIconCloseTab_on = *p;
	if((p = g_pIconManager->getPixmap("notifier_icon_tab_close_clicked.png")))
		m_pixIconCloseTab_clicked = *p;

	m_pixIconCloseTab = m_pixIconCloseTab_off;

	m_closeTabIconState = WDG_ICON_OFF;

}

void KviNotifierWindowTabs::addMessage(KviWindow * pWnd, KviNotifierMessage * message)
{
	QString sender = pWnd ? pWnd->windowName() : "----";

	// Qui servirebbe anche una lista... perche' quando si inserisce
	// un tab lui lo piazza dove gli pare nell'ordine
	// e quindi iterando i tab nuovi possono trovarsi PRIMA dei tab vecchi
	// il che' confonde un po
	KviNotifierWindowTab * tab;
	if (!m_tabMap.contains(pWnd)) {
		m_tabMap.insert(pWnd, tab = new KviNotifierWindowTab(pWnd, sender));
		m_tabPtrList.append(tab);
	} else {
		tab = m_tabMap[pWnd];
	}

	tab->appendMessage(message);

	if((g_pNotifierWindow->state()==Hidden) || (!m_pTabFocused))
	{
		setFocusOn(tab);
	} else {
		needToRedraw();
	}
}

void KviNotifierWindowTabs::resize(QRect r)
{
	m_rct.setX(r.x());
	m_rct.setY(r.y());
	m_rct.setHeight(r.height());
	setWidth(r.width());

	recalculatePositions();
}

void KviNotifierWindowTabs::setWidth(int w) {
	m_rct.setWidth(w);
	recalculatePositions();
}

void KviNotifierWindowTabs::recalculatePositions()
{
	m_rctCloseTabIcon.setX(m_rct.x()+m_rct.width()-m_pixIconCloseTab.width());
	m_rctCloseTabIcon.setY(m_rct.y());
	m_rctCloseTabIcon.setWidth(m_pixIconCloseTab.width());
	m_rctCloseTabIcon.setHeight(m_pixIconCloseTab.height());
	// The sensible area for the Close Icon
	m_rctCloseTabIconHotArea.setX(m_rctCloseTabIcon.x()+6);
	m_rctCloseTabIconHotArea.setY(m_rctCloseTabIcon.y()+3);
	m_rctCloseTabIconHotArea.setWidth(16);
	m_rctCloseTabIconHotArea.setHeight(16);

	if (m_bIsOverRightBound) {
		m_rctNextIcon.setX(m_rct.x() + m_rct.width() - m_rctCloseTabIcon.width() - m_pixIconTabNext.width());
		m_rctNextIcon.setY(m_rct.y());
		m_rctNextIcon.setWidth(m_pixIconTabNext.width());
		m_rctNextIcon.setHeight(m_pixIconTabNext.height());
	}

	if (m_bIsOverLeftBound) {
		m_rctPrevIcon.setX(m_rct.x());
		m_rctPrevIcon.setY(m_rct.y());
		m_rctPrevIcon.setWidth(m_pixIconTabPrev.width());
		m_rctPrevIcon.setHeight(m_pixIconTabPrev.height());
	}

	int tmpX, tmpWidth;

	tmpWidth = m_rctCloseTabIcon.x() - m_rct.x();

	if (m_bIsOverRightBound) {
		tmpWidth -= m_rctNextIcon.width();
	}

	if (m_bIsOverLeftBound) {
		tmpWidth -= m_rctPrevIcon.width();
		tmpX = m_rct.x() + m_rctPrevIcon.width();
	} else {
		tmpX = m_rct.x();
	}

	m_rctTabs.setX(tmpX);
	m_rctTabs.setY(m_rct.y());

	m_rctTabs.setWidth(tmpWidth);
	m_rctTabs.setHeight(m_rct.height());

	needToRedraw();
}

void KviNotifierWindowTabs::prev()
{
	if(!m_pTabFocused)return;

	KviNotifierWindowTab * tab;
	KviPointerListIterator<KviNotifierWindowTab> tabIterator (m_tabPtrList);

	tab = m_tabMap[m_pTabFocused->wnd()];

	tabIterator.moveFirst();

	while ((tabIterator.current()) != tab) {
		++tabIterator;
	}

	if (!tabIterator.moveFirst()) {
		--tabIterator;
		tab = tabIterator.current();
		setFocusOn(tab);
	}

}

void KviNotifierWindowTabs::next()
{
	if(!m_pTabFocused)return;

	KviNotifierWindowTab * tab;
	KviPointerListIterator<KviNotifierWindowTab> tabIterator (m_tabPtrList);

	tab = m_tabMap[m_pTabFocused->wnd()];
	tabIterator.moveFirst();
	while ((tabIterator.current()) != tab) {
		++tabIterator;
	}

	if (!tabIterator.moveLast()) {
		++tabIterator;
		tab = tabIterator.current();
		setFocusOn(tab);
	}

}

void KviNotifierWindowTabs::mousePressEvent(QMouseEvent * e)
{

	if (m_bIsOverRightBound && m_rctNextIcon.contains(e->pos())) {
		return;
	}

	if (m_bIsOverLeftBound && m_rctPrevIcon.contains(e->pos())) {
		return;
	}

	if(m_rctTabs.contains(e->pos()))
	{
		QMap<KviWindow *, KviNotifierWindowTab *>::Iterator tab;
		for (tab = m_tabMap.begin(); tab != m_tabMap.end(); tab++ ) {
			if (tab.value()->rect().contains(e->pos())) {
				setFocusOn(tab.value());
				return;
			}
		}
	}

	if (m_rctCloseTabIconHotArea.contains(e->pos())) {
		setCloseTabIcon(WDG_ICON_CLICKED);
		return;
	}

}

void KviNotifierWindowTabs::mouseMoveEvent(QMouseEvent * e)
{
	if (m_rctCloseTabIconHotArea.contains(e->pos()))
	{
		setCloseTabIcon(WDG_ICON_ON);
	} else {
		if (closeTabIconState()!=WDG_ICON_OFF)
			setCloseTabIcon(WDG_ICON_OFF);
	}
}

void KviNotifierWindowTabs::mouseReleaseEvent(QMouseEvent * e)
{
	if (m_bIsOverRightBound && m_rctNextIcon.contains(e->pos())) {
		scrollTabsLeft();
		return;
	}

	if (m_bIsOverLeftBound && m_rctPrevIcon.contains(e->pos())) {
		scrollTabsRight();
		return;
	}

	if(currentTab())
	{
		if (rctCloseTabIconHotArea().contains(e->pos()))
		{
			closeCurrentTab();
			g_pNotifierWindow->update();
		}
	}

}

void KviNotifierWindowTabs::scrollTabsLeft()
{
	if (m_bIsOverRightBound) {
		m_iTabToStartFrom++;
	}
	needToRedraw();
	g_pNotifierWindow->update();
}

void KviNotifierWindowTabs::scrollTabsRight()
{
	if (m_bIsOverLeftBound) {
		m_iTabToStartFrom--;
	}
	needToRedraw();
	g_pNotifierWindow->update();
}

void KviNotifierWindowTabs::setCloseTabIcon(int state)	{

	//if (m_writeIconState==WDG_ICON_OFF && state!=WDG_ICON_OFF)
	if (m_closeTabIconState!=state) {
		switch (state) {
			case WDG_ICON_ON: m_pixIconCloseTab = m_pixIconCloseTab_on; break;
			case WDG_ICON_OFF: m_pixIconCloseTab = m_pixIconCloseTab_off; break;
			case WDG_ICON_CLICKED: m_pixIconCloseTab = m_pixIconCloseTab_clicked; break;
		}
		needToRedraw();
		m_closeTabIconState = state;
	}
}

void KviNotifierWindowTabs::resetIcons()
{
	setCloseTabIcon(WDG_ICON_OFF);
}

void KviNotifierWindowTabs::setFocusOn(KviNotifierWindowTab * tab)
{
	if(m_pTabFocused)m_pTabFocused->setFocused(false);
	m_pTabFocused = tab;
	if(m_pTabFocused)m_pTabFocused->setFocused();

	m_lastVisitedTabPtrList.removeRef(tab);

	m_lastVisitedTabPtrList.insert(0, tab);

	needToRedraw();

	g_pNotifierWindow->update();
}

void KviNotifierWindowTabs::draw(QPainter * p)
{
	if(!m_bNeedToRedraw)return;

	*m_pPixmap = QPixmap(m_rct.size());

	m_pPainter->begin(m_pPixmap);

	QFont tmpFont;
	tmpFont = p->font();

	m_pPainter->setFont(tmpFont);

	QString str;

	bool isBigger = false;

	int offset = 0;

	int closeIcon_X = m_rct.width() - m_rctCloseTabIcon.width();
	int nextIcon_X = closeIcon_X  - m_pixIconTabNext_out.width();
	int prevIcon_X = m_rct.x();

	int tmpTabsWidth = 0;

	m_pPainter->drawPixmap(0,0,m_pixSX);
	m_pPainter->drawPixmap(m_rct.width()-m_pixDX.width(),0,m_pixDX);
	m_pPainter->drawTiledPixmap(m_pixSX.width(),0,m_rct.width()-m_pixSX.width()-m_pixDX.width(),m_rct.height(),m_pixBKG);

	KviPointerListIterator<KviNotifierWindowTab> tabIterator (m_tabPtrList);

	//m_tabPtrList.findRef(m_tabMap[m_pTabFocused->wnd()]);

//	QMap<KviWindow *, KviNotifierWindowTab *>::Iterator tab;
	KviNotifierWindowTab * tab;
	//for (tab = m_tabMap.begin(); tab != m_tabMap.end() && !isBigger; tab++ )
	tabIterator.moveFirst();

	int i = 0;
	while(m_iTabToStartFrom!=i) {
		i++;
		++tabIterator;
	}

	while ( ((tab = tabIterator.current()) != 0) && !isBigger)
	{
		++tabIterator;

		if (tab->focused()) {

			m_pPainter->setFont(*m_pFocusedFont);

			tab->setRect(m_rct.x() + offset, m_rctTabs.y(), tab->width(), m_rctTabs.height());

			m_pPainter->drawPixmap(offset,0,m_pixSXFocused);
			m_pPainter->drawTiledPixmap(offset+m_pixSXFocused.width(),0,tab->width(true),m_rctTabs.height(),m_pixBKGFocused);
			m_pPainter->drawPixmap(offset+m_pixSXFocused.width()+tab->width(true),0,m_pixDXFocused);
			QPen tmpP = m_pPainter->pen(); m_pPainter->setPen(tab->labelColor());
			m_pPainter->drawText(offset+m_pixSXFocused.width()+1,m_rctTabs.height()-NTF_TABS_FONT_BASELINE,tab->label());
			m_pPainter->setPen(tmpP);

			offset += tab->width();

		} else {

			m_pPainter->setFont(*m_pUnfocusedFont);

			tab->setRect(m_rct.x() + offset, m_rctTabs.y(), tab->width(), m_rctTabs.height());

			m_pPainter->drawPixmap(offset, 0, m_pixSXUnfocused);
			m_pPainter->drawTiledPixmap(offset+m_pixSXUnfocused.width(), 0, tab->width(true), m_rctTabs.height(), m_pixBKGUnfocused);
			m_pPainter->drawPixmap(offset+m_pixSXUnfocused.width()+tab->width(true), 0, m_pixDXUnfocused);
			QPen tmpP = m_pPainter->pen(); m_pPainter->setPen(tab->labelColor());
			m_pPainter->drawText(offset+m_pixSXUnfocused.width()+1,m_rctTabs.height()-NTF_TABS_FONT_BASELINE+1,tab->label());
			m_pPainter->setPen(tmpP);
			offset += tab->width();
		}

		if (offset > m_rctTabs.width()) isBigger = true; else isBigger = false;
	}

	if (m_iTabToStartFrom) {
		// Draw the left arrow for more tabs
		m_bIsOverLeftBound = true;
		m_pPainter->drawPixmap(0, 0, m_pixIconTabPrev);
	} else {
		m_bIsOverLeftBound = false;
	}

	if (isBigger) {
		// Draw the right arrow for more tabs
		m_bIsOverRightBound = true;
		m_pPainter->drawPixmap(nextIcon_X, 0, m_pixIconTabNext);
	} else {
		m_bIsOverRightBound = false;
	}

	m_pPainter->drawPixmap(closeIcon_X, 0, m_pixIconCloseTab);

	m_pPainter->end();

	p->drawPixmap(m_rct.x(), m_rct.y(), *m_pPixmap, 0, 0, m_pPixmap->width(), m_pPixmap->height());

	m_bNeedToRedraw = false;
}

void KviNotifierWindowTabs::markAllMessagesAsHistoric()
{
/*
	QMap<KviWindow *, KviNotifierWindowTab *>::Iterator tab;
	// Iterating every tab
	for (tab = m_tabMap.begin(); tab != m_tabMap.end(); ++tab) {

		if (!tab.value()) break;

		KviPointerList<KviNotifierMessage> * tmpMessageList = tab.value()->messageList();

		if (!tmpMessageList) break;

		for(KviNotifierMessage * m = tmpMessageList->last(); m; m = tmpMessageList->prev()) {
			if(m->historic()) break;;
			m->setHistoric();
		}
	}
*/
}

void KviNotifierWindowTabs::closeTab(KviWindow * pWnd)
{
	KviNotifierWindowTab * pTab = m_tabMap.find(pWnd).value();
	closeTab(pWnd, pTab);
}

void KviNotifierWindowTabs::closeTab(KviNotifierWindowTab * pTab)
{
	KviWindow * pWnd = pTab->wnd();
	closeTab(pWnd, pTab);
}

void KviNotifierWindowTabs::closeCurrentTab()
{
	// Paranoic checks...
	if (!m_pTabFocused) return; // The current tab is null? Please, kill me...
	KviNotifierWindowTab * pTab = m_pTabFocused;
	if (m_tabMap.isEmpty()) return; // Empty??? The World is ending...
	KviWindow * pWnd = pTab->wnd();
	if (!m_tabMap.contains(pWnd)) return; // The current tab is not on the map??? Call 911!!
	// End paranoic stuff, go back to sanity...

	closeTab(pWnd, pTab);
}

void KviNotifierWindowTabs::closeTab(KviWindow * pWnd, KviNotifierWindowTab * pTab)
{
	// Ok, we should be sure to delete the right tab...
	m_tabPtrList.removeRef(pTab);
	m_lastVisitedTabPtrList.removeRef(pTab);
	m_tabMap.remove(pWnd);
	delete pTab;

	if(m_tabMap.empty())
	{
		m_pTabFocused = 0;
		g_pNotifierWindow->showLineEdit(false);
		g_pNotifierWindow->doHide(false);
	} else {

		if (m_lastVisitedTabPtrList.count()) {
			m_pTabFocused = m_lastVisitedTabPtrList.first();
		} else {
			m_pTabFocused = m_tabPtrList.last();
		}

		m_pTabFocused->setFocused(true);
	}
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_notifierwindowtabs.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
