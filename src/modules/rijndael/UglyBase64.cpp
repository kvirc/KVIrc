//=============================================================================
//
//   File : UglyBase64.cpp
//   Creation date : Sun Feb 6 2011 22:25:10 CEST by Fabio Bas
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2011 Fabio Bas (ctrlaltca at gmail dot com)
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

#include "UglyBase64.h"
#include "KviCString.h"
#include "KviMemory.h"

namespace UglyBase64
{
	static unsigned char fake_base64[] = "./0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

	void byteswap_buffer(unsigned char * p, int len)
	{
		while(len > 0)
		{
			unsigned char aux = p[0];
			p[0] = p[3];
			p[3] = aux;
			aux = p[1];
			p[1] = p[2];
			p[2] = aux;
			p += 4;
			len -= 4;
		}
	}

	unsigned int fake_base64dec(unsigned char c)
	{
		static char base64unmap[255];
		static bool didinit = false;

		if(!didinit)
		{
			// initialize base64unmap
			for(char & i : base64unmap)
				i = 0;
			for(int i = 0; i < 64; ++i)
				base64unmap[fake_base64[i]] = i;
			didinit = true;
		}

		return base64unmap[c];
	}

	void encode(const unsigned char * out, const int len, KviCString & szText)
	{
		// FIXME: this is probably needed only on LittleEndian machines!
		byteswap_buffer((unsigned char *)out, len);

		// da uglybase64 encoding
		const unsigned char * outb = out;
		const unsigned char * oute = out + len;

		int ll = (len * 3) / 2;
		szText.setLen(ll);

		unsigned char * p = (unsigned char *)szText.ptr();
		while(outb < oute)
		{
			quint32 * dd1 = (quint32 *)outb;
			outb += 4;
			quint32 * dd2 = (quint32 *)outb;
			outb += 4;
			*p++ = fake_base64[*dd2 & 0x3f];
			*dd2 >>= 6;
			*p++ = fake_base64[*dd2 & 0x3f];
			*dd2 >>= 6;
			*p++ = fake_base64[*dd2 & 0x3f];
			*dd2 >>= 6;
			*p++ = fake_base64[*dd2 & 0x3f];
			*dd2 >>= 6;
			*p++ = fake_base64[*dd2 & 0x3f];
			*dd2 >>= 6;
			*p++ = fake_base64[*dd2 & 0x3f];

			*p++ = fake_base64[*dd1 & 0x3f];
			*dd1 >>= 6;
			*p++ = fake_base64[*dd1 & 0x3f];
			*dd1 >>= 6;
			*p++ = fake_base64[*dd1 & 0x3f];
			*dd1 >>= 6;
			*p++ = fake_base64[*dd1 & 0x3f];
			*dd1 >>= 6;
			*p++ = fake_base64[*dd1 & 0x3f];
			*dd1 >>= 6;
			*p++ = fake_base64[*dd1 & 0x3f];
		}
	}

	void decode(KviCString & szText, unsigned char ** buf, int * len)
	{
		// make sure its length is multiple of 12 (eventually pad with zeroes)
		if(szText.len() % 12)
		{
			int oldL = szText.len();
			szText.setLen(szText.len() + (12 - (szText.len() % 12)));
			char * padB = szText.ptr() + oldL;
			char * padE = szText.ptr() + szText.len();
			while(padB < padE)
				*padB++ = 0;
		}

		*len = (int)(szText.len() * 2) / 3;

		*buf = (unsigned char *)KviMemory::allocate(*len);
		unsigned char * p = (unsigned char *)szText.ptr();
		unsigned char * e = p + szText.len();
		int i;
		unsigned char * bufp = *buf;
		while(p < e)
		{
			quint32 * dw1 = (quint32 *)bufp;
			bufp += 4;
			quint32 * dw2 = (quint32 *)bufp;
			bufp += 4;
			*dw2 = 0;
			for(i = 0; i < 6; i++)
				*dw2 |= (fake_base64dec(*p++)) << (i * 6);
			*dw1 = 0;
			for(i = 0; i < 6; i++)
				*dw1 |= (fake_base64dec(*p++)) << (i * 6);
		}

		// FIXME: this is probably needed only on LittleEndian machines!
		byteswap_buffer(*buf, *len);
	}
};
