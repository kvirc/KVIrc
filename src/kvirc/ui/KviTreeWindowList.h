#ifndef _KVI_WINDOWLIST_TREE_H_
#define _KVI_WINDOWLIST_TREE_H_
//=============================================================================
//
//   File : KviTreeWindowList.h
//   Creation date : Thu Jan 7 1999 03:56:50 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 1999-2010 Szymon Stefanek (pragma at kvirc dot net)
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
	KviTreeWindowListItem(QTreeWidget * par, KviWindow * wnd);
	KviTreeWindowListItem(KviTreeWindowListItem * par, KviWindow * wnd);
	~KviTreeWindowListItem();

public:
	virtual QString key() const;
	void captionChanged() override;
	void highlight(int iLevel = 1) override;
	void unhighlight() override;
	void setProgress(int progress) override;
	virtual void applyOptions();

protected:
	void setActive(bool bActive);
	bool operator<(const QTreeWidgetItem & other) const
	{
		return key() < ((KviTreeWindowListItem *)&other)->key();
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
	bool isReverseSort() { return bReverseSort; };
protected:
	void mousePressEvent(QMouseEvent * e) override;
	void mouseMoveEvent(QMouseEvent * e) override;
	void wheelEvent(QWheelEvent * e) override;
	void mouseDoubleClickEvent(QMouseEvent * e) override;
	void paintEvent(QPaintEvent * event) override;

private:
	KviWindowListItem * lastItem();
	bool bReverseSort;
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
	KviTreeWindowListTreeWidget * m_pTreeWidget;
	KviTreeWindowListItem * m_pCurrentItem = nullptr;
	KviDynamicToolTip * m_pToolTip;
	QStyledItemDelegate * m_pItemDelegate;

public:
	KviWindowListItem * addItem(KviWindow *) override;
	bool removeItem(KviWindowListItem *) override;
	void setActiveItem(KviWindowListItem *) override;
	KviWindowListItem * firstItem() override;
	KviWindowListItem * nextItem(void) override;
	KviWindowListItem * lastItem() override;
	KviWindowListItem * prevItem(void) override;
	bool setIterationPointer(KviWindowListItem * it) override;
	void updatePseudoTransparency() override;
	void updateActivityMeter() override;

	void wheelEvent(QWheelEvent * e) override;

protected:
	void moveEvent(QMoveEvent *) override;
protected slots:
	void tipRequest(KviDynamicToolTip * tip, const QPoint & pnt);
};

#define KVI_TTBID_HIGHLIGHT Qt::UserRole
#define KVI_TTBID_PROGRESS Qt::UserRole + 1

class KVIRC_API KviTreeWindowListItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	KviTreeWindowListItemDelegate(QAbstractItemView * pWidget = nullptr)
	    : QStyledItemDelegate(pWidget){};
	~KviTreeWindowListItemDelegate(){};
	QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const;
	void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
};

#endif //_KVI_WINDOWLIST_TREE_H_
