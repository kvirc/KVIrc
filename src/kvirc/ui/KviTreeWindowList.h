#ifndef _KVI_WINDOWLIST_TREE_H_
#define _KVI_WINDOWLIST_TREE_H_
//=============================================================================
//
//   File : KviTreeWindowList.h
//   Creation date : Thu Jan 7 1999 03:56:50 by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 1999-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviWindowListBase.h"
#include <QTreeWidget>
#include <QStyledItemDelegate>
#include <QAbstractItemView>

class KviTreeWindowList;
class KviTreeWindowListItemInternal;

class KVIRC_API KviTreeWindowListItem : public QTreeWidgetItem, public KviWindowListItem
{
	friend class KviTreeWindowList;
	friend class KviTreeWindowListTreeWidget;
	friend class KviTreeWindowListItemInternal;
public:
	KviTreeWindowListItem(QTreeWidget * par,KviWindow * wnd);
	KviTreeWindowListItem(KviTreeWindowListItem * par,KviWindow * wnd);
	~KviTreeWindowListItem();
public:
	virtual QString key() const;
	virtual void captionChanged();
	virtual void highlight(int iLevel = 1);
	virtual void unhighlight();
	virtual void setProgress(int progress);
	virtual void applyOptions();
protected:
	void setActive(bool bActive);
	bool operator<(const QTreeWidgetItem &other)const
	{
		return key() < ((KviTreeWindowListItem*)&other)->key();
	}
};

class KVIRC_API KviTreeWindowListTreeWidget : public QTreeWidget
{
	friend class KviTreeWindowListItem;
	friend class KviTreeWindowListItemDelegate;
	Q_OBJECT
public:
	KviTreeWindowListTreeWidget(QWidget * par);
	~KviTreeWindowListTreeWidget();
protected:
	virtual void mousePressEvent(QMouseEvent * e);
	virtual void mouseMoveEvent(QMouseEvent * e);
	virtual void wheelEvent(QWheelEvent * e);
	virtual void mouseDoubleClickEvent(QMouseEvent * e);
	virtual void paintEvent(QPaintEvent * event);
private:
	KviWindowListItem * lastItem();
public slots:
	void sort();
	void reverseSort();
};

class KVIRC_API KviTreeWindowList : public KviWindowListBase
{
	Q_OBJECT
public:
	KviTreeWindowList();
	~KviTreeWindowList();
private:
	KviTreeWindowListTreeWidget   * m_pTreeWidget;
	KviTreeWindowListItem         * m_pCurrentItem;
	KviDynamicToolTip             * m_pToolTip;
	QStyledItemDelegate           * m_pItemDelegate;
public:
	virtual KviWindowListItem * addItem(KviWindow *);
	virtual bool removeItem(KviWindowListItem *);
	virtual void setActiveItem(KviWindowListItem *);
	virtual KviWindowListItem * firstItem();
	virtual KviWindowListItem * nextItem(void);
	virtual KviWindowListItem * lastItem();
	virtual KviWindowListItem * prevItem(void);
	virtual bool setIterationPointer(KviWindowListItem * it);
	virtual void updatePseudoTransparency();
	virtual void updateActivityMeter();
protected:
	virtual void moveEvent(QMoveEvent *);
protected slots:
	void tipRequest(KviDynamicToolTip *tip,const QPoint &pnt);
};

#define KVI_TTBID_HIGHLIGHT Qt::UserRole
#define KVI_TTBID_PROGRESS Qt::UserRole + 1

class KVIRC_API KviTreeWindowListItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	KviTreeWindowListItemDelegate(QAbstractItemView * pWidget=0)
		: QStyledItemDelegate(pWidget) {};
	~KviTreeWindowListItemDelegate(){};
	QSize sizeHint(const QStyleOptionViewItem &option,const QModelIndex &index) const;
	void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
};

#endif //_KVI_WINDOWLIST_TREE_H_
