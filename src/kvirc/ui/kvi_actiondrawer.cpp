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

#include "kvi_app.h"
#include "kvi_actiondrawer.h"
#include "kvi_action.h"
#include "kvi_actionmanager.h"
#include "kvi_iconmanager.h"
#include "kvi_locale.h"
#include "kvi_draganddrop.h"
#include "kvi_tal_listwidget.h"

#include <QLayout>
#include <QLabel>
//#include <qscrollview.h>
#include <QPainter>
#include <QPixmap>
//#include <QHeaderView>
//#include <q3simplerichtext.h>
#include <QEvent>
#include <QMouseEvent>
#include <QTextDocument>

/*#define LVI_ICON_SIZE 32
#define LVI_BORDER 4
#define LVI_SPACING 8
#define LVI_MINIMUM_TEXT_WIDTH 300
#define LVI_MINIMUM_CELL_WIDTH (LVI_MINIMUM_TEXT_WIDTH + LVI_BORDER + LVI_ICON_SIZE + LVI_SPACING + LVI_BORDER)
*/
KviActionDrawerPageListWidgetItem::KviActionDrawerPageListWidgetItem(KviTalListWidget * v,KviAction * a)
: KviTalListWidgetItem(v)
{
	m_pListWidget = v;
	//setDragEnabled(true);
	m_szName = a->name();
	QString t = "<b>" + a->visibleName() + "</b>";
	if(a->isKviUserActionNeverOverrideThis())
		t += " <font color=\"#a0a0a0\">[" + __tr2qs("Script") + "]</font>";
	t += "<br><font size=\"-1\">" + a->description()+ "</font>";
	m_szKey = a->visibleName().upper();

	QPixmap * p = a->bigIcon();
	setIcon(*p);
//	m_pIcon = p ? new QPixmap(*p) : new QPixmap(LVI_ICON_SIZE,LVI_ICON_SIZE);
}

KviActionDrawerPageListWidgetItem::~KviActionDrawerPageListWidgetItem()
{
//delete m_pIcon;
//	delete m_pText;
}
/*
QString KviActionDrawerPageListWidgetItem::key(int,bool) const
{
	return m_szKey;
}

void KviActionDrawerPageListWidgetItem::setup()
{
	KviTalListViewItem::setup();
	int iWidth = m_pListWidget->visibleWidth();
	if(iWidth < LVI_MINIMUM_CELL_WIDTH)iWidth = LVI_MINIMUM_CELL_WIDTH;
	iWidth -= LVI_BORDER + LVI_ICON_SIZE + LVI_SPACING + LVI_BORDER;
	m_pText->setWidth(iWidth);
	int iHeight = m_pText->height() + (2 * LVI_BORDER);
	if(iHeight < (LVI_ICON_SIZE + (2 * LVI_BORDER)))iHeight = LVI_ICON_SIZE + (2 * LVI_BORDER);
	setHeight(iHeight);
}

void KviActionDrawerPageListWidgetItem::paintCell(QPainter * p,const QColorGroup & cg,int column,int width,int align)
{
	KviTalListViewItem::paintCell(p,cg,column,width,align);
	//p->fillRect(QRect(0,0,width,height()),isSelected() ? cg.highlight() : cg.base());
	p->drawPixmap(LVI_BORDER,LVI_BORDER,*m_pIcon);
	int afterIcon = LVI_BORDER + LVI_ICON_SIZE + LVI_SPACING;
	int www = m_pListWidget->visibleWidth() - (afterIcon + LVI_BORDER);
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
*/
KviActionDrawerPageListWidget::KviActionDrawerPageListWidget(KviActionDrawerPage * pParent)
: KviTalListWidget(pParent)
{
	/*QPixmap * p = g_pIconManager->getImage("kvi_actiondrawer.png");
	if(p)setBackgroundOverlayPixmap(p,Qt::AlignRight | Qt::AlignBottom);
*/
	KviTalIconAndRichTextItemDelegate *itemDelegate=new KviTalIconAndRichTextItemDelegate(this);
	setItemDelegate(itemDelegate);
	setSelectionMode(QAbstractItemView::SingleSelection);
	setSortingEnabled(true);
	setMinimumHeight(400);
	setMinimumWidth(380);

	QString szPic;
	g_pApp->getGlobalKvircDirectory(szPic,KviApp::Pics);

	szPic += "/kvi_actiondrawer.png";
	QString szStyle("QListWidget {background-image: url(" + szPic + ");background-repeat: no-repeat;background-position: bottom right;}");
	setStyleSheet(szStyle);

//	m_pPage = pParent;
//	setSelectionMode(Single);
	//header()->hide();
	int iWidth = viewport()->width();
	if(iWidth < LVI_MINIMUM_CELL_WIDTH)iWidth = LVI_MINIMUM_CELL_WIDTH;
//	setHeaderLabel("");
//	addColumn("",iWidth);
//	setSorting(0,true);
}

KviActionDrawerPageListWidget::~KviActionDrawerPageListWidget()
{
}


void KviActionDrawerPageListWidget::contentsMousePressEvent(QMouseEvent * e)
{
	/*
	KviListView::contentsMousePressEvent(e);
	KviActionDrawerPageListWidgetItem * i = (KviActionDrawerPageListWidgetItem *)itemAt(QPoint(5,contentsToViewport(e->pos()).y()));
	if(!i)return;
	KviTextDrag * dr = new KviTextDrag();
	dr->setText(i->name());
	if(i->icon()) dr->setImageData(i->icon());
	*/
}

void KviActionDrawerPageListWidget::resizeEvent(QResizeEvent * e)
{
	KviTalListWidget::resizeEvent(e);
	int iWidth = viewport()->width();
	if(iWidth < LVI_MINIMUM_CELL_WIDTH)iWidth = LVI_MINIMUM_CELL_WIDTH;
	//setColumnWidth(0,iWidth);
}


KviActionDrawerPage::KviActionDrawerPage(QWidget * pParent,const QString &szDescription)
: QWidget(pParent)
{
	QGridLayout * g = new QGridLayout(this);
	
	QString t = "<b>" + szDescription + "</b>";
	QLabel * l = new QLabel(t,this);
	g->addWidget(l,0,0);

	m_pListWidget = new KviActionDrawerPageListWidget(this);

	g->addWidget(m_pListWidget,1,0);

	g->setRowStretch(1,1);
}

KviActionDrawerPage::~KviActionDrawerPage()
{
}

void KviActionDrawerPage::add(KviAction * a)
{
	(void)new KviActionDrawerPageListWidgetItem(m_pListWidget,a);
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

	KviPointerHashTable<QString,KviActionDrawerPage> pages;
	pages.setAutoDelete(false);

	KviPointerHashTable<QString,KviAction> * d = KviActionManager::instance()->actions();
	if(!d)return; // ooops

	KviPointerHashTableIterator<QString,KviAction> it(*d);
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
