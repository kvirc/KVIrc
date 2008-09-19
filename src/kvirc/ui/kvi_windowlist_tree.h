#ifndef _KVI_WINDOWLIST_TREE_H_
#define _KVI_WINDOWLIST_TREE_H_
//=============================================================================
//
//   File : kvi_windowlist_tree.h
//   Creation date : Thu Jan 7 1999 03:56:50 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_windowlist.h"
#include "kvi_tal_treewidget.h"
#include "kvi_tal_itemdelegates.h"
#include <QAbstractItemView>

class KviTreeWindowList;
class KviTreeWindowListItemInternal;

class KVIRC_API KviTreeWindowListItem : public KviTalTreeWidgetItem , public KviWindowListItem
{
	friend class KviTreeWindowList;
	friend class KviTreeWindowListTreeWidget;
	friend class KviTreeWindowListItemInternal;
public:
	KviTreeWindowListItem(KviTalTreeWidget * par,KviWindow * wnd);
	KviTreeWindowListItem(KviTreeWindowListItem * par,KviWindow * wnd);
	~KviTreeWindowListItem();
protected:
	int m_iStepNumber;
	bool m_bIncreasing;
	QTimer* m_pAnimTimer;
	KviTreeWindowListItemInternal *m_pInternal;
public:
	virtual QString key() const;
	virtual void captionChanged();
	virtual void highlight(int iLevel = 1);
	virtual void unhighlight();
	virtual void setProgress(int progress);
	virtual void applyOptions();
protected:
	void setActive(bool bActive);
	void mouseEnter();
	void mouseLeave();
	void timerShot();
	int calculateColor(int col1,int col2);
	bool operator<(const QTreeWidgetItem &other)const
	{
		return key() < ((KviTreeWindowListItem*)&other)->key();
	}
};

class KviTreeWindowListItemInternal : public QObject
{
	Q_OBJECT
public:
	KviTreeWindowListItemInternal(KviTreeWindowListItem* pItem):m_pItem(pItem) {};
	~KviTreeWindowListItemInternal() {};
protected:
	KviTreeWindowListItem* m_pItem;
public slots:
	void timerShot() { m_pItem->timerShot();};
};

class KVIRC_API KviTreeWindowListTreeWidget : public KviTalTreeWidget
{
	friend class KviTreeWindowListItem;
	friend class KviTreeWindowListItemDelegate;
	Q_OBJECT
	KviTreeWindowListItem* m_pPrevItem;
public:
	KviTreeWindowListTreeWidget(QWidget * par);
	~KviTreeWindowListTreeWidget();
protected:
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseDoubleClickEvent(QMouseEvent * e);
	virtual void paintEvent(QPaintEvent * event);
	virtual void mouseMoveEvent (QMouseEvent * e);
	virtual void leaveEvent(QEvent *);
private:
	KviWindowListItem * lastItem();
signals:
	void leftMousePress(KviTalTreeWidgetItem * it);
	void rightMousePress(KviTalTreeWidgetItem * it);
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
	KviTreeWindowListItem * m_pCurrentItem;
	KviDynamicToolTip  * m_pToolTip;
	QAbstractItemDelegate* m_pItemDelegate;
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
	virtual void applyOptions();
protected:
	virtual void moveEvent(QMoveEvent *);
protected slots:
	void tipRequest(KviDynamicToolTip *tip,const QPoint &pnt);
};

#define KVI_TTBID_STEPNUM Qt::UserRole
#define KVI_TTBID_REDDIFF Qt::UserRole + 1
#define KVI_TTBID_GREENDIFF Qt::UserRole + 2
#define KVI_TTBID_BLUEDIFF Qt::UserRole + 3
#define KVI_TTBID_HIGHLIGHT Qt::UserRole + 4
#define KVI_TTBID_PROGRESS Qt::UserRole + 5

class KVIRC_API KviTreeWindowListItemDelegate : public KviTalIconAndRichTextItemDelegate
{
	Q_OBJECT
public:
	KviTreeWindowListItemDelegate(QAbstractItemView * pWidget=0)
		: KviTalIconAndRichTextItemDelegate(pWidget) {};
	~KviTreeWindowListItemDelegate(){};
	 QSize sizeHint(const QStyleOptionViewItem &option,const QModelIndex &index) const;
	void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
	int calculateColor(int col1,int col2, int iStepNumber) const;
};

#endif //_KVI_WINDOWLIST_TREE_H_
