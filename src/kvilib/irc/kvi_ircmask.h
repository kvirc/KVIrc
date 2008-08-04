#ifndef _KVI_IRCMASK_H_
#define _KVI_IRCMASK_H_

//=============================================================================
//
//   File : kvi_ircmask.h
//   Creation date : Fri Jan 8 1999 19:50:35 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2004 Szymon Stefanek (pragma at kvirc dot net)
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

// originally this file was named kvi_ircuser.h and the class was KviIrcUser
// ported to UNICODE on 2004.10.28 1:50 am

#include "kvi_settings.h"
#include "kvi_heapobject.h"
#include "kvi_qstring.h"

//=============================================================================
// Irc user mask abstraction
//=============================================================================

class KVILIB_API KviIrcMask : public KviHeapObject
{
	friend class KviIrcUserList;
	friend class KviIrcUserChanList;
private:
	QString m_szNick;
	QString m_szUser;
	QString m_szHost;
	static QString m_szWild;
public:
	// Sets the nick for this user.<br>
	// If szNick is NULL or it points to an empty string the nick is set to "*".<br>
	void setNick(const QString &szNick){ m_szNick = szNick.isEmpty() ? m_szWild : szNick; };
	// Sets the username for this user.<br>
	// If szUsername is NULL or it points to an empty string the username is set to "*".<br>
	void setUsername(const QString &szUser){ m_szUser = szUser.isEmpty() ? m_szWild : szUser; };
	void setUser(const QString &szUser){ m_szUser = szUser.isEmpty() ? m_szWild : szUser; };
	// Sets the host for this user.<br>
	// If szHost is NULL or it points to an empty string the host is set to "*".<br>
	void setHost(const QString &szHost){ m_szHost = szHost.isEmpty() ? m_szWild : szHost; };
	// Sets the host , nick and username extracting it from an irc mask:<br>
	// nick!user@host<br>
	// The mask is terminated by end-of string null character or a character equal to c in the string.<br>
	// Returns the pointer to the end of the mask in the szMask string.(c or null-terminator)<br>
	//const char * setMask(const QString &szMask,char c=' ');
	// Sets the host , nick and username extracting it from an userhost mask:<br>
	// nick[*]=<+|->user@host<br>
	// The mask is terminated by end-of string null char or a space character.<br>
	// Returns the pointer to the next non-space char in the szMask string or to the null-terminator<br>
	// If there are no more masks avaiable.
	// WARNING : the szMask pointer can NOT be NULL
	//const char *setUserhostMask(const QString &szMask);
	// Returns the nick of this user.<br>
	// In the worst case you get a string == "*"<br>
	const QString &nick() const { return m_szNick; };
	const QString &user() const { return m_szUser; };
	const QString &host() const { return m_szHost; };

	bool hasUser() const { return !(m_szUser.isEmpty() || (m_szUser == m_szWild)); };
	bool hasHost() const { return !(m_szHost.isEmpty() || (m_szHost == m_szWild)); };

	bool hasNumericHost() const;
	// Retuns in szMask the specified (if possible) mask of this user.<br>
	// If the host or username are not known , the mask may contain less information
	// than requested.<br>

	enum MaskType
	{
		NickUserHost = 0,           // nick!~user@machine.host.top (nick!~user@XXX.XXX.XXX.XXX) (default)
		NickUserNet = 1,            // 1 : nick!~user@*.abc.host.top       (nick!~user@XXX.XXX.XXX.*)
		NickUser = 2,               // 2 : nick!~user@*
		NickHost = 3,               // 3 : nick!*@machine.host.top    (nick!*@XXX.XXX.XXX.XXX)
		NickNet = 4,                // 4 : nick!*@*.abc.host.top          (nick!*@XXX.XXX.XXX.*)
		Nick = 5,                   // 5 : nick!*@*
		UserHost = 6,               // 6 : *!~user@machine.host.top    (*!~user@XXX.XXX.XXX.XXX)
		UserNet = 7,                // 7 : *!~user@*.abc.host.top          (*!~user@XXX.XXX.XXX.*)
		User = 8,                   // 8 : *!~user@*
		Host = 9,                   // 9 : *!*@machine.host.top       (*!*@XXX.XXX.XXX.XXX)
		Net = 10,                   // 10: *!*@*.abc.host.top             (*!*@XXX.XXX.XXX.*)
		NickCleanUserHost = 11,     // 11 : nick!*user@machine.host.top    (nick!*user@XXX.XXX.XXX.XXX)
		NickCleanUserNet = 12,      // 12 : nick!*user@*.abc.host.top          (nick!*user@XXX.XXX.XXX.*)
		NickCleanUser = 13,         // 13 : nick!*user@*
		CleanUserHost = 14,         // 14 : *!*user@machine.host.top    (*!user@*XXX.XXX.XXX.XXX)
		CleanUserNet = 15,          // 15 : *!*user@*.abc.host.top          (*!user@*XXX.XXX.XXX.*)
		CleanUser = 16,             // 16 : *!*user@*
		NickUserLargeNet = 17,      // 17 : nick!~user@*.host.top       (nick!~user@XXX.XXX.*)
		NickLargeNet = 18,          // 18 : nick!*@*.host.top          (nick!*@XXX.XXX.*)
		UserLargeNet = 19,          // 19 : *!~user@*.host.top          (*!~user@XXX.XXX.*)
		NickCleanUserLargeNet = 20, // 20 : nick!*user@*.host.top          (nick!*user@XXX.XXX.*)
		CleanUserLargeNet = 21,     // 21 : *!*user@*.host.top          (*!user@*XXX.XXX.*)
		// smart versions of the masks 17-21 that try take care of masked ip addresses
		// in the form xxx.xxx.INVALID-TOP-MASK
		NickUserSmartNet = 22,      // 22 : nick!~user@*.host.top       (nick!~user@XXX.XXX.*)
		NickSmartNet = 23,          // 23 : nick!*@*.host.top          (nick!*@XXX.XXX.*)
		UserSmartNet = 24,          // 24 : *!~user@*.host.top          (*!~user@XXX.XXX.*)
		NickCleanUserSmartNet = 25, // 25 : nick!*user@*.host.top          (nick!*user@XXX.XXX.*)
		CleanUserSmartNet = 26      // 26 : *!*user@*.host.top          (*!user@*XXX.XXX.*)
	};

	void mask(QString &szMask,MaskType eMaskType = NickCleanUserHost) const;
	bool hasWildNick();

	// Wild external matches (this and external are wild)

//	bool matches(const char *szMask);
//	// passing 0 as one of params here means that it is a match by default
//	bool matches(const char *nick,const char *user,const char *host);
	bool matches(const KviIrcMask &mask) const;


	// Fixed external matches  (this is wild , external is fixed)
	bool matchesFixed(const QString &nick,const QString &user,const QString &host) const;
//	bool matchesFixed(const QString &szMask) const;
	bool matchesFixed(const KviIrcMask &mask) const;

	// Fixed internal matches (this is fixed , external is wild)
	//bool matchedBy(const QString &szMask) const;
	bool matchedBy(const KviIrcMask &mask) const { return mask.matchesFixed(*this); };

	int nonWildChars();
	bool operator==(const KviIrcMask &user);
public:
	// Constructs an empty mask (*!*@*)
	KviIrcMask();
	// Constructs this KviIrcMask object from a string mask
	KviIrcMask(const QString &szMask);
	// Carbon copy
	KviIrcMask(const KviIrcMask &ircUser)
		: m_szNick(ircUser.m_szNick), m_szUser(ircUser.m_szUser), m_szHost(ircUser.m_szHost) {};
	KviIrcMask(const QString &nick,const QString &user,const QString &host)
		: m_szNick(nick), m_szUser(user), m_szHost(host) {};
private:
	int getIpDomainMaskLen() const;
	QString getHostDomainMask() const;
	int getLargeIpDomainMaskLen() const;
	QString getLargeHostDomainMask() const;
	// this is just a GUESS and must be called AFTER making sure that it is NOT a plain numeric IP
	bool hasMaskedIp() const;
};

#endif //_KVI_IRCMASK_H_
