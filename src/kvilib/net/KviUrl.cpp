//=============================================================================
//
//   File : KviUrl.cpp
//   Creation date : Sat Aug 17 14:09:18 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviUrl.h"

#include <QUrl>

KviUrl::KviUrl()
    = default;

KviUrl::KviUrl(const char * szUrl)
{
	m_szUrl = QString::fromLocal8Bit(szUrl);
	parse();
}

KviUrl::KviUrl(const QString & szUrl)
{
	m_szUrl = szUrl;
	parse();
}

KviUrl::KviUrl(const KviUrl &)
    = default;

KviUrl::~KviUrl()
    = default;

void KviUrl::parse()
{
	QUrl url(m_szUrl);
	m_szProtocol = url.scheme();
	m_szHost = url.host();
	m_szPath.append(url.path(QUrl::FullyEncoded));

	if(url.hasQuery())
	{
		m_szPath.append(QChar('?'));
		m_szPath.append(url.query(QUrl::FullyEncoded));
	}
	if(m_szPath.isEmpty())
		m_szPath = QString("/");
	m_szUser = url.userName();
	m_szPass = url.password();
	m_uPort = url.port(0);
}

void KviUrl::build()
{
	QUrl url;
	url.setScheme(m_szProtocol);
	url.setHost(m_szHost);
	url.setPath(m_szPath);
	url.setUserName(m_szUser);
	url.setPassword(m_szPass);
	url.setPort(m_uPort);
	m_szUrl = url.toString();
}

KviUrl & KviUrl::operator=(const QString & szUrl)
{
	m_szUrl = szUrl;
	parse();
	return *this;
}

KviUrl & KviUrl::operator=(const KviUrl &)
    = default;

void KviUrl::setUrl(QString & szUrl)
{
	m_szUrl = szUrl;
	parse();
}

void KviUrl::setProtocol(QString & szProtocol)
{
	m_szProtocol = szProtocol;
	build();
}

void KviUrl::setHost(QString & szHost)
{
	m_szHost = szHost;
	build();
}

void KviUrl::setPath(QString & szPath)
{
	m_szPath = szPath;
	if(m_szPath.isEmpty())
		m_szPath = QString("/");
	build();
}

void KviUrl::setUser(QString & szUser)
{
	m_szUser = szUser;
	build();
}

void KviUrl::setPass(QString & szPass)
{
	m_szPass = szPass;
	build();
}

void KviUrl::setPort(kvi_u32_t uPort)
{
	m_uPort = uPort;
	build();
}
