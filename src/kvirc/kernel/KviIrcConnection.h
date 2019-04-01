#ifndef _KVI_IRCCONNECTION_H_
#define _KVI_IRCCONNECTION_H_
//=============================================================================
//
//   File : KviIrcConnection.h
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
* \file KviIrcConnection.h
* \author Szymon Stefanek
* \brief An abstraction of a connection to an IRC server
*/

#include "kvi_settings.h"
#include "KviQString.h"
#include "KviTimeUtils.h"

#include <QByteArray>
#include <QObject>
#include <QStringList>

#include <memory>
#include <utility>
#include <vector>

class QTimer;
class QTextCodec;
class KviConsoleWindow;
class KviIrcNetworkData;
class KviIrcServer;
class KviProxy;
class KviIrcSocket;
class KviIrcLink;
class KviDataBuffer;
class KviIrcContext;
class KviMainWindow;
class KviChannelWindow;
class KviQueryWindow;
class KviIrcConnectionTarget;
class KviIrcUserDataBase;
class KviIrcConnectionUserInfo;
class KviIrcConnectionServerInfo;
class KviIrcConnectionStateData;
class KviIrcConnectionAntiCtcpFloodData;
class KviIrcConnectionNetsplitDetectorData;
class KviIrcConnectionAsyncWhoisData;
class KviIrcConnectionStatistics;
class KviIrcConnectionRequestQueue;
class KviLagMeter;
class KviNotifyListManager;
class KviDnsResolver;
class KviUserIdentity;
class KviIdentityProfileSet;
class KviCString;

/**
* \class KviIrcConnection
* \brief An abstraction of a connection to an IRC server
*
* This class deals with the high-level logic of a connection to an IRC server.
* It's always attached to a KviIrcContext and thus to a KviConsoleWindow window.
* The connection has a KviIrcConnectionTarget object which specifies the
* data of the server that needs to be contacted and a KviUserIdentity object
* which specifies the user data to use.
*
* The connection is the upper layer of the KVIrc networking stack. It owns
* KviIrcLink which is the layer immediately below and which in turn contains
* a KviIrcSocket.
*
* The connection creates and manages a lot of objects that handle
* runtime information. These objects include a KviIrcConnectionUserInfo which
* contains the ACTUAL user information, KviIrcConnectionServerInfo which
* contains the ACTUAL server information, KviIrcUserDataBase which is
* substantially a big hash table containing all the users visible to KVIrc in
* this connection, a list of channels, a list of queries etc...
*/
class KVIRC_API KviIrcConnection : public QObject
{
	friend class KviConsoleWindow;
	friend class KviIrcContext;
	friend class KviIrcLink;
	friend class KviIrcServerParser;
	Q_OBJECT
protected:
	/**
	* \brief Creates a KviIrcConnection object.
	*
	* This is actually used only by KviConsoleWindow.
	*
	* pContext must not be nullptr and is kept as shallow pointer (that is, it's
	* not owned and must persists for the entire life of KviIrcConnection:
	* caller is responsable for that).
	*
	* pTarget must not be nullptr and must be allocated with new as this class
	* takes the ownership.
	*
	* pIdentity must not be nullptr and must be allocated with new as this class
	* takes the ownership.
	* \param pContext The KviIrcContext we're attacched to
	* \param pTarget The server data
	* \param pIdentity User information to use in this connection
	* \return KviIrcConnection
	*/
	KviIrcConnection(KviIrcContext * pContext, KviIrcConnectionTarget * pTarget, KviUserIdentity * pIdentity);

	/**
	* \brief Destroys a KviIrcConnection object. KviConsoleWindow uses this.
	*/
	~KviIrcConnection();

public:
	/**
	* \enum State
	* \brief The possible "high level" states of this connection.
	*/
	enum State
	{
		Idle,       /**< The connection is doing nothing */
		Connecting, /**< A connection attempt is in progress */
		Connected   /**< We're connected to an IRC server */
	};

private:
	KviConsoleWindow * m_pConsole; // shallow, never null
	KviIrcContext * m_pContext;    // shallow, never null

	State m_eState = Idle;
	bool m_bIdentdAttached = false;

	KviIrcConnectionTarget * m_pTarget; // owned, never null

	KviIrcLink * m_pLink; // owned, never null

	// The initial information about the user we'll send out to the server
	// Note that the ACTUAL user information are in m_pUserInfo instead
	KviUserIdentity * m_pUserIdentity; // owned, never null

	// The ACTUAL user information
	KviIrcConnectionUserInfo * m_pUserInfo; // owned, never null
	// The ACTUAL server information
	KviIrcConnectionServerInfo * m_pServerInfo; // owned, never null

	KviIrcConnectionStateData * m_pStateData; // owned, never null

	std::vector<KviChannelWindow *> m_pChannelList; // elements are borrowed and never null
	std::vector<KviQueryWindow *> m_pQueryList;     // elements are borrowed and never null

	KviIrcUserDataBase * m_pUserDataBase; // owned, never null

	KviNotifyListManager * m_pNotifyListManager = nullptr; // owned, see restartNotifyList()
	QTimer * m_pNotifyListTimer = nullptr;       // delayed startup timer for the notify lists

	KviLagMeter * m_pLagMeter = nullptr; // owned, may be null (when not running)

	KviIrcConnectionAntiCtcpFloodData * m_pAntiCtcpFloodData;       // owned, never null
	KviIrcConnectionNetsplitDetectorData * m_pNetsplitDetectorData; // owned, never null
	KviIrcConnectionAsyncWhoisData * m_pAsyncWhoisData;             // owned, never null

	std::unique_ptr<KviIrcConnectionStatistics> m_pStatistics; // owned, never null

	KviDnsResolver * m_pLocalhostDns = nullptr; // FIXME: this should go to an aux structure

	QTextCodec * m_pSrvCodec;                       // connection codec: never null
	QTextCodec * m_pTextCodec;                      // connection codec: never null
	KviIrcConnectionRequestQueue * m_pRequestQueue; // owned, never null
public:
	/**
	* \brief Returns a pointer to the owning console
	*
	* The pointer is never nullptr
	* \return KviConsoleWindow *
	*/
	KviConsoleWindow * console() const { return m_pConsole; }

	/**
	* \brief Returns a pointer to the owning KviIrcContext.
	*
	* The returned value is never nullptr
	* \return KviIrcContext *
	*/
	KviIrcContext * context() const { return m_pContext; }

	/**
	* \brief Returns the target of this connection.
	*
	* Please note that the target doesn't necessairly contain up-to-date data.
	* You might want to look at serverInfo() instead.
	* The returned pointer is never nullptr.
	* \return KviIrcConnectionTarget *
	*/
	KviIrcConnectionTarget * target() const { return m_pTarget; }

	/**
	* \brief Returns the underlying KviIrcLink object
	*
	* The returned pointer is never nullptr.
	* \return KviIrcLink *
	*/
	KviIrcLink * link() const { return m_pLink; }

	/**
	* \brief Returns the current state of the connection
	* \return State
	*/
	State state() const { return m_eState; }

	/**
	* \brief Returns a pointer to the big connection user database.
	*
	* The database contains ALL the users KVIrc can "see" in this connection.
	* The returned pointer is never nullptr.
	* \return KviIrcUserDataBase *
	*/
	KviIrcUserDataBase * userDataBase() const { return m_pUserDataBase; }

	/**
	* \brief Returns a pointer to the KviIrcConnectionUserInfo object
	*
	* It contains runtime information about the user. This includes the
	* current nickname, username, flags and other stuff that KviUserIdentity
	* actually doesn't contain (or has only "default" values for).
	* The returned pointer is never nullptr.
	* Include "KviIrcConnectionUserInfo.h" as the class is only
	* forwarded here.
	* \return KviIrcConnectionUserInfo *
	*/
	KviIrcConnectionUserInfo * userInfo() const { return m_pUserInfo; }

	/**
	* \brief Returns a pointer to the KviIrcConnectionServerInfo object
	*
	* It contains runtime information about the current server. This includes
	* the current servername, the server capabilities and other stuff that
	* KviConnectionTarget actually doesn't contain (or has only "default"
	* values for).
	* The returned pointer is never nullptr.
	* Include "KviIrcConnectionServerInfo.h" as the class is only
	* forwarded here.
	* \return KviIrcConnectionServerInfo *
	*/
	KviIrcConnectionServerInfo * serverInfo() const { return m_pServerInfo; }

	/**
	* \brief Returns a pointer to the KviIrcConnectionStateData object
	*
	* It contains _generic_ connection state data. This includes the current
	* nickname index at login time, flags that signal "micro-states" etc...
	* This data *could* be part of KviIrcConnection itself but we prefer to
	* keep it in a separate class in order to cleanup the implementation.
	* The returned pointer is never nullptr.
	* Include "KviIrcConnectionStateData.h" as the class is only
	* forwarded here.
	* \return KviIrcConnectionStateData *
	*/
	KviIrcConnectionStateData * stateData() const { return m_pStateData; }

	/**
	* \brief Returns a pointer to the KviIrcConnectionAntiCtcpFloodData object
	*
	* It contains data private to the Anti CTCP Flood engine. Very similar
	* to KviIrcConnectionStateData but dedicated to Ctcp flood.
	* The returned pointer is never nullptr.
	* Include "KviIrcConnectionAntiCtcpFloodData.h" as the class is
	* only forwarded here.
	* \return KviIrcConnectionAntiCtcpFloodData *
	*/
	KviIrcConnectionAntiCtcpFloodData * antiCtcpFloodData() const
	{
		return m_pAntiCtcpFloodData;
	}

	/**
	* \brief Returns a pointer to the KviIrcConnectionNetsplitDetectorData object
	*
	* It contains data private to the netsplit detector engine. Very similar
	* to KviIrcConnectionStateData but dedicated to netsplit detection.
	* The returned pointer is never nullptr.
	* Include "KviIrcConnectionNetsplitDetectorData.h" as the class is
	* only forwarded here.
	* \return KviIrcConnectionNetsplitDetectorData *
	*/
	KviIrcConnectionNetsplitDetectorData * netsplitDetectorData() const
	{
		return m_pNetsplitDetectorData;
	}

	/**
	* \brief Returns a pointer to the KviIrcConnectionAsyncWhoisData object
	*
	* It contains data private to the async whois engine. Very similar to
	* KviIrcConnectionStateData but dedicated to async whois.
	* The returned pointer is never nullptr.
	* Include "KviIrcConnectionAsyncWhoisData.h" as the class is only
	* forwarded here.
	* \return KviIrcConnectionAsyncWhoisData *
	*/
	KviIrcConnectionAsyncWhoisData * asyncWhoisData() const
	{
		return m_pAsyncWhoisData;
	}

	/**
	* \brief Returns a pointer to the KviIrcConnectionStatistics object
	*
	* It contains runtime statistics about the connection. Very similar to
	* KviIrcConnectionStateData but dedicated to statistics.
	* The returned pointer is never nullptr.
	* Include "KviIrcConnectionStatistics.h" as the class is only
	* forwarded here.
	* \return KviIrcConnectionStatistics *
	*/
	KviIrcConnectionStatistics * statistics() const { return m_pStatistics.get(); }

	/**
	* \brief Returns a pointer to the current KviNotifyListManager.
	*
	* The returned pointer is nullptr if notify list management is disabled for
	* the current connection.
	* \return KviNotifyListManager *
	*/
	KviNotifyListManager * notifyListManager() const
	{
		return m_pNotifyListManager;
	}

	/**
	* \brief Returns a pointer to the current KviLagMeter.
	*
	* The returned pointer is nullptr if lag measurement is disabled for the
	* current connection.
	* \return KviLagMeter *
	*/
	KviLagMeter * lagMeter() const { return m_pLagMeter; }

	/**
	* \brief Returns a pointer to the current KviIrcConnectionRequestQueue.
	* \return KviIrcConnectionRequestQueue *
	*/
	KviIrcConnectionRequestQueue * requestQueue() const { return m_pRequestQueue; }

	/**
	* \brief Returns the list of the channels bound to the current connection.
	*
	* The pointer itself is never null (though the list may be empty).
	* \return & std::vector<KviChannelWindow *>
	*/
	std::vector<KviChannelWindow *> & channelList() { return m_pChannelList; }

	/**
	* \brief Helper that provides a shortcut for really common access to serverInfo()->networkName()
	* \return const QString &
	*/
	const QString & currentNetworkName() const;

	/**
	* \brief Helper that provides a shortcut for really common access to userInfo()->nickName()
	* \return const QString &
	*/
	const QString & currentNickName() const;

	/**
	* \brief Helper that provides a shortcut for really common access to userInfo()->userName()
	* \return const QString &
	*/
	const QString & currentUserName() const;

	/**
	* \brief Helper that provides a shortcut for really common access to serverInfo()->name()
	* \return const QString &
	*/
	const QString & currentServerName() const;

	//
	// Channel management
	//
	// FIXME: Delegate to a KviChannelManager
	//

	/**
	* \brief Finds the channel with the specified unicode name.
	*
	* Returns the pointer to the channel found or nullptr if there is no such
	* channel.
	* \param szName The name of the channel
	* \return KviChannelWindow *
	*/
	KviChannelWindow * findChannel(const QString & szName);

	/**
	* \brief Returns a list of channels bound to the current connection
	*
	* This is actually the list of channels that the local user has in common
	* with the specified user.
	* The list is returned as a comma separated string. If bAddEscapeSequences
	* is true, then the string includes special escape sequences for
	* KviIrcView that will make the channel names clickable.
	*
	* Returns the number of channels found.
	* \param szNick The nickname of the user
	* \param szChansBuffer The buffer where to store data
	* \param bAddEscapeSequences Whether to include escape sequences
	* \return int
	*/
	int getCommonChannels(const QString & szNick, QString & szChansBuffer, bool bAddEscapeSequences = true);

	/**
	* \brief Creates a new channel with the specified name.
	*
	* This should be called in response to a JOIN message.
	* This function _assumes_ that such a channel doesn't exist yet (or if it
	* exists then it's actually in DEAD state). You can assume that channel
	* creation never fails: if the returned pointer is nullptr then we're screwed
	* anyway as virtual memory is exausted.
	* \param szName The name of the channel
	* \return KviChannelWindow *
	*/
	KviChannelWindow * createChannel(const QString & szName);

	///
	/// This is called by KviChannelWindow upon creation. You shouldn't need to call it.
	///
	/// FIXME: Could be made protected.
	///
	void registerChannel(KviChannelWindow * c);

	///
	/// This is called by KviChannelWindow just before destruction. You shouldn't need to call it.
	///
	/// FIXME: Could be made protected.
	///
	void unregisterChannel(KviChannelWindow * c);

	/**
	* \brief Marks all the currently existing channels as DEAD
	*
	* They are preserved across connections. Marking a channel as dead
	* actually removes the ownership of KviChannelWindow from KviIrcConnection and
	* moves it to KviIrcContext (as the contex is preserved across
	* connections).
	* \return void
	*/
	void keepChannelsOpenAfterDisconnect();

	/**
	* \brief Closes all the currently open channels not marked as dead.
	* \return void
	*/
	void closeAllChannels();

	/**
	* \brief Closes all the currently open queries not marked as dead.
	* \return void
	*/
	void closeAllQueries();

	//
	// Query management
	//
	// FIXME: Delegate to a KviQueryManager ?
	//

	/**
	* \brief Finds the query with the specified nick.
	*
	* Returns the pointer to the query found or nullptr if there is no such
	* query.
	* \param szNick The nickname of the user
	* \return KviQueryWindow *
	*/
	KviQueryWindow * findQuery(const QString & szNick);

	/**
	* \brief Returns the list of the currently open queries.
	*
	* The returned pointer is never nullptr (the list may be empty though).
	* \return std::vector<KviQueryWindow *> &
	*/
	std::vector<KviQueryWindow *> & queryList() { return m_pQueryList; }

	///
	/// Visibility mode for createQuery()
	///
	enum CreateQueryVisibilityMode
	{
		///
		/// Create minimized query, overriding settings
		///
		CreateQueryVisibilityMinimized,
		///
		/// Create visible query, overriding settings
		///
		CreateQueryVisibilityVisible,
		///
		/// Follow global settings.
		///
		CreateQueryVisibilityFollowSettings
	};

	/**
	* \brief Creates a query with the specified nick as target.
	*
	* This function assumes that such a query doesn't exist yet (or if it
	* exists it's actually marked as dead and needs to be resurrected).
	* \param szNick The nickname of the user
	* \param eShowMode Specifies the show mode for the window
	* \return KviQueryWindow *
	*/
	KviQueryWindow * createQuery(const QString & szNick, CreateQueryVisibilityMode eVisibilityMode = CreateQueryVisibilityFollowSettings);

	///
	/// This is called by KviQueryWindow upon creation, you shouldn't need to use it.
	///
	/// FIXME: Could be made protected.
	///
	void registerQuery(KviQueryWindow * q);

	///
	/// This is called by KviQueryWindow just before destruction, you shouldn't need to use it.
	///
	/// FIXME: Could be made protected.
	///
	void unregisterQuery(KviQueryWindow * q);

	/**
	* \brief Marks all the currently open queries as DEAD
	*
	* They are kept open after a disconnect. Marking a query as dead removes
	* ownership from this class and gives it to KviIrcContext (which survives
	* disconnects).
	* \return void
	*/
	void keepQueriesOpenAfterDisconnect();

	/**
	* \brief This is the inverse of keepQueriesOpenAfterDisconnect().
	*
	* It's meant to be called upon reconnection (as queries don't need a JOIN
	* message to be opened).
	* \return void
	*/
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

	/**
	* \brief Sends the specified text on the connected link (and thus socket).
	*
	* This function basically works like sprintf(). Please note that you
	* should send only ENCODED text to the server. This is why the accepted
	* parameter is a const char *. IRC is an 8 (I'd say even 7 sometimes) bit
	* protocol.
	*
	* Please note that you don't actually need to append a CRLF pair to your
	* buffer:
	* the implementation of the networking stack will do it for you. Hovewer
	* you can send multiple IRC commands by separating them by CRLF pairs.
	* Just take care of avoiding a CRLF after the last command you specify.
	*
	* Returns true on success or false if the data can't be actually
	* enqueued/sent (and this usually means that there is no connection at
	* the moment). Please note that it MAY happen that a send attempt on a
	* correctly connected link causes the link to actually disconnect (or
	* rather "discover" the fact that it's disconnected at OS level).
	* \param pcBuffer The _encoded_ text data to send
	* \param iBuflen The length of the data: if -1 the length is determined via strlen().
	* \return bool
	*/
	bool sendData(const char * pcBuffer, int iBuflen = -1);

	/**
	* \brief Sends the specified text on the connected link (and thus socket).
	*
	* This function basically works like sprintf(). Please note that you
	* should send only ENCODED text to the server. This is why the accepted
	* parameter is a const char *. IRC is an 8 (I'd say even 7 sometimes) bit
	* protocol.
	*
	* Please note that you don't actually need to append a CRLF pair to your
	* buffer:
	* the implementation of the networking stack will do it for you. Hovewer
	* you can send multiple IRC commands by separating them by CRLF pairs.
	* Just take care of avoiding a CRLF after the last command you specify.
	*
	* Returns true on success or false if the data can't be actually
	* enqueued/sent (and this usually means that there is no connection at
	* the moment). Please note that it MAY happen that a send attempt on a
	* correctly connected link causes the link to actually disconnect (or
	* rather "discover" the fact that it's disconnected at OS level).
	* \param pcFmt The format string to be first sprintf'ed with the variadic params and then sent
	* \param ... The variadic arguments (see man sprintf for an explanation)
	* \return bool
	*
	* FIXME: change name to sendFormattedData() ?
	*/
	bool sendFmtData(const char * pcFmt, ...);

	/**
	* Clears the underlying output queue.
	* Exposed basically for /context.clearQueue
	* You shouldn't need it for any other reason :)
	*/
	void clearOutputQueue(bool bPrivateMessagesOnly);

	/**
	* \brief Returns the current size of the output queue.
	* \return unsigned int
	*/
	unsigned int outputQueueSize();

	/**
	* \brief This function is part of the networking stack.
	*
	* It's called by KviIrcLink OR KviMexLinkFilter when a message is arriving
	* from the server. Needs to be public because subclasses of
	* KviMexLinkFilter may call it.
	* \param pcMessage The message :)
	* \return void
	*/
	void incomingMessage(const char * pcMessage);

	/**
	* \brief This function is part of the networking stack.
	*
	* It's called by KviIrcDataStreamMonitor subclasses when they want to
	* inject some message without getting it filtered again
	* \param pcMessage The message :)
	* \return void
	*/
	void incomingMessageNoFilter(const char * pcMessage);

	//
	// Encoding related stuff
	//

	/**
	* \brief Returns a pointer to the current global codec for outbound text.
	*
	* This codec is a global setting and may be overridden by specific
	* windows. The returned pointer may be null if things really went wrong.
	* \return QTextCodec *
	*/
	QTextCodec * textCodec() const { return m_pTextCodec; }

	/**
	* \brief Returns a pointer to the current global codec for inbound data.
	*
	* This codec is a global setting and may be overridden by specific
	* windows. The returned pointer may be null if things really went wrong.
	* \return QTextCodec *
	*/
	QTextCodec * serverCodec() const { return m_pSrvCodec; }

	/**
	* \brief Sets the global encoding for this connection.
	*
	* This is a default value and can be overridden by specific windows.
	* This is tipically called upon a successful CODEPAGE change (numerical 222)
	* response from a server; from now on, the server will translate (encode)
	* everything (including IDN hostnames, channel names, messages) using this
	* encoding. So we expect this encoding to be the new default for the server
	* (connection), the console, each other channel and query.
	* \param szEncoding The encoding to use
	* \return void
	*/
	void setEncoding(const QString & szEncoding);

	/**
	* \brief Decodes the specified text by using the server codec.
	*
	* You should first attempt to decode the text by using the window codec
	* (if you belong to a window).
	* \param szText The text to decode
	* \return QString
	*/
	QString decodeText(const char * szText);

	/**
	* \brief Encodes the specified text by using the local text codec.
	*
	* You should first attempt to encode the text by using the window codec
	* (if you belong to a window).
	* \param szText The text to encode
	* \return QByteArray
	*/
	QByteArray encodeText(const QString & szText);

protected:
	//
	// Notify list management
	//

	/**
	* \brief Starts the notify list in 15 seconds
	*
	* We have this delay to wait an eventual RPL_PROTOCTL from the server
	* telling us that the WATCH notify list method is supported
	* \return void
	*/
	void delayedStartNotifyList();

	//
	// Login operations
	//

	/**
	* \brief Resolves the localhost into an IP address
	* \return void
	*/
	void resolveLocalHost();

#ifdef COMPILE_SSL_SUPPORT
	void handleFailedInitialStartTls();
	bool trySTARTTLS(bool bAppendPing);
	void enableStartTlsSupport(bool bEnable);
#endif

	/**
	* \brief Uses the profiles' data as connection data
	*
	* It checks if the identity profiles are enabled and updates the data
	* of the connection if a profile rule matches the network name
	* \param pSet The instance of the network identity profile set
	* \param szNetwork The name of the network
	* \return void
	*/
	void useProfileData(KviIdentityProfileSet * pSet, const QString & szNetwork);

	/**
	* Sets the specified real name in the user information set after
	* evaluating it via KVS engine (so identifiers are substituted).
	* Call this function instead of m_pUserInfo->setRealName().
	*/
	void useRealName(const QString & szRealName);

	/**
	* \brief Logins to the irc server
	* \return void
	*/
	void loginToIrcServer();

	/**
	* Joins a list of channels.
	* The first element of the pair is the channel name, the second element of the pair is the eventual password.
	* May send multiple JOIN messages.
	*/
	void joinChannels(const std::vector<std::pair<QString, QString>> & lChannelsAndPasses);

	/**
	* Gather the list of currently joined channels with the relative passwords.
	*/
	void gatherChannelAndPasswordPairs(std::vector<std::pair<QString, QString>> & lChannelsAndPasses);

	/**
	* Gather the list of currently open query names.
	*/
	void gatherQueryNames(QStringList & lQueryNames);

	/**
	* Picks the next login nickname to be tried. If bForceDefaultIfPrimaryNicknamesEmpty is true then the algorithm
	* will return a default nickname in case the primary setting in the options is empty. If bForceDefaultIfPrimaryNicknamesEmpty
	* is false then the algorithm will try the alternative nicknames instead, then try some random options and finally
	* return an empty string signaling that no suitable choice could be made. bForceDefaultIfPrimaryNicknamesEmpty should
	* be set to true only in loginToIrcServer(), that is, the first time this function is called (otherwise there exists
	* a possibility of infinite nickname loop because of the repeated fallback choice). szBaseNickForRandomChoices will be
	* used for the random alternatives as the first left 7 characters. Upon return szChoiceDescriptionBuffer will contain
	* the textual description of the choice made (network specific, alternative, server specific, global etc...).
	*/
	QString pickNextLoginNickName(bool bForceDefaultIfPrimaryNicknamesEmpty, const QString & szBaseNickForRandomChoices, QString & szChoiceDescriptionBuffer);

	//
	// KviIrcServerParser interface
	//

	/**
	* \brief Called to start the notify lists when RPL_ENDOFMOTD is received
	* \return void
	*/
	void endOfMotdReceived();

	/**
	* \brief Called when RPL_SERVINFO (004) is received and sets the info
	* \param szServerName The name of the server
	* \param szUserModes The user modes supported by the server
	* \param szChanModes The chan modes supported by the server
	* \return void
	*/
	void serverInfoReceived(const QString & szServerName, const QString & szUserModes, const QString & szChanModes);

	/**
	* \brief Called when AUTHENTICATE answer is received
	* \return void
	*/
	void handleAuthenticate(KviCString & szResponse);

	/**
	* \brief Called when CAP LS answer is received
	* \return void
	*/
	void handleInitialCapLs();

	/**
	* \brief Called when CAP LS negotiation fails
	* \return void
	*/
	void handleFailedInitialCapLs();

	/**
	* \brief Called when CAP ACK answer is received
	* \return void
	*/
	void handleInitialCapAck();

	/**
	* \brief Called when CAP NAK answer is received
	* \return void
	*/
	void handleInitialCapNak();

	/**
	* \brief Called when we want to end CAP negotiation and go on wi the login
	* \return void
	*/
	void endInitialCapNegotiation();

	/**
	* \brief Called to update the away state
	* \param bAway Whether we enter the away mode
	* \return void
	*/
	void changeAwayState(bool bAway);

	/**
	* \brief Called on each JOIN and sets the info
	*
	* It's called meaningful almost only on the first time
	* \param szUserName The username
	* \param szHostName The hostname
	* \return void
	* FIXME: call it also on other messages ?
	*/
	void userInfoReceived(const QString & szUserName, const QString & szHostName);

	/**
	* \brief Called when NICK is received
	* \param szNewNick The new nickname for the user
	* \return void
	*/
	void nickChange(const QString & szNewNick);

	/**
	* \brief Called when MODE is received for each mode character
	* \param cMode The mode flag :)
	* \param bSet Whether to set the mode
	* \return bool
	*/
	bool changeUserMode(char cMode, bool bSet);

	/**
	* \brief Called on the first numeric received from server
	* \param szNickName The nickname to use
	* \return void
	*/
	void loginComplete(const QString & szNickName);

	//
	// KviIrcContext interface
	//

	/**
	* \brief This starts an asynchronous connection attempt
	*
	* You must return control to the Qt core for the connection operations to
	* be initiated
	* \return void
	*/
	void start();

	/**
	* \brief Kills any running connection or attempt
	* \return void
	*/
	void abort();

	/**
	* \brief Called once in a while (every 5 secs at the moment)
	* \param tNow The current time
	* \return void
	*/
	void heartbeat(kvi_time_t tNow);

	//
	// KviIrcLink interface (down)
	//

	/**
	* \brief This signals that the connection attempt has failed
	* \param iError The error code
	* \return void
	*/
	void linkAttemptFailed(int iError);

	/**
	* \brief Called by KviIrcLink when the socket state changes to Connected
	* \return void
	*/
	void linkEstablished();

	/**
	* \brief Called by KviIrcLink when the socket state changes to Ready
	* \return void
	*/
	void linkTerminated();

private:
	/**
	* \brief Setups the codec for the text
	* \return void
	*/
	void setupTextCodec();

	/**
	* \brief Setups the codec for the server
	* \return void
	*/
	void setupSrvCodec();
public slots:
	/**
	* \brief Called when we unhighlight all channels
	* \return void
	*/
	void unhighlightAllChannels();

	/**
	* \brief Called when we unhighlight all queries
	* \return void
	*/
	void unhighlightAllQueries();

	/**
	* \brief Called to restart the notify list
	* \return void
	*/
	void restartNotifyList();
private slots:
	/**
	* \brief Called when the hostname lookup is finished
	* \return void
	*/
	void hostNameLookupTerminated(KviDnsResolver * pDns);
signals:
	/**
	* \brief Emitted when the away state changes
	* \return void
	*/
	void awayStateChanged();

	/**
	* \brief Emitted when the user modes change
	* \return void
	*/
	void userModeChanged();

	/**
	* \brief Emitted when the nickname changes
	* \return void
	*/
	void nickNameChanged();

	/**
	* \brief Emitted when we register a channel
	* \param pChan The channel :)
	* \return void
	*/
	void channelRegistered(KviChannelWindow * pChan);

	/**
	* \brief Emitted when we unregister a channel
	* \param pChan The channel :)
	* \return void
	*/
	void channelUnregistered(KviChannelWindow * pChan);

	/**
	* \brief Emitted when the channels list change
	* \return void
	*/
	void chanListChanged();
};

// TODO: KviIdentity

#endif //_KVI_IRCCONNECTION_H_
