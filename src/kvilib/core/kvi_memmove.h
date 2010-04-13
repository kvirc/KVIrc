#ifndef _KVI_MEMMOVE_H_
#define _KVI_MEMMOVE_H_

//=============================================================================
//
//   File : kvi_memmove.h
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
* \file kvi_memmove.h
* \author Szymon Stefanek
* \brief This file contains an internal implementation of the memove/memcpy functions
*
* If COMPILE_WITH_SYSTEM_MEMMOVE is disabled, kvirc will use its internal implementation
* of the memmove/memcpy family of functions. If COMPILE_ix86_ASM is enabled, the implementation
* is based on x86 (32-bit only) asm.
*/

#include "kvi_settings.h"

#ifndef _KVI_MEMMOVE_CPP_

	#ifdef COMPILE_WITH_SYSTEM_MEMMOVE

		#include <string.h>

		#define kvi_memmove memmove
		#define kvi_memmoveodd memmove
		#define kvi_memset memset
		#define kvi_fastmove memcpy
		#define kvi_fastmoveodd memcpy

	#else

		#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
			#error "This stuff should be never compiled on Windows"
		#endif

		/**
		* \brief Moves len bytes from src_ptr to dst_ptr
		*
		* \param dst_ptr The destination memory pointer
		* \param src_ptr The source memory pointer
		* \param len the number of bytes to move
		* \return void *
		*/
		extern void *kvi_memmove(void *dst_ptr,const void *src_ptr,int len);

		/**
		* \brief Moves an odd number of bytes (specified in len) from src_ptr to dst_ptr
		*
		* \param dst_ptr The destination memory pointer
		* \param src_ptr The source memory pointer
		* \param len the number of bytes to move; has to be odd
		* \return void *
		*/
		extern void *kvi_memmoveodd(void *dst_ptr,const void *src_ptr,int len);

		/**
		* \brief Initializes len bytes of memory starting from dst_ptr to c
		*
		* \param dst_ptr The destination memory pointer
		* \param c The character that will fill the memory
		* \param len the number of bytes to initialize
		* \return void *
		*/
		extern void *kvi_memset(void *dst_ptr,char c,int len);

		#ifdef COMPILE_ix86_ASM

			// WE WANT repnz; movsq\n"!!!

			inline void kvi_fastmove(void * dst_ptr,const void *src_ptr,int len)
			{
				__asm__ __volatile__(
					"	cld\n"
					"	shr $1,%0\n"
					"	jnc 1f\n"
					"	movsb\n"
					"1:\n"
					"	shr $1,%0\n"
					"	jnc 2f\n"
					"	movsw\n"
					"2:\n"
					"	repnz; movsl\n"
					: "=c" (len), "=&S" (src_ptr), "=&D" (dst_ptr)
					: "0"  (len), "1"   (src_ptr), "2"   (dst_ptr)
				);
			}

			inline void kvi_fastmoveodd(void * dst_ptr,const void *src_ptr,int len)
			{
				__asm__ __volatile__(
					"	cld\n"
					"	shr $2,%0\n"
					"	jnc 1f\n"
					"	movsw\n"
					"1:\n"
					"	repnz; movsl\n"
					: "=c" (len), "=&S" (src_ptr), "=&D" (dst_ptr)
					: "0"  (len), "1"   (src_ptr), "2"   (dst_ptr)
				);
			}

		#else // ! COMPILE_ix86_ASM

			/**
			* \brief Moves len bytes from src_ptr to dst_ptr
			*
			* \param dst_ptr The destination memory pointer
			* \param src_ptr The source memory pointer
			* \param len the number of bytes to move
			* \return void *
			* \warning In fastmove the src and dst may not overlap
			*/
			extern void kvi_fastmove(void *dst_ptr,const void *src_ptr,int len);

			/**
			* \brief Moves an odd number of bytes (specified in len) from src_ptr to dst_ptr
			*
			* \param dst_ptr The destination memory pointer
			* \param src_ptr The source memory pointer
			* \param len the number of bytes to move; has to be odd
			* \return void *
			* \warning In fastmove the src and dst may not overlap
			*/
			extern void kvi_fastmoveodd(void *dst_ptr,const void *src_ptr,int len);

		#endif // !COMPILE_ix86_ASM

	#endif // COMPILE_WITH_SYSTEM_MEMMOVE

#endif // _KVI_MEMMOVE_CPP_

#endif // !_KVI_MEMMOVE_H_
