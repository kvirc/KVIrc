#ifndef _KVI_KVS_ARRAY_H_
#define _KVI_KVS_ARRAY_H_
//=============================================================================
//
//   File : kvi_kvs_array.h
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

#include "kvi_settings.h"
#include "kvi_qstring.h"
#include "kvi_kvs_variant.h"
#include "kvi_heapobject.h"


// This class must not have virtual funcitons nor destructor
// Otherwise it will happily crash on windows when it is
// allocated in modules and destroyed anywhere else around...
class KVIRC_API KviKvsArray : public KviHeapObject
{
public:
	KviKvsArray();
	KviKvsArray(const KviKvsArray &a);
	~KviKvsArray();
protected:
	KviKvsVariant ** m_pData;
	kvs_uint_t       m_uSize;
	kvs_uint_t       m_uAllocSize;
public:
	void unset(kvs_uint_t uIdx);
	void set(kvs_uint_t uIdx,KviKvsVariant * pVal);
	KviKvsVariant * at(kvs_uint_t uIdx) const { return (uIdx < m_uSize) ? m_pData[uIdx] : 0; };
	KviKvsVariant * getAt(kvs_uint_t uIdx);
	bool isEmpty(){ return m_uSize == 0; };
	kvs_uint_t size(){ return m_uSize; };
	void appendAsString(QString &szBuffer);
	void serialize(QString& result);
	void sort();
	void rsort();
protected:
	void findNewSize();
};


#endif //!_KVI_KVS_ARRAY_H_
