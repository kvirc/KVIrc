#ifndef _KVI_IRCSERVERDB_H_
#define _KVI_IRCSERVERDB_H_
//=============================================================================
//
//   File : kvi_ircserverdb.h
//   Creation date : Mon Jul 10 2000 14:15:42 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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

/**
* \file kvi_ircserverdb.h
* \author Szymon Stefanek
* \brief Irc server database handling
*/

#include "kvi_settings.h"
#include "kvi_qstring.h"
#include "kvi_ircserver.h"
#include "kvi_network.h"
#include "kvi_pointerhashtable.h"

/**
* \typedef KviServerDefinition
* \struct _KviServerDefinition
* \brief Server definition
*/
typedef struct _KviServerDefinition
{
	QString            szServer;
	kvi_u32_t          uPort;
	bool               bPortIsValid;
	bool               bIPv6;
	bool               bSSL;
	bool               bSTARTTLS;
	QString            szLinkFilter;
	QString            szPass;
	QString            szNick;
	QString            szInitUMode;
	QString            szId;
} KviServerDefinition;

/**
* \class KviServerDataBase
* \brief Irc server database handling class
*/
class KVILIB_API KviServerDataBase
{
public:
	/**
	* \brief Constructs the server database object
	* \return KviServerDataBase
	*/
	KviServerDataBase();

	/**
	* \brief Destroys the server database object
	*/
	~KviServerDataBase();
private:
	KviPointerHashTable<QString,KviNetwork> * m_pRecords;
	QString                                   m_szCurrentNetwork;
	KviPointerList<KviServer>               * m_pAutoConnectOnStartupServers;
	KviPointerList<KviNetwork>              * m_pAutoConnectOnStartupNetworks;
public:
	/**
	* \brief Deletes the database
	* \return void
	*/
	void clear();

	/**
	* \brief Returns the record dictionary of the database
	* \return KviPointerHashTable<QString,KviNetwork> *
	*/
	inline KviPointerHashTable<QString,KviNetwork> * recordDict(){ return m_pRecords; };

	/**
	* \brief Returns a list of servers to connect on startup
	* This list is computed when the data are loaded from disk during the startup
	* and is used by KviApp to start the connections.
	* The pointer is zero if there are no autoConnect servers. The list is valid
	* only during the startup phase because it contains shallow pointers to the
	* servers really contained in the server/network list and it is never updated
	* later.
	* \return KviPointerList<KviServer> *
	*/
	inline KviPointerList<KviServer>  * autoConnectOnStartupServers(){ return m_pAutoConnectOnStartupServers; };

	/**
	* \brief Returns a list of networks to connect on startup
	* This list is computed when the data are loaded from disk during the startup
	* and is used by KviApp to start the connections.
	* The pointer is zero if there are no autoConnect networks. The list is valid
	* only during the startup phase because it contains shallow pointers to the
	* networks really contained in the server/network list and it is never
	* updated later.
	* \return KviPointerList<KviNetwork> *
	*/
	inline KviPointerList<KviNetwork> * autoConnectOnStartupNetworks(){ return m_pAutoConnectOnStartupNetworks; };

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
	inline void setCurrentNetwork(const QString & szNetName){ m_szCurrentNetwork = szNetName; };

	/**
	* \brief Returns the current network name
	* \return const QString &
	*/
	inline const QString & currentNetworkName(){ return m_szCurrentNetwork; };

	/**
	* \brief Returns the current network
	* \return KviNetwork
	*/
	KviNetwork * currentNetwork();

	/**
	* \brief Adds a network to the database
	* \param n The source network
	* \return void
	*/
	void addNetwork(KviNetwork * n);

	/**
	* \brief Searches for a network
	* \param name The name of the network to find
	* \return KviNetwork
	*/
	KviNetwork * findNetwork(const QString & name);

	/**
	* \brief Returns the number of networks
	* \return unsigned integer
	*/
	unsigned int networkCount() const;

	/**
	* \brief Loads the database data
	* \param filename The filename of the database data to load
	* \return void
	*/
	void load(const QString & filename);

	/**
	* \brief Saves the database data
	* \param filename The filename of the database data to save
	* \return void
	*/
	void save(const QString & filename);

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
	* \param d The server definition
	* \param szError The container for a possible error
	* \return bool
	*/
	bool makeCurrentServer(KviServerDefinition * d, QString  & szError);

	/**
	* \brief Marks the current servers as the best in the network
	* \param szNetName The name of the network
	* \param d The source network
	* \param szError The container for a possible error
	* \return bool
	*/
	bool makeCurrentBestServerInNetwork(const QString & szNetName, KviNetwork * d, QString & szError);
};

#endif //_KVI_IRCSERVERDB_H_
