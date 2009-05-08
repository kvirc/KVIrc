//=============================================================================
//
//   File : kvi_kvs_variantlist.cpp
//   Creation date : Sat 11 Oct 2003 02:20:51 by Szymon Stefanek
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



#include "kvi_kvs_variantlist.h"

KviKvsVariantList::KviKvsVariantList()
{
	m_pList = new KviPointerList<KviKvsVariant>();
	m_pList->setAutoDelete(true);
}

KviKvsVariantList::KviKvsVariantList(KviKvsVariant * v1)
{
	m_pList = new KviPointerList<KviKvsVariant>();
	m_pList->setAutoDelete(true);
	m_pList->append(v1);
}

KviKvsVariantList::KviKvsVariantList(KviKvsVariant * v1,KviKvsVariant * v2)
{
	m_pList = new KviPointerList<KviKvsVariant>();
	m_pList->setAutoDelete(true);
	m_pList->append(v1);
	m_pList->append(v2);
}

KviKvsVariantList::KviKvsVariantList(KviKvsVariant * v1,KviKvsVariant * v2,KviKvsVariant * v3)
{
	m_pList = new KviPointerList<KviKvsVariant>();
	m_pList->setAutoDelete(true);
	m_pList->append(v1);
	m_pList->append(v2);
	m_pList->append(v3);
}

KviKvsVariantList::KviKvsVariantList(KviKvsVariant * v1,KviKvsVariant * v2,KviKvsVariant * v3,KviKvsVariant * v4)
{
	m_pList = new KviPointerList<KviKvsVariant>();
	m_pList->setAutoDelete(true);
	m_pList->append(v1);
	m_pList->append(v2);
	m_pList->append(v3);
	m_pList->append(v4);
}

KviKvsVariantList::KviKvsVariantList(KviKvsVariant * v1,KviKvsVariant * v2,KviKvsVariant * v3,KviKvsVariant * v4,KviKvsVariant * v5)
{
	m_pList = new KviPointerList<KviKvsVariant>();
	m_pList->setAutoDelete(true);
	m_pList->append(v1);
	m_pList->append(v2);
	m_pList->append(v3);
	m_pList->append(v4);
	m_pList->append(v5);
}

KviKvsVariantList::KviKvsVariantList(KviKvsVariant * v1,KviKvsVariant * v2,KviKvsVariant * v3,KviKvsVariant * v4,KviKvsVariant * v5,KviKvsVariant * v6)
{
	m_pList = new KviPointerList<KviKvsVariant>();
	m_pList->setAutoDelete(true);
	m_pList->append(v1);
	m_pList->append(v2);
	m_pList->append(v3);
	m_pList->append(v4);
	m_pList->append(v5);
	m_pList->append(v6);
}

KviKvsVariantList::KviKvsVariantList(KviKvsVariant * v1,KviKvsVariant * v2,KviKvsVariant * v3,KviKvsVariant * v4,KviKvsVariant * v5,KviKvsVariant * v6,KviKvsVariant * v7)
{
	m_pList = new KviPointerList<KviKvsVariant>();
	m_pList->setAutoDelete(true);
	m_pList->append(v1);
	m_pList->append(v2);
	m_pList->append(v3);
	m_pList->append(v4);
	m_pList->append(v5);
	m_pList->append(v6);
	m_pList->append(v7);
}

KviKvsVariantList::KviKvsVariantList(QString * s1)
{
	m_pList = new KviPointerList<KviKvsVariant>();
	m_pList->setAutoDelete(true);
	m_pList->append(new KviKvsVariant(s1));
}

KviKvsVariantList::KviKvsVariantList(QString * s1,QString * s2)
{
	m_pList = new KviPointerList<KviKvsVariant>();
	m_pList->setAutoDelete(true);
	m_pList->append(new KviKvsVariant(s1));
	m_pList->append(new KviKvsVariant(s2));
}

KviKvsVariantList::KviKvsVariantList(QString * s1,QString * s2,QString * s3)
{
	m_pList = new KviPointerList<KviKvsVariant>();
	m_pList->setAutoDelete(true);
	m_pList->append(new KviKvsVariant(s1));
	m_pList->append(new KviKvsVariant(s2));
	m_pList->append(new KviKvsVariant(s3));
}

KviKvsVariantList::KviKvsVariantList(QString * s1,QString * s2,QString * s3,QString * s4)
{
	m_pList = new KviPointerList<KviKvsVariant>();
	m_pList->setAutoDelete(true);
	m_pList->append(new KviKvsVariant(s1));
	m_pList->append(new KviKvsVariant(s2));
	m_pList->append(new KviKvsVariant(s3));
	m_pList->append(new KviKvsVariant(s4));
}

KviKvsVariantList::KviKvsVariantList(QString * s1,QString * s2,QString * s3,QString * s4,QString * s5)
{
	m_pList = new KviPointerList<KviKvsVariant>();
	m_pList->setAutoDelete(true);
	m_pList->append(new KviKvsVariant(s1));
	m_pList->append(new KviKvsVariant(s2));
	m_pList->append(new KviKvsVariant(s3));
	m_pList->append(new KviKvsVariant(s4));
	m_pList->append(new KviKvsVariant(s5));
}

KviKvsVariantList::KviKvsVariantList(QString * s1,QString * s2,QString * s3,QString * s4,QString * s5,QString * s6)
{
	m_pList = new KviPointerList<KviKvsVariant>();
	m_pList->setAutoDelete(true);
	m_pList->append(new KviKvsVariant(s1));
	m_pList->append(new KviKvsVariant(s2));
	m_pList->append(new KviKvsVariant(s3));
	m_pList->append(new KviKvsVariant(s4));
	m_pList->append(new KviKvsVariant(s5));
	m_pList->append(new KviKvsVariant(s6));
}

KviKvsVariantList::~KviKvsVariantList()
{
	delete m_pList;
}


void KviKvsVariantList::setAutoDelete(bool bAutoDelete)
{
	m_pList->setAutoDelete(bAutoDelete);
}

void KviKvsVariantList::allAsString(QString &szBuffer)
{
	szBuffer = QString();
	bool bFirst = true;
	for(KviKvsVariant * v = first();v;v = next())
	{
		if(bFirst)bFirst = false;
		else szBuffer.append(' ');
		v->appendAsString(szBuffer);
	}
}

bool KviKvsVariantList::firstAsString(QString &szBuffer)
{
	KviKvsVariant * v = first();
	if(!v)return false;
	v->asString(szBuffer);
	return true;
}

bool KviKvsVariantList::nextAsString(QString &szBuffer)
{
	KviKvsVariant * v = next();
	if(!v)return false;
	v->asString(szBuffer);
	return true;
}
