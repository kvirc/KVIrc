//=============================================================================
//
//   File : kvi_tal_iconview.cpp
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
#include "kvi_tal_iconview.h"

KviTalIconView::KviTalIconView(QWidget * pParent,Qt::WFlags f)
: Q3IconView(pParent,0,f)
{
	connect(this,SIGNAL(selectionChanged(Q3IconViewItem *)),this,SLOT(redirect_selectionChanged(Q3IconViewItem *)));
	connect(this,SIGNAL(currentChanged(Q3IconViewItem *)),this,SLOT(redirect_currentChanged(Q3IconViewItem *)));
	connect(this,SIGNAL(clicked(Q3IconViewItem *)),this,SLOT(redirect_clicked(Q3IconViewItem *)));
	connect(this,SIGNAL(clicked(Q3IconViewItem *,const QPoint &)),this,SLOT(redirect_clicked(Q3IconViewItem *,const QPoint &)));
	connect(this,SIGNAL(pressed(Q3IconViewItem *)),this,SLOT(redirect_pressed(Q3IconViewItem *)));
	connect(this,SIGNAL(pressed(Q3IconViewItem *,const QPoint &)),this,SLOT(redirect_pressed(Q3IconViewItem *,const QPoint &)));
	connect(this,SIGNAL(doubleClicked(Q3IconViewItem *)),this,SLOT(redirect_doubleClicked(Q3IconViewItem *)));
	connect(this,SIGNAL(returnPressed(Q3IconViewItem *)),this,SLOT(redirect_returnPressed(Q3IconViewItem *)));
	connect(this,SIGNAL(rightButtonClicked(Q3IconViewItem *,const QPoint &)),this,SLOT(redirect_rightButtonClicked(Q3IconViewItem *,const QPoint &)));
	connect(this,SIGNAL(rightButtonPressed(Q3IconViewItem *,const QPoint &)),this,SLOT(redirect_rightButtonPressed(Q3IconViewItem *,const QPoint &)));
	connect(this,SIGNAL(mouseButtonClicked(int,Q3IconViewItem *,const QPoint &)),this,SLOT(redirect_mouseButtonClicked(int,Q3IconViewItem *,const QPoint &)));
	connect(this,SIGNAL(mouseButtonPressed(int,Q3IconViewItem *,const QPoint &)),this,SLOT(redirect_mouseButtonPressed(int,Q3IconViewItem *,const QPoint &)));
	connect(this,SIGNAL(contextMenuRequested(Q3IconViewItem *,const QPoint &)),this,SLOT(redirect_contextMenuRequested(Q3IconViewItem *,const QPoint &)));
	connect(this,SIGNAL(onItem(Q3IconViewItem *)),this,SLOT(redirect_onItem(Q3IconViewItem *)));
}

void KviTalIconView::redirect_selectionChanged(Q3IconViewItem * pItem)
{
	emit selectionChanged((KviTalIconViewItem *)pItem);
}

void KviTalIconView::redirect_currentChanged(Q3IconViewItem * pItem)
{
	emit currentChanged((KviTalIconViewItem *)pItem);
}

void KviTalIconView::redirect_clicked(Q3IconViewItem * pItem)
{
	emit clicked((KviTalIconViewItem *)pItem);
}

void KviTalIconView::redirect_clicked(Q3IconViewItem * pItem,const QPoint &pnt)
{
	emit clicked((KviTalIconViewItem *)pItem,pnt);
}

void KviTalIconView::redirect_pressed(Q3IconViewItem * pItem)
{
	emit pressed((KviTalIconViewItem *)pItem);
}

void KviTalIconView::redirect_pressed(Q3IconViewItem * pItem,const QPoint &pnt)
{
	emit pressed((KviTalIconViewItem *)pItem,pnt);
}

void KviTalIconView::redirect_doubleClicked(Q3IconViewItem * pItem)
{
	emit doubleClicked((KviTalIconViewItem *)pItem);
}

void KviTalIconView::redirect_returnPressed(Q3IconViewItem * pItem)
{
	emit returnPressed((KviTalIconViewItem *)pItem);
}

void KviTalIconView::redirect_rightButtonClicked(Q3IconViewItem * pItem,const QPoint &pnt)
{
	emit rightButtonClicked((KviTalIconViewItem *)pItem,pnt);
}

void KviTalIconView::redirect_rightButtonPressed(Q3IconViewItem * pItem,const QPoint &pnt)
{
	emit rightButtonPressed((KviTalIconViewItem *)pItem,pnt);
}

void KviTalIconView::redirect_mouseButtonClicked(int iButton,Q3IconViewItem * pItem,const QPoint &pnt)
{
	emit mouseButtonClicked(iButton,(KviTalIconViewItem *)pItem,pnt);
}

void KviTalIconView::redirect_mouseButtonPressed(int iButton,Q3IconViewItem * pItem,const QPoint &pnt)
{
	emit mouseButtonPressed(iButton,(KviTalIconViewItem *)pItem,pnt);
}

void KviTalIconView::redirect_contextMenuRequested(Q3IconViewItem * pItem,const QPoint &pnt)
{
	emit contextMenuRequested((KviTalIconViewItem *)pItem,pnt);
}

void KviTalIconView::redirect_onItem(Q3IconViewItem * pItem)
{
	emit onItem((KviTalIconViewItem *)pItem);
}


#include "kvi_tal_iconview.moc"
