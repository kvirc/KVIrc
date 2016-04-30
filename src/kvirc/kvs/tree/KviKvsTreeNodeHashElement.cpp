//=============================================================================
//
//   File : KviKvsTreeNodeHashElement.cpp
//   Creation date : Tue 07 Oct 2003 03:04:18 by Szymon Stefanek
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

#include "KviKvsTreeNodeHashElement.h"
#include "KviKvsRunTimeContext.h"
#include "KviLocale.h"
#include "KviKvsHash.h"
#include "KviKvsObject.h"

KviKvsTreeNodeHashElement::KviKvsTreeNodeHashElement(const QChar * pLocation, KviKvsTreeNodeData * pSource, KviKvsTreeNodeData * pKey)
    : KviKvsTreeNodeArrayOrHashElement(pLocation, pSource)
{
	m_pKey = pKey;
	m_pKey->setParent(this);
}

KviKvsTreeNodeHashElement::~KviKvsTreeNodeHashElement()
{
	delete m_pKey;
}

void KviKvsTreeNodeHashElement::contextDescription(QString & szBuffer)
{
	szBuffer = "Hash Element Evaluation";
}

void KviKvsTreeNodeHashElement::dump(const char * prefix)
{
	qDebug("%s HashElement", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pSource->dump(tmp.toUtf8().data());
	m_pKey->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeHashElement::evaluateReadOnlyInObjectScope(KviKvsObject * o, KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	KviKvsVariant key;
	if(!m_pKey->evaluateReadOnly(c, &key))
		return false;

	QString szKey;
	key.asString(szKey);

	if(szKey.isEmpty())
	{
		c->warning(this, __tr2qs_ctx("Hash key evaluated to empty string: fix the script", "kvs"));
		pBuffer->setNothing();
		return true;
	}

	KviKvsVariant val;
	if(o)
	{
		if(!m_pSource->evaluateReadOnlyInObjectScope(o, c, &val))
			return false;
	}
	else
	{
		if(!m_pSource->evaluateReadOnly(c, &val))
			return false;
	}

	if(!val.isHash())
	{
		if(!val.isNothing())
		{
			QString szType;
			val.getTypeName(szType);
			c->warning(this, __tr2qs_ctx("The argument of the {} subscript didn't evaluate to a hash: automatic conversion from type '%Q' supplied", "kvs"), &szType);
		}
		pBuffer->setNothing();
		return true;
	}

	KviKvsVariant * v = val.hash()->find(szKey);
	if(!v)
	{
		pBuffer->setNothing();
		return true;
	}

	pBuffer->copyFrom(v);
	return true;
}

KviKvsRWEvaluationResult * KviKvsTreeNodeHashElement::evaluateReadWriteInObjectScope(KviKvsObject * o, KviKvsRunTimeContext * c)
{
	KviKvsVariant key;
	if(!m_pKey->evaluateReadOnly(c, &key))
		return nullptr;

	QString szKey;
	key.asString(szKey);

	if(szKey.isEmpty())
	{
		c->warning(this, __tr2qs_ctx("Hash key evaluated to empty string: fix the script", "kvs"));
	}

	KviKvsRWEvaluationResult * result;
	if(o)
		result = m_pSource->evaluateReadWriteInObjectScope(o, c);
	else
		result = m_pSource->evaluateReadWrite(c);
	if(!result)
		return nullptr;

	if(!result->result()->isHash())
	{
		// convert to hash in some way
		//#warning "Supply a *real* conversion from other types to array ?"
		if(!result->result()->isNothing())
		{
			QString szType;
			result->result()->getTypeName(szType);
			c->warning(this, __tr2qs_ctx("The argument of the {} subscript didn't evaluate to a hash: automatic conversion from %Q supplied", "kvs"), &szType);
		}
		result->result()->setHash(new KviKvsHash());
	}

	return new KviKvsHashElement(result, result->result()->hash()->get(szKey), result->result()->hash(), szKey);
}

bool KviKvsTreeNodeHashElement::evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer)
{
	return evaluateReadOnlyInObjectScope(nullptr, c, pBuffer);
}

KviKvsRWEvaluationResult * KviKvsTreeNodeHashElement::evaluateReadWrite(KviKvsRunTimeContext * c)
{
	return evaluateReadWriteInObjectScope(nullptr, c);
}
