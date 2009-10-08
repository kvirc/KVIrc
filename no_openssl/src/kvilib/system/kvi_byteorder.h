#ifndef _KVI_BYTEORDER_H_
#define _KVI_BYTEORDER_H_

//=============================================================================
//
//   File : kvi_byteorder.h
//   Creation date : Mon Dec 25 2006 19:56:16 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2006-2008 Szymon Stefanek (pragma at kvirc dot net)
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
#include "kvi_bswap.h"
#include "kvi_inttypes.h"


//
// Byte Orders Reminder
//   Number                             0xaabbccdd
//   Little Endian Stores               0xdd 0xcc 0xbb 0xaa
//   Big Endian Stores                  0xaa 0xbb 0xcc 0xdd
//   Perverse Middle Endian             0xbb 0xaa 0xdd 0xcc or another braindamaged combination (unsupported)
//   Network Byte Order is Big Endian
//   Intel Stuff uses Little Endian
//   PPC is Big Endian
//   Universal binaries on MacOSX use both Big and Little Endian
//

#ifdef BIG_ENDIAN_MACHINE_BYTE_ORDER
	#define kvi_localCpuToLittleEndian16(u) kvi_swap16((kvi_u16_t)(u))
	#define kvi_localCpuToLittleEndian32(u) kvi_swap32((kvi_u32_t)(u))
	#define kvi_localCpuToLittleEndian64(u) kvi_swap64((kvi_u64_t)(u))
	#define kvi_littleEndianToLocalCpu16(u) kvi_swap16((kvi_u16_t)(u))
	#define kvi_littleEndianToLocalCpu32(u) kvi_swap32((kvi_u32_t)(u))
	#define kvi_littleEndianToLocalCpu64(u) kvi_swap64((kvi_u64_t)(u))
#else
	// We ASSUME that the local cpu is little endian.. if it isn't.. well :)
	#define LOCAL_CPU_LITTLE_ENDIAN
	#define kvi_localCpuToLittleEndian16(u) (u)
	#define kvi_localCpuToLittleEndian32(u) (u)
	#define kvi_localCpuToLittleEndian64(u) (u)
	#define kvi_littleEndianToLocalCpu16(u) (u)
	#define kvi_littleEndianToLocalCpu32(u) (u)
	#define kvi_littleEndianToLocalCpu64(u) (u)
#endif


#endif // !_KVI_BYTEORDER_H_
