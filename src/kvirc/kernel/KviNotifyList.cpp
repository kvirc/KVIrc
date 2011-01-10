//=============================================================================
//
//   File : KviNotifyList.cpp
//   Creation date : Fri Oct 27 2000 23:41:01 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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




#include "kvi_debug.h"
#include "KviNotifyList.h"
#include "KviConsoleWindow.h"
#include "KviIrcSocket.h"
#include "KviRegisteredUserDataBase.h"
#include "KviUserListView.h"
#include "KviChannelWindow.h"
#include "KviOptions.h"
#include "KviWindow.h"
#include "KviLocale.h"
#include "kvi_out.h"
#include "KviIrcServerParser.h"
#include "KviIrcMask.h"
#include "KviIrcNumericCodes.h"
#include "KviParameterList.h"
#include "KviIrcConnection.h"
#include "KviApplication.h"
#include "KviQString.h"
#include "KviLagMeter.h"
#include "KviKvsEventTriggers.h"

#include <QStringList>
#include <QByteArray>

// FIXME: #warning "Finish this doc!"

/*
	@doc: notify_list
	@title:
		Notify lists
	@short:
		Tracking users on IRC
	@keyterms:
		notify property, watch property, notify lists
	@body:
		The notify list is a means of keeping track of users on IRC.[br]
		Once connected to an IRC server, you can tell KVIrc to check
		periodically if your friends are online.[br]
		This is basically achieved by setting a property in the [doc:registered_users]registered users database[/doc]
		entry.[br]
		The property is called "notify", and you have to set it to the nickname
		that you want to look for.[br]
		So for example, assume to register a frend of yours like Szymon:[br]
		[example]
			[cmd:reguser.add]reguser.add[/cmd] Szymon
			[cmd:reguser.addmask]reguser.addmask[/cmd] Szymon Pragma!*@*.it
		[/example]
		And then want it in the notify list; nothing easier, just set
		hist "notify" property to the nickname that you want him to be "looked for":[br]
		[example]
			[cmd:reguser.setproperty]reguser.setproperty[/cmd] Szymon notify Pragma
		[/example]
		In this way, once in a while, KVIrc will send to the server an ISON message
		with the nickname Pragma. If Szymon is online, you will be notified with a message:[br]
		"Pragma [someuser@somehost.it] is on IRC".[br]
		If Szymon uses often "[Pragma]" as his secondary nickname, you can do the following:[br]
		[example]
			[cmd:reguser.addmask]reguser.addmask[/cmd] Szymon [Pragma]*@*.it
			[cmd:reguser.setproperty]reguser.setproperty[/cmd] Szymon notify "Pragma [Pragma]"
		[/example]
		KVIrc will then look for both nicknames getting online.[br]
		KVIrc supports three notify lists management methods:[br]
		The "stupid ISON method", the "intelligent ISON method" and the "WATCH method".[br]
		The "stupid ISON method" will assume that Szymon is online if any user with nickname
		Pragma (or [Pragma] in the second example) gets online; this means that also Pragma!someuser@somehost.com will be
		assumed to be "Szymon" and will be shown in the notify list.[br]
		This might be a false assumption (since somehod.com does not even match *.it),
		but it is the best result that the "stupid ISON method" can achieve.[br]
		The "intelligent ISON method" will also check the Pragma's username and hostname
		and match it in the registered masks; so in the example above, you will be notified if
		any user that matches Pragma!*@*.it gets online; (but you will NOT be notified if
		(for example) Pragma!someuser@somehost.com gets online).[br]
		So what's the point in including a stupid method? :) Well...the intelligent
		method "eats" some of your IRC bandwidth; it has to send USERHOST messages
		for every group of 5 users in the notify list. If you have a lot of users
		in the notify list, it might become slow and eventually cause a
		client to server flood.[br]
		So finally, the intelligent method is the default. If you have "flood" problems,
		or if you think that the notify list is quite slow, try the "stupid" method:
		it is not that bad after all.[br]
		The third notify list management method is the "WATCH method".[br]
		It uses a totally different (and better) approach to the notify lists management,
		and can be used only on the networks that support the WATCH notify method (DALnet, WebNet, etc.).[br]
		KVIrc will attempt to guess if the server you're currently using supports the WATCH command
		and eventually use this last method.[br]
		The WATCH method uses the "notify" property to get the nicknames that have to be
		sent to the server in the /WATCH commands.
*/

// Basic NotifyListManager: this does completely nothing

KviNotifyListManager::KviNotifyListManager(KviIrcConnection * pConnection)
: QObject(0)
{
	setObjectName("notify_list_manager");
	m_pConnection = pConnection;
	m_pConsole = pConnection->console();
}

KviNotifyListManager::~KviNotifyListManager()
{
}

void KviNotifyListManager::start()
{
}

void KviNotifyListManager::stop()
{
}

bool KviNotifyListManager::handleUserhost(KviIrcMessage *)
{
	return false;
}

bool KviNotifyListManager::handleIsOn(KviIrcMessage *)
{
	return false;
}

bool KviNotifyListManager::handleWatchReply(KviIrcMessage *)
{
	return false;
}

void KviNotifyListManager::notifyOnLine(const QString & szNick, const QString & szUser, const QString & szHost, const QString & szReason, bool bJoin)
{
	if(bJoin)
		m_pConsole->notifyListView()->join(szNick,szUser,szHost);

	KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolNotifyListChangesToActiveWindow) ? m_pConsole->activeWindow() : m_pConsole;
	if(KVS_TRIGGER_EVENT_1_HALTED(KviEvent_OnNotifyOnLine,pOut,szNick))
		return;

	QString szWho;
	QString szMsg;

	if(!(szUser.isEmpty() || szHost.isEmpty()))
		szWho = QString("\r!n\r%1\r [%2@\r!h\r%3\r]").arg(szNick,szUser,szHost);
	else
		szWho = QString("\r!n\r%1\r").arg(szNick);

	KviPointerHashTable<QString,KviRegisteredUser> * d = g_pRegisteredUserDataBase->userDict();
	KviPointerHashTableIterator<QString,KviRegisteredUser> it(*d);

	while(KviRegisteredUser * pUser = it.current())
	{
		QString szProp = pUser->getProperty("notify");
		if(!szProp.isEmpty())
		{
			if(szProp.split(",",QString::SkipEmptyParts).indexOf(szNick) != -1)
			{
				QString szComment = pUser->getProperty("comment");
				if(!szComment.isEmpty())
					szMsg = QString("%1 (%2), Group \"%3\" is on IRC as (%4)").arg(pUser->name(),szComment,pUser->group(),szWho);
				else
					szMsg = QString("%1, Group \"%2\" is on IRC as (%3)").arg(pUser->name(),pUser->group(),szWho);
				break;
			}
		}
		++it;
	}
	
	QString szFmt = __tr2qs("%1 is on IRC");

	if(szMsg.isEmpty())
		szMsg = QString(szFmt).arg(szWho);

	if((!szReason.isEmpty()) && (_OUTPUT_VERBOSE))
	{
		szMsg += "(";
		szMsg += szReason;
		szMsg += ")";
	}

	pOut->outputNoFmt(KVI_OUT_NOTIFYONLINE,szMsg);

	if(!(pOut->hasAttention()))
	{
		if(KVI_OPTION_BOOL(KviOption_boolFlashWindowOnNotifyOnLine))
			pOut->demandAttention();
		if(KVI_OPTION_BOOL(KviOption_boolPopupNotifierOnNotifyOnLine))
		{
			szWho = "<b>";
			szWho += szNick;
			szWho += "</b>";
			szMsg = QString(szFmt).arg(szWho);
			g_pApp->notifierMessage(0,KVI_OPTION_MSGTYPE(KVI_OUT_NOTIFYONLINE).pixId(),szMsg,KVI_OPTION_UINT(KviOption_uintNotifierAutoHideTime));
		}
	}
}

void KviNotifyListManager::notifyOffLine(const QString & szNick, const QString & szUser, const QString & szHost, const QString & szReason)
{
	KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolNotifyListChangesToActiveWindow) ? m_pConsole->activeWindow() : m_pConsole;
	if(!KVS_TRIGGER_EVENT_1_HALTED(KviEvent_OnNotifyOffLine,pOut,szNick))
	{
		QString szWho;

		if(!(szUser.isEmpty() || szHost.isEmpty()))
			szWho = QString("\r!n\r%1\r [%2@\r!h\r%3\r]").arg(szNick,szUser,szHost);
		else
			szWho = QString("\r!n\r%1\r").arg(szNick);

		QString szMsg;

		KviPointerHashTable<QString,KviRegisteredUser> * d = g_pRegisteredUserDataBase->userDict();
		KviPointerHashTableIterator<QString,KviRegisteredUser> it(*d);

		while(KviRegisteredUser * pUser = it.current())
		{
			QString szProp = pUser->getProperty("notify");
			if(!szProp.isEmpty())
			{
				if(szProp.split(",",QString::SkipEmptyParts).indexOf(szNick) != -1)
				{
					QString szComment = pUser->getProperty("comment");
					if(!szComment.isEmpty())
						szMsg = QString("%1 (%2), Group \"%3\" has left IRC as (%4)").arg(pUser->name(),szComment,pUser->group(),szWho);
					else
						szMsg = QString("%1, Group \"%2\" has left IRC as (%3)").arg(pUser->name(),pUser->group(),szWho);
					break;
				}
			}
			++it;
		}

		if(szMsg.isEmpty())
			szMsg = QString(__tr2qs("%1 has left IRC")).arg(szWho);

		if((!szReason.isEmpty()) && (_OUTPUT_VERBOSE))
		{
			szMsg += "(";
			szMsg += szReason;
			szMsg += ")";
		}

		pOut->outputNoFmt(KVI_OUT_NOTIFYOFFLINE,szMsg);
	}

	m_pConsole->notifyListView()->part(szNick);
}



//
//  INTELLIGENT NOTIFY LIST MANAGER: NOTIFY PROCESS:
//
//            start()                              stop()
//               |                                   ^
//         buildRegUserDict()                        |
//               |                                   |
//     m_pRegUserDict->isEmpty() ? -- YES ---------->+
//                       |                           |
//                      NO                           |
//                       |                           |
//         newNotifySession()<------- TIMER ---------------- delayedNotifySession() --------------------------------+
//                       |    (can be stopped here)  |              ^                                               |
//                       |                           |              ^                                               |
//                  buildNotifyList()                |              |                                              YES
//                       |                           |              |                                               |
//                m_pNotifyList->isEmpty() ? - YES ->+              |                                               |
//                       |                                          |                                               |
//                      NO                                          |                                               |
//                       |                                          |                                               |
//                  newIsOnSession()<------------- TIMER -------------------- delayedIsOnSession() -- NO - m_pNotifyList->isEmpty() ?
//                               |           (can be stopped here)  |                                               |
//                               |                                  |                                               |
//                            buildIsOnList()                       |                                               |
//                               |                                  |                                               |
//                       m_pIsOnList->isEmpty() ? -- YES ---------->+                                               |
//                               |                                                                                  |
//                              NO                                                                                  |
//                               |                                                                                  |
//                            sendIsOn() - - - - - - - - - - - -> handleIsOn()                                      |
//                                                                      |                                           |
//                                                            (build m_pOnlineList)                                 |
//                                                                      |                                           |
//                                                         m_pOnlineList->isEmpty() ? - YES ----------------------->+
//                                                                      |                                           |
//                                                                     NO                                          YES
//                                                                      |                                           |
//                                                            delayedUserhostSession()<--------------- NO - m_pOnlineList->isEmpty() ?
//                                                                               |                                  ^
//                                                                             TIMER (can be stopped here)          |
//                                                                               |                                  |
//                                                                           newUserhostSession()                   |
//                                                                               |                                  |
//                                                                           buildUserhostList()                    |
//                                                                               |                                  |
//                                                                           m_pUserhostList->isEmpty() ? - YES --->+
//                                                                               |                          ^^^     |
//                                                                               |             (unexpected!)|||     |
//                                                                               NO                                 |
//                                                                               |                                  |
//                                                                           sendUserhost() - - - - - - - - > handleUserhost()
//


KviIsOnNotifyListManager::KviIsOnNotifyListManager(KviIrcConnection * pConnection)
: KviNotifyListManager(pConnection)
{
	m_pRegUserDict = new KviPointerHashTable<QString,QString>(17,false); // case insensitive, copy keys
	m_pRegUserDict->setAutoDelete(true);
	m_pNotifyList  = new KviPointerList<QString>;
	m_pNotifyList->setAutoDelete(true);
	m_pIsOnList = new KviPointerList<QString>;
	m_pIsOnList->setAutoDelete(true);
	m_pOnlineList = new KviPointerList<QString>;
	m_pOnlineList->setAutoDelete(true);
	m_pUserhostList = new KviPointerList<QString>;
	m_pUserhostList->setAutoDelete(true);
	m_pDelayedNotifyTimer = new QTimer();
	connect(m_pDelayedNotifyTimer,SIGNAL(timeout()),this,SLOT(newNotifySession()));
	m_pDelayedIsOnTimer = new QTimer();
	connect(m_pDelayedIsOnTimer,SIGNAL(timeout()),this,SLOT(newIsOnSession()));
	m_pDelayedUserhostTimer = new QTimer();
	connect(m_pDelayedUserhostTimer,SIGNAL(timeout()),this,SLOT(newUserhostSession()));
	m_bRunning = false;
}


KviIsOnNotifyListManager::~KviIsOnNotifyListManager()
{
	if(m_bRunning)stop();
	delete m_pDelayedNotifyTimer;
	delete m_pDelayedIsOnTimer;
	delete m_pDelayedUserhostTimer;
	delete m_pRegUserDict;
	delete m_pOnlineList;
	delete m_pNotifyList;
	delete m_pIsOnList;
	delete m_pUserhostList;
}

void KviIsOnNotifyListManager::start()
{
	if(m_bRunning)stop();
	m_bRunning = true;
	m_pConsole->notifyListView()->partAllButOne(m_pConnection->currentNickName());

	m_bExpectingIsOn = false;
	m_bExpectingUserhost = false;

	buildRegUserDict();
	if(m_pRegUserDict->isEmpty())
	{
		if(_OUTPUT_VERBOSE)
			m_pConsole->output(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Notify list: No users to check for, quitting"));
		stop();
		return;
	}
	newNotifySession();
}

void KviIsOnNotifyListManager::buildRegUserDict()
{
	m_pRegUserDict->clear();

	const KviPointerHashTable<QString,KviRegisteredUser> * d = g_pRegisteredUserDataBase->userDict();
	KviPointerHashTableIterator<QString,KviRegisteredUser> it(*d);
	while(KviRegisteredUser * u = it.current())
	{
		QString notify;
		if(u->getProperty("notify",notify))
		{
			notify.trimmed();
			while(!notify.isEmpty())
			{
				int idx = notify.indexOf(' ');
				if(idx > 0)
				{
					QString single = notify.left(idx);
					m_pRegUserDict->replace(single,new QString(u->name()));
					notify.remove(0,idx+1);
				} else {
					m_pRegUserDict->replace(notify,new QString(u->name()));
					notify = "";
				}
			}
		}
		++it;
	}
}

void KviIsOnNotifyListManager::delayedNotifySession()
{
	unsigned int iTimeout = KVI_OPTION_UINT(KviOption_uintNotifyListCheckTimeInSecs);
	if(iTimeout < 15)
	{
		// life first of all.
		// don't allow the user to suicide
		if(_OUTPUT_VERBOSE)
			m_pConsole->output(KVI_OUT_SYSTEMWARNING,
				__tr2qs("Notify list: Timeout (%d sec) is too short, resetting to something more reasonable (15 sec)"),
			iTimeout);
		iTimeout = 15;
		KVI_OPTION_UINT(KviOption_uintNotifyListCheckTimeInSecs) = 15;
	}
	m_pDelayedNotifyTimer->setInterval(iTimeout * 1000);
	m_pDelayedNotifyTimer->setSingleShot(true);
	m_pDelayedNotifyTimer->start();
}

void KviIsOnNotifyListManager::newNotifySession()
{
	buildNotifyList();
	if(m_pNotifyList->isEmpty())
	{
		if(_OUTPUT_VERBOSE)
			m_pConsole->output(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Notify list: Notify list empty, quitting"));
		stop();
		return;
	}
	newIsOnSession();
}

void KviIsOnNotifyListManager::buildNotifyList()
{
	m_pNotifyList->clear();
	KviPointerHashTableIterator<QString,QString> it(*m_pRegUserDict);
	while(it.current())
	{
		m_pNotifyList->append(new QString(it.currentKey()));
		++it;
	}
}

void KviIsOnNotifyListManager::delayedIsOnSession()
{
	unsigned int iTimeout = KVI_OPTION_UINT(KviOption_uintNotifyListIsOnDelayTimeInSecs);
	if(iTimeout < 5)
	{
		// life first of all.
		// don't allow the user to suicide
		if(_OUTPUT_VERBOSE)
			m_pConsole->output(KVI_OUT_SYSTEMWARNING,
				__tr2qs("Notify list: ISON delay (%d sec) is too short, resetting to something more reasonable (5 sec)"),
			iTimeout);
		iTimeout = 5;
		KVI_OPTION_UINT(KviOption_uintNotifyListIsOnDelayTimeInSecs) = 5;
	}
	m_pDelayedIsOnTimer->setInterval(iTimeout * 1000);
	m_pDelayedIsOnTimer->setSingleShot(true);
	m_pDelayedIsOnTimer->start();
}

void KviIsOnNotifyListManager::newIsOnSession()
{
	buildIsOnList();
	if(m_pIsOnList->isEmpty())delayedNotifySession();
	else sendIsOn();
}

void KviIsOnNotifyListManager::buildIsOnList()
{
	m_pIsOnList->clear();
	m_szIsOnString = "";
	m_pNotifyList->setAutoDelete(false);
	while(QString * s = m_pNotifyList->first())
	{
		if(((m_szIsOnString.length() + s->length()) + 1) < 504)
		{
			if(!m_szIsOnString.isEmpty())m_szIsOnString.append(' ');
			m_szIsOnString.append(*s);
			m_pIsOnList->append(s);
			m_pNotifyList->removeFirst();
		} else break;
	}
	m_pNotifyList->setAutoDelete(true);
}

void KviIsOnNotifyListManager::sendIsOn()
{
	if(_OUTPUT_PARANOIC)
		m_pConsole->output(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Notify list: Checking for: %Q"),&m_szIsOnString);
	QByteArray szDec = m_pConnection->encodeText(m_szIsOnString);
	m_pConnection->sendFmtData("ISON %s",szDec.data());
	if(m_pConnection->lagMeter())
		m_pConnection->lagMeter()->lagCheckRegister("@notify_ison",40); // not that reliable
	m_szIsOnString = "";
	m_bExpectingIsOn = true;
	// FIXME: #warning "And if can't send ?"
}


bool KviIsOnNotifyListManager::handleIsOn(KviIrcMessage *msg)
{
	if(!m_bExpectingIsOn)return false;

	// Check if it is our ISON
	// all the nicks must be on the IsOnList

	KviPointerList<QString> tmplist;
	tmplist.setAutoDelete(false);

	KviCString nk;
	const char * aux = msg->trailing();

	while(*aux)
	{
		nk = "";
		aux = kvi_extractToken(nk,aux,' ');
		if(nk.hasData())
		{
			bool bGotIt = false;
			QString dnk = m_pConnection->decodeText(nk.ptr());
			for(QString * s = m_pIsOnList->first();s && (!bGotIt);s = m_pIsOnList->next())
			{
				if(KviQString::equalCI(*s,dnk))
				{
					tmplist.append(s);
					bGotIt = true;
				}
			}
			if(!bGotIt)
			{
				// ops...not my userhost!
				if(_OUTPUT_VERBOSE)
					m_pConsole->output(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Notify list: Hey! You've used ISON behind my back? (I might be confused now...)"));
				return false;
			}
		}
	}

	// Ok...looks to be my ison (still not sure at 100%, but can't do better)
	if(m_pConnection->lagMeter())
		m_pConnection->lagMeter()->lagCheckComplete("@notify_ison");

	m_bExpectingIsOn = false;

	m_pOnlineList->clear();

	m_pIsOnList->setAutoDelete(false);

	// Ok...we have an IsOn reply here
	// The nicks in the IsOnList that are also in the reply are online, and go to the OnlineList
	// the remaining in the IsOnList are offline

	QString * s;

	for(s = tmplist.first();s;s = tmplist.next())
	{
		m_pIsOnList->removeRef(s);
		m_pOnlineList->append(s);
	}

	m_pIsOnList->setAutoDelete(true);
	// Ok...all the users that are online, are on the OnlineList
	// the remaining users are in the m_pIsOnList, and are no longer online

	// first the easy step: remove the users that have just left irc or have never been online
	// we're clearling the m_pIsOnList
	while((s = m_pIsOnList->first()))
	{
		if(m_pConsole->notifyListView()->findEntry(*s))
		{
			// has just left IRC... make him part
			notifyOffLine(*s);
		} // else has never been here

		m_pIsOnList->removeFirst(); // autodelete is true
	}

	// ok... complex step now: the remaining users in the userhost list are online
	// if they have been online before, just remove them from the list
	// otherwise they must be matched for masks
	// and eventually inserted in the notify view later

	KviIrcUserDataBase * db = console()->connection()->userDataBase();

	KviPointerList<QString> l;
	l.setAutoDelete(false);

	for(s = m_pOnlineList->first();s;s = m_pOnlineList->next())
	{
		if(KviUserListEntry * ent = m_pConsole->notifyListView()->findEntry(*s))
		{
			// the user was online from a previous notify session
			// might the mask have been changed ? (heh...this is tricky, maybe too much even)
			if(KVI_OPTION_BOOL(KviOption_boolNotifyListSendUserhostForOnlineUsers))
			{
				// user wants to be sure about online users....
				// check if he is on some channels
				if(ent->globalData()->nRefs() > 1)
				{
					// mmmh...we have more than one ref, so the user is at least in one query or channel
					// look him up on channels, if we find his entry, we can be sure that he is
					// still the right user
					KviPointerList<KviChannelWindow> * chlist = m_pConsole->connection()->channelList();
					for(KviChannelWindow * ch = chlist->first();ch;ch = chlist->next())
					{
						if(KviUserListEntry * le = ch->findEntry(*s))
						{
							l.append(s); // ok...found on a channel...we don't need an userhost to match him
							KviIrcMask mk(*s,le->globalData()->user(),le->globalData()->host());
							if(!doMatchUser(*s,mk))return true; // critical problems = have to restart!!!
							break;
						}
					}
				} // else Only one ref...we need an userhost to be sure (don't remove from the list)
			} else {
				// user wants no userhost for online users...we "hope" that everything will go ok.
				l.append(s);
			}
			//l.append(s); // we will remove him from the list
		} else {
			// the user was not online!
			// check if we have a cached mask
			if(db)
			{
				if(KviIrcUserEntry * ue = db->find(*s))
				{
					// already in the db... do we have a mask ?
					if(ue->hasUser() && ue->hasHost())
					{
						// yup! we have a complete mask to match on
						KviIrcMask mk(*s,ue->user(),ue->host());
						// lookup the user's name in the m_pRegUserDict
						if(!doMatchUser(*s,mk))return true; // critical problems = have to restart!!!
						l.append(s); // remove anyway
					}
				}
			}
		}
	}

	for(s = l.first();s;s = l.next())
	{
		m_pOnlineList->removeRef(s); // autodelete is true
	}

	if(m_pOnlineList->isEmpty())
	{
		if(m_pNotifyList->isEmpty())delayedNotifySession();
		else delayedIsOnSession();
	} else delayedUserhostSession();

	return true;
}

// FIXME: #warning "Nickname escapes (links) in the notifylist messages!"

bool KviIsOnNotifyListManager::doMatchUser(const QString &notifyString,const KviIrcMask & mask)
{
	QString * nam = m_pRegUserDict->find(notifyString);
	if(nam)
	{
		// ok...find the user
		if(KviRegisteredUser * u = g_pRegisteredUserDataBase->findUserByName(*nam))
		{
			// ok ... match the user
			if(u->matchesFixed(mask))
			{
				// new user online
				if(!(m_pConsole->notifyListView()->findEntry(mask.nick())))
				{
					notifyOnLine(mask.nick(),mask.user(),mask.host());
				} // else already online, and matching...all ok
			} else {
				// not matched.... has he been online before ?
				if(m_pConsole->notifyListView()->findEntry(mask.nick()))
				{
					// has been online just a sec ago, but now the mask does not match
					// either reguserdb has changed, or the user went offline and another one got his nick
					// in the meantime... (ugly situation anyway)
					notifyOffLine(mask.nick(),mask.user(),mask.host(),__tr2qs("registration mask changed, or nickname is being used by someone else"));
				} else {
					// has never been online
					if(_OUTPUT_VERBOSE)
						m_pConsole->output(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Notify list: \r!n\r%Q\r appears to be online, but the mask [%Q@\r!h\r%Q\r] does not match (registration mask does not match, or nickname is being used by someone else)"),&(mask.nick()),&(mask.user()),&(mask.host()));
				}
			}
		} else {
			// ops... unexpected inconsistency .... reguser db modified ?
			m_pConsole->output(KVI_OUT_SYSTEMWARNING,__tr2qs("Notify list: Unexpected inconsistency, registered user DB modified? (restarting)"));
			stop();
			start();
			return false; // critical ... exit from the call stack
		}
	} else {
		// ops...unexpected inconsistency
		m_pConsole->output(KVI_OUT_SYSTEMWARNING,__tr2qs("Notify list: Unexpected inconsistency, expected \r!n\r%Q\r in the registered user DB"),&notifyString);
	}
	return true;
}

void KviIsOnNotifyListManager::delayedUserhostSession()
{
	unsigned int iTimeout = KVI_OPTION_UINT(KviOption_uintNotifyListUserhostDelayTimeInSecs);
	if(iTimeout < 5)
	{
		// life first of all.
		// don't allow the user to suicide
		if(_OUTPUT_VERBOSE)
			m_pConsole->output(KVI_OUT_SYSTEMWARNING,
				__tr2qs("Notify list: USERHOST delay (%d sec) is too short, resetting to something more reasonable (5 sec)"),
			iTimeout);
		iTimeout = 5;
		KVI_OPTION_UINT(KviOption_uintNotifyListUserhostDelayTimeInSecs) = 5;
	}
	m_pDelayedUserhostTimer->setInterval(iTimeout * 1000);
	m_pDelayedUserhostTimer->setSingleShot(true);
	m_pDelayedUserhostTimer->start();
}

void KviIsOnNotifyListManager::newUserhostSession()
{
	buildUserhostList();
	if(m_pUserhostList->isEmpty())
	{
		// this is unexpected!
		m_pConsole->output(KVI_OUT_SYSTEMWARNING,__tr2qs("Notify list: Unexpected inconsistency, userhost list is empty!"));
		if(m_pOnlineList->isEmpty())
		{
			if(m_pNotifyList->isEmpty())delayedNotifySession();
			else delayedIsOnSession();
		} else delayedUserhostSession();
		return;
	}
	sendUserhost();
}

#define MAX_USERHOST_ENTRIES 5

void KviIsOnNotifyListManager::buildUserhostList()
{
	m_szUserhostString = "";
	m_pUserhostList->clear();

	m_pOnlineList->setAutoDelete(false);
	int i = 0;
	QString * s;
	while((s = m_pOnlineList->first()) && (i < MAX_USERHOST_ENTRIES))
	{
		if(!m_szUserhostString.isEmpty())m_szUserhostString.append(' ');
		m_szUserhostString.append(*s);
		m_pUserhostList->append(s);
		m_pOnlineList->removeFirst();
		i++;
	}
	m_pOnlineList->setAutoDelete(true);
}

void KviIsOnNotifyListManager::sendUserhost()
{
	if(_OUTPUT_PARANOIC)
		m_pConsole->output(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Notify list: Checking userhost for: %Q"),&m_szUserhostString);
	QByteArray ccc = m_pConnection->encodeText(m_szUserhostString);
	m_pConnection->sendFmtData("USERHOST %s",ccc.data());
	if(m_pConnection->lagMeter())
		m_pConnection->lagMeter()->lagCheckRegister("@notify_userhost",50);
	m_szUserhostString = "";
	m_bExpectingUserhost = true;
// FIXME: #warning "And if can't send ?"
}

bool KviIsOnNotifyListManager::handleUserhost(KviIrcMessage *msg)
{
	if(!m_bExpectingUserhost)return false;
	// first check for consistency: all the replies must be on the USERHOST list
	KviPointerList<KviIrcMask> tmplist;
	tmplist.setAutoDelete(true);

	KviCString nk;
	const char * aux = msg->trailing();

	while(*aux)
	{
		nk = "";
		aux = kvi_extractToken(nk,aux,' ');
		if(nk.hasData())
		{
			// split it in a mask
			KviCString nick;
			KviCString user;
			KviCString host;

			int idx = nk.findFirstIdx('=');
			if(idx != -1)
			{
				nick = nk.left(idx);
				if(nick.lastCharIs('*'))nick.cutRight(1);
				nk.cutLeft(idx + 1);
				if(nk.firstCharIs('+') || nk.firstCharIs('-'))nk.cutLeft(1);

				idx = nk.findFirstIdx('@');
				if(idx != -1)
				{
					user = nk.left(idx);
					nk.cutLeft(idx + 1);
					host = nk;
				} else {
					user = "*";
					host = nk;
				}

				bool bGotIt = false;
				QString szNick = m_pConnection->decodeText(nick.ptr());
				QString szUser = m_pConnection->decodeText(user.ptr());
				QString szHost = m_pConnection->decodeText(host.ptr());

				for(QString * s = m_pUserhostList->first();s && (!bGotIt);s = m_pUserhostList->next())
				{
					if(KviQString::equalCI(*s,szNick))
					{
						KviIrcMask * mk = new KviIrcMask(szNick,szUser,szHost);
						tmplist.append(mk);
						bGotIt = true;
						m_pUserhostList->removeRef(s);
					}
				}

				if(!bGotIt)
				{
					// ops...not my userhost!
					if(_OUTPUT_VERBOSE)
						m_pConsole->output(KVI_OUT_SYSTEMWARNING,__tr2qs("Notify list: Hey! You've used USERHOST behind my back? (I might be confused now...)"));
					return false;
				}
			} else {
				if(_OUTPUT_VERBOSE)
					m_pConsole->output(KVI_OUT_SYSTEMWARNING,__tr2qs("Notify list: Broken USERHOST reply from the server? (%s)"),nk.ptr());
			}
		}
	}

	// Ok...looks to be my usershot (still not sure at 100%, but can't do better)

	if(m_pConnection->lagMeter())
		m_pConnection->lagMeter()->lagCheckComplete("@notify_userhost");


	m_bExpectingUserhost = false;

	for(KviIrcMask * mk = tmplist.first();mk;mk = tmplist.next())
	{
		if(!doMatchUser(mk->nick(),*mk))return true; // have to restart!!!
	}

	if(!(m_pUserhostList->isEmpty()))
	{
		// ops...someone is no longer online ?
		while(QString * s = m_pUserhostList->first())
		{
			if(_OUTPUT_VERBOSE)
				m_pConsole->output(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Notify list: \r!n\r%Q\r appears to have gone offline before USERHOST reply was received, will recheck in the next loop"),s);
			m_pUserhostList->removeFirst();
		}

	}

	if(m_pOnlineList->isEmpty())
	{
		if(m_pNotifyList->isEmpty())delayedNotifySession();
		else delayedIsOnSession();
	} else delayedUserhostSession();

	return true;
}

void KviIsOnNotifyListManager::stop()
{
	if(!m_bRunning)return;

	if(m_pConnection->lagMeter())
		m_pConnection->lagMeter()->lagCheckAbort("@notify_userhost");
	if(m_pConnection->lagMeter())
		m_pConnection->lagMeter()->lagCheckAbort("@notify_ison");

	m_pDelayedNotifyTimer->stop();
	m_pDelayedIsOnTimer->stop();
	m_pDelayedUserhostTimer->stop();
	m_pConsole->notifyListView()->partAllButOne(m_pConnection->currentNickName());
	m_pRegUserDict->clear();
	m_pNotifyList->clear();
	m_pIsOnList->clear();
	m_pOnlineList->clear();
	m_pUserhostList->clear();
	m_szIsOnString = "";
	m_szUserhostString = "";
	m_bRunning = false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Stupid notify list manager
//
///////////////////////////////////////////////////////////////////////////////////////////////////

KviStupidNotifyListManager::KviStupidNotifyListManager(KviIrcConnection * pConnection)
: KviNotifyListManager(pConnection)
{
	m_pNickList = new KviPointerList<QString>;
	m_pNickList->setAutoDelete(true);
	m_iRestartTimer = 0;
}

KviStupidNotifyListManager::~KviStupidNotifyListManager()
{
	if(m_iRestartTimer)
	{
		killTimer(m_iRestartTimer);
		m_iRestartTimer = 0;
	}
	delete m_pNickList;
}

void KviStupidNotifyListManager::start()
{
	if(m_iRestartTimer)
	{
		killTimer(m_iRestartTimer);
		m_iRestartTimer = 0;
	}
	if(_OUTPUT_VERBOSE)
		m_pConsole->outputNoFmt(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Starting notify list"));
	buildNickList();
	if(m_pNickList->isEmpty())
	{
		if(_OUTPUT_VERBOSE)
			m_pConsole->outputNoFmt(KVI_OUT_SYSTEMMESSAGE,__tr2qs("No users in the notify list"));
		return; // Ok...no nicknames in the list
	}
	m_iNextNickToCheck = 0;
	m_pConsole->notifyListView()->partAllButOne(m_pConnection->currentNickName());
	sendIsOn();
}

void KviStupidNotifyListManager::sendIsOn()
{
	m_szLastIsOnMsg = "";
	QString * nick = m_pNickList->at(m_iNextNickToCheck);
	KVI_ASSERT(nick);

	int i = 0;
	while(nick && ((nick->length() + 5 + m_szLastIsOnMsg.length()) < 510))
	{
		KviQString::appendFormatted(m_szLastIsOnMsg," %Q",nick);
		nick = m_pNickList->next();
		i++;
	}
	if(_OUTPUT_PARANOIC)
		m_pConsole->output(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Notify list: Checking for: %Q"),&m_szLastIsOnMsg);
	QByteArray dat = m_pConnection->encodeText(m_szLastIsOnMsg);
	m_pConnection->sendFmtData("ISON%s",dat.data());

	if(m_pConnection->lagMeter())
		m_pConnection->lagMeter()->lagCheckRegister("@notify_naive",20);

	m_iNextNickToCheck += i;
}

bool KviStupidNotifyListManager::handleIsOn(KviIrcMessage * msg)
{
	if(m_pConnection->lagMeter())
		m_pConnection->lagMeter()->lagCheckComplete("@notify_naive");

	KviCString nk;
	const char * aux = msg->trailing();
	while(*aux)
	{
		nk = "";
		aux = kvi_extractToken(nk,aux,' ');
		if(nk.hasData())
		{
			QString nkd = m_pConnection->decodeText(nk.ptr());
			QString nksp = " " + nkd;
			m_szLastIsOnMsg.replace(nksp,"",Qt::CaseInsensitive);
			if(!(m_pConsole->notifyListView()->findEntry(nkd)))
			{
				// not yet notified
				notifyOnLine(nkd);
			}
		}
	}
	// ok...check the users that have left irc now...
	QStringList sl = m_szLastIsOnMsg.isEmpty()?QStringList():m_szLastIsOnMsg.split(' ',QString::SkipEmptyParts);

	for(QStringList::Iterator it = sl.begin();it != sl.end();++it)
	{
		if(m_pConsole->notifyListView()->findEntry(*it))
		{
			// has just left irc
			notifyOffLine(*it);
		} // else has never been here...
	}

	if(((unsigned int)m_iNextNickToCheck) >= m_pNickList->count())
	{
		// have to restart
		unsigned int iTimeout = KVI_OPTION_UINT(KviOption_uintNotifyListCheckTimeInSecs);
		if(iTimeout < 5)
		{
			// life first of all.
			// don't allow the user to suicide
			if(_OUTPUT_VERBOSE)
				m_pConsole->output(KVI_OUT_SYSTEMWARNING,
					__tr2qs("Notify list: Timeout (%d sec) is too short, resetting to something more reasonable (5 sec)"),
				iTimeout);
			iTimeout = 5;
			KVI_OPTION_UINT(KviOption_uintNotifyListCheckTimeInSecs) = 5;
		}
		m_iRestartTimer = startTimer(iTimeout * 1000);
	} else sendIsOn();
	return true;
}

void KviStupidNotifyListManager::timerEvent(QTimerEvent *e)
{
	if(e->timerId() == m_iRestartTimer)
	{
		killTimer(m_iRestartTimer);
		m_iRestartTimer = 0;
		m_iNextNickToCheck = 0;
		sendIsOn();
		return;
	}
	QObject::timerEvent(e);
}

void KviStupidNotifyListManager::stop()
{
	if(m_pConnection->lagMeter())
		m_pConnection->lagMeter()->lagCheckAbort("@notify_naive");

	if(m_iRestartTimer)
	{
		killTimer(m_iRestartTimer);
		m_iRestartTimer = 0;
	}
	m_pConsole->notifyListView()->partAllButOne(m_pConnection->currentNickName());

	// The ISON Method needs no stopping
}

void KviStupidNotifyListManager::buildNickList()
{
	const KviPointerHashTable<QString,KviRegisteredUser> * d = g_pRegisteredUserDataBase->userDict();
	KviPointerHashTableIterator<QString,KviRegisteredUser> it(*d);
	m_pNickList->clear();
	while(it.current())
	{
		QString notify;
		if(it.current()->getProperty("notify",notify))
		{
			m_pNickList->append(new QString(notify));
		}
		++it;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Watch notify list manager
//
///////////////////////////////////////////////////////////////////////////////////////////////////

KviWatchNotifyListManager::KviWatchNotifyListManager(KviIrcConnection * pConnection)
: KviNotifyListManager(pConnection)
{
	m_pRegUserDict = new KviPointerHashTable<QString,QString>(17,false);
	m_pRegUserDict->setAutoDelete(true);
}

KviWatchNotifyListManager::~KviWatchNotifyListManager()
{
	delete m_pRegUserDict;
}

void KviWatchNotifyListManager::buildRegUserDict()
{
	m_pRegUserDict->clear();

	const KviPointerHashTable<QString,KviRegisteredUser> * d = g_pRegisteredUserDataBase->userDict();
	KviPointerHashTableIterator<QString,KviRegisteredUser> it(*d);
	while(KviRegisteredUser * u = it.current())
	{
		QString notify;
		if(u->getProperty("notify",notify))
		{
			notify.trimmed();
			QStringList sl = notify.split(' ',QString::SkipEmptyParts);
			for(QStringList::Iterator it = sl.begin();it != sl.end();++it)
			{
				m_pRegUserDict->replace(*it,new QString(u->name()));
			}
		}
		++it;
	}
}

void KviWatchNotifyListManager::start()
{
	m_pConsole->notifyListView()->partAllButOne(m_pConnection->currentNickName());

	buildRegUserDict();

	QString watchStr;

	KviPointerHashTableIterator<QString,QString> it(*m_pRegUserDict);
	while(it.current())
	{
		QString nk = it.currentKey();
		if(nk.indexOf('*') == -1)
		{
			if((watchStr.length() + nk.length() + 2) > 501)
			{
				QByteArray dat = m_pConnection->encodeText(watchStr);
				m_pConnection->sendFmtData("WATCH%s",dat.data());
				if(_OUTPUT_VERBOSE)
					m_pConsole->output(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Notify list: Adding watch entries for %Q"),&watchStr);
				watchStr = "";
			}
			KviQString::appendFormatted(watchStr," +%Q",&nk);
		}
		++it;
	}

	if(!watchStr.isEmpty())
	{
		QByteArray dat = m_pConnection->encodeText(watchStr);
		m_pConnection->sendFmtData("WATCH%s",dat.data());
		if(_OUTPUT_VERBOSE)
			m_pConsole->output(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Notify list: Adding watch entries for %Q"),&watchStr);
	}
}
void KviWatchNotifyListManager::stop()
{
	m_pConsole->notifyListView()->partAllButOne(m_pConnection->currentNickName());
	m_pConnection->sendFmtData("WATCH clear");
	m_pRegUserDict->clear();
}

bool KviWatchNotifyListManager::doMatchUser(KviIrcMessage * msg,const QString &notifyString,const KviIrcMask & mask)
{
	QString * nam = m_pRegUserDict->find(notifyString);

	if(nam)
	{
		// ok...find the user
		if(KviRegisteredUser * u = g_pRegisteredUserDataBase->findUserByName(*nam))
		{
			// ok ... match the user
			if(u->matchesFixed(mask))
			{
				// new user online
				if(!(m_pConsole->notifyListView()->findEntry(mask.nick())))
				{
					notifyOnLine(mask.nick(),mask.user(),mask.host(),"watch");
				} else {
					// else already online, and matching...all ok
					if(msg->numeric() == RPL_NOWON)
					{
						// This is a reply to a /watch +something (should not happen, unless the user is messing) or to /watch l (user requested)
						notifyOnLine(mask.nick(),mask.user(),mask.host(),
								__tr2qs("watch entry listing requested by user"),false);
					} else {
						// This is a RPL_LOGON....we're desynched ?
						notifyOnLine(mask.nick(),mask.user(),mask.host(),
								__tr2qs("possible watch list desync"),false);
					}
				}
			} else {
				// not matched.... has he been online before ?
				if(m_pConsole->notifyListView()->findEntry(mask.nick()))
				{
					// has been online just a sec ago, but now the mask does not match
					// prolly the reguserdb has been changed
					notifyOffLine(mask.nick(),mask.user(),mask.host(),
						__tr2qs("registration mask changed or desync with the watch service"));
				} else {
					// has never been online
					if(_OUTPUT_VERBOSE)
						m_pConsole->output(KVI_OUT_SYSTEMMESSAGE,
							__tr("Notify list: \r!n\r%Q\r appears to be online, but the mask [%Q@\r!h\r%Q\r] does not match (watch: registration mask does not match, or nickname is being used by someone else)"),
							&(mask.nick()),&(mask.user()),&(mask.host()));
				}
			}
		} else {
			// ops... unexpected inconsistency .... reguser db modified ?
			m_pConsole->output(KVI_OUT_SYSTEMWARNING,
				__tr2qs("Notify list: Unexpected inconsistency, registered user DB modified? (watch: restarting)"));
			stop();
			start();
			return false; // critical ... exit from the call stack
		}
	} else {
		// not in our dictionary
		// prolly someone used /WATCH behind our back... bad boy!
		if(!(m_pConsole->notifyListView()->findEntry(mask.nick())))
		{
			notifyOnLine(mask.nick(),mask.user(),mask.host(),__tr2qs("watch entry added by user"));
		}
	}
	return true;
}

// FIXME: #warning "DEDICATED WATCH LIST VERBOSITY FLAG ? (To allow the user to use /WATCH l and manual /WATCH)"

bool KviWatchNotifyListManager::handleWatchReply(KviIrcMessage *msg)
{
	// 600: RPL_LOGON
	// :prefix 600 <target> <nick> <user> <host> <logintime> :logged online
	// 601: RPL_LOGON
	// :prefix 601 <target> <nick> <user> <host> <logintime> :logged offline
	// 604: PRL_NOWON
	// :prefix 604 <target> <nick> <user> <host> <logintime> :is online
	// 605: PRL_NOWOFF
	// :prefix 605 <target> <nick> <user> <host> 0 :is offline

// FIXME: #warning "Use the logintime in some way ?"

	const char * nk = msg->safeParam(1);
	const char * us = msg->safeParam(2);
	const char * ho = msg->safeParam(3);
	QString dnk = m_pConnection->decodeText(nk);
	QString dus = m_pConnection->decodeText(us);
	QString dho = m_pConnection->decodeText(ho);

	if((msg->numeric() == RPL_LOGON) || (msg->numeric() == RPL_NOWON))
	{
		KviIrcMask m(dnk,dus,dho);
		doMatchUser(msg,dnk,m);
		return true;

	} else if(msg->numeric() == RPL_WATCHOFF)
	{
		if(m_pConsole->notifyListView()->findEntry(dnk))
		{
			notifyOffLine(dnk,dus,dho,__tr2qs("removed from watch list"));
		} else {
			if(_OUTPUT_VERBOSE)
				m_pConsole->output(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Notify list: Stopped watching for \r!n\r%Q\r"),&dnk);
		}
		if(m_pRegUserDict->find(dnk))m_pRegUserDict->remove(dnk); // kill that

		return true;

	} else if((msg->numeric() == RPL_LOGOFF) || (msg->numeric() == RPL_NOWOFF))
	{
		if(m_pConsole->notifyListView()->findEntry(dnk))
		{
			notifyOffLine(dnk,dus,dho,__tr2qs("watch"));
		} else {
			if(msg->numeric() == RPL_NOWOFF)
			{
				// This is a reply to a /watch +something
				if(_OUTPUT_VERBOSE)
					m_pConsole->output(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Notify list: \r!n\r%Q\r is offline (watch)"),&dnk);
			} else {
				// This is a RPL_LOGOFF for an user that has not matched the reg-mask
				notifyOffLine(dnk,dus,dho,__tr2qs("unmatched watch list entry"));
			}
		}
		return true;
	}

	return false;
}


#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "KviNotifyList.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
