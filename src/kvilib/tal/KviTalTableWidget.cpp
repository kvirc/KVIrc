//=============================================================================
//
//   File : KviTalTableWidget.cpp
//   Creation date : Fri Jun 27 2008 10:00:08 by Fabio Bas
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2008 Fabio Bas (ctrlaltca at gmail dot com)
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

#include "KviTalTableWidget.h"
/*
#include "KviPointerHashTable.h"

#include <QPainter>
#include <QStyle>
#include <QHeaderView>
#include <QApplication>
#include <QHash>
#include <QStyleOption>
#include <QAccessible>
*/
KviTalTableWidget::KviTalTableWidget(QWidget * pParent)
    : QTableWidget(pParent)
{
	/*
  	connect(this,SIGNAL(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)),this,SLOT(redirect_currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)));
	connect(this,SIGNAL(itemActivated(QTreeWidgetItem *,int)),this,SLOT(redirect_itemActivated(QTreeWidgetItem *,int)));
	connect(this,SIGNAL(itemChanged(QTreeWidgetItem *,int)),this,SLOT(redirect_itemChanged(QTreeWidgetItem *,int)));
	connect(this,SIGNAL(itemClicked(QTreeWidgetItem *,int)),this,SLOT(redirect_itemClicked(QTreeWidgetItem *,int)));
	connect(this,SIGNAL(itemCollapsed(QTreeWidgetItem *)),this,SLOT(redirect_itemCollapsed(QTreeWidgetItem *)));
	connect(this,SIGNAL(itemExpanded(QTreeWidgetItem *)),this,SLOT(redirect_itemExpanded(QTreeWidgetItem *)));
	connect(this,SIGNAL(itemDoubleClicked(QTreeWidgetItem *,int)),this,SLOT(redirect_itemDoubleClicked(QTreeWidgetItem *,int)));
	connect(this,SIGNAL(itemEntered(QTreeWidgetItem *,int)),this,SLOT(redirect_itemEntered(QTreeWidgetItem *,int)));
	connect(this,SIGNAL(itemPressed(QTreeWidgetItem *,int)),this,SLOT(redirect_itemPressed(QTreeWidgetItem *,int)));
*/
}
/*
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
*/
