//=============================================================================
//
//   File : kvi_tal_application.cpp
//   Creation date : Sun Aug 12 2001 04:34:21 by Szymon Stefanek
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================


#include "kvi_tal_application.h"

#ifdef COMPILE_KDE_SUPPORT

	KviTalApplication::KviTalApplication(int &, char **)
	: KApplication()
	{
	}

	#ifdef COMPILE_X11_SUPPORT
		KviTalApplication::KviTalApplication(Display * display, int &, char **, Qt::HANDLE visual, Qt::HANDLE colormap)
		: KApplication(display, visual, colormap)
		{
		}
	#endif
#else

	KviTalApplication::KviTalApplication(int & iArgc, char ** ppcArgv)
	: QApplication(iArgc,ppcArgv)
	{
	}

	#ifdef COMPILE_X11_SUPPORT
		KviTalApplication::KviTalApplication(Display * display, int & iArgc, char ** ppcArgv, Qt::HANDLE visual, Qt::HANDLE colormap)
		: QApplication(display, iArgc, ppcArgv, visual, colormap)
		{
		}
	#endif
#endif

KviTalApplication::~KviTalApplication()
{
}

void KviTalApplication::commitData(QSessionManager & manager)
{
	saveConfiguration();
#ifdef COMPILE_KDE_SUPPORT
	KApplication::commitData(manager);
#else
	QApplication::commitData(manager);
#endif
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
	#include "kvi_tal_application.moc"
#endif //COMPILE_USE_STANDALONE_MOC_SOURCES
