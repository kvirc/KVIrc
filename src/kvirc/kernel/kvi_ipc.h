#ifndef _KVI_IPC_H_
#define _KVI_IPC_H_
//=============================================================================
//
//   File : kvi_ipc.h
//   Creation date : Tue Apr 10 2001 15:04:45 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2007 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"


#ifndef COMPILE_NO_IPC

	#include "kvi_string.h"

	#include <qwidget.h>

#ifdef COMPILE_ON_WINDOWS
 #include <winsock2.h>
	#include <windows.h>
#else //!COMPILE_ON_WINDOWS
	#ifdef COMPILE_X11_SUPPORT
		#ifdef COMPILE_USE_QT4
			// We need this to be included BEFORE X11/Xlib.h
			// because of preprocessor constant collisions...
			#include <qcoreevent.h>
		#endif
		#include "kvi_xlib.h" // for XEvent
	#endif //!COMPILE_X11_SUPPORT
#endif //!COMPILE_ON_WINDOWS

	//////////////////////////////////////////////////////////////////////
	// class KviIpcSentinel
	//
	//////////////////////////////////////////////////////////////////////

	class KVIRC_API KviIpcSentinel : public QWidget
	{
		Q_OBJECT
	public:
		KviIpcSentinel();
		~KviIpcSentinel();
	protected: // protected members
#ifdef COMPILE_ON_WINDOWS
		virtual bool winEvent(MSG * msg);
#else //!COMPILE_ON_WINDOWS
	#ifdef COMPILE_X11_SUPPORT
		virtual bool x11Event(XEvent *e);
	#endif //!COMPILE_X11_SUPPORT
#endif //!COMPILE_ON_WINDOWS
	};

#endif //!COMPILE_NO_IPC



#endif //_KVI_IPC_H_
