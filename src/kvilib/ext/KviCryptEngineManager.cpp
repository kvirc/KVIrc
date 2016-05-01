//=============================================================================
//
//   File : KviCryptEngineManager.cpp
//   Creation date : Wed Dec 29 2010 22:10:05 CEST by Elvio Basello
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2010 Elvio Basello (hellvis69 at gmail dot com)
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

#include "KviCryptEngineManager.h"
#include "KviCryptEngine.h"
#include "KviCryptEngineDescription.h"

#ifdef COMPILE_CRYPT_SUPPORT

KviCryptEngineManager::KviCryptEngineManager()
{
	m_pEngineDict = new KviPointerHashTable<QString, KviCryptEngineDescription>;
	m_pEngineDict->setAutoDelete(true);
}

KviCryptEngineManager::~KviCryptEngineManager()
{
	delete m_pEngineDict;
}

void KviCryptEngineManager::registerEngine(KviCryptEngineDescription * pDesc)
{
	m_pEngineDict->replace(pDesc->m_szName, pDesc);
}

void KviCryptEngineManager::unregisterEngine(const QString & szName)
{
	m_pEngineDict->remove(szName);
}

void KviCryptEngineManager::unregisterEngines(void * providerHandle)
{
	KviPointerList<QString> lEnginesToRemove;
	lEnginesToRemove.setAutoDelete(true);

	for(KviPointerHashTableEntry<QString, KviCryptEngineDescription> * pDesc = m_pEngineDict->firstEntry(); pDesc; pDesc = m_pEngineDict->nextEntry())
	{
		if(pDesc->data()->m_providerHandle == providerHandle)
			lEnginesToRemove.append(new QString(pDesc->key()));
	}

	for(QString * pszEngineName = lEnginesToRemove.first(); pszEngineName; pszEngineName = lEnginesToRemove.next())
		m_pEngineDict->remove(*pszEngineName);
}

KviCryptEngine * KviCryptEngineManager::allocateEngine(const QString & szName)
{
	KviCryptEngineDescription * pDesc = m_pEngineDict->find(szName);
	if(!pDesc)
		return nullptr;
	KviCryptEngine * pEngine = pDesc->m_allocFunc();
	if(!pEngine)
		return nullptr;
	pEngine->m_deallocFunc = pDesc->m_deallocFunc; // remember the dealloc func from now on
	return pEngine;
}

void KviCryptEngineManager::deallocateEngine(KviCryptEngine * pEngine)
{
	if(!pEngine)
		return;
	crypt_engine_deallocator_func deallocFunc = pEngine->m_deallocFunc;
	deallocFunc(pEngine);
}

#endif // COMPILE_CRYPT_SUPPORT
