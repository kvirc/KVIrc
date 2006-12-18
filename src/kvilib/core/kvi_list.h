#ifndef _KVI_LIST_H_
#define _KVI_LIST_H_
//=================================================================================================
//
//   File : kvi_list.h
//   Creation date : Tue Jul 6 1999 14:52:20 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2006 Szymon Stefanek (pragma at kvirc dot net)
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
//=================================================================================================


#include "kvi_settings.h"

#if QT_VERSION >= 300
	#include <qptrlist.h>
	#define KviPtrList QPtrList
	#define KviPtrListBase QPtrList
	#define KviPtrListIterator QPtrListIterator
#else
	#include <qlist.h>
	#define KviPtrList QList
	#define KviPtrListBase QList
	#define KviPtrListIterator QListIterator
#endif

// BROKEN MSVC LINKER
#ifdef COMPILE_ON_WINDOWS
	#include "kvi_string.h"
	template class KVILIB_API KviPtrList<KviStr>;
#endif

#endif //_KVI_LIST_H_
