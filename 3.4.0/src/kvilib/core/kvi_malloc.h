#ifndef _KVI_MALLOC_H_
#define _KVI_MALLOC_H_

//=============================================================================
//
//   File : kvi_malloc.h
//   Creation date : Sun Jun 18 2000 18:18:36 CEST by Szymon Stefanek
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
//=============================================================================

//=============================================================================
// C memory allocation routines: macros in common compilations
//=============================================================================

#include "kvi_settings.h"

#include <stdlib.h>

#ifdef COMPILE_MEMORY_PROFILE

	#ifdef COMPILE_ON_WINDOWS
		#error "This stuff should be never compiled on Windows"
	#endif

	extern void * kvi_malloc(int size);
	extern void * kvi_realloc(void * ptr,int size);
	extern void   kvi_free(void * ptr);

#else

	#ifndef COMPILE_MEMORY_CHECKS

		// These two are the "common" ones
		#define kvi_malloc(__size_) malloc(__size_)
		#define kvi_realloc(__ptr_,__size_) realloc((void *)__ptr_,__size_)

	#else

		#ifdef COMPILE_ON_WINDOWS
			#error "This stuff should be never compiled on Windows"
		#endif

		// Want to check all the pointers
		#define kvi_malloc(__size_) kvi_safe_malloc(__size_)
		#define kvi_realloc(__ptr_,__size_) kvi_safe_realloc((void *)__ptr_,__size_)
	
		#ifndef _KVI_MALLOC_CPP_
			extern void outOfMemory();
		#endif

		inline void * kvi_safe_malloc(int size)
		{
			void * ptr = malloc(size);
			if(!ptr)outOfMemory();
			return ptr;
		}

		inline void * kvi_safe_realloc(void * ptr,int size)
		{
			ptr = realloc(ptr,size);
			if(!ptr)outOfMemory();
			return ptr;
		}

	#endif //COMPILE_MEMORY_CHECKS

	#define kvi_free(__ptr_) free((void *)__ptr_)

#endif

#endif //_KVI_MALLOC_H_
