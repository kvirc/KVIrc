#ifndef _KVI_IPC_H_
#define _KVI_IPC_H_
//=============================================================================
//
//   File : kvi_ipc.h
//   Creation date : Tue Apr 10 2001 15:04:45 by Szymon Stefanek
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

#include "kvi_settings.h"


#ifndef COMPILE_NO_IPC

	#include "kvi_string.h"

	#include <QWidget>

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
 #include <winsock2.h>
	#include <windows.h>
#else
	#ifdef COMPILE_X11_SUPPORT
		#include <qcoreevent.h>
		#include "kvi_xlib.h" // for XEvent
	#endif //!COMPILE_X11_SUPPORT
#endif

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
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		virtual bool winEvent(MSG * msg);
#else
	#ifdef COMPILE_X11_SUPPORT
		virtual bool x11Event(XEvent *e);
	#endif //!COMPILE_X11_SUPPORT
#endif
	};
#endif //!COMPILE_NO_IPC

#endif //_KVI_IPC_H_
