//=============================================================================
//
//   File : KviKvsTreeNodeCompositeData.cpp
//   Creation date : Tue 07 Oct 2003 02:24:33 by Szymon Stefanek
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

#include "KviKvsTreeNodeCompositeData.h"
#include "KviQString.h"

#define DEBUGME

KviKvsTreeNodeCompositeData::KviKvsTreeNodeCompositeData(const QChar * pLocation, KviPointerList<KviKvsTreeNodeData> * pSubData)
    : KviKvsTreeNodeData(pLocation)
{
#ifdef DEBUGME
	if(pSubData->count() < 2)
		qDebug("KviKvsTreeNodeCompositeData constructor called with less than two child items!");
#endif
	m_pSubData = pSubData;
	m_pSubData->setAutoDelete(true);
	for(KviKvsTreeNodeData * d = m_pSubData->first(); d; d = m_pSubData->next())
		d->setParent(this);
}

KviKvsTreeNodeCompositeData::~KviKvsTreeNodeCompositeData()
{
	delete m_pSubData;
}

bool KviKvsTreeNodeCompositeData::evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	QString * pS = new QString();

	pBuffer->setString(pS);

	KviKvsVariant res;

	// we need to use an iterator to accommodate recursion
	KviPointerListIterator<KviKvsTreeNodeData> it(*m_pSubData);

	while(KviKvsTreeNodeData * d = it.current())
	{
		if(!d->evaluateReadOnly(c, &res))
			return false;
		res.appendAsString(*pS);
		++it;
	}
	return true;
}

void KviKvsTreeNodeCompositeData::contextDescription(QString & szBuffer)
{
	szBuffer = "Composite Data Evaluation (Implicit String Cast)";
}

void KviKvsTreeNodeCompositeData::dump(const char * prefix)
{
	qDebug("%s CompositeData", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	for(KviKvsTreeNodeData * p = m_pSubData->first(); p; p = m_pSubData->next())
	{
		p->dump(tmp.toUtf8().data());
	}
}
