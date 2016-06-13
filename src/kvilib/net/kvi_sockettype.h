#ifndef _KVI_SOCKETTYPE_H_
#define _KVI_SOCKETTYPE_H_
//=============================================================================
//
//   File : kvi_sockettype.h
//   Creation date : Thu Sep 20 05:41:46 2001 GMT by Szymon Stefanek
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

//
// Socket stuff abstraction layer
//

#include "kvi_settings.h"

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
#include <winsock2.h>
#if defined(COMPILE_ON_WINDOWS)
#include <ws2ipdef.h>
#endif
typedef SOCKET kvi_socket_t;
#else
typedef int kvi_socket_t;
#endif

#endif //_KVI_SOCKETTYPE_H_
