#ifndef _KVI_CUSTOMTOOLBAR_H_
#define _KVI_CUSTOMTOOLBAR_H_
//=============================================================================
//
//   File : KviCustomToolBar.h
//   Creation date : Sun 21 Nov 2004 05:28:57 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2004-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "kvi_settings.h"
#include "KviToolBar.h"
#include "KviPointerHashTable.h"

#ifdef COMPILE_ON_WINDOWS
	#include "KviCustomToolBarDescriptor.h"
#else
	class KviCustomToolBarDescriptor;
#endif

class KVIRC_API KviCustomToolBar : public KviToolBar
{
	friend class KviCustomToolBarDescriptor;
	Q_OBJECT
protected:
	KviCustomToolBar(KviCustomToolBarDescriptor * d, const QString & szLabel, Qt::ToolBarArea type = Qt::TopToolBarArea, bool bNewLine = false, const char * pcName = 0);
public:
	~KviCustomToolBar();
protected:
	KviCustomToolBarDescriptor       * m_pDescriptor;
	QWidget                          * m_pMovedChild;
	QAction                          * m_pMovedAction;
	QWidget                          * m_pDraggedChild;
	KviPointerHashTable<void *,bool> * m_pFilteredChildren;
public:
	KviCustomToolBarDescriptor * descriptor(){ return m_pDescriptor; };
protected:
	virtual void mousePressEvent(QMouseEvent * e);
	virtual void dragEnterEvent(QDragEnterEvent * e);
	virtual void dragMoveEvent(QDragMoveEvent * e);
	virtual void dragLeaveEvent(QDragLeaveEvent * e);
	virtual void dropEvent(QDropEvent * e);
	virtual void childEvent(QChildEvent * e);
	virtual bool eventFilter(QObject * o, QEvent * e);
	int dropIndexAt(const QPoint & pnt, QWidget * pExclude, int * piExcludeIdx);
	QWidget * widgetAt(int iIndex);
	QAction * actionForWidget(QWidget * pWidget);
	void drag(QWidget * pChild, const QPoint & pnt);
	void filterChild(QObject * o);
	void unfilterChild(QObject * o);
	virtual void paintEvent(QPaintEvent * e);
	void syncDescriptor();
protected slots:
	void beginCustomize();
	void endCustomize();
	void filteredChildDestroyed();
};

class KviCustomToolBarSeparator : public QWidget
{
	Q_OBJECT
public:
	KviCustomToolBarSeparator(KviCustomToolBar * pParent, const char * pcName);
protected:
	KviCustomToolBar * m_pToolBar;
public:
	QSize sizeHint() const;
protected:
	void paintEvent(QPaintEvent * e);
};

#endif //_KVI_CUSTOMTOOLBAR_H_
