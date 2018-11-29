#ifndef _KviDnsResolver_h_
#define _KviDnsResolver_h_
//=============================================================================
//
//   File : KviDnsResolverNew.h
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

// This is a nicely working DNS resolver that uses the QHostInfo interface
// and thus gets rid of all the ugly OS dependencies we have.
// It should be sufficient to rename this file and use this class
// instead of the old KviDnsResolver.

// However at present time (04.01.2010) QHostInfo doesn't seem to support
// IPv6. Despite the documentation saying that IPv6 addresses are supported
// the query performed by QHostInfo::lookupHost() asks only for the A record.
//
// Check it again with newer Qt versions and verify.

#include "kvi_settings.h"
#include "KviError.h" // FIXME: Get rid of this at all..
#include "KviPointerList.h"

#include <QObject>

class KviDnsResolverPrivate;
class QHostInfo;
class QString;

///
/// \class KviDnsResolver
/// \brief The KviDnsResolver class
///
/// This class...
///
class KVILIB_API KviDnsResolver : public QObject
{
	Q_OBJECT
public:
	///
	/// Creates an instance of KviDnsResolver
	///
	KviDnsResolver(QObject * pParent = nullptr);

	///
	/// Destroys the instance of KviDnsResolver
	/// and frees all the relevant resources
	///
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

private:
	KviDnsResolverPrivate * m_pPrivateData;

public:
	///
	/// Returns the resolver state
	///
	State state() const;

	///
	/// Starts a lookup for the specified DNS query of the specified type.
	/// Returns true on success and false on failure.
	///
	bool lookup(const QString & szQuery, QueryType eType);

	// Results (return always non null-data..but valid results only if state() == Success or Failure)
	KviError::Code error() const; // DEPRECATED use errorString()
	const QString & errorString() const;

	const QString & hostName() const;
	const QString & firstIpAddress();
	int ipAddressCount() const;
	KviPointerList<QString> * ipAddressList();
	const QString & query() const;
	bool isRunning() const;

private slots:
	void slotHostLookupTerminated(const QHostInfo & oHostInfo);

signals:
	void lookupDone(KviDnsResolver *);

}; // class KviDnsResolver

#endif //!_KviDnsResolver_h_
