//=============================================================================
//
//   File : KviKvsTreeNodeSpecialCommandClass.cpp
//   Creation date : Fri 12 Aug 2005 03:23:31 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2005-2010 Szymon Stefanek <pragma at kvirc dot net>
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
#include "KviLocale.h"
#include "KviKvsVariantList.h"
#include "KviKvsVariant.h"
#include "KviKvsTreeNodeSpecialCommandClass.h"
#include "KviKvsKernel.h"
#include "KviKvsObjectController.h"
#include "KviKvsObjectClass.h"

KviKvsTreeNodeSpecialCommandClassFunctionDefinition::KviKvsTreeNodeSpecialCommandClassFunctionDefinition(const QChar * pLocation, const QString & szName, const QString & szBuffer, const QString & szReminder, unsigned int uHandlerFlags)
    : KviKvsTreeNode(pLocation)
{
	m_uHandlerFlags = uHandlerFlags;
	m_szName = szName;
	m_szReminder = szReminder;
	m_szBuffer = szBuffer;
}

void KviKvsTreeNodeSpecialCommandClassFunctionDefinition::dump(const char * prefix)
{
	qDebug("%s SpecialCommandClassFunctionDefinition(%s)", prefix, m_szName.toUtf8().data());
	qDebug("%s    (command buffer with %d characters)", prefix, m_szBuffer.length());
}

void KviKvsTreeNodeSpecialCommandClassFunctionDefinition::contextDescription(QString & szBuffer)
{
	szBuffer = QString("Object Member Function Definition '%1'").arg(m_szName);
}

KviKvsTreeNodeSpecialCommandClass::KviKvsTreeNodeSpecialCommandClass(const QChar * pLocation, KviKvsTreeNodeDataList * pParams)
    : KviKvsTreeNodeSpecialCommand(pLocation, "class")
{
	m_pParams = pParams;
	m_pParams->setParent(this);
	m_pFunctions = new KviPointerList<KviKvsTreeNodeSpecialCommandClassFunctionDefinition>;
	m_pFunctions->setAutoDelete(true);
}

KviKvsTreeNodeSpecialCommandClass::~KviKvsTreeNodeSpecialCommandClass()
{
	delete m_pParams;
	delete m_pFunctions;
}

void KviKvsTreeNodeSpecialCommandClass::addFunctionDefinition(KviKvsTreeNodeSpecialCommandClassFunctionDefinition * pDef)
{
	pDef->setParent(this);
	m_pFunctions->append(pDef);
}

void KviKvsTreeNodeSpecialCommandClass::contextDescription(QString & szBuffer)
{
	szBuffer = "Special Command 'class'";
}

void KviKvsTreeNodeSpecialCommandClass::dump(const char * prefix)
{
	qDebug("%s SpecialCommandClass", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pParams->dump(tmp.toUtf8().data());
	for(KviKvsTreeNodeSpecialCommandClassFunctionDefinition * d = m_pFunctions->first(); d; d = m_pFunctions->next())
		d->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeSpecialCommandClass::execute(KviKvsRunTimeContext * c)
{
	KviKvsVariantList l;
	if(!m_pParams->evaluate(c, &l))
		return false;

	KviKvsVariant * pClassName = l.first();
	if(!pClassName)
	{
		c->error(this, __tr2qs_ctx("Missing class name", "kvs"));
		return false;
	}

	KviKvsVariant * pBaseClassName = l.next();

	QString szClassName;
	QString szBaseClassName;
	pClassName->asString(szClassName);
	QRegExp re("[\\w:]+");
	if(!re.exactMatch(szClassName))
	{
		c->error(this, __tr2qs_ctx("Class names can contain only letters, digits, underscores and '::' namespace separators", "kvs"));
		return false;
	}
	if(pBaseClassName)
		pBaseClassName->asString(szBaseClassName);

	if(szClassName.isEmpty())
	{
		c->error(this, __tr2qs_ctx("Missing class name", "kvs"));
		return false;
	}

	if(szBaseClassName.isEmpty())
		szBaseClassName = "object";

	// avoid infinite recursion in loading the base class
	if(KviQString::equalCI(szBaseClassName, szClassName))
	{
		c->error(__tr2qs_ctx("A class can't be a subclass of itself", "kvs"));
		return false;
	}

	KviKvsObjectClass * pBaseClass = KviKvsKernel::instance()->objectController()->lookupClass(szBaseClassName);
	if(!pBaseClass)
	{
		c->error(this, __tr2qs_ctx("Couln't find base class named '%Q'", "kvs"), &szBaseClassName);
		return false;
	}

	// walk the inheritance tree of the base class in order to detect loops
	KviKvsObjectClass * pClass = pBaseClass;
	while(pClass)
	{
		if(KviQString::equalCI(pClass->name(), szClassName))
		{
			c->error(this, __tr2qs_ctx("Detected a loop in the inheritance tree of the base class '%Q': redefine that class first", "kvs"), &szBaseClassName);
			return false;
		}
		pClass = pClass->parentClass();
	}

	KviKvsObjectClass * pActualClass = KviKvsKernel::instance()->objectController()->lookupClass(szClassName, true);
	if(pActualClass)
	{
		c->error(this, __tr2qs_ctx("Can't override the builtin class '%Q'", "kvs"), &szClassName);
		return false;
	}
	pActualClass = new KviKvsObjectClass(pBaseClass, szClassName, nullptr, false);

	for(KviKvsTreeNodeSpecialCommandClassFunctionDefinition * d = m_pFunctions->first(); d; d = m_pFunctions->next())
	{
		pActualClass->registerFunctionHandler(d->name(), d->buffer(), d->reminder(), d->handlerFlags());
	}
	return true;
}
