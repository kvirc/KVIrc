#ifndef _KVI_IRCSERVERDB_H_
#define _KVI_IRCSERVERDB_H_
//=============================================================================
//
//   File : KviIrcServerDataBase.h
//   Creation date : Mon Jul 10 2000 14:15:42 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

/**
* \file KviIrcServerDataBase.h
* \author Szymon Stefanek
* \brief Irc server database handling
*/

#include "kvi_settings.h"
#include "kvi_inttypes.h"
#include "KviPointerHashTable.h"

#include <QString>

class KviIrcNetwork;
class KviIrcServer;

/**
* \typedef KviIrcServerDefinition
* \struct _KviIrcServerDefinition
* \brief Server definition
*/
struct KviIrcServerDefinition
{
	QString szServer;
	kvi_u32_t uPort;
	bool bPortIsValid;
	bool bIPv6;
	bool bSSL;
	bool bSTARTTLS;
	QString szLinkFilter;
	QString szPass;
	QString szNick;
	QString szInitUMode;
	QString szId;
};

/**
* \class KviIrcServerDataBase
* \brief Irc server database handling class
*/
class KVILIB_API KviIrcServerDataBase
{
public:
	/**
	* \brief Constructs the server database object
	* \return KviIrcServerDataBase
	*/
	KviIrcServerDataBase();

	/**
	* \brief Destroys the server database object
	*/
	~KviIrcServerDataBase();

private:
	KviPointerHashTable<QString, KviIrcNetwork> * m_pRecords;
	QString m_szCurrentNetwork;
	KviPointerList<KviIrcServer> * m_pAutoConnectOnStartupServers;
	KviPointerList<KviIrcNetwork> * m_pAutoConnectOnStartupNetworks;

public:
	/**
	* \brief Deletes the database
	* \return void
	*/
	void clear();

	/**
	* \brief Returns the record dictionary of the database
	* \return KviPointerHashTable<QString,KviIrcNetwork> *
	*/
	KviPointerHashTable<QString, KviIrcNetwork> * recordDict() const { return m_pRecords; }

	/**
	* \brief Returns a list of servers to connect on startup
	* This list is computed when the data are loaded from disk during the startup
	* and is used by KviApplication to start the connections.
	* The pointer is zero if there are no autoConnect servers. The list is valid
	* only during the startup phase because it contains shallow pointers to the
	* servers really contained in the server/network list and it is never updated
	* later.
	* \return KviPointerList<KviIrcServer> *
	*/
	KviPointerList<KviIrcServer> * autoConnectOnStartupServers() const { return m_pAutoConnectOnStartupServers; }

	/**
	* \brief Returns a list of networks to connect on startup
	* This list is computed when the data are loaded from disk during the startup
	* and is used by KviApplication to start the connections.
	* The pointer is zero if there are no autoConnect networks. The list is valid
	* only during the startup phase because it contains shallow pointers to the
	* networks really contained in the server/network list and it is never
	* updated later.
	* \return KviPointerList<KviIrcNetwork> *
	*/
	KviPointerList<KviIrcNetwork> * autoConnectOnStartupNetworks() const { return m_pAutoConnectOnStartupNetworks; }

	/**
	* \brief Deletes the list of autoconnect servers
	* \return void
	*/
	void clearAutoConnectOnStartupServers();

	/**
	* \brief Deletes the list of autoconnect networks
	* \return void
	*/
	void clearAutoConnectOnStartupNetworks();

	/**
	* \brief Sets the current network
	* \param szNetName The name of the network
	* \return void
	*/
	void setCurrentNetwork(const QString & szNetName) { m_szCurrentNetwork = szNetName; }

	/**
	* \brief Returns the current network name
	* \return const QString &
	*/
	const QString & currentNetworkName() const { return m_szCurrentNetwork; }

	/**
	* \brief Returns the current network
	* \return KviIrcNetwork
	*/
	KviIrcNetwork * currentNetwork();

	/**
	* \brief Adds a network to the database
	* \param pNet The source network
	* \return void
	*/
	void addNetwork(KviIrcNetwork * pNet);

	/**
	* \brief Searches for a network
	* \param szName The name of the network to find
	* \return KviIrcNetwork
	*/
	KviIrcNetwork * findNetwork(const QString & szName);

	/**
	* \brief Returns the number of networks
	* \return unsigned integer
	*/
	unsigned int networkCount() const;

	/**
	* \brief Loads the database data
	* \param szFilename The filename of the database data to load
	* \return void
	*/
	void load(const QString & szFilename);

	/**
	* \brief Saves the database data
	* \param szFilename The filename of the database data to save
	* \return void
	*/
	void save(const QString & szFilename);

	/**
	* \brief Import servers and networks from a mirc ini file
	* \param filename The database file where to add new servers
	* \param szMircIni The source mirc ini file to import
	* \param recentServers The list of recent servers where to add new servers
	* \return void
	*/
	void importFromMircIni(const QString & filename, const QString & szMircIni, QStringList & recentServers);

	/**
	* \brief Marks a server as current
	* \param pDef The server definition
	* \param szError The container for a possible error
	* \return bool
	*/
	bool makeCurrentServer(KviIrcServerDefinition * pDef, QString & szError);

	/**
	* \brief Marks the current servers as the best in the network
	* \param szNetName The name of the network
	* \param pNet The source network
	* \param szError The container for a possible error
	* \return bool
	*/
	bool makeCurrentBestServerInNetwork(const QString & szNetName, KviIrcNetwork * pNet, QString & szError);
};

#endif //_KVI_IRCSERVERDB_H_
