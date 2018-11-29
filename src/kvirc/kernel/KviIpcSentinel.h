#ifndef _KVI_IPC_H_
#define _KVI_IPC_H_
//=============================================================================
//
//   File : KviIpcSentinel.h
//   Creation date : Tue Apr 10 2001 15:04:45 by Szymon Stefanek
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

#include "kvi_settings.h"

#ifndef COMPILE_NO_IPC
#include "KviCString.h"
#include <QWidget>

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
# include <winsock2.h>
#elif defined(COMPILE_X11_SUPPORT) && defined(COMPILE_QX11INFO_SUPPORT)
# include "KviXlib.h" // for XEvent
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

protected: // protected members
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	virtual bool winEvent(MSG * msg, long * result);
#elif defined(COMPILE_X11_SUPPORT) && defined(COMPILE_QX11INFO_SUPPORT)
	virtual bool x11Event(XEvent * e);
	bool x11GetRemoteMessage();
#endif //!COMPILE_X11_SUPPORT

	bool nativeEvent(const QByteArray & id, void * msg, long * res) override;
};
#endif //!COMPILE_NO_IPC

#endif //_KVI_IPC_H_
