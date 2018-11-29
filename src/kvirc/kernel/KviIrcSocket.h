#ifndef _KVI_IRCSOCKET_H_
#define _KVI_IRCSOCKET_H_
//=============================================================================
//
//   File : KviIrcSocket.h
//   Creation date : Tue Jul 30 19:25:17 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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
* \file KviIrcSocket.h
* \author Szymon Stefanek
* \brief Lower stack protocol for IRC connection
*/

#include "kvi_settings.h"
#include "kvi_socket.h"
#include "kvi_sockettype.h"
#include "KviCString.h"
#include "KviError.h"
#include "KviPointerList.h"
#include "KviTimeUtils.h"

#include <QObject>

#include <memory>

class KviConsoleWindow;
class KviDataBuffer;
class KviIrcConnection;
class KviIrcConnectionTarget;
class KviIrcLink;
class KviIrcServer;
class KviProxy;
class KviSSL;
class QSocketNotifier;
class QTimer;

/**
* \typedef KviIrcSocketMsgEntry
* \struct _KviIrcSocketMsgEntry
* \brief Holds the messages entries
*/
struct KviIrcSocketMsgEntry
{
	KviDataBuffer * pData;
	KviIrcSocketMsgEntry * next_ptr;
};

/**
* \class KviIrcSocket
* \brief This class is the lowest level of the KVIrc networking stack
*
* It sends and receives raw data packets from the underlying system socket.
* It should output only verbose "progress" messages.
*/
class KVIRC_API KviIrcSocket : public QObject
{
	Q_OBJECT
public:
	/**
	* \brief Constructs the IRC socket object
	* \param pLink The middle networking stack class
	* \return KviIrcSocket
	*/
	KviIrcSocket(KviIrcLink * pLink);

	/**
	* \brief Destroys the IRC socket object
	*/
	~KviIrcSocket();

public:
	/**
	* \enum SocketState
	*/
	enum SocketState
	{
		Idle,                    /**< Socket idling */
		Connecting,              /**< Socket connecting */
		ProxyLogin,              /**< Socket logging in to a proxy */
		ProxyHttpError,          /**< Socket error on http proxy */
		Connected,               /**< Socket connected */
		ProxyFinalV4,            /**< Socket received the final proxy v4 reply */
		ProxyFinalV5,            /**< Socket received the final proxy v5 reply */
		ProxyFinalHttp,          /**< Socket received the final proxy http reply */
		ProxySelectAuthMethodV5, /**< Socket received v5 method select reply */
		ProxyUserPassV5,         /**< Socket received user/pass reply */
		SSLHandshake             /**< Socket is doing the SSL handshake */
	};

protected:
	unsigned int m_uId;
	KviIrcLink * m_pLink;
	KviConsoleWindow * m_pConsole;
	kvi_socket_t m_sock = KVI_INVALID_SOCKET; // socket
	SocketState m_state = Idle;            // current socket state
	QSocketNotifier * m_pWsn = nullptr;    // read socket notifier
	QSocketNotifier * m_pRsn = nullptr;    // write socket notifier
	KviIrcServer * m_pIrcServer = nullptr; // current server data
	KviProxy * m_pProxy = nullptr;         // current proxy data
	QTimer * m_pTimeoutTimer = nullptr;    // timeout for connect()
	unsigned int m_uReadBytes = 0;         // total read bytes per session
	unsigned int m_uSentBytes = 0;         // total sent bytes per session
	unsigned int m_uSentPackets = 0;       // total packets sent per session
	KviError::Code m_eLastError = KviError::Success;
	KviIrcSocketMsgEntry * m_pSendQueueHead = nullptr; // data queue
	KviIrcSocketMsgEntry * m_pSendQueueTail = nullptr;
	std::unique_ptr<QTimer> m_pFlushTimer;
	struct timeval m_tAntiFloodLastMessageTime;
	bool m_bInProcessData = false;
#ifdef COMPILE_SSL_SUPPORT
	KviSSL * m_pSSL = nullptr;
#endif
public:
	/**
	* \brief Returns the console
	* \return KviConsoleWindow *
	*/
	KviConsoleWindow * console() const { return m_pConsole; }

	/**
	* \brief Returns the link
	* \return KviIrcLink *
	*/
	KviIrcLink * link() const { return m_pLink; }

	/**
	* \brief Returns the state of the socket
	* \return SocketState
	*/
	SocketState state() const { return m_state; }

	/**
	* \brief Returns the last error
	* \return int
	*/
	int lastError() const { return m_eLastError; }

	/**
	* \brief Returns the id of the socket
	* \return unsigned int
	*/
	unsigned int id() const { return m_uId; }

/**
	* \brief Returns true if the socket is a Secure Socket Layer (SSL)
	* \return bool
	*/
	bool usingSSL() const
	{
#ifdef COMPILE_SSL_SUPPORT
		return m_pSSL;
#else
		return false;
#endif
	}

#ifdef COMPILE_SSL_SUPPORT
	/**
	* \brief Returns the current SSL object for this socket
	* \return bool
	*/
	KviSSL * getSSL() const { return m_pSSL; }
#endif
	/**
	* \brief Returns the number of bytes read
	* \return unsigned int
	*/
	unsigned int readBytes() const { return m_uReadBytes; }

	/**
	* \brief Returns the number of bytes sent
	* \return unsigned int
	*/
	unsigned int sentBytes() const { return m_uSentBytes; }

	/**
	* \brief Returns the number of packets sent
	* \return unsigned int
	*/
	unsigned int sentPackets() const { return m_uSentPackets; }
	//unsigned int readPackets() const { return m_uReadPackets; }

	/**
	* \brief Returns true if the socket is connected
	* \return bool
	*/
	bool isConnected() const { return m_state == Connected; }

	/**
	* \brief Starts the connection
	* \param pServer The server where to connect to
	* \param pProxy The proxy to use during connection
	* \param pcBindAddress The address to bind the connection to
	* \return int
	*/
	KviError::Code startConnection(KviIrcServer * pServer, KviProxy * pProxy = nullptr, const char * pcBindAddress = nullptr);

#ifdef COMPILE_SSL_SUPPORT
	/**
	* \brief Called to enter SSL connection mode.
	*
	* It can be called either internally or from outside
	* \return void
	*/
	void enterSSLMode();
#endif // COMPILE_SSL_SUPPORT

	/**
	* \brief Returns true if the packet is sent to the socket
	* \param pData The source data packet
	* \return bool
	*/
	bool sendPacket(KviDataBuffer * pData);

	/**
	* \brief Aborts the connection
	* \return void
	*/
	void abort();

	/**
	* \brief Gets the localhost IP
	* \param szIp The source IP address
	* \param bIPv6 Whether to use IPv6 sockets
	* \return bool
	*/
	bool getLocalHostIp(QString & szIp, bool bIPv6);

	/**
	* \brief Clears the output queue
	* \param bPrivateMessagesOnly Whether to clear only private messages
	* \return void
	*/
	void clearOutputQueue(bool bPrivateMessagesOnly);

	/**
	* \brief Returns the current size of the output queue.
	* \return unsigned int
	*/
	unsigned int outputQueueSize();

protected:
#ifdef COMPILE_SSL_SUPPORT
	/**
	* \brief Raises an SSL error
	* \return void
	*/
	void raiseSSLError();

	/**
	* \brief Prints the SSL certificate of the peer
	* \return void
	*/
	void printSSLPeerCertificate();

	/**
	* \brief Prints the SSL cipher used
	* \return void
	*/
	void printSSLCipherInfo();
#endif // COMPILE_SSL_SUPPORT

	/**
	* \brief Sends raw data to the socket
	* \param pcBuffer The data buffer
	* \param iBufLen The size of the buffer
	* \return bool
	*/
	bool sendRawData(const char * pcBuffer, int iBufLen);

	/**
	* \brief Raises an error
	* \param eError The error to raise
	* \return void
	*/
	void raiseError(KviError::Code eError);

	/**
	* \brief Called when the connection has been established
	*
	* If it's a proxy we need to perform the login operations, otherwise
	* we're connected to the irc server
	* \return void
	*/
	void connectionEstablished();

	/**
	* \brief Called when the connection to the proxy has been established
	* \return void
	*/
	void connectedToProxy();

	/**
	* \brief Called when the connection to the irc server has been established
	* \return void
	*/
	void connectedToIrcServer();

	/**
	* \brief Called to login to an HTTP proxy
	* \return void
	*/
	void proxyLoginHttp();

	/**
	* \brief Called to login to a SOCKSv4 proxy
	* \return void
	*/
	void proxyLoginV4();

	/**
	* \brief Called to login to a SOCKSv5 proxy
	* \return void
	*/
	void proxyLoginV5();

	/**
	* \brief Starts the negotiation of user/pass for the proxy
	* \return void
	*/
	void proxyAuthUserPassV5();

	/**
	* \brief Sends the request details
	* \return void
	*/
	void proxySendTargetDataV5();

	/**
	* \brief Gets the authentication reply and handles it
	* \param cReply The proxy reply
	* \return void
	*/
	void proxyHandleV5AuthReply(unsigned char cReply);

	/**
	* \brief Gets the method selection reply and handles it
	* \param cReply The proxy reply
	* \return void
	*/
	void proxyHandleV5MethodReply(unsigned char cReply);

	/**
	* \brief Gets the final SOCKSv5 reply and handles it
	* \param cReply The proxy reply
	* \return void
	*/
	void proxyHandleV5FinalReply(unsigned char cReply);

	/**
	* \brief Gets the final SOCKSv4 reply and handles it
	* \param cReply The proxy reply
	* \return void
	*/
	void proxyHandleV4FinalReply(unsigned char cReply);

	/**
	* \brief Gets the final HTTP reply and handles it
	* \param pcBuffer The buffer containing the proxy reply
	* \param iBufLen Size of the buffer containing the proxy reply
	* \return void
	*/
	void proxyHandleHttpFinalReply(const char * pcBuffer, int iBufLen);

	/**
	* \brief Connection established. Sets the link up
	* \return void
	*/
	void linkUp();

	/**
	* \brief Checks if the socket error is a transient error
	*
	* If it is not a transient error it resets the socket and fires the
	* appropriate event.
	* Otherwise it does nothing.
	* \param iReadLength The number of bytes read
	* \return void
	*/
	void handleInvalidSocketRead(int iReadLength);

	/**
	* \brief Resets the connection
	* \return void
	*/
	virtual void reset();

	/**
	* \brief Removes the message entry
	* \param e The entry
	* \return void
	*/
	void free_msgEntry(KviIrcSocketMsgEntry * e);

	/**
	* \brief Appends a KviIrcSocketMsgEntry to the tail of the message queue.
	*
	* The pMsg for this message is set to 0.
	* \param pMsg The message to append to the queue
	* \return void
	*/
	virtual void queue_insertMessage(KviIrcSocketMsgEntry * pMsg);

	/**
	* \brief Removes a message from the head of the queue.
	* \return bool
	*/
	bool queue_removeMessage();

	/**
	* \brief Removes all messages from the queue.
	* \return void
	*/
	void queue_removeAllMessages();

	/**
	* \brief Removes private messages from the queue.
	* \return void
	*/
	void queue_removePrivateMessages();

	/**
	* \brief Sets the state of the socket
	* \param state The state :)
	* \return void
	*/
	virtual void setState(SocketState state);

private:
	/**
	* \brief Outputs a SSL message
	* \param szMsg The message :)
	* \return void
	*/
	void outputSSLMessage(const QString & szMsg);

	/**
	* \brief Outputs a SSL error
	* \param szMsg The message :)
	* \return void
	*/
	void outputSSLError(const QString & szMsg);

	/**
	* \brief Outputs a proxy message
	* \param szMsg The message :)
	* \return void
	*/
	void outputProxyMessage(const QString & szMsg);

	/**
	* \brief Outputs a proxy error
	* \param szMsg The message :)
	* \return void
	*/
	void outputProxyError(const QString & szMsg);

	/**
	* \brief Outputs a socket message
	* \param szMsg The message :)
	* \return void
	*/
	void outputSocketMessage(const QString & szMsg);

	/**
	* \brief Outputs a socket warning
	* \param szMsg The message :)
	* \return void
	*/
	void outputSocketWarning(const QString & szMsg);

	/**
	* \brief Outputs a socket error
	* \param szMsg The message :)
	* \return void
	*/
	void outputSocketError(const QString & szMsg);
protected slots:
	/**
	* \brief Called when the connection timeouts
	* \return void
	*/
	void connectionTimedOut();

	/**
	* \brief Called when the write notifier is enabled
	* \return void
	*/
	void writeNotifierFired(int);

	/**
	* \brief Called when the read notifier is enabled
	* \return void
	*/
	void readData(int);

	/**
	* \brief Called when the proxy read notifier is enabled
	* \return void
	*/
	void readProxyData(int);

	/**
	* \brief Called when the proxy read notifier encounters an error
	* \return void
	*/
	void readHttpProxyErrorData(int);

	/**
	* \brief Attempts to send as much as possible to the server
	*
	* If fails (happens only on really lagged servers) calls itself with a
	* QTimer shot after KVI_OPTION_UINT(KviOption_uintSocketQueueFlushTimeout)
	* ms to retry again...
	* \return void
	*/
	void flushSendQueue();

	/**
	* \brief Starts the SSL handshake
	* \return void
	*/
	void doSSLHandshake(int);
};

#endif //_KVI_IRCSOCKET_H_
