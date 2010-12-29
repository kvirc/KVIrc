#ifndef _KVI_PROXYDB_H_
#define _KVI_PROXYDB_H_

//=============================================================================
//
//   File : KviProxyDataBase.h
//   Creation date : Sat Jul 22 2000 18:19:01 by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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
#include "KviPointerList.h"

class KviProxy;

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
