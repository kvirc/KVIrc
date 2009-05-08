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
//   Inc. ,51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#define __KVILIB__
#include "kvi_tal_iconview.h"

#ifdef COMPILE_USE_QT4

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


	#include "kvi_tal_iconview_qt4.moc"
#else

	KviTalIconView::KviTalIconView(QWidget * pParent,Qt::WFlags f)
	: QIconView(pParent,0,f)
	{
		connect(this,SIGNAL(selectionChanged(QIconViewItem *)),this,SLOT(redirect_selectionChanged(QIconViewItem *)));
		connect(this,SIGNAL(currentChanged(QIconViewItem *)),this,SLOT(redirect_currentChanged(QIconViewItem *)));
		connect(this,SIGNAL(clicked(QIconViewItem *)),this,SLOT(redirect_clicked(QIconViewItem *)));
		connect(this,SIGNAL(clicked(QIconViewItem *,const QPoint &)),this,SLOT(redirect_clicked(QIconViewItem *,const QPoint &)));
		connect(this,SIGNAL(pressed(QIconViewItem *)),this,SLOT(redirect_pressed(QIconViewItem *)));
		connect(this,SIGNAL(pressed(QIconViewItem *,const QPoint &)),this,SLOT(redirect_pressed(QIconViewItem *,const QPoint &)));
		connect(this,SIGNAL(doubleClicked(QIconViewItem *)),this,SLOT(redirect_doubleClicked(QIconViewItem *)));
		connect(this,SIGNAL(returnPressed(QIconViewItem *)),this,SLOT(redirect_returnPressed(QIconViewItem *)));
		connect(this,SIGNAL(rightButtonClicked(QIconViewItem *,const QPoint &)),this,SLOT(redirect_rightButtonClicked(QIconViewItem *,const QPoint &)));
		connect(this,SIGNAL(rightButtonPressed(QIconViewItem *,const QPoint &)),this,SLOT(redirect_rightButtonPressed(QIconViewItem *,const QPoint &)));
		connect(this,SIGNAL(mouseButtonClicked(int,QIconViewItem *,const QPoint &)),this,SLOT(redirect_mouseButtonClicked(int,QIconViewItem *,const QPoint &)));
		connect(this,SIGNAL(mouseButtonPressed(int,QIconViewItem *,const QPoint &)),this,SLOT(redirect_mouseButtonPressed(int,QIconViewItem *,const QPoint &)));
		connect(this,SIGNAL(contextMenuRequested(QIconViewItem *,const QPoint &)),this,SLOT(redirect_contextMenuRequested(QIconViewItem *,const QPoint &)));
		connect(this,SIGNAL(onItem(QIconViewItem *)),this,SLOT(redirect_onItem(QIconViewItem *)));
	}

	void KviTalIconView::redirect_selectionChanged(QIconViewItem * pItem)
	{
		emit selectionChanged((KviTalIconViewItem *)pItem);
	}

	void KviTalIconView::redirect_currentChanged(QIconViewItem * pItem)
	{
		emit currentChanged((KviTalIconViewItem *)pItem);
	}

	void KviTalIconView::redirect_clicked(QIconViewItem * pItem)
	{
		emit clicked((KviTalIconViewItem *)pItem);
	}

	void KviTalIconView::redirect_clicked(QIconViewItem * pItem,const QPoint &pnt)
	{
		emit clicked((KviTalIconViewItem *)pItem,pnt);
	}

	void KviTalIconView::redirect_pressed(QIconViewItem * pItem)
	{
		emit pressed((KviTalIconViewItem *)pItem);
	}

	void KviTalIconView::redirect_pressed(QIconViewItem * pItem,const QPoint &pnt)
	{
		emit pressed((KviTalIconViewItem *)pItem,pnt);
	}

	void KviTalIconView::redirect_doubleClicked(QIconViewItem * pItem)
	{
		emit doubleClicked((KviTalIconViewItem *)pItem);
	}

	void KviTalIconView::redirect_returnPressed(QIconViewItem * pItem)
	{
		emit returnPressed((KviTalIconViewItem *)pItem);
	}

	void KviTalIconView::redirect_rightButtonClicked(QIconViewItem * pItem,const QPoint &pnt)
	{
		emit rightButtonClicked((KviTalIconViewItem *)pItem,pnt);
	}

	void KviTalIconView::redirect_rightButtonPressed(QIconViewItem * pItem,const QPoint &pnt)
	{
		emit rightButtonPressed((KviTalIconViewItem *)pItem,pnt);
	}

	void KviTalIconView::redirect_mouseButtonClicked(int iButton,QIconViewItem * pItem,const QPoint &pnt)
	{
		emit mouseButtonClicked(iButton,(KviTalIconViewItem *)pItem,pnt);
	}

	void KviTalIconView::redirect_mouseButtonPressed(int iButton,QIconViewItem * pItem,const QPoint &pnt)
	{
		emit mouseButtonPressed(iButton,(KviTalIconViewItem *)pItem,pnt);
	}

	void KviTalIconView::redirect_contextMenuRequested(QIconViewItem * pItem,const QPoint &pnt)
	{
		emit contextMenuRequested((KviTalIconViewItem *)pItem,pnt);
	}

	void KviTalIconView::redirect_onItem(QIconViewItem * pItem)
	{
		emit onItem((KviTalIconViewItem *)pItem);
	}


	#include "kvi_tal_iconview_qt3.moc"
#endif

