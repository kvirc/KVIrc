#ifndef _KVI_TAL_ICONVIEW_QT4_H_
#define _KVI_TAL_ICONVIEW_QT4_H_

//=============================================================================
//
//   File : kvi_tal_iconview_qt4.h
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
//   Inc. ,59 Temple Place - Suite 440, Boston, MA 02111-1407, USA.
//
//=============================================================================

#include "kvi_settings.h"
#include "kvi_qstring.h"

#include <q3iconview.h>

class KviTalIconViewItem;

class KVILIB_API KviTalIconView : public Q3IconView
{
	Q_OBJECT
public:
	KviTalIconView(QWidget * pParent,Qt::WFlags f = 0);
	virtual ~KviTalIconView() {};
signals:
	void selectionChanged(KviTalIconViewItem * pItem);
	void currentChanged(KviTalIconViewItem * pItem);
	void clicked(KviTalIconViewItem * pItem);
	void clicked(KviTalIconViewItem * pItem,const QPoint &pnt);
	void pressed(KviTalIconViewItem * pItem);
	void pressed(KviTalIconViewItem * pItem,const QPoint &pnt);
	void doubleClicked(KviTalIconViewItem * pItem);
	void returnPressed(KviTalIconViewItem * pItem);
	void rightButtonClicked(KviTalIconViewItem * pItem,const QPoint &pnt);
	void rightButtonPressed(KviTalIconViewItem * pItem,const QPoint &pnt);
	void mouseButtonClicked(int iButton,KviTalIconViewItem * pItem,const QPoint &pnt);
	void mouseButtonPressed(int iButton,KviTalIconViewItem * pItem,const QPoint &pnt);
	void contextMenuRequested(KviTalIconViewItem * pItem,const QPoint &pnt);
	void onItem(KviTalIconViewItem * pItem);
protected slots:
	void redirect_selectionChanged(Q3IconViewItem * pItem);
	void redirect_currentChanged(Q3IconViewItem * pItem);
	void redirect_clicked(Q3IconViewItem * pItem);
	void redirect_clicked(Q3IconViewItem * pItem,const QPoint &pnt);
	void redirect_pressed(Q3IconViewItem * pItem);
	void redirect_pressed(Q3IconViewItem * pItem,const QPoint &pnt);
	void redirect_doubleClicked(Q3IconViewItem * pItem);
	void redirect_returnPressed(Q3IconViewItem * pItem);
	void redirect_rightButtonClicked(Q3IconViewItem * pItem,const QPoint &pnt);
	void redirect_rightButtonPressed(Q3IconViewItem * pItem,const QPoint &pnt);
	void redirect_mouseButtonClicked(int iButton,Q3IconViewItem * pItem,const QPoint &pnt);
	void redirect_mouseButtonPressed(int iButton,Q3IconViewItem * pItem,const QPoint &pnt);
	void redirect_contextMenuRequested(Q3IconViewItem * pItem,const QPoint &pnt);
	void redirect_onItem(Q3IconViewItem * pItem);
public:
	KviTalIconViewItem * firstItem() const { return (KviTalIconViewItem *)Q3IconView::firstItem(); };
	KviTalIconViewItem * lastItem() const { return (KviTalIconViewItem *)Q3IconView::lastItem(); };
	KviTalIconViewItem * currentItem() const { return (KviTalIconViewItem *)Q3IconView::currentItem(); };
};

class KVILIB_API KviTalIconViewItem : public Q3IconViewItem
{
public:
	KviTalIconViewItem(KviTalIconView * parent)
	: Q3IconViewItem(parent) {};
	KviTalIconViewItem(KviTalIconView * parent,KviTalIconViewItem * after)
	: Q3IconViewItem(parent,after) {};
	KviTalIconViewItem(KviTalIconView * parent, const QString & text)
	: Q3IconViewItem(parent,text) {};
	KviTalIconViewItem(KviTalIconView * parent, KviTalIconViewItem * after, const QString & text)
	: Q3IconViewItem(parent,after,text) {};
	KviTalIconViewItem(KviTalIconView * parent, const QString & text, const QPixmap & icon)
	: Q3IconViewItem(parent,text,icon) {};
	KviTalIconViewItem(KviTalIconView * parent, KviTalIconViewItem * after, const QString & text, const QPixmap & icon)
	: Q3IconViewItem(parent,after,text,icon) {};
public:
	KviTalIconView * iconView() const { return (KviTalIconView *)Q3IconViewItem::iconView(); };
	KviTalIconViewItem * prevItem() const { return (KviTalIconViewItem *)Q3IconViewItem::prevItem(); };
	KviTalIconViewItem * nextItem() const { return (KviTalIconViewItem *)Q3IconViewItem::nextItem(); };
};


#endif // _KVI_TAL_ICONVIEW_QT4_H_
