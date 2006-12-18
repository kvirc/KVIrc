#ifndef _KVI_MD5_H_
#define _KVI_MD5_H_
//
//   File : kvi_md5.h
//   Creation date : Wed Sep  4 22:16:44 2002 GMT by Szymon Stefanek
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//



/* ######################################################################
   
   MD5SumValue - Storage for a MD5Sum
   MD5Summation - MD5 Message Digest Algorithm.
   
   This is a C++ interface to a set of MD5Sum functions. The class can
   store a MD5Sum in 16 bytes of memory.
   
   A MD5Sum is used to generate a (hopefully) unique 16 byte number for a
   block of data. This can be used to gaurd against corruption of a file.
   MD5 should not be used for tamper protection, use SHA or something more
   secure.
   
   There are two classes because computing a MD5 is not a continual 
   operation unless 64 byte blocks are used. Also the summation requires an
   extra 18*4 bytes to operate.
   
   ##################################################################### */

#include "kvi_settings.h"
#include "kvi_inttypes.h"
#include "kvi_string.h"

class KviMd5
{
public:
	KviMd5();
	~KviMd5();
protected:
	unsigned char m_pBuf[4*4];
	unsigned char m_pBytes[2*4];
	unsigned char m_pIn[16*4];
	bool          m_bDone;

public:
	bool add(const unsigned char *Data,unsigned long Size);

	KviStr result();
};


#endif //_KVI_MD5_H_
