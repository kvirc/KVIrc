//=============================================================================
//
//   File : KviDnsResolverNew.cpp
//   Creation date : Sat Jul 21 2000 17:19:31 by Szymon Stefanek
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

#include "KviDnsResolver.h"
#include "KviLocale.h"
#include "kvi_debug.h"

#include <QHostInfo>
#include <QString>

class KviDnsResolverPrivate
{
public:
	int iHostLookupId;
	KviDnsResolver::State eState;
	KviDnsResolver::QueryType eQueryType;
	KviError::Code eError; // DEPRECATED
	QString szError;
	QString szQuery;
	QString szHostName;
	QString szDummy;
	KviPointerList<QString> * pAddressList;
};

KviDnsResolver::KviDnsResolver(QObject * pParent)
    : QObject(pParent)
{
	m_pPrivateData = new KviDnsResolverPrivate;
	m_pPrivateData->eState = Idle;
	m_pPrivateData->pAddressList = new KviPointerList<QString>(true);
}

KviDnsResolver::~KviDnsResolver()
{
	if(m_pPrivateData->eState == Busy)
		QHostInfo::abortHostLookup(m_pPrivateData->iHostLookupId);
	delete m_pPrivateData->pAddressList;
	delete m_pPrivateData;
}

KviDnsResolver::State KviDnsResolver::state() const
{
	return m_pPrivateData->eState;
}

bool KviDnsResolver::lookup(const QString & szQuery, QueryType eType)
{
	if(m_pPrivateData->eState == Busy)
	{
		m_pPrivateData->eError = KviError::DNSQueryFailed;
		m_pPrivateData->eState = Failure;
		m_pPrivateData->szError = __tr2qs("DNS resolver busy");
		return false;
	}

	m_pPrivateData->eQueryType = eType;
	m_pPrivateData->eState = Busy;
	m_pPrivateData->szQuery = szQuery;
	m_pPrivateData->iHostLookupId = QHostInfo::lookupHost(szQuery, this, SLOT(slotHostLookupTerminated(const QHostInfo &)));

	if(m_pPrivateData->iHostLookupId < 0)
	{
		m_pPrivateData->eError = KviError::DNSQueryFailed;
		m_pPrivateData->eState = Failure;
		m_pPrivateData->szError = __tr2qs("Failed to start the host lookup");
		//emit lookupDone(this);
		return false;
	}

	return true;
}

void KviDnsResolver::slotHostLookupTerminated(const QHostInfo & oHostInfo)
{
	if(oHostInfo.error() != QHostInfo::NoError)
	{
		m_pPrivateData->eState = Failure;

		switch(oHostInfo.error())
		{
			case QHostInfo::HostNotFound:
				m_pPrivateData->eError = KviError::HostNotFound;
				break;
			default:
				m_pPrivateData->eError = KviError::DNSQueryFailed;
				break;
		}

		m_pPrivateData->szError = oHostInfo.errorString();

		emit lookupDone(this);

		return;
	}

	m_pPrivateData->szHostName = oHostInfo.hostName();

	QList<QHostAddress> lAddresses = oHostInfo.addresses();

	m_pPrivateData->pAddressList->clear();

	m_pPrivateData->eError = KviError::Success;
	m_pPrivateData->szError = QString();
	m_pPrivateData->eState = Success;

	foreach(QHostAddress oAddress, lAddresses)
	{
		switch(m_pPrivateData->eQueryType)
		{
			case IPv4:
				if(oAddress.protocol() == QAbstractSocket::IPv4Protocol)
					m_pPrivateData->pAddressList->append(new QString(oAddress.toString()));
				break;
			case IPv6:
				if(oAddress.protocol() == QAbstractSocket::IPv6Protocol)
					m_pPrivateData->pAddressList->append(new QString(oAddress.toString()));
				break;
			case Any:
				m_pPrivateData->pAddressList->append(new QString(oAddress.toString()));
				break;
			default:
				KVI_ASSERT_MSG(false, "Invalid DNS query type!");
				m_pPrivateData->eState = Failure;
				m_pPrivateData->eError = KviError::InternalError;
				m_pPrivateData->szError = __tr2qs("Internal error: unhandled DNS query type");
				emit lookupDone(this);
				return;
				break;
		}
	}

	if(m_pPrivateData->pAddressList->isEmpty())
	{
		m_pPrivateData->eState = Failure;
		m_pPrivateData->eError = KviError::ValidNameButNoIpAddress;
		m_pPrivateData->szError = __tr2qs("DNS query returned no IP address results of the requested type");
	}

	emit lookupDone(this);
}

KviError::Code KviDnsResolver::error() const
{
	return m_pPrivateData->eError;
}

const QString & KviDnsResolver::errorString() const
{
	return m_pPrivateData->szError;
}

const QString & KviDnsResolver::hostName() const
{
	return m_pPrivateData->szHostName;
}

const QString & KviDnsResolver::firstIpAddress()
{
	QString * pIpAddress = m_pPrivateData->pAddressList->first();
	if(!pIpAddress)
		return m_pPrivateData->szDummy;
	return *pIpAddress;
}
const QString & firstIpAddress();

int KviDnsResolver::ipAddressCount() const
{
	return m_pPrivateData->pAddressList->count();
}

KviPointerList<QString> * KviDnsResolver::ipAddressList()
{
	return m_pPrivateData->pAddressList;
}

const QString & KviDnsResolver::query() const
{
	return m_pPrivateData->szQuery;
}

bool KviDnsResolver::isRunning() const
{
	return m_pPrivateData->eState == Busy;
}
