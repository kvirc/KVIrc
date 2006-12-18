//=============================================================================
//
//   File : kvi_file.cpp
//   Creation date : Mon Dec 17 2001 00:04:12 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2002 Szymon Stefanek (pragma at kvirc dot net)
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

#define __KVILIB__


#include "kvi_file.h"
#include "kvi_bswap.h"


KviFile::KviFile()
: QFile()
{
}

KviFile::KviFile(const QString &name)
: QFile(name)
{
}

KviFile::~KviFile()
{
}


bool KviFile::save(const QString &szData)
{
	QCString c = szData.utf8();
	if(!save(c.length()))return false;
	return (writeBlock(c.data(),c.length()) == ((int)(c.length())));
}

bool KviFile::load(QString &szData)
{
	int iLen;
	if(!load(iLen))return false;
	QCString tmp;
	tmp.resize(iLen + 1);
	if(readBlock((char *)(tmp.data()),iLen) != iLen)return false;
	*(tmp.data() + iLen) = 0;
	szData = QString::fromUtf8(tmp.data());
	return true;
}

bool KviFile::save(const KviStr &szData)
{
	if(!save(szData.len()))return false;
	return (writeBlock(szData.ptr(),szData.len()) == (int) szData.len());
}

bool KviFile::load(KviStr &szData)
{
	int iLen;
	if(!load(iLen))return false;
	szData.setLength(iLen);
	return (readBlock((char *)(szData.ptr()),iLen) == iLen);
}

bool KviFile::save(kvi_i32_t t)
{
#ifdef BIGENDIAN_MACHINE_BYTE_ORDER
	t = kvi_swap32(t);
#endif
	return (writeBlock((const char *)(&t),sizeof(kvi_i32_t)) == sizeof(kvi_i32_t));
}

bool KviFile::load(kvi_i32_t &t)
{
#ifdef BIGENDIAN_MACHINE_BYTE_ORDER
	if(!(readBlock((char *)(&t),sizeof(kvi_i32_t)) == sizeof(kvi_i32_t)))return false;
	t = kvi_swap32(t);
	return true;
#else
	return (readBlock((char *)(&t),sizeof(kvi_i32_t)) == sizeof(kvi_i32_t));
#endif
}

bool KviFile::save(kvi_i16_t t)
{
#ifdef BIGENDIAN_MACHINE_BYTE_ORDER
	t = kvi_swap16(t);
#endif
	return (writeBlock((const char *)(&t),sizeof(kvi_i16_t)) == sizeof(kvi_i16_t));
}

bool KviFile::load(kvi_i16_t &t)
{
#ifdef BIGENDIAN_MACHINE_BYTE_ORDER
	if(!(readBlock((char *)(&t),sizeof(kvi_i16_t)) == sizeof(kvi_i16_t)))return false;
	t = kvi_swap32(t);
	return true;
#else
	return (readBlock((char *)(&t),sizeof(kvi_i16_t)) == sizeof(kvi_i16_t));
#endif
}

bool KviFile::save(kvi_i8_t t)
{
	return (writeBlock((const char *)(&t),sizeof(kvi_i8_t)) == sizeof(kvi_i8_t));
}

bool KviFile::load(kvi_i8_t &t)
{
	return (readBlock((char *)(&t),sizeof(kvi_i8_t)) == sizeof(kvi_i8_t));
}




bool KviFile::save(KviPtrList<KviStr> * pData)
{
	if(!save((int)(pData->count())))return false;
	for(KviStr * s = pData->first();s;s = pData->next())
	{
		if(!save(*s))return false;
	}
	return true;
}

bool KviFile::load(KviPtrList<KviStr> * pData)
{
	pData->clear();
	int iCount;
	if(!load(iCount))return false;
	for(int i=0;i<iCount;i++)
	{
		KviStr * s = new KviStr();
		if(!load(*s))
		{
			delete s;
            s = 0;
			return false;
		}
		pData->append(s);
	}
	return true;
}

bool KviFile::skipFirst(char t,unsigned int maxdist)
{
	while(maxdist > 0)
	{
		int c = getch();
		if(((char)c) == t)return true;
		if(c == -1)return false;
		maxdist--;
	}
	return false;
}

bool KviFile::skipFirst(const KviStr &t,unsigned int maxdist)
{
	char * ptr = t.ptr();
	while(maxdist > 0)
	{
		int c = getch();
		if(c == *ptr)
		{
			ptr++;
			if(!*ptr)return true;
		} else {
			if(c == -1)return false;
			ptr = t.ptr();
		}
		maxdist--;
	}
	return false;
}

