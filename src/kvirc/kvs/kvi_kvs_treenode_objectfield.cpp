//=============================================================================
//
//   File : kvi_kvs_treenode_objectfield.cpp
//   Created on Thu 28 Apr 2005 17:12:14 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005 Szymon Stefanek <pragma at kvirc dot net>
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

#include "kvi_kvs_treenode_objectfield.h"
#include "kvi_kvs_runtimecontext.h"
#include "kvi_kvs_object.h"
#include "kvi_kvs_hash.h"

KviKvsTreeNodeObjectField::KviKvsTreeNodeObjectField(const QChar * pLocation,const QString &szIdentifier)
: KviKvsTreeNodeVariable(pLocation,szIdentifier)
{
}

KviKvsTreeNodeObjectField::~KviKvsTreeNodeObjectField()
{
}

void KviKvsTreeNodeObjectField::contextDescription(QString &szBuffer)
{
#ifdef COMPILE_NEW_KVS
	szBuffer = "Object Field \"";
	szBuffer += m_szIdentifier;
	szBuffer += "\"";
#endif
}

void KviKvsTreeNodeObjectField::dump(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	debug("%s ObjectField(%s)",prefix,m_szIdentifier.utf8().data());
#endif
}

bool KviKvsTreeNodeObjectField::canEvaluateInObjectScope()
{
	return true;
}

bool KviKvsTreeNodeObjectField::evaluateReadOnlyInObjectScope(KviKvsObject * o,KviKvsRunTimeContext * c,KviKvsVariant * pBuffer)
{
#ifdef COMPILE_NEW_KVS
	KviKvsVariant * v = o->dataContainer()->find(m_szIdentifier);
	if(v)pBuffer->copyFrom(v);
	else pBuffer->setNothing();
#endif
	return true;
}

KviKvsRWEvaluationResult * KviKvsTreeNodeObjectField::evaluateReadWriteInObjectScope(KviKvsObject * o,KviKvsRunTimeContext * c)
{
#ifdef COMPILE_NEW_KVS
	return new KviKvsHashElement(0,o->dataContainer()->get(m_szIdentifier),o->dataContainer(),m_szIdentifier);
#else
	return 0;
#endif
}

