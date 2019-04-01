#ifndef _KVI_DNS_H_
#define _KVI_DNS_H_
//=============================================================================
//
//   File : KviDnsResolver.h
//   Creation date : Sat Jul 21 2000 13:59:11 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"
#include "KviError.h"
#include "KviHeapObject.h"

#include <QObject>
#include <QString>
#include <vector>

class KviDnsResolverThread;

class KVILIB_API KviDnsResolverResult : public KviHeapObject
{
	friend class KviDnsResolver;
	friend class KviDnsResolverThread;

protected:
	KviDnsResolverResult();

protected:
	KviError::Code m_eError;
	std::vector<QString> m_pHostnameList;
	std::vector<QString> m_pIpAddressList;
	QString m_szQuery;

public:
	KviError::Code error()
	{
		return m_eError;
	}

	const std::vector<QString> & hostnameList()
	{
		return m_pHostnameList;
	}

	const std::vector<QString> & ipAddressList()
	{
		return m_pIpAddressList;
	}

	const QString & query()
	{
		return m_szQuery;
	}

protected:
	void setError(KviError::Code eError)
	{
		m_eError = eError;
	}

	void setQuery(const QString & szQuery)
	{
		m_szQuery = szQuery;
	}

	void appendHostname(const QString & szHost);
	void appendAddress(const QString & szAddr);
};

#include <QObject>

class KVILIB_API KviDnsResolver : public QObject, public KviHeapObject
{
	Q_OBJECT
	Q_PROPERTY(bool blockingDelete READ isRunning)
public:
	KviDnsResolver();
	~KviDnsResolver();

public:
	enum QueryType
	{
		IPv4,
		IPv6,
		Any
	};
	enum State
	{
		Idle,
		Busy,
		Failure,
		Success
	};

protected:
	KviDnsResolverThread * m_pSlaveThread;
	KviDnsResolverResult * m_pDnsResult;
	State m_state;

public:
	//
	// Public interface
	//

	// Lookup start
	bool lookup(const QString & szQuery, QueryType type);

	// Current object state
	State state() { return m_state; };

	// Results (return always non null-data..but valid results only if state() == Success or Failure)
	KviError::Code error();
	QString errorString();
	const QString & hostName();
	const QString & firstIpAddress();
	std::size_t hostnameCount();
	std::size_t ipAddressCount();
	const std::vector<QString> & hostnameList();
	const std::vector<QString> & ipAddressList();
	const QString & query();
	bool isRunning() const;

protected:
	bool event(QEvent * e) override;

private:
	KviDnsResolverResult * result();
signals:
	void lookupDone(KviDnsResolver *);
};

//
// INTERNAL CLASSES
//

#include <QEvent>
#include <QThread>

#include "kvi_debug.h"

class KviDnsResolverThreadEvent : public QEvent
{
private:
	KviDnsResolverResult * m_pResult;

public:
	KviDnsResolverThreadEvent(KviDnsResolverResult * pResult)
	    : QEvent(QEvent::User), m_pResult(pResult)
	{
		KVI_ASSERT(pResult);
	}

	~KviDnsResolverThreadEvent()
	{
		delete m_pResult;
	}

public:
	KviDnsResolverResult * releaseResult()
	{
		KviDnsResolverResult * pResult = m_pResult;
		m_pResult = nullptr;
		return pResult;
	}
};

class KviDnsResolverThread : public QThread
{
	friend class KviDnsResolver;

protected:
	KviDnsResolverThread(KviDnsResolver * pDns);
	~KviDnsResolverThread();

protected:
	QString m_szQuery;
	KviDnsResolver::QueryType m_queryType = KviDnsResolver::Any;
	KviDnsResolver * m_pParentDns;

public:
	void setQuery(const QString & szQuery, KviDnsResolver::QueryType type)
	{
		m_szQuery = szQuery;
		m_queryType = type;
	};

protected:
	void run() override;
	KviError::Code translateDnsError(int iErr);
	void postDnsError(KviDnsResolverResult * pDns, KviError::Code error);
};

#endif //_KVI_DNS_H_
