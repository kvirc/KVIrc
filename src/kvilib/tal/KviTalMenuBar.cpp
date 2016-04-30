//=============================================================================
//
//   File : KviTalMenuBar.cpp
//   Creation date : Sun Aug 12 06:35:18 2001 GMT by Szymon Stefanek
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

#include "KviTalMenuBar.h"

#ifdef COMPILE_KDE4_SUPPORT

KviTalMenuBar::KviTalMenuBar(QWidget * pParent, const char * pcName)
    : KMenuBar(pParent)
{
	setWindowTitle(pcName);
}

#else //!COMPILE_KDE4_SUPPORT

KviTalMenuBar::KviTalMenuBar(QWidget * pParent, const char * pcName)
#ifdef COMPILE_ON_MAC
    // global menubar must have 0 as its parent
    : QMenuBar(0)
#else
    : QMenuBar(pParent)
#endif
{
}

#endif //!COMPILE_KDE4_SUPPORT

KviTalMenuBar::~KviTalMenuBar()
    = default;
