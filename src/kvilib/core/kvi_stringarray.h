#ifndef _KVI_STRINGARRAY_H_
#define _KVI_STRINGARRAY_H_
//=================================================================================================
//
//   File : kvi_stringarray.h
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
//   Inc. ,51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=================================================================================================

#include "kvi_settings.h"
#include "kvi_string.h"
#include "kvi_heapobject.h"

#define KVI_STRING_ARRAY_FREESPACE_SIZE 16

class KVILIB_API KviStringArray : public KviHeapObject
{
public:
	KviStringArray();
	~KviStringArray();
public:
	unsigned int   m_uSize;
	unsigned int   m_uHighestIdx;
	KviStr      ** m_pData;
public:
	void clear();
	void insert(unsigned int uIdx,KviStr * pVal);
	void copyFrom(KviStringArray * a);
	unsigned int size(){ return (m_uSize == 0) ? 0 : (m_uHighestIdx + 1); };
	bool isEmpty(){ return m_uSize == 0; };
	void remove(unsigned int uIdx);
	void shrink(unsigned int uMaxItem);
	KviStr * uncheckedAt(unsigned int uIdx){ return m_pData[uIdx]; };
	KviStr * at(unsigned int uIdx){ return m_uSize > uIdx ? m_pData[uIdx] : 0; };
	KviStr * getAt(unsigned int uIdx){ KviStr * t = at(uIdx); if(t)m_pData[uIdx] = 0; return t; };
};

#endif //_KVI_STRINGARRAY_H_
