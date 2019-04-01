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
	KviCustomToolBar(KviCustomToolBarDescriptor * d, const QString & szLabel, Qt::ToolBarArea type = Qt::TopToolBarArea, const char * pcName = "unnamed_customtoolbar");

public:
	~KviCustomToolBar();

protected:
	KviCustomToolBarDescriptor * m_pDescriptor;
	KviPointerHashTable<void *, bool> * m_pFilteredChildren;

public:
	KviCustomToolBarDescriptor * descriptor() { return m_pDescriptor; };
protected:
	void dragEnterEvent(QDragEnterEvent * e) override;
	void dragMoveEvent(QDragMoveEvent * e) override;
	void dragLeaveEvent(QDragLeaveEvent * e) override;
	void dropEvent(QDropEvent * e) override;
	void childEvent(QChildEvent * e) override;
	bool eventFilter(QObject * o, QEvent * e) override;
	QAction * actionForWidget(QWidget * pWidget);
	void drag(QWidget * pChild, const QPoint & pnt);
	void filterChild(QObject * o);
	void unfilterChild(QObject * o);
	void paintEvent(QPaintEvent * e) override;
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
	QSize sizeHint() const override;

protected:
	void paintEvent(QPaintEvent * e) override;
};

#endif //_KVI_CUSTOMTOOLBAR_H_
