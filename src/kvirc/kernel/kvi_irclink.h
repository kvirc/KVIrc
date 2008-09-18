#ifndef _KVI_IRCLINK_H_
#define _KVI_IRCLINK_H_
//=============================================================================
//
//   File : kvi_irclink.h
//   Creation date : Mon 03 May 2004 01:45:42 by Szymon Stefanek
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#include "kvi_settings.h"
#include "kvi_qstring.h"
#include <QObject>

class KviConsole;
class KviServer;
class KviProxy;
class KviIrcSocket;
class KviDns;
class KviIrcConnection;
class KviIrcConnectionTarget;
class KviIrcConnectionTargetResolver;
class KviDataBuffer;
class KviMexLinkFilter;


class KVIRC_API KviIrcLink : public QObject
{
	friend class KviIrcConnection; // upper protocol in the stack
	friend class KviIrcSocket; // lower protocol in the stack
	Q_OBJECT
public:
	enum State { Idle, Connecting, Connected };
protected:
	// only KviConsole can create this
	// pConsole must NOT be null
	// pServer is a shallow pointer: Connection makes a copy of it internally, must NOT be null
	// pProxy may be null if a proxy is not desired. Connection makes a copy of it internally
	KviIrcLink(KviIrcConnection * pConnection);
	~KviIrcLink();
private:
	KviIrcConnection               * m_pConnection;       // shallow, never null
	KviIrcConnectionTarget         * m_pTarget;           // shallow, never null
	KviConsole                     * m_pConsole;          // shallow, never null
	KviIrcSocket                   * m_pSocket;           // owned, may be null!
	KviMexLinkFilter               * m_pLinkFilter;       // owned, may be null!

	State                            m_eState;

	char                           * m_pReadBuffer;
	unsigned int                     m_uReadBufferLen;
	unsigned int                     m_uReadPackets;

	KviIrcConnectionTargetResolver * m_pResolver;         // owned
public:
	// da socket(): may be null!
	KviIrcSocket * socket(){ return m_pSocket; };
	// da connection: never null
	KviIrcConnection * connection(){ return m_pConnection; };
	// da console: never null
	KviConsole * console(){ return m_pConsole; };
	State state(){ return m_eState; };
protected:
	//
	// interface for KviIrcConnection (up)
	//

	// This is used by KviIrcConnection::send*()
	// This should be used to intercept the outgoing packets
	// when implementing a new protocol
	virtual bool sendPacket(KviDataBuffer * pData);
	// this aborts any connection or attempt
	void abort();
protected:
	//
	// local overridables (called internally)
	//
	
	// This is the function used to start a connection attempt.
	// It starts the server or proxy DNS lookup
	// The function MUST be asynchronous: it must return succesfully
	// and report any error by using m_pConnection->linkAttemptFailed()
	void start();
protected:
	//
	// interface for KviIrcSocket (down)
	//
	
	// This is called by KviIrcSocket to process a packet
	// of raw data from the server. The buffer is iLength+1
	// bytes long and contains a null terminator
	void processData(char * buffer,int iLength);
	// this is called at each state change
	void socketStateChange();
protected slots:
	void linkFilterDestroyed();
private:
	void destroySocket();
	void createSocket(const QString &szLinkFilterName);
signals:
	void connectionFailed(); // the connection attempt has failed
private slots:
	void resolverTerminated();
};

#endif //!_KVI_IRCLINK_H_
