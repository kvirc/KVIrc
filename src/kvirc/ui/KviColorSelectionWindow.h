#ifndef _KVI_COLORWIN_H_
#define _KVI_COLORWIN_H_
//=============================================================================
//
//   File : KviColorSelectionWindow.h
//   Creation date : Wed Jan  6 1999 04:27:45 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 1999-2010 Szymon Stefanek (pragma at kvirc dot net)
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
* \file KviColorSelectionWindow.h
* \author Szymon Stefanek
* \brief Mirc color window
*/

#include "kvi_settings.h"

#include <QWidget>

/**
* \class KviColorWindow
* \brief Mirc color window handling
*/
class KVIRC_API KviColorWindow : public QWidget
{
	Q_OBJECT
public:
	/**
	* \brief Constructs the mirc color window object
	* \return KviColorWindow
	*/
	KviColorWindow();

	/**
	* \brief Destroys the mirc color window object
	*/
	~KviColorWindow();

private:
	QWidget * m_pOwner;
	int m_iTimerId;

public:
	/**
	* \brief Popups the color window
	* \param pOwner The owner widget
	* \return void
	*/
	void popup(QWidget * pOwner);

private:
	virtual void show();
	void paintEvent(QPaintEvent * e) override;
	void keyPressEvent(QKeyEvent * e) override;
	void mousePressEvent(QMouseEvent * e) override;
	void timerEvent(QTimerEvent * e) override;
};

#endif //_KVI_COLORWIN_H_
