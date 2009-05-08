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
//   Inc. ,51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include "kvi_settings.h"
#include "kvi_qstring.h"
#include "kvi_ircserver.h"

#include "kvi_pointerhashtable.h"

typedef struct _KviIrcServerDefinition
{
	QString            szServer;
	kvi_u32_t	   uPort;
	bool               bPortIsValid;
	bool               bIpV6;
	bool               bSSL;
	QString            szLinkFilter;
	QString            szPass;
	QString            szNick;
	QString            szInitUMode;
	QString            szId;
} KviIrcServerDefinition;



class KVILIB_API KviIrcServerDataBaseRecord
{
	friend class KviIrcServerDataBase;
public:
	KviIrcServerDataBaseRecord(KviIrcNetwork * n);
	~KviIrcServerDataBaseRecord();
protected:
	KviIrcNetwork            * m_pNetwork;
	KviPointerList<KviIrcServer> * m_pServerList;

	KviIrcServer             * m_pCurrentServer;
public:
	KviIrcNetwork * network(){ return m_pNetwork; };
	KviPointerList<KviIrcServer> * serverList(){ return m_pServerList; };
	KviIrcServer * currentServer();
	void insertServer(KviIrcServer *srv);
	KviIrcServer * findServer(const KviIrcServer * pServer);
	void setCurrentServer(KviIrcServer *srv);
};





class KVILIB_API KviIrcServerDataBase
{
public:
	KviIrcServerDataBase();
	~KviIrcServerDataBase();
private:
	KviPointerHashTable<QString,KviIrcServerDataBaseRecord> * m_pRecords;
	QString                             m_szCurrentNetwork;
	// This list is computed when the data are loaded from disk
	// during the startup and is used by KviApp to
	// start the connections.
	// The pointer is zero if there are no autoConnect servers
	// The list is valid only during the startup phase
	// because it contains shallow pointers to the servers
	// really contained in the server/network list
	// and it is never updated later
	KviPointerList<KviIrcServer>                * m_pAutoConnectOnStartupServers;
	KviPointerList<KviIrcServerDataBaseRecord>  * m_pAutoConnectOnStartupNetworks;
public:
	void clear();
	KviPointerHashTable<QString,KviIrcServerDataBaseRecord> * recordDict(){ return m_pRecords; };
	KviPointerList<KviIrcServer> * autoConnectOnStartupServers(){ return m_pAutoConnectOnStartupServers; };
	KviPointerList<KviIrcServerDataBaseRecord> * autoConnectOnStartupNetworks(){ return m_pAutoConnectOnStartupNetworks; };
	void clearAutoConnectOnStartupServers();
	void clearAutoConnectOnStartupNetworks();
	void setCurrentNetwork(const QString &szNetName){ m_szCurrentNetwork = szNetName; };
	const QString & currentNetworkName(){ return m_szCurrentNetwork; };
	KviIrcServerDataBaseRecord * currentRecord();
	KviIrcServerDataBaseRecord * findRecord(const QString &szNetName);
	KviIrcNetwork * findNetwork(const QString &name);
	void loadFromMircIni(const QString & filename, const QString & szMircIni, QStringList& recentServers);
	void load(const QString & filename);
	void save(const QString & filename);
	KviIrcServerDataBaseRecord * insertNetwork(KviIrcNetwork * n);
	void updateServerIp(KviIrcServer * pServer,const QString &ip);
	bool makeCurrentServer(KviIrcServerDefinition * d,QString &szError);
	bool makeCurrentBestServerInNetwork(const QString &szNetName,KviIrcServerDataBaseRecord * d,QString &szError);
};






#endif //_KVI_IRCSERVERDB_H_
