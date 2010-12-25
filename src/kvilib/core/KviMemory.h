#ifndef _KVI_MALLOC_H_
#define _KVI_MALLOC_H_

//=============================================================================
//
//   File : KviMemory.h
//   Creation date : Sun Jun 18 2000 18:18:36 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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

//=============================================================================
// C memory allocation routines: macros in common compilations
//=============================================================================

#include "kvi_settings.h"

#include <stdlib.h>

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
	void * allocate(int size);

	/**
	* \brief Changes the size of the memory block pointed to by ptr to size bytes
	*
	* \param ptr pointer to the memory block
	* \param size number of bytes
	* \return void *
	*/
	void * reallocate(void * ptr,int size);

	/**
	* \brief Frees the memory space pointed to by ptr
	*
	* \param ptr pointer to the memory block
	* \return void
	*/
	void free(void * ptr);

#else //!COMPILE_MEMORY_PROFILE


	#ifdef COMPILE_MEMORY_CHECKS

		#ifdef COMPILE_ON_WINDOWS
			#error "This stuff should be never compiled on Windows"
		#endif

		/**
		* \brief Prints a warning about failed memory allocation
		*/
		void outOfMemory();

		inline void * allocate(int size)
		{
			void * ptr = malloc(size);
			if(!ptr)
				outOfMemory();
			return ptr;
		}

		inline void * reallocate(void * ptr,int size)
		{
			ptr = realloc(ptr,size);
			if(!ptr)
				outOfMemory();
			return ptr;
		}

	#else //!COMPILE_MEMORY_CHECKS

		inline void * allocate(int size)
		{
			return ::malloc(size);
		}

		inline void * reallocate(void * ptr,int size)
		{
			return ::realloc(ptr,size);
		}

	#endif //!COMPILE_MEMORY_CHECKS


	inline void free(void * ptr)
	{
		::free(ptr);
	}

#endif //!COMPILE_MEMORY_PROFILE

} // namespace KviMemory

#endif //_KVI_MALLOC_H_
