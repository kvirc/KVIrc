//=============================================================================
//
//   File : KviProxy.cpp
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

#include "KviProxy.h"

#include <QStringList>

KviProxy::KviProxy()
{
	m_szHostname = "proxy.example.net";
	m_uPort = 1080;
	m_protocol = Socks4;
	m_bIsIPv6 = false;
}

KviProxy::~KviProxy()
    = default;

static const QStringList proxy_protocols_table{ "SOCKSv4", "SOCKSv5", "HTTP" };

const QString KviProxy::protocolName() const
{
	switch(m_protocol)
	{
		case Socks5:
			return proxy_protocols_table[1];
			break;
		case Http:
			return proxy_protocols_table[2];
			break;
		default:
			return proxy_protocols_table[0];
	}
}

void KviProxy::setNamedProtocol(const QString & proto)
{
	if(proto.compare(proxy_protocols_table[1], Qt::CaseInsensitive) == 0)
		m_protocol = KviProxy::Socks5;
	else if(proto.compare(proxy_protocols_table[2], Qt::CaseInsensitive) == 0)
		m_protocol = KviProxy::Http;
	else
		m_protocol = KviProxy::Socks4;
}

QStringList KviProxy::getSupportedProtocolNames()
{
	return proxy_protocols_table;
}

void KviProxy::normalizeUserAndPass()
{
	m_szUser = m_szUser.trimmed();
	m_szPass = m_szPass.trimmed();
}
