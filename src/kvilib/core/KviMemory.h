#ifndef _KVI_MALLOC_H_
#define _KVI_MALLOC_H_
//=============================================================================
//
//   File : KviMemory.h
//   Creation date : Sun Jun 18 2000 18:18:36 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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
// C memory allocation routines: macros in common compilations
//

#include "kvi_settings.h"

#include <stdlib.h>
#include <string.h>

/**
* \file KviMemory.h
* \author Szymon Stefanek
* \brief This file contains an internal implementation of the malloc/free functions
*
* If COMPILE_MEMORY_PROFILE is enabled, kvirc will use its internal implementation that includes a memory profiler.
* If COMPILE_MEMORY_CHECKS is enabled, kvirc will check and report memory exhaustion problems.
* If none of the previous is enabled, this will just bind the KviMemory and kvi_free functions to the proper
* functions of the underlaying system.
*/

namespace KviMemory
{

#ifdef COMPILE_MEMORY_PROFILE

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
#error "This stuff should be never compiled on Windows"
#endif

	/**
	* \brief Allocates size bytes of memory
	*
	* \param size number of bytes
	* \return void *
	*/
	KVILIB_API void * allocate(int size);

	/**
	* \brief Changes the size of the memory block pointed to by ptr to size bytes
	*
	* \param ptr pointer to the memory block
	* \param size number of bytes
	* \return void *
	*/
	KVILIB_API void * reallocate(void * ptr, int size);

	/**
	* \brief Frees the memory space pointed to by ptr
	*
	* \param ptr pointer to the memory block
	* \return void
	*/
	KVILIB_API void free(void * ptr);

#else //!COMPILE_MEMORY_PROFILE

#ifdef COMPILE_MEMORY_CHECKS

#ifdef COMPILE_ON_WINDOWS
#error "This stuff should be never compiled on Windows"
#endif

	/**
		* \brief Prints a warning about failed memory allocation
		*/
	KVILIB_API void outOfMemory();

	inline void * allocate(int size)
	{
		void * ptr = ::malloc(size);
		if(!ptr)
			outOfMemory();
		return ptr;
	}

	inline void * reallocate(void * ptr, int size)
	{
		ptr = ::realloc(ptr, size);
		if(!ptr)
			outOfMemory();
		return ptr;
	}

#else //!COMPILE_MEMORY_CHECKS

	inline void * allocate(int size)
	{
		return ::malloc(size);
	}

	inline void * reallocate(void * ptr, int size)
	{
		return ::realloc(ptr, size);
	}

#endif //!COMPILE_MEMORY_CHECKS

	inline void free(void * ptr)
	{
		::free(ptr);
	}
#endif //!COMPILE_MEMORY_PROFILE

	/**
	* \brief Moves len bytes from src_ptr to dst_ptr
	*
	* \param dst_ptr The destination memory pointer
	* \param src_ptr The source memory pointer
	* \param len the number of bytes to move
	* \return void *
	*/
	inline void move(void * dst_ptr, const void * src_ptr, int len)
	{
		memmove(dst_ptr, src_ptr, len);
	}

	/**
	* \brief Initializes len bytes of memory starting from dst_ptr to c
	*
	* \param dst_ptr The destination memory pointer
	* \param c The character that will fill the memory
	* \param len the number of bytes to initialize
	* \return void *
	*/
	inline void set(void * dst_ptr, char c, int len)
	{
		memset(dst_ptr, c, len);
	}

	/**
	* \brief Moves len bytes from src_ptr to dst_ptr
	*
	* \param dst_ptr The destination memory pointer
	* \param src_ptr The source memory pointer
	* \param len the number of bytes to move
	* \return void *
	* \warning In fastmove the src and dst may not overlap
	*/
	inline void copy(void * dst_ptr, const void * src_ptr, int len)
	{
		memcpy(dst_ptr, src_ptr, len);
	}

} // namespace KviMemory

#endif //_KVI_MALLOC_H_
