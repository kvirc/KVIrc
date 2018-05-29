//=============================================================================
//
//   File : KviKvsAliasManager.cpp
//   Creation date : Mon 15 Dec 2003 02:11:41 by Szymon Stefanek
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

#include "KviKvsAliasManager.h"
#include "KviConfigurationFile.h"

KviKvsAliasManager * KviKvsAliasManager::m_pAliasManager = nullptr;

KviKvsAliasManager::KviKvsAliasManager()
{
	m_pAliasManager = this;
	m_pAliasDict = new KviPointerHashTable<QString, KviKvsScript>(51, false);
	m_pAliasDict->setAutoDelete(true);
}

KviKvsAliasManager::~KviKvsAliasManager()
{
	delete m_pAliasDict;
	m_pAliasManager = nullptr;
}

void KviKvsAliasManager::init()
{
	if(KviKvsAliasManager::instance())
	{
		qDebug("WARNING: trying to create the KviKvsAliasManager twice!");
		return;
	}
	(void)new KviKvsAliasManager();
}

void KviKvsAliasManager::done()
{
	if(!KviKvsAliasManager::instance())
	{
		qDebug("WARNING: trying to destroy the KviKvsAliasManager twice!");
		return;
	}
	delete KviKvsAliasManager::instance();
}

bool KviKvsAliasManager::removeNamespace(const QString & szName)
{
	KviPointerHashTableIterator<QString, KviKvsScript> it(*m_pAliasDict);
	QList<QString> lKill;
	while(it.current())
	{
		if(it.current()->name().startsWith(szName, Qt::CaseInsensitive))
			lKill.append(it.current()->name());
		it.moveNext();
	}

	if(lKill.isEmpty())
		return false;

	for(auto & szKill : lKill)
		remove(szKill);

	return true;
}

void KviKvsAliasManager::add(const QString & szName, KviKvsScript * pAlias)
{
	// This piece of code, when inlined by gcc (i.e, placed in a header),
	// shows an interesting problem.

	// KviKvsPointerHashTable<>::replace() is a template and is likely to be inlined too.
	// The internals of replace() use another template called KviKvsPointerHashTableEntry<>.
	// When this function is called from a dynamically loaded shared library
	// the internal template is likely to be instantiated in the shared library itself.
	// Now if the shared library is unloaded, a dangling vtable pointer is stored
	// in the inner template and it will die once we attempt to clear the m_pAliasDict ht.
	//
	// This happens for KviAliasEditor, for instance. In it's commit() function
	// it called this method and caused the vtable pointer to be 0x7fffdfab0030.
	//
	// (gdb) info symbol 0x7fffdfab0030
	// vtable for KviPointerList<KviPointerHashTableEntry<QString, KviKvsScript> > + 16 in section .data.rel.ro
	//    of /usr/local/lib/kvirc/4.0/modules/libkvialiaseditor.so <-- inside the module
	//
	// while the correct vtable (the one that appears when the add() function is called from somewhere else) is
	//
	//(gdb) info symbol 0x6bc210
	// vtable for KviPointerList<KviPointerHashTableEntry<QString, KviKvsScript> > + 16 in section .rodata
	//    of /usr/local/bin/kvirc <-- inside the executable
	//
	// This is a linkage problem. Either caused by gcc itself, or maybe wrong flags
	// passed by the cmake generated Makefiles. Gcc should notice that the symbol is already defined
	// inside the executable and use that one instead.
	//
	// So finally, we can't inline this.

	// The bad news is that this problem may pop up also in other pieces of code...
	m_pAliasDict->replace(szName, pAlias);
	emit aliasRefresh(szName);
}

void KviKvsAliasManager::completeCommand(const QString & word, std::vector<QString> & matches)
{
	KviPointerHashTableIterator<QString, KviKvsScript> it(*m_pAliasDict);
	while(it.current())
	{
		if(KviQString::equalCIN(word, it.current()->name(), word.length()))
			matches.push_back(it.current()->name());
		++it;
	}
}

// FIXME: #warning "A binary config would work better and faster here!"

void KviKvsAliasManager::save(const QString & filename)
{
	KviConfigurationFile cfg(filename, KviConfigurationFile::Write);
	cfg.clear();

	KviPointerHashTableIterator<QString, KviKvsScript> it(*m_pAliasDict);

	while(it.current())
	{
		cfg.setGroup(it.current()->name());
		cfg.writeEntry("_Buffer", it.current()->code());
		++it;
	}
}

void KviKvsAliasManager::load(const QString & filename)
{
	m_pAliasDict->clear();
	KviConfigurationFile cfg(filename, KviConfigurationFile::Read);

	KviConfigurationFileIterator it(*(cfg.dict()));

	KviPointerList<QString> l;
	l.setAutoDelete(true);

	while(it.current())
	{
		l.append(new QString(it.currentKey()));
		++it;
	}

	for(QString * s = l.first(); s; s = l.next())
	{
		cfg.setGroup(*s);
		QString szCode = cfg.readEntry("_Buffer", "");
		if(!szCode.isEmpty())
		{
			KviKvsScript * m = new KviKvsScript(*s, szCode);
			m_pAliasDict->insert(*s, m);
		}
		++it;
	}
}
