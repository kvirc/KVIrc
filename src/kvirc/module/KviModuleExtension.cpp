//=============================================================================
//
//   File : KviModuleExtension.cpp
//   Creation date : Tue Sep 10 01:16:25 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#define _KVI_MODULEEXTENSION_CPP_

#include "KviModuleExtension.h"
#include "KviModule.h"
#include "KviModuleManager.h"
#include "KviApplication.h"

// created and destroyed in KviApplication.cpp
KVIRC_API KviModuleExtensionManager    * g_pModuleExtensionManager    = 0;


KviModuleExtensionDescriptor::KviModuleExtensionDescriptor(KviModule * m,const KviCString &szType,const KviCString &szName,const QString &szVisibleName,KviModuleExtensionAllocRoutine r,const QPixmap &pix)
{
	m_iId = KviApplication::getGloballyUniqueId();

	m_pModule = m;
	m_szType = szType;
	m_szName = szName;
	m_szVisibleName = szVisibleName;
	m_allocRoutine = r;
	m_pObjectList = new KviPointerList<KviModuleExtension>;
	m_pObjectList->setAutoDelete(false);
	if(pix.isNull())m_pIcon = 0;
	else m_pIcon = new QPixmap(pix);
}

KviModuleExtensionDescriptor::~KviModuleExtensionDescriptor()
{
	while(KviModuleExtension * e = m_pObjectList->first())e->die();
	delete m_pObjectList;
	if(m_pIcon)delete m_pIcon;
}

void KviModuleExtensionDescriptor::setIcon(const QPixmap &pix)
{
	if(m_pIcon)delete m_pIcon;
	if(pix.isNull())m_pIcon = 0;
	else m_pIcon = new QPixmap(pix);
}

KviModuleExtension * KviModuleExtensionDescriptor::allocate(KviWindow * pWnd,KviPointerHashTable<QString,QVariant> * pParams,void * pSpecial)
{
	KviModuleExtensionAllocStruct s;
	s.pDescriptor = this;
	s.pWindow = pWnd;
	s.pParams = pParams;
	s.pSpecial = pSpecial;
	return m_allocRoutine(&s);
}


void KviModuleExtensionDescriptor::registerObject(KviModuleExtension * e)
{
	m_pObjectList->append(e);
}

void KviModuleExtensionDescriptor::unregisterObject(KviModuleExtension * e)
{
	m_pObjectList->removeRef(e);
}


KviModuleExtensionManager::KviModuleExtensionManager()
{
	m_pExtensionDict = new KviPointerHashTable<const char *,KviModuleExtensionDescriptorList>(17,false);
	m_pExtensionDict->setAutoDelete(true);
}

KviModuleExtensionManager::~KviModuleExtensionManager()
{
	delete m_pExtensionDict;
}

KviModuleExtensionDescriptorList * KviModuleExtensionManager::getExtensionList(const KviCString &szType)
{
	g_pModuleManager->loadModulesByCaps(szType.ptr());
	return m_pExtensionDict->find(szType.ptr());
}

KviModuleExtensionDescriptor * KviModuleExtensionManager::registerExtension(KviModule * m,const KviCString &szType,const KviCString &szName,const QString &szVisibleName,KviModuleExtensionAllocRoutine r,const QPixmap &icon)
{
	KviModuleExtensionDescriptor * d = new KviModuleExtensionDescriptor(m,szType,szName,szVisibleName,r,icon);
	KviModuleExtensionDescriptorList * l = m_pExtensionDict->find(szType.ptr());
	if(!l)
	{
		l = new KviModuleExtensionDescriptorList();
		l->setAutoDelete(false);
		m_pExtensionDict->insert(szType.ptr(),l);
	}
	l->append(d);
	return d;
}

void KviModuleExtensionManager::unregisterExtensionsByModule(KviModule * m)
{
	KviPointerHashTableIterator<const char *,KviModuleExtensionDescriptorList> it(*m_pExtensionDict);
	KviPointerList<KviCString> dying;
	dying.setAutoDelete(true);
	while(KviModuleExtensionDescriptorList * l = it.current())
	{
		KviPointerList<KviModuleExtensionDescriptor> dying2;
		dying2.setAutoDelete(true);

		for(KviModuleExtensionDescriptor * d = l->first();d;d = l->next())
		{
			if(d->module() == m)dying2.append(d);
		}

		for(KviModuleExtensionDescriptor * de = dying2.first();de;de = dying2.next())
		{
			l->removeRef(de);
		}

		if(l->isEmpty())dying.append(new KviCString(it.currentKey()));
		++it;
	}
	for(KviCString * li = dying.first();li;li = dying.next())
	{
		m_pExtensionDict->remove(li->ptr());
	}
}

KviModuleExtensionDescriptorList * KviModuleExtensionManager::allocateExtensionGetDescriptorList(const KviCString &szType,const QString &preloadModule)
{
	if(!preloadModule.isEmpty())
	{
		KviModule * m = g_pModuleManager->getModule(preloadModule);
		(void)m; // get rid of the unused warning :D
	}

	KviModuleExtensionDescriptorList * l = m_pExtensionDict->find(szType.ptr());
	if(!l)
	{
		// retry : it might have been unloaded
		g_pModuleManager->loadModulesByCaps(szType.ptr());
		l = m_pExtensionDict->find(szType.ptr());
	}

	return l;
}

KviModuleExtensionDescriptor * KviModuleExtensionManager::findExtensionDescriptor(const KviCString &szType,const KviCString &szName)
{
	KviModuleExtensionDescriptorList * l = m_pExtensionDict->find(szType.ptr());
	if(!l)return 0;

	for(KviModuleExtensionDescriptor * d = l->first();d;d = l->next())
	{
		if(d->name().equalsCI(szName))return d;
	}

	return 0;
}

KviModuleExtension * KviModuleExtensionManager::allocateExtension(const KviCString &szType,const KviCString &szName,KviWindow * pWnd,KviPointerHashTable<QString,QVariant> * pParams,void * pSpecial,const QString &preloadModule)
{
	KviModuleExtensionDescriptorList * l = allocateExtensionGetDescriptorList(szType,preloadModule);
	if(!l)return 0;

	KviModuleExtensionDescriptor * d;

	for(d = l->first();d;d = l->next())
	{
		if(d->name().equalsCI(szName))return d->allocate(pWnd,pParams,pSpecial);
	}

	// uhm... not there ?
	g_pModuleManager->loadModulesByCaps(szType.ptr());
	// try again after loading the modules
	// l = m_pExtensionDict->find(szType.ptr()); <--- this shouldn't change!
	for(d = l->first();d;d = l->next())
	{
		if(d->name().equalsCI(szName))return d->allocate(pWnd,pParams,pSpecial);
	}

	// no way : no such extension

	return 0;
}


KviModuleExtension * KviModuleExtensionManager::allocateExtension(const KviCString &szType,int id,KviWindow * pWnd,KviPointerHashTable<QString,QVariant> * pParams,void * pSpecial,const QString &preloadModule)
{
	KviModuleExtensionDescriptorList * l = allocateExtensionGetDescriptorList(szType,preloadModule);
	if(!l)return 0;

	KviModuleExtensionDescriptor * d;
	for(d = l->first();d;d = l->next())
	{
		if(d->id() == id)return d->allocate(pWnd,pParams,pSpecial);
	}

	// uhm... not there ?
	g_pModuleManager->loadModulesByCaps(szType.ptr());
	// try again after loading the modules
	// l = m_pExtensionDict->find(szType.ptr()); <--- this shouldn't change!
	for(d = l->first();d;d = l->next())
	{
		if(d->id() == id)return d->allocate(pWnd,pParams,pSpecial);
	}
	// no way : no such extension

	return 0;
}


KviModuleExtension::KviModuleExtension(KviModuleExtensionDescriptor * d)
: KviHeapObject()
{
	m_pDescriptor = d;
	m_pDescriptor->registerObject(this);
}

KviModuleExtension::~KviModuleExtension()
{
	m_pDescriptor->unregisterObject(this);
}
