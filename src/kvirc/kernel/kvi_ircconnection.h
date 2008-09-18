#ifndef _KVI_IRCCONNECTION_H_
#define _KVI_IRCCONNECTION_H_
//=============================================================================
//
//   File : kvi_ircconnection.h
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
#include "kvi_qcstring.h"
#include "kvi_pointerlist.h"
#include "kvi_time.h"
#include "kvi_ircconnectionuserinfo.h"
#include "kvi_ircconnectionserverinfo.h"

#include <QObject>

class KviConsole;
class KviNetworkData;
class KviServer;
class KviProxy;
class KviIrcSocket;
class KviIrcLink;
class KviDataBuffer;
class KviIrcContext;
class KviFrame;
class KviChannel;
class KviQuery;
class KviIrcConnectionTarget;
class KviIrcUserDataBase;
class KviIrcConnectionUserInfo;
class KviIrcConnectionServerInfo;
class KviIrcConnectionStateData;
class KviIrcConnectionAntiCtcpFloodData;
class KviIrcConnectionNetsplitDetectorData;
class KviIrcConnectionAsyncWhoisData;
class KviIrcConnectionStatistics;
class KviLagMeter;
class KviNotifyListManager;
class KviDns;
class KviUserIdentity;

class QTimer;
class QTextCodec;

///
/// \class KviIrcConnection
/// \brief An abstraction of a connection to an IRC server
///
/// This class deals with the high-level logic of a connection to an IRC server.
/// It's always attached to a KviIrcContext and thus to a KviConsole window.
/// The connection has a KviIrcConnectionTarget object which specifies the
/// data of the server that needs to be contacted and a KviUserIdentity object
/// which specifies the user data to use.
///
/// The connection is the upper layer of the KVIrc networking stack. It owns
/// KviIrcLink which is the layer immediately below and which in turn contains
/// a KviIrcSocket.
///
/// The connection creates and manages a lot of objects that handle
/// runtime informations. These objects include a KviIrcConnectionUserInfo which
/// contains the ACTUAL user informations, KviIrcConnectionServerInfo which
/// contains the ACTUAL server informations, KviIrcUserDataBase which is substantially
/// a big hash table containing all the users visible to KVIrc in this connection,
/// a list of channels, a list of queries etc...
///
class KVIRC_API KviIrcConnection : public QObject
{
	friend class KviConsole;
	friend class KviIrcContext;
	friend class KviIrcLink;
	friend class KviServerParser;
	Q_OBJECT
protected:
	///
	/// Creates a KviIrcConnection object. This is actually used only by KviConsole.
	///
	/// \param pContext  The KviIrcContext we're attacched to: must not be NULL and is
	///                  kept as shallow pointer (that is, it's not owned and must persist
	///                  for the entire life of KviIrcConnection: caller is responsable for that).
	/// \param pTarget   The server data: must not be NULL and must be allocated with new as this class
	///                  takes the ownership.
	/// \param pIdentity The KviUserIdentity object that specifies the user informations to
	///                  use in ths connection. Must not be NULL and must be allocated with new
	///                  as this class takes the ownership.
	///
	KviIrcConnection(KviIrcContext * pContext,KviIrcConnectionTarget * pTarget,KviUserIdentity * pIdentity);
	///
	/// Destroys a KviIrcConnection object. KviConsole uses this.
	///
	~KviIrcConnection();
public:
	///
	/// The possible "high level" states of this connection.
	///
	enum State
	{
		Idle,       ///< The connection is doing nothing
		Connecting, ///< A connection attempt is in progress
		Connected   ///< We're connected to an IRC server
	};
private:
	KviConsole                           * m_pConsole;              // shallow, never null
	KviIrcContext                        * m_pContext;              // shallow, never null

	State                                  m_eState;
	bool                                   m_bIdentdAttached;

	KviIrcConnectionTarget               * m_pTarget;               // owned, never null

	KviIrcLink                           * m_pLink;                 // owned, never null

	// The initial informations about the user we'll send out to the server
	// Note that the ACTUAL user informations are in m_pUserInfo instead
	KviUserIdentity                      * m_pUserIdentity;         // owned, never null

	// The ACTUAL user informations
	KviIrcConnectionUserInfo             * m_pUserInfo;             // owned, never null
	// The ACTUAL server informations
	KviIrcConnectionServerInfo           * m_pServerInfo;           // owned, never null

	KviIrcConnectionStateData            * m_pStateData;            // owned, never null

	KviPointerList<KviChannel>           * m_pChannelList;          // owned, never null, elements shallow
	KviPointerList<KviQuery>             * m_pQueryList;            // owned, never null, elements shallow

	KviIrcUserDataBase                   * m_pUserDataBase;         // owned, never null

	KviNotifyListManager                 * m_pNotifyListManager;    // owned, see restartNotifyList()
	QTimer                               * m_pNotifyListTimer;      // delayed startup timer for the notify lists

	KviLagMeter                          * m_pLagMeter;             // owned, may be null (when not running)

	KviIrcConnectionAntiCtcpFloodData    * m_pAntiCtcpFloodData;    // owned, never null
	KviIrcConnectionNetsplitDetectorData * m_pNetsplitDetectorData; // owned, never null
	KviIrcConnectionAsyncWhoisData       * m_pAsyncWhoisData;       // owned, never null

	KviIrcConnectionStatistics           * m_pStatistics;           // owned, never null

	KviDns                               * m_pLocalhostDns;         // FIXME: this should go to an aux structure

	QTextCodec                           * m_pSrvCodec;             // connection codec: never null
	QTextCodec                           * m_pTextCodec;            // connection codec: never null

public:

	///
	/// Returns a pointer to the owning console
	/// The pointer is never NULL
	///
	inline KviConsole * console()
		{ return m_pConsole; };

	///
	/// Returns a pointer to the owning KviIrcContext.
	/// The returned value is never NULL
	///
	inline KviIrcContext * context()
		{ return m_pContext; };

	///
	/// Forwarder from KviIrcConnectionTarget, never null
	/// Deprecated: use target()->server()
	///
	KVI_DEPRECATED KviServer * server();

	///
	/// Forwarder from KviIrcConnectionTarget, may be null
	/// Deprecated: use target()->proxy()
	///
	KVI_DEPRECATED KviProxy * proxy();

	///
	/// Returns the target of this connection. Please note
	/// that the target doesn't necessairly contain up-to-date data.
	/// You might want to look at serverInfo() instead.
	/// The returned pointer is never NULL.
	///
	inline KviIrcConnectionTarget * target()
		{ return m_pTarget; };

	///
	/// Returns the underlying KviIrcLink object which is one level
	/// lower in the KVIrc's networking stack.
	/// The returned pointer is never NULL.
	///
	inline KviIrcLink * link()
		{ return m_pLink; };

	///
	/// Returns a pointer to the KviIrcSocket which is two levels
	/// lower in the KVIrc's networking stack.
	/// Deprecated: don't use it at all: create wrappers in KviIrcLink
	/// instead as we want to achieve networking stack layer insulation.
	///
	KVI_DEPRECATED KviIrcSocket * socket();

	///
	/// Returns the current state of the connection
	///
	inline State state()
		{ return m_eState; };

	///
	/// Forwarder from KviIrcConnectionTarget, always non-empty string
	///
	const QString & networkName();

	///
	/// Returns a pointer to the big connection user database.
	/// The database contains ALL the users KVIrc can "see" in this connection.
	/// The returned pointer is never NULL.
	///
	inline KviIrcUserDataBase * userDataBase()
		{ return m_pUserDataBase; };

	///
	/// Returns a pointer to the KviIrcConnectionUserInfo object
	/// which contains runtime informations about the user. This includes
	/// the current nickname, username, flags and other stuff that
	/// KviUserIdentity actually doesn't contain (or has only "default" values for).
	/// The returned pointer is never NULL.
	/// Use #include "kvi_ircconnectionuserinfo.h" as the class is only forwarded here.
	///
	inline KviIrcConnectionUserInfo * userInfo()
		{ return m_pUserInfo; };

	///
	/// Returns a pointer to the KviIrcConnectionServerInfo object
	/// which contains runtime informations about the current server. This includes
	/// the current servername, the server capabilities and other stuff that
	/// KviConnectionTarget actually doesn't contain (or has only "default" values for).
	/// The returned pointer is never NULL.
	/// Use #include "kvi_ircconnectionserverinfo.h" as the class is only forwarded here.
	///
	inline KviIrcConnectionServerInfo * serverInfo()
		{ return m_pServerInfo; };

	///
	/// Returns a pointer to the KviIrcConnectionStateData object
	/// which contains _generic_ connection state data. This includes the current
	/// nickname index at login time, flags that signal "micro-states" etc...
	/// This data *could* be part of KviIrcConnection itself but we
	/// prefer to keep it in a separate class in order to cleanup the implementation.
	/// The returned pointer is never NULL.
	/// Use #include "kvi_ircconnectionstatedata.h" as the class is only forwarded here.
	///
	inline KviIrcConnectionStateData * stateData()
		{ return m_pStateData; };

	///
	/// Returns a pointer to the KviIrcConnectionAntiCtcpFloodData object
	/// which contains data private to the Anti CTCP Flood engine. Very similar
	/// to KviIrcConnectionStateData but dedicated to Ctcp flood.
	/// The returned pointer is never NULL.
	/// Use #include "kvi_ircconnectionantictcpflooddata.h" as the class is only forwarded here.
	///
	inline KviIrcConnectionAntiCtcpFloodData * antiCtcpFloodData()
		{ return m_pAntiCtcpFloodData; };

	///
	/// Returns a pointer to the KviIrcConnectionNetsplitDetectorData object
	/// which contains data private to the netsplit detector engine. Very similar
	/// to KviIrcConnectionStateData but dedicated to netsplit detection.
	/// The returned pointer is never NULL.
	/// Use #include "kvi_ircconnectionnetsplitdetectordata.h" as the class is only forwarded here.
	///
	inline KviIrcConnectionNetsplitDetectorData * netsplitDetectorData()
		{ return m_pNetsplitDetectorData; };

	///
	/// Returns a pointer to the KviIrcConnectionAsyncWhoisData object
	/// which contains data private to the async whois engine. Very similar
	/// to KviIrcConnectionStateData but dedicated to async whois.
	/// The returned pointer is never NULL.
	/// Use #include "kvi_ircconnectionasyncwhoisdata.h" as the class is only forwarded here.
	///
	inline KviIrcConnectionAsyncWhoisData * asyncWhoisData()
		{ return m_pAsyncWhoisData; };

	///
	/// Returns a pointer to the KviIrcConnectionStatistics object
	/// which contains runtime statistics about the connection. Very similar
	/// to KviIrcConnectionStateData but dedicated to statistics.
	/// The returned pointer is never NULL.
	/// Use #include "kvi_ircconnectionstatistics.h" as the class is only forwarded here.
	///
	inline KviIrcConnectionStatistics * statistics()
		{ return m_pStatistics; };

	///
	/// Returns a pointer to the current KviNotifyListManager.
	/// The returned pointer is NULL if notify list management is disabled
	/// for the current connection.
	///
	inline KviNotifyListManager * notifyListManager()
		{ return m_pNotifyListManager; };

	///
	/// Returns a pointer to the current KviLagMeter.
	/// The returned pointer is NULL if lag measurement is disabled
	/// for the current connection.
	///
	inline KviLagMeter * lagMeter()
		{ return m_pLagMeter; };

	///
	/// Helper that provides a shortcut for really common access to userInfo()->nickName()
	///
	inline const QString & currentNickName();

	///
	/// Helper that provides a shortcut for really common access to userInfo()->userName()
	///
	inline const QString & currentUserName();

	///
	/// Helper that provides a shortcut for really common access to serverInfo()->name()
	///
	inline const QString & currentServerName();

	//
	// channel management
	//
	// FIXME: Delegate to a KviChannelManager
	//

	///
	/// Finds the channel with the specified unicode name. Returns the
	/// pointer to the channel found or NULL if there is no such channel.
	///
	KviChannel * findChannel(const QString &name);
	
	///
	/// Returns the list of the channels bound to the current connection.
	/// The pointer itself is never null (though the list may be empty).
	///
	inline KviPointerList<KviChannel> * channelList()
		{ return m_pChannelList; };

	///
	/// Returns a list of channels bound to the current connection that the user
	/// with the specified nickname is on. This is actually the list of channels
	/// that the local user has in common with the specified user.
	/// The list is returned as a comma separated string. If bAddEscapeSequences
	/// is true then the string includes special escape sequences for KviIrcView
	/// that will make the channel names clickable.
	///
	/// Returns the number of channels found.
	///
	int getCommonChannels(const QString &nick,QString &szChansBuffer,bool bAddEscapeSequences = true);
	
	///
	/// Creates a new channel with the specified name. This should be called
	/// in response to a JOIN message.
	/// This function _assumes_ that such a channel doesn't exist yet (or if it
	/// exists then it's actually in DEAD state). You can assume that channel
	/// creation never fails: if the returned pointer is NULL then we're screwed
	/// anyway as virtual memory is exausted.
	///
	KviChannel * createChannel(const QString &name);
	
	///
	/// This is called by KviChannel upon creation. You shouldn't need to call it.
	///
	/// FIXME: Could be made protected.
	///
	void registerChannel(KviChannel *c);

	///
	/// This is called by KviChannel just before destruction. You shouldn't need to call it.
	///
	/// FIXME: Could be made protected.
	///
	void unregisterChannel(KviChannel *c);

	///
	/// Marks all the currently existing channels as DEAD so they are preserved
	/// across connections. Marking a channel as dead actually removes the ownership
	/// of KviChannel from KviIrcConnection and moves it to KviIrcContext (as the
	/// contex is preserved across connections).
	///
	void keepChannelsOpenAfterDisconnect();
	
	///
	/// Closes all the currently open channels (that aren't marked as dead).
	///
	void closeAllChannels();
	
	//
	// query management
	//
	// FIXME: Delegate to a KviQueryManager ?
	//

	///
	/// Finds the query with the specified nick. Returns the
	/// pointer to the query found or NULL if there is no such query.
	///
	KviQuery * findQuery(const QString &nick);

	///
	/// Returns the list of the currently open queries. The returned
	/// pointer is never NULL (the list may be empty though).
	///
	inline KviPointerList<KviQuery> * queryList(){ return m_pQueryList; };

	///
	/// Creates a query with the specified nick as target. This function
	/// assumes that such a query doesn't exist yet (or if it exists
	/// it's actually marked as dead and needs to be resurrected).
	///
	KviQuery * createQuery(const QString &nick);

	///
	/// This is called by KviQuery upon creation, you shouldn't need to use it.
	///
	/// FIXME: Could be made protected.
	///
	void registerQuery(KviQuery *q);

	///
	/// This is called by KviQuery just before destruction, you shouldn't need to use it.
	///
	/// FIXME: Could be made protected.
	///
	void unregisterQuery(KviQuery *q);
	
	///
	/// Marks all the currently open queries as dead in order to keep them
	/// open after a disconnect. Marking a query as dead removes ownership from
	/// this class and gives it to KviIrcContext (which survives disconnects).
	///
	void keepQueriesOpenAfterDisconnect();
	
	///
	/// This is the inverse of keepQueriesOpenAfterDisconnect().
	/// It's meant to be called upon reconnection (as queries don't need
	/// a JOIN message to be opened).
	///
	void resurrectDeadQueries();
	
	///
	/// This function actually restarts the lag meter.
	///
	/// FIXME: Why ?
	///
	void restartLagMeter();

	//
	// Networking stack public stuff
	//

	///
	/// Sends (or rather enqueues) the specified text on the connected link (and thus socket).
	/// This function basically works like sprintf(). Please note that you should
	/// send only ENCODED text to the server. This is why the accepted parameter
	/// is a const char *. IRC is an 8 (I'd say even 7 sometimes) bit protocol.
	///
	/// Please note that you don't actually need to append a CRLF pair to your buffer:
	/// the implementation of the networking stack will do it for you. Hovewer you
	/// can send multiple IRC commands by separating them by CRLF pairs. Just take
	/// care of avoiding a CRLF after the last command you specify.
	///
	/// Returns true on success or false if the data can't be actually enqueued/sent (and
	/// this usually means that there is no connection at the moment). Please note
	/// that it MAY happen that a send attempt on a correctly connected link
	/// causes the link to actually disconnect (or rather "discover" the fact
	/// that it's disconnected at OS level).
	///
	/// \param buffer The _encoded_ text data to send
	/// \param buflen The length of the data: if -1 the length is determined via strlen().
	///
	bool sendData(const char *buffer,int buflen = -1);

	///
	/// Sends (or rather enqueues) the specified formatted text on the connected link (and thus socket).
	/// This function basically works like sprintf(). Please note that you should
	/// send only ENCODED text to the server. This is why the accepted parameter
	/// is a const char *. IRC is an 8 (I'd say even 7 sometimes) bit protocol.
	///
	/// Please note that you don't actually need to append a CRLF pair to your buffer:
	/// the implementation of the networking stack will do it for you. Hovewer you
	/// can send multiple IRC commands by separating them by CRLF pairs. Just take
	/// care of avoiding a CRLF after the last command you specify.
	///
	/// Returns true on success or false if the data can't be actually enqueued/sent (and
	/// this usually means that there is no connection at the moment). Please note
	/// that it MAY happen that a send attempt on a correctly connected link
	/// causes the link to actually disconnect (or rather "discover" the fact
	/// that it's disconnected at OS level).
	///
	/// \param fmt The format string to be first sprintf'ed with the variadic params and then sent
	/// \param ... The variadic arguments (see man sprintf for an explaination)
	///
	/// FIXME: change name to sendFormattedData() ?
	///
	bool sendFmtData(const char *fmt,...);

	///
	/// This function is part of the networking stack.
	/// It's called by KviIrcLink OR KviMexLinkFilter when a message is arriving
	/// from the server. Needs to be public because subclasses of KviMexLinkFilter may call it.
	///
	void incomingMessage(const char * message);

	//
	// Encoding related stuff
	//
	
	///
	/// Returns a pointer to the current global codec for outbound text.
	/// This codec is a global setting and may be overridden by specific
	/// windows. The returned pointer may be null if things really went
	/// wrong.
	///
	inline QTextCodec * textCodec()
		{ return m_pTextCodec; };

	///
	/// Returns a pointer to the current global codec for inbound data.
	/// This codec is a global setting and may be overridden by specific
	/// windows. The returned pointer may be null if things really went
	/// wrong.
	///
	/// FIXME: The name of this function is ugly.
	///
	inline QTextCodec * srvCodec()
		{ return m_pSrvCodec; };
	///
	/// Sets the global encoding for this connection. This is a default
	/// value and can be overridden by specific windows.
	///
	/// FIXME: This is quite unclear (why it forces encoding to all the windows?)
	///
	void setEncoding(const QString &szEncoding);

	///
	/// Decodes the specified text by using the server codec.
	/// You should first attempt to decode the text by using
	/// the window codec (if you belong to a window).
	///
	QString decodeText(const char * szText);
	
	///
	/// Encodes the specified text by using the local text codec.
	/// You should first attempt to encode the text by using
	/// the window codec (if you belong to a window).
	///
	KviQCString encodeText(const QString &szText);

public slots:
	void partAllChannels();
	void unhighlightAllChannels();
	void unhighlightAllQueries();
	void restartNotifyList();
	void closeAllQueries();
protected:
	//
	// notify list management
	//
	void delayedStartNotifyList();
	//
	// login operations
	//
	void resolveLocalHost();
	void checkCapSupport();
	void closeCap();
#ifdef COMPILE_SSL_SUPPORT
	void trySTARTTLS();
	void enableStartTlsSupport(bool bEnable);
#endif
	void loginToIrcServer();
	//
	// KviServerParser interface
	//
	// called to start the notify lists when RPL_ENDOFMOTD is received
	void endOfMotdReceived();
	// called when RPL_SERVINFO (004) is received
	void serverInfoReceived(const QString &szServerName,const QString &szUserModes,const QString &szChanModes);
	// called to update the away state
	void changeAwayState(bool bAway);
	// called on each JOIN (but meaningful almost only on the first one) (FIXME: call it also on other messages ?)
	void userInfoReceived(const QString &szUserName,const QString &szHostName);
	// called when NICK is received
	void nickChange(const QString &szNewNick);
	// called when MODE is received for each mode character
	bool changeUserMode(char mode,bool bSet);
	// called on the first numerics received from server
	void loginComplete(const QString &szNickName);
	//
	// KviIrcContext interface
	//
	// this starts an asynchronous connection attempt
	// you must return control to the qt core for the connection
	// operations to be initiated
	void start();
	// kills any running connection or attempt
	void abort();
	// called once in a while (every 5 secs at the moment)
	void heartbeat(kvi_time_t tNow);
	//
	// KviIrcLink interface (down)
	//
	// this signals that the connection attempt has failed
	void linkAttemptFailed(int iError);
	// called by KviIrcLink when the socket state changes to Connected
	void linkEstabilished();
	// called by KviIrcLink when the socket state changes to Ready
	void linkTerminated();
signals:
	void awayStateChanged();
	void userModeChanged();
	void nickNameChanged();
	void channelRegistered(KviChannel*);
	void channelUnregistered(KviChannel*);
	void chanListChanged();
private:
	void setupTextCodec();
	void setupSrvCodec();
private slots:
	void hostNameLookupTerminated(KviDns * pDns);
};


// TODO: KviIdentity

inline const QString & KviIrcConnection::currentServerName()
{
	return serverInfo()->name();
}

inline const QString & KviIrcConnection::currentNickName()
{
	return userInfo()->nickName();
}

inline const QString & KviIrcConnection::currentUserName()
{
	return userInfo()->userName();
}

#endif //!_KVI_IRCCONNECTION_H_
