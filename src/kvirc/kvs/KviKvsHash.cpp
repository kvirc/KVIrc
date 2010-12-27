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

#include "KviKvsHash.h"

KviKvsHash::KviKvsHash()
{
	m_pDict = new KviPointerHashTable<QString,KviKvsVariant>(17,false);
	m_pDict->setAutoDelete(true);
}

KviKvsHash::KviKvsHash(const KviKvsHash & hash)
{
	m_pDict = new KviPointerHashTable<QString,KviKvsVariant>();
	m_pDict->setAutoDelete(true);
	KviPointerHashTableIterator<QString,KviKvsVariant> it(*(hash.m_pDict));
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

void KviKvsHash::appendAsString(QString & szBuffer) const
{
	KviPointerHashTableIterator<QString,KviKvsVariant> it(*m_pDict);
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
	KviPointerHashTableIterator<QString,KviKvsVariant> it(*m_pDict);
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
	m_pDict->replace(szKey,pVariant);
	return pVariant;
}
