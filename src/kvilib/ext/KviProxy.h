#ifndef _KVIPROXY_H_
#define _KVIPROXY_H_
//=============================================================================
//
//   File : KviProxy.h
//   Creation date : Wed Dec 29 2010 01:58:01 by Elvio Basello
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2010 Elvio Basello (hellvis69 at gmail dot com)
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

// this file was originally part of KviProxyDataBase.h

#include "kvi_settings.h"
#include "kvi_inttypes.h"

#include <QString>

class QStringList;

class KVILIB_API KviProxy
{
public:
	enum Protocol
	{
		Socks4,
		Socks5,
		Http
	};
	KviProxy();
	~KviProxy();

private:
	QString m_szHostname;
	QString m_szIp;
	QString m_szUser;
	QString m_szPass;
	kvi_u32_t m_uPort;
	Protocol m_protocol;
	bool m_bIsIPv6;

public:
	bool isIPv6() const { return m_bIsIPv6; }
	Protocol protocol() const { return m_protocol; }
	const QString protocolName() const;
	void setNamedProtocol(const QString & proto);
	kvi_u32_t port() const { return m_uPort; }
	const QString & user() const { return m_szUser; }
	const QString & pass() const { return m_szPass; }
	const QString & ip() const { return m_szIp; }
	const QString & hostname() const { return m_szHostname; }
	void normalizeUserAndPass();
	bool hasPass() const { return !m_szPass.isEmpty(); }
	bool hasUser() const { return !m_szUser.isEmpty(); }
	int passLen() const { return m_szPass.length(); }
	int userLen() const { return m_szUser.length(); }
	static QStringList getSupportedProtocolNames();

	void setProtocol(Protocol p) { m_protocol = p; }
	void setIPv6(bool b) { m_bIsIPv6 = b; }
	void setPort(kvi_u32_t p) { m_uPort = p; }
	void setUser(const QString & u) { m_szUser = u; }
	void setPass(const QString & p) { m_szPass = p; }
	void setIp(const QString & i) { m_szIp = i; }
	void setHostname(const QString & h) { m_szHostname = h; }
};

#endif // _KVIPROXY_H_
