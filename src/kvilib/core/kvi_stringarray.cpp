//=================================================================================================
//
//   File : kvi_stringarray.cpp
//   Creation date : Tue Jun 6 02:20:20 by Szymon Stefanek
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
//=================================================================================================



#include "kvi_stringarray.h"
#include "kvi_malloc.h"

KviStringArray::KviStringArray()
{
	m_uSize = 0;
	m_pData = 0;
	m_uHighestIdx = 0;
}

KviStringArray::~KviStringArray()
{
	if(m_pData)clear();
}


void KviStringArray::clear()
{
	if(!m_pData)return;
	for(unsigned int i=0;i<m_uSize;i++)
	{
		if(m_pData[i])delete m_pData[i];
	}
	kvi_free(m_pData);
	m_pData = 0;
	m_uHighestIdx = 0;
	m_uSize = 0;
}

void KviStringArray::insert(unsigned int uIdx,KviStr * pVal)
{
	if(m_uSize <= uIdx)
	{
		unsigned int uOldSize = m_uSize;
		m_uSize = uIdx + KVI_STRING_ARRAY_FREESPACE_SIZE;
		m_pData = (KviStr **)kvi_realloc(m_pData,m_uSize * sizeof(KviStr *));
		for(unsigned int u = uOldSize;u < m_uSize;u++)
		{
			m_pData[u] = 0;
		}
	} else {
		if(m_pData[uIdx])delete m_pData[uIdx];
	}
	if(uIdx > m_uHighestIdx)m_uHighestIdx = uIdx;
	m_pData[uIdx] = pVal;
}

void KviStringArray::remove(unsigned int uIdx)
{
	if(uIdx > m_uHighestIdx)return;
	if(m_pData[uIdx])
	{
		delete m_pData[uIdx];
		m_pData[uIdx] = 0;
		if(uIdx == m_uHighestIdx)
		{
			// shrink the array
			if(m_uHighestIdx == 0)clear();
			else {
				unsigned int u = m_uHighestIdx - 1;
				while(!m_pData[u])u--;
				if((m_uHighestIdx - u) > KVI_STRING_ARRAY_FREESPACE_SIZE)shrink(u);
				else m_uHighestIdx = u; // just set the max index
			}
		}
	}
}

void KviStringArray::shrink(unsigned int uMaxItem)
{
	m_uHighestIdx = uMaxItem;
	m_uSize = uMaxItem + 1;
	m_pData = (KviStr **)kvi_realloc(m_pData,sizeof(KviStr *) * m_uSize);
}

void KviStringArray::copyFrom(KviStringArray * a)
{
	clear();
	m_uSize = a->m_uSize;
	m_uHighestIdx = a->m_uHighestIdx;
	if(m_uSize > 0)
	{
		m_pData = (KviStr **)kvi_malloc(sizeof(KviStr *) * m_uSize);
		for(unsigned int i=0;i<m_uSize;i++)
		{
			if(a->m_pData[i])m_pData[i] = new KviStr(*(a->m_pData[i]));
			else m_pData[i] = 0;
		}
	} else {
		m_pData = 0;
	}
}
