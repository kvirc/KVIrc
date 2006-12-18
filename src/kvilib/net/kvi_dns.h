#ifndef _KVI_DNS_H_
#define _KVI_DNS_H_

//
//   File : kvi_dns.h
//   Creation date : Sat Jul 21 2000 13:59:11 by Szymon Stefanek
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//

#include "kvi_settings.h"
#include "kvi_heapobject.h"
#include "kvi_thread.h"
#include "kvi_string.h"


class KviDnsThread; // not part of the API


class KVILIB_API KviDnsResult : public KviHeapObject
{
	friend class KviDns;
	friend class KviDnsThread;
protected:
	KviDnsResult();
public:
	~KviDnsResult();
protected:
	int             m_iError;
	KviPtrList<KviStr> * m_pHostnameList;
	KviPtrList<KviStr> * m_pIpAddressList;
	KviStr               m_szQuery;
public:
	int error(){ return m_iError; };
	// never store nor delete these pointers!
	// (these are NEVER 0)
	KviPtrList<KviStr> * hostnameList(){ return m_pHostnameList; };
	KviPtrList<KviStr> * ipAddressList(){ return m_pIpAddressList; };
	const char * query(){ return m_szQuery.ptr(); };
protected:
	void setError(int iError){ m_iError = iError; };
	void setQuery(const QString &query){ m_szQuery = query; };
	void appendHostname(const char * host);
	void appendAddress(const char * addr);
};



class KVILIB_API KviDns : public QObject, public KviHeapObject
{
	Q_OBJECT
	Q_PROPERTY(bool blockingDelete READ isRunning)
public:
	KviDns();
	~KviDns();
public:
	enum QueryType { IpV4 , IpV6 , Any };
	enum State     { Idle , Busy , Failure , Success };
protected:
	void         * m_pAuxData;
	KviDnsThread * m_pSlaveThread;
	KviDnsResult * m_pDnsResult;
	State          m_state;
public:
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Public interface
	//

	// Lookup start
	bool lookup(const QString &szQuery,QueryType type);

	// Current object state
	State state(){ return m_state; };

	// Results (return always non null-data..but valid results only if state() == Success or Failure)
	int error();
	const char * firstHostname();
	const char * firstIpAddress();
	int hostnameCount();
	int ipAddressCount();
	KviPtrList<KviStr> * hostnameList();
	KviPtrList<KviStr> * ipAddressList();
	const char * query();
	bool isRunning() const;

	// Auxiliary data store
	void setAuxData(void * pAuxData){ m_pAuxData = pAuxData; };
	void * releaseAuxData(){ void * pData = m_pAuxData; m_pAuxData = 0; return pData; };
protected:
	virtual bool event(QEvent *e);
private:
	KviDnsResult * result();
signals:
	void lookupDone(KviDns *);
};



///////////////////////////////////////////////////////////////////////////////////////////////////////////
// INTERNAL CLASSES
//

#define KVI_DNS_THREAD_EVENT_DATA (KVI_THREAD_USER_EVENT_BASE + 7432)

class KviDnsThread : public KviThread
{
	friend class KviDns;
protected:
	KviDnsThread(KviDns * pDns);
	~KviDnsThread();
protected:
	KviStr              m_szQuery;
	KviDns::QueryType   m_queryType;
	KviDns            * m_pParentDns;
public:
	void setQuery(const char * query,KviDns::QueryType type){ m_szQuery = query; m_queryType = type; };
protected:
	virtual void run();
	int translateDnsError(int iErr);
	void postDnsError(KviDnsResult * dns,int iErr);
};


#endif //_KVI_DNS_H_
