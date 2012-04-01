//=============================================================================
//
//   File : KviTalApplication.cpp
//   Creation date : Sun Aug 12 2001 04:34:21 by Szymon Stefanek
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


#include "KviTalApplication.h"

#ifdef COMPILE_KDE_SUPPORT
	KviTalApplication::KviTalApplication(int &, char **)
	: KApplication()
	{
	}
#else
	KviTalApplication::KviTalApplication(int & iArgc, char ** ppcArgv)
	: QApplication(iArgc,ppcArgv)
	{
	}
#endif

KviTalApplication::~KviTalApplication()
{
}

void KviTalApplication::commitData(QSessionManager & manager)
{
	saveConfiguration();
#ifdef COMPILE_KDE_SUPPORT
	KApplication::commitData(manager);
#endif
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
	#include "KviTalApplication.moc"
#endif //COMPILE_USE_STANDALONE_MOC_SOURCES
