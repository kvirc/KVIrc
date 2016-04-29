#ifndef _KVI_XLIB_H_
#define _KVI_XLIB_H_
//=============================================================================
//
//   File : KviXlib.h
//   Creation date : Tue Aug 14 18:17:21 2001 GMT by Szymon Stefanek
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

#ifdef COMPILE_X11_SUPPORT
#ifdef Bool
// Someone has defined Bool ?
#undef Bool
#endif

#ifdef None
#undef None
#endif

#include <X11/Xlib.h>

// Too bad that X11/Xlib.h defines Bool, Error and Success... this basically
// SUX since we can't use them anywhere in the source!
// this breaks, enums in Qt, enums in KVIrc and other stuff all around...
// Shame on you Xlib.h author :D

#ifdef Bool
// NO!
#undef Bool
#endif

#ifdef None
// No
#undef None
#endif

#ifdef KeyPress
#undef KeyPress
#endif

#ifdef KeyRelease
#undef KeyRelease
#endif

#ifdef FocusIn
#undef FocusIn
#endif

#ifdef FocusOut
#undef FocusOut
#endif

#ifdef FontChange
#undef FontChange
#endif

#ifdef Expose
#undef Expose
#endif

#ifdef CursorShape
#undef CursorShape
#endif

#endif //COMPILE_X11_SUPPORT

#endif //_KVI_XLIB_H_
