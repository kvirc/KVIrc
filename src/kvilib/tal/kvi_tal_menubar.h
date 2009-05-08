#ifndef _KVI_TAL_MENUBAR_H_
#define _KVI_TAL_MENUBAR_H_
//=============================================================================
//
//   File : kvi_tal_menubar.h
//   Creation date : Sun Aug 12 06:35:15 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2008 Szymon Stefanek (pragma at kvirc dot net)
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
* \file kvi_tal_menubar.h
* \author Szymon Stefanek
* \brief Toolkit Abstraction Layer class
*/

#include "kvi_settings.h"

/**
* \class KviTalMenuBar
* \brief Toolkit Abstraction Layer: menubar class
*/

#ifdef COMPILE_KDE_SUPPORT

#include <kmenubar.h>

class KVILIB_API KviTalMenuBar : public KMenuBar

#else

#include <QMenuBar>

class KVILIB_API KviTalMenuBar : public QMenuBar

#endif
{
	Q_OBJECT
public:
	/**
	* \brief Constructs the menubar object
	* \param pParent The parent object
	* \param pcName The name of the menubar
	* \return KviTalMenuBar
	*/
	KviTalMenuBar(QWidget * pParent, const char * pcName);

	/**
	* \brief Destroys the menubar object
	*/
	~KviTalMenuBar();
};


#endif //_KVI_TAL_MENUBAR_H_
