//=============================================================================
//
//   File : kvi_tal_treewidget.cpp
//   Creation date : Mon Jan 22 2007 11:25:08 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2007 Szymon Stefanek (pragma at kvirc dot net)
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

#define __KVILIB__

#include "kvi_tal_treewidget.h"
#include "kvi_pointerhashtable.h"

#include <QPainter>
#include <QStyle>
#include <QHeaderView>
#include <QApplication>
#include <QHash>
#include <QStyleOption>
#include <QAccessible>
/*
static QStyleOptionQ3ListView getStyleOption(const Q3ListView *lv, const Q3ListViewItem *item)
{
	QStyleOptionQ3ListView opt;
	opt.init(lv);
	opt.subControls = QStyle::SC_None;
	opt.activeSubControls = QStyle::SC_None;
	QWidget *vp = lv->viewport();
	opt.viewportPalette = vp->palette();
	opt.viewportBGRole = vp->backgroundRole();
	opt.itemMargin = lv->itemMargin();
	opt.sortColumn = 0;
	opt.treeStepSize = lv->treeStepSize();
	opt.rootIsDecorated = lv->rootIsDecorated();
	bool firstItem = true;
	while (item) {
		QStyleOptionQ3ListViewItem lvi;
		lvi.height = item->height();
		lvi.totalHeight = item->totalHeight();
		lvi.itemY = item->itemPos();
		lvi.childCount = item->childCount();
		lvi.features = QStyleOptionQ3ListViewItem::None;
		lvi.state = QStyle::State_None;
		if (item->isEnabled())
			lvi.state |= QStyle::State_Enabled;
		if (item->isOpen())
			lvi.state |= QStyle::State_Open;
		if (item->isExpandable())
			lvi.features |= QStyleOptionQ3ListViewItem::Expandable;
		if (item->multiLinesEnabled())
			lvi.features |= QStyleOptionQ3ListViewItem::MultiLine;
		if (item->isVisible())
			lvi.features |= QStyleOptionQ3ListViewItem::Visible;
		if (item->parent() && item->parent()->rtti() == 1
			&& static_cast<Q3CheckListItem *>(item->parent())->type() == Q3CheckListItem::Controller)
			lvi.features |= QStyleOptionQ3ListViewItem::ParentControl;
		opt.items.append(lvi);
		if (!firstItem) {
			item = item->nextSibling();
		} else {
			firstItem = false;
			item = item->firstChild();
		}
	}
	return opt;
}
*/
KviTalTreeWidget::KviTalTreeWidget(QWidget * pParent)
: QTreeWidget(pParent)
{
  	connect(this,SIGNAL(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)),this,SLOT(redirect_currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)));
	connect(this,SIGNAL(itemActivated(QTreeWidgetItem *,int)),this,SLOT(redirect_itemActivated(QTreeWidgetItem *,int)));
	connect(this,SIGNAL(itemChanged(QTreeWidgetItem *,int)),this,SLOT(redirect_itemChanged(QTreeWidgetItem *,int)));
	connect(this,SIGNAL(itemClicked(QTreeWidgetItem *,int)),this,SLOT(redirect_itemClicked(QTreeWidgetItem *,int)));
	connect(this,SIGNAL(itemCollapsed(QTreeWidgetItem *)),this,SLOT(redirect_itemCollapsed(QTreeWidgetItem *)));
	connect(this,SIGNAL(itemExpanded(QTreeWidgetItem *)),this,SLOT(redirect_itemExpanded(QTreeWidgetItem *)));
	connect(this,SIGNAL(itemDoubleClicked(QTreeWidgetItem *,int)),this,SLOT(redirect_itemDoubleClicked(QTreeWidgetItem *,int)));
	connect(this,SIGNAL(itemEntered(QTreeWidgetItem *,int)),this,SLOT(redirect_itemEntered(QTreeWidgetItem *,int)));
	connect(this,SIGNAL(itemPressed(QTreeWidgetItem *,int)),this,SLOT(redirect_itemPressed(QTreeWidgetItem *,int)));
}

void KviTalTreeWidget::redirect_currentItemChanged(QTreeWidgetItem *pItemCurr,QTreeWidgetItem *pItemPrev)
{
	emit currentItemChanged((KviTalTreeWidgetItem *)pItemCurr,(KviTalTreeWidgetItem *)pItemPrev);
}

void KviTalTreeWidget::redirect_itemActivated(QTreeWidgetItem *pItem,int col)
{
	emit itemActivated((KviTalTreeWidgetItem *)pItem,col);
}

void KviTalTreeWidget::redirect_itemChanged(QTreeWidgetItem *pItem,int col)
{
	emit itemChanged((KviTalTreeWidgetItem *)pItem,col);
}


void KviTalTreeWidget::redirect_itemClicked(QTreeWidgetItem *pItem,int col)
{
	emit itemClicked((KviTalTreeWidgetItem *)pItem,col);
}

void KviTalTreeWidget::redirect_itemCollapsed(QTreeWidgetItem *pItem)
{
	emit itemCollapsed((KviTalTreeWidgetItem *)pItem);
}

void KviTalTreeWidget::redirect_itemExpanded(QTreeWidgetItem *pItem)
{
	emit itemExpanded((KviTalTreeWidgetItem *)pItem);
}

void KviTalTreeWidget::redirect_itemDoubleClicked(QTreeWidgetItem *pItem,int col)
{
	emit itemDoubleClicked((KviTalTreeWidgetItem *)pItem,col);
}

void KviTalTreeWidget::redirect_itemEntered(QTreeWidgetItem *pItem,int col)
{
	emit itemEntered((KviTalTreeWidgetItem *)pItem,col);
}
void KviTalTreeWidget::redirect_itemPressed(QTreeWidgetItem *pItem,int col)
{
	emit itemPressed((KviTalTreeWidgetItem *)pItem,col);
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
	#include "kvi_tal_treewidget.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
