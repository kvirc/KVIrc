//=============================================================================
//
//   File : KviKvsVariantList.cpp
//   Creation date : Sat 11 Oct 2003 02:20:51 by Szymon Stefanek
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

#include "KviKvsVariantList.h"

#include <QStringList>

KviKvsVariantList::KviKvsVariantList()
{
	m_pList = new KviPointerList<KviKvsVariant>();
	m_pList->setAutoDelete(true);
}

KviKvsVariantList::KviKvsVariantList(KviKvsVariant * pV1)
{
	m_pList = new KviPointerList<KviKvsVariant>();
	m_pList->setAutoDelete(true);
	m_pList->append(pV1);
}

KviKvsVariantList::KviKvsVariantList(KviKvsVariant * pV1, KviKvsVariant * pV2)
{
	m_pList = new KviPointerList<KviKvsVariant>();
	m_pList->setAutoDelete(true);
	m_pList->append(pV1);
	m_pList->append(pV2);
}

KviKvsVariantList::KviKvsVariantList(KviKvsVariant * pV1, KviKvsVariant * pV2, KviKvsVariant * pV3)
{
	m_pList = new KviPointerList<KviKvsVariant>();
	m_pList->setAutoDelete(true);
	m_pList->append(pV1);
	m_pList->append(pV2);
	m_pList->append(pV3);
}

KviKvsVariantList::KviKvsVariantList(KviKvsVariant * pV1, KviKvsVariant * pV2, KviKvsVariant * pV3, KviKvsVariant * pV4)
{
	m_pList = new KviPointerList<KviKvsVariant>();
	m_pList->setAutoDelete(true);
	m_pList->append(pV1);
	m_pList->append(pV2);
	m_pList->append(pV3);
	m_pList->append(pV4);
}

KviKvsVariantList::KviKvsVariantList(KviKvsVariant * pV1, KviKvsVariant * pV2, KviKvsVariant * pV3, KviKvsVariant * pV4, KviKvsVariant * pV5)
{
	m_pList = new KviPointerList<KviKvsVariant>();
	m_pList->setAutoDelete(true);
	m_pList->append(pV1);
	m_pList->append(pV2);
	m_pList->append(pV3);
	m_pList->append(pV4);
	m_pList->append(pV5);
}

KviKvsVariantList::KviKvsVariantList(KviKvsVariant * pV1, KviKvsVariant * pV2, KviKvsVariant * pV3, KviKvsVariant * pV4, KviKvsVariant * pV5, KviKvsVariant * pV6)
{
	m_pList = new KviPointerList<KviKvsVariant>();
	m_pList->setAutoDelete(true);
	m_pList->append(pV1);
	m_pList->append(pV2);
	m_pList->append(pV3);
	m_pList->append(pV4);
	m_pList->append(pV5);
	m_pList->append(pV6);
}

KviKvsVariantList::KviKvsVariantList(KviKvsVariant * pV1, KviKvsVariant * pV2, KviKvsVariant * pV3, KviKvsVariant * pV4, KviKvsVariant * pV5, KviKvsVariant * pV6, KviKvsVariant * pV7)
{
	m_pList = new KviPointerList<KviKvsVariant>();
	m_pList->setAutoDelete(true);
	m_pList->append(pV1);
	m_pList->append(pV2);
	m_pList->append(pV3);
	m_pList->append(pV4);
	m_pList->append(pV5);
	m_pList->append(pV6);
	m_pList->append(pV7);
}

KviKvsVariantList::KviKvsVariantList(QString * pS1)
{
	m_pList = new KviPointerList<KviKvsVariant>();
	m_pList->setAutoDelete(true);
	m_pList->append(new KviKvsVariant(pS1));
}

KviKvsVariantList::KviKvsVariantList(QString * pS1, QString * pS2)
{
	m_pList = new KviPointerList<KviKvsVariant>();
	m_pList->setAutoDelete(true);
	m_pList->append(new KviKvsVariant(pS1));
	m_pList->append(new KviKvsVariant(pS2));
}

KviKvsVariantList::KviKvsVariantList(QString * pS1, QString * pS2, QString * pS3)
{
	m_pList = new KviPointerList<KviKvsVariant>();
	m_pList->setAutoDelete(true);
	m_pList->append(new KviKvsVariant(pS1));
	m_pList->append(new KviKvsVariant(pS2));
	m_pList->append(new KviKvsVariant(pS3));
}

KviKvsVariantList::KviKvsVariantList(QString * pS1, QString * pS2, QString * pS3, QString * pS4)
{
	m_pList = new KviPointerList<KviKvsVariant>();
	m_pList->setAutoDelete(true);
	m_pList->append(new KviKvsVariant(pS1));
	m_pList->append(new KviKvsVariant(pS2));
	m_pList->append(new KviKvsVariant(pS3));
	m_pList->append(new KviKvsVariant(pS4));
}

KviKvsVariantList::KviKvsVariantList(QString * pS1, QString * pS2, QString * pS3, QString * pS4, QString * pS5)
{
	m_pList = new KviPointerList<KviKvsVariant>();
	m_pList->setAutoDelete(true);
	m_pList->append(new KviKvsVariant(pS1));
	m_pList->append(new KviKvsVariant(pS2));
	m_pList->append(new KviKvsVariant(pS3));
	m_pList->append(new KviKvsVariant(pS4));
	m_pList->append(new KviKvsVariant(pS5));
}

KviKvsVariantList::KviKvsVariantList(QString * pS1, QString * pS2, QString * pS3, QString * pS4, QString * pS5, QString * pS6)
{
	m_pList = new KviPointerList<KviKvsVariant>();
	m_pList->setAutoDelete(true);
	m_pList->append(new KviKvsVariant(pS1));
	m_pList->append(new KviKvsVariant(pS2));
	m_pList->append(new KviKvsVariant(pS3));
	m_pList->append(new KviKvsVariant(pS4));
	m_pList->append(new KviKvsVariant(pS5));
	m_pList->append(new KviKvsVariant(pS6));
}

KviKvsVariantList::KviKvsVariantList(QString * pS1, QString * pS2, QString * pS3, QString * pS4, QString * pS5, QString * pS6, QString * pS7)
{
	m_pList = new KviPointerList<KviKvsVariant>();
	m_pList->setAutoDelete(true);
	m_pList->append(new KviKvsVariant(pS1));
	m_pList->append(new KviKvsVariant(pS2));
	m_pList->append(new KviKvsVariant(pS3));
	m_pList->append(new KviKvsVariant(pS4));
	m_pList->append(new KviKvsVariant(pS5));
	m_pList->append(new KviKvsVariant(pS6));
	m_pList->append(new KviKvsVariant(pS7));
}

KviKvsVariantList::KviKvsVariantList(QStringList * pSL)
{
	m_pList = new KviPointerList<KviKvsVariant>();
	m_pList->setAutoDelete(true);
	if(!pSL)
		return;

	foreach(QString pS, *pSL)
		m_pList->append(new KviKvsVariant(new QString(pS)));
}

KviKvsVariantList::~KviKvsVariantList()
{
	delete m_pList;
}

void KviKvsVariantList::setAutoDelete(bool bAutoDelete)
{
	m_pList->setAutoDelete(bAutoDelete);
}

void KviKvsVariantList::allAsString(QString & szBuffer)
{
	szBuffer = QString();
	bool bFirst = true;
	for(KviKvsVariant * v = first(); v; v = next())
	{
		if(bFirst)
			bFirst = false;
		else
			szBuffer.append(' ');
		v->appendAsString(szBuffer);
	}
}

bool KviKvsVariantList::firstAsString(QString & szBuffer)
{
	KviKvsVariant * v = first();
	if(!v)
		return false;
	v->asString(szBuffer);
	return true;
}

bool KviKvsVariantList::nextAsString(QString & szBuffer)
{
	KviKvsVariant * v = next();
	if(!v)
		return false;
	v->asString(szBuffer);
	return true;
}
