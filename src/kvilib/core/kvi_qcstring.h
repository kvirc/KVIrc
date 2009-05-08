#ifndef _KVI_QCSTRING_H_
#define _KVI_QCSTRING_H_

//=============================================================================
//
//   File : kvi_qcstring.h
//   Creation date : Thu Jan 18 2007 00:34:33 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2007 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include "kvi_settings.h"

#ifdef COMPILE_USE_QT4
	#include <q3cstring.h> // includes <qbytearray.h>
	#define KviQCString QByteArray
#else
	// this is dead in Qt 4.x
	#include <qcstring.h>
	#define KviQCString QCString
#endif

#endif //!_KVI_QCSTRING_H_
