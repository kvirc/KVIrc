#ifndef _KVI_TEXTICONWIN_H_
#define _KVI_TEXTICONWIN_H_
//=============================================================================
//
//   File : KviTextIconWindow.h
//   Creation date : Fri May 17 2002 02:33:45 by Szymon Stefanek
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

#include "kvi_settings.h"
#include "KviIconManager.h"

class QTableWidget;

constexpr int KVI_TEXTICON_WIN_WIDTH = 230;
constexpr int KVI_TEXTICON_WIN_HEIGHT = 200;
constexpr int KVI_TEXTICON_COLUMNS = 6;

/**
* \class KviTextIconWindow
* \brief Text icon window class
*/
class KVIRC_API KviTextIconWindow : public QWidget
{
	Q_OBJECT
public:
	KviTextIconWindow();
	~KviTextIconWindow();

private:
	QWidget * m_pOwner = nullptr;
	QTableWidget * m_pTable;
	bool m_bAltMode = false; // in alt mode the inserted string will contains also the Alt+E escape code
public:
	/**
	* \brief Shows the popup
	* \param pOwner The owner of the widget
	* \param bAltMode Whether to prepend the Alt+E escape code in the inserted string
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
	bool eventFilter(QObject * o, QEvent * e) override;
	void autoSelectBestMatchBasedOnOwnerText();

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
