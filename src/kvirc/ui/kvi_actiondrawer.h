#ifndef _KVI_ACTIONDRAWER_H_
#define _KVI_ACTIONDRAWER_H_
//=============================================================================
//
//   File : kvi_actiondrawer.h
//   Creation date : Sun 21 Nov 2004 05:44:22 by Szymon Stefanek
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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

/**
* \file kvi_actiondrawer.h
* \author Szymon Stefanek
* \brief Handling of actions drawer
*/

#include "kvi_settings.h"
#include "kvi_qstring.h"
#include "kvi_tal_listwidget.h"

#include <QWidget>
#include <QTabWidget>

class QPixmap;
class KviAction;
class KviActionDrawerPageListWidget;

/**
* \class KviActionDrawer
* \brief Action drawer class
*/
class KVIRC_API KviActionDrawer : public QTabWidget
{
	Q_OBJECT
public:
	/**
	* \brief Constructs an action drawer object
	* \param pParent The parent widget
	* \return KviActionDrawer
	*/
	KviActionDrawer(QWidget * pParent);

	/**
	* \brief Destroys an action drawer object
	*/
	~KviActionDrawer();
public:
	/**
	* \brief Fills in the drawer page
	* \return void
	*/
	void fill();
};

/**
* \class KviActionDrawer
* \brief Action drawer page class
*/
class KVIRC_API KviActionDrawerPage : public QWidget
{
	friend class KviActionDrawer;
	Q_OBJECT
protected:
	/**
	* \brief Constructs an action drawer page object
	* \param pParent The parent widget
	* \param szDescription The description of the page
	* \return KviActionDrawerPage
	*/
	KviActionDrawerPage(QWidget * pParent, const QString & szDescription);
public:
	/**
	* \brief Destroys an action drawer page object
	*/
	~KviActionDrawerPage();
protected:
	KviActionDrawerPageListWidget * m_pListWidget;
protected:
	/**
	* \brief Adds an action
	* \param pAction The pointer to the action to add
	* \return void
	*/
	void add(KviAction * pAction);
};

/**
* \class KviActionDrawer
* \brief Action drawer page list class
*/
class KVIRC_API KviActionDrawerPageListWidget : public KviTalListWidget
{
	friend class KviActionDrawerPage;
	Q_OBJECT
protected:
	/**
	* \brief Constructs an action drawer page list object
	* \param pParent The parent widget
	* \return KviActionDrawerPageListWidget
	*/
	KviActionDrawerPageListWidget(KviActionDrawerPage * pParent);
public:
	/**
	* \brief Destroys an action drawer page list object
	*/
	~KviActionDrawerPageListWidget();
protected:
	virtual void resizeEvent(QResizeEvent * e);
	virtual void mousePressEvent(QMouseEvent * e);
};

/**
* \class KviActionDrawer
* \brief Action drawer page list item class
*/
class KVIRC_API KviActionDrawerPageListWidgetItem : public KviTalListWidgetItem
{
public:
	/**
	* \brief Constructs an action drawer page list item object
	* \param pList The list where to add the item
	* \param pAction The action associated to the item
	* \return KviActionDrawerPageListWidget
	*/
	KviActionDrawerPageListWidgetItem(KviTalListWidget * pList, KviAction * pAction);

	/**
	* \brief Destroys an action drawer page list item object
	*/
	~KviActionDrawerPageListWidgetItem();
protected:
	QString            m_szName;
	QPixmap          * m_pIcon;
	KviTalListWidget * m_pListWidget;
	QString            m_szKey;

public:
	QPixmap * icon(){ return m_pIcon; };
	const QString & name(){ return m_szName; };
/*
protected:
	virtual void paintCell(QPainter * p,const QColorGroup & cg,int column,int width,int align);
	virtual void setup();
	virtual QString key(int,bool) const;
*/
};

#endif //_KVI_ACTIONDRAWER_H_
