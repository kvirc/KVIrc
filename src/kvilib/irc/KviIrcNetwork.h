#ifndef KVI_NETWORK_H_
#define KVI_NETWORK_H_
//=============================================================================
//
//   File : KviIrcNetwork.h
//   Creation date : Wed Aug 27 2008 17:44:56 by Alexey Uzhva
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2008 Alexey Uzhva (wizard at opendoor dot ru)
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
* \file KviIrcNetwork.cpp
* \author Alexey Uzhva
* \brief Network handling
*/

#include "kvi_settings.h"
#include "KviHeapObject.h"
#include "KviPointerList.h"

#include <QString>
#include <QStringList>

class KviNickServRuleSet;
class KviIrcServer;

/**
* \class KviIrcNetwork
* \brief Network handling class
*/
class KVILIB_API KviIrcNetwork : public KviHeapObject
{
	friend class KviIrcServerDataBase;

public:
	/**
	* \brief Construct the network object
	* \param name The name of the network
	* \return KviIrcNetwork
	*/
	KviIrcNetwork(const QString & name);

	/**
	* \brief Carbon copy
	* \param src The source network
	* \return KviIrcNetwork
	*/
	KviIrcNetwork(const KviIrcNetwork & src);

	/**
	* \brief Destroys the network object
	*/
	~KviIrcNetwork();

protected:
	QString m_szName;
	QString m_szDescription;
	QString m_szEncoding;                    /**< if empty, use system default */
	QString m_szTextEncoding;                /**< if empty, use system default */
	QString m_szNickName;                    /**< preferred nick name */
	QString m_szAlternativeNickName;         /**< alternative nick name */
	QString m_szUserName;                    /**< preferred user name */
	QString m_szRealName;                    /**< preferred real name */
	QString m_szPass;                        /**< special password */
	QString m_szOnConnectCommand;            /**< the command to run on connect */
	QString m_szOnLoginCommand;              /**< the command to run after login */
	QStringList * m_pChannelList;            /**< Channels to auto join */
	KviNickServRuleSet * m_pNickServRuleSet; /**< set of nick serv rules */
	bool m_bAutoConnect;                     /**< autoconnect */
	QString m_szUserIdentityId;              /**< The user identity to use for this server: if empty then use the global primary identity moved from KviIrcServerDataBaseRecord */
	KviPointerList<KviIrcServer> * m_pServerList;
	KviIrcServer * m_pCurrentServer = nullptr;

public:
	/**
	* \brief Returns the name of the network
	* \return const QString &
	*/
	const QString & name() const { return m_szName; }

	/**
	* \brief Returns the encoding of the network
	*
	* Some information as nickname and channel names are encoded when
	* communicating with the server
	* \return const QString &
	*/
	const QString & encoding() const { return m_szEncoding; }

	/**
	* \brief Returns the text encoding of the network
	*
	* This is the default encoding when talking on channels or queries
	* \return const QString &
	*/
	const QString & textEncoding() const { return m_szTextEncoding; }

	/**
	* \brief Returns the description of the network
	* \return const QString &
	*/
	const QString & description() const { return m_szDescription; }

	/**
	* \brief Returns the nickname of the user associated to the network
	* \return const QString &
	*/
	const QString & nickName() const { return m_szNickName; }

	/**
	* \brief Returns the alternative nickname of the user associated to the network
	* \return const QString &
	*/
	const QString & alternativeNickName() const { return m_szAlternativeNickName; }

	/**
	* \brief Returns the realname of the user associated to the network
	* \return const QString &
	*/
	const QString & realName() const { return m_szRealName; }

	/**
	* \brief Returns the username of the user associated to the network
	* \return const QString &
	*/
	const QString & userName() const { return m_szUserName; }

	/**
	* \brief Returns the password of the user associated to the network
	* \return const QString &
	*/
	const QString & password() const { return m_szPass; }

	/**
	* \brief Returns the commands to run on network login
	* \return const QString &
	*/
	const QString & onLoginCommand() const { return m_szOnLoginCommand; }

	/**
	* \brief Returns the commands to run on network connect
	* \return const QString &
	*/
	const QString & onConnectCommand() const { return m_szOnConnectCommand; }

	/**
	* \brief Returns the user identity of the user associated to the network
	* \return const QString &
	*/
	const QString & userIdentityId() const { return m_szUserIdentityId; }

	/**
	* \brief Returns true if the network has the autoconnect state on
	* \return bool
	*/
	bool autoConnect() const { return m_bAutoConnect; }

	/**
	* \brief Returns the list of channels with autojoin flag
	* \return QStringList *
	*/
	QStringList * autoJoinChannelList() { return m_pChannelList; }

	/**
	* \brief Returns the list of channels with autojoin flag as a string
	* \return const QString &
	*/
	const QString autoJoinChannelListAsString() { return m_pChannelList ? m_pChannelList->join(",") : ""; }

	/**
	* \brief Returns a set of rules for the NickServ
	* \return KviNickServRuleSet *
	*/
	KviNickServRuleSet * nickServRuleSet() { return m_pNickServRuleSet; }

	/**
	* \brief Sets the rules for NickServ
	* \param pSet The rule set where to add rules
	* \return void
	*/
	void setNickServRuleSet(KviNickServRuleSet * pSet);

	/**
	* \brief Carbon copy
	* \param net The source network to copy from
	* \return void
	*/
	void copyFrom(const KviIrcNetwork & net);

	/**
	* \brief Sets the name of the network
	* \param szName The name of the network
	* \return void
	*/
	void setName(const QString & szName) { m_szName = szName; }

	/**
	* \brief Sets the encondig of the network
	*
	* Some information as nickname and channel names are encoded when
	* communicating with the server
	* \param szEncoding The encoding of the network
	* \return void
	*/
	void setEncoding(const QString & szEncoding) { m_szEncoding = szEncoding; }

	/**
	* \brief Sets the text encondig of the network
	*
	* This is the default encoding when talking on channels or queries
	* \param szEncoding The text encoding of the network
	* \return void
	*/
	void setTextEncoding(const QString & szEncoding) { m_szTextEncoding = szEncoding; }

	/**
	* \brief Sets the description of the network
	* \param szDescription The description of the network
	* \return void
	*/
	void setDescription(const QString & szDescription) { m_szDescription = szDescription; }

	/**
	* \brief Sets the list of commands to run on network connection
	* \param szCmd The commands list to run
	* \return void
	*/
	void setOnConnectCommand(const QString & szCmd) { m_szOnConnectCommand = szCmd; }

	/**
	* \brief Sets the list of commands to run on network login
	* \param szCmd The commands list to run
	* \return void
	*/
	void setOnLoginCommand(const QString & szCmd) { m_szOnLoginCommand = szCmd; }

	/**
	* \brief Sets the nickname of the user associated to the network
	* \param szNick The nickname
	* \return void
	*/
	void setNickName(const QString & szNick) { m_szNickName = szNick; }

	/**
	* \brief Sets the alternative nickname of the user associated to the network
	* \param szNick The nickname
	* \return void
	*/
	void setAlternativeNickName(const QString & szNick) { m_szAlternativeNickName = szNick; }

	/**
	* \brief Sets the realname of the user associated to the network
	* \param szReal The realname
	* \return void
	*/
	void setRealName(const QString & szReal) { m_szRealName = szReal; }

	/**
	* \brief Sets the username of the user associated to the network
	* \param szUser The username
	* \return void
	*/
	void setUserName(const QString & szUser) { m_szUserName = szUser; }

	/**
	* \brief Sets the password of the user associated to the network
	* \param szPass The password
	* \return void
	*/
	void setPassword(const QString & szPass) { m_szPass = szPass; }

	/**
	* \brief Sets the list of channels to mark for autojoin
	* \param pNewChannelList The channel list
	* \return void
	*/
	void setAutoJoinChannelList(QStringList * pNewChannelList);

	/**
	* \brief Sets the list of channels to mark for autojoin
	* \param szNewChannelList A comma separated list of channels
	* \return void
	*/
	void setAutoJoinChannelList(const QString & szNewChannelList);

	/**
	* \brief Sets the autoconnect flag
	* \param bAutoConnect The state of the autoconnect flag
	* \return void
	*/
	void setAutoConnect(bool bAutoConnect) { m_bAutoConnect = bAutoConnect; }

	/**
	* \brief Sets the user identity id of the user associated to the network
	* \param szUserIdentityId The user identity
	* \return void
	*/
	void setUserIdentityId(const QString & szUserIdentityId) { m_szUserIdentityId = szUserIdentityId; }

	/**
	* \brief Returns a list of servers associated to the network
	* \return KviPointerList<KviIrcServer> *
	*/
	KviPointerList<KviIrcServer> * serverList() const { return m_pServerList; }

	/**
	* \brief Returns the current server
	* \return KviIrcServer
	*/
	KviIrcServer * currentServer();

	/**
	* \brief Adds a new server to the network
	* \param pServer The source server to add
	* \return void
	*/
	void insertServer(KviIrcServer * pServer);

	/**
	* \brief Searches for a server in the network
	* \param szHostname The hostname of the server to find
	* \return KviIrcServer
	*/
	KviIrcServer * findServer(const QString & szHostname);

	/**
	* \brief Searches for a server in the network
	* \param pServer The server to find
	* \return KviIrcServer
	*/
	KviIrcServer * findServer(const KviIrcServer * pServer);

	/**
	* \brief Sets the current server
	* \param pServer The source server
	* \return void
	*/
	void setCurrentServer(KviIrcServer * pServer);
};

#endif // KVI_NETWORK_H_
