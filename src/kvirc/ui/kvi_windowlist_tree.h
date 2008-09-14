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

class KviTreeWindowList;

class KVIRC_API KviTreeWindowListItem : public KviTalTreeWidgetItem , public KviWindowListItem
{
	friend class KviTreeWindowList;
	friend class KviTreeWindowListTreeWidget;
public:
	KviTreeWindowListItem(KviTalTreeWidget * par,KviWindow * wnd);
	KviTreeWindowListItem(KviTreeWindowListItem * par,KviWindow * wnd);
	~KviTreeWindowListItem() {};
public:
	virtual QString key() const;
	virtual void captionChanged();
	virtual void highlight(int iLevel = 1);
	virtual void unhighlight();
protected:
	void refreshBrush();
	int calculateColor(int col1,int col2);
	QString currentCaption() const;
	bool operator<(const QTreeWidgetItem &other)const
	{
		return key() < ((KviTreeWindowListItem*)&other)->key();
	}
};

class KVIRC_API KviTreeWindowListTreeWidget : public KviTalTreeWidget
{
	friend class KviTreeWindowListItem;
	Q_OBJECT
public:
	KviTreeWindowListTreeWidget(QWidget * par);
	~KviTreeWindowListTreeWidget() {};
protected:
	virtual void mousePressEvent(QMouseEvent * e);
	virtual void paintEvent(QPaintEvent * event);
protected slots:
	void reverseSort();
	void sort();
private:
	KviWindowListItem * lastItem();
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

#endif //_KVI_WINDOWLIST_TREE_H_
