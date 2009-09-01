//=============================================================================
//
//   File : kvi_kvs_array.cpp
//   Creation date : Tue 07 Oct 2003 01:07:31 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2008 Szymon Stefanek <pragma at kvirc dot net>
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



#include "kvi_kvs_array.h"
#include "kvi_malloc.h"

#include <stdlib.h>

#define KVI_KVS_ARRAY_ALLOC_CHUNK 8

KviKvsArray::KviKvsArray()
: KviHeapObject()
{
	m_pData = 0;
	m_uSize = 0;
	m_uAllocSize = 0;
}

KviKvsArray::KviKvsArray(const KviKvsArray &a)
: KviHeapObject()
{
	m_uSize = a.m_uSize;
	m_uAllocSize = a.m_uAllocSize;
	if(m_uAllocSize > 0)
	{
		m_pData = (KviKvsVariant **)kvi_malloc((sizeof(KviKvsVariant *)) * m_uAllocSize);
		kvs_uint_t i;
		for(i=0;i<m_uSize;i++)
			if(a.m_pData[i])m_pData[i] = new KviKvsVariant(*(a.m_pData[i]));
			else m_pData[i] = 0;
	} else {
		m_pData = 0;
	}
}

KviKvsArray::~KviKvsArray()
{
	if(m_pData)
	{
		for(kvs_uint_t i=0;i<m_uSize;i++)
			if(m_pData[i])delete m_pData[i];
		kvi_free(m_pData);
	}
}

static int kvs_array_reverse_compare_func(const void * v1,const void * v2)
{
	if(*((KviKvsVariant **)v1))return (*((KviKvsVariant **)v1))->compare(*((KviKvsVariant **)v2));
	if(*((KviKvsVariant **)v2))return -(*((KviKvsVariant **)v2))->compare(*((KviKvsVariant **)v1));
	return 0;
}

static int kvs_array_compare_func(const void * v1,const void * v2)
{
	if(*((KviKvsVariant **)v1))return -(*((KviKvsVariant **)v1))->compare(*((KviKvsVariant **)v2));
	if(*((KviKvsVariant **)v2))return (*((KviKvsVariant **)v2))->compare(*((KviKvsVariant **)v1));
	return 0;
}


void KviKvsArray::sort()
{
	if(m_uSize < 2)return; // already sorted
	qsort(m_pData,m_uSize,sizeof(KviKvsVariant *),kvs_array_compare_func);
	findNewSize();
}

void KviKvsArray::rsort()
{
	if(m_uSize < 2)return; // already sorted
	qsort(m_pData,m_uSize,sizeof(KviKvsVariant *),kvs_array_reverse_compare_func);
	findNewSize();
}

void KviKvsArray::unset(kvs_uint_t uIdx)
{
	if(uIdx >= m_uSize)return;

	if(m_pData[uIdx])
	{
		delete m_pData[uIdx];
		m_pData[uIdx] = 0;
	}

	if(uIdx == (m_uSize - 1))
	{
		findNewSize();
	}
}

void KviKvsArray::findNewSize()
{
	// find the new size
	if(m_uSize == 0)return;
	kvs_uint_t u = m_uSize - 1;
	while(u > 0)
	{
		if(m_pData[u])break;
		u--;
	}
	if(m_pData[u])m_uSize = u + 1;
	else {
		// u == 0, and there is no data in there
		m_uSize = 0;
	}
	// need to shrink ?

	if((m_uAllocSize - m_uSize) > KVI_KVS_ARRAY_ALLOC_CHUNK)
	{
		m_uAllocSize = m_uSize;
		// m_pData is non-zero here since was m_uSize > 0 initally
		if(m_uSize > 0)
		{
			m_pData = (KviKvsVariant **)kvi_realloc(m_pData,(sizeof(KviKvsVariant *)) * m_uAllocSize);
		} else {
			kvi_free(m_pData);
			m_pData = 0;
		}
	}
}

void KviKvsArray::set(kvs_uint_t uIdx,KviKvsVariant * pVal)
{
	if(uIdx >= m_uSize)
	{
		if(uIdx == m_uSize)
		{
			m_uAllocSize += KVI_KVS_ARRAY_ALLOC_CHUNK; // sequential set
		} else {
			m_uAllocSize = uIdx + 1;
		}
		if(m_pData)
			m_pData = (KviKvsVariant **)kvi_realloc(m_pData,(sizeof(KviKvsVariant *)) * m_uAllocSize);
		else
			m_pData = (KviKvsVariant **)kvi_malloc((sizeof(KviKvsVariant *)) * m_uAllocSize);
		for(kvs_uint_t u=m_uSize;u<uIdx;u++)
			m_pData[u] = 0;
		m_uSize = uIdx+1;
		m_pData[uIdx] = pVal;
	} else {
		if(m_pData[uIdx])delete m_pData[uIdx];
		m_pData[uIdx] = pVal;
	}
}

KviKvsVariant * KviKvsArray::getAt(kvs_uint_t uIdx)
{
	if(uIdx >= m_uSize)
	{
		if(uIdx == m_uSize)
		{
			m_uAllocSize += KVI_KVS_ARRAY_ALLOC_CHUNK; // sequential set
		} else {
			m_uAllocSize = uIdx + 1;
		}
		if(m_pData)
			m_pData = (KviKvsVariant **)kvi_realloc(m_pData,(sizeof(KviKvsVariant *)) * m_uAllocSize);
		else
			m_pData = (KviKvsVariant **)kvi_malloc((sizeof(KviKvsVariant *)) * m_uAllocSize);
		for(kvs_uint_t u=m_uSize;u<uIdx;u++)
			m_pData[u] = 0;
		m_uSize = uIdx+1;
		m_pData[uIdx] = new KviKvsVariant();
	} else {
		if(!m_pData[uIdx])
			m_pData[uIdx] = new KviKvsVariant();
	}
	return m_pData[uIdx];
}

void KviKvsArray::serialize(QString& result)
{
	QString tmpBuffer;
	result="[";
	kvs_uint_t u = 0;
	bool bNeedComma = false;
	while(u < m_uSize)
	{
		if(bNeedComma)result.append(',');
		else bNeedComma = true;
		if(m_pData[u]) {
			m_pData[u]->serialize(tmpBuffer);
			result.append(tmpBuffer);
		} else {
			result.append("null");
		}
		u++;
	}
	result.append(']');
}

void KviKvsArray::appendAsString(QString &szBuffer)
{
	kvs_uint_t u = 0;
	bool bNeedComma = false;
	while(u < m_uSize)
	{
		if(bNeedComma)szBuffer.append(',');
		else bNeedComma = true;
		if(m_pData[u])m_pData[u]->appendAsString(szBuffer);
		u++;
	}
}
