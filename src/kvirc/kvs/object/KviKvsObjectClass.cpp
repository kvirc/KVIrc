//=============================================================================
//
//   File : KviKvsObjectClass.cpp
//   Creation date : Sat 23 Apr 2005 20:31:32 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
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

#include "KviKvsObjectClass.h"
#include "KviKvsObjectController.h"
#include "KviKvsObject.h"
#include "KviKvsKernel.h"
#include "KviKvsObjectFunctionHandlerImpl.h"
#include "KviKvsObjectFunctionCall.h"
#include "KviKvsRunTimeContext.h"
#include "KviKvsVariantList.h"
#include "KviKvsScript.h"
#include "KviFileUtils.h"
#include "KviWindow.h"
#include "KviCommandFormatter.h"
#include "KviLocale.h"

KviKvsObjectClass::KviKvsObjectClass(
    KviKvsObjectClass * pParent,
    const QString & szName,
    KviKvsObjectAllocateInstanceProc pProc,
    bool bBuiltin)
{
	m_pParentClass = pParent;
	if(m_pParentClass)
		m_pParentClass->registerChildClass(this);
	m_szName = szName;
	m_bBuiltin = bBuiltin;
	m_bDirty = !bBuiltin;
	m_pFunctionHandlers = new KviPointerHashTable<QString, KviKvsObjectFunctionHandler>(17, false);
	m_pFunctionHandlers->setAutoDelete(true);
	m_pChildClasses = new KviPointerList<KviKvsObjectClass>;
	m_pChildClasses->setAutoDelete(false);
	m_allocProc = pProc ? pProc : pParent->m_allocProc;

	// inherit everything from the class above
	if(pParent)
	{
		KviPointerHashTableIterator<QString, KviKvsObjectFunctionHandler> it(*(pParent->functionHandlers()));
		while(KviKvsObjectFunctionHandler * fh = it.current())
		{
			m_pFunctionHandlers->insert(it.currentKey(), fh->clone());
			KviKvsObjectFunctionHandler * f = m_pFunctionHandlers->find(it.currentKey());
			if(f->isScriptHandler())
			{
				f->setClone(true);
			}
			++it;
		}
	}

	// "object" class is automatically registered in the controller constructor
	KviKvsKernel::instance()->objectController()->registerClass(this);
}

KviKvsObjectClass::~KviKvsObjectClass()
{
	// order here is critical

	// first of all kill our child classes
	while(m_pChildClasses->first())
	{
		// KviKvsKernel::instance()->objectController()->deleteClass(m_pChildClasses->first());
		delete m_pChildClasses->first();
	}
	// then kill all objects that belong to our class
	KviKvsKernel::instance()->objectController()->killAllObjectsWithClass(this);
	// now we're quite clean: should have no object depending on us alive
	// unregister from the parent, if any
	if(m_pParentClass)
		m_pParentClass->unregisterChildClass(this);
	// unregister from the object controller
	KviKvsKernel::instance()->objectController()->unregisterClass(this);
	// and start effectively dying
	delete m_pFunctionHandlers;
	// this is empty now
	delete m_pChildClasses;
}

void KviKvsObjectClass::registerFunctionHandler(const QString & szFunctionName, KviKvsObjectFunctionHandlerProc pProc, unsigned int uFlags)
{
	m_pFunctionHandlers->replace(szFunctionName, new KviKvsObjectCoreCallFunctionHandler(pProc, uFlags));
}

void KviKvsObjectClass::registerFunctionHandler(const QString & szFunctionName, const QString & szBuffer, const QString & szReminder, unsigned int uFlags)
{
	QString szContext = m_szName;
	szContext += "::";
	szContext += szFunctionName;
	m_pFunctionHandlers->replace(szFunctionName, new KviKvsObjectScriptFunctionHandler(szContext, szBuffer, szReminder, uFlags));
}

void KviKvsObjectClass::registerStandardNothingReturnFunctionHandler(const QString & szFunctionName)
{
	m_pFunctionHandlers->replace(szFunctionName, new KviKvsObjectStandardNothingReturnFunctionHandler());
}

void KviKvsObjectClass::registerStandardTrueReturnFunctionHandler(const QString & szFunctionName)
{
	m_pFunctionHandlers->replace(szFunctionName, new KviKvsObjectStandardTrueReturnFunctionHandler());
}

void KviKvsObjectClass::registerStandardFalseReturnFunctionHandler(const QString & szFunctionName)
{
	m_pFunctionHandlers->replace(szFunctionName, new KviKvsObjectStandardFalseReturnFunctionHandler());
}

KviKvsObject * KviKvsObjectClass::allocateInstance(KviKvsObject * pParent, const QString & szName, KviKvsRunTimeContext * pContext, KviKvsVariantList * pParams)
{
	if(!m_allocProc)
		return nullptr;

	KviKvsObject * pObject = m_allocProc(this, pParent, szName);
	if(!pObject)
		return nullptr;

	if(!pObject->init(pContext, pParams))
	{
		// internal init failure : abort
		pObject->dieNow();
		return nullptr;
	}

	// copy params
	KviKvsVariantList copy;
	copy.setAutoDelete(false);

	KviKvsVariant * v = pParams->first();

	while(v)
	{
		copy.append(v);
		v = pParams->next();
	}
	KviKvsVariant ret;

	if(!pObject->callFunction(pObject, "constructor", QString(), pContext, &ret, &copy))
	{
		// oops... constructor failed (script error!)
		pObject->dieNow();
		return nullptr;
	}

	if(!ret.isEmpty())
		pContext->warning(__tr2qs_ctx("It's not allowed to return values in the constructor", "kvs"));

	return pObject;
}

void KviKvsObjectClass::registerChildClass(KviKvsObjectClass * pClass)
{
	m_pChildClasses->append(pClass);
}

void KviKvsObjectClass::unregisterChildClass(KviKvsObjectClass * pClass)
{
	m_pChildClasses->removeRef(pClass);
}

bool KviKvsObjectClass::save(const QString & szFileName)
{
	if(!m_pParentClass)
		return false;
	if(isBuiltin())
		return false;

	QString szBuffer = QString("# Automatically generated KVS class dump\n\nclass(\"%1\",\"%2\")\n{\n").arg(m_szName, m_pParentClass->name());

	KviPointerHashTableIterator<QString, KviKvsObjectFunctionHandler> it(*m_pFunctionHandlers);

	while(KviKvsObjectFunctionHandler * h = it.current())
	{
		if(h->isScriptHandler() && !h->isClone())
		{
			szBuffer += "	";
			if(h->flags() & KviKvsObjectFunctionHandler::Internal)
				szBuffer += "internal ";
			szBuffer += "function ";
			szBuffer += it.currentKey();
			szBuffer += "(" + h->reminder() + ")\n";
			QString szCode = h->scriptHandlerCode();
			KviCommandFormatter::blockFromBuffer(szCode);
			KviCommandFormatter::indent(szCode);
			szBuffer += szCode;
			szBuffer += "\n";
		}
		++it;
	}

	szBuffer += "}\n";

	return KviFileUtils::writeFile(szFileName, szBuffer);
}
void KviKvsObjectClass::getFunctionCode(QString & szCode, KviKvsObjectFunctionHandler & h)
{
	szCode = h.scriptHandlerCode();
}

void KviKvsObjectClass::setReminder(const QString & szReminder, KviKvsObjectFunctionHandler * h)
{
	h->setReminder(szReminder);
}
QString KviKvsObjectClass::reminder(KviKvsObjectFunctionHandler * h)
{
	return h->reminder();
}

bool KviKvsObjectClass::load(const QString & szFileName)
{
	QString szBuffer;
	if(!KviFileUtils::readFile(szFileName, szBuffer, 0xffffff))
		return false;
	return KviKvsScript::run(szBuffer, g_pActiveWindow);
}
