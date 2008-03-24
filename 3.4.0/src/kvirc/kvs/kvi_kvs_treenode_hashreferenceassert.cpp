//=============================================================================
//
//   File : kvi_kvs_treenode_hashreferenceassert.cpp
//   Created on Thu 16 Oct 2003 23:43:06 by Szymon Stefanek
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

#include "kvi_kvs_treenode_hashreferenceassert.h"
#include "kvi_kvs_rwevaluationresult.h"
#include "kvi_kvs_runtimecontext.h"
#include "kvi_kvs_variant.h"
#include "kvi_locale.h"

KviKvsTreeNodeHashReferenceAssert::KviKvsTreeNodeHashReferenceAssert(const QChar * pLocation,KviKvsTreeNodeData * pSource)
: KviKvsTreeNodeIndirectData(pLocation,pSource)
{
}

KviKvsTreeNodeHashReferenceAssert::~KviKvsTreeNodeHashReferenceAssert()
{
}

bool KviKvsTreeNodeHashReferenceAssert::isReadOnly()
{
	return m_pSource->isReadOnly();
}

void KviKvsTreeNodeHashReferenceAssert::contextDescription(QString &szBuffer)
{
#ifdef COMPILE_NEW_KVS
	szBuffer = "Hash Reference Assert";
#endif
}

void KviKvsTreeNodeHashReferenceAssert::dump(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	debug("%s HashReferenceAssert",prefix);
#endif
}

bool KviKvsTreeNodeHashReferenceAssert::evaluateReadOnlyInObjectScope(KviKvsObject *o,KviKvsRunTimeContext * c,KviKvsVariant * pBuffer)
{
#ifdef COMPILE_NEW_KVS
	if(o)
	{
		if(!m_pSource->evaluateReadOnlyInObjectScope(o,c,pBuffer))return false;
	} else {
		if(!m_pSource->evaluateReadOnly(c,pBuffer))return false;
	}
	
	if(!pBuffer->isHash())
	{
		if(!pBuffer->isNothing())
		{
			QString szType;
			pBuffer->getTypeName(szType);
			c->error(this,__tr2qs("Hash reference assert failed: the variable evaluated to type '%Q'"),&szType);
			return false;
		}
	}
#endif
	return true;
}

KviKvsRWEvaluationResult * KviKvsTreeNodeHashReferenceAssert::evaluateReadWriteInObjectScope(KviKvsObject *o,KviKvsRunTimeContext * c)
{
#ifdef COMPILE_NEW_KVS
	KviKvsRWEvaluationResult * r;
	if(o)r = m_pSource->evaluateReadWriteInObjectScope(o,c);
	else r = m_pSource->evaluateReadWrite(c);
	if(!r)return false;

	return r;
#else
	return 0;
#endif
}

bool KviKvsTreeNodeHashReferenceAssert::evaluateReadOnly(KviKvsRunTimeContext * c,KviKvsVariant * pBuffer)
{
	return evaluateReadOnlyInObjectScope(0,c,pBuffer);
}

KviKvsRWEvaluationResult * KviKvsTreeNodeHashReferenceAssert::evaluateReadWrite(KviKvsRunTimeContext * c)
{
	return evaluateReadWriteInObjectScope(0,c);
}
