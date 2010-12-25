#ifndef _KVI_BYTEORDER_H_
#define _KVI_BYTEORDER_H_

//=============================================================================
//
//   File : KviByteOrder.h
//   Creation date : Mon Dec 25 2006 19:56:16 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2006-2010 Szymon Stefanek (pragma at kvirc dot net)
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

namespace KviByteOrder
{

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

	inline kvi_u16_t localCpuToLittleEndian16(kvi_u16_t u)
	{
		return kvi_swap16(u);
	}
	
	inline kvi_u32_t localCpuToLittleEndian32(kvi_u32_t u)
	{
		return kvi_swap32(u);
	}

	inline kvi_u64_t localCpuToLittleEndian64(kvi_u64_t u)
	{
		return kvi_swap64(u);
	}

	inline kvi_u16_t littleEndianToLocalCpu16(kvi_u16_t u)
	{
		return kvi_swap16(u);
	}

	inline kvi_u32_t littleEndianToLocalCpu32(kvi_u32_t u)
	{
		return kvi_swap32(u);
	}

	inline kvi_u64_t littleEndianToLocalCpu64(kvi_u64_t u)
	{
		return kvi_swap64(u);
	}

#else
	// We ASSUME that the local cpu is little endian.. if it isn't.. well :)
	#define LOCAL_CPU_LITTLE_ENDIAN 1

	inline kvi_u16_t localCpuToLittleEndian16(kvi_u16_t u)
	{
		return u;
	}
	
	inline kvi_u32_t localCpuToLittleEndian32(kvi_u32_t u)
	{
		return u;
	}

	inline kvi_u64_t localCpuToLittleEndian64(kvi_u64_t u)
	{
		return u;
	}

	inline kvi_u16_t littleEndianToLocalCpu16(kvi_u16_t u)
	{
		return u;
	}

	inline kvi_u32_t littleEndianToLocalCpu32(kvi_u32_t u)
	{
		return u;
	}

	inline kvi_u64_t littleEndianToLocalCpu64(kvi_u64_t u)
	{
		return u;
	}

#endif

} // namespace KviByteOrder


#endif // !_KVI_BYTEORDER_H_
