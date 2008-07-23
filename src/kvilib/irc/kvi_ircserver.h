#ifndef _KVI_IRCSERVER_H_
#define _KVI_IRCSERVER_H_

//=============================================================================
//
//   File : kvi_ircserver.h
//   Creation date : Mon Jul 10 2000 03:24:11 by Szymon Stefanek
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

class KVILIB_API KviServer : public KviHeapObject
{
public:
	KviServer();
	KviServer(const KviServer &serv);
	~KviServer();
public:
	KviServerReconnectInfo *m_pReconnectInfo;
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
	QStringList      * m_pChannelList;        // Channels to auto join
	bool               m_bAutoConnect;        // autoconnect
	QString            m_szId;                // the server's may-be-unique id, may be auto-generated
	int                m_iProxy;              // proxy server's id
public:
	int proxy() { return m_iProxy; };
	KviProxy* proxyServer(KviProxyDataBase * pDb);
	
	kvi_u32_t port() const { return m_uPort; };
	const QString & password() const { return m_szPass; };
	const QString & nickName() const { return m_szNick; };
	const QString & initUMode() const { return m_szInitUMode; };
	const QString & hostName() const { return m_szHostname; };
	const QString & ip() const { return m_szIp; };
	const QString & onLoginCommand() const { return m_szOnLoginCommand; };
	const QString & onConnectCommand() const { return m_szOnConnectCommand; };
	const QString & userName() const { return m_szUser; };
	const QString & realName() const { return m_szRealName; };
	const QString & linkFilter() const { return m_szLinkFilter; };
	const QString & description() const { return m_szDescription; };
	const QString & encoding() const { return m_szEncoding; };
	const QString & textEncoding() const { return m_szTextEncoding; };
	const QString & id() const { return m_szId; };
	const QString & userIdentityId() const { return m_szUserIdentityId; };
	bool autoConnect() const { return m_bAutoConnect; };
	QStringList* autoJoinChannelList(){ return m_pChannelList; };
	bool isIPv6() const { return (m_uFlags & KVI_IRCSERVER_FLAG_IPV6); };
	bool useSSL() const { return (m_uFlags & KVI_IRCSERVER_FLAG_SSL); };
	bool cacheIp() const { return (m_uFlags & KVI_IRCSERVER_FLAG_CACHEIP); };
	
	void setProxy(int p){ m_iProxy = p; };
	void setIp(const QString &a){ m_szIp = a; };
	void setPort(kvi_u32_t p){ m_uPort = p; };
	void setHostName(const QString &n){ m_szHostname = n; };
	void setDescription(const QString &d){ m_szDescription = d; };
	void setUserName(const QString &u){ m_szUser = u; };
	void setPassword(const QString &p){ m_szPass = p; };
	void setNickName(const QString &n){ m_szNick = n; };
	void setRealName(const QString &r){ m_szRealName = r; };
	void setEncoding(const QString &e){ m_szEncoding = e; };
	void setTextEncoding(const QString &e){ m_szTextEncoding = e; };
	void setInitUMode(const QString &u){ m_szInitUMode = u; };
	void setOnConnectCommand(const QString &cmd){ m_szOnConnectCommand = cmd; };
	void setOnLoginCommand(const QString &cmd){ m_szOnLoginCommand = cmd; };
	void setLinkFilter(const QString &f){ m_szLinkFilter = f; };
	// the channel list must be allocated with new!
	void setAutoJoinChannelList(QStringList * pNewChannelList);
	void setAutoConnect(bool autoconnect) { m_bAutoConnect = autoconnect; };
	void setUserIdentityId(const QString &szUserIdentityId){ m_szUserIdentityId = szUserIdentityId; };
	void setIPv6(bool bSet)
	{
		if(bSet)m_uFlags |= KVI_IRCSERVER_FLAG_IPV6;
		else m_uFlags &= ((unsigned short)~KVI_IRCSERVER_FLAG_IPV6);
	};
	void setUseSSL(bool bSet)
	{
		if(bSet)m_uFlags |= KVI_IRCSERVER_FLAG_SSL;
		else m_uFlags &= ((unsigned short)~KVI_IRCSERVER_FLAG_SSL);
	};
	void setCacheIp(bool bSet)
	{
		if(bSet)m_uFlags |= KVI_IRCSERVER_FLAG_CACHEIP;
		else m_uFlags &= ((unsigned short)~KVI_IRCSERVER_FLAG_CACHEIP);
	};
	void operator =(const KviServer &s);

	bool load(KviConfig * cfg,const QString &prefix);
	void save(KviConfig * cfg,const QString &prefix);
	
	void generateUniqueId();
	void setId(const QString &szId){ m_szId = szId; if(m_szId.isEmpty())generateUniqueId(); };

	QString ircUri();
};

class KVILIB_API KviNetwork : public KviHeapObject
{
	friend class KviServerDataBase;
public:
	KviNetwork(const KviNetwork &src);
	KviNetwork(const QString &name);
	~KviNetwork();
protected:
	QString              m_szName;
	QString              m_szDescription;
	QString              m_szEncoding;                // if empty, use system default
	QString              m_szTextEncoding;                // if empty, use system default
	QString              m_szNickName;                // preferred nick name
	QString              m_szUserName;                // preferred user name
	QString              m_szRealName;                // preferred real name
	QString              m_szOnConnectCommand;        // the command to run on connect
	QString              m_szOnLoginCommand;          // the command to run after login
	QStringList        * m_pChannelList;              // Channels to auto join
	KviNickServRuleSet * m_pNickServRuleSet;          // set of nick serv rules
	bool                 m_bAutoConnect;              // autoconnect
	QString              m_szUserIdentityId;          // The user identity to use for this server: if empty
	                                                  // Then use the global primary identity
public:
	const QString & name() const { return m_szName; };
	const QString & encoding() const { return m_szEncoding; };
	const QString & textEncoding() const { return m_szTextEncoding; };
	const QString & description() const { return m_szDescription; };
	const QString & nickName() const { return m_szNickName; };
	const QString & realName() const { return m_szRealName; };
	const QString & userName() const { return m_szUserName; };
	const QString & onLoginCommand() const { return m_szOnLoginCommand; };
	const QString & onConnectCommand() const { return m_szOnConnectCommand; };
	const QString & userIdentityId() const { return m_szUserIdentityId; };
	bool autoConnect() const { return m_bAutoConnect; };
	QStringList* autoJoinChannelList(){ return m_pChannelList; };
	KviNickServRuleSet * nickServRuleSet(){ return m_pNickServRuleSet; };
	void setNickServRuleSet(KviNickServRuleSet * s);
	void copyFrom(const KviNetwork &d);
	void setName(const QString &szName){ m_szName = szName; };
	void setEncoding(const QString &szEncoding){ m_szEncoding = szEncoding; };
	void setTextEncoding(const QString &szEncoding){ m_szTextEncoding = szEncoding; };
	void setDescription(const QString &szDescription){ m_szDescription = szDescription; };
	void setOnConnectCommand(const QString &cmd){ m_szOnConnectCommand = cmd; };
	void setOnLoginCommand(const QString &cmd){ m_szOnLoginCommand = cmd; };
	void setNickName(const QString &n){ m_szNickName = n; };
	void setRealName(const QString &r){ m_szRealName = r; };
	void setUserName(const QString &u){ m_szUserName = u; };
	void setAutoJoinChannelList(QStringList * pNewChannelList);
	void setAutoConnect(bool bAutoConnect){ m_bAutoConnect = bAutoConnect; };
	void setUserIdentityId(const QString &szUserIdentityId){ m_szUserIdentityId = szUserIdentityId; };
};



#endif //_KVI_IRCSERVER_H_
