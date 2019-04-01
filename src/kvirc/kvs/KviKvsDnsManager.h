#ifndef _KVI_KVS_DNSMANAGER_H_
#define _KVI_KVS_DNSMANAGER_H_
//=============================================================================
//
//   File : KviKvsDnsManager.h
//   Creation date : Wed 04 Aug 2004 04:38:31 by Szymon Stefanek
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
#include "KviPointerHashTable.h"

#include <QObject>
#include <QString>

class KviDnsResolver;
class KviWindow;
class KviKvsScript;
class KviKvsVariantList;

class KviKvsDnsObject
{
public:
	KviKvsDnsObject(KviDnsResolver * pDns,
	    KviWindow * pWnd,
	    const QString & szQuery,
	    bool bRebindOnWindowClose = true,
	    KviKvsScript * pCallback = nullptr,
	    KviKvsVariantList * pParameterList = nullptr);
	~KviKvsDnsObject();

protected:
	KviDnsResolver * m_pDns;
	KviWindow * m_pWindow;
	QString m_szQuery;
	KviKvsScript * m_pCallback;           // callback to be executed when the dns terminates
	KviKvsVariantList * m_pParameterList; // parameter list (magic)
	bool m_bRebindOnWindowClose;          // should we trigger the callback even if m_pWindow no longer exists ?
public:
	KviDnsResolver * dns() { return m_pDns; };
	KviWindow * window() { return m_pWindow; };
	KviKvsScript * callback() { return m_pCallback; };
	KviKvsVariantList * parameterList() { return m_pParameterList; };
	const QString & query() const { return m_szQuery; };
	bool rebindOnWindowClose() const { return m_bRebindOnWindowClose; };
	void setWindow(KviWindow * pWnd) { m_pWindow = pWnd; };
	void setParameterList(KviKvsVariantList * pList) { m_pParameterList = pList; };
};

class KviKvsDnsManager : public QObject
{
	Q_OBJECT
public:
	KviKvsDnsManager();
	~KviKvsDnsManager();

protected:
	static KviKvsDnsManager * m_pInstance;
	KviPointerHashTable<void *, KviKvsDnsObject> * m_pDnsObjects;

public:
	static KviKvsDnsManager * instance() { return m_pInstance; };
	static void init();
	static void done();

	void addDns(KviKvsDnsObject * pObject);
protected slots:
	void dnsLookupTerminated(KviDnsResolver * pDns);

private:
	void abortAll();
};

#endif //!_KVI_KVS_DNSMANAGER_H_
