#ifndef _KVI_BYTEORDER_H_
#define _KVI_BYTEORDER_H_
//=============================================================================
//
//   File : KviByteOrder.h
//   Creation date : Mon Dec 25 2006 19:56:16 CEST by Szymon Stefanek
//   Based on file : kvi_bswap.h
//   Creation date : Fri Mar 19 1999 03:15:21 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 1999-2010 Szymon Stefanek (pragma at kvirc dot net)
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
#include "kvi_inttypes.h"

namespace KviByteOrder
{
	/**
	* \brief Swaps the endianness of a kvi_u64_t
	*
	* \param i the original value
	* \return kvi_u64_t
	*/
	inline kvi_u64_t swap64(kvi_u64_t i)
	{
		// abcdefgh to hgfedcba
		return ((i << 56) |           /* h to a */
		    ((i & 0xff00) << 40) |    /* g to b */
		    ((i & 0xff0000) << 24) |  /* f to c */
		    ((i & 0xff000000) << 8) | /* e to d */
		    ((i >> 8) & 0xff000000) | /* d to e */
		    ((i >> 24) & 0xff0000) |  /* c to f */
		    ((i >> 40) & 0xff00) |    /* b to g */
		    (i >> 56));               /* a to h */
	}

	/**
	* \brief Swaps the endianness of a kvi_u32_t
	*
	* \param i the original value
	* \return kvi_u32_t
	*/
	inline kvi_u32_t swap32(kvi_u32_t i)
	{
		// abcd to dcba
		return ((i << 24) | ((i & 0xff00) << 8) | ((i >> 8) & 0xff00) | (i >> 24));
	}

	/**
	* \brief Swaps the endianness of a kvi_u16_t
	*
	* \param i the original value
	* \return kvi_u16_t
	*/
	inline kvi_u16_t swap16(kvi_u16_t i)
	{
		// ab to ba
		return ((i << 8) | (i >> 8));
	}

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
		return swap16(u);
	}

	inline kvi_u32_t localCpuToLittleEndian32(kvi_u32_t u)
	{
		return swap32(u);
	}

	inline kvi_u64_t localCpuToLittleEndian64(kvi_u64_t u)
	{
		return swap64(u);
	}

	inline kvi_u16_t littleEndianToLocalCpu16(kvi_u16_t u)
	{
		return swap16(u);
	}

	inline kvi_u32_t littleEndianToLocalCpu32(kvi_u32_t u)
	{
		return swap32(u);
	}

	inline kvi_u64_t littleEndianToLocalCpu64(kvi_u64_t u)
	{
		return swap64(u);
	}

	inline kvi_u16_t localCpuToBigEndian16(kvi_u16_t u)
	{
		return u;
	}

	inline kvi_u32_t localCpuToBigEndian32(kvi_u32_t u)
	{
		return u;
	}

	inline kvi_u64_t localCpuToBigEndian64(kvi_u64_t u)
	{
		return u;
	}

	inline kvi_u16_t bigEndianToLocalCpu16(kvi_u16_t u)
	{
		return u;
	}

	inline kvi_u32_t bigEndianToLocalCpu32(kvi_u32_t u)
	{
		return u;
	}

	inline kvi_u64_t bigEndianToLocalCpu64(kvi_u64_t u)
	{
		return u;
	}

	inline kvi_u16_t localCpuToNetworkByteOrder16(kvi_u16_t u)
	{
		return u;
	}

	inline kvi_u32_t localCpuToNetworkByteOrder32(kvi_u32_t u)
	{
		return u;
	}

	inline kvi_u64_t localCpuToNetworkByteOrder64(kvi_u64_t u)
	{
		return u;
	}

	inline kvi_u16_t networkByteOrderToLocalCpu16(kvi_u16_t u)
	{
		return u;
	}

	inline kvi_u32_t networkByteOrderToLocalCpu32(kvi_u32_t u)
	{
		return u;
	}

	inline kvi_u64_t networkByteOrderToLocalCpu64(kvi_u64_t u)
	{
		return u;
	}

#else //!BIG_ENDIAN_MACHINE_BYTE_ORDER

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

	inline kvi_u16_t localCpuToBigEndian16(kvi_u16_t u)
	{
		return swap16(u);
	}

	inline kvi_u32_t localCpuToBigEndian32(kvi_u32_t u)
	{
		return swap32(u);
	}

	inline kvi_u64_t localCpuToBigEndian64(kvi_u64_t u)
	{
		return swap64(u);
	}

	inline kvi_u16_t bigEndianToLocalCpu16(kvi_u16_t u)
	{
		return swap16(u);
	}

	inline kvi_u32_t bigEndianToLocalCpu32(kvi_u32_t u)
	{
		return swap32(u);
	}

	inline kvi_u64_t bigEndianToLocalCpu64(kvi_u64_t u)
	{
		return swap64(u);
	}

	inline kvi_u16_t localCpuToNetworkByteOrder16(kvi_u16_t u)
	{
		return swap16(u);
	}

	inline kvi_u32_t localCpuToNetworkByteOrder32(kvi_u32_t u)
	{
		return swap32(u);
	}

	inline kvi_u64_t localCpuToNetworkByteOrder64(kvi_u64_t u)
	{
		return swap64(u);
	}

	inline kvi_u16_t networkByteOrderToLocalCpu16(kvi_u16_t u)
	{
		return swap16(u);
	}

	inline kvi_u32_t networkByteOrderToLocalCpu32(kvi_u32_t u)
	{
		return swap32(u);
	}

	inline kvi_u64_t networkByteOrderToLocalCpu64(kvi_u64_t u)
	{
		return swap64(u);
	}

#endif //!BIG_ENDIAN_MACHINE_BYTE_ORDER

} // namespace KviByteOrder

#endif // !_KVI_BYTEORDER_H_
