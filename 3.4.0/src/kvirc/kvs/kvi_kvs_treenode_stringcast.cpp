//=============================================================================
//
//   File : kvi_kvs_treenode_stringcast.cpp
//   Created on Mon 26 Jun 2006 15:17:33 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2006 Szymon Stefanek <pragma at kvirc dot net>
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
//=============================================================================

#define __KVIRC__

#include "kvi_kvs_treenode_stringcast.h"
#include "kvi_qstring.h"

KviKvsTreeNodeStringCast::KviKvsTreeNodeStringCast(const QChar * pLocation,KviKvsTreeNodeData * pChildData)
: KviKvsTreeNodeData(pLocation)
{
	m_pChildData = pChildData;
	m_pChildData->setParent(this);
}


KviKvsTreeNodeStringCast::~KviKvsTreeNodeStringCast()
{
	delete m_pChildData;
}

bool KviKvsTreeNodeStringCast::evaluateReadOnly(KviKvsRunTimeContext * c,KviKvsVariant * pBuffer)
{
	if(!m_pChildData->evaluateReadOnly(c,pBuffer))return false;
	if(!pBuffer->isString())
	{
		QString tmp;
		pBuffer->asString(tmp);
		pBuffer->setString(tmp);
	}
	return true;
}

void KviKvsTreeNodeStringCast::contextDescription(QString &szBuffer)
{
	szBuffer = "String Cast";
}

void KviKvsTreeNodeStringCast::dump(const char * prefix)
{
	debug("%s StringCast",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pChildData->dump(tmp.utf8().data());
}

