//
//   File : kvi_md5.cpp
//   Creation date : Wed Sep  4 22:16:45 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//

#define __KVILIB__



/*
   ######################################################################
   
   MD5Sum - MD5 Message Digest Algorithm.

   This code implements the MD5 message-digest algorithm. The algorithm is 
   due to Ron Rivest.  This code was written by Colin Plumb in 1993, no 
   copyright is claimed. This code is in the public domain; do with it what 
   you wish.
 
   Equivalent code is available from RSA Data Security, Inc. This code has 
   been tested against that, and is equivalent, except that you don't need to 
   include two pages of legalese with every copy.

   To compute the message digest of a chunk of bytes, instantiate the class,
   and repeatedly call one of the Add() members. When finished the Result 
   method will return the Hash and finalize the value.
   
   Changed so as no longer to depend on Colin Plumb's `usual.h' header
   definitions; now uses stuff from dpkg's config.h.
    - Ian Jackson <ijackson@nyx.cs.du.edu>.
   
   Changed into a C++ interface and made work with APT's config.h.
    - Jason Gunthorpe <jgg@gpu.srv.ualberta.ca>

   Interface adapted to the KVIrc irc client
    - Szymon Stefanek <pragma at kvirc dot net>
   
   The classes use arrays of char that are a specific size. We cast those
   arrays to uint8_t's and go from there. This allows us to advoid using
   the uncommon inttypes.h in a public header or internally newing memory.
   In theory if C9x becomes nicely accepted
   
   ##################################################################### */

#include "kvi_md5.h"
#include "kvi_settings.h"
#include "kvi_bswap.h"
#include "kvi_memmove.h"

/* Swap n 32 bit longs in given buffer */
#ifdef BIG_ENDIAN_MACHINE_BYTE_ORDER
	static void byteSwap(kvi_u32_t *buf,unsigned int words)
	{
//	   kvi_u8_t *p = (kvi_u8_t *)buf;
//	   do 
//	   {
//	      *buf++ = (kvi_u32_t)((unsigned)p[3] << 8 | p[2]) << 16 | ((unsigned)p[1] << 8 | p[0]);
//	      p += 4;
//	   } while (--words);
		do {
			*buf = kvi_swap32(*buf);
			buf++;
		} while(--words);
	}
#else
	#define byteSwap(buf,words)
#endif

/* The core of the MD5 algorithm, this alters an existing MD5 hash to
   reflect the addition of 16 longwords of new data. Add blocks
   the data and converts bytes into longwords for this routine. */

// The four core functions - F1 is optimized somewhat
// #define F1(x, y, z) (x & y | ~x & z)
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

// This is the central step in the MD5 algorithm.
#define MD5STEP(f,w,x,y,z,in,s) \
	 (w += f(x,y,z) + in, w = (w<<s | w>>(32-s)) + x)

static void MD5Transform(kvi_u32_t buf[4],const kvi_u32_t in[16])
{
   register kvi_u32_t a, b, c, d;
   
   a = buf[0];
   b = buf[1];
   c = buf[2];
   d = buf[3];
   
   MD5STEP(F1, a, b, c, d, in[0] + 0xd76aa478, 7);
   MD5STEP(F1, d, a, b, c, in[1] + 0xe8c7b756, 12);
   MD5STEP(F1, c, d, a, b, in[2] + 0x242070db, 17);
   MD5STEP(F1, b, c, d, a, in[3] + 0xc1bdceee, 22);
   MD5STEP(F1, a, b, c, d, in[4] + 0xf57c0faf, 7);
   MD5STEP(F1, d, a, b, c, in[5] + 0x4787c62a, 12);
   MD5STEP(F1, c, d, a, b, in[6] + 0xa8304613, 17);
   MD5STEP(F1, b, c, d, a, in[7] + 0xfd469501, 22);
   MD5STEP(F1, a, b, c, d, in[8] + 0x698098d8, 7);
   MD5STEP(F1, d, a, b, c, in[9] + 0x8b44f7af, 12);
   MD5STEP(F1, c, d, a, b, in[10] + 0xffff5bb1, 17);
   MD5STEP(F1, b, c, d, a, in[11] + 0x895cd7be, 22);
   MD5STEP(F1, a, b, c, d, in[12] + 0x6b901122, 7);
   MD5STEP(F1, d, a, b, c, in[13] + 0xfd987193, 12);
   MD5STEP(F1, c, d, a, b, in[14] + 0xa679438e, 17);
   MD5STEP(F1, b, c, d, a, in[15] + 0x49b40821, 22);

   MD5STEP(F2, a, b, c, d, in[1] + 0xf61e2562, 5);
   MD5STEP(F2, d, a, b, c, in[6] + 0xc040b340, 9);
   MD5STEP(F2, c, d, a, b, in[11] + 0x265e5a51, 14);
   MD5STEP(F2, b, c, d, a, in[0] + 0xe9b6c7aa, 20);
   MD5STEP(F2, a, b, c, d, in[5] + 0xd62f105d, 5);
   MD5STEP(F2, d, a, b, c, in[10] + 0x02441453, 9);
   MD5STEP(F2, c, d, a, b, in[15] + 0xd8a1e681, 14);
   MD5STEP(F2, b, c, d, a, in[4] + 0xe7d3fbc8, 20);
   MD5STEP(F2, a, b, c, d, in[9] + 0x21e1cde6, 5);
   MD5STEP(F2, d, a, b, c, in[14] + 0xc33707d6, 9);
   MD5STEP(F2, c, d, a, b, in[3] + 0xf4d50d87, 14);
   MD5STEP(F2, b, c, d, a, in[8] + 0x455a14ed, 20);
   MD5STEP(F2, a, b, c, d, in[13] + 0xa9e3e905, 5);
   MD5STEP(F2, d, a, b, c, in[2] + 0xfcefa3f8, 9);
   MD5STEP(F2, c, d, a, b, in[7] + 0x676f02d9, 14);
   MD5STEP(F2, b, c, d, a, in[12] + 0x8d2a4c8a, 20);
   
   MD5STEP(F3, a, b, c, d, in[5] + 0xfffa3942, 4);
   MD5STEP(F3, d, a, b, c, in[8] + 0x8771f681, 11);
   MD5STEP(F3, c, d, a, b, in[11] + 0x6d9d6122, 16);
   MD5STEP(F3, b, c, d, a, in[14] + 0xfde5380c, 23);
   MD5STEP(F3, a, b, c, d, in[1] + 0xa4beea44, 4);
   MD5STEP(F3, d, a, b, c, in[4] + 0x4bdecfa9, 11);
   MD5STEP(F3, c, d, a, b, in[7] + 0xf6bb4b60, 16);
   MD5STEP(F3, b, c, d, a, in[10] + 0xbebfbc70, 23);
   MD5STEP(F3, a, b, c, d, in[13] + 0x289b7ec6, 4);
   MD5STEP(F3, d, a, b, c, in[0] + 0xeaa127fa, 11);
   MD5STEP(F3, c, d, a, b, in[3] + 0xd4ef3085, 16);
   MD5STEP(F3, b, c, d, a, in[6] + 0x04881d05, 23);
   MD5STEP(F3, a, b, c, d, in[9] + 0xd9d4d039, 4);
   MD5STEP(F3, d, a, b, c, in[12] + 0xe6db99e5, 11);
   MD5STEP(F3, c, d, a, b, in[15] + 0x1fa27cf8, 16);
   MD5STEP(F3, b, c, d, a, in[2] + 0xc4ac5665, 23);
   
   MD5STEP(F4, a, b, c, d, in[0] + 0xf4292244, 6);
   MD5STEP(F4, d, a, b, c, in[7] + 0x432aff97, 10);
   MD5STEP(F4, c, d, a, b, in[14] + 0xab9423a7, 15);
   MD5STEP(F4, b, c, d, a, in[5] + 0xfc93a039, 21);
   MD5STEP(F4, a, b, c, d, in[12] + 0x655b59c3, 6);
   MD5STEP(F4, d, a, b, c, in[3] + 0x8f0ccc92, 10);
   MD5STEP(F4, c, d, a, b, in[10] + 0xffeff47d, 15);
   MD5STEP(F4, b, c, d, a, in[1] + 0x85845dd1, 21);
   MD5STEP(F4, a, b, c, d, in[8] + 0x6fa87e4f, 6);
   MD5STEP(F4, d, a, b, c, in[15] + 0xfe2ce6e0, 10);
   MD5STEP(F4, c, d, a, b, in[6] + 0xa3014314, 15);
   MD5STEP(F4, b, c, d, a, in[13] + 0x4e0811a1, 21);
   MD5STEP(F4, a, b, c, d, in[4] + 0xf7537e82, 6);
   MD5STEP(F4, d, a, b, c, in[11] + 0xbd3af235, 10);
   MD5STEP(F4, c, d, a, b, in[2] + 0x2ad7d2bb, 15);
   MD5STEP(F4, b, c, d, a, in[9] + 0xeb86d391, 21);
   
   buf[0] += a;
   buf[1] += b;
   buf[2] += c;
   buf[3] += d;
}

KviMd5::KviMd5()
{
   kvi_u32_t *buf   = (kvi_u32_t *)m_pBuf;
   kvi_u32_t *bytes = (kvi_u32_t *)m_pBytes;
   
   buf[0] = 0x67452301;
   buf[1] = 0xefcdab89;
   buf[2] = 0x98badcfe;
   buf[3] = 0x10325476;
   
   bytes[0] = 0;
   bytes[1] = 0;

   m_bDone = false;
}

KviMd5::~KviMd5()
{
}

bool KviMd5::add(const unsigned char *data,unsigned long len)
{
	if(m_bDone)return false;

	kvi_u32_t *buf = (kvi_u32_t *)m_pBuf;
	kvi_u32_t *bytes = (kvi_u32_t *)m_pBytes;
	kvi_u32_t *in = (kvi_u32_t *)m_pIn;

	// Update byte count and carry (this could be done with a long long?)
	kvi_u32_t t = bytes[0];

	if ((bytes[0] = t + len) < t)bytes[1]++;	

	// Space available (at least 1)
	t = 64 - (t & 0x3f);	
	if (t > len) 
	{
		kvi_fastmove((unsigned char *)in + 64 - t,data,len);
		return true;
	}

	// First chunk is an odd size
	kvi_fastmove((unsigned char *)in + 64 - t,data,t);
	byteSwap(in, 16);

	MD5Transform(buf,in);
	data += t;
	len -= t;
   
	// Process data in 64-byte chunks
	while (len >= 64)
	{
		kvi_fastmove(in,data,64);
		byteSwap(in,16);
		MD5Transform(buf,in);
		data += 64;
		len -= 64;
	}

	// Handle any remaining bytes of data.
	kvi_memmove(in,data,len);

	return true;   
}


// ---------------------------------------------------------------------
/* Because this must add in the last bytes of the series it prevents anyone
   from calling add after. */

KviStr KviMd5::result()
{
	kvi_u32_t *buf = (kvi_u32_t *)m_pBuf;
	kvi_u32_t *bytes = (kvi_u32_t *)m_pBytes;
	kvi_u32_t *in = (kvi_u32_t *)m_pIn;
   
	if(!m_bDone)
	{
		// Number of bytes in In
		int count = bytes[0] & 0x3f;	
		unsigned char *p = (unsigned char *)in + count;

		// Set the first char of padding to 0x80.  There is always room.
		*p++ = 0x80;

		// Bytes of padding needed to make 56 bytes (-8..55)
		count = 56 - 1 - count;

		// Padding forces an extra block 
		if (count < 0) 
		{
			kvi_memset(p,0,count + 8);
			byteSwap(in, 16);
			MD5Transform(buf,in);
			p = (unsigned char *)in;
			count = 56;
		}

		kvi_memset(p, 0, count);
		byteSwap(in, 14);

		// Append length in bits and transform
		in[14] = bytes[0] << 3;
		in[15] = bytes[1] << 3 | bytes[0] >> 29;
		MD5Transform(buf,in);   
		byteSwap(buf,4);
		m_bDone = true;
	}

	// m_pBuf now contains the md5 sum
	KviStr ret;
	ret.bufferToHex((char *)m_pBuf,16);

	return ret;
}
