//=============================================================================
//
//   File : kvi_url.cpp
//   Creation date : Sat Aug 17 14:09:18 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002-2008 Szymon Stefanek (pragma at kvirc dot net)
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



#include "kvi_url.h"
#include <QUrl>

KviUrl::KviUrl()
{
}

KviUrl::KviUrl(const KviUrl & u)
{
	*this = u;
}

KviUrl::KviUrl(const char * szUrl)
{
	m_szUrl = QString::fromLocal8Bit(szUrl);
	parse();
}

KviUrl::KviUrl(const QString &szUrl)
{
	m_szUrl = szUrl;
	parse();
}

KviUrl::~KviUrl()
{

}

void KviUrl::parse()
{
	QUrl url(m_szUrl);
	m_szProtocol = url.scheme();
	m_szHost = url.host();
	m_szPath = url.encodedPath();
	if(url.hasQuery())
	{
		m_szPath.append(QChar('?'));
		m_szPath.append(url.encodedQuery());
	}
	if(m_szPath.isEmpty()) m_szPath=QString("/");
	m_szUser = url.userName();
	m_szPass = url.password();
	m_uPort = url.port() > 0 ? url.port() : 80 ;
}


KviUrl & KviUrl::operator=(const QString& szUrl)
{
	m_szUrl = szUrl;
	parse();
	return *this;
}

KviUrl & KviUrl::operator=(const KviUrl &u)
{
	m_szUrl = u.m_szUrl;
	m_szProtocol = u.m_szProtocol;
	m_szHost = u.m_szHost;
	m_szPath = u.m_szPath;
	m_szUser = u.m_szUser;
	m_szPass = u.m_szPass;
	m_uPort = u.m_uPort;
	return *this;
}


