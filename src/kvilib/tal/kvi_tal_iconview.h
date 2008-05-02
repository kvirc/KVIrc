#ifndef _KVI_TAL_ICONVIEW_H_
#define _KVI_TAL_ICONVIEW_H_

//=============================================================================
//
//   File : kvi_tal_iconview.h
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

#include <QTableWidget>
#include <QItemDelegate>

class KviTalIconViewItem;
class KviTalIconViewItemDelegate;
class KVILIB_API KviTalIconView : public QTableWidget
{
	Q_OBJECT
public:
	KviTalIconView(QWidget * pParent,Qt::WFlags f = 0);
	void setPixmap(QPixmap *pix);

	virtual ~KviTalIconView() {};
protected:
	KviTalIconViewItemDelegate *m_pDelegate;
signals:
	void cellActivated(KviTalIconViewItem * pItem);
	void currentItemChanged(KviTalIconViewItem *pItem,KviTalIconViewItem *prev);
protected slots:
	void redirect_cellActivated(int,int);
	void redirect_currentItemChanged(QTableWidgetItem * pItem,QTableWidgetItem * prev);
};

class KVILIB_API KviTalIconViewItem : public QTableWidgetItem
{
public:
	KviTalIconViewItem(QString text,const QIcon & icon)
	: QTableWidgetItem(icon,text) {setSizeHint(QSize(30,20));};
protected:
	QItemDelegate m_pDelegate; 
};

class KviTalIconViewItemDelegate : public QItemDelegate
{
protected:
	QTableWidget * m_pTableWidget;
public:
	KviTalIconViewItemDelegate(QTableWidget * pTableWidget)
		: QItemDelegate(pTableWidget), m_pTableWidget(pTableWidget){};
	~KviTalIconViewItemDelegate(){};
//	 QSize sizeHint(const QStyleOptionViewItem &option,const QModelIndex &index) const;
//	 void drawDisplay ( QPainter * painter, const QStyleOptionViewItem & option, const QRect & rect, const QString & text ) const;
	 void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
	};
#endif // _KVI_TAL_ICONVIEW_H_
