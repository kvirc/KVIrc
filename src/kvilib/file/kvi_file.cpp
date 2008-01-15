//=============================================================================
//
//   File : kvi_file.cpp
//   Creation date : Mon Dec 17 2001 00:04:12 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2007 Szymon Stefanek (pragma at kvirc dot net)
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
#include "kvi_byteorder.h"


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

bool KviFile::openForReading()
{
#ifdef COMPILE_USE_QT4
	return open(QFile::ReadOnly);
#else
	return open(IO_ReadOnly);
#endif
}

bool KviFile::openForWriting(bool bAppend)
{
#ifdef COMPILE_USE_QT4
	return open(QFile::WriteOnly | (bAppend ? QFile::Append : QFile::Truncate));
#else
	return open(IO_WriteOnly | (bAppend ? IO_Append : IO_Truncate));
#endif
}


bool KviFile::save(const QByteArray &bData)
{
	if(!save((kvi_u32_t)(bData.size())))return false;
	return (writeBlock(bData.data(),bData.size()) == ((int)(bData.size())));
}

bool KviFile::load(QByteArray &bData)
{
	kvi_u32_t iLen;
	if(!load(iLen))return false;
	bData.resize(iLen); // it is automatically null terminated in Qt 4.x... BLEAH :D
	if(readBlock((char *)(bData.data()),iLen) != iLen)return false;
	return true;
}

#ifndef COMPILE_USE_QT4

bool KviFile::save(const KviQCString &szData)
{
	if(!save((kvi_u32_t)(szData.length())))return false;
	return (writeBlock(szData.data(),szData.length()) == ((int)(szData.length())));
}

bool KviFile::load(KviQCString &szData)
{
	kvi_u32_t iLen;
	if(!load(iLen))return false;
	szData.resize(iLen + 1); // this would allocate one extra byte with Qt 4.x...
	if(readBlock((char *)(szData.data()),iLen) != iLen)return false;
	*(szData.data() + iLen) = 0;
	return true;
}

#endif


bool KviFile::save(const QString &szData)
{
	KviQCString c = KviQString::toUtf8(szData);
	if(!save((kvi_u32_t)(c.length())))return false;
	return (writeBlock(c.data(),c.length()) == ((int)(c.length())));
}

bool KviFile::load(QString &szData)
{
	kvi_u32_t iLen;
	if(!load(iLen))return false;
	KviQCString tmp;
	tmp.resize(iLen + 1);
	if(readBlock((char *)(tmp.data()),iLen) != iLen)return false;
	*(tmp.data() + iLen) = 0;
	szData = QString::fromUtf8(tmp.data());
	return true;
}

bool KviFile::save(const KviStr &szData)
{
	if(!save((kvi_u32_t)(szData.len())))return false;
	return (writeBlock(szData.ptr(),szData.len()) == (int) szData.len());
}

bool KviFile::load(KviStr &szData)
{
	kvi_u32_t iLen;
	if(!load(iLen))return false;
	szData.setLength(iLen);
	return (readBlock((char *)(szData.ptr()),iLen) == iLen);
}

bool KviFile::save(kvi_u32_t t)
{
#ifndef LOCAL_CPU_LITTLE_ENDIAN
	t = kvi_localCpuToLittleEndian32(t);
#endif
	return (writeBlock((const char *)(&t),sizeof(kvi_u32_t)) == sizeof(kvi_u32_t));
}

bool KviFile::load(kvi_u32_t &t)
{
	if(!(readBlock((char *)(&t),sizeof(kvi_u32_t)) == sizeof(kvi_u32_t)))return false;
#ifndef LOCAL_CPU_LITTLE_ENDIAN
	t = kvi_littleEndianToLocalCpu32(t);
#endif
	return true;
}

bool KviFile::save(kvi_u64_t t)
{
#ifndef LOCAL_CPU_LITTLE_ENDIAN
	t = kvi_localCpuToLittleEndian64(t);
#endif
	return (writeBlock((const char *)(&t),sizeof(kvi_u64_t)) == sizeof(kvi_u64_t));
}

bool KviFile::load(kvi_u64_t &t)
{
	if(!(readBlock((char *)(&t),sizeof(kvi_u32_t)) == sizeof(kvi_u32_t)))return false;
#ifndef LOCAL_CPU_LITTLE_ENDIAN
	t = kvi_littleEndianToLocalCpu32(t);
#endif
	return true;
}


bool KviFile::save(kvi_u16_t t)
{
#ifndef LOCAL_CPU_LITTLE_ENDIAN
	t = kvi_localCpuToLittleEndian16(t);
#endif
	return (writeBlock((const char *)(&t),sizeof(kvi_u16_t)) == sizeof(kvi_u16_t));
}

bool KviFile::load(kvi_u16_t &t)
{
	if(!(readBlock((char *)(&t),sizeof(kvi_u16_t)) == sizeof(kvi_u16_t)))return false;
#ifndef LOCAL_CPU_LITTLE_ENDIAN
	t = kvi_littleEndianToLocalCpu16(t);
#endif
	return true;
}

bool KviFile::save(kvi_u8_t t)
{
	return (writeBlock((const char *)(&t),sizeof(kvi_u8_t)) == sizeof(kvi_u8_t));
}

bool KviFile::load(kvi_u8_t &t)
{
	return (readBlock((char *)(&t),sizeof(kvi_u8_t)) == sizeof(kvi_u8_t));
}


bool KviFile::save(KviPointerList<KviStr> * pData)
{
	if(!save((int)(pData->count())))return false;
	for(KviStr * s = pData->first();s;s = pData->next())
	{
		if(!save(*s))return false;
	}
	return true;
}

bool KviFile::load(KviPointerList<KviStr> * pData)
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
		char c;
		if(!getChar(&c))return false;
		if(((char)c) == t)return true;
		maxdist--;
	}
	return false;
}

bool KviFile::skipFirst(const KviStr &t,unsigned int maxdist)
{
	char * ptr = t.ptr();
	while(maxdist > 0)
	{
		char c;
		if(!getChar(&c))return false;
		if(c == *ptr)
		{
			ptr++;
			if(!*ptr)return true;
		} else {
			ptr = t.ptr();
		}
		maxdist--;
	}
	return false;
}

