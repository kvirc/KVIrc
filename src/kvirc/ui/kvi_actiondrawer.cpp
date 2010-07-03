//=============================================================================
//
//   File : kvi_actiondrawer.cpp
//   Creation date : Sun 21 Nov 2004 05:44:22 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2004-2008 Szymon Stefanek <pragma at kvirc dot net>
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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================



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

KviActionDrawer::KviActionDrawer(QWidget * pParent)
: QTabWidget(pParent)
{
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
	if(!d)
		return; // ooops

	KviPointerHashTableIterator<QString,KviAction> it(*d);
	while(KviAction * pAction = it.current())
	{
		KviActionCategory * pCat = pAction->category();
		if(!pCat)
			pCat = KviActionManager::categoryGeneric();

		KviActionDrawerPage * pPage = pages.find(pCat->visibleName());
		if(!pPage)
		{
			pPage = new KviActionDrawerPage(this,pCat->description());
			pages.replace(pCat->visibleName(),pPage);
			addTab(pPage,pCat->visibleName());
			//p->show();
		}
		pPage->add(pAction);
		++it;
	}

	KviActionDrawerPage * pPage = pages.find(KviActionManager::categoryIrc()->visibleName());
	if(pPage)
	{
		int iId = indexOf(pPage);
		if(iId >= 0)
			setCurrentIndex(iId);
	}
}

KviActionDrawerPage::KviActionDrawerPage(QWidget * pParent, const QString & szDescription)
: QWidget(pParent)
{
	QGridLayout * pLayout = new QGridLayout(this);

	QString szDesc = "<b>" + szDescription + "</b>";
	QLabel * pLabel = new QLabel(szDesc,this);
	pLayout->addWidget(pLabel,0,0);

	m_pListWidget = new KviActionDrawerPageListWidget(this);

	pLayout->addWidget(m_pListWidget,1,0);

	pLayout->setRowStretch(1,1);
}

KviActionDrawerPage::~KviActionDrawerPage()
{
}

void KviActionDrawerPage::add(KviAction * pAction)
{
	(void)new KviActionDrawerPageListWidgetItem(m_pListWidget,pAction);
}

KviActionDrawerPageListWidget::KviActionDrawerPageListWidget(KviActionDrawerPage * pParent)
: KviTalListWidget(pParent)
{
	KviTalIconAndRichTextItemDelegate * pItemDelegate = new KviTalIconAndRichTextItemDelegate(this);
	setItemDelegate(pItemDelegate);
	setSelectionMode(QAbstractItemView::SingleSelection);
	setDragEnabled(true);
	setSortingEnabled(true);
}

KviActionDrawerPageListWidget::~KviActionDrawerPageListWidget()
{
}

void KviActionDrawerPageListWidget::mousePressEvent(QMouseEvent * e)
{
	KviTalListWidget::mousePressEvent(e);
	KviActionDrawerPageListWidgetItem * pItem = (KviActionDrawerPageListWidgetItem *)itemAt(e->pos());
	if(!pItem)
		return;

	QDrag * pDrag = new QDrag(this);
	QMimeData *mimeData = new QMimeData;

	mimeData->setText(pItem->name());
	pDrag->setMimeData(mimeData);

	KviAction * act = KviActionManager::instance()->getAction(pItem->name());
	if(act)
	{
		QPixmap * pixie = act->bigIcon();
		if(pixie)
		{
			pDrag->setPixmap(*pixie);
			pDrag->setHotSpot(QPoint(3,3));
		}
	}
	pDrag->exec();
}

void KviActionDrawerPageListWidget::resizeEvent(QResizeEvent * e)
{
	KviTalListWidget::resizeEvent(e);
	int iWidth = viewport()->width();
	if(iWidth < LVI_MINIMUM_CELL_WIDTH)
		iWidth = LVI_MINIMUM_CELL_WIDTH;
	//setColumnWidth(0,iWidth);
}

KviActionDrawerPageListWidgetItem::KviActionDrawerPageListWidgetItem(KviTalListWidget * pList, KviAction * pAction)
: KviTalListWidgetItem(pList)
{
	m_pListWidget = pList;
	m_szName = pAction->name();
	QString szText = "<b>" + pAction->visibleName() + "</b>";
	if(pAction->isKviUserActionNeverOverrideThis())
		szText += " <font color=\"#a0a0a0\">[" + __tr2qs("Script") + "]</font>";
	szText += "<br><font size=\"-1\">" + pAction->description() + "</font>";
	m_szKey = pAction->visibleName().toUpper();

	QPixmap * p = pAction->bigIcon();
	if(p)
		setIcon(QIcon(*p));
	setText(szText);
}

KviActionDrawerPageListWidgetItem::~KviActionDrawerPageListWidgetItem()
{
}
