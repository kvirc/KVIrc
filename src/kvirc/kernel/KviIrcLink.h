#ifndef _KVI_IRCLINK_H_
#define _KVI_IRCLINK_H_
//=============================================================================
//
//   File : KviIrcLink.h
//   Creation date : Mon 03 May 2004 01:45:42 by Szymon Stefanek
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

/**
* \file KviIrcLink.h
* \author Szymon Stefanek
* \brief Middle stack protocol for IRC connection
*/

#include "kvi_settings.h"
#include "KviQString.h"

#include <QObject>

class KviConsoleWindow;
class KviIrcServer;
class KviProxy;
class KviIrcSocket;
class KviDnsResolver;
class KviIrcConnection;
class KviIrcConnectionTarget;
class KviIrcConnectionTargetResolver;
class KviDataBuffer;
class KviMexLinkFilter;

/**
* \class KviIrcLink
* \brief Middle stack class for IRC connection
*
* This class gets data from KviIrcSocket and pass it to KviIrcConnection
*/
class KVIRC_API KviIrcLink : public QObject
{
	friend class KviIrcConnection; // upper protocol in the stack
	friend class KviIrcSocket;     // lower protocol in the stack
	Q_OBJECT
public:
	/**
	* \enum State
	*/
	enum State
	{
		Idle,       /**< Socket idling */
		Connecting, /**< Socket connecting */
		Connected   /**< Socket connected */
	};

protected:
	/**
	* \brief Constructs the IrcLink object
	*
	* Only KviConsoleWindow can create this.
	* pConsole must NOT be null;
	* pServer is a shallow pointer: Connection makes a copy of it internally,
	* must NOT be null;
	* pProxy may be null if a proxy is not desired. Connection makes a copy
	* of it internally;
	* \param pConnection The pointer to the KviIrcConnection class
	* \return KviIrcLink
	*/
	KviIrcLink(KviIrcConnection * pConnection);

	/**
	* \brief Destroys the IrcLink object
	*/
	~KviIrcLink();

private:
	KviIrcConnection * m_pConnection;   // shallow, never null
	KviIrcConnectionTarget * m_pTarget; // shallow, never null
	KviConsoleWindow * m_pConsole;      // shallow, never null
	KviIrcSocket * m_pSocket = nullptr;         // owned, may be null!
	KviMexLinkFilter * m_pLinkFilter = nullptr; // owned, may be null!

	State m_eState = Idle;

	char * m_pReadBuffer = nullptr;    // incoming data buffer
	unsigned int m_uReadBufferLen = 0; // incoming data buffer length
	unsigned int m_uReadPackets = 0;   // total packets read per session

	KviIrcConnectionTargetResolver * m_pResolver = nullptr; // owned
public:
	/**
	* \brief Returns the socket
	*
	* May be null!
	* \return KviIrcSocket *
	*/
	KviIrcSocket * socket() const { return m_pSocket; }

	/**
	* \brief Returns the connection object
	*
	* Never null
	* \return KviIrcConnection *
	*/
	KviIrcConnection * connection() const { return m_pConnection; }

	/**
	* \brief Returns the console
	*
	* Never null
	* \return KviConsoleWindow *
	*/
	KviConsoleWindow * console() const { return m_pConsole; }

	/**
	* \brief Returns the state of the socket
	* \return State
	*/
	State state() const { return m_eState; }
protected:
	/**
	* \brief Sends a data packet
	*
	* This is used by KviIrcConnection::send*()
	* This should be used to intercept the outgoing packets when implementing
	* a new protocol.
	* It's an interface for KviIrcConnection (upper protocol in stack)
	* \param pData The pointer to the data packet
	* \return bool
	*/
	virtual bool sendPacket(KviDataBuffer * pData);

	/**
	* \brief Clears the output queue
	*
	* Requests to clear the underlying packet queue.
	*/
	virtual void clearOutputQueue(bool bPrivateMessagesOnly);

	/**
	* \brief Returns the current size of the output queue.
	* \return unsigned int
	*/
	unsigned int outputQueueSize();

	/**
	* \brief Aborts any connection or attempt
	* \return void
	*/
	void abort();

protected:
	/**
	* \brief Starts a connection attempt.
	*
	* It starts the server or proxy DNS lookup.
	* The function MUST be asynchronous: it must return successfully and
	* report any error by using m_pConnection->linkAttemptFailed()
	* It's called internally and it's overridable
	* \return void
	*/
	void start();

protected:
	/**
	* \brief Process a packet of raw data from the server
	*
	* This is called by KviIrcSocket.
	* The buffer is iLength+1 bytes long and contains a null terminator
	* It's an interface for KviIrcSocket (lower protocol in stack)
	* \param buffer The buffer :)
	* \param iLength The length of the buffer
	* \return void
	*/
	void processData(char * buffer, int iLength);

	/**
	* \brief Called at each state change
	* \return void
	*/
	void socketStateChange();
protected slots:
	/**
	* \brief Called when the link filter has been destroyed without permission :D
	*
	* This should NEVER happen (?)
	* This is part of the KviIrcSocket management
	* \return void
	*/
	void linkFilterDestroyed();

private:
	/**
	* \brief Destroys the socket
	* \return void
	*/
	void destroySocket();

	/**
	* \brief Creates the socket
	* \param szLinkFilterName The source linkfilter name
	* \return void
	*/
	void createSocket(const QString & szLinkFilterName);
signals:
	/**
	* \brief Emitted when the connection attempt has failed
	* \return void
	*/
	void connectionFailed();
private slots:
	/**
	* \brief Called when the DNS lookup has terminated
	* \return void
	*/
	void resolverTerminated();
};

#endif //_KVI_IRCLINK_H_
