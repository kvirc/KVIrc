//=============================================================================
//
//   File : KviDataBuffer.cpp
//   Creation date : Thu Aug 23 17:04:24 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#define _KVI_DEBUG_CHECK_RANGE_

#include "kvi_debug.h"

#include "KviDataBuffer.h"
#include "KviMemory.h"

// FIXME: this could resize in chunks!...this would be damn faster :)

KviDataBuffer::KviDataBuffer(int uSize, const unsigned char * data)
{
	KVI_ASSERT(uSize > 0);
	m_uSize = uSize;
	m_pData = (unsigned char *)KviMemory::allocate(sizeof(unsigned char) * uSize);
	if(data)
		KviMemory::move(m_pData, data, uSize);
}

KviDataBuffer::KviDataBuffer()
{
	m_uSize = 0;
	m_pData = nullptr;
}

KviDataBuffer::~KviDataBuffer()
{
	if(m_pData)
	{
		KVI_ASSERT(m_uSize);
		KviMemory::free(m_pData);
	}
}

int KviDataBuffer::find(const unsigned char * block, int uSize)
{
	if(uSize < 1)
		return -1;
	if(uSize > m_uSize)
		return -1;

	int uSearchSize = (m_uSize - uSize) + 1;

	for(int i = 0; i < uSearchSize; i++)
	{
		if(m_pData[i] == *block)
		{
			// good beginning
			if(uSize == 1)
				return i;
			int j;
			for(j = 1; j < uSize; j++)
			{
				if(m_pData[i + j] != block[j])
				{
					j = 0;
					break;
				}
			}
			if(j > 0)
				return i;
		}
	}

	return -1;
}

int KviDataBuffer::find(unsigned char c)
{
	const unsigned char * p = m_pData;
	const unsigned char * e = p + m_uSize;
	while(p < e)
	{
		if(*p == c)
			return (p - m_pData);
		p++;
	}
	return -1;
}

void KviDataBuffer::remove(int uSize)
{
	KVI_ASSERT((uSize <= m_uSize) && (uSize > 0));

	m_uSize -= uSize;

	if(m_uSize > 0)
	{
		KviMemory::move(m_pData, m_pData + uSize, m_uSize);
		m_pData = (unsigned char *)KviMemory::reallocate(m_pData, m_uSize * sizeof(unsigned char));
	}
	else
	{
		KviMemory::free(m_pData);
		m_pData = nullptr;
	}
}

void KviDataBuffer::resize(int uSize)
{
	KVI_ASSERT(uSize >= 0);
	if(uSize > 0)
	{
		m_pData = (unsigned char *)KviMemory::reallocate(m_pData, uSize * sizeof(unsigned char));
	}
	else
	{
		KviMemory::free(m_pData);
		m_pData = nullptr;
	}
	m_uSize = uSize;
}

void KviDataBuffer::append(const unsigned char * data, int uSize)
{
	m_pData = (unsigned char *)KviMemory::reallocate(m_pData, m_uSize + uSize);
	KviMemory::move(m_pData + m_uSize, data, uSize);
	m_uSize += uSize;
}
