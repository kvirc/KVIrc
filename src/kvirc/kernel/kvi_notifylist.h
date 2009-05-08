#ifndef _KVI_NOTIFYLIST_H_
#define _KVI_NOTIFYLIST_H_

//=============================================================================
//
//   File : kvi_notifylist.h
//   Creation date : Fri Oct 27 2000 23:39:09 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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
#include "kvi_pointerlist.h"
#include "kvi_pointerhashtable.h"

#include <QObject>
#include <QTimer>

class KviConsole;
class KviIrcMessage;
class KviIrcMask;
class KviIrcConnection;

class KVIRC_API KviNotifyListManager : public QObject
{
	friend class KviConsole;
	friend class KviConnectionInfo;
	friend class KviServerParser;
	friend class KviIrcConnection;
	Q_OBJECT
protected:
	KviNotifyListManager(KviIrcConnection * pConnection);
	~KviNotifyListManager();
protected:
	KviConsole       * m_pConsole;
	KviIrcConnection * m_pConnection;
protected:
	virtual void start();
	virtual void stop();
	virtual bool handleUserhost(KviIrcMessage * msg);
	virtual bool handleIsOn(KviIrcMessage * msg);
	virtual bool handleWatchReply(KviIrcMessage *msg);
	void notifyOnLine(const QString &nick,const QString &user = QString(),const QString &host = QString(),const QString &szReason = QString(),bool bJoin=true);
	void notifyOffLine(const QString &nick,const QString &user = QString(),const QString &host = QString(),const QString &szReason = QString());
public:
	KviConsole * console(){ return m_pConsole;  };
};

class KVIRC_API KviIsOnNotifyListManager : public KviNotifyListManager
{
	friend class KviConsole;
	friend class KviServerParser;
	friend class KviIrcConnection;
	Q_OBJECT
protected:
	KviIsOnNotifyListManager(KviIrcConnection * pConnection);
	~KviIsOnNotifyListManager();
private:
	KviPointerHashTable<QString,QString>      * m_pRegUserDict;            // dict notifystring->reguser name
	KviPointerList<QString> * m_pNotifyList;                  // list of notifystring (total)
	KviPointerList<QString> * m_pIsOnList;                    // list of notifystring (one session)
	QString               m_szIsOnString;                 // m_pIsOnList in form of a string
	KviPointerList<QString> * m_pOnlineList;                  //
	KviPointerList<QString> * m_pUserhostList;
	QString               m_szUserhostString;
	bool                  m_bExpectingIsOn;
	bool                  m_bExpectingUserhost;
	QTimer              * m_pDelayedIsOnTimer;
	QTimer              * m_pDelayedNotifyTimer;
	QTimer              * m_pDelayedUserhostTimer;
	bool                  m_bRunning;
protected:
	virtual void start();
	virtual void stop();
	virtual bool handleUserhost(KviIrcMessage *msg);
	virtual bool handleIsOn(KviIrcMessage *msg);
private:
	void delayedNotifySession();
	void delayedIsOnSession();
	void delayedUserhostSession();
private slots:
	void newNotifySession();
	void newIsOnSession();
	void newUserhostSession();

	void buildRegUserDict();
	void buildIsOnList();
	void buildUserhostList();
	void buildNotifyList();
	void sendIsOn();
	void sendUserhost();
	bool doMatchUser(const QString &notifyString,const KviIrcMask & mask);

};


class KVIRC_API KviStupidNotifyListManager : public KviNotifyListManager
{
	friend class KviConsole;
	friend class KviServerParser;
	friend class KviIrcConnection;
	Q_OBJECT
protected:
	KviStupidNotifyListManager(KviIrcConnection * pConnection);
	~KviStupidNotifyListManager();
protected:
	KviPointerList<QString> * m_pNickList;
	QString m_szLastIsOnMsg;
	int m_iNextNickToCheck;
	int m_iRestartTimer;
protected:
	virtual void start();
	virtual void stop();
	virtual bool handleIsOn(KviIrcMessage *msg);
protected:
	virtual void timerEvent(QTimerEvent *e);
private:
	void buildNickList();
	void sendIsOn();
};

class KVIRC_API KviWatchNotifyListManager : public KviNotifyListManager
{
	friend class KviConsole;
	friend class KviServerParser;
	friend class KviIrcConnection;
	Q_OBJECT
public:
	KviWatchNotifyListManager(KviIrcConnection * pConnection);
	~KviWatchNotifyListManager();
protected:
	KviPointerHashTable<QString,QString> * m_pRegUserDict;            // dict notifystring->reguser name
protected:
	void buildRegUserDict();
	virtual void start();
	virtual void stop();
	virtual bool handleWatchReply(KviIrcMessage *msg);
	bool doMatchUser(KviIrcMessage *msg,const QString &notifyString,const KviIrcMask & mask);

};

#endif //_KVI_NOTIFYLIST_H_
