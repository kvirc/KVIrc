#ifndef _KVI_HISTORYWIN_H_
#define _KVI_HISTORYWIN_H_
//=============================================================================
//
//   File : KviHistoryWindow.h
//   Creation date : Mon Aug 19 01:34:46 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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
* \file KviHistoryWindow.h
* \author Szymon Stefanek
* \brief History window management
*
* \def KVI_HISTORY_WIN_HEIGHT The height of the history window
*/

#include "kvi_settings.h"

#include <QListWidget>

class KviInput;

#define KVI_HISTORY_WIN_HEIGHT 130

/**
* \class KviHistoryWindow
* \brief History window class
*/
class KVIRC_API KviHistoryWindow : public QListWidget
{
	Q_OBJECT
public:
	/**
	* \brief Constructs the history window object
	* \param pParent The parent widget
	* \return KviHistoryWindow
	*/
	KviHistoryWindow(QWidget * pParent);

	/**
	* \brief Destroys the history window object
	*/
	~KviHistoryWindow();

private:
	KviInput * m_pOwner;
	int m_iTimerId;
	QWidget * m_pParent;

public:
	/**
	* \brief Shows the popup
	* \param pOwner The owner of the popup
	* \return void
	*/
	void popup(KviInput * pOwner);

	//KviInput * owner(){ return m_pOwner; };

	/**
	* \brief Hides the parent and set the focus to the owner
	* \return void
	*/
	void doHide();

private:
	/**
	* \brief Shows the widget
	* \return void
	*/
	virtual void show();

	/**
	* \brief Fills in the window with strings from input history
	* \return void
	*/
	void fill();

	//bool findTypedSeq(); // returns true if it is a complete word
	void keyPressEvent(QKeyEvent * e) override;
	void mousePressEvent(QMouseEvent * e) override;
	void timerEvent(QTimerEvent * e) override;
	void hideEvent(QHideEvent * e) override;
public slots:
	/**
	* \brief Called when the owner is dead
	* \return void
	*/
	void ownerDead();

	/**
	* \brief Called when an item is selected
	* \param pItem The selected item
	* \return void
	*/
	void itemSelected(QListWidgetItem * pItem);
};

/**
* \class KviHistoryWindowWidget
* \brief History window widget class
*/
class KVIRC_API KviHistoryWindowWidget : public QWidget
{
	Q_OBJECT
public:
	KviHistoryWindow * m_pWindow;

public:
	/**
	* \brief Constructs the history window widget object
	* \return KviHistoryWindowWidget
	*/
	KviHistoryWindowWidget();

	/**
	* \brief Destroys the history window widget object
	*/
	~KviHistoryWindowWidget();

public:
	/**
	* \brief Shows the popup
	* \param pOwner The owner of the popup
	* \return void
	*/
	void popup(KviInput * pOwner);
};

#endif //_KVI_HISTORYWIN_H_
