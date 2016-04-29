#ifndef _DESCRIPTOR_H_
#define _DESCRIPTOR_H_
//=============================================================================
//
//   File : DccDescriptor.h
//   Creation date : Tue Jul 23 01:11:52 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (oragma at kvirc dot net)
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

#include "KviCString.h"
#include "KviConsoleWindow.h"
#include "KviPointerHashTable.h"

class DccWindow;
class DccFileTransfer;
class DccDescriptor
{
public:
	DccDescriptor(KviConsoleWindow * pConsole);
	DccDescriptor(const DccDescriptor & src);

	~DccDescriptor();

protected:
	KviConsoleWindow * m_pConsole;

	// mIrc zero port reverse send/chat extension
	KviCString m_szZeroPortRequestTag;

	unsigned int m_uId; // this dcc session ID
	QString m_szId;

	DccWindow * m_pDccWindow;         // 0 if it has no window
	DccFileTransfer * m_pDccTransfer; // 0 if it is not a transfer

	bool m_bCreationEventTriggered;

public:
	// A console that this DCC is bound to (might be replaced while we wait for user acknowledge in dialogs)
	KviConsoleWindow * console() const { return m_pConsole; };
	void setConsole(KviConsoleWindow * c) { m_pConsole = c; };

	DccWindow * window() const { return m_pDccWindow; };
	void setWindow(DccWindow * w) { m_pDccWindow = w; };

	DccFileTransfer * transfer() const { return m_pDccTransfer; };
	void setTransfer(DccFileTransfer * t) { m_pDccTransfer = t; };

	// mIrc zero port reverse send/chat extension
	bool isZeroPortRequest() const { return m_szZeroPortRequestTag.hasData(); };
	const char * zeroPortRequestTag() const { return m_szZeroPortRequestTag.ptr(); };
	void setZeroPortRequestTag(const KviCString & szTag) { m_szZeroPortRequestTag = szTag; };

	unsigned int id() const { return m_uId; };
	const QString & idString() const { return m_szId; };
	static DccDescriptor * find(unsigned int uId);
	static KviPointerHashTable<int, DccDescriptor> * descriptorDict();

	void triggerCreationEvent(); // this MUST be called by the creator of the descriptor!
private:
	void copyFrom(const DccDescriptor & src);

public:
	// Generic parameters
	QString szType; // DCC protocol : CHAT, SCHAT, SEND, TSSEND....

	bool bActive; // active or passive connection ?

	QString szNick; // remote user nickname
	QString szUser; // remote user name (unknown for passive dcc)
	QString szHost; // remote user host (unknown for passive dcc)

	QString szLocalNick; // local user nickname (always from irc)
	QString szLocalUser; // local user username (always from irc)
	QString szLocalHost; // local user hostname (always from irc)

	QString szIp;   // remote user ip (active dcc only)
	QString szPort; // remote user port (active dcc only)

	QString szListenIp;   // passive only : ip to listen on
	QString szListenPort; // passive only : port to listen on

	bool bSendRequest; // passive only : true if we have to send the CTCP request

	QString szFakeIp;   // passive only : fake ip to send in the CTCP
	QString szFakePort; // passive only : fake port to send in the CTCP

	bool bDoTimeout; // the marshall has to setup a timeout ?

	bool bIsTdcc; // is this a TDCC ?

	bool bOverrideMinimize; // Override the default minimize option ?
	bool bShowMinimized;    // Show minimized ? (valid if bOverrideMinimize is true)

	bool bAutoAccept; // Auto accepted dcc send/chat ?
#ifdef COMPILE_SSL_SUPPORT
	bool bIsSSL; // do we have to use SSL ?
#endif
	// Specific parameters

	// DCC SEND/RECV

	QString szFileName; // RECVFILE: incoming file name, SENDFILE: filename sent to the remote end
	QString szFileSize; // RECVFILE: incoming file size, SENDFILE: remote resume size

	QString szLocalFileName; // RECVFILE: save file name selected, SENDFILE: file to send
	QString szLocalFileSize; // RECVFILE: local file size (to resume), SENDFILE: file to send size

	bool bRecvFile; // do we have to RECEIVE the file or SEND it ?
	bool bResume;   // do we want to resume ?
	bool bNoAcks;   // blind dcc send ? (do not receive nor send acknowledges)

	bool bIsIncomingAvatar; // It is an Incoming Avatar DCC SEND ?

	// DCC VOICE
	KviCString szCodec; // codec name
	int iSampleRate;    // Sample rate
public:
	QString protocol() const { return szType; };
	bool isActive() const { return bActive; };
	QString remoteNick() const { return szNick; };
	QString remoteUser() const { return szUser; };
	QString remoteHost() const { return szHost; };
	QString remoteIp() const { return szIp; };
	QString remotePort() const { return szPort; };
	QString remoteFileName() const { return szFileName; };
	QString remoteFileSize() const { return szFileSize; };
	QString localNick() const { return szLocalNick; };
	QString localUser() const { return szLocalUser; };
	QString localHost() const { return szLocalHost; };
	QString localIp() const { return szIp; };
	QString localPort() const { return szPort; };
	QString localFileName() const { return szLocalFileName; };
	QString localFileSize() const { return szLocalFileSize; };
	bool isFileUpload();
	bool isFileDownload();
	bool isDccChat();
	bool isFileTransfer() { return (isFileUpload() || isFileDownload()); };
#ifdef COMPILE_SSL_SUPPORT
	bool isSSL() const
	{
		return bIsSSL;
	};
#endif
};

#endif //_DESCRIPTOR_H_
