#ifndef _KVI_TAL_MAINWINDOW_H_
#define _KVI_TAL_MAINWINDOW_H_
//=============================================================================
//
//   File : KviTalMainWindow.h
//   Creation date : Sun Aug 12 2001 04:41:08 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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
* \file KviTalMainWindow.h
* \author Szymon Stefanek
* \brief Toolkit Abstraction Layer class
*/

#include "kvi_settings.h"

/**
* \class KviTalMainWindow
* \brief Toolkit Abstraction Layer: mainwindow class
*/

#ifdef COMPILE_KDE_SUPPORT

#include <kmainwindow.h>

class KVILIB_API KviTalMainWindow : public KMainWindow

#else //!COMPILE_KDE_SUPPORT

#include <QMainWindow>

class KVILIB_API KviTalMainWindow : public QMainWindow

#endif //!COMPILE_KDE_SUPPORT
{
	Q_OBJECT
public:
	/**
	* \brief Constructs the mainwindow object
	* \param pParent The parent object
	* \param pcName The name of the object
	* \return KviTalMainWindow
	*/
	KviTalMainWindow(QWidget * pParent, const char * pcName);

	/**
	* \brief Destroys the mainwindow object
	* \return KviTalMainWindow
	*/
	~KviTalMainWindow();
};

#endif // _KVI_TAL_MAINWINDOW_H_
