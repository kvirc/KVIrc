//=============================================================================
//
//   File : KviProxy.h
//   Creation date : Wed Dec 29 2010 01:58:01 by Elvio Basello
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2010 Elvio Basello (hellvis69 at gmail dot com)
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

#include "KviProxy.h"
#include "KviQString.h"
#include "KviCString.h"

#include <QStringList>

KviProxy::KviProxy()
{
	m_szHostname = "proxy.example.net";
	m_uPort      = 1080;
	m_protocol   = Socks4;
	m_bIsIPv6    = false;
}

KviProxy::KviProxy(const KviProxy &prx)
{
	m_szHostname = prx.m_szHostname;
	m_szIp       = prx.m_szIp;
	m_szUser     = prx.m_szUser;
	m_szPass     = prx.m_szPass;
	m_uPort      = prx.m_uPort;
	m_protocol   = prx.m_protocol;
	m_bIsIPv6    = prx.m_bIsIPv6;
}

KviProxy::~KviProxy()
{
}

static const QString proxy_protocols_table[3]= { "SOCKSv4", "SOCKSv5", "HTTP" };

const QString KviProxy::protocolName() const
{
	switch(m_protocol)
	{
		case Socks5: return proxy_protocols_table[1]; break;
		case Http:   return proxy_protocols_table[2]; break;
		default:     return proxy_protocols_table[0]; break;
	}

	return proxy_protocols_table[0];
}

void KviProxy::setNamedProtocol(const char * proto)
{
	if(kvi_strEqualCI(proto,"SOCKSv5"))m_protocol = KviProxy::Socks5;
	else if(kvi_strEqualCI(proto,"HTTP"))m_protocol = KviProxy::Http;
	else m_protocol = KviProxy::Socks4;
}

void KviProxy::getSupportedProtocolNames(QStringList & buf)
{
	for(int i=0;i<3;i++)buf.append(QString(proxy_protocols_table[i]));
}

void KviProxy::normalizeUserAndPass()
{
	m_szUser = m_szUser.trimmed();
	m_szPass = m_szPass.trimmed();
}
