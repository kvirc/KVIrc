//=============================================================================
//
//   File : KviKvsTreeNodeScopeOperator.cpp
//   Creation date : Tue 07 Oct 2003 02:33:01 by Szymon Stefanek
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

#include "KviKvsTreeNodeScopeOperator.h"

#include "KviQString.h"

#include "KviKvsVariant.h"
#include "KviKvsRunTimeContext.h"
#include "KviKvsKernel.h"
#include "KviKvsObject.h"
#include "KviKvsObjectController.h"

#include "KviLocale.h"

KviKvsTreeNodeScopeOperator::KviKvsTreeNodeScopeOperator(const QChar * pLocation, KviKvsTreeNodeData * pObject, KviKvsTreeNodeData * pData)
    : KviKvsTreeNodeData(pLocation)
{
	m_pObjectReference = pObject;
	m_pObjectReference->setParent(this);
	m_pRightSide = pData;
	m_pRightSide->setParent(this);
}

KviKvsTreeNodeScopeOperator::~KviKvsTreeNodeScopeOperator()
{
	delete m_pObjectReference;
	delete m_pRightSide;
}

void KviKvsTreeNodeScopeOperator::contextDescription(QString & szBuffer)
{
	szBuffer = "Scope Operator";
}

void KviKvsTreeNodeScopeOperator::dump(const char * prefix)
{
	qDebug("%s ScopeOperator", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pObjectReference->dump(tmp.toUtf8().data());
	m_pRightSide->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeScopeOperator::isReadOnly()
{
	return m_pRightSide->isReadOnly();
}

bool KviKvsTreeNodeScopeOperator::canEvaluateToObjectReference()
{
	return m_pRightSide->canEvaluateToObjectReference();
}

bool KviKvsTreeNodeScopeOperator::isFunctionCall()
{
	return m_pRightSide->isFunctionCall();
}

bool KviKvsTreeNodeScopeOperator::canEvaluateInObjectScope()
{
	return m_pObjectReference->canEvaluateInObjectScope();
}

KviKvsObject * KviKvsTreeNodeScopeOperator::objectFromVariant(KviKvsRunTimeContext * c, KviKvsVariant * v)
{
	kvs_hobject_t hObject;
	if(!v->asHObject(hObject))
	{
		c->error(this, __tr2qs_ctx("The left side of the scope operator didn't evaluate as an object reference", "kvs"));
		return nullptr;
	}

	if(hObject == (kvs_hobject_t) nullptr)
	{
		c->error(this, __tr2qs_ctx("The left side of the scope operator evaluated to a null object reference", "kvs"));
		return nullptr;
	}

	KviKvsObject * o = KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if(!o)
	{
		c->error(this, __tr2qs_ctx("The left side of the scope operator evaluated to an invalid object reference (object doesn't exist)", "kvs"));
		return nullptr;
	}
	return o;
}

bool KviKvsTreeNodeScopeOperator::evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	KviKvsVariant ret;
	if(!m_pObjectReference->evaluateReadOnly(c, &ret))
		return false;
	KviKvsObject * o = objectFromVariant(c, &ret);
	if(!o)
		return false;
	return m_pRightSide->evaluateReadOnlyInObjectScope(o, c, pBuffer);
}

KviKvsRWEvaluationResult * KviKvsTreeNodeScopeOperator::evaluateReadWrite(KviKvsRunTimeContext * c)
{
	KviKvsVariant ret;
	if(!m_pObjectReference->evaluateReadOnly(c, &ret))
		return nullptr;
	KviKvsObject * o = objectFromVariant(c, &ret);
	if(!o)
		return nullptr;
	return m_pRightSide->evaluateReadWriteInObjectScope(o, c);
}

bool KviKvsTreeNodeScopeOperator::evaluateReadOnlyInObjectScope(KviKvsObject * o, KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	KviKvsVariant ret;
	if(!m_pObjectReference->evaluateReadOnlyInObjectScope(o, c, &ret))
		return false;
	KviKvsObject * pObject = objectFromVariant(c, &ret);
	if(!pObject)
		return false;
	return m_pRightSide->evaluateReadOnlyInObjectScope(pObject, c, pBuffer);
}

KviKvsRWEvaluationResult * KviKvsTreeNodeScopeOperator::evaluateReadWriteInObjectScope(KviKvsObject * o, KviKvsRunTimeContext * c)
{
	KviKvsVariant ret;
	if(!m_pObjectReference->evaluateReadOnlyInObjectScope(o, c, &ret))
		return nullptr;
	KviKvsObject * pObject = objectFromVariant(c, &ret);
	if(!pObject)
		return nullptr;
	return m_pRightSide->evaluateReadWriteInObjectScope(pObject, c);
}
