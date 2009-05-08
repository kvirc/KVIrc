//=============================================================================
//
//   File : kvi_kvs_aliasmanager.cpp
//   Creation date : Mon 15 Dec 2003 02:11:41 by Szymon Stefanek
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



#include "kvi_kvs_aliasmanager.h"
#include "kvi_config.h"

KviKvsAliasManager * KviKvsAliasManager::m_pAliasManager = 0;

KviKvsAliasManager::KviKvsAliasManager()
{
	m_pAliasManager = this;
	m_pAliasDict = new KviPointerHashTable<QString,KviKvsScript>(51,false);
	m_pAliasDict->setAutoDelete(true);
}

KviKvsAliasManager::~KviKvsAliasManager()
{
	delete m_pAliasDict;
}

void KviKvsAliasManager::init()
{
	if(KviKvsAliasManager::instance())
	{
		debug("WARNING: Trying to create the KviKvsAliasManager twice!");
		return;
	}
	(void)new KviKvsAliasManager();
}

void KviKvsAliasManager::done()
{
	if(!KviKvsAliasManager::instance())
	{
		debug("WARNING: Trying to destroy the KviKvsAliasManager twice!");
		return;
	}
	delete KviKvsAliasManager::instance();
}

void KviKvsAliasManager::completeCommand(const QString &word,KviPointerList<QString> * matches)
{
	KviPointerHashTableIterator<QString,KviKvsScript> it(*m_pAliasDict);
	while(it.current())
	{
		if(KviQString::equalCIN(word,it.current()->name(),word.length()))
			matches->append(new QString(it.current()->name()));
		++it;
	}
}

// FIXME: #warning "A binary config would work better and faster here!"

void KviKvsAliasManager::save(const QString & filename)
{
	KviConfig cfg(filename,KviConfig::Write);
	cfg.clear();

	KviPointerHashTableIterator<QString,KviKvsScript> it(*m_pAliasDict);

	while(it.current())
	{
		cfg.setGroup(it.current()->name());
		cfg.writeEntry("_Buffer",it.current()->code());
		++it;
	}
}

void KviKvsAliasManager::load(const QString & filename)
{
	m_pAliasDict->clear();
	KviConfig cfg(filename,KviConfig::Read);

	KviConfigIterator it(*(cfg.dict()));

	KviPointerList<QString> l;
	l.setAutoDelete(true);

	while(it.current())
	{
		l.append(new QString(it.currentKey()));
		++it;
	}

	for(QString * s = l.first();s;s = l.next())
	{
		cfg.setGroup(*s);
		QString szCode = cfg.readQStringEntry("_Buffer","");
		if(!szCode.isEmpty())
		{
			KviKvsScript * m = new KviKvsScript(*s,szCode);
			m_pAliasDict->insert(*s,m);
		}
		++it;
	}
}
