#ifndef _KVI_IRCCONNECTIONSTATEDATA_H_
#define _KVI_IRCCONNECTIONSTATEDATA_H_
//=============================================================================
//
//   File : kvi_ircconnectionstatedata.h
//   Creation date : Sat 26 Jun 2004 09:31:52 by Szymon Stefanek
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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include "kvi_settings.h"
#include "kvi_time.h"
#include "kvi_qstring.h"

#include <QStringList>

//
// This class is used as container for miscelaneous connection state variables
// that do not have a class for their own.
// If you need to add some "minor" variable to the connection state then
// this is the right place for it.
//

class KVIRC_API KviIrcConnectionStateData
{
	friend class KviIrcConnection;
	friend class KviServerParser;
	friend class KviUserParser;
public:
	KviIrcConnectionStateData();
	~KviIrcConnectionStateData();
protected:
	bool         m_bInsideInitialCapLs;                  // true if there's a CAP LS request pending
	bool         m_bIgnoreOneYouHaveNotRegisteredError; // true if we have sent a CAP LS request followed by a PING which will generate an error (and we need to ignore it)
	bool         m_bInsideInitialCapReq;                 // true if there's a CAP REQ request pending
	bool         m_bInsideAuthenticate;           // true if there's a AUTHENTICATE request pending
	bool         m_bSentStartTls;                 // the state of STARTTLS protocol
	bool         m_bSentQuit;                     // have we sent the quit message for this connection ?
	unsigned int m_uLoginNickIndex;               // the index of the identity nicknames used until now (see KviIrcConnection::loginToIrcServer())
	QString      m_szCommandToExecAfterConnect;   // yes.. this is a special command to execute after connection
	bool         m_bSimulateUnexpectedDisconnect; // this is set to true if we have to simulate an unexpected disconnect even if we have sent a normal quit message
	kvi_time_t   m_tLastReceivedChannelWhoReply;  // the time that we have received our last channel who reply
	kvi_time_t   m_tLastSentChannelWhoRequest;    // the time that we have sent our last channel who request
	kvi_time_t   m_tLastReceivedWhoisReply;       // the time that we have received the last whois reply, reset to 0 when we receive an /END OF WHOIS
	QStringList  m_lEnabledCaps;                  // the CAPs currently enabled
	bool         m_bIdentifyMsgCapabilityEnabled; // do we have the msg-identity CAP enabled ?
public:
	const QStringList & enabledCaps(){ return m_lEnabledCaps; };
	void changeEnabledCapList(const QString &szCapList);

	bool identifyMsgCapabilityEnabled() const
		{ return m_bIdentifyMsgCapabilityEnabled; };

	bool sentStartTls(){ return m_bSentStartTls; };
	void setSentStartTls(){ m_bSentStartTls = true; };

	bool isInsideAuthenticate(){ return m_bInsideAuthenticate; };
	void setInsideAuthenticate(bool bInside){ m_bInsideAuthenticate = bInside; };

	bool isInsideInitialCapLs(){ return m_bInsideInitialCapLs; };
	void setInsideInitialCapLs(bool bInside){ m_bInsideInitialCapLs = bInside; };

	void setIgnoreOneYouHaveNotRegisteredError(bool bIgnore)
		{ m_bIgnoreOneYouHaveNotRegisteredError = bIgnore; };
	bool ignoreOneYouHaveNotRegisteredError() const
		{ return m_bIgnoreOneYouHaveNotRegisteredError; };

	bool isInsideInitialCapReq(){ return m_bInsideInitialCapReq; };
	void setInsideInitialCapReq(bool bInside){ m_bInsideInitialCapReq = bInside; };

	bool sentQuit(){ return m_bSentQuit; };
	void setSentQuit(){ m_bSentQuit = true; };

	kvi_time_t lastReceivedChannelWhoReply(){ return m_tLastReceivedChannelWhoReply; };
	void setLastReceivedChannelWhoReply(kvi_time_t tTime){ m_tLastReceivedChannelWhoReply = tTime; };

	kvi_time_t lastSentChannelWhoRequest(){ return m_tLastSentChannelWhoRequest; };
	void setLastSentChannelWhoRequest(kvi_time_t tTime){ m_tLastSentChannelWhoRequest = tTime; };

	kvi_time_t lastReceivedWhoisReply(){ return m_tLastReceivedWhoisReply; };
	void setLastReceivedWhoisReply(kvi_time_t tTime){ m_tLastReceivedWhoisReply = tTime; };

	bool simulateUnexpectedDisconnect(){ return m_bSimulateUnexpectedDisconnect; };
	void setSimulateUnexpectedDisconnect(){ m_bSimulateUnexpectedDisconnect = true; };

	unsigned int loginNickIndex(){ return m_uLoginNickIndex; };
	void setLoginNickIndex(unsigned int uNickIdx){ m_uLoginNickIndex = uNickIdx; };

	const QString & commandToExecAfterConnect(){ return m_szCommandToExecAfterConnect; };
	void setCommandToExecAfterConnect(const QString &szCmd){ m_szCommandToExecAfterConnect = szCmd; };
};

#endif //!_KVI_IRCCONNECTIONSTATEDATA_H_
