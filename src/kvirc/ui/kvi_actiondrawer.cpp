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
#include "kvi_tal_listwidget.h"

#include <QLayout>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QEvent>
#include <QMouseEvent>
#include <QTextDocument>

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
	m_szKey = a->visibleName().toUpper();

	QPixmap * p = a->bigIcon();
	setIcon(QIcon(*p));
	setText(t);
}

KviActionDrawerPageListWidgetItem::~KviActionDrawerPageListWidgetItem()
{

}
KviActionDrawerPageListWidget::KviActionDrawerPageListWidget(KviActionDrawerPage * pParent)
: KviTalListWidget(pParent)
{
	KviTalIconAndRichTextItemDelegate *itemDelegate=new KviTalIconAndRichTextItemDelegate(this);
	setItemDelegate(itemDelegate);
	setSelectionMode(QAbstractItemView::SingleSelection);
	setSortingEnabled(true);
	setMinimumHeight(400);
	setMinimumWidth(380);

	QString szPic;
	g_pApp->getGlobalKvircDirectory(szPic,KviApp::Pics);
	szPic.replace('\\',"/");

	szPic += "/kvi_actiondrawer.png";
	QString szStyle("QListWidget {background-image: url(" + szPic + ");background-repeat: no-repeat;background-position: bottom right;}");
	setStyleSheet(szStyle);
//	m_pPage = pParent;
//	setSelectionMode(Single);[{['

	//header()->hide();
//	int iWidth = viewport()->width();
//	if(iWidth < LVI_MINIMUM_CELL_WIDTH)iWidth = LVI_MINIMUM_CELL_WIDTH;
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
		if(iii >= 0)setCurrentIndex(iii);
	}
}
