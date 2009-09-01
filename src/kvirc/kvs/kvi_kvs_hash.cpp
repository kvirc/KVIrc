//=============================================================================
//
//   File : kvi_kvs_hash.cpp
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



#include "kvi_kvs_hash.h"

KviKvsHash::KviKvsHash()
{
	m_pDict = new KviPointerHashTable<QString,KviKvsVariant>(17,false);
	m_pDict->setAutoDelete(true);
}

KviKvsHash::KviKvsHash(const KviKvsHash &h)
{
	m_pDict = new KviPointerHashTable<QString,KviKvsVariant>();
	m_pDict->setAutoDelete(true);
	KviPointerHashTableIterator<QString,KviKvsVariant> it(*(h.m_pDict));
	while(it.current())
	{
		m_pDict->replace(it.currentKey(),new KviKvsVariant(*(it.current())));
		++it;
	}
}

KviKvsHash::~KviKvsHash()
{
	delete m_pDict;
}

void KviKvsHash::appendAsString(QString &szBuffer) const
{
	KviPointerHashTableIterator<QString,KviKvsVariant> it(*m_pDict);
	bool bNeedComma = false;
	while(KviKvsVariant * s = it.current())
	{
		if(bNeedComma)szBuffer.append(',');
		else bNeedComma = true;
		s->appendAsString(szBuffer);
		++it;
	}
}

void KviKvsHash::serialize(QString& result)
{
	QString tmpBuffer;
	result="{";
	KviPointerHashTableIterator<QString,KviKvsVariant> it(*m_pDict);
	bool bNeedComma = false;
	while(KviKvsVariant * s = it.current())
	{
		if(bNeedComma)result.append(',');
		else bNeedComma = true;

		tmpBuffer = it.currentKey();
		KviKvsVariant::serializeString(tmpBuffer);
		result.append(tmpBuffer);

		result.append(":");
		s->serialize(tmpBuffer);
		result.append(tmpBuffer);

		++it;
	}
	result.append('}');
}

KviKvsVariant * KviKvsHash::get(const QString &szKey)
{
	KviKvsVariant * v = m_pDict->find(szKey);
	if(v)return v;
	v = new KviKvsVariant();
	m_pDict->replace(szKey,v);
	return v;
}
