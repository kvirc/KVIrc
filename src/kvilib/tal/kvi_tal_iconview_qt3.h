#ifndef _KVI_TAL_ICONVIEW_QT3_H_
#define _KVI_TAL_ICONVIEW_QT3_H_

//=============================================================================
//
//   File : kvi_tal_iconview_qt3.h
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

#include "kvi_settings.h"
#include "kvi_qstring.h"

#include <qiconview.h>

class KviTalIconViewItem;


class KVILIB_API KviTalIconView : public QIconView
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
	void redirect_selectionChanged(QIconViewItem * pItem);
	void redirect_currentChanged(QIconViewItem * pItem);
	void redirect_clicked(QIconViewItem * pItem);
	void redirect_clicked(QIconViewItem * pItem,const QPoint &pnt);
	void redirect_pressed(QIconViewItem * pItem);
	void redirect_pressed(QIconViewItem * pItem,const QPoint &pnt);
	void redirect_doubleClicked(QIconViewItem * pItem);
	void redirect_returnPressed(QIconViewItem * pItem);
	void redirect_rightButtonClicked(QIconViewItem * pItem,const QPoint &pnt);
	void redirect_rightButtonPressed(QIconViewItem * pItem,const QPoint &pnt);
	void redirect_mouseButtonClicked(int iButton,QIconViewItem * pItem,const QPoint &pnt);
	void redirect_mouseButtonPressed(int iButton,QIconViewItem * pItem,const QPoint &pnt);
	void redirect_contextMenuRequested(QIconViewItem * pItem,const QPoint &pnt);
	void redirect_onItem(QIconViewItem * pItem);
public:
	KviTalIconViewItem * firstItem() const { return (KviTalIconViewItem *)QIconView::firstItem(); };
	KviTalIconViewItem * lastItem() const { return (KviTalIconViewItem *)QIconView::lastItem(); };
	KviTalIconViewItem * currentItem() const { return (KviTalIconViewItem *)QIconView::currentItem(); };
};

class KVILIB_API KviTalIconViewItem : public QIconViewItem
{
public:
	KviTalIconViewItem(KviTalIconView * parent)
	: QIconViewItem(parent) {};
	KviTalIconViewItem(KviTalIconView * parent,KviTalIconViewItem * after)
	: QIconViewItem(parent,after) {};
	KviTalIconViewItem(KviTalIconView * parent, const QString & text)
	: QIconViewItem(parent,text) {};
	KviTalIconViewItem(KviTalIconView * parent, KviTalIconViewItem * after, const QString & text)
	: QIconViewItem(parent,after,text) {};
	KviTalIconViewItem(KviTalIconView * parent, const QString & text, const QPixmap & icon)
	: QIconViewItem(parent,text,icon) {};
	KviTalIconViewItem(KviTalIconView * parent, KviTalIconViewItem * after, const QString & text, const QPixmap & icon)
	: QIconViewItem(parent,after,text,icon) {};
public:
	KviTalIconView * iconView() const { return (KviTalIconView *)QIconViewItem::iconView(); };
	KviTalIconViewItem * prevItem() const { return (KviTalIconViewItem *)QIconViewItem::prevItem(); };
	KviTalIconViewItem * nextItem() const { return (KviTalIconViewItem *)QIconViewItem::nextItem(); };
};


#endif // _KVI_TAL_ICONVIEW_QT3_H_
