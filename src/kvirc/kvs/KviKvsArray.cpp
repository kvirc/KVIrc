//=============================================================================
//
//   File : KviKvsArray.cpp
//   Creation date : Tue 07 Oct 2003 01:07:31 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "KviKvsArray.h"
#include "KviMemory.h"

#include <cstdlib>

#define KVI_KVS_ARRAY_ALLOC_CHUNK 8

KviKvsArray::KviKvsArray()
    : KviHeapObject()
{
	m_pData = nullptr;
	m_uSize = 0;
	m_uAllocSize = 0;
}

KviKvsArray::KviKvsArray(const KviKvsArray & array)
    : KviHeapObject()
{
	m_uSize = array.m_uSize;
	m_uAllocSize = array.m_uAllocSize;
	if(m_uAllocSize > 0)
	{
		m_pData = (KviKvsVariant **)KviMemory::allocate((sizeof(KviKvsVariant *)) * m_uAllocSize);
		kvs_uint_t u;
		for(u = 0; u < m_uSize; u++)
		{
			if(array.m_pData[u])
				m_pData[u] = new KviKvsVariant(*(array.m_pData[u]));
			else
				m_pData[u] = nullptr;
		}
	}
	else
	{
		m_pData = nullptr;
	}
}

KviKvsArray::~KviKvsArray()
{
	if(m_pData)
	{
		for(kvs_uint_t u = 0; u < m_uSize; u++)
		{
			if(m_pData[u])
				delete m_pData[u];
		}
		KviMemory::free(m_pData);
	}
}

int KviKvsArray::compareReverse(const void * pV1, const void * pV2)
{
	if(*((KviKvsVariant **)pV1))
		return (*((KviKvsVariant **)pV1))->compare(*((KviKvsVariant **)pV2));
	if(*((KviKvsVariant **)pV2))
		return -(*((KviKvsVariant **)pV2))->compare(*((KviKvsVariant **)pV1));
	return 0;
}

int KviKvsArray::compare(const void * pV1, const void * pV2)
{
	if(*((KviKvsVariant **)pV1))
		return -(*((KviKvsVariant **)pV1))->compare(*((KviKvsVariant **)pV2));
	if(*((KviKvsVariant **)pV2))
		return (*((KviKvsVariant **)pV2))->compare(*((KviKvsVariant **)pV1));
	return 0;
}

void KviKvsArray::sort()
{
	if(m_uSize < 2)
		return; // already sorted
	qsort(m_pData, m_uSize, sizeof(KviKvsVariant *), compare);
	findNewSize();
}

void KviKvsArray::rsort()
{
	if(m_uSize < 2)
		return; // already sorted
	qsort(m_pData, m_uSize, sizeof(KviKvsVariant *), compareReverse);
	findNewSize();
}

void KviKvsArray::unset(kvs_uint_t uIdx)
{
	if(uIdx >= m_uSize)
		return;

	if(m_pData[uIdx])
	{
		delete m_pData[uIdx];
		m_pData[uIdx] = nullptr;
	}

	if(uIdx == (m_uSize - 1))
	{
		findNewSize();
	}
}

void KviKvsArray::findNewSize()
{
	// find the new size
	if(m_uSize == 0)
		return;
	kvs_uint_t u = m_uSize - 1;
	while(u > 0)
	{
		if(m_pData[u])
			break;
		u--;
	}

	if(m_pData[u])
		m_uSize = u + 1;
	else
	{
		// u == 0, and there is no data in there
		m_uSize = 0;
	}
	// need to shrink ?

	if((m_uAllocSize - m_uSize) > KVI_KVS_ARRAY_ALLOC_CHUNK)
	{
		m_uAllocSize = m_uSize;
		// m_pData is non-zero here since was m_uSize > 0 initially
		if(m_uSize > 0)
		{
			m_pData = (KviKvsVariant **)KviMemory::reallocate(m_pData, (sizeof(KviKvsVariant *)) * m_uAllocSize);
		}
		else
		{
			KviMemory::free(m_pData);
			m_pData = nullptr;
		}
	}
}

void KviKvsArray::set(kvs_uint_t uIdx, KviKvsVariant * pVal)
{
	if(uIdx >= m_uSize)
	{
		if(uIdx == m_uSize)
			m_uAllocSize += KVI_KVS_ARRAY_ALLOC_CHUNK; // sequential set
		else
			m_uAllocSize = uIdx + 1;

		if(m_pData)
			m_pData = (KviKvsVariant **)KviMemory::reallocate(m_pData, (sizeof(KviKvsVariant *)) * m_uAllocSize);
		else
			m_pData = (KviKvsVariant **)KviMemory::allocate((sizeof(KviKvsVariant *)) * m_uAllocSize);

		for(kvs_uint_t u = m_uSize; u < uIdx; u++)
			m_pData[u] = nullptr;
		m_uSize = uIdx + 1;
		m_pData[uIdx] = pVal;
	}
	else
	{
		if(m_pData[uIdx])
			delete m_pData[uIdx];
		m_pData[uIdx] = pVal;
	}
}

void KviKvsArray::append(KviKvsVariant * pVal)
{
	set(m_uSize, pVal);
}

KviKvsVariant * KviKvsArray::getAt(kvs_uint_t uIdx)
{
	if(uIdx >= m_uSize)
	{
		if(uIdx == m_uSize)
			m_uAllocSize += KVI_KVS_ARRAY_ALLOC_CHUNK; // sequential set
		else
			m_uAllocSize = uIdx + 1;

		if(m_pData)
			m_pData = (KviKvsVariant **)KviMemory::reallocate(m_pData, (sizeof(KviKvsVariant *)) * m_uAllocSize);
		else
			m_pData = (KviKvsVariant **)KviMemory::allocate((sizeof(KviKvsVariant *)) * m_uAllocSize);

		for(kvs_uint_t u = m_uSize; u < uIdx; u++)
			m_pData[u] = nullptr;
		m_uSize = uIdx + 1;
		m_pData[uIdx] = new KviKvsVariant();
	}
	else
	{
		if(!m_pData[uIdx])
			m_pData[uIdx] = new KviKvsVariant();
	}
	return m_pData[uIdx];
}

void KviKvsArray::serialize(QString & szResult)
{
	QString szBuffer;
	szResult = "[";
	kvs_uint_t u = 0;
	bool bNeedComma = false;
	while(u < m_uSize)
	{
		if(bNeedComma)
			szResult.append(',');
		else
			bNeedComma = true;

		if(m_pData[u])
		{
			m_pData[u]->serialize(szBuffer);
			szResult.append(szBuffer);
		}
		else
		{
			szResult.append("null");
		}
		u++;
	}
	szResult.append(']');
}

void KviKvsArray::appendAsString(QString & szBuffer)
{
	kvs_uint_t u = 0;
	bool bNeedComma = false;
	while(u < m_uSize)
	{
		if(bNeedComma)
			szBuffer.append(',');
		else
			bNeedComma = true;

		if(m_pData[u])
			m_pData[u]->appendAsString(szBuffer);
		u++;
	}
}
