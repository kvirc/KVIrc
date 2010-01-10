#ifndef KVI_NETWORK_H_
#define KVI_NETWORK_H_
//=============================================================================
//
//   File : kvi_network.cpp
//   Creation date : Wed Aug 27 2008 17:44:56 by Alexey Uzhva
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2008 Alexey Uzhva (wizard at opendoor dot ru)
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
* \file kvi_network.cpp
* \author Alexey Uzhva
* \brief Network handling
*/

#include "kvi_settings.h"
#include "kvi_heapobject.h"
#include "kvi_pointerlist.h"

#include <QString>

class KviNickServRuleSet;
class KviServer;
class QStringList;

/**
* \class KviNetwork
* \brief Network handling class
*/
class KVILIB_API KviNetwork : public KviHeapObject
{
	friend class KviServerDataBase;
public:
	/**
	* \brief Construct the network object
	* \param name The name of the network
	* \return KviNetwork
	*/
	KviNetwork(const QString & name);

	/**
	* \brief Carbon copy
	* \param src The source network
	* \return KviNetwork
	*/
	KviNetwork(const KviNetwork & src);

	/**
	* \brief Destroys the network object
	*/
	~KviNetwork();
protected:
	QString              m_szName;
	QString              m_szDescription;
	QString              m_szEncoding;                // if empty, use system default
	QString              m_szTextEncoding;            // if empty, use system default
	QString              m_szNickName;                // preferred nick name
	QString              m_szUserName;                // preferred user name
	QString              m_szRealName;                // preferred real name
	QString              m_szPass;                    // special password
	QString              m_szOnConnectCommand;        // the command to run on connect
	QString              m_szOnLoginCommand;          // the command to run after login
	QStringList        * m_pChannelList;              // Channels to auto join
	KviNickServRuleSet * m_pNickServRuleSet;          // set of nick serv rules
	bool                 m_bAutoConnect;              // autoconnect
	QString              m_szUserIdentityId;          // The user identity to use for this server: if empty
	                                                  // Then use the global primary identity
	//moved from KviServerDataBaseRecord
	KviPointerList<KviServer> * m_pServerList;
	KviServer                 * m_pCurrentServer;
public:
	/**
	* \brief Returns the name of the network
	* \return const QString &
	*/
	inline const QString & name() const { return m_szName; };

	/**
	* \brief Returns the encoding of the network
	*
	* Some information as nickname and channel names are encoded when
	* communicating with the server
	* \return const QString &
	*/
	inline const QString & encoding() const { return m_szEncoding; };

	/**
	* \brief Returns the text encoding of the network
	*
	* This is the default encoding when talking on channels or queries
	* \return const QString &
	*/
	inline const QString & textEncoding() const { return m_szTextEncoding; };

	/**
	* \brief Returns the description of the network
	* \return const QString &
	*/
	inline const QString & description() const { return m_szDescription; };

	/**
	* \brief Returns the nickname of the user associated to the network
	* \return const QString &
	*/
	inline const QString & nickName() const { return m_szNickName; };

	/**
	* \brief Returns the realname of the user associated to the network
	* \return const QString &
	*/
	inline const QString & realName() const { return m_szRealName; };

	/**
	* \brief Returns the username of the user associated to the network
	* \return const QString &
	*/
	inline const QString & userName() const { return m_szUserName; };

	/**
	* \brief Returns the password of the user associated to the network
	* \return const QString &
	*/
	inline const QString & password() const { return m_szPass; };

	/**
	* \brief Returns the commands to run on network login
	* \return const QString &
	*/
	inline const QString & onLoginCommand() const { return m_szOnLoginCommand; };

	/**
	* \brief Returns the commands to run on network connect
	* \return const QString &
	*/
	inline const QString & onConnectCommand() const { return m_szOnConnectCommand; };

	/**
	* \brief Returns the user identity of the user associated to the network
	* \return const QString &
	*/
	inline const QString & userIdentityId() const { return m_szUserIdentityId; };

	/**
	* \brief Returns true if the network has the autoconnect state on
	* \return bool
	*/
	inline bool autoConnect() const { return m_bAutoConnect; };

	/**
	* \brief Returns a list of channels with autojoin flag
	* \return QStringList *
	*/
	inline QStringList * autoJoinChannelList(){ return m_pChannelList; };

	/**
	* \brief Returns a set of rules for the NickServ
	* \return KviNickServRuleSet *
	*/
	inline KviNickServRuleSet * nickServRuleSet(){ return m_pNickServRuleSet; };
	
	/**
	* \brief Sets the rules for NickServ
	* \param s The rule set where to add rules
	* \return void
	*/
	void setNickServRuleSet(KviNickServRuleSet * s);

	/**
	* \brief Carbon copy
	* \param d The source network to copy from
	* \return void
	*/
	void copyFrom(const KviNetwork & d);

	/**
	* \brief Sets the name of the network
	* \param szName The name of the network
	* \return void
	*/
	inline void setName(const QString & szName){ m_szName = szName; };

	/**
	* \brief Sets the encondig of the network
	*
	* Some information as nickname and channel names are encoded when
	* communicating with the server
	* \param szEncoding The encoding of the network
	* \return void
	*/
	inline void setEncoding(const QString & szEncoding){ m_szEncoding = szEncoding; };

	/**
	* \brief Sets the text encondig of the network
	*
	* This is the default encoding when talking on channels or queries
	* \param szEncoding The text encoding of the network
	* \return void
	*/
	inline void setTextEncoding(const QString & szEncoding){ m_szTextEncoding = szEncoding; };

	/**
	* \brief Sets the description of the network
	* \param szDescription The description of the network
	* \return void
	*/
	inline void setDescription(const QString & szDescription){ m_szDescription = szDescription; };

	/**
	* \brief Sets the list of commands to run on network connection
	* \param cmd The commands list to run
	* \return void
	*/
	inline void setOnConnectCommand(const QString & cmd){ m_szOnConnectCommand = cmd; };

	/**
	* \brief Sets the list of commands to run on network login
	* \param cmd The commands list to run
	* \return void
	*/
	inline void setOnLoginCommand(const QString & cmd){ m_szOnLoginCommand = cmd; };

	/**
	* \brief Sets the nickname of the user associated to the network
	* \param n The nickname
	* \return void
	*/
	inline void setNickName(const QString & n){ m_szNickName = n; };

	/**
	* \brief Sets the realname of the user associated to the network
	* \param r The realname
	* \return void
	*/
	inline void setRealName(const QString & r){ m_szRealName = r; };

	/**
	* \brief Sets the username of the user associated to the network
	* \param u The username
	* \return void
	*/
	inline void setUserName(const QString & u){ m_szUserName = u; };

	/**
	* \brief Sets the password of the user associated to the network
	* \return void
	*/
	inline void setPassword(const QString & p){ m_szPass = p; };
	
	/**
	* \brief Sets the list of channels to mark for autojoin
	* \param pNewChannelList The channel list
	* \return void
	*/
	void setAutoJoinChannelList(QStringList * pNewChannelList);
	
	/**
	* \brief Sets the autoconnect flag
	* \param bAutoConnect The state of the autoconnect flag
	* \return void
	*/
	inline void setAutoConnect(bool bAutoConnect){ m_bAutoConnect = bAutoConnect; };

	/**
	* \brief Sets the user identity id of the user associated to the network
	* \param szUserIdentityId The user identity
	* \return void
	*/
	inline void setUserIdentityId(const QString & szUserIdentityId){ m_szUserIdentityId = szUserIdentityId; };

	/**
	* \brief Returns a list of servers associated to the network
	* \return KviPointerList<KviServer> *
	*/
	inline KviPointerList<KviServer> * serverList(){ return m_pServerList; };

	/**
	* \brief Returns the current server
	* \return KviServer
	*/
	KviServer * currentServer();

	/**
	* \brief Adds a new server to the network
	* \param srv The source server to add
	* \return void
	*/
	void        insertServer(KviServer * srv);

	/**
	* \brief Searches for a server in the network
	* \param szHostname The hostname of the server to find
	* \return KviServer
	*/
	KviServer * findServer(const QString & szHostname);

	/**
	* \brief Searches for a server in the network
	* \param pServer The server to find
	* \return KviServer
	*/
	KviServer * findServer(const KviServer * pServer);

	/**
	* \brief Sets the current server
	* \param srv The source server
	* \return void
	*/
	void setCurrentServer(KviServer * srv);
};

#endif // KVI_NETWORK_H_
