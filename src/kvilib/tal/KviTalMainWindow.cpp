//=============================================================================
//
//   File : KviTalMainWindow.coo
//   Creation date : Sun Aug 12 2001 04:40:24 by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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


#include "KviTalMainWindow.h"

#ifdef COMPILE_KDE_SUPPORT

	KviTalMainWindow::KviTalMainWindow(QWidget * pParent, const char * pcName)
	: KMainWindow(pParent)
	{
		//setWindowTitle(pcName);
		setObjectName(pcName);
	}

#else
	KviTalMainWindow::KviTalMainWindow(QWidget * pParent, const char * pcName)
	: QMainWindow(pParent)
	{
		setObjectName(pcName);
	}

#endif

KviTalMainWindow::~KviTalMainWindow()
{
}

bool KviTalMainWindow::usesBigPixmaps()
{
	return (iconSize().width() > 30);
}

void KviTalMainWindow::setUsesBigPixmaps(bool bUse)
{
	if(bUse)
		setIconSize(QSize(32,32));
	else
		setIconSize(QSize(22,22));
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
	#include "KviTalMainWindow.moc"
#endif //COMPILE_USE_STANDALONE_MOC_SOURCES
