#ifndef _KVI_ACTIONDRAWER_H_
#define _KVI_ACTIONDRAWER_H_
//=============================================================================
//
//   File : kvi_actiondrawer.h
//   Created on Sun 21 Nov 2004 05:44:22 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2004 Szymon Stefanek <pragma at kvirc dot net>
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
#include "kvi_listview.h"

#include <QWidget>
#include <QTabWidget>

class KviActionDrawerPage;
class QPixmap;
#ifdef COMPILE_USE_QT4
	class Q3SimpleRichText;
#else
	class QSimpleRichText;
#endif
class KviAction;

class KVIRC_API KviActionDrawerPageListViewItem : public KviTalListViewItem
{
public:
	KviActionDrawerPageListViewItem(KviTalListView * v,KviAction * a);
	~KviActionDrawerPageListViewItem();
protected:
	QString m_szName;
#ifdef COMPILE_USE_QT4
	Q3SimpleRichText * m_pText;
#else
	QSimpleRichText * m_pText;
#endif
	QPixmap * m_pIcon;
	KviTalListView * m_pListView;
	QString m_szKey;
public:
	QPixmap * icon(){ return m_pIcon; };
	const QString & name(){ return m_szName; };
protected:
	virtual void paintCell(QPainter * p,const QColorGroup & cg,int column,int width,int align);
	virtual void setup();
	virtual QString key(int,bool) const;
};

class KVIRC_API KviActionDrawerPageListView : public KviListView
{
	friend class KviActionDrawerPage;
	Q_OBJECT
protected:
	KviActionDrawerPageListView(KviActionDrawerPage * pParent);
public:
	~KviActionDrawerPageListView();
//protected:
//	KviActionDrawerPage * m_pPage;
protected:
	virtual void resizeEvent(QResizeEvent * e);
	virtual void contentsMousePressEvent(QMouseEvent * e);
};

class KVIRC_API KviActionDrawerPage : public QWidget
{
	friend class KviActionDrawer;
	Q_OBJECT
protected:
	KviActionDrawerPage(QWidget * pParent,const QString &szDescription);
public:
	~KviActionDrawerPage();
protected:
	KviActionDrawerPageListView * m_pListView;
protected:
	void add(KviAction * a);
};

class KVIRC_API KviActionDrawer : public QTabWidget
{
	Q_OBJECT
public:
	KviActionDrawer(QWidget * pParent);
	~KviActionDrawer();
public:
	void fill();
};

#endif //!_KVI_ACTIONDRAWER_H_
