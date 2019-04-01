#ifndef _KVI_IRCSERVER_H_
#define _KVI_IRCSERVER_H_
//=============================================================================
//
//   File : KviIrcServer.h
//   Creation date : Mon Jul 10 2000 03:24:11 by Szymon Stefanek
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
* \file KviIrcServer.h
* \author Szymon Stefanek
* \brief Irc server handling
*/

#include "kvi_settings.h"
#include "kvi_inttypes.h"
#include "KviHeapObject.h"
#include "KviPointerList.h"

#include <QString>
#include <QStringList>

class KviConfigurationFile;
class KviNickServRuleSet;
class KviProxy;
class KviProxyDataBase;
class KviIrcServerReconnectInfo;

/**
* \class KviIrcServer
* \brief The class which manages the irc servers
*/
class KVILIB_API KviIrcServer : public KviHeapObject
{
public:
	/**
	* \enum Flags
	* \brief Contains the features supported by the server
	*/
	enum Flags
	{
		IPv6 = 1,     /**< IPv6 support */
		CacheIP = 2,  /**< whether we cache the server's IP */
		SSL = 4,      /**< SSL support */
		STARTTLS = 8, /**< STARTTLS support */
		SASL = 16,    /**< SASL support */
		CAP = 32,     /**< CAP support */
		FAVORITE = 64 /**< Favorite Server */
	};

	/**
	* \brief Constructs the server object
	* \return KviIrcServer
	*/
	KviIrcServer();

	/**
	* \brief Carbon copy
	* \param serv The irc server
	* \return KviIrcServer
	*/
	KviIrcServer(const KviIrcServer & serv);

	/**
	* \brief Destroys the server object
	*/
	~KviIrcServer();

private:
	KviIrcServerReconnectInfo * m_pReconnectInfo;
	QString m_szHostname;        /**< the server hostname (or IP eventually) */
	QString m_szIp;              /**< the server's cached ip (if we're caching) */
	QString m_szDescription;     /**< the server description */
	kvi_u32_t m_uPort;           /**< the server's port */
	unsigned short int m_uFlags; /**< flags */

	// Extended data
	QString m_szUserIdentityId;           /**< the user identity to use for this server: if empty, then use the network identity instead */
	QString m_szUser;                     /**< special username */
	QString m_szPass;                     /**< special password */
	QString m_szNick;                     /**< special nickname */
	QString m_szAlternativeNick;          /**< alternate special nickname */
	QString m_szRealName;                 /**< special real name */
	QString m_szInitUMode;                /**< special user mode */
	QString m_szOnConnectCommand;         /**< the command to run on connect */
	QString m_szOnLoginCommand;           /**< the command to run after login */
	QString m_szLinkFilter;               /**< the link filter object */
	QString m_szEncoding;                 /**< if empty, use network encoding */
	QString m_szTextEncoding;             /**< if empty, use network encoding */
	QStringList * m_pAutoJoinChannelList; /**< Channels to auto join */
	bool m_bAutoConnect;                  /**< autoconnect */
	QString m_szId;                       /**< the server's may-be-unique id, may be auto-generated */
	int m_iProxy;                         /**< proxy server's id */
	QString m_szSaslNick;                 /**< nickname for sasl auth */
	QString m_szSaslPass;                 /**< password for sasl auth */
	QString m_szSaslMethod;               /**< method name for sasl auth */

public:
	KviIrcServerReconnectInfo * reconnectInfo()
	{
		return m_pReconnectInfo;
	}

	void clearReconnectInfo();

	void setReconnectInfo(KviIrcServerReconnectInfo * pInfo);

	/**
	* \brief Returns the proxy server's id
	* \return int
	*/
	int proxy() const { return m_iProxy; }

	/**
	* \brief Returns the proxy server
	* \param pDb The proxy database
	* \return KviProxy
	*/
	KviProxy * proxyServer(KviProxyDataBase * pDb);

	/**
	* \brief Returns the port number
	* \return kvi_u32_t
	*/
	kvi_u32_t port() const { return m_uPort; }

	/**
	* \brief Returns the password of the user associated to the server
	* \return const QString &
	*/
	const QString & password() const { return m_szPass; }

	/**
	* \brief Returns the nickname used for sasl auth
	* \return const QString &
	*/
	const QString & saslNick() const { return m_szSaslNick; }

	/**
	* \brief Returns the password used for sasl auth
	* \return const QString &
	*/
	const QString & saslPass() const { return m_szSaslPass; }

	/**
	* \brief Returns the sasl authentication method to be used
	* \return const QString &
	*/
	const QString & saslMethod() const { return m_szSaslMethod; }

	/**
	* \brief Returns the nickname of the user associated to the server
	* \return const QString &
	*/
	const QString & nickName() const { return m_szNick; }

	/**
	* \brief Returns the alternative nickname of the user associated to the server
	* \return const QString &
	*/
	const QString & alternativeNickName() const { return m_szAlternativeNick; }

	/**
	* \brief Returns the user modes of the user associated to the server
	* \return const QString &
	*/
	const QString & initUMode() const { return m_szInitUMode; }

	/**
	* \brief Returns the hostname of the user associated to the server
	* \return const QString &
	*/
	const QString & hostName() const { return m_szHostname; }

	/**
	* \brief Returns the IP address of the server
	* \return const QString &
	*/
	const QString & ip() const { return m_szIp; }

	/**
	* \brief Returns the commands to run on server login
	* \return const QString &
	*/
	const QString & onLoginCommand() const { return m_szOnLoginCommand; }

	/**
	* \brief Returns the commands to run on server connection
	* \return const QString &
	*/
	const QString & onConnectCommand() const { return m_szOnConnectCommand; }

	/**
	* \brief Returns the username of the user associated to the server
	* \return const QString &
	*/
	const QString & userName() const { return m_szUser; }

	/**
	* \brief Returns the realname of the user associated to the server
	* \return const QString &
	*/
	const QString & realName() const { return m_szRealName; }

	/**
	* \brief Returns the filter applied on the server
	* \return const QString &
	*/
	const QString & linkFilter() const { return m_szLinkFilter; }

	/**
	* \brief Returns the description of the server
	* \return const QString &
	*/
	const QString & description() const { return m_szDescription; }

	/**
	* \brief Returns the encoding associated to the server
	* Some information as nickname and channel names are encoded when
	* communicating with the server
	* \return const QString &
	*/
	const QString & encoding() const { return m_szEncoding; }

	/**
	* \brief Returns the text encoding associated to the server
	* This is the default encoding when talking on channels or queries
	* \return const QString &
	*/
	const QString & textEncoding() const { return m_szTextEncoding; }

	/**
	* \brief Returns the id of the server
	* \return const QString &
	*/
	const QString & id() const { return m_szId; }

	/**
	* \brief Returns the id of the user associated to the server
	* \return const QString &
	*/
	const QString & userIdentityId() const { return m_szUserIdentityId; }

	/**
	* \brief Returns true if the server is in autoconnect mode
	* \return bool
	*/
	bool autoConnect() const { return m_bAutoConnect; }

	/**
	* \brief Returns the list of the channels in the autojoin list
	* \return QStringList *
	*/
	QStringList * autoJoinChannelList() { return m_pAutoJoinChannelList; }

	/**
	* \brief Returns the list of the channels in the autojoin list as a string
	* \return const QString &
	*/
	const QString autoJoinChannelListAsString() { return m_pAutoJoinChannelList ? m_pAutoJoinChannelList->join(",") : ""; }

	/**
	* \brief Returns true if the server uses IPv6
	* \return bool
	*/
	bool isIPv6() const { return (m_uFlags & KviIrcServer::IPv6); }

	/**
	* \brief Returns true if the server uses SSL
	* \return bool
	*/
	bool useSSL() const { return (m_uFlags & KviIrcServer::SSL); }

	/**
	* \brief Returns true if the CAP protocol is enabled for this server
	* \return bool
	*/
	bool enabledCAP() const { return (m_uFlags & KviIrcServer::CAP); }

	/**
	* \brief Returns true if the STARTTLS protocol is enabled for this server
	* \return bool
	*/
	bool enabledSTARTTLS() const { return (m_uFlags & KviIrcServer::STARTTLS); }

	/**
	* \brief Returns true if the SASL protocol is enabled for this server
	* \return bool
	*/
	bool enabledSASL() const { return (m_uFlags & KviIrcServer::SASL); }

	/**
	* \brief Returns true if the server caches the IP
	* \return bool
	*/
	bool cacheIp() const { return (m_uFlags & KviIrcServer::CacheIP); }

	/**
	* \brief Returns the irc URI for the server
	* The URI is in the form irc[[s]6]://name.serverhost.tld
	* \return QString
	*/
	QString ircUri();

	/**
	* \brief Sets the proxy server for the server
	* \param iProxy The proxy to connect through
	* \return void
	*/
	void setProxy(int iProxy) { m_iProxy = iProxy; }

	/**
	* \brief Sets the IP for the server
	* \param szIp The IP of the server
	* \return void
	*/
	void setIp(const QString & szIp) { m_szIp = szIp; }

	/**
	* \brief Sets the port for the server
	* \param uPort The port of the server
	* \return void
	*/
	void setPort(kvi_u32_t uPort) { m_uPort = uPort; }

	/**
	* \brief Sets the hostname for the server
	* \param szHost The host name of the user
	* \return void
	*/
	void setHostName(const QString & szHost) { m_szHostname = szHost; }

	/**
	* \brief Sets the description for the server
	* \param szDesc The description of the server
	* \return void
	*/
	void setDescription(const QString & szDesc) { m_szDescription = szDesc; }

	/**
	* \brief Sets the username of the user associated to the server
	* \param szUser The user name of the user
	* \return void
	*/
	void setUserName(const QString & szUser) { m_szUser = szUser; }

	/**
	* \brief Sets the password of the user associated to the server
	* \param szPass The password of the user
	* \return void
	*/
	void setPassword(const QString & szPass) { m_szPass = szPass; }

	/**
	* \brief Sets the nickname of the user associated to the server
	* \param szNick The nick name of the user
	* \return void
	*/
	void setNickName(const QString & szNick) { m_szNick = szNick; }

	/**
	* \brief Sets the alternative nickname of the user associated to the server
	* \param szNick The nick name of the user
	* \return void
	*/
	void setAlternativeNickName(const QString & szNick) { m_szAlternativeNick = szNick; }

	/**
	* \brief Sets the password used for sasl auth
	* \param szPass The password of the user
	* \return void
	*/
	void setSaslPass(const QString & szPass) { m_szSaslPass = szPass; }

	/**
	* \brief Sets the nickname used for sasl auth
	* \param szNick The nick name of the user
	* \return void
	*/
	void setSaslNick(const QString & szNick) { m_szSaslNick = szNick; }

	/**
	* \brief Sets the sasl method to be used for auth
	* \param szMethod The method name
	* \return void
	*/
	void setSaslMethod(const QString & szMethod) { m_szSaslMethod = szMethod; }

	/**
	* \brief Sets the realname of the user associated to the server
	* \param szReal The real name of the user
	* \return void
	*/
	void setRealName(const QString & szReal) { m_szRealName = szReal; }

	/**
	* \brief Sets the encoding associated to the server
	* Some information as nickname and channel names are encoded when
	* communicating with the server
	* This is the default encoding when talking on channels or queries
	* \param szEncoding The default encoding of the text
	* \return void
	*/
	void setEncoding(const QString & szEncoding) { m_szEncoding = szEncoding; }

	/**
	* \brief Sets the encoding associated to the server
	* This is the default encoding when talking on channels or queries
	* \param szEncoding The default encoding of the text
	* \return void
	*/
	void setTextEncoding(const QString & szEncoding) { m_szTextEncoding = szEncoding; }

	/**
	* \brief Sets the user modes of the user associated to the server
	* \param szUMode The user modes of the user
	* \return void
	*/
	void setInitUMode(const QString & szUMode) { m_szInitUMode = szUMode; }

	/**
	* \brief Sets the commands to run on server connection
	* \param szCmd The comands to run on connection
	* \return void
	*/
	void setOnConnectCommand(const QString & szCmd) { m_szOnConnectCommand = szCmd; }

	/**
	* \brief Sets the commands to run on server login
	* \param szCmd The comands to run on login
	* \return void
	*/
	void setOnLoginCommand(const QString & szCmd) { m_szOnLoginCommand = szCmd; }

	/**
	* \brief Applies the filter to the server
	*
	* A link filter resides between the low-level socket, which send/receive blocks
	* of bytes, and the connection which talk using IRC messages.
	* The filter can remap blocks of bytes in any direction
	* \param szFilter
	* \return void
	*/
	void setLinkFilter(const QString & szFilter) { m_szLinkFilter = szFilter; }

	/**
	* \brief Sets the list of channels to autojoin after connection
	* The channel list must be allocated with a new!
	* \param pNewChannelList The list of channels to autojoin
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
	* \brief Sets the autoconnection mode for the server
	* \param bAutoConnect Whether to set the autoconnection
	* \return void
	*/
	void setAutoConnect(bool bAutoConnect) { m_bAutoConnect = bAutoConnect; }

	/**
	* \brief Sets the id of the user associated to the server
	* \param szUserIdentityId The user identity id to set
	* \return void
	*/
	void setUserIdentityId(const QString & szUserIdentityId) { m_szUserIdentityId = szUserIdentityId; }

	/**
	* \brief Sets if the server uses IPv6
	* \param bSet Whether to set the support for IPv6
	* \return void
	*/
	void setIPv6(bool bSet)
	{
		if(bSet)
			m_uFlags |= KviIrcServer::IPv6;
		else
			m_uFlags &= static_cast<unsigned short>(~KviIrcServer::IPv6);
	}

	/**
	* \brief Sets if the server uses SSL
	* \param bSet Whether to set the support for SSL
	* \return void
	*/
	void setUseSSL(bool bSet)
	{
		if(bSet)
			m_uFlags |= KviIrcServer::SSL;
		else
			m_uFlags &= static_cast<unsigned short>(~KviIrcServer::SSL);
	}

	/**
	* \brief Sets if STARTTLS support is enabled/disabled for this server
	* \param bSet Whether to enable the support for STARTTLS
	* \return void
	*/
	void setEnabledSTARTTLS(bool bSet)
	{
		if(bSet)
			m_uFlags |= KviIrcServer::STARTTLS;
		else
			m_uFlags &= static_cast<unsigned short>(~KviIrcServer::STARTTLS);
	}

	/**
	* \brief Sets if CAP support is enabled/disabled for this server
	* \param bSet Whether to enable the support for CAP
	* \return void
	*/
	void setEnabledCAP(bool bSet)
	{
		if(bSet)
			m_uFlags |= KviIrcServer::CAP;
		else
			m_uFlags &= static_cast<unsigned short>(~KviIrcServer::CAP);
	}

	/**
	* \brief Sets if SASL support is enabled/disabled for this server
	* \param bSet Whether to enable the support for SASL
	* \return void
	*/
	void setEnabledSASL(bool bSet)
	{
		if(bSet)
			m_uFlags |= KviIrcServer::SASL;
		else
			m_uFlags &= static_cast<unsigned short>(~KviIrcServer::SASL);
	}

	/**
	* \brief Sets if the server caches the IP
	* \param bSet Whether to set the cache for the IP
	* \return void
	*/
	void setCacheIp(bool bSet)
	{
		if(bSet)
			m_uFlags |= KviIrcServer::CacheIP;
		else
			m_uFlags &= static_cast<unsigned short>(~KviIrcServer::CacheIP);
	}

	/**
	* \brief Generates an unique id for the server and sets it
	* \return void
	*/
	void generateUniqueId();

	/**
	* \brief Sets an unique id for the server
	* \param szId The id of the server
	* \return void
	*/
	void setId(const QString & szId)
	{
		m_szId = szId;
		if(m_szId.isEmpty())
			generateUniqueId();
	}

	/**
	* \brief Loads the information from the configuration file
	* \param pCfg The configuration file
	* \param szPrefix The prefix of the server
	* \return bool
	*/
	bool load(KviConfigurationFile * pCfg, const QString & szPrefix);

	/**
	* \brief Saves the information to the configuration file
	* \param pCfg The configuration file
	* \param szPrefix The prefix of the server
	* \return void
	*/
	void save(KviConfigurationFile * pCfg, const QString & szPrefix);

	/**
	* \brief Carbon copy
	* \param serv The server to copy
	* \return void
	*/
	void operator=(const KviIrcServer & serv);

	/**
	* \brief Sets the server to a favorite
	* \param bSet whether the server is a favorite or not
	* \return void
	*/
	void setFavorite(bool bSet)
	{
		if(bSet)
			m_uFlags |= KviIrcServer::FAVORITE;
		else
			m_uFlags &= static_cast<unsigned short>(~KviIrcServer::FAVORITE);
	}

	/**
	* \brief Returns if the server is a favorite
	* \return bool
	*/
	bool favorite() const { return (m_uFlags & KviIrcServer::FAVORITE); }
};

#endif //_KVI_IRCSERVER_H_
