//=============================================================================
//
//   File : KviCustomToolBarManager.cpp
//   Creation date : Sun 05 Dec 2004 18:20:18 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "KviCustomToolBarManager.h"
#include "KviCustomToolBarDescriptor.h"
#include "KviConfigurationFile.h"
#include "KviLocale.h"

KviCustomToolBarManager * KviCustomToolBarManager::m_pInstance = nullptr;

KviCustomToolBarManager::KviCustomToolBarManager()
{
	m_pDescriptors = new KviPointerHashTable<QString, KviCustomToolBarDescriptor>(17, false);
	m_pDescriptors->setAutoDelete(true);
}

KviCustomToolBarManager::~KviCustomToolBarManager()
{
	delete m_pDescriptors;
}

KviCustomToolBar * KviCustomToolBarManager::firstExistingToolBar()
{
	KviPointerHashTableIterator<QString, KviCustomToolBarDescriptor> it(*m_pDescriptors);
	while(KviCustomToolBarDescriptor * d = it.current())
	{
		if(d->toolBar())
			return d->toolBar();
		++it;
	}
	return nullptr;
}

void KviCustomToolBarManager::init()
{
	if(!m_pInstance)
		m_pInstance = new KviCustomToolBarManager();
}

void KviCustomToolBarManager::done()
{
	if(m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

QString KviCustomToolBarManager::idForNewToolBar(const QString & szTemplate)
{
	QString s;
	QString szTT = szTemplate.toLower();
	szTT.remove(' ');
	szTT.remove("$tr");
	szTT.remove('(');
	szTT.remove(')');
	szTT.remove('"');
	for(int idx = 0;; idx++)
	{
		s = szTT;
		if(idx > 0)
			s += QString::number(idx);
		if(!m_pDescriptors->find(s))
			return s;
	}
	return s;
}

KviCustomToolBarDescriptor * KviCustomToolBarManager::findDescriptorByInternalId(int id)
{
	KviPointerHashTableIterator<QString, KviCustomToolBarDescriptor> it(*m_pDescriptors);
	while(KviCustomToolBarDescriptor * d = it.current())
	{
		if(d->internalId() == id)
			return d;
		++it;
	}
	return nullptr;
}

bool KviCustomToolBarManager::renameDescriptor(const QString & szId, const QString & szNewId, const QString & szNewLabelCode)
{
	KviCustomToolBarDescriptor * d = m_pDescriptors->find(szId);
	if(!d)
		return false;
	d->rename(szNewLabelCode);
	if(szId == szNewId)
		return true; // already done
	m_pDescriptors->setAutoDelete(false);
	m_pDescriptors->remove(szId);
	m_pDescriptors->replace(szNewId, d);
	m_pDescriptors->setAutoDelete(true);
	return true;
}

bool KviCustomToolBarManager::destroyDescriptor(const QString & szId)
{
	KviCustomToolBarDescriptor * d = m_pDescriptors->find(szId);
	if(!d)
		return false;
	m_pDescriptors->remove(szId); // will delete it too!
	return true;
}

void KviCustomToolBarManager::clear()
{
	m_pDescriptors->clear(); // bye!
}

KviCustomToolBarDescriptor * KviCustomToolBarManager::create(const QString & szId, const QString & szLabelCode)
{
	KviCustomToolBarDescriptor * d = m_pDescriptors->find(szId);
	if(d)
		return d;
	d = new KviCustomToolBarDescriptor(szId, szLabelCode);
	m_pDescriptors->replace(szId, d);
	return d;
}

void KviCustomToolBarManager::storeVisibilityState()
{
	KviPointerHashTableIterator<QString, KviCustomToolBarDescriptor> it(*m_pDescriptors);
	while(KviCustomToolBarDescriptor * d = it.current())
	{
		d->m_bVisibleAtStartup = d->toolBar() != nullptr;
		++it;
	}
}

int KviCustomToolBarManager::visibleToolBarCount()
{
	int cnt = 0;
	KviPointerHashTableIterator<QString, KviCustomToolBarDescriptor> it(*m_pDescriptors);
	while(KviCustomToolBarDescriptor * d = it.current())
	{
		if(d->toolBar())
			cnt++;
		++it;
	}
	return cnt;
}

void KviCustomToolBarManager::createToolBarsVisibleAtStartup()
{
	KviPointerHashTableIterator<QString, KviCustomToolBarDescriptor> it(*m_pDescriptors);
	while(KviCustomToolBarDescriptor * d = it.current())
	{
		if(d->m_bVisibleAtStartup && (!d->toolBar()))
			d->createToolBar();
		++it;
	}
}

void KviCustomToolBarManager::updateVisibleToolBars()
{
	KviPointerHashTableIterator<QString, KviCustomToolBarDescriptor> it(*m_pDescriptors);
	while(KviCustomToolBarDescriptor * d = it.current())
	{
		if(d->toolBar())
			d->updateToolBar();
		++it;
	}
}

void KviCustomToolBarManager::load(const QString & szFileName)
{
	KviConfigurationFile cfg(szFileName, KviConfigurationFile::Read);

	KviConfigurationFileIterator it(*(cfg.dict()));
	while(it.current())
	{
		cfg.setGroup(it.currentKey());
		KviCustomToolBarDescriptor * d = new KviCustomToolBarDescriptor(it.currentKey(), QString());
		d->m_bVisibleAtStartup = (cfg.readIntEntry("Visible", 0) > 0);
		if(!d->load(&cfg))
			delete d;
		else
			m_pDescriptors->replace(it.currentKey(), d);
		++it;
	}
}

void KviCustomToolBarManager::save(const QString & szFileName)
{
	KviConfigurationFile cfg(szFileName, KviConfigurationFile::Write);
	KviPointerHashTableIterator<QString, KviCustomToolBarDescriptor> it(*m_pDescriptors);
	while(KviCustomToolBarDescriptor * d = it.current())
	{
		cfg.setGroup(d->id());
		cfg.writeEntry("Visible", d->m_bVisibleAtStartup ? 1 : 0);
		d->save(&cfg);
		++it;
	}
}
