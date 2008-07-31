//
//   File : kvi_tal_menubar.cpp
//   Creation date : Sun Aug 12 06:35:18 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
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



#include "kvi_tal_menubar.h"

#ifdef COMPILE_KDE_SUPPORT

	KviTalMenuBar::KviTalMenuBar(QWidget * par,const char * nam)
	: KMenuBar(par)
	{
		setWindowTitle(nam);
	}

	KviTalMenuBar::~KviTalMenuBar()
	{
	}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
	#include "kvi_tal_menubar_kde.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES

#else

	KviTalMenuBar::KviTalMenuBar(QWidget * par,const char * nam)
	: QMenuBar(par)
	{
	}

	KviTalMenuBar::~KviTalMenuBar()
	{
	}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
	#include "kvi_tal_menubar_qt.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES

#endif
