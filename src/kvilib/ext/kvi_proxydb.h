#ifndef _KVI_PROXYDB_H_
#define _KVI_PROXYDB_H_

//
//   File : kvi_proxydb.h
//   Creation date : Sat Jul 22 2000 18:19:01 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2000 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"

#include "kvi_string.h"
#include "kvi_inttypes.h"
#include "kvi_pointerlist.h"
#include <qstringlist.h>

class KVILIB_API KviProxy
{
public:
	enum Protocol { Socks4 , Socks5 , Http };
	KviProxy();
	KviProxy(const KviProxy &prx);
	~KviProxy();
public:
	KviStr             m_szHostname;
	KviStr             m_szIp;
	KviStr             m_szPass;
	KviStr             m_szUser;
	kvi_u32_t          m_uPort;
	Protocol           m_protocol;
	bool               m_bIsIpV6;
public:
	bool isIpV6() const             { return m_bIsIpV6; };
	Protocol protocol() const       { return m_protocol; };
	const char * protocolName() const;
	void setNamedProtocol(const char * proto);
	kvi_u32_t port() const { return m_uPort; };
	const char * user() const       { return m_szUser.ptr(); };
	const char * pass() const       { return m_szPass.ptr(); };
	const char * ip() const         { return m_szIp.ptr(); };
	const char * hostname() const   { return m_szHostname.ptr(); };
	void normalizeUserAndPass();
	bool hasPass() const            { return m_szPass.hasData(); };
	bool hasUser() const            { return m_szUser.hasData(); };
	unsigned int passLen() const    { return (unsigned int)m_szPass.len(); };
	unsigned int userLen() const    { return (unsigned int)m_szUser.len(); };
	static void getSupportedProtocolNames(QStringList & buf);
};


class KVILIB_API KviProxyDataBase
{
public:
	KviProxyDataBase();
	~KviProxyDataBase();
private:
	KviPointerList<KviProxy> * m_pProxyList;
	KviProxy        * m_pCurrentProxy;
public:
	void clear();
	KviPointerList<KviProxy> * proxyList(){ return m_pProxyList; };
	KviProxy * currentProxy(){ return m_pCurrentProxy; };
	void updateProxyIp(const char * proxy,const char * ip);
	void setCurrentProxy(KviProxy * prx){ m_pCurrentProxy = prx; };
	void insertProxy(KviProxy * prx){ m_pProxyList->append(prx); };
	void load(const char * filename);
	void save(const char * filename);
};

#endif //_KVI_PROXYDB_H_
