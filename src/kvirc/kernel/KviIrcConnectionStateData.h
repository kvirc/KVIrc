#ifndef _KVI_IRCCONNECTIONSTATEDATA_H_
#define _KVI_IRCCONNECTIONSTATEDATA_H_
//=============================================================================
//
//   File : KviIrcConnectionStateData.h
//   Creation date : Sat 26 Jun 2004 09:31:52 by Szymon Stefanek
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

#include "kvi_settings.h"
#include "KviQString.h"
#include "KviTimeUtils.h"

#include <QStringList>

//
// This class is used as container for miscellaneous connection state variables
// that do not have a class for their own.
// If you need to add some "minor" variable to the connection state then
// this is the right place for it.
//

class KVIRC_API KviIrcConnectionStateData
{
	friend class KviIrcConnection;
	friend class KviIrcServerParser;
	friend class KviUserParser;

public:
	KviIrcConnectionStateData();
	~KviIrcConnectionStateData();

protected:
	// The members of this enum must be in the correct
	// order since the nickname selection algorithm
	// uses their numeric values for comparisons.
	enum LoginNickNameState
	{
		UsedConnectionSpecificNickName,
		UsedProfileSpecificNickName,
		UsedAlternativeProfileSpecificNickName,
		UsedServerSpecificNickName,
		UsedAlternativeServerSpecificNickName,
		UsedNetworkSpecificNickName,
		UsedAlternativeNetworkSpecificNickName,
		UsedGlobalNickName1,
		UsedGlobalNickName2,
		UsedGlobalNickName3,
		UsedGlobalNickName4,
		UsedRandomNickName1,
		UsedRandomNickName2,
		UsedRandomNickName3,
		UsedRandomNickName4,
		UsedManualNickname // Given up assigning it automatically
	};

	///
	/// the current login nickname state
	///
	LoginNickNameState m_eLoginNickNameState = UsedConnectionSpecificNickName;

	bool m_bInsideInitialCapLs = false; // true if there's a CAP LS request pending
	///
	/// This is set to true if a forced STARTTLS request has been sent
	/// to the server followed by a PING. We use this flag to gracefully
	/// handle a ERR_NOTREGISTERED related to the PING if STARTTLS is not supported.
	///
	/// Note that in this case the STARTTLS support wasn't detected by a previous CAP LS
	/// (which wasn't sent at all).
	///
	bool m_bInsideInitialStartTls = false;
	bool m_bIgnoreOneYouHaveNotRegisteredError = false; // true if we have sent a CAP LS request followed by a PING which will generate an error (and we need to ignore it)
	bool m_bInsideInitialCapReq = false;        // true if there's a CAP REQ request pending
	bool m_bInsideAuthenticate = false;         // true if there's a AUTHENTICATE request pending
	bool m_bSentStartTls = false;               // the state of STARTTLS protocol
	bool m_bSentQuit = false;                   // have we sent the quit message for this connection ?
	QString m_szCommandToExecAfterConnect;      // yes.. this is a special command to execute after connection
	bool m_bSimulateUnexpectedDisconnect = false; // this is set to true if we have to simulate an unexpected disconnect even if we have sent a normal quit message
	kvi_time_t m_tLastReceivedChannelWhoReply;  // the time that we have received our last channel who reply
	kvi_time_t m_tLastSentChannelWhoRequest;    // the time that we have sent our last channel who request
	kvi_time_t m_tLastReceivedWhoisReply = 0;   // the time that we have received the last whois reply, reset to 0 when we receive an /END OF WHOIS
	QStringList m_lEnabledCaps;                 // the CAPs currently enabled
	bool m_bIdentifyMsgCapabilityEnabled = false; // do we have the msg-identity CAP enabled ?
	QString m_szSentSaslMethod;
public:
	///
	/// Sets the current login nickname state
	///
	void setLoginNickNameState(const LoginNickNameState & eLoginNickNameState)
	{
		m_eLoginNickNameState = eLoginNickNameState;
	}

	///
	/// Returns the current login nickname state
	///
	const LoginNickNameState & loginNickNameState() const
	{
		return m_eLoginNickNameState;
	}

	const QStringList & enabledCaps() const { return m_lEnabledCaps; }
	void changeEnabledCapList(const QString & szCapList);

	bool identifyMsgCapabilityEnabled() const
	{
		return m_bIdentifyMsgCapabilityEnabled;
	}

	const QString & sentSaslMethod() const { return m_szSentSaslMethod; }
	void setSentSaslMethod(const QString& szMethod) { m_szSentSaslMethod = szMethod; }

	bool sentStartTls() const { return m_bSentStartTls; }
	void setSentStartTls() { m_bSentStartTls = true; }

	bool isInsideAuthenticate() const { return m_bInsideAuthenticate; }
	void setInsideAuthenticate(bool bInside) { m_bInsideAuthenticate = bInside; }

	bool isInsideInitialCapLs() const { return m_bInsideInitialCapLs; }
	void setInsideInitialCapLs(bool bInside) { m_bInsideInitialCapLs = bInside; }

	bool isInsideInitialStartTls() const { return m_bInsideInitialStartTls; }
	void setInsideInitialStartTls(bool bInside) { m_bInsideInitialStartTls = bInside; }

	void setIgnoreOneYouHaveNotRegisteredError(bool bIgnore)
	{
		m_bIgnoreOneYouHaveNotRegisteredError = bIgnore;
	}
	bool ignoreOneYouHaveNotRegisteredError() const
	{
		return m_bIgnoreOneYouHaveNotRegisteredError;
	}

	bool isInsideInitialCapReq() const { return m_bInsideInitialCapReq; }
	void setInsideInitialCapReq(bool bInside) { m_bInsideInitialCapReq = bInside; }

	bool sentQuit() const { return m_bSentQuit; }
	void setSentQuit() { m_bSentQuit = true; }

	kvi_time_t lastReceivedChannelWhoReply() const { return m_tLastReceivedChannelWhoReply; }
	void setLastReceivedChannelWhoReply(kvi_time_t tTime) { m_tLastReceivedChannelWhoReply = tTime; }

	kvi_time_t lastSentChannelWhoRequest() const { return m_tLastSentChannelWhoRequest; }
	void setLastSentChannelWhoRequest(kvi_time_t tTime) { m_tLastSentChannelWhoRequest = tTime; }

	kvi_time_t lastReceivedWhoisReply() const { return m_tLastReceivedWhoisReply; }
	void setLastReceivedWhoisReply(kvi_time_t tTime) { m_tLastReceivedWhoisReply = tTime; }

	bool simulateUnexpectedDisconnect() const { return m_bSimulateUnexpectedDisconnect; }
	void setSimulateUnexpectedDisconnect(bool bSimulate) { m_bSimulateUnexpectedDisconnect = bSimulate; }

	const QString & commandToExecAfterConnect() const { return m_szCommandToExecAfterConnect; }
	void setCommandToExecAfterConnect(const QString & szCmd) { m_szCommandToExecAfterConnect = szCmd; }
};

#endif //!_KVI_IRCCONNECTIONSTATEDATA_H_
