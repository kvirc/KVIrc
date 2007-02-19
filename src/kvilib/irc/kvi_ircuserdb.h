#ifndef _KVI_IRCUSERDB_H_
#define _KVI_IRCUSERDB_H_

//=============================================================================
//
//   File : kvi_ircuserdb.h
//   Creation date : Mon Jul 31 2000 20:59:12 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2004 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_dict.h"

#include "kvi_string.h"
#include "kvi_avatar.h"
#include "kvi_regusersdb.h"

class KviIrcUserDataBase;

class KVILIB_API KviIrcUserEntry
{
	friend class KviIrcUserDataBase;
public:
	KviIrcUserEntry(const QString &user,const QString &host);
	~KviIrcUserEntry();

	enum Gender {
		Male = 0,
		Female = 1,
		Unknown = 3
	};

protected:
	QString     m_szUser;
	QString     m_szHost;

	QString     m_szServer;
	QString     m_szRealName;
	int         m_iHops;
	Gender      m_eGender;
	
	bool        m_bAway;

	KviAvatar * m_pAvatar;

	int         m_nRefs;
	bool        m_bBot;

	bool        m_bNotFoundRegUserLoockup;
	QString     m_szRegisteredUserName;
	QString     m_szLastRegisteredMatchNick;

	QColor      m_cachedColor;
	bool        m_bUseCustomColor;
public:
	Gender gender() { return m_eGender; };
	void setBot(bool bIsBot) { m_bBot = bIsBot; };
	bool isBot() { return m_bBot; };
	void setGender(Gender g) { m_eGender=g; };
	void setUser(const QString &user)   { m_szUser = user; };
	bool hasUser()                      { return (!m_szUser.isEmpty()); };
	void setHost(const QString &host)   { m_szHost = host; };
	bool hasHost()                      { return (!m_szHost.isEmpty()); };
	void setServer(const QString &serv) { m_szServer = serv; };
	void setRealName(const QString &rn);
	void setHops(int hops)              { m_iHops = hops; };
	const QString &user()               { return m_szUser; };
	const QString &host()               { return m_szHost; };
	bool hasServer()                    { return (!m_szServer.isEmpty()); };
	const QString &server()             { return m_szServer; };
	bool hasRealName()                  { return (!m_szRealName.isEmpty()); };
	const QString &realName()           { return m_szRealName; };
	bool hasHops()                      { return m_iHops >= 0; };
	int hops()                          { return m_iHops; };
	KviAvatar * avatar()                { return m_pAvatar; };
	int nRefs()                         { return m_nRefs; };
	bool isAway() const                 { return m_bAway; };
	void setAway(bool bAway)            { m_bAway = bAway; };
	// warning! the ownership passes to this class!
	void setAvatar(KviAvatar * av = 0);
	KviAvatar * forgetAvatar();
};

// from least powerful to most powerful
#define KVI_USERFLAG_USEROP 1
#define KVI_USERFLAG_VOICE 2
#define KVI_USERFLAG_HALFOP 4
#define KVI_USERFLAG_OP 8
#define KVI_USERFLAG_CHANADMIN 16
#define KVI_USERFLAG_CHANOWNER 32

// 1 | 2 | 4 | 8 | 16 | 32
#define KVI_USERFLAG_MASK (KVI_USERFLAG_OP | KVI_USERFLAG_VOICE | KVI_USERFLAG_HALFOP | KVI_USERFLAG_CHANADMIN | KVI_USERFLAG_USEROP | KVI_USERFLAG_CHANOWNER)
// at the moment all the flags are usermodes
#define KVI_USERFLAG_MODEMASK KVI_USERFLAG_MASK


class KVILIB_API KviIrcUserDataBase : public QObject
{
	Q_OBJECT
public:
	KviIrcUserDataBase();
	~KviIrcUserDataBase();
private:
	KviDict<KviIrcUserEntry> * m_pDict;
public:
	void clear();
	KviIrcUserEntry * insertUser(const QString &nick,const QString &user,const QString &hostname);
	KviIrcUserEntry * find(const QString &nick){ return m_pDict->find(nick); };
	void removeUser(const QString &nick,KviIrcUserEntry * e);
	KviDict<KviIrcUserEntry> * dict(){ return m_pDict; };

	KviRegisteredUser* registeredUser(const QString & nick);
	KviRegisteredUser* registeredUser(const QString & nick,const QString & user,const QString & host);

	bool haveCustomColor(const QString & nick);
	QColor* customColor(const QString & nick);

	void setupConnectionWithReguserDb();
protected slots:
	void registeredUserRemoved(const QString&);
	void registeredUserChanged(const QString&);
	void registeredUserAdded  (const QString&);
	void registeredDatabaseCleared();
};

#endif //_KVI_IRCUSERDB_H_
