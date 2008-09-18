#ifndef _KVI_CUSTOMTOOLBAR_H_
#define _KVI_CUSTOMTOOLBAR_H_
//=============================================================================
//
//   File : kvi_customtoolbar.h
//   Creation date : Sun 21 Nov 2004 05:28:57 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2004-2008 Szymon Stefanek <pragma at kvirc dot net>
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
#include "kvi_toolbar.h"
#include "kvi_pointerhashtable.h"

#ifdef COMPILE_ON_WINDOWS
	#include "kvi_customtoolbardescriptor.h"
#else
	class KviCustomToolBarDescriptor;
#endif
class KviCustomToolBar;

class KviCustomToolBarSeparator : public QWidget
{
	Q_OBJECT
public:
	KviCustomToolBarSeparator(KviCustomToolBar *pParent,const char * name);
	QSize sizeHint() const;
protected:
	KviCustomToolBar * m_pToolBar;
protected:
	void paintEvent(QPaintEvent *e);
};

class KVIRC_API KviCustomToolBar : public KviToolBar
{
	friend class KviCustomToolBarDescriptor;
	Q_OBJECT
protected:
	KviCustomToolBar(KviCustomToolBarDescriptor * d,const QString &label,QT_TOOLBARDOCK_TYPE dock = QT_DOCK_TOP,bool bNewLine = false,const char * nam = 0);
public:
	~KviCustomToolBar();
protected:
	KviCustomToolBarDescriptor * m_pDescriptor;
	QWidget * m_pMovedChild;
	QWidget * m_pDraggedChild;
	KviPointerHashTable<void *,bool> * m_pFilteredChildren;
public:
	KviCustomToolBarDescriptor * descriptor(){ return m_pDescriptor; };
protected:
	virtual void mousePressEvent(QMouseEvent * e);
	virtual void dragEnterEvent(QDragEnterEvent *e);
	virtual void dragMoveEvent(QDragMoveEvent *e);
	virtual void dragLeaveEvent(QDragLeaveEvent *e);
	virtual void dropEvent(QDropEvent *e);
	virtual void childEvent(QChildEvent *e);
	virtual bool eventFilter(QObject *o,QEvent *e);
	int dropIndexAt(const QPoint &pnt,QWidget * exclude,int * excludeIdx);
	QWidget * widgetAt(int index);
	QAction * actionForWidget(QWidget * pWidget);
	void drag(QWidget * child,const QPoint &pnt);
	void filterChild(QObject * o);
	void unfilterChild(QObject * o);
	virtual void paintEvent(QPaintEvent * e);
	void syncDescriptor();
protected slots:
	void beginCustomize();
	void endCustomize();
	void filteredChildDestroyed();
};

#endif //!_KVI_CUSTOMTOOLBAR_H_
