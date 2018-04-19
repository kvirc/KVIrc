//=============================================================================
//
//   File : KviIrcConnectionTarget.cpp
//   Creation date : Tue 08 Jun 2004 14:11:59 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "KviIrcConnectionTarget.h"
#include "KviIrcServer.h"
#include "KviIrcNetwork.h"
#include "KviProxy.h"
#include "KviProxyDataBase.h"

KviIrcConnectionTarget::KviIrcConnectionTarget(
    const KviIrcNetwork * pNetwork,
    const KviIrcServer * pServer,
    const KviProxy * pProxy,
    const QString & szBindAddress)
{
	m_pNetwork = new KviIrcNetwork(*pNetwork);
	m_pServer = new KviIrcServer(*pServer);
	if(pProxy)
		m_pProxy = new KviProxy(*pProxy);
	m_szBindAddress = szBindAddress;
}

KviIrcConnectionTarget::~KviIrcConnectionTarget()
{
	delete m_pNetwork;
	delete m_pServer;
	if(m_pProxy)
		delete m_pProxy;
}

void KviIrcConnectionTarget::clearProxy()
{
	if(!m_pProxy)
		return;
	delete m_pProxy;
	m_pProxy = nullptr;
}
