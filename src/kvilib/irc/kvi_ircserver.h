#ifndef _KVI_IRCSERVER_H_
#define _KVI_IRCSERVER_H_

//=============================================================================
//
//   File : kvi_ircserver.h
//   Creation date : Mon Jul 10 2000 03:24:11 by Szymon Stefanek
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

/**
* \file kvi_ircserver.h
* \author Szymon Stefanek
* \brief Irc server handling
* \def KVI_IRCSERVER_FLAG_IPV6 Defines if the server uses IPv6
* \def KVI_IRCSERVER_FLAG_CACHEIP Defines if the server caches the IP
* \def KVI_IRCSERVER_FLAG_SSL Defines if the server uses SSL
* \def KVI_IRCSERVER_FLAG_STARTTLS Defines if the server uses STARTTLS
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

/**
* \class KviServerReconnectInfo
* \brief A class for reconnecting purposes
*/
class KVILIB_API KviServerReconnectInfo {
public:
	KviServerReconnectInfo();
	KviServerReconnectInfo(const KviServerReconnectInfo &info);
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
public:
	/**
	* \brief Returns the proxy server's id
	* \return int
	*/
	inline int proxy() { return m_iProxy; };

	/**
	* \brief Returns the proxy server
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
	* Some informations as nickname and channel names are encoded when
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
	* \brief Returns true if the server supports the STARTTLS protocol
	* \return bool
	*/
	inline bool supportsSTARTTLS() const { return (m_uFlags & KVI_IRCSERVER_FLAG_STARTTLS); };

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
	* \return void
	*/
	inline void setProxy(int p){ m_iProxy = p; };

	/**
	* \brief Sets the IP for the server
	* \return void
	*/
	inline void setIp(const QString & a){ m_szIp = a; };

	/**
	* \brief Sets the port for the server
	* \return void
	*/
	inline void setPort(kvi_u32_t p){ m_uPort = p; };

	/**
	* \brief Sets the hostname for the server
	* \return void
	*/
	inline void setHostName(const QString & n){ m_szHostname = n; };

	/**
	* \brief Sets the description for the server
	* \return void
	*/
	inline void setDescription(const QString & d){ m_szDescription = d; };

	/**
	* \brief Sets the username of the user associated to the server
	* \return void
	*/
	inline void setUserName(const QString & u){ m_szUser = u; };

	/**
	* \brief Sets the password of the user associated to the server
	* \return void
	*/
	inline void setPassword(const QString & p){ m_szPass = p; };

	/**
	* \brief Sets the nickname of the user associated to the server
	* \return void
	*/
	inline void setNickName(const QString & n){ m_szNick = n; };

	/**
	* \brief Sets the realname of the user associated to the server
	* \return void
	*/
	inline void setRealName(const QString & r){ m_szRealName = r; };

	/**
	* \brief Sets the encoding associated to the server
	* Some informations as nickname and channel names are encoded when
	* communicating with the server
	* This is the default encoding when talking on channels or queries
	* \return void
	*/
	inline void setEncoding(const QString & e){ m_szEncoding = e; };

	/**
	* \brief Sets the encoding associated to the server
	* This is the default encoding when talking on channels or queries
	* \return void
	*/
	inline void setTextEncoding(const QString & e){ m_szTextEncoding = e; };

	/**
	* \brief Sets the user modes of the user associated to the server
	* \return void
	*/
	inline void setInitUMode(const QString & u){ m_szInitUMode = u; };

	/**
	* \brief Sets the commands to run on server connection
	* \return void
	*/
	inline void setOnConnectCommand(const QString & cmd){ m_szOnConnectCommand = cmd; };

	/**
	* \brief Sets the commands to run on server login
	* \return void
	*/
	inline void setOnLoginCommand(const QString & cmd){ m_szOnLoginCommand = cmd; };

	/**
	* \brief
	* \return void
	*/
	inline void setLinkFilter(const QString &f){ m_szLinkFilter = f; };

	/**
	* \brief Sets the list of channels to autojoin after connection
	* The channel list must be allocated with a new!
	* \return void
	*/
	void setAutoJoinChannelList(QStringList * pNewChannelList);

	/**
	* \brief Sets the autoconnection mode for the server
	* \return void
	*/
	inline void setAutoConnect(bool autoconnect) { m_bAutoConnect = autoconnect; };

	/**
	* \brief Sets the id of the user associated to the server
	* \return void
	*/
	inline void setUserIdentityId(const QString &szUserIdentityId){ m_szUserIdentityId = szUserIdentityId; };

	/**
	* \brief Sets if the server uses IPv6
	* \return void
	*/
	inline void setIPv6(bool bSet)
	{
		if(bSet)m_uFlags |= KVI_IRCSERVER_FLAG_IPV6;
		else m_uFlags &= ((unsigned short)~KVI_IRCSERVER_FLAG_IPV6);
	};

	/**
	* \brief Sets if the server uses SSL
	* \return void
	*/
	inline void setUseSSL(bool bSet)
	{
		if(bSet)m_uFlags |= KVI_IRCSERVER_FLAG_SSL;
		else m_uFlags &= ((unsigned short)~KVI_IRCSERVER_FLAG_SSL);
	};

	/**
	* \brief Sets if the server supports the STARTTLS protocol
	* \return void
	*/
	inline void setSupportsSTARTTLS(bool bSet)
	{
		if(bSet)m_uFlags |= KVI_IRCSERVER_FLAG_STARTTLS;
		else m_uFlags &= ((unsigned short)~KVI_IRCSERVER_FLAG_STARTTLS);
	};

	/**
	* \brief Sets if the server caches the IP
	* \return void
	*/
	inline void setCacheIp(bool bSet)
	{
		if(bSet)m_uFlags |= KVI_IRCSERVER_FLAG_CACHEIP;
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
	inline void setId(const QString & szId){ m_szId = szId; if(m_szId.isEmpty())generateUniqueId(); };

	/**
	* \brief Loads the informations from the configuration file
	* \param cfg The configuration file
	* \param prefix The prefix of the server
	* \return bool
	*/
	bool load(KviConfig * cfg, const QString & prefix);

	/**
	* \brief Saves the informations to the configuration file
	* \param cfg The configuration file
	* \param prefix The prefix of the server
	* \return void
	*/
	void save(KviConfig * cfg, const QString & prefix);

	/**
	* \brief Carbon copy
	* \param s The server to copy
	* \return void
	*/
	void operator =(const KviServer & s );
};

#endif //_KVI_IRCSERVER_H_
