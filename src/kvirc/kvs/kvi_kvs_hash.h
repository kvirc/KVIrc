#ifndef _KVI_KVS_HASH_H_
#define _KVI_KVS_HASH_H_
//=============================================================================
//
//   File : kvi_kvs_hash.h
//   Creation date : Tue 07 Oct 2003 01:22:37 by Szymon Stefanek
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

#include "kvi_pointerhashtable.h"
#include "kvi_qstring.h"

#include "kvi_kvs_variant.h"
#include "kvi_heapobject.h"

typedef KVIRC_API_TYPEDEF KviPointerHashTableIterator<QString,KviKvsVariant> KviKvsHashIterator;

// This class must not have virtual funcitons nor destructor
// Otherwise it will happily crash on windows when it is
// allocated in modules and destroyed anywhere else around...
class KVIRC_API KviKvsHash : public KviHeapObject
{
public:
	KviKvsHash();
	KviKvsHash(const KviKvsHash &h);
	~KviKvsHash();
protected:
	KviPointerHashTable<QString,KviKvsVariant> * m_pDict;
public:
	void unset(const QString &szKey){ m_pDict->remove(szKey); };
	void set(const QString &szKey,KviKvsVariant * pVal){ m_pDict->replace(szKey,pVal); };
	KviKvsVariant * find(const QString &szKey) const { return m_pDict->find(szKey); };
	KviKvsVariant * get(const QString &szKey);

	bool isEmpty() const { return m_pDict->isEmpty(); };
	kvs_uint_t size() const { return m_pDict->count(); };

	void appendAsString(QString &szBuffer) const;

	const KviPointerHashTable<QString,KviKvsVariant> * dict(){ return m_pDict; };

	void serialize(QString& result);
};

#endif //!_KVI_KVS_HASH_H_
