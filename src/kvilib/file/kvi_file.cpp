//=============================================================================
//
//   File : kvi_file.cpp
//   Creation date : Mon Dec 17 2001 00:04:12 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2009 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2010 Elvio Basello (hell at hellvis69 dot netsons dot org)
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

#include "kvi_file.h"
#include "kvi_byteorder.h"
#include "kvi_qstring.h"

#include <QByteArray>

KviFile::KviFile()
: QFile()
{
}

KviFile::KviFile(const QString & szName)
: QFile(szName)
{
}

KviFile::~KviFile()
{
}

bool KviFile::save(kvi_u8_t t)
{
	return (write((const char *)(&t),sizeof(kvi_u8_t)) == sizeof(kvi_u8_t));
}

bool KviFile::save(kvi_u16_t t)
{
#ifndef LOCAL_CPU_LITTLE_ENDIAN
	t = kvi_localCpuToLittleEndian16(t);
#endif
	return (write((const char *)(&t),sizeof(kvi_u16_t)) == sizeof(kvi_u16_t));
}

bool KviFile::save(kvi_u32_t t)
{
#ifndef LOCAL_CPU_LITTLE_ENDIAN
	t = kvi_localCpuToLittleEndian32(t);
#endif
	return (write((const char *)(&t),sizeof(kvi_u32_t)) == sizeof(kvi_u32_t));
}

bool KviFile::save(kvi_u64_t t)
{
#ifndef LOCAL_CPU_LITTLE_ENDIAN
	t = kvi_localCpuToLittleEndian64(t);
#endif
	return (write((const char *)(&t),sizeof(kvi_u64_t)) == sizeof(kvi_u64_t));
}

bool KviFile::save(const QByteArray & data)
{
	if(!save((kvi_u32_t)(data.size())))return false;
	return (write(data.data(),data.size()) == ((unsigned int)(data.size())));
}

bool KviFile::save(const QString & szData)
{
	QByteArray c = KviQString::toUtf8(szData);
	if(!save((kvi_u32_t)(c.length())))return false;
	return (write(c.data(),c.length()) == ((unsigned int)(c.length())));
}

bool KviFile::save(const KviStr & szData)
{
	if(!save((kvi_u32_t)(szData.len())))return false;
	return (write(szData.ptr(),szData.len()) == (unsigned int) szData.len());
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

bool KviFile::load(kvi_u8_t &t)
{
	return (read((char *)(&t),sizeof(kvi_u8_t)) == sizeof(kvi_u8_t));
}

bool KviFile::load(kvi_u16_t &t)
{
	if(!(read((char *)(&t),sizeof(kvi_u16_t)) == sizeof(kvi_u16_t)))return false;
#ifndef LOCAL_CPU_LITTLE_ENDIAN
	t = kvi_littleEndianToLocalCpu16(t);
#endif
	return true;
}

bool KviFile::load(kvi_u32_t &t)
{
	if(!(read((char *)(&t),sizeof(kvi_u32_t)) == sizeof(kvi_u32_t)))return false;
#ifndef LOCAL_CPU_LITTLE_ENDIAN
	t = kvi_littleEndianToLocalCpu32(t);
#endif
	return true;
}

bool KviFile::load(kvi_u64_t &t)
{
	if(!(read((char *)(&t),sizeof(kvi_u32_t)) == sizeof(kvi_u32_t)))return false;
#ifndef LOCAL_CPU_LITTLE_ENDIAN
	t = kvi_littleEndianToLocalCpu32(t);
#endif
	return true;
}

bool KviFile::load(QByteArray & data)
{
	kvi_u32_t iLen;
	if(!load(iLen))return false;
	data.resize(iLen); // it is automatically null terminated in Qt 4.x... BLEAH :D
	if(read((char *)(data.data()),iLen) != iLen)return false;
	return true;
}

bool KviFile::load(QString & szData)
{
	kvi_u32_t iLen;
	if(!load(iLen))return false;
	QByteArray tmp;
	tmp.resize(iLen + 1);
	if(read((char *)(tmp.data()),iLen) != iLen)return false;
	*(tmp.data() + iLen) = 0;
	szData = QString::fromUtf8(tmp.data());
	return true;
}

bool KviFile::load(KviStr & szData)
{
	kvi_u32_t iLen;
	if(!load(iLen))return false;
	szData.setLength(iLen);
	return (read((char *)(szData.ptr()),iLen) == iLen);
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

bool KviFile::skipFirst(char t, unsigned int uMaxDist)
{
	while(uMaxDist > 0)
	{
		char c;
		if(!getChar(&c))return false;
		if(((char)c) == t)return true;
		uMaxDist--;
	}
	return false;
}

bool KviFile::skipFirst(const KviStr & szT, unsigned int uMaxDist)
{
	char * pcPtr = szT.ptr();
	while(uMaxDist > 0)
	{
		char c;
		if(!getChar(&c))return false;
		if(c == *pcPtr)
		{
			pcPtr++;
			if(!*pcPtr)return true;
		} else {
			pcPtr = szT.ptr();
		}
		uMaxDist--;
	}
	return false;
}
