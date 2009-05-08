#ifndef _KVI_PROXYDB_H_
#define _KVI_PROXYDB_H_
//=============================================================================
//
//   File : kvi_proxydb.h
//   Creation date : Sat Jul 22 2000 18:19:01 by Szymon Stefanek
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

#include "kvi_settings.h"
#include "kvi_string.h"
#include "kvi_inttypes.h"
#include "kvi_pointerlist.h"

#include <QStringList>

class KVILIB_API KviProxy
{
public:
	enum Protocol { Socks4 , Socks5 , Http };
	KviProxy();
	KviProxy(const KviProxy &prx);
	~KviProxy();
public:
	QString             m_szHostname;
	QString             m_szIp;
	QString             m_szPass;
	QString             m_szUser;
	kvi_u32_t           m_uPort;
	Protocol            m_protocol;
	bool                m_bIsIPv6;
public:
	bool isIPv6() const             { return m_bIsIPv6; };
	Protocol protocol() const       { return m_protocol; };
	const QString protocolName() const;
	void setNamedProtocol(const char * proto);
	kvi_u32_t port() const { return m_uPort; };
	const QString & user() const       { return m_szUser; };
	const QString & pass() const       { return m_szPass; };
	const QString & ip() const         { return m_szIp; };
	const QString & hostname() const   { return m_szHostname; };
	void normalizeUserAndPass();
	bool hasPass() const         { return !m_szPass.isEmpty(); };
	bool hasUser() const         { return !m_szUser.isEmpty(); };
	unsigned int passLen() const { return (unsigned int)m_szPass.length(); };
	unsigned int userLen() const { return (unsigned int)m_szUser.length(); };
	static void getSupportedProtocolNames(QStringList & buf);

	void setProtocol(Protocol p){ m_protocol = p; };
	void setIPv6(bool b){ if(b) m_bIsIPv6 = true; else m_bIsIPv6 = false; };
	void setPort(kvi_u32_t &p){ m_uPort = p; };
	void setUser(const QString &u){ m_szUser = u; };
	void setPass(const QString &p){ m_szPass = p; };
	void setIp(const QString &i){ m_szIp = i; };
	void setHostname(const QString &h){ m_szHostname = h; };
};


class KVILIB_API KviProxyDataBase
{
public:
	KviProxyDataBase();
	~KviProxyDataBase();
private:
	KviPointerList<KviProxy> * m_pProxyList;
	KviProxy                 * m_pCurrentProxy;
public:
	void clear();
	KviPointerList<KviProxy> * proxyList(){ return m_pProxyList; };
	KviProxy * currentProxy(){ return m_pCurrentProxy; };
	KviProxy * findProxy(const KviProxy * pProxy, bool bName);
	void updateProxyIp(const char * proxy,const char * ip);
	void setCurrentProxy(KviProxy * prx){ m_pCurrentProxy = prx; };
	void insertProxy(KviProxy * prx){ m_pProxyList->append(prx); };
	void load(const QString &filename);
	void save(const QString &filename);
};

#endif //_KVI_PROXYDB_H_
