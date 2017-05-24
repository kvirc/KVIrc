#ifndef _KVI_KVS_ALIASMANAGER_H_
#define _KVI_KVS_ALIASMANAGER_H_
//=============================================================================
//
//   File : KviKvsAliasManager.h
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

#include "kvi_settings.h"
#include "KviPointerHashTable.h"

#include "KviQString.h"

#include "KviKvsScript.h"

#include <vector>

class KVIRC_API KviKvsAliasManager : public QObject
{
	Q_OBJECT
protected: // it only can be created and destroyed by KviKvsAliasManager::init()/done()
	KviKvsAliasManager();
	~KviKvsAliasManager();

protected:
	KviPointerHashTable<QString, KviKvsScript> * m_pAliasDict;
	static KviKvsAliasManager * m_pAliasManager;

public:
	static KviKvsAliasManager * instance()
	{
		return m_pAliasManager;
	};
	static void init(); // called by KviKvs::init()
	static void done(); // called by KviKvs::done()

	KviPointerHashTable<QString, KviKvsScript> * aliasDict() { return m_pAliasDict; };
	const KviKvsScript * lookup(const QString & szName)
	{
		return m_pAliasDict->find(szName);
	};
	void add(const QString & szName, KviKvsScript * pAlias);
	bool remove(const QString & szName)
	{
		return m_pAliasDict->remove(szName);
	};
	bool removeNamespace(const QString & szName);
	void clear()
	{
		m_pAliasDict->clear();
	};

	void save(const QString & filename);
	void load(const QString & filename);

	void completeCommand(const QString & word, std::vector<QString> & matches);
signals:
	void aliasRefresh(const QString &);
};
// namespaces are handled completely in the editing!
#endif //!_KVI_KVS_ALIASMANAGER_H_
