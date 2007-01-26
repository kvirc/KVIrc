//=============================================================================
//
//   File : kvi_actiondrawer.cpp
//   Created on Sun 21 Nov 2004 05:44:22 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2004 Szymon Stefanek <pragma at kvirc dot net>
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

#define __KVIRC__

#include "kvi_actiondrawer.h"
#include "kvi_action.h"
#include "kvi_actionmanager.h"
#include "kvi_iconmanager.h"
#include "kvi_locale.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qscrollview.h>
#include <qheader.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qsimplerichtext.h>
#include <qdragobject.h>

#define LVI_ICON_SIZE 32
#define LVI_BORDER 4
#define LVI_SPACING 8
#define LVI_MINIMUM_TEXT_WIDTH 300
#define LVI_MINIMUM_CELL_WIDTH (LVI_MINIMUM_TEXT_WIDTH + LVI_BORDER + LVI_ICON_SIZE + LVI_SPACING + LVI_BORDER)

KviActionDrawerPageListViewItem::KviActionDrawerPageListViewItem(KviTalListView * v,KviAction * a)
: KviTalListViewItem(v,"")
{
	m_pListView = v;
	setDragEnabled(true);
	m_szName = a->name();
	QString t = "<b>" + a->visibleName() + "</b>";
	if(a->isKviUserActionNeverOverrideThis())
		t += " <font color=\"#a0a0a0\">[" + __tr2qs("Script") + "]</font>";
	t += "<br><font size=\"-1\">" + a->description()+ "</font>";
	m_szKey = a->visibleName().upper();
	m_pText = new QSimpleRichText(t,v->font());
	QPixmap * p = a->bigIcon();
	m_pIcon = p ? new QPixmap(*p) : new QPixmap(LVI_ICON_SIZE,LVI_ICON_SIZE);
}

KviActionDrawerPageListViewItem::~KviActionDrawerPageListViewItem()
{
	delete m_pIcon;
	delete m_pText;
}

QString KviActionDrawerPageListViewItem::key(int,bool) const
{
	return m_szKey;
}

void KviActionDrawerPageListViewItem::setup()
{
	KviTalListViewItem::setup();
	int iWidth = m_pListView->visibleWidth();
	if(iWidth < LVI_MINIMUM_CELL_WIDTH)iWidth = LVI_MINIMUM_CELL_WIDTH;
	iWidth -= LVI_BORDER + LVI_ICON_SIZE + LVI_SPACING + LVI_BORDER;
	m_pText->setWidth(iWidth);
	int iHeight = m_pText->height() + (2 * LVI_BORDER);
	if(iHeight < (LVI_ICON_SIZE + (2 * LVI_BORDER)))iHeight = LVI_ICON_SIZE + (2 * LVI_BORDER);
	setHeight(iHeight);
}

void KviActionDrawerPageListViewItem::paintCell(QPainter * p,const QColorGroup & cg,int column,int width,int align)
{
	KviTalListViewItem::paintCell(p,cg,column,width,align);
	//p->fillRect(QRect(0,0,width,height()),isSelected() ? cg.highlight() : cg.base());
	p->drawPixmap(LVI_BORDER,LVI_BORDER,*m_pIcon);
	int afterIcon = LVI_BORDER + LVI_ICON_SIZE + LVI_SPACING;
	int www = m_pListView->visibleWidth() - (afterIcon + LVI_BORDER);
	m_pText->setWidth(www);
	if(isSelected())
	{
		QColorGroup cg2(cg);
		cg2.setColor(QColorGroup::HighlightedText,cg.text());
		m_pText->draw(p,afterIcon,LVI_BORDER,QRect(afterIcon,LVI_BORDER,www,height() - (LVI_BORDER * 2)),cg2);
	} else {
		m_pText->draw(p,afterIcon,LVI_BORDER,QRect(afterIcon,LVI_BORDER,www,height() - (LVI_BORDER * 2)),cg);
	}
}

KviActionDrawerPageListView::KviActionDrawerPageListView(KviActionDrawerPage * pParent)
: KviListView(pParent)
{
	QPixmap * p = g_pIconManager->getImage("kvi_actiondrawer.png");
	if(p)setBackgroundOverlayPixmap(p,Qt::AlignRight | Qt::AlignBottom);

//	m_pPage = pParent;
	setSelectionMode(Single);
	header()->hide();
	int iWidth = visibleWidth();
	if(iWidth < LVI_MINIMUM_CELL_WIDTH)iWidth = LVI_MINIMUM_CELL_WIDTH;
	addColumn("",iWidth);
	setSorting(0,true);
}

KviActionDrawerPageListView::~KviActionDrawerPageListView()
{
}


void KviActionDrawerPageListView::contentsMousePressEvent(QMouseEvent * e)
{
	KviListView::contentsMousePressEvent(e);
	KviActionDrawerPageListViewItem * i = (KviActionDrawerPageListViewItem *)itemAt(QPoint(5,contentsToViewport(e->pos()).y()));
	if(!i)return;
	QTextDrag * d = new QTextDrag(i->name(),this); // does this leak memory ?
	if(i->icon())d->setPixmap(*(i->icon()),QPoint(3,3));
	d->dragCopy();
}

void KviActionDrawerPageListView::resizeEvent(QResizeEvent * e)
{
	KviListView::resizeEvent(e);
	int iWidth = visibleWidth();
	if(iWidth < LVI_MINIMUM_CELL_WIDTH)iWidth = LVI_MINIMUM_CELL_WIDTH;
	setColumnWidth(0,iWidth);
}


KviActionDrawerPage::KviActionDrawerPage(QWidget * pParent,const QString &szDescription)
: QWidget(pParent)
{
	QGridLayout * g = new QGridLayout(this,2,1,4,4);
	
	QString t = "<b>" + szDescription + "</b>";
	QLabel * l = new QLabel(t,this);
	g->addWidget(l,0,0);

	m_pListView = new KviActionDrawerPageListView(this);

	g->addWidget(m_pListView,1,0);

	g->setRowStretch(1,1);
}

KviActionDrawerPage::~KviActionDrawerPage()
{
}

void KviActionDrawerPage::add(KviAction * a)
{
	(void)new KviActionDrawerPageListViewItem(m_pListView,a);
}

KviActionDrawer::KviActionDrawer(QWidget * pParent)
: QTabWidget(pParent)
{
	setMinimumWidth(400);
	setMinimumHeight(300);
}

KviActionDrawer::~KviActionDrawer()
{
}

void KviActionDrawer::fill()
{
	KviActionManager::loadAllAvailableActions();

	KviDict<KviActionDrawerPage> pages;
	pages.setAutoDelete(false);

	KviDict<KviAction> * d = KviActionManager::instance()->actions();
	if(!d)return; // ooops

	KviDictIterator<KviAction> it(*d);
	while(KviAction * a = it.current())
	{
		KviActionCategory * c = a->category();
		if(!c)c = KviActionManager::categoryGeneric();
		KviActionDrawerPage * p = pages.find(c->visibleName());
		if(!p)
		{
			p = new KviActionDrawerPage(this,c->description());
			pages.replace(c->visibleName(),p);
			addTab(p,c->visibleName());
			//p->show();
		}
		p->add(a);
		++it;
	}

	KviActionDrawerPage * p = pages.find(KviActionManager::categoryIrc()->visibleName());
	if(p)
	{
		int iii = indexOf(p);
		if(iii >= 0)setCurrentPage(iii);
	}
}


