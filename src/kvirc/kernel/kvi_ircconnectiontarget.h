#ifndef _KVI_IRCCONNECTIONTARGET_H_
#define _KVI_IRCCONNECTIONTARGET_H_
//=============================================================================
//
//   File : kvi_ircconnectiontarget.h
//   Creation date : Tue 08 Jun 2004 14:11:59 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2008 Szymon Stefanek <pragma at kvirc dot net>
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
#include "kvi_qstring.h"
#include "kvi_heapobject.h"

class KviNetwork;
class KviServer;
class KviProxy;

class KVIRC_API KviIrcConnectionTarget : public KviHeapObject
{
	friend class KviIrcConnectionTargetResolver;
public:
	// pServer is a shallow pointer: this class makes a copy of it internally, must NOT be null
	// pProxy may be null if a proxy is not desired. this class makes a copy of it internally
	KviIrcConnectionTarget(const KviNetwork * pNetwork,
							const KviServer * pServer,
							const KviProxy * pProxy = 0,
							const QString &szBindAddress = QString());
	~KviIrcConnectionTarget();
private:
	KviNetwork      * m_pNetwork;        // owned, never null
	KviServer       * m_pServer;         // owned, never null
	KviProxy        * m_pProxy;          // owned, may be null
	QString           m_szBindAddress;   // forced bind address
public:
	KviServer * server(){ return m_pServer; };
	KviNetwork * network(){ return m_pNetwork; };
	KviProxy * proxy(){ return m_pProxy; };
	const QString & bindAddress(){ return m_szBindAddress; };
	bool hasBindAddress(){ return (!m_szBindAddress.isEmpty()); };
	const QString & networkName();
	// this allows to force the network name: it should be used only by KviConsole
	// but it's KviServerParser (kvi_sp_numeric.cpp) that first
	// gets the new network name... thus we make it public
	// to remove one level of indirection.
	void setNetworkName(const QString &szNetName);
protected:
	// this is for KviIrcConnectionTargetResolver only
	void clearProxy();
	void setBindAddress(const QString &szBindAddress){ m_szBindAddress = szBindAddress; };
};

#endif //!_KVI_IRCCONNECTIONTARGET_H_
