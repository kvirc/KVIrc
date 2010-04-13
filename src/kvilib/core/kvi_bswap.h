#ifndef _KVI_BSWAP_H_
#define _KVI_BSWAP_H_

//=============================================================================
//
//   File : kvi_bswap.h
//   Creation date : Fri Mar 19 1999 03:15:21 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2008 Szymon Stefanek (pragma at kvirc dot net)
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

/**
* \file kvi_bswap.h
* \author Szymon Stefanek
* \brief Bit Swap functions
*/

#include "kvi_settings.h"
#include "kvi_inttypes.h"

// KVILIB_API has been removed from these two functions
// these should always go inlined

/**
* \brief Swaps the endianess of a kvi_u64_t
*
* \param i the original value
* \return kvi_u64_t
*/
inline kvi_u64_t kvi_swap64(kvi_u64_t i)
{
	// abcdefgh to hgfedcba
	return ((i << 56) |                /* h to a */
		((i & 0xff00) << 40) |     /* g to b */
		((i & 0xff0000) << 24) |   /* f to c */
		((i & 0xff000000) << 8) |  /* e to d */
		((i >> 8) & 0xff000000) |  /* d to e */
		((i >> 24) & 0xff0000) |   /* c to f */
		((i >> 40) & 0xff00) |     /* b to g */
		(i >> 56));                /* a to h */
}

/**
* \brief Swaps the endianess of a kvi_u32_t
*
* \param i the original value
* \return kvi_u32_t
*/
inline kvi_u32_t kvi_swap32(kvi_u32_t i)
{
	// abcd to dcba
	return ((i << 24) | ((i & 0xff00) << 8) | ((i >> 8) & 0xff00) | (i >> 24));
}

/**
* \brief Swaps the endianess of a kvi_u16_t
*
* \param i the original value
* \return kvi_u16_t
*/
inline kvi_u16_t kvi_swap16(kvi_u16_t i)
{
	// ab to ba
	return ((i << 8) | (i >> 8));
}

#endif // !_KVI_BSWAP_H_
