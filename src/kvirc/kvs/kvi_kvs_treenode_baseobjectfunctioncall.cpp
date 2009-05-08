//=============================================================================
//
//   File : kvi_kvs_treenode_baseobjectfunctioncall.cpp
//   Creation date : Tue 07 Oct 2003 03:30:10 by Szymon Stefanek
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

#include "kvi_kvs_treenode_baseobjectfunctioncall.h"
#include "kvi_qstring.h"
#include "kvi_kvs_object.h"
#include "kvi_kvs_variant.h"

KviKvsTreeNodeBaseObjectFunctionCall::KviKvsTreeNodeBaseObjectFunctionCall(const QChar * pLocation,const QString &szBaseClass,const QString &szFncName,KviKvsTreeNodeDataList * pParams)
: KviKvsTreeNodeObjectFunctionCall(pLocation,szFncName,pParams)
{
	m_szBaseClass = szBaseClass;
}

KviKvsTreeNodeBaseObjectFunctionCall::~KviKvsTreeNodeBaseObjectFunctionCall()
{
}

void KviKvsTreeNodeBaseObjectFunctionCall::contextDescription(QString &szBuffer)
{
	KviQString::sprintf(szBuffer,"Base Object Function Call \"%s::%s\"",&m_szBaseClass,&m_szFunctionName);
}

void KviKvsTreeNodeBaseObjectFunctionCall::dump(const char * prefix)
{
	qDebug("%s BaseObjectFunctionCall(%s::%s)",prefix,m_szBaseClass.toUtf8().data(),m_szFunctionName.toUtf8().data());
	QString tmp = prefix;
	tmp.append("  ");
	m_pParams->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeBaseObjectFunctionCall::evaluateReadOnlyInObjectScope(KviKvsObject * o,KviKvsRunTimeContext * c,KviKvsVariant * pBuffer)
{
	KviKvsVariantList l;
	if(!m_pParams->evaluate(c,&l))return false;
	pBuffer->setNothing();
	c->setDefaultReportLocation(this);
	return o->callFunction(c->thisObject(),m_szFunctionName,m_szBaseClass,c,pBuffer,&l);
}
