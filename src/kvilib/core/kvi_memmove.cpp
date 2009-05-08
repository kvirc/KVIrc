//=============================================================================
//
//   File : kvi_memmove.cpp
//   Creation date : Sun Jun 18 2000 18:27:50 CEST by Szymon Stefanek
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


#define _KVI_DEBUG_CHECK_RANGE_
#include "kvi_debug.h"

#define _KVI_MEMMOVE_CPP_
#include "kvi_memmove.h"

// FIXME: #warning "With system memmove could be guessed by configure"

#ifndef COMPILE_WITH_SYSTEM_MEMMOVE

	#ifdef COMPILE_ix86_ASM


		void *kvi_memmove(void * dst_ptr,const void *src_ptr,int len)
		{
			__range_valid(dst_ptr);
			__range_valid(src_ptr);
			__range_valid(len >= 0);
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
			asm("	shr $1,%ecx");                    // %ecx >> 1 , shifted bit -> CF
			asm("	jnc move_two_bytes_top_to_bottom_directly");  // if !carry (CF == 0) skip this move
			// Move the first byte (non-odd)
			asm("	movsb %ds:(%esi),%es:(%edi)");    // *dst-- = *src-- if DF  else *dst++ = *src++
			asm("move_two_bytes_top_to_bottom_directly:");
			asm("	decl %esi");                      // %esi--; (src_ptr--);
			asm("	decl %edi");                      // %edi--; (dst_ptr--);
			asm("move_two_bytes_top_to_bottom:");
			asm("	shr $1,%ecx");                    // %ecx >> 1 , shifted bit -> CF
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
			asm("	shr $1,%ecx");                    // %ecx >> 1 , shifted bit -> CF
			asm("	jnc move_two_bytes");             // if !carry (CF == 0) skip this move
			// Move the first byte (non-odd)
			asm("	movsb %ds:(%esi),%es:(%edi)");    // *dst-- = *src-- if DF  else *dst++ = *src++
			// Optimization : pass 2 , check for %2 and %3
			asm("move_two_bytes:");
			asm("	shr $1,%ecx");                    // %ecx >> 1 , shifted bit -> CF
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

		void *kvi_memmoveodd(void * dst_ptr,const void *src_ptr,int len)
		{
			__range_valid(dst_ptr);
			__range_valid(src_ptr);
			__range_valid(len >= 0);
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
			asm("	shr $2,%ecx");                    // %ecx >> 2 , last shifted bit -> CF
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
			asm("	shr $2,%ecx");                    // %ecx >> 2 , last shifted bit -> CF
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

	#else // ndef COMPILE_ix86_ASM



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
		//
		//

		void *kvi_memmove(void *dst_ptr,const void *src_ptr,int len)
		{
			__range_valid(dst_ptr);
			__range_valid(src_ptr);
			__range_valid(len >= 0);
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

		void *kvi_memmoveodd(void *dst_ptr,const void *src_ptr,int len)
		{
			__range_valid(dst_ptr);
			__range_valid(src_ptr);
			__range_valid(len >= 0);
			__range_valid((len & 1) == 0);
			register short *dst;
			register const short *src;
			if(dst_ptr > src_ptr){
				dst = (short *) (((char *)dst_ptr) + len - 2);
				src = (const short *) (((const char *)src_ptr) + len - 2);
				while(len > 0)
				{
					*dst-- = *src--;
					len -= 2;
				}
			} else { //it is valid even if dst_ptr == src_ptr
				dst = (short *)dst_ptr;
				src = (const short *)src_ptr;
				while(len > 0)
				{
					*dst++ = *src++;
					len -= 2;
				}
			}
			return dst_ptr;
		}

		void kvi_fastmove(void *dst_ptr,const void *src_ptr,int len)
		{
			__range_valid(dst_ptr);
			__range_valid(src_ptr);
			__range_valid(len >= 0);
			register const char *src = (const char *)src_ptr;
			register char *dst = (char *)dst_ptr;
			while(len--)*dst++ = *src++;
		}

		void kvi_fastmoveodd(void *dst_ptr,const void *src_ptr,int len)
		{
			__range_valid(dst_ptr);
			__range_valid(src_ptr);
			__range_valid(len >= 0);
			__range_valid((len & 1) == 0);
			register const short *src = (const short *)src_ptr;
			register short *dst = (short *)dst_ptr;
			while(len > 0){
				*dst++ = *src++;
				len -= 2;
			}
		}

	#endif // !COMPILE_ix86_ASM

	void kvi_memset(void *dst_ptr,char c,int len)
	{
		__range_valid(dst_ptr);
		__range_valid(len >= 0);
		register char *dst = (char *)dst_ptr;
		while(len--)*dst++ = c;
	}

#endif // !COMPILE_WITH_SYSTEM_MEMMOVE
