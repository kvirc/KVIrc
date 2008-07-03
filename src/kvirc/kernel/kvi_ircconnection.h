#ifndef _KVI_IRCCONNECTION_H_
#define _KVI_IRCCONNECTION_H_
//=============================================================================
//
//   File : kvi_ircconnection.h
//   Created on Mon 03 May 2004 01:45:42 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004 Szymon Stefanek <pragma at kvirc dot net>
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


class KVIRC_API KviIrcConnection : public QObject
{
	friend class KviConsole;
	friend class KviIrcContext;
	friend class KviIrcLink;
	friend class KviServerParser;
	Q_OBJECT
public:
	// only KviConsole can create this
	// pContext must NOT be null
	// pTarget must NOT be null and must be allocated with new! this class takes the ownership
	KviIrcConnection(KviIrcContext * pContext,KviIrcConnectionTarget * pTarget,KviUserIdentity * pIdentity);
	~KviIrcConnection();
public:
	enum State { Idle, Connecting, Connected };
private:
	KviFrame                             * m_pFrame;                // shallow, never null
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

	KviPointerList<KviChannel>               * m_pChannelList;          // owned, never null, elements shallow
	KviPointerList<KviQuery>                 * m_pQueryList;            // owned, never null, elements shallow

	KviIrcUserDataBase                   * m_pUserDataBase;         // owned, never null
	
	KviNotifyListManager                 * m_pNotifyListManager;    // owned, see restartNotifyList()
	QTimer                               * m_pNotifyListTimer;      // delayed startup timer for the notify lists

	KviLagMeter                          * m_pLagMeter;             // owned, may be null (when not running)

	KviIrcConnectionAntiCtcpFloodData    * m_pAntiCtcpFloodData;    // owned, never null
	KviIrcConnectionNetsplitDetectorData * m_pNetsplitDetectorData; // owned, never null
	KviIrcConnectionAsyncWhoisData       * m_pAsyncWhoisData;       // owned, never null
	
	KviIrcConnectionStatistics           * m_pStatistics;           // owned, never null

	KviDns                               * m_pLocalhostDns;         // FIXME: this should go to an aux structure
	
	QTextCodec                           * m_pTextCodec;            // connection codec: never null
public:
	// returns a pointer to the owning console
	// the pointer is NEVER null
	KviConsole * console(){ return m_pConsole; };
	// never null
	KviFrame * frame(){ return m_pFrame; };
	// never null
	KviIrcContext * context(){ return m_pContext; };
	// forwarder from KviIrcConnectionTarget, never null
	KVI_DEPRECATED KviServer * server();
	// forwarder from KviIrcConnectionTarget, may be null
	KVI_DEPRECATED KviProxy * proxy();
	// never null!
	KviIrcConnectionTarget * target(){ return m_pTarget; };
	// never null!
	KviIrcLink * link(){ return m_pLink; };
	// this should not be used...
	KviIrcSocket * socket() KVI_DEPRECATED;
	// the current state
	State state(){ return m_eState; };
	// forwarder from KviIrcConnectionTarget, always non-empty string
	const QString & networkName();
	// never null!
	KviIrcUserDataBase * userDataBase(){ return m_pUserDataBase; };
	// never null. Use #include "kvi_ircconnectionuserinfo.h"
	KviIrcConnectionUserInfo * userInfo(){ return m_pUserInfo; };
	// never null. Use #include "kvi_ircconnectionserverinfo.h"
	KviIrcConnectionServerInfo * serverInfo(){ return m_pServerInfo; };
	// never null. Use #include "kvi_ircconnectionstatedata.h"
	KviIrcConnectionStateData * stateData(){ return m_pStateData; };
	// never null. Use #include "kvi_ircconnectionantictcpflooddata.h"
	KviIrcConnectionAntiCtcpFloodData * antiCtcpFloodData(){ return m_pAntiCtcpFloodData; };
	// never null. Use #include "kvi_ircconnectionnetsplitdetectordata.h" 
	KviIrcConnectionNetsplitDetectorData * netsplitDetectorData(){ return m_pNetsplitDetectorData; };
	// never null. Use #include "kvi_ircconnectionasyncwhoisdata.h"
	KviIrcConnectionAsyncWhoisData * asyncWhoisData(){ return m_pAsyncWhoisData; };
	// never null. Use #include "kvi_ircconnectionstatistics.h"
	KviIrcConnectionStatistics * statistics(){ return m_pStatistics; };
	// may be null
	KviNotifyListManager * notifyListManager(){ return m_pNotifyListManager; };
	// may be null (when not running)
	KviLagMeter * lagMeter(){ return m_pLagMeter; };
	// should be never null.. but if really everything goes wrong, it might be...
	QTextCodec * textCodec(){ return m_pTextCodec; };
public:
	// helper (really common access to userInfo()->nickName())
	const QString & currentNickName();
	// helper (really common access to userInfo()->userName())
	const QString & currentUserName();
	// helper (really common access to serverInfo()->name())
	const QString & currentServerName();

	// world interface: the following three functions use m_pLink->sendPacket()
	bool sendData(const char *buffer,int buflen = -1);
	bool sendFmtData(const char *fmt,...);
	//
	// channel management
	//
	// ----> KviChannelManager ?
	KviChannel * findChannel(const QString &name);
	KviPointerList<KviChannel> * channelList(){ return m_pChannelList; };
	int getCommonChannels(const QString &nick,QString &szChansBuffer,bool bAddEscapeSequences = true);
	KviChannel * createChannel(const QString &name);
	void registerChannel(KviChannel *c);
	void unregisterChannel(KviChannel *c);
	void keepChannelsOpenAfterDisconnect();
	void closeAllChannels();
	void setEncoding(const QString &szEncoding);
public slots:
	void partAllChannels();
	void unhighlightAllChannels();
	void unhighlightAllQueries();
	void restartNotifyList();
	void closeAllQueries();
public:
	//
	// query management
	//
	// ----> KviQueryManager ?
	KviQuery * findQuery(const QString &nick);
	KviPointerList<KviQuery> * queryList(){ return m_pQueryList; };
	KviQuery * createQuery(const QString &nick);
	void registerQuery(KviQuery *q);
	void unregisterQuery(KviQuery *q);
	void keepQueriesOpenAfterDisconnect();
	void resurrectDeadQueries();
	void restartLagMeter();
protected:
	//
	// notify list management
	//
	void delayedStartNotifyList();
	//
	// login operations
	//
	void resolveLocalHost();
	void loginToIrcServer();
protected:
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
private slots:
	void hostNameLookupTerminated(KviDns * pDns);
protected:
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
protected:
	//
	// KviIrcLink interface (down)
	//
	// this signals that the connection attempt has failed
	void linkAttemptFailed(int iError);
	// called by KviIrcLink when the socket state changes to Connected
	void linkEstabilished();
	// called by KviIrcLink when the socket state changes to Ready
	void linkTerminated();
public:
	// this is called by KviIrcLink OR KviMexLinkFilter when a message is arriving from the server
	// needs to be public because subclasses of KviMexLinkFilter may call it.
	void incomingMessage(const char * message);
signals:
	void awayStateChanged();
	void userModeChanged();
	void nickNameChanged();
	void channelRegistered(KviChannel*);
	void channelUnregistered(KviChannel*);
	void chanListChanged();
public:
	QString decodeText(const char * szText);
	KviQCString encodeText(const QString &szText);
private:
	void setupTextCodec();
};


// TODO: KviIdentity


#endif //!_KVI_IRCCONNECTION_H_

