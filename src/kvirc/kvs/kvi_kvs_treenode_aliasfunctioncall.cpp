//=============================================================================
//
//   File : kvi_kvs_treenode_aliasfunctioncall.cpp
//   Created on Tue 07 Oct 2003 03:21:23 by Szymon Stefanek
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

#include "kvi_kvs_treenode_aliasfunctioncall.h"
#include "kvi_kvs_variantlist.h"
#include "kvi_kvs_aliasmanager.h"
#include "kvi_locale.h"
#include "kvi_qstring.h"

KviKvsTreeNodeAliasFunctionCall::KviKvsTreeNodeAliasFunctionCall(const QChar * pLocation,const QString &szAliasName,KviKvsTreeNodeDataList * pParams)
: KviKvsTreeNodeFunctionCall(pLocation,szAliasName,pParams)
{

}

KviKvsTreeNodeAliasFunctionCall::~KviKvsTreeNodeAliasFunctionCall()
{
}

void KviKvsTreeNodeAliasFunctionCall::contextDescription(QString &szBuffer)
{
	KviQString::sprintf(szBuffer,"Alias Function Call \"%Q\"",&m_szFunctionName);
}



void KviKvsTreeNodeAliasFunctionCall::dump(const char * prefix)
{
	qDebug("%s AliasFunctionCall(%s)",prefix,m_szFunctionName.toUtf8().data());
	QString tmp = prefix;
	tmp.append("  ");
	m_pParams->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeAliasFunctionCall::evaluateReadOnly(KviKvsRunTimeContext * c,KviKvsVariant * pBuffer)
{
	KviKvsVariantList l;
	if(!m_pParams->evaluate(c,&l))return false;
	
	pBuffer->setNothing();

	const KviKvsScript * s = KviKvsAliasManager::instance()->lookup(m_szFunctionName);
	if(!s)
	{
		c->error(this,__tr2qs("Call to undefined function '%Q'"),&m_szFunctionName);
		return false;
	}

	KviKvsScript copy(*s); // quick reference

	if(!copy.run(c->window(),&l,pBuffer,KviKvsScript::PreserveParams))
	{
		c->error(this,__tr2qs("Error in inner alias function call '%Q', called from this context"),&m_szFunctionName);
		return false;
	}
	return true;
}
