//
//   File : kvi_tal_application.coo
//   Creation date : Sun Aug 12 2001 04:34:21 by Szymon Stefanek
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

#define __KVILIB__


#include "kvi_tal_application.h"

#ifdef COMPILE_KDE_SUPPORT

	// The constructor below triggers a warning
	// ../tal/kvi_tal_application.cpp: In constructor 
	//   `KviTalApplication::KviTalApplication(int, char**)':
	// ../tal/kvi_tal_application.cpp:31: warning: `__base_ctor' is deprecated 
	//   (declared at /opt/kde/include/kapplication.h:198)
	//
	// The KApplication constructor has been declared as deprecated
	// in favor of a complexier initialization that uses KCmdLineArgs
	// and can't be abstracted easily.
	// The other constructors are not "old" and "stable" enough
	// to be safely used at this time. We'll rethink this when KDE 4 is out.

	KviTalApplication::KviTalApplication(int &argc,char ** argv)
	: KApplication(argc,argv,"kvirc")
	{
	

	
	}

	KviTalApplication::~KviTalApplication()
	{
	}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
	#include "kvi_tal_application_kde.moc"
#endif //COMPILE_USE_STANDALONE_MOC_SOURCES

#else

	KviTalApplication::KviTalApplication(int &argc,char ** argv)
	: QApplication(argc,argv)
	{
	}

	KviTalApplication::~KviTalApplication()
	{
	}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
	#include "kvi_tal_application_qt.moc"
#endif //COMPILE_USE_STANDALONE_MOC_SOURCES

#endif
