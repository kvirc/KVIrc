#ifndef _KVI_ACTIONDRAWER_H_
#define _KVI_ACTIONDRAWER_H_
//=============================================================================
//
//   File : KviActionDrawer.h
//   Creation date : Sun 21 Nov 2004 05:44:22 by Szymon Stefanek
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

/**
* \file KviActionDrawer.h
* \author Szymon Stefanek
* \brief Handling of actions drawer
*/

#include "kvi_settings.h"
#include "KviTalListWidget.h"

#include <QString>
#include <QTabWidget>
#include <QWidget>

class KviAction;
class KviActionDrawerPageListWidget;
class QPixmap;

/**
* \class KviActionDrawer
* \brief Action drawer class
*/
class KVIRC_API KviActionDrawer final : public QTabWidget
{
	Q_OBJECT
public:
	/**
	* \brief Constructs an action drawer object
	* \param pParent The parent widget
	* \return KviActionDrawer
	*/
	KviActionDrawer(QWidget * pParent);

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
class KVIRC_API KviActionDrawerPage final : public QWidget
{
	friend class KviActionDrawer;
	Q_OBJECT
private:
	/**
	* \brief Constructs an action drawer page object
	* \param pParent The parent widget
	* \param szDescription The description of the page
	* \return KviActionDrawerPage
	*/
	KviActionDrawerPage(QWidget * pParent, const QString & szDescription);

private:
	KviActionDrawerPageListWidget * m_pListWidget;

private:
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
class KVIRC_API KviActionDrawerPageListWidget final : public KviTalListWidget
{
	friend class KviActionDrawerPage;
	Q_OBJECT
private:
	/**
	* \brief Constructs an action drawer page list object
	* \param pParent The parent widget
	* \return KviActionDrawerPageListWidget
	*/
	KviActionDrawerPageListWidget(KviActionDrawerPage * pParent);

private:
	void resizeEvent(QResizeEvent * e) override;
	void mousePressEvent(QMouseEvent * e) override;
};

/**
* \class KviActionDrawer
* \brief Action drawer page list item class
*/
class KVIRC_API KviActionDrawerPageListWidgetItem final : public KviTalListWidgetItem
{
public:
	/**
	* \brief Constructs an action drawer page list item object
	* \param pList The list where to add the item
	* \param pAction The action associated to the item
	* \return KviActionDrawerPageListWidget
	*/
	KviActionDrawerPageListWidgetItem(KviTalListWidget * pList, KviAction * pAction);

private:
	QString m_szName;

public:
	const QString & name() const { return m_szName; }
};

#endif //_KVI_ACTIONDRAWER_H_
