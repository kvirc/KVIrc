//=============================================================================
//
//   File : KviMemory.cpp
//   Creation date : Sun Jun 18 2000 18:26:27 CEST by Szymon Stefanek
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
// C memory allocation routines
// This stuff is rather unused, because in normal compilations
// KviMemory, kvi_free and kvi_realloc are macros (see KviMemory.h)
//

#define _KVI_MALLOC_CPP_
#include "KviMemory.h"

#include <cstdio>

#ifdef COMPILE_MEMORY_PROFILE
#include "KviPointerList.h"
#endif //COMPILE_MEMORY_PROFILE

namespace KviMemory
{

#ifdef COMPILE_MEMORY_PROFILE

	//
	// Memory profile stuff
	// Used to find memory leaks etc...
	//

	struct KviMallocEntry
	{
		KviMallocEntry * prev;
		void * pointer;
		int size;
		void * return_addr1;
		void * return_addr2;
		KviMallocEntry * next;
	};

	int g_iMaxRequestSize = 0;
	void * g_pMaxRequestReturnAddress1 = nullptr;
	void * g_pMaxRequestReturnAddress2 = nullptr;
	unsigned int g_iMallocCalls = 0;
	unsigned int g_iReallocCalls = 0;
	unsigned int g_iFreeCalls = 0;
	unsigned int g_iTotalMemAllocated = 0;
	unsigned int g_uAllocationPeak = 0;
	KviMallocEntry * g_pEntries = nullptr;

	void * allocate(int size)
	{
		g_iMallocCalls++;
		g_iTotalMemAllocated += size;
		if(g_iTotalMemAllocated > g_uAllocationPeak)
			g_uAllocationPeak = g_iTotalMemAllocated;
		if(g_iMaxRequestSize < size)
		{
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
		e->prev = nullptr;
		if(g_pEntries)
			g_pEntries->prev = e;
		g_pEntries = e;
		return e->pointer;
	}

	void * reallocate(void * ptr, int size)
	{
		g_iReallocCalls++;
		if(ptr == 0)
			return allocate(size);
		if(g_iMaxRequestSize < size)
		{
			g_iMaxRequestSize = size;
			g_pMaxRequestReturnAddress1 = __builtin_return_address(1);
			g_pMaxRequestReturnAddress2 = __builtin_return_address(2);
		}
		KviMallocEntry * e = g_pEntries;
		while(e)
		{
			if(e->pointer == ptr)
			{
				g_iTotalMemAllocated -= e->size;
				g_iTotalMemAllocated += size;
				if(g_iTotalMemAllocated > g_uAllocationPeak)
					g_uAllocationPeak = g_iTotalMemAllocated;
				e->pointer = realloc(ptr, size);
				e->size = size;
				e->return_addr1 = __builtin_return_address(1);
				e->return_addr2 = __builtin_return_address(2);
				return e->pointer;
			}
			e = e->next;
		}
		fprintf(stderr, "Attempt to realloc a non-existent pointer (%p) (called from %p (%p))\n", ptr, __builtin_return_address(1), __builtin_return_address(2));
		return realloc(ptr, size);
	}

	void free(void * ptr)
	{
		g_iFreeCalls++;
		if(ptr == 0)
		{
			fprintf(stderr, "Attempt to free a null pointer (called from %p (%p))\n", __builtin_return_address(1), __builtin_return_address(2));
			exit(-1);
		}
		KviMallocEntry * e = g_pEntries;
		while(e)
		{
			if(e->pointer == ptr)
			{
				g_iTotalMemAllocated -= e->size;
				if(e->prev)
				{
					if(e == g_pEntries)
						fprintf(stderr, "Mem profiling internal error!\n");
					e->prev->next = e->next;
					if(e->next)
						e->next->prev = e->prev;
				}
				else
				{
					if(e != g_pEntries)
						fprintf(stderr, "Mem profiling internal error!\n");
					if(e->next)
						e->next->prev = nullptr;
					g_pEntries = e->next;
				}
				free(e);
				return;
			}
			e = e->next;
		}
		fprintf(stderr, "Attempt to free a non-existent pointer (%p) (called from %p (%p))\n", ptr, __builtin_return_address(1), __builtin_return_address(2));
	}

	void kvi_memory_profile() __attribute__((destructor));
	void kvi_memory_profile()
	{
		unsigned int countUnfreed = 0;
		KviMallocEntry * e = g_pEntries;
		while(e)
		{
			countUnfreed++;
			e = e->next;
		}
		fprintf(stderr, "|====|====|====|====|====|====|====|====\n");
		fprintf(stderr, "| Memory profile for KVIrc\n");
		fprintf(stderr, "| Unfreed chunks : %d\n", countUnfreed);
		fprintf(stderr, "| Total unfreed memory : %u bytes\n", g_iTotalMemAllocated);
		fprintf(stderr, "|====|====|====|====|====|====|====|====\n");
		fprintf(stderr, "| Possible unfreed chunks dump:\n");
		e = g_pEntries;
		while(e)
		{
			fprintf(stderr, "|====|====|\n");
			fprintf(stderr, "| Currently unfreed chunk: %p\n", e->pointer);
			fprintf(stderr, "| Size: %d\n", e->size);
			fprintf(stderr, "| Caller address 1: %p\n", e->return_addr1);
			fprintf(stderr, "| Caller address 2: %p\n", e->return_addr2);
			if(e->size > 10)
				fprintf(stderr, "| Data: %.10s\n", (char *)e->pointer);
			else if(e->size > 5)
				fprintf(stderr, "| Data: %.5s\n", (char *)e->pointer);
			KviMallocEntry * toFree = e;
			e = e->next;
			free(toFree);
		}
		fprintf(stderr, "|====|====|====|====|====|====|====|====\n");
		fprintf(stderr, "| Allocation peak : %u bytes\n", g_uAllocationPeak);
		fprintf(stderr, "|====|====|====|====|====|====|====|====\n");
		fprintf(stderr, "| Max request size : %d bytes\n", g_iMaxRequestSize);
		fprintf(stderr, "| Called from %p (%p)\n", g_pMaxRequestReturnAddress1, g_pMaxRequestReturnAddress2);
		fprintf(stderr, "|====|====|====|====|====|====|====|====\n");
		fprintf(stderr, "| Malloc calls: %u\n", g_iMallocCalls);
		fprintf(stderr, "| Realloc calls: %u\n", g_iReallocCalls);
		fprintf(stderr, "| Free calls: %u\n", g_iFreeCalls);
		fprintf(stderr, "|====|====|====|====|====|====|====|====\n");
	}

#else //!COMPILE_MEMORY_PROFILE

#ifdef COMPILE_MEMORY_CHECKS

	void outOfMemory()
	{
		//What a cool message :)
		fprintf(stderr, "Virtual memory exhausted in malloc call... bye!\n");
		exit(-1);
	}

#endif //COMPILE_MEMORY_CHECKS

#endif //!COMPILE_MEMORY_PROFILE

#if 0

	// The code below is unused and remains here only for historical reasons.
	// At the time KVIrc was first written there were some platforms with weak
	// memmove and memcpy implementations.
	//
	// Modern compilers coupled with decent C library implementations we have these days
	// can do better than plain assembler. The i586 glibc memcpy implementation, for instance,
	// will keep the cache well-filled and both pipelines of the pentium busy at the same time
	// while the mach implementation can even copy entire pages of memory at very low cost by
	// using virtual address manipulationtricks. 64bit architectures can move bytes in larger
	// chunks and there are MXX and SSE optimisations available.

#ifdef COMPILE_ix86_ASM

	// WE WANT repnz; movsq\n"!!!

	void copy(void * dst_ptr,const void *src_ptr,int len)
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

	void copy(void * dst_ptr,const void *src_ptr,int len)
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

	void * move(void * dst_ptr,const void *src_ptr,int len)
	{
		KVI_ASSERT(dst_ptr);
		KVI_ASSERT(src_ptr);
		KVI_ASSERT(len >= 0);
		// Save pointer registers
		asm("	pushl %esi");                     // save %esi
		asm("	pushl %edi");                     // save %edi
		// Load arguments
		asm("	movl 16(%ebp),%ecx");             // %ecx = len
		asm("	movl 12(%ebp),%esi");             // %esi = src
		asm("	movl 8(%ebp),%edi");              // %edi = dst
		// Compare src and dest
		asm("	cmpl %esi,%edi");                 // %edi - %esi
		asm("	jbe move_from_bottom_to_top");    // if(%edi < %esi) jump to move_from_bottom_to_top
		// dst_ptr > src_ptr
		asm("	addl %ecx,%esi");                 // %esi += %ecx (src_ptr += len);
		asm("	addl %ecx,%edi");                 // %edi += %ecx (dst_ptr += len);
		asm("	decl %esi");                      // %esi--; (src_ptr--);
		asm("	decl %edi");                      // %edi--; (dst_ptr--);
		asm("	std");                            // set direction flag (decrement esi and edi in movsb)
		// Optimization : check for non-odd len (1,3,5,7...)
		asm("	shr $1,%ecx");                    // %ecx >> 1, shifted bit -> CF
		asm("	jnc move_two_bytes_top_to_bottom_directly");  // if !carry (CF == 0) skip this move
		// Move the first byte (non-odd)
		asm("	movsb %ds:(%esi),%es:(%edi)");    // *dst-- = *src-- if DF  else *dst++ = *src++
		asm("move_two_bytes_top_to_bottom_directly:");
		asm("	decl %esi");                      // %esi--; (src_ptr--);
		asm("	decl %edi");                      // %edi--; (dst_ptr--);
		asm("move_two_bytes_top_to_bottom:");
		asm("	shr $1,%ecx");                    // %ecx >> 1, shifted bit -> CF
		asm("	jnc move_the_rest_top_to_bottom_directly"); // if !carry (CF == 0) skip this move
		// Move the next two bytes
		asm("	movsw %ds:(%esi),%es:(%edi)");    // *((word *)dst)-- = *((word)src)-- if DF else *((word *)dst)++ = *((word)src)++
		asm("move_the_rest_top_to_bottom_directly:");
		asm("	subl $2,%esi");                   // %esi-=2; (src-=2);
		asm("   subl $2,%edi");                   // %edi-=2; (dst-=2);
		asm("   jmp move_the_rest");              // call last repnz movsl
		// dst_ptr <= src_ptr
		asm("move_from_bottom_to_top:");
		asm("	cld");                            // clear direction flag (increment esi and edi in movsb)
		// Optimization : check for non-odd len (1,3,5,7...)
		asm("	shr $1,%ecx");                    // %ecx >> 1, shifted bit -> CF
		asm("	jnc move_two_bytes");             // if !carry (CF == 0) skip this move
		// Move the first byte (non-odd)
		asm("	movsb %ds:(%esi),%es:(%edi)");    // *dst-- = *src-- if DF  else *dst++ = *src++
		// Optimization : pass 2, check for %2 and %3
		asm("move_two_bytes:");
		asm("	shr $1,%ecx");                    // %ecx >> 1, shifted bit -> CF
		asm("	jnc move_the_rest");              // if !carry (CF == 0) skip this move
		// Move the next two bytes
		asm("	movsw %ds:(%esi),%es:(%edi)");    // *((word *)dst)-- = *((word)src)-- if DF else *((word *)dst)++ = *((word)src)++
		// Main move remaining part
		asm("move_the_rest:");
		asm("	repnz; movsl %ds:(%esi),%es:(%edi)"); // loop moving 4 bytes at once (increment or decrement as above)
		// Restore pointer registers
		asm("	popl %edi");                      // restore %edi
		asm("	popl %esi");                      // restore %esi
		return dst_ptr; //asm("   movl 8(%ebp),%eax"); <-- gcc will put that (AFTER THE OPTIMISATION PASS!)
	}

	void * move(void * dst_ptr,const void *src_ptr,int len)
	{
		KVI_ASSERT(dst_ptr);
		KVI_ASSERT(src_ptr);
		KVI_ASSERT(len >= 0);
		// Save pointer registers
		asm("	pushl %esi");                     // save %esi
		asm("	pushl %edi");                     // save %edi
		// Load arguments
		asm("	movl 16(%ebp),%ecx");             // %ecx = len
		asm("	movl 12(%ebp),%esi");             // %esi = src
		asm("	movl 8(%ebp),%edi");              // %edi = dst
		// Compare src and dest
		asm("	cmpl %esi,%edi");                 // %edi - %esi
		asm("	jbe xmove_from_bottom_to_top");    // if(%edi < %esi) jump to move_from_bottom_to_top
		// dst_ptr > src_ptr
		asm("	addl %ecx,%esi");                 // %esi += %ecx (src_ptr += len);
		asm("	addl %ecx,%edi");                 // %edi += %ecx (dst_ptr += len);
		asm("	std");                            // set direction flag (decrement esi and edi in movsb)
		// start moving
		asm("	shr $2,%ecx");                    // %ecx >> 2, last shifted bit -> CF
		asm("	jnc xmove_the_rest_top_to_bottom_directly"); // if !carry (CF == 0) skip this move
		// Move the next two bytes
		asm("	subl $2,%esi");                   // %esi-=2; (src_ptr-=2);
		asm("	subl $2,%edi");                   // %edi-=2; (dst_ptr-=2);
		asm("	movsw %ds:(%esi),%es:(%edi)");    // *((word *)dst)-- = *((word)src)-- if DF else *((word *)dst)++ = *((word)src)++
		asm("	subl $2,%esi");                   // %esi-=2; (src_ptr-=2);
		asm("	subl $2,%edi");                   // %edi-=2; (dst_ptr-=2);
		asm("   jmp xmove_the_rest");
		asm("xmove_the_rest_top_to_bottom_directly:");
		asm("	subl $4,%esi");                   // %esi-=4; (src-=4);
		asm("   subl $4,%edi");                   // %edi-=4; (dst-=4);
		asm("   jmp xmove_the_rest");              // call last repnz movsl
		// dst_ptr <= src_ptr
		asm("xmove_from_bottom_to_top:");
		asm("	cld");                            // clear direction flag (increment esi and edi in movsb)
		// move it
		asm("	shr $2,%ecx");                    // %ecx >> 2, last shifted bit -> CF
		asm("	jnc xmove_the_rest");              // if !carry (CF == 0) skip this move
		// Move the next two bytes
		asm("	movsw %ds:(%esi),%es:(%edi)");    // *((word *)dst)-- = *((word)src)-- if DF else *((word *)dst)++ = *((word)src)++
		// Main move remaining part
		asm("xmove_the_rest:");
		asm("	repnz; movsl %ds:(%esi),%es:(%edi)"); // loop moving 4 bytes at once (increment or decrement as above)
		// Restore pointer registers
		asm("	popl %edi");                      // restore %edi
		asm("	popl %esi");                      // restore %esi
		return dst_ptr; //asm("   movl 8(%ebp),%eax"); <-- gcc will put that (AFTER THE OPTIMISATION PASS!)
	}

#endif //COMPILE_ix86_ASM

	// The next 4 functions could be optimized with the & and shift technique
	// used in the assembly implementations but the compilers usually
	// will not translate the carry bit trick producing code
	// that works slower on short block of memory (really near the average case)

	// The trick would be:
	//
	//    if(len & 1) // the length is even
	//       *dst-- = *src--; // move one byte
	//    len >> 1; // drop the last bit (thus divide by 2)
	//    if(len & 1) // the length is still even
	//       *((short *)dst)-- = *((short *)src)--; // move two bytes
	//    len >> 1; // again drop the last bit (thus divide by 2)
	//    while(len--)*((int *)dst)-- = *((int *)src)--; // move four bytes at a time

	void * move(void *dst_ptr,const void *src_ptr,int len)
	{
		KVI_ASSERT(dst_ptr);
		KVI_ASSERT(src_ptr);
		KVI_ASSERT(len >= 0);
		register char *dst;
		register const char *src;
		if(dst_ptr > src_ptr){
			dst = (char *)dst_ptr + len - 1;
			src = (const char *)src_ptr + len - 1;
			while(len--)*dst-- = *src--;
		} else { //it is valid even if dst_ptr == src_ptr
			dst = (char *)dst_ptr;
			src = (const char *)src_ptr;
			while(len--)*dst++ = *src++;
		}
		return dst_ptr;
	}

	//=============================================================================
	//
	//   File : kvi_strasm.h
	//   Creation date : Sun Jun 18 2000 18:38:26 by Szymon Stefanek
	//
	//   This file is part of the KVIrc IRC client distribution
	//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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
	//   Inline assembly implementations of the commonly used string functions
	//   These will work only on i386 based machines and can be compiled
	//   only by gcc
	//

	bool kvi_strEqualCS(const char * pcStr1, const char * pcStr2)
	{
		// An instruction pattern is really useful in this case.
		// When inlining, GCC can optimize to load esi and edi
		// directly with the strings, without pushing and getting it
		// from the stack...
		register bool bEax;
		__asm__ __volatile__ (
			"	cld\n"
			"1:\n"
			"	lodsb %%ds:(%%esi),%%al\n"
			"	scasb %%es:(%%edi),%%al\n"
			"	jne 2f\n"
			"	testb %%al,%%al\n"
			"	jne 1b\n"
			"	movl $0x1,%%eax\n"
			"	jmp 3f\n"
			"2:\n"
			"	xorl %%eax,%%eax\n"
			"3:"
			: "=a" (bEax), "=&S" (pcStr1), "=&D" (pcStr2)
			:             "1"   (pcStr1), "2"   (pcStr2)
		);
		return bEax;
	}

	bool kvi_strEqualCSN(const char * pcStr1, const char * pcStr2, int iLen)
	{
		register bool bEax;
		__asm__ __volatile__ (
			"1:\n"
			"	decl %3\n"
			"	js 2f\n"
			"	movb (%1),%%al\n"
			"	incl %1\n"
			"	cmpb %%al,(%2)\n"
			"	jne 3f\n"
			"	incl %2\n"
			"	testb %%al,%%al\n"
			"	jne 1b\n"
			"2:\n"
			"	movl $0x1,%%eax\n"
			"	jmp 4f\n"
			"3:\n"
			"	xorl %%eax,%%eax\n"
			"4:\n"
			: "=a" (bEax),  "=r" (pcStr1), "=r" (pcStr2), "=r" (iLen)
			:              "1"  (pcStr1), "2"  (pcStr2), "3"  (iLen)
		);
		return bEax;
	}

	// OPTIMIZATION
	// The following two functions are used to compare a variable string with one in that
	// only A-Z<->a-z case insensivity is significant.
	// For example
	// kvi_strEqualNoLocalCI("a string that does not contain any strange char",str2)
	// will always give the correct result
	// These will NOT work with localizable characters:
	// 'a' with umlaut will be not equal to 'A' with umlaut

	bool kvi_strEqualNoLocaleCI(const char * pcStr1, const char * pcStr2)
	{
		// Trivial implementation
		// Ignores completely locales....only A-Z chars are transformed to a-z
		// Anyway...it will work for IRC :)
		register int iReg;
		register bool bEax;
		__asm__ __volatile__ (
			"1:\n"
			"	movb (%2),%%al\n"
			"	cmpb $65,%%al\n"
			"	jb 2f\n"
			"	cmpb $90,%%al\n"
			"	ja 2f\n"
			"	addb $32,%%al\n"
			"2:\n"
			"	movb (%3),%b1\n"
			"	cmpb $65,%b1\n"
			"	jb 3f\n"
			"	cmpb $90,%b1\n"
			"	ja 3f\n"
			"	addb $32,%b1\n"
			"3:\n"
			"	cmpb %%al,%b1\n"
			"	jne 4f\n"
			"	incl %2\n"
			"	incl %3\n"
			"	testb %%al,%%al\n"
			"	jne 1b\n"
			"	movl $1,%%eax\n"
			"	jmp 5f\n"
			"4:\n"
			"	xorl %%eax,%%eax\n"
			"5:\n"
			: "=a" (bEax), "=q" (iReg), "=r" (pcStr1), "=r" (pcStr2)
			:                         "2"  (pcStr1), "3"  (pcStr2)
		);
		return bEax;
	}

	bool kvi_strEqualNoLocaleCIN(const char * pcStr1, const char * pcStr2, int iLen)
	{

		register int iReg;
		register bool bEax;
		__asm__ __volatile__ (
			"1:\n"
			"	decl %4\n"
			"	js 4f\n"
			"	movb (%2),%%al\n"
			"	cmpb $65,%%al\n"
			"	jb 2f\n"
			"	cmpb $90,%%al\n"
			"	ja 2f\n"
			"	addb $32,%%al\n"
			"2:\n"
			"	movb (%3),%b1\n"
			"	cmpb $65,%b1\n"
			"	jb 3f\n"
			"	cmpb $90,%b1\n"
			"	ja 3f\n"
			"	addb $32,%b1\n"
			"3:\n"
			"	cmpb %%al,%b1\n"
			"	jne 5f\n"
			"	incl %2\n"
			"	incl %3\n"
			"	testb %%al,%%al\n"
			"	jne 1b\n"
			"4:\n"
			"	movl $1,%%eax\n"
			"	jmp 6f\n"
			"5:\n"
			"	xorl %%eax,%%eax\n"
			"6:\n"
			: "=a" (bEax), "=q" (iReg), "=r" (pcStr1), "=r" (pcStr2), "=r" (iLen)
			:                         "2"  (pcStr1), "3"  (pcStr2), "4"  (iLen)
		);
		return bEax;
	}

	int kvi_strLen(const char * pcStr)
	{
		register int iEcx;
		__asm__ __volatile__(
			"	cld\n"
			"	repne\n"
			"	scasb\n"
			"	notl %0\n"
			"	decl %0"
			: "=c" (iEcx),        "=&D" (pcStr)
			: "0"  (0xffffffff), "1"   (pcStr), "a" (0)
		);
		return iEcx;
	}

#endif //0
}
