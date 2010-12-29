#ifndef _KVI_STDARG_H_
#define _KVI_STDARG_H_

//=============================================================================
//
//   File : kvi_stdarg.h
//   Creation date : Sat Jan 03 2004 02:08:14 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2004-2010 Szymon Stefanek (pragma at kvirc dot net)
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
#include <stdarg.h>

#define kvi_va_list va_list
#define kvi_va_start va_start
//
// kvi_va_start_by_reference should be used when the last known argument
// is a reference type and not a pointer
//
// int SomeClass::sprintf(const QString &fmt,...)
// {
//       kvi_va_list list;
//       kvi_va_start_by_reference(list,fmt);
//       ...
// }
//
//
#if defined(COMPILE_ON_WINDOWS) && defined(MINGW)
	#define kvi_va_start_by_reference(__list,__arg) \
	{ \
		int supercalifragilisticoespiralidoso=_INTSIZEOF(__arg); \
		__asm lea eax,__arg \
		__asm add eax,supercalifragilisticoespiralidoso \
		__asm mov __list,eax \
	}
#elif defined(__GNUC__)
	// gcc doesn't use the second argument
	// so we just fool it to avoid the warnings
	#define kvi_va_start_by_reference(__list,__arg) va_start(__list,((const char *)(&(__arg))))
#else
	#define kvi_va_start_by_reference va_start
#endif

#define kvi_va_arg va_arg
#define kvi_va_end va_end


#ifdef va_copy
	#define kvi_va_copy(a,b) va_copy(a,b)
#elif defined(__va_copy)
	#define kvi_va_copy(a,b) __va_copy(a,b)
#else
	#define kvi_va_copy(a,b) ((a) = (b))
#endif


#endif //_KVI_STDARG_H_
