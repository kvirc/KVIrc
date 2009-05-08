//=============================================================================
//
//   File : kvi_malloc.cpp
//   Creation date : Sun Jun 18 2000 18:26:27 CEST by Szymon Stefanek
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
// C memory allocation routines
// This stuff is rather unused, because in normal compilations
// kvi_malloc , kvi_free and kvi_realloc are macros (see kvi_malloc.h)
//=============================================================================


#define _KVI_MALLOC_CPP_
#include "kvi_malloc.h"

#include <stdio.h>



#ifdef COMPILE_MEMORY_PROFILE

	//
	// Memory profile stuff
	// Used to find memory leaks etc...
	//

	#include "kvi_pointerlist.h"

	typedef struct _KviMallocEntry {
		struct _KviMallocEntry * prev;
		void                   * pointer;
		int                      size;
		void                   * return_addr1;
		void                   * return_addr2;
		struct _KviMallocEntry * next;
	} KviMallocEntry;

	int              g_iMaxRequestSize           = 0;
	void           * g_pMaxRequestReturnAddress1 = 0;
	void           * g_pMaxRequestReturnAddress2 = 0;
	unsigned int     g_iMallocCalls              = 0;
	unsigned int     g_iReallocCalls             = 0;
	unsigned int     g_iFreeCalls                = 0;
	unsigned int     g_iTotalMemAllocated        = 0;
	unsigned int     g_uAllocationPeak           = 0;
	KviMallocEntry * g_pEntries                  = 0;

	void * kvi_malloc(int size)
	{
		g_iMallocCalls ++;
		g_iTotalMemAllocated += size;
		if(g_iTotalMemAllocated > g_uAllocationPeak)g_uAllocationPeak = g_iTotalMemAllocated;
		if(g_iMaxRequestSize < size){
			g_iMaxRequestSize = size;
			g_pMaxRequestReturnAddress1 = __builtin_return_address(1);
			g_pMaxRequestReturnAddress2 = __builtin_return_address(2);
		}
		KviMallocEntry * e = (KviMallocEntry *)malloc(sizeof(KviMallocEntry));
		e->pointer = malloc(size);
		e->size = size;
		e->return_addr1 = __builtin_return_address(1);
		e->return_addr2 = __builtin_return_address(2);
		e->next = g_pEntries;
		e->prev = 0;
		if(g_pEntries)g_pEntries->prev = e;
		g_pEntries = e;
		return e->pointer;
	}

	void * kvi_realloc(void * ptr,int size)
	{
		g_iReallocCalls ++;
		if(ptr == 0)return kvi_malloc(size);
		if(g_iMaxRequestSize < size){
			g_iMaxRequestSize = size;
			g_pMaxRequestReturnAddress1 = __builtin_return_address(1);
			g_pMaxRequestReturnAddress2 = __builtin_return_address(2);
		}
		KviMallocEntry *e = g_pEntries;
		while(e){
			if(e->pointer == ptr){
				g_iTotalMemAllocated -= e->size;
				g_iTotalMemAllocated += size;
				if(g_iTotalMemAllocated > g_uAllocationPeak)g_uAllocationPeak = g_iTotalMemAllocated;
				e->pointer = realloc(ptr,size);
				e->size = size;
				e->return_addr1 = __builtin_return_address(1);
				e->return_addr2 = __builtin_return_address(2);
				return e->pointer;
			}
			e = e->next;
		}
		fprintf(stderr,"Attempt to realloc an inexisting pointer (%p) (called from %p (%p))\n",ptr,__builtin_return_address(1),__builtin_return_address(2));
		return realloc(ptr,size);
	}

	void kvi_free(void * ptr)
	{
		g_iFreeCalls++;
		if(ptr == 0){
			fprintf(stderr,"Attempt to free a null pointer (called from %p (%p))\n",__builtin_return_address(1),__builtin_return_address(2));
			exit(-1);
		}
		KviMallocEntry * e= g_pEntries;
		while(e){
			if(e->pointer == ptr){
				g_iTotalMemAllocated -= e->size;
				if(e->prev){
					if(e == g_pEntries)fprintf(stderr,"Mem profiling internal error!\n");
					e->prev->next = e->next;
					if(e->next)e->next->prev = e->prev;
				} else {
					if(e != g_pEntries)fprintf(stderr,"Mem profiling internal error!\n");
					if(e->next)e->next->prev = 0;
					g_pEntries = e->next;
				}
				free(e);
				return;
			}
			e = e->next;
		}
		fprintf(stderr,"Attempt to free an inexisting pointer (%p) (called from %p (%p))\n",ptr,__builtin_return_address(1),__builtin_return_address(2));
	}

	void kvi_memory_profile() __attribute__((destructor));
	void kvi_memory_profile()
	{
		unsigned int countUnfreed = 0;
		KviMallocEntry * e = g_pEntries;
		while(e){
			countUnfreed++;
			e = e->next;
		}
		fprintf(stderr,"|====|====|====|====|====|====|====|====\n");
		fprintf(stderr,"| Memory profile for KVIrc\n");
		fprintf(stderr,"| Unfreed chunks : %d\n",countUnfreed);
		fprintf(stderr,"| Total unfreed memory : %u bytes\n",g_iTotalMemAllocated);
		fprintf(stderr,"|====|====|====|====|====|====|====|====\n");
		fprintf(stderr,"| Possible unfreed chunks dump:\n");
		e = g_pEntries;
		while(e){
			fprintf(stderr,"|====|====|\n");
			fprintf(stderr,"| Currently unfreed chunk: %p\n",e->pointer);
			fprintf(stderr,"| Size: %d\n",e->size);
			fprintf(stderr,"| Caller address 1: %p\n",e->return_addr1);
			fprintf(stderr,"| Caller address 2: %p\n",e->return_addr2);
			if(e->size > 10)fprintf(stderr,"| Data: %.10s\n",e->pointer);
			else if(e->size > 5)fprintf(stderr,"| Data: %.5s\n",e->pointer);
			KviMallocEntry *toFree = e;
			e = e->next;
			free(toFree);
		}
		fprintf(stderr,"|====|====|====|====|====|====|====|====\n");
		fprintf(stderr,"| Allocation peak : %u bytes\n",g_uAllocationPeak);
		fprintf(stderr,"|====|====|====|====|====|====|====|====\n");
		fprintf(stderr,"| Max request size : %d bytes\n",g_iMaxRequestSize);
		fprintf(stderr,"| Called from %p (%p)\n",g_pMaxRequestReturnAddress1,g_pMaxRequestReturnAddress2);
		fprintf(stderr,"|====|====|====|====|====|====|====|====\n");
		fprintf(stderr,"| Malloc calls: %u\n",g_iMallocCalls);
		fprintf(stderr,"| Realloc calls: %u\n",g_iReallocCalls);
		fprintf(stderr,"| Free calls: %u\n",g_iFreeCalls);
		fprintf(stderr,"|====|====|====|====|====|====|====|====\n");
	}

#else

	#ifdef COMPILE_MEMORY_CHECKS

	void outOfMemory()
	{
		//What a cool message :)
		fprintf(stderr,"Virtual memory exhausted in malloc call....bye!\n");
		exit(-1);
	}

	#endif

#endif
