//=============================================================================
//
//   File : KviKvsHash.cpp
//   Creation date : Tue 07 Oct 2003 01:22:37 by Szymon Stefanek
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

#include "KviKvsHash.h"

KviKvsHash::KviKvsHash()
{
	m_pDict = new KviPointerHashTable<QString, KviKvsVariant>(17, false);
	m_pDict->setAutoDelete(true);
}

KviKvsHash::KviKvsHash(const KviKvsHash & hash)
{
	m_pDict = new KviPointerHashTable<QString, KviKvsVariant>();
	m_pDict->setAutoDelete(true);
	KviPointerHashTableIterator<QString, KviKvsVariant> it(*(hash.m_pDict));
	while(it.current())
	{
		m_pDict->replace(it.currentKey(), new KviKvsVariant(*(it.current())));
		++it;
	}
}

KviKvsHash::~KviKvsHash()
{
	delete m_pDict;
}

//
// Don't inline these short functions as they instantiate a huge template
// which would be then placed in every single module.
//
// There would be also an interesting problem when the modules are unloaded.
// KviPointerHashTable uses a KviPointerList which has a _vtable.
// Assume you allocate a KviKvsHash inside a module and the KviPointerList
// implementation ends up in the module's text segment. Now if you unload the
// module and then later delete the KviKvsHash (or just cause the destruction
// of the internal KviPointerList in some way) you'll end up calling the
// destructor via _vtable. The _vtable will no longer be there and you'll
// be dead :)
//
// It took me a whole day to figure this out.
//

void KviKvsHash::unset(const QString & szKey)
{
	m_pDict->remove(szKey);
}

void KviKvsHash::set(const QString & szKey, KviKvsVariant * pVal)
{
	m_pDict->replace(szKey, pVal);
}

KviKvsVariant * KviKvsHash::find(const QString & szKey) const
{
	return m_pDict->find(szKey);
}

bool KviKvsHash::isEmpty() const
{
	return m_pDict->isEmpty();
}

void KviKvsHash::clear()
{
	m_pDict->clear();
}

const KviPointerHashTable<QString, KviKvsVariant> * KviKvsHash::dict()
{
	return m_pDict;
}

kvs_uint_t KviKvsHash::size() const
{
	return m_pDict->count();
}

void KviKvsHash::appendAsString(QString & szBuffer) const
{
	KviPointerHashTableIterator<QString, KviKvsVariant> it(*m_pDict);
	bool bNeedComma = false;
	while(KviKvsVariant * pVariant = it.current())
	{
		if(bNeedComma)
			szBuffer.append(',');
		else
			bNeedComma = true;
		pVariant->appendAsString(szBuffer);
		++it;
	}
}

void KviKvsHash::serialize(QString & szResult)
{
	QString szTmp;
	szResult = "{";
	KviPointerHashTableIterator<QString, KviKvsVariant> it(*m_pDict);
	bool bNeedComma = false;
	while(KviKvsVariant * pVariant = it.current())
	{
		if(bNeedComma)
			szResult.append(',');
		else
			bNeedComma = true;

		szTmp = it.currentKey();
		KviKvsVariant::serializeString(szTmp);
		szResult.append(szTmp);

		szResult.append(":");
		pVariant->serialize(szTmp);
		szResult.append(szTmp);

		++it;
	}
	szResult.append('}');
}

KviKvsVariant * KviKvsHash::get(const QString & szKey)
{
	KviKvsVariant * pVariant = m_pDict->find(szKey);
	if(pVariant)
		return pVariant;
	pVariant = new KviKvsVariant();
	m_pDict->replace(szKey, pVariant);
	return pVariant;
}
