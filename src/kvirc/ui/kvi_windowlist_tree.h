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
#include <QLabel>
#include <QHBoxLayout>

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
private:
	inline void setFont(QFont& font) { m_Text.setFont(font); };
	inline void setText(QString& txt) { m_Text.setText(txt); };
	inline void setWindowIcon(QPixmap& ico) { m_Icon.setPixmap(ico); };
	inline void setContextIcon(QPixmap& ico) { m_Context.setPixmap(ico); };
	inline void setActivityIcon(QPixmap& ico) { m_Activity.setPixmap(ico); };
	inline void setForeground(QColor & col) {
		QPalette pal = m_Text.palette();
		pal.setColor(QPalette::Text, col);
		m_Text.setPalette(pal);
	}
protected:
	void refreshBrush();
	void refreshActivityIcon();
	int calculateColor(int col1,int col2);
	QString currentCaption() const;
	bool operator<(const QTreeWidgetItem &other)const
	{
		return key() < ((KviTreeWindowListItem*)&other)->key();
	}
	QLabel m_Icon;
	QLabel m_Context;
	QLabel m_Activity;
	QLabel m_Text;
	QWidget *m_pBox;
	QHBoxLayout m_Layout;
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
