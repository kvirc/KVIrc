//
//   File : kvi_url.cpp
//   Creation date : Sat Aug 17 14:09:18 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)
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


#define __KVILIB__


#include "kvi_url.h"

KviUrl::KviUrl()
{
}

KviUrl::KviUrl(const KviUrl & u)
{
	*this = u;
}

KviUrl::KviUrl(const char * szUrl)
{
	m_szUrl = szUrl;
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
	m_szProtocol = "";
	m_szHost = "";
	m_szPath = "";
	m_szUser = "";
	m_szPass = "";

	m_szUrl.stripWhiteSpace();

	KviStr u = m_szUrl;

	// proto
	
	kvi_u32_t uDefaultPort = 80;

	int i = u.findFirstIdx(":/");
	if(i != -1)
	{
		// there is a protocol path
		m_szProtocol = u.left(i);
		u.cutLeft(i + 2);
		u.stripLeft('/');
		u.stripWhiteSpace();
		
		// fix the default ports
		if(kvi_strEqualCI(m_szProtocol,"https"))uDefaultPort = 443;
		else if(kvi_strEqualCI(m_szProtocol,"ftp"))uDefaultPort = 21;
	} else {
		// no proto... assume http
		u.stripLeft('/');
		m_szProtocol = "http";
	}
	
	m_uPort = uDefaultPort;

	// user and pass

	i = u.findFirstIdx('@');

	if(i != -1)
	{
		KviStr szUserPass = u.left(i);
		szUserPass.stripWhiteSpace();
		u.cutLeft(i + 1);

		i = szUserPass.findFirstIdx(':');
		if(i != -1)
		{
			m_szUser = szUserPass.left(i);
			szUserPass.cutLeft(i + 1);
			m_szPass = szUserPass;
			m_szPass.stripWhiteSpace();
		} else {
			m_szUser = szUserPass;
		}
	}

	// host

	i = u.findFirstIdx('/');
	if(i != -1)
	{
		KviStr h = u.left(i);
		u.cutLeft(i + 1);
		i = h.findFirstIdx(':');
		if(i != -1)
		{
			// has a port part
			m_szHost = h.left(i);
			h.cutLeft(i + 1);
			h.stripWhiteSpace();
			bool bOk;
			m_uPort = h.toUInt(&bOk);
			if(!bOk)m_uPort = uDefaultPort;
		} else {
			// no port : assume default
			m_szHost = h;
		}
		m_szPath = u;
	} else {
		m_szHost = u;
	}

	m_szHost.stripWhiteSpace();
	m_szPath.stripWhiteSpace();
	if(!m_szPath.firstCharIs('/'))m_szPath.prepend('/');
}


KviUrl & KviUrl::operator=(const char * szUrl)
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


