#ifndef _KVI_IRCCONNECTIONUSERINFO_H_
#define _KVI_IRCCONNECTIONUSERINFO_H_
//=============================================================================
//
//   File : kvi_ircconnectionuserinfo.h
//   Creation date : Sun 20 Jun 2004 01:45:42 by Szymon Stefanek
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
#include "kvi_qstring.h"
#include "kvi_time.h"

class KVIRC_API KviIrcConnectionUserInfo
{
	friend class KviIrcConnection;
	friend class KviServerParser;
	friend class KviConsole;
protected:
	KviIrcConnectionUserInfo();
	~KviIrcConnectionUserInfo(){};
private:
	QString    m_szRealName;          // the actual real name sent from the server
	QString    m_szNickName;          // the actual nick name acknowledged by the server
	QString    m_szUserMode;          // the actual user mode
	QString    m_szUserName;          // user name with ident char
	QString    m_szLocalHostIp;       // the local host ip as found by resolveLocalHost() at connection startup
	QString    m_szHostName;          // the local host name that the server reports
	QString    m_szHostIp;            // the host name above resolved, if possible
	QString    m_szAwayReason;
	bool       m_bAway;               // is the user away ?
	kvi_time_t m_tAway;               // time at that the user went away
	QString    m_szNickBeforeAway;    // the nickname that the user had just before going away
	// From bugtrack:
	// On many IRC networks the host is masked or hashed, and if one also is connected through a LAN,
	// it is basically impossible to initiate DCC transfers, as 192.168.0.13 or similar will be sent as IP.
	// But IRC servers usually (at least in my experience) send the unmasked host in the 001 raw event
	// with something like ":Welcome to the Internet Relay Chat network, nick!ident@host". I think
	// it'd be a good idea to just grab the IP from that event and use it for DCC transfers by default.
	QString    m_szUnmaskedHostName;
public:
	const QString & realName(){ return m_szRealName; };
	const QString & nickName(){ return m_szNickName; };
	const QString & userMode(){ return m_szUserMode; };
	const QString & userName(){ return m_szUserName; };
	const QString & localHostIp(){ return m_szLocalHostIp; };
	const QString & hostName(){ return m_szHostName; };
	const QString & unmaskedHostName(){ return m_szUnmaskedHostName; };
	const QString & hostIp(){ return m_szHostIp; };
	const QString & awayReason() { return m_szAwayReason; };
	bool hasUserMode(const QChar & m);
	bool isAway(){ return m_bAway; };
	time_t awayTime(){ return m_tAway; };
	const QString & nickNameBeforeAway(){ return m_szNickBeforeAway; };
protected:
	void setRealName(const QString &szRealName){ m_szRealName = szRealName; };
	void setNickName(const QString &szNickName){ m_szNickName = szNickName; };
	void setUserMode(const QString &szUserMode){ m_szUserMode = szUserMode; };
	void setUserName(const QString &szUserName){ m_szUserName = szUserName; };
	void setHostName(const QString &szHostName){ m_szHostName = szHostName; };
	void setUnmaskedHostName(const QString &szHostName){ m_szUnmaskedHostName = szHostName; };
	void setHostIp(const QString &szHostIp){ m_szHostIp = szHostIp; };
	void setLocalHostIp(const QString &szLocalHostIp){ m_szLocalHostIp = szLocalHostIp; };
	bool addUserMode(const QChar &m); // returns false if the mode was already there
	bool removeUserMode(const QChar &m); // returns fales if the mode was not there
	void setAwayReason(const QString& szReazon) { m_szAwayReason=szReazon; };
	void setAway();
	void setBack();
};

#endif //!_KVI_IRCCONNECTIONUSERINFO_H_
