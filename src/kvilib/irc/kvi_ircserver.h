#ifndef _KVI_IRCSERVER_H_
#define _KVI_IRCSERVER_H_

//=============================================================================
//
//   File : kvi_ircserver.h
//   Creation date : Mon Jul 10 2000 03:24:11 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2009 Szymon Stefanek (pragma at kvirc dot net)
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
* \file kvi_ircserver.h
* \author Szymon Stefanek
* \brief Irc server handling
* \def KVI_IRCSERVER_FLAG_IPV6 Defines if the server uses IPv6
* \def KVI_IRCSERVER_FLAG_CACHEIP Defines if the server caches the IP
* \def KVI_IRCSERVER_FLAG_SSL Defines if the server uses SSL
* \def KVI_IRCSERVER_FLAG_STARTTLS Defines if the server can use STARTTLS
* \def KVI_IRCSERVER_FLAG_SASL Defines if the server can use SASL
*/

#include "kvi_settings.h"
#include "kvi_heapobject.h"
#include "kvi_qstring.h"
#include "kvi_pointerlist.h"

#include <QStringList>

class KviConfig;
class KviNickServRuleSet;
class KviProxy;
class KviProxyDataBase;
class KviServer;

#define KVI_IRCSERVER_FLAG_IPV6 1
#define KVI_IRCSERVER_FLAG_CACHEIP 2
#define KVI_IRCSERVER_FLAG_SSL 4
#define KVI_IRCSERVER_FLAG_STARTTLS 8
#define KVI_IRCSERVER_FLAG_SASL 16
#define KVI_IRCSERVER_FLAG_CAP 32

/**
* \class KviServerReconnectInfo
* \brief A class for reconnecting purposes
*/
class KVILIB_API KviServerReconnectInfo
{
public:
	/**
	* \brief Constructs the server reconnect info object
	* \return KviServerReconnectInfo
	*/
	KviServerReconnectInfo();

	/**
	* \brief Carbon copy
	* \param info The object to copy the info from
	* \return KviServerReconnectInfo
	*/
	KviServerReconnectInfo(const KviServerReconnectInfo & info);

	/**
	* \brief Destroys the server reconnect info object
	*/
	~KviServerReconnectInfo();
public:
	QString               m_szNick;
	QString               m_szAwayReason;
	QString               m_szJoinChannels;
	QStringList           m_szOpenQueryes;
	bool                  m_bIsAway;
};

/**
* \class KviServer
* \brief The class which manages the irc servers
*/
class KVILIB_API KviServer : public KviHeapObject
{
public:
	/**
	* \brief Constructs the server object
	* \return KviServer
	*/
	KviServer();

	/**
	* \brief Carbon copy
	* \param serv The irc server
	* \return KviServer
	*/
	KviServer(const KviServer & serv);

	/**
	* \brief Destroys the server object
	*/
	~KviServer();
public:
	KviServerReconnectInfo * m_pReconnectInfo;
	QString            m_szHostname;          // the server hostname (or ip eventually)
	QString            m_szIp;                // the server's cached ip (if we're caching)
	QString            m_szDescription;       // the server description
	kvi_u32_t          m_uPort;               // the server's port
	unsigned short int m_uFlags;              // flags

	// Extended data
	QString            m_szUserIdentityId;    // The user identity to use for this server: if empty
	                                          // Then use the network identity instead

	QString            m_szUser;              // special username
	QString            m_szPass;              // special password
	QString            m_szNick;              // special nickname
	QString            m_szRealName;          // special real name
	QString            m_szInitUMode;         // special user mode
	QString            m_szOnConnectCommand;  // the command to run on connect
	QString            m_szOnLoginCommand;    // the command to run after login
	QString            m_szLinkFilter;        // the link filter object
	QString            m_szEncoding;          // if empty, use network encoding
	QString            m_szTextEncoding;      // if empty, use network encoding
	QStringList      * m_pAutoJoinChannelList;        // Channels to auto join
	bool               m_bAutoConnect;        // autoconnect
	QString            m_szId;                // the server's may-be-unique id, may be auto-generated
	int                m_iProxy;              // proxy server's id
	QString            m_szSaslNick;          // nickname for sasl auth
	QString            m_szSaslPass;          // password for sasl auth

public:
	/**
	* \brief Returns the proxy server's id
	* \return int
	*/
	inline int proxy() { return m_iProxy; };

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
	inline kvi_u32_t port() const { return m_uPort; };

	/**
	* \brief Returns the password of the user associated to the server
	* \return const QString &
	*/
	inline const QString & password() const { return m_szPass; };

	/**
	* \brief Returns the nickname used for sasl auth
	* \return const QString &
	*/
	inline const QString & saslNick() const { return m_szSaslNick; };

	/**
	* \brief Returns the password used for sasl auth
	* \return const QString &
	*/
	inline const QString & saslPass() const { return m_szSaslPass; };

	/**
	* \brief Returns the nickname of the user associated to the server
	* \return const QString &
	*/
	inline const QString & nickName() const { return m_szNick; };

	/**
	* \brief Returns the user modes of the user associated to the server
	* \return const QString &
	*/
	inline const QString & initUMode() const { return m_szInitUMode; };

	/**
	* \brief Returns the hostname of the user associated to the server
	* \return const QString &
	*/
	inline const QString & hostName() const { return m_szHostname; };

	/**
	* \brief Returns the IP address of the server
	* \return const QString &
	*/
	inline const QString & ip() const { return m_szIp; };

	/**
	* \brief Returns the commands to run on server login
	* \return const QString &
	*/
	inline const QString & onLoginCommand() const { return m_szOnLoginCommand; };

	/**
	* \brief Returns the commands to run on server connection
	* \return const QString &
	*/
	inline const QString & onConnectCommand() const { return m_szOnConnectCommand; };

	/**
	* \brief Returns the username of the user associated to the server
	* \return const QString &
	*/
	inline const QString & userName() const { return m_szUser; };

	/**
	* \brief Returns the realname of the user associated to the server
	* \return const QString &
	*/
	inline const QString & realName() const { return m_szRealName; };

	/**
	* \brief
	* \return const QString &
	*/
	inline const QString & linkFilter() const { return m_szLinkFilter; };

	/**
	* \brief Returns the description of the server
	* \return const QString &
	*/
	inline const QString & description() const { return m_szDescription; };

	/**
	* \brief Returns the encoding associated to the server
	* Some information as nickname and channel names are encoded when
	* communicating with the server
	* \return const QString &
	*/
	inline const QString & encoding() const { return m_szEncoding; };

	/**
	* \brief Returns the text encoding associated to the server
	* This is the default encoding when talking on channels or queries
	* \return const QString &
	*/
	inline const QString & textEncoding() const { return m_szTextEncoding; };

	/**
	* \brief Returns the id of the server
	* \return const QString &
	*/
	inline const QString & id() const { return m_szId; };

	/**
	* \brief Returns the id of the user associated to the server
	* \return const QString &
	*/
	inline const QString & userIdentityId() const { return m_szUserIdentityId; };

	/**
	* \brief Returns true if the server is in autoconnect mode
	* \return bool
	*/
	inline bool autoConnect() const { return m_bAutoConnect; };

	/**
	* \brief Returns the list of the channels in the autojoin list
	* \return QStringList *
	*/
	inline QStringList * autoJoinChannelList(){ return m_pAutoJoinChannelList; };

	/**
	* \brief Returns true if the server uses IPv6
	* \return bool
	*/
	inline bool isIPv6() const { return (m_uFlags & KVI_IRCSERVER_FLAG_IPV6); };

	/**
	* \brief Returns true if the server uses SSL
	* \return bool
	*/
	inline bool useSSL() const { return (m_uFlags & KVI_IRCSERVER_FLAG_SSL); };

	/**
	* \brief Returns true if the CAP protocol is enabled for this server
	* \return bool
	*/
	inline bool enabledCAP() const { return (m_uFlags & KVI_IRCSERVER_FLAG_CAP); };

	/**
	* \brief Returns true if the STARTTLS protocol is enabled for this server
	* \return bool
	*/
	inline bool enabledSTARTTLS() const { return (m_uFlags & KVI_IRCSERVER_FLAG_STARTTLS); };

	/**
	* \brief Returns true if the SASL protocol is enabled for this server
	* \return bool
	*/
	inline bool enabledSASL() const { return (m_uFlags & KVI_IRCSERVER_FLAG_SASL); };

	/**
	* \brief Returns true if the server caches the IP
	* \return bool
	*/
	inline bool cacheIp() const { return (m_uFlags & KVI_IRCSERVER_FLAG_CACHEIP); };

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
	inline void setProxy(int iProxy){ m_iProxy = iProxy; };

	/**
	* \brief Sets the IP for the server
	* \param szIp The IP of the server
	* \return void
	*/
	inline void setIp(const QString & szIp){ m_szIp = szIp; };

	/**
	* \brief Sets the port for the server
	* \param uPort The port of the server
	* \return void
	*/
	inline void setPort(kvi_u32_t uPort){ m_uPort = uPort; };

	/**
	* \brief Sets the hostname for the server
	* \param szHost The host name of the user
	* \return void
	*/
	inline void setHostName(const QString & szHost){ m_szHostname = szHost; };

	/**
	* \brief Sets the description for the server
	* \param szDesc The description of the server
	* \return void
	*/
	inline void setDescription(const QString & szDesc){ m_szDescription = szDesc; };

	/**
	* \brief Sets the username of the user associated to the server
	* \param szUser The user name of the user
	* \return void
	*/
	inline void setUserName(const QString & szUser){ m_szUser = szUser; };

	/**
	* \brief Sets the password of the user associated to the server
	* \param szPass The password of the user
	* \return void
	*/
	inline void setPassword(const QString & szPass){ m_szPass = szPass; };

	/**
	* \brief Sets the nickname of the user associated to the server
	* \param szNick The nick name of the user
	* \return void
	*/
	inline void setNickName(const QString & szNick){ m_szNick = szNick; };

	/**
	* \brief Sets the password used for sasl auth
	* \param szPass The password of the user
	* \return void
	*/
	inline void setSaslPass(const QString & szPass){ m_szSaslPass = szPass; };

	/**
	* \brief Sets the nickname used for sasl auth
	* \param szNick The nick name of the user
	* \return void
	*/
	inline void setSaslNick(const QString & szNick){ m_szSaslNick = szNick; };

	/**
	* \brief Sets the realname of the user associated to the server
	* \param szReal The real name of the user
	* \return void
	*/
	inline void setRealName(const QString & szReal){ m_szRealName = szReal; };

	/**
	* \brief Sets the encoding associated to the server
	* Some information as nickname and channel names are encoded when
	* communicating with the server
	* This is the default encoding when talking on channels or queries
	* \param szEncoding The default encoding of the text
	* \return void
	*/
	inline void setEncoding(const QString & szEncoding){ m_szEncoding = szEncoding; };

	/**
	* \brief Sets the encoding associated to the server
	* This is the default encoding when talking on channels or queries
	* \param szEncoding The default encoding of the text
	* \return void
	*/
	inline void setTextEncoding(const QString & szEncoding){ m_szTextEncoding = szEncoding; };

	/**
	* \brief Sets the user modes of the user associated to the server
	* \param szUMode The user modes of the user
	* \return void
	*/
	inline void setInitUMode(const QString & szUMode){ m_szInitUMode = szUMode; };

	/**
	* \brief Sets the commands to run on server connection
	* \param szCmd The comands to run on connection
	* \return void
	*/
	inline void setOnConnectCommand(const QString & szCmd){ m_szOnConnectCommand = szCmd; };

	/**
	* \brief Sets the commands to run on server login
	* \param szCmd The comands to run on login
	* \return void
	*/
	inline void setOnLoginCommand(const QString & szCmd){ m_szOnLoginCommand = szCmd; };

	/**
	* \brief
	* \param szFilter
	* \return void
	*/
	inline void setLinkFilter(const QString & szFilter){ m_szLinkFilter = szFilter; };

	/**
	* \brief Sets the list of channels to autojoin after connection
	* The channel list must be allocated with a new!
	* \param pNewChannelList The list of channels to autojoin
	* \return void
	*/
	void setAutoJoinChannelList(QStringList * pNewChannelList);

	/**
	* \brief Sets the autoconnection mode for the server
	* \param bAutoConnect Whether to set the autoconnection
	* \return void
	*/
	inline void setAutoConnect(bool bAutoConnect) { m_bAutoConnect = bAutoConnect; };

	/**
	* \brief Sets the id of the user associated to the server
	* \param szUserIdentityId The user identity id to set
	* \return void
	*/
	inline void setUserIdentityId(const QString & szUserIdentityId){ m_szUserIdentityId = szUserIdentityId; };

	/**
	* \brief Sets if the server uses IPv6
	* \param bSet Whether to set the support for IPv6
	* \return void
	*/
	inline void setIPv6(bool bSet)
	{
		if(bSet)m_uFlags |= KVI_IRCSERVER_FLAG_IPV6;
		else m_uFlags &= ((unsigned short)~KVI_IRCSERVER_FLAG_IPV6);
	};

	/**
	* \brief Sets if the server uses SSL
	* \param bSet Whether to set the support for SSL
	* \return void
	*/
	inline void setUseSSL(bool bSet)
	{
		if(bSet)m_uFlags |= KVI_IRCSERVER_FLAG_SSL;
		else m_uFlags &= ((unsigned short)~KVI_IRCSERVER_FLAG_SSL);
	};

	/**
	* \brief Sets if STARTTLS support is enabled/disabled for this server
	* \param bSet Whether to enable the support for STARTTLS
	* \return void
	*/
	inline void setEnabledSTARTTLS(bool bSet)
	{
		if(bSet)m_uFlags |= KVI_IRCSERVER_FLAG_STARTTLS;
		else m_uFlags &= ((unsigned short)~KVI_IRCSERVER_FLAG_STARTTLS);
	};

	/**
	* \brief Sets if CAP support is enabled/disabled for this server
	* \param bSet Whether to enable the support for CAP
	* \return void
	*/
	inline void setEnabledCAP(bool bSet)
	{
		if(bSet)m_uFlags |= KVI_IRCSERVER_FLAG_CAP;
		else m_uFlags &= ((unsigned short)~KVI_IRCSERVER_FLAG_CAP);
	};

	/**
	* \brief Sets if SASL support is enabled/disabled for this server
	* \param bSet Whether to enable the support for SASL
	* \return void
	*/
	inline void setEnabledSASL(bool bSet)
	{
		if(bSet)m_uFlags |= KVI_IRCSERVER_FLAG_SASL;
		else m_uFlags &= ((unsigned short)~KVI_IRCSERVER_FLAG_SASL);
	};

	/**
	* \brief Sets if the server caches the IP
	* \param bSet Whether to set the cache for the IP
	* \return void
	*/
	inline void setCacheIp(bool bSet)
	{
		if(bSet) m_uFlags |= KVI_IRCSERVER_FLAG_CACHEIP;
		else m_uFlags &= ((unsigned short)~KVI_IRCSERVER_FLAG_CACHEIP);
	};

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
	inline void setId(const QString & szId){ m_szId = szId; if(m_szId.isEmpty()) generateUniqueId(); };

	/**
	* \brief Loads the information from the configuration file
	* \param cfg The configuration file
	* \param szPrefix The prefix of the server
	* \return bool
	*/
	bool load(KviConfig * cfg, const QString & szPrefix);

	/**
	* \brief Saves the information to the configuration file
	* \param cfg The configuration file
	* \param szPrefix The prefix of the server
	* \return void
	*/
	void save(KviConfig * cfg, const QString & szPrefix);

	/**
	* \brief Carbon copy
	* \param s The server to copy
	* \return void
	*/
	void operator=(const KviServer & s);
};

#endif //_KVI_IRCSERVER_H_
