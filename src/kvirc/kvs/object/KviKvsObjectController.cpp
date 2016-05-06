//=============================================================================
//
//   File : KviKvsObjectController.cpp
//   Creation date : Sun 24 Apr 2005 05:23:04 by Szymon Stefanek
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

#include "KviKvsObjectController.h"
#include "KviModuleManager.h"
#include "KviFileUtils.h"
#include "KviApplication.h"

#include <QPointer>

static KviKvsObject * objectClassCreateInstance(KviKvsObjectClass * pClass, KviKvsObject * pParent, const QString & szName)
{
	return new KviKvsObject(pClass, pParent, szName);
}

KviKvsObjectController::KviKvsObjectController()
{
	m_pTopLevelObjectList = new KviPointerList<KviKvsObject>;
	m_pTopLevelObjectList->setAutoDelete(false);
	m_pObjectDict = new KviPointerHashTable<void *, KviKvsObject>(101);
	m_pObjectDict->setAutoDelete(false);
	m_pClassDict = new KviPointerHashTable<QString, KviKvsObjectClass>(31, false);
	m_pClassDict->setAutoDelete(false);
}

KviKvsObjectController::~KviKvsObjectController()
{
	flushUserClasses();
	while(m_pTopLevelObjectList->first())
		m_pTopLevelObjectList->first()->dieNow();
	delete m_pTopLevelObjectList; // empty list
	delete m_pObjectDict;         // empty dict
	m_pObjectDict = nullptr;
	delete m_pObjectClass; // delete the class tree
	delete m_pClassDict;   // empty dict
}

void KviKvsObjectController::init()
{
	// allocate the "object" builtin class
	// this is the only one that is always in core memory
	m_pObjectClass = new KviKvsObjectClass(nullptr, "object", objectClassCreateInstance, true);

	m_pObjectClass->registerFunctionHandler("name", KVI_PTR2MEMBER(KviKvsObject::function_name));
	m_pObjectClass->registerFunctionHandler("startTimer", KVI_PTR2MEMBER(KviKvsObject::function_startTimer));
	m_pObjectClass->registerFunctionHandler("killTimer", KVI_PTR2MEMBER(KviKvsObject::function_killTimer));
	m_pObjectClass->registerFunctionHandler("className", KVI_PTR2MEMBER(KviKvsObject::function_className));
	m_pObjectClass->registerFunctionHandler("findChild", KVI_PTR2MEMBER(KviKvsObject::function_findChild));
	m_pObjectClass->registerFunctionHandler("childCount", KVI_PTR2MEMBER(KviKvsObject::function_childCount));
	m_pObjectClass->registerFunctionHandler("emit", KVI_PTR2MEMBER(KviKvsObject::function_emit));
	m_pObjectClass->registerFunctionHandler("children", KVI_PTR2MEMBER(KviKvsObject::function_children));
	m_pObjectClass->registerFunctionHandler("signalSender", KVI_PTR2MEMBER(KviKvsObject::function_signalSender));
	m_pObjectClass->registerFunctionHandler("signalName", KVI_PTR2MEMBER(KviKvsObject::function_signalName));
	m_pObjectClass->registerFunctionHandler("destructor", KVI_PTR2MEMBER(KviKvsObject::function_destructor));
	m_pObjectClass->registerFunctionHandler("parent", KVI_PTR2MEMBER(KviKvsObject::function_parent));
	m_pObjectClass->registerFunctionHandler("property", KVI_PTR2MEMBER(KviKvsObject::function_property));
	m_pObjectClass->registerFunctionHandler("setProperty", KVI_PTR2MEMBER(KviKvsObject::function_setProperty));
	m_pObjectClass->registerFunctionHandler("listProperties", KVI_PTR2MEMBER(KviKvsObject::function_listProperties));

	m_pObjectClass->registerStandardNothingReturnFunctionHandler("constructor");
	m_pObjectClass->registerStandardNothingReturnFunctionHandler("timerEvent");

	m_pClassDict->insert("object", m_pObjectClass);
}

void KviKvsObjectController::killAllObjectsWithClass(KviKvsObjectClass * pClass)
{
	if(!m_pObjectDict)
		return; // no more objects at all...

	KviPointerList<QPointer<KviKvsObject>> lDying;
	lDying.setAutoDelete(true);

	KviKvsObject * pObject;

	for(pObject = m_pTopLevelObjectList->first(); pObject; pObject = m_pTopLevelObjectList->next())
	{
		if(pObject->getClass() == pClass)
		{
			lDying.append(new QPointer<KviKvsObject>(pObject));
		}
		else
		{
			pObject->killAllChildrenWithClass(pClass);
		}
	}

	for(QPointer<KviKvsObject> * pObject = lDying.first(); pObject; pObject = lDying.next())
	{
		if(pObject->isNull())
			continue; // already dead ?
		(*pObject)->dieNow();
	}
}

void KviKvsObjectController::clearUserClasses()
{
	flushUserClasses();
	KviPointerHashTableIterator<QString, KviKvsObjectClass> it(*m_pClassDict);
	KviPointerList<KviKvsObjectClass> lDying;
	lDying.setAutoDelete(false);
	while(it.current())
	{
		if(!(it.current()->isBuiltin()))
			lDying.append(it.current());
		++it;
	}

	for(KviKvsObjectClass * pDyingClass = lDying.first(); pDyingClass; pDyingClass = lDying.next())
	{
		if(!m_pClassDict->findRef(pDyingClass))
			continue; // already deleted (by parent <-> child relations)
		delete pDyingClass;
	}
}

void KviKvsObjectController::clearInstances()
{
	while(m_pTopLevelObjectList->first())
		m_pTopLevelObjectList->first()->dieNow();

	delete m_pTopLevelObjectList; // empty list
	delete m_pObjectDict;         // empty dict

	m_pTopLevelObjectList = new KviPointerList<KviKvsObject>;
	m_pTopLevelObjectList->setAutoDelete(false);

	m_pObjectDict = new KviPointerHashTable<void *, KviKvsObject>(101);
	m_pObjectDict->setAutoDelete(false);
}

void KviKvsObjectController::registerClass(KviKvsObjectClass * pClass)
{
	m_pClassDict->insert(pClass->name(), pClass);
}

void KviKvsObjectController::unregisterClass(KviKvsObjectClass * pClass)
{
	if(!pClass->isBuiltin())
	{
		if(pClass->isDirty())
			flushUserClasses();
	}
	m_pClassDict->remove(pClass->name());
}

void KviKvsObjectController::registerObject(KviKvsObject * pObject)
{
	m_pObjectDict->insert(pObject->handle(), pObject);
	if(pObject->parent() == nullptr)
		m_pTopLevelObjectList->append(pObject);
}

void KviKvsObjectController::unregisterObject(KviKvsObject * pObject)
{
	m_pObjectDict->remove(pObject->handle());
	if(pObject->parent() == nullptr)
		m_pTopLevelObjectList->removeRef(pObject);
}

void KviKvsObjectController::flushUserClasses()
{
	KviPointerHashTableIterator<QString, KviKvsObjectClass> it(*m_pClassDict);
	while(KviKvsObjectClass * c = it.current())
	{
		if(!c->isBuiltin())
		{
			if(c->isDirty())
			{
				QString szPath;
				QString szFileName = c->name().toLower();
				szFileName += ".kvs";
				szFileName.replace("::", "--");
				g_pApp->getLocalKvircDirectory(szPath, KviApplication::Classes, szFileName);
				if(c->save(szPath))
					c->clearDirtyFlag();
				else
					qDebug("Oops! Failed to save the object class %s", c->name().toLatin1().data());
			}
		}
		++it;
	}
}

KviKvsObjectClass * KviKvsObjectController::lookupClass(const QString & szClass, bool bBuiltinOnly)
{
	KviKvsObjectClass * pC = m_pClassDict->find(szClass);
	if(!pC)
	{
		// maybe we need to load the object library ?
		KviModule * pModule = g_pModuleManager->getModule("objects");
		if(!pModule)
		{
			qDebug("Oops! something wrong with the libkviobjects module!");
			return nullptr;
		}
		else
			pC = m_pClassDict->find(szClass);
		if(!pC)
		{
			if(bBuiltinOnly)
				return nullptr;
			// maybe we need to load it from permanent storage...
			QString szPath;
			QString szFileName = szClass.toLower();
			szFileName += ".kvs";
			szFileName.replace("::", "--");
			g_pApp->getLocalKvircDirectory(szPath, KviApplication::Classes, szFileName);
			if(!KviFileUtils::fileExists(szPath))
				g_pApp->getGlobalKvircDirectory(szPath, KviApplication::Classes, szFileName);
			if(!KviFileUtils::fileExists(szPath))
				return nullptr;
			if(!KviKvsObjectClass::load(szPath))
				return nullptr;
			pC = m_pClassDict->find(szClass);
			if(pC)
				pC->clearDirtyFlag(); // just loaded from disk: no need to sync it
		}
	}
	else
	{
		if(bBuiltinOnly)
		{
			if(!pC->isBuiltin())
				return nullptr;
		}
	}
	return pC;
};

void KviKvsObjectController::deleteClass(KviKvsObjectClass * pClass)
{
	KVI_ASSERT(pClass);

	pClass->clearDirtyFlag(); // don't flush it to disk

	QString szPath;
	QString szFileName = pClass->name().toLower();
	szFileName += ".kvs";
	szFileName.replace("::", "--");
	g_pApp->getLocalKvircDirectory(szPath, KviApplication::Classes, szFileName);
	KviFileUtils::removeFile(szPath);

	delete pClass;
}
