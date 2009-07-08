#ifndef _KVI_IRCMASK_H_
#define _KVI_IRCMASK_H_

//=============================================================================
//
//   File : kvi_ircmask.h
//   Creation date : Fri Jan 8 1999 19:50:35 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2008 Szymon Stefanek (pragma at kvirc dot net)
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
// originally this file was named kvi_ircuser.h and the class was KviIrcUser
// ported to UNICODE on 2004.10.28 1:50 am
//=============================================================================

/**
* \file kvi_ircmask.h
* \author Szymon Stefanek
* \brief Irc user mask abstraction
*/

#include "kvi_settings.h"
#include "kvi_heapobject.h"
#include "kvi_qstring.h"

/**
* \class KviIrcMask
* \brief Irc user mask handling
*/
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
	/**
	* \brief Constructs an empty mask (*!*@*)
	* \return KviIrcMask
	*/
	KviIrcMask();

	/**
	* \brief Constructs this KviIrcMask object from a string mask
	* \param szMask The mask of the user
	* \return KviIrcMask
	*/
	KviIrcMask(const QString & szMask);

	/**
	* \brief Carbon copy
	* \param ircUser The mask of the user
	* \return KviIrcMask
	*/
	KviIrcMask(const KviIrcMask & ircUser)
	: m_szNick(ircUser.m_szNick), m_szUser(ircUser.m_szUser), m_szHost(ircUser.m_szHost) {};

	/**
	* \brief Carbon copy
	* \param nick The nickname of the user
	* \param user The username of the user
	* \param host The hostname of the user
	* \return KviIrcMask
	*/
	KviIrcMask(const QString & nick, const QString & user, const QString & host)
	: m_szNick(nick), m_szUser(user), m_szHost(host) {};

	/**
	* \enum MaskType
	*/
	enum MaskType
	{
		NickUserHost = 0,           /**< nick!~user@machine.host.top (default) */
		NickUserNet = 1,            /**<  1 : nick!~user@*.abc.host.top */
		NickUser = 2,               /**<  2 : nick!~user@* */
		NickHost = 3,               /**<  3 : nick!*@machine.host.top */
		NickNet = 4,                /**<  4 : nick!*@*.abc.host.top */
		Nick = 5,                   /**<  5 : nick!*@* */
		UserHost = 6,               /**<  6 : *!~user@machine.host.top */
		UserNet = 7,                /**<  7 : *!~user@*.abc.host.top */
		User = 8,                   /**<  8 : *!~user@* */
		Host = 9,                   /**<  9 : *!*@machine.host.top */
		Net = 10,                   /**< 10 : *!*@*.abc.host.top */
		NickCleanUserHost = 11,     /**< 11 : nick!*user@machine.host.top */
		NickCleanUserNet = 12,      /**< 12 : nick!*user@*.abc.host.top */
		NickCleanUser = 13,         /**< 13 : nick!*user@* */
		CleanUserHost = 14,         /**< 14 : *!*user@machine.host.top */
		CleanUserNet = 15,          /**< 15 : *!*user@*.abc.host.top */
		CleanUser = 16,             /**< 16 : *!*user@* */
		NickUserLargeNet = 17,      /**< 17 : nick!~user@*.host.top */
		NickLargeNet = 18,          /**< 18 : nick!*@*.host.top */
		UserLargeNet = 19,          /**< 19 : *!~user@*.host.top */
		NickCleanUserLargeNet = 20, /**< 20 : nick!*user@*.host.top */
		CleanUserLargeNet = 21,     /**< 21 : *!*user@*.host.top */
		NickUserSmartNet = 22,      /**< 22 : nick!~user@*.host.top */
		NickSmartNet = 23,          /**< 23 : nick!*@*.host.top */
		UserSmartNet = 24,          /**< 24 : *!~user@*.host.top */
		NickCleanUserSmartNet = 25, /**< 25 : nick!*user@*.host.top */
		CleanUserSmartNet = 26      /**< 26 : *!*user@*.host.top */
	};
public:
	/**
	* \brief Sets the nick for this user.
	*
	* If szNick is NULL or it points to an empty string the nick is set to "*".
	* \param szNick The nickname of the user
	* \return void
	*/
	void setNick(const QString & szNick){ m_szNick = szNick.isEmpty() ? m_szWild : szNick; };

	/**
	* \brief Sets the username for this user.
	*
	* If szUsername is NULL or it points to an empty string the username is set to "*".
	* \param szUser The username of the user
	* \return void
	*/
	void setUsername(const QString & szUser){ m_szUser = szUser.isEmpty() ? m_szWild : szUser; };

	/**
	* \brief Sets the username for this user.
	*
	* If szUsername is NULL or it points to an empty string the username is set to "*".
	* This is an alias of setUsername()
	* \param szUser The username of the user
	* \return void
	*/
	void setUser(const QString &szUser){ m_szUser = szUser.isEmpty() ? m_szWild : szUser; };

	/**
	* \brief Sets the host for this user.
	*
	* If szHost is NULL or it points to an empty string the host is set to "*".
	* \param szHost The hostname of the user
	* \return void
	*/
	void setHost(const QString &szHost){ m_szHost = szHost.isEmpty() ? m_szWild : szHost; };

	// Sets the host , nick and username extracting it from an irc mask:
	// nick!user@host
	// The mask is terminated by end-of string null character or a character equal to c in the string.
	// Returns the pointer to the end of the mask in the szMask string.(c or null-terminator)
	//const char * setMask(const QString &szMask,char c=' ');

	// Sets the host , nick and username extracting it from an userhost mask:
	// nick[*]=<+|->user@host
	// The mask is terminated by end-of string null char or a space character.
	// Returns the pointer to the next non-space char in the szMask string or to the null-terminator
	// If there are no more masks avaiable.
	// WARNING : the szMask pointer can NOT be NULL
	//const char *setUserhostMask(const QString &szMask);

	/**
	* \brief Returns the nickname of this user.
	*
	* In the worst case you get a string == "*"
	* \return const QString &
	*/
	const QString & nick() const { return m_szNick; };

	/**
	* \brief Returns the username of this user.
	*
	* In the worst case you get a string == "*"
	* \return const QString &
	*/
	const QString & user() const { return m_szUser; };

	/**
	* \brief Returns the hostname of this user.
	*
	* In the worst case you get a string == "*"
	* \return const QString &
	*/
	const QString & host() const { return m_szHost; };

	/**
	* \brief Returns true if the username is set in the mask
	* \return bool
	*/
	bool hasUser() const { return !(m_szUser.isEmpty() || (m_szUser == m_szWild)); };

	/**
	* \brief Returns true if the hostname is set in the mask
	* \return bool
	*/
	bool hasHost() const { return !(m_szHost.isEmpty() || (m_szHost == m_szWild)); };

	/**
	* \brief Returns true if the hostname is a numeric IP
	* \return bool
	*/
	bool hasNumericHost() const;

	/**
	* \brief Returns in szMask the specified (if possible) mask of this user.
	*
	* If the host or username are not known , the mask may contain less
	* information than requested.
	* \param szMask The mask of the user
	* \param eMaskType The mask type
	*/
	void mask(QString & szMask, MaskType eMaskType = NickCleanUserHost) const;

	/**
	* \brief Returns true if the nickname contains wildcards (* and ?)
	* \return bool
	*/
	bool hasWildNick();

	/**
	* \brief Wild external matches (this and external are wild)
	* \param mask The mask of the user
	* \return bool
	*/
	//bool matches(const char *szMask);
	// passing 0 as one of params here means that it is a match by default
	//bool matches(const char *nick,const char *user,const char *host);
	bool matches(const KviIrcMask & mask) const;

	/**
	* \brief Fixed external matches (this is wild , external is fixed)
	* \param nick The nickname of the user
	* \param user The username of the user
	* \param host The hostname of the user
	* \return bool
	*/
	bool matchesFixed(const QString & nick, const QString & user, const QString & host) const;
	//bool matchesFixed(const QString &szMask) const;

	/**
	* \brief Fixed external matches (this is wild , external is fixed)
	* \param mask The make of the user
	* \return bool
	*/
	bool matchesFixed(const KviIrcMask & mask) const;

	/**
	* \brief Fixed internal matches (this is fixed, external is wild)
	* \param mask The mask of the user
	* \return bool
	*/
	//bool matchedBy(const QString &szMask) const;
	bool matchedBy(const KviIrcMask & mask) const { return mask.matchesFixed(*this); };

	/**
	* \brief Returns the number of non-wildcards characters in the mask
	* \return int
	*/
	int nonWildChars();

	/**
	* \brief Comparison
	* \param user The username of the user
	* \return bool
	*/
	bool operator==(const KviIrcMask & user);
private:
	/**
	* \brief Returns the number of characters which are parts of the IP
	*
	* The IP is in the form xxx.xxx.xxx.*
	* \return int
	*/
	int getIpDomainMaskLen() const;

	/**
	* \brief Returns the number of characters which are parts of the IP
	*
	* The IP is in the form xxx.xxx.*
	* \return int
	*/
	int getLargeIpDomainMaskLen() const;

	/**
	* \brief Returns the ip in the form xxx.xxx.xxx.*
	* \return QString
	*/
	QString getHostDomainMask() const;

	/**
	* \brief Returns the ip in the form xxx.xxx.*
	* \return QString
	*/
	QString getLargeHostDomainMask() const;

	/**
	* \brief Returns true if the user has the ip in the mask
	* \return bool
	* \warning This is just a GUESS and must be called AFTER making sure that it is NOT a plain numeric IP
	*/
	bool hasMaskedIp() const;
};

#endif //_KVI_IRCMASK_H_
