//=============================================================================
//
//   File : kvi_kvs_treenode_hashcount.cpp
//   Created on Tue 07 Oct 2003 03:05:30 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003 Szymon Stefanek <pragma at kvirc dot net>
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

#include "kvi_kvs_treenode_hashcount.h"
#include "kvi_kvs_variant.h"
#include "kvi_kvs_runtimecontext.h"
#include "kvi_kvs_object.h"
#include "kvi_locale.h"


KviKvsTreeNodeHashCount::KviKvsTreeNodeHashCount(const QChar * pLocation,KviKvsTreeNodeData * pSource)
: KviKvsTreeNodeIndirectData(pLocation,pSource)
{
}

KviKvsTreeNodeHashCount::~KviKvsTreeNodeHashCount()
{
}

void KviKvsTreeNodeHashCount::contextDescription(QString &szBuffer)
{
#ifdef COMPILE_NEW_KVS
	szBuffer = "Hash Count Operator";
#endif
}

void KviKvsTreeNodeHashCount::dump(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	debug("%s HashCount",prefix);
#endif
}

bool KviKvsTreeNodeHashCount::evaluateReadOnlyInObjectScope(KviKvsObject *o,KviKvsRunTimeContext * c,KviKvsVariant * pBuffer)
{
#ifdef COMPILE_NEW_KVS
	KviKvsVariant val;
	if(o)
	{
		if(!m_pSource->evaluateReadOnlyInObjectScope(o,c,&val))return false;
	} else {
		if(!m_pSource->evaluateReadOnly(c,&val))return false;
	}

	if(!val.isHash())
	{
		if(!val.isNothing())
		{
			QString szType;
			val.getTypeName(szType);
			c->warning(this,__tr2qs("The argument of the hash count '#' operator didn't evaluate to a hash: automatic conversion from type '%Q' supplied"),&szType);
		}
		pBuffer->setInteger(0);
		return true;
	}

	pBuffer->setInteger(val.hash()->size());
#endif
	return true;
}
bool KviKvsTreeNodeHashCount::evaluateReadOnly(KviKvsRunTimeContext * c,KviKvsVariant * pBuffer)
{
	return evaluateReadOnlyInObjectScope(0,c,pBuffer);
}

