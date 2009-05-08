//=============================================================================
//
//   File : kvi_kvs_object_controller.cpp
//   Creation date : Sun 24 Apr 2005 05:23:04 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005-2008 Szymon Stefanek <pragma at kvirc dot net>
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

#include "kvi_kvs_object_controller.h"
#include "kvi_modulemanager.h"
#include "kvi_fileutils.h"
#include "kvi_app.h"

////////////////////////////////////////////////////////////////////////////////////////

static KviKvsObject * objectClassCreateInstance(KviKvsObjectClass *pClass,KviKvsObject *pParent,const QString &szName)
{
	return new KviKvsObject(pClass,pParent,szName);
}

////////////////////////////////////////////////////////////////////////////////////////

KviKvsObjectController::KviKvsObjectController()
{
	m_pTopLevelObjectList = new KviPointerList<KviKvsObject>;
	m_pTopLevelObjectList->setAutoDelete(false);
	m_pObjectDict = new KviPointerHashTable<void *,KviKvsObject>(101);
	m_pObjectDict->setAutoDelete(false);
	m_pClassDict = new KviPointerHashTable<QString,KviKvsObjectClass>(31,false);
	m_pClassDict->setAutoDelete(false);
}

KviKvsObjectController::~KviKvsObjectController()
{
	flushUserClasses();
	while(m_pTopLevelObjectList->first())delete m_pTopLevelObjectList->first();
	delete m_pTopLevelObjectList; // empty list
	delete m_pObjectDict; // empty dict
	m_pObjectDict = 0;
	delete m_pObjectClass; // delete the class tree
	delete m_pClassDict;  // empty dict
}

void KviKvsObjectController::init()
{
	// allocate the "object" builtin class
	// this is the only one that is always in core memory
	m_pObjectClass = new KviKvsObjectClass(0,"object",objectClassCreateInstance,true);

	m_pObjectClass->registerFunctionHandler("name",KVI_PTR2MEMBER(KviKvsObject::function_name));
	m_pObjectClass->registerFunctionHandler("startTimer",KVI_PTR2MEMBER(KviKvsObject::function_startTimer));
	m_pObjectClass->registerFunctionHandler("killTimer",KVI_PTR2MEMBER(KviKvsObject::function_killTimer));
	m_pObjectClass->registerFunctionHandler("className",KVI_PTR2MEMBER(KviKvsObject::function_className));
	m_pObjectClass->registerFunctionHandler("findChild",KVI_PTR2MEMBER(KviKvsObject::function_findChild));
	m_pObjectClass->registerFunctionHandler("childCount",KVI_PTR2MEMBER(KviKvsObject::function_childCount));
	m_pObjectClass->registerFunctionHandler("emit",KVI_PTR2MEMBER(KviKvsObject::function_emit));
	m_pObjectClass->registerFunctionHandler("children",KVI_PTR2MEMBER(KviKvsObject::function_children));
	m_pObjectClass->registerFunctionHandler("signalSender",KVI_PTR2MEMBER(KviKvsObject::function_signalSender));
	m_pObjectClass->registerFunctionHandler("signalName",KVI_PTR2MEMBER(KviKvsObject::function_signalName));
	m_pObjectClass->registerFunctionHandler("destructor",KVI_PTR2MEMBER(KviKvsObject::function_destructor));
	m_pObjectClass->registerFunctionHandler("parent",KVI_PTR2MEMBER(KviKvsObject::function_parent));
	m_pObjectClass->registerFunctionHandler("property",KVI_PTR2MEMBER(KviKvsObject::function_property));
	m_pObjectClass->registerFunctionHandler("setProperty",KVI_PTR2MEMBER(KviKvsObject::function_setProperty));
	m_pObjectClass->registerFunctionHandler("listProperties",KVI_PTR2MEMBER(KviKvsObject::function_listProperties));

	m_pObjectClass->registerStandardNothingReturnFunctionHandler("constructor");
	m_pObjectClass->registerStandardNothingReturnFunctionHandler("timerEvent");

	m_pClassDict->insert("object",m_pObjectClass);
}

void KviKvsObjectController::killAllObjectsWithClass(KviKvsObjectClass * pClass)
{
	if(!m_pObjectDict)return; // no more objects at all...
	KviPointerList<KviKvsObject> l;
	l.setAutoDelete(true);

	for(KviKvsObject * o = m_pTopLevelObjectList->first();o;o = m_pTopLevelObjectList->next())
	{
		if(o->getClass() == pClass)l.append(o);
		else o->killAllChildrenWithClass(pClass);
	}
}

void KviKvsObjectController::clearUserClasses()
{
	flushUserClasses();
	KviPointerHashTableIterator<QString,KviKvsObjectClass> it(*m_pClassDict);
	KviPointerList<KviKvsObjectClass> l;
	l.setAutoDelete(true);
	while(it.current())
	{
		if(!(it.current()->isBuiltin()))
		{
			l.append(it.current());
		}
		++it;
	}
}

void KviKvsObjectController::clearInstances()
{
	while(m_pTopLevelObjectList->first())delete m_pTopLevelObjectList->first();
	delete m_pTopLevelObjectList; // empty list
	delete m_pObjectDict; // empty dict
	m_pTopLevelObjectList = new KviPointerList<KviKvsObject>;
	m_pTopLevelObjectList->setAutoDelete(false);
	m_pObjectDict = new KviPointerHashTable<void *,KviKvsObject>(101);
	m_pObjectDict->setAutoDelete(false);
}

void KviKvsObjectController::registerClass(KviKvsObjectClass *pClass)
{
	m_pClassDict->insert(pClass->name(),pClass);
}

void KviKvsObjectController::unregisterClass(KviKvsObjectClass *pClass)
{
	if(!pClass->isBuiltin())
	{
		if(pClass->isDirty())flushUserClasses();
	}
	m_pClassDict->remove(pClass->name());
}

void KviKvsObjectController::registerObject(KviKvsObject *pObject)
{
	m_pObjectDict->insert(pObject->handle(),pObject);
	if(pObject->parent() == 0)m_pTopLevelObjectList->append(pObject);
}

void KviKvsObjectController::unregisterObject(KviKvsObject *pObject)
{
	m_pObjectDict->remove(pObject->handle());
	if(pObject->parent() == 0)m_pTopLevelObjectList->removeRef(pObject);
}

void KviKvsObjectController::flushUserClasses()
{
	KviPointerHashTableIterator<QString,KviKvsObjectClass> it(*m_pClassDict);
	while(KviKvsObjectClass * c = it.current())
	{
		if(!c->isBuiltin())
		{
			if(c->isDirty())
			{
				QString szPath;
				QString szFileName = c->name().toLower();
				szFileName += ".kvs";
				szFileName.replace("::","--");
				g_pApp->getLocalKvircDirectory(szPath,KviApp::Classes,szFileName);
				if(c->save(szPath))
					c->clearDirtyFlag();
				else
					debug("Oops.. failed to save the object class %s",c->name().toLatin1().data());
			}
		}
		++it;
	}
}

KviKvsObjectClass * KviKvsObjectController::lookupClass(const QString &szClass,bool bBuiltinOnly)
{
	KviKvsObjectClass * pC = m_pClassDict->find(szClass);
	if(!pC)
	{
		// maybe we need to load the object library ?
		KviModule * pModule = g_pModuleManager->getModule("objects");
		if(!pModule)
		{
			debug("ops...something wrong with the libkviobjects module!");
			return 0;
		} else pC = m_pClassDict->find(szClass);
		if(!pC)
		{
			if(bBuiltinOnly)return 0;
			// maybe we need to load it from permanent storage...
			QString szPath;
			QString szFileName = szClass.toLower();
			szFileName += ".kvs";
			szFileName.replace("::","--");
			g_pApp->getLocalKvircDirectory(szPath,KviApp::Classes,szFileName);
			if(!KviFileUtils::fileExists(szPath))
				g_pApp->getGlobalKvircDirectory(szPath,KviApp::Classes,szFileName);
			if(!KviFileUtils::fileExists(szPath))return 0;
			if(!KviKvsObjectClass::load(szPath))return 0;
			pC = m_pClassDict->find(szClass);
			if(pC)pC->clearDirtyFlag(); // just loaded from disk: no need to sync it
		}
	} else {
		if(bBuiltinOnly)
		{
			if(!pC->isBuiltin())return 0;
		}
	}
	return pC;
};


void KviKvsObjectController::deleteClass(KviKvsObjectClass * pClass)
{
	QString szPath;
	QString szFileName = pClass->name().toLower();
	szFileName += ".kvs";
	szFileName.replace("::","--");
	g_pApp->getLocalKvircDirectory(szPath,KviApp::Classes,szFileName);
	KviFileUtils::removeFile(szPath);
	delete pClass;
}
