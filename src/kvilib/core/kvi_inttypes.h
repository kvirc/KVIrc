#ifndef _KVI_INTTYPES_H_
#define _KVI_INTTYPES_H_
//=============================================================================
//
//   File : kvi_inttypes.h
//   Creation date : Wed Sep  4 22:28:00 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

// backward compatibility
#ifndef SYSTEM_SIZE_OF_SHORT_INT
#define SYSTEM_SIZE_OF_SHORT_INT SIZEOF_SHORT_INT
#define SYSTEM_SIZE_OF_INT SIZEOF_INT
#define SYSTEM_SIZE_OF_LONG_INT SIZEOF_LONG_INT
#define SYSTEM_SIZE_OF_LONG_LONG_INT SIZEOF_LONG_LONG_INT
#endif //!SYSTEM_SIZE_OF_SHORT_INT

#ifdef COMPILE_ON_WINDOWS
// we don't have a configure script here
// so we can't check the size of types
// We rely on the ms specific definitions then
typedef __int64 kvi_i64_t;
typedef unsigned __int64 kvi_u64_t;
typedef int kvi_i32_t;
typedef unsigned int kvi_u32_t;
typedef short int kvi_i16_t;
typedef short unsigned int kvi_u16_t;
typedef char kvi_i8_t;
typedef unsigned char kvi_u8_t;
#else
#if SYSTEM_SIZE_OF_LONG_INT == 8
// the most common case on 64 bit machines
typedef long int kvi_i64_t;
typedef unsigned long int kvi_u64_t;
#elif SYSTEM_SIZE_OF_INT == 8
// 64 bit ints ?.. a Cray ? :D
typedef int kvi_i64_t;
typedef unsigned int kvi_u64_t;
#elif SYSTEM_SIZE_OF_LONG_LONG_INT == 8
// the most common case on 32 bit machines
typedef long long int kvi_i64_t;
typedef unsigned long long int kvi_u64_t;
#else
// attempt to live without a 64bit integer type anyway...
// dunno if it will work tough...
typedef long long int kvi_i64_t;
typedef unsigned long long int kvi_u64_t;
#endif

#if SYSTEM_SIZE_OF_INT == 4
// the most common case
typedef int kvi_i32_t;
typedef unsigned int kvi_u32_t;
#elif SYSTEM_SIZE_OF_SHORT_INT == 4
// 32 bit shorts ?.. a Cray ? :D
typedef short int kvi_i32_t;
typedef short unsigned int kvi_u32_t;
#elif SYSTEM_SIZE_OF_LONG_INT == 4
typedef long int kvi_i32_t;
typedef unsigned long int kvi_u32_t;
#else
#error "Can't find a 32-bit integral type on this system"
#error "Please report to pragma at kvirc dot net"
#endif

#if SYSTEM_SIZE_OF_SHORT_INT == 2
// the most common case
typedef short int kvi_i16_t;
typedef short unsigned int kvi_u16_t;
#elif SYSTEM_SIZE_OF_INT == 2
// this isn't going to work anyway, I think..
typedef int kvi_i16_t;
typedef long int kvi_u16_t;
#else
#error "Can't find a 16-bit integral type on this system"
#error "Please report to pragma at kvirc dot net"
#endif

// assume that char is always 8 bit
typedef char kvi_i8_t;
typedef unsigned char kvi_u8_t;
#endif

#endif //_KVI_INTTYPES_H_
