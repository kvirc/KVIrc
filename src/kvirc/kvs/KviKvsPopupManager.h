#ifndef _KVI_KVS_POPUPMANAGER_H_
#define _KVI_KVS_POPUPMANAGER_H_
//=============================================================================
//
//   File : KviKvsPopupManager.h
//   Creation date : Tue 7 Jan 2004 02:11:41 by Szymon Stefanek
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

#include "kvi_settings.h"
#include "KviKvsPopupMenu.h"

class KVIRC_API KviKvsPopupManager : public QObject
{
	Q_OBJECT
protected: // can only be created by init/done
	KviKvsPopupManager();
	~KviKvsPopupManager();

protected:
	KviPointerHashTable<QString, KviKvsPopupMenu> * m_pPopupDict;
	static KviKvsPopupManager * m_pInstance;

public:
	static KviKvsPopupManager * instance() { return m_pInstance; };
	static void init();
	static void done();
	int popupCount() { return m_pPopupDict->count(); };

	KviPointerHashTable<QString, KviKvsPopupMenu> * popupDict() { return m_pPopupDict; };

	KviKvsPopupMenu * lookup(const QString & szPopupName) { return m_pPopupDict->find(szPopupName); };
	KviKvsPopupMenu * get(const QString & szPopupName);
	void add(const QString & szPopupName, KviKvsPopupMenu * pPopup);
	void remove(const QString & szPopupName) { m_pPopupDict->remove(szPopupName); };
	void clear() { m_pPopupDict->clear(); };

	void save(const QString & filename);
	void load(const QString & filename);
	void emitRefresh(const QString & szPopupName);
signals:
	void popupRefresh(const QString & szPopupName);
};

#endif //!_KVI_KVS_POPUPMANAGER_H_
