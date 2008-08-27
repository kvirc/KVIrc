#ifndef _KVI_IRCSERVERDB_H_
#define _KVI_IRCSERVERDB_H_
//=============================================================================
//
//   File : kvi_ircserverdb.h
//   Creation date : Mon Jul 10 2000 14:15:42 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2004 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#include "kvi_settings.h"
#include "kvi_qstring.h"
#include "kvi_ircserver.h"
#include "kvi_network.h"
#include "kvi_pointerhashtable.h"

typedef struct _KviServerDefinition
{
	QString            szServer;
	kvi_u32_t          uPort;
	bool               bPortIsValid;
	bool               bIPv6;
	bool               bSSL;
	QString            szLinkFilter;
	QString            szPass;
	QString            szNick;
	QString            szInitUMode;
	QString            szId;
} KviServerDefinition;


class KVILIB_API KviServerDataBase
{
public:
	KviServerDataBase();
	~KviServerDataBase();
private:
	KviPointerHashTable<QString,KviNetwork> * m_pRecords;
	QString                             m_szCurrentNetwork;
	// This list is computed when the data are loaded from disk
	// during the startup and is used by KviApp to
	// start the connections.
	// The pointer is zero if there are no autoConnect servers
	// The list is valid only during the startup phase
	// because it contains shallow pointers to the servers
	// really contained in the server/network list
	// and it is never updated later
	KviPointerList<KviServer>   * m_pAutoConnectOnStartupServers;
	KviPointerList<KviNetwork>  * m_pAutoConnectOnStartupNetworks;
public:
	void clear();
	inline KviPointerHashTable<QString,KviNetwork> * recordDict(){ return m_pRecords; };

	inline KviPointerList<KviServer>  * autoConnectOnStartupServers(){ return m_pAutoConnectOnStartupServers; };
	inline KviPointerList<KviNetwork> * autoConnectOnStartupNetworks(){ return m_pAutoConnectOnStartupNetworks; };

	void clearAutoConnectOnStartupServers();
	void clearAutoConnectOnStartupNetworks();

	inline void setCurrentNetwork(const QString &szNetName){ m_szCurrentNetwork = szNetName; };
	inline const QString & currentNetworkName(){ return m_szCurrentNetwork; };
	KviNetwork * currentNetwork();

	void addNetwork(KviNetwork * n);
	KviNetwork * findNetwork(const QString &name);

	void load(const QString & filename);
	void save(const QString & filename);
	void importFromMircIni(const QString & filename, const QString & szMircIni, QStringList& recentServers);

	bool makeCurrentServer(KviServerDefinition * d,QString &szError);
	bool makeCurrentBestServerInNetwork(const QString &szNetName,KviNetwork * d,QString &szError);
};

#endif //_KVI_IRCSERVERDB_H_
