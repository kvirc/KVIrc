#ifndef _KVI_TEXTICONWIN_H_
#define _KVI_TEXTICONWIN_H_
//=============================================================================
//
//   File : KviTextIconWindow.h
//   Creation date : Fri May 17 2002 02:33:45 by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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
* \file KviTextIconWindow.h
* \author Szymon Stefanek
* \brief Text icon window
*
* \def KVI_TEXTICON_WIN_WIDTH The width of the window
* \def KVI_TEXTICON_WIN_HEIGHT The height of the window
*/

#include "kvi_settings.h"
#include "KviCString.h"
#include "KviIconManager.h"

#include <QPainter>
#include <QTableWidget>

#define KVI_TEXTICON_WIN_WIDTH 230
#define KVI_TEXTICON_WIN_HEIGHT 200
#define KVI_TEXTICON_COLUMNS 6
/**
* \class KviTextIconWindow
* \brief Text icon window class
*/
class KVIRC_API KviTextIconWindow : public QWidget
{
	Q_OBJECT
public:
	/**
	* \brief Constructs the text icon window objet
	* \return KviTextIconWindow
	*/
	KviTextIconWindow();

	/**
	* \brief Destroys the text icon window objet
	*/
	~KviTextIconWindow();
private:
	QWidget            * m_pOwner;
	QTableWidget       * m_pTable;
	bool                 m_bAltMode; // in alt mode the inserted string will contains also the CTRL+I escape code
public:
	/**
	* \brief Shows the popup
	* \param pOwner The owner of the widget
	* \param bAltMode Whether to prepend the CTRL+I escape code in the inserted string
	* \return void
	*/
	void popup(QWidget * pOwner, bool bAltMode);
private:
	/**
	* \brief Hides the widget and set the focus to the owner
	* \return void
	*/
	void doHide();
private:
	virtual void keyPressEvent(QKeyEvent * e);
public slots:
	/**
	* \brief Fills in the text icon window
	* \return void
	*/
	void fill();

	/**
	* \brief Called when the owner is destroyed. Then it calls doHide()
	* \return void
	*/
	void ownerDead();

	/**
	* \brief Called when a cell is selected
	* \param row The row containing selected item
	* \param column The column containing selected item
	* \return void
	*/
	void cellSelected(int row, int column);
};

#endif //_KVI_TEXTICONWIN_H_
