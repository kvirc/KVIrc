//=============================================================================
//
//   File : KviIrcServerParser_literalHandlers.cpp
//   Creation date : Thu Aug 3 2000 01:29:12 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)
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

#include "KviIrcServerParser.h"
#include "KviWindow.h"
#include "KviConsoleWindow.h"
#include "kvi_out.h"
#include "KviLocale.h"
#include "KviIrcSocket.h"
#include "KviOptions.h"
#include "KviIrcMask.h"
#include "KviChannelWindow.h"
#include "KviTopicWidget.h"
#include "KviMainWindow.h"
#include "KviControlCodes.h"
#include "KviQueryWindow.h"
#include "KviUserListView.h"
#include "KviAntiSpam.h"
#include "KviNickServRuleSet.h"
#include "KviIrcUserDataBase.h"
#include "KviApplication.h"
#include "KviRegisteredUserDataBase.h"
#include "kvi_debug.h"
#include "KviTimeUtils.h"
#include "KviUserAction.h"
#include "KviIrcConnection.h"
#include "KviIrcConnectionUserInfo.h"
#include "KviIrcConnectionTarget.h"
#include "KviIrcConnectionRequestQueue.h"
#include "KviIrcConnectionServerInfo.h"
#include "KviIrcConnectionStateData.h"
#include "KviIrcConnectionNetsplitDetectorData.h"
#include "KviIconManager.h"
#include "KviLagMeter.h"
#include "KviIrcServer.h"
#include "KviKvsEventTriggers.h"
#include "KviIrcNetwork.h"
#include "kvi_settings.h"
#include "KviIrcMessage.h"

#ifdef COMPILE_CRYPT_SUPPORT
#include "KviCryptEngine.h"
#include "KviCryptController.h"
#endif

#include "KviKvsScript.h"

#include <QDateTime>
#include <QByteArray>
#include <QLocale>

extern KviNickServRuleSet * g_pNickServRuleSet;

//
// PING
//

void KviIrcServerParser::parseLiteralPing(KviIrcMessage * msg)
{
	// PING
	// <optional_prefix> PING :<argument>
	msg->connection()->sendFmtData("PONG %s", msg->console()->connection()->encodeText(msg->allParams()).data());

	QString szPrefix = msg->connection()->decodeText(msg->safePrefix());
	QString szAllParams = msg->connection()->decodeText(msg->allParams());

	if(KVS_TRIGGER_EVENT_2_HALTED(KviEvent_OnPing, msg->console(), szPrefix, szAllParams))
		msg->setHaltOutput();

	if((!msg->haltOutput()) && KVI_OPTION_BOOL(KviOption_boolShowPingPong))
	{
		msg->console()->output(KVI_OUT_SERVERPING,
		    __tr2qs("Received ping from \r!s\r%Q\r (PING %Q), replied pong"),
		    &szPrefix, &szAllParams);
	}
}

//
// PONG
//

void KviIrcServerParser::parseLiteralPong(KviIrcMessage * msg)
{
	QString szPrefix = msg->connection()->decodeText(msg->safePrefix());
	QString szAllParams = msg->connection()->decodeText(msg->allParams());

	if(KVS_TRIGGER_EVENT_2_HALTED(KviEvent_OnPong, msg->console(), szPrefix, szAllParams))
		msg->setHaltOutput();

	if(msg->console()->connection()->lagMeter())
	{
		if(msg->console()->connection()->lagMeter()->lagCheckComplete("@ping@"))
			msg->setHaltOutput(); // was internally generated!
	}

	if((!msg->haltOutput()) && KVI_OPTION_BOOL(KviOption_boolShowPingPong))
	{
		msg->console()->output(KVI_OUT_SERVERPING,
		    __tr2qs("Received pong from \r!s\r%s\r (PONG %s)"), msg->safePrefix(), msg->allParams());
	}
}

//
// ERROR
//

void KviIrcServerParser::parseLiteralError(KviIrcMessage * msg)
{
	// ERROR
	// <optional_prefix> ERROR :<argument>
	// ERROR :Closing Link: phoenix.pragmaware.net (Ping timeout)

	QString szPrefix = msg->connection()->decodeText(msg->safePrefix());
	QString szParams = msg->connection()->decodeText(msg->allParams());

	if(KVS_TRIGGER_EVENT_2_HALTED(KviEvent_OnError, msg->console(), szPrefix, szParams))
		msg->setHaltOutput();

	if(!msg->haltOutput())
	{
		msg->console()->output(KVI_OUT_SERVERERROR,
		    __tr2qs("Server ERROR: %Q"), &szParams);
	}
}

//
// ACCOUNT
//

void KviIrcServerParser::parseLiteralAccount(KviIrcMessage * msg)
{
	// ACCOUNT
	// :<mask> ACCOUNT <account|*>
	// If the "account" is an asterisk, they have logged out.
	QString szNick, szUser, szHost;
	QString szAccount = msg->connection()->decodeText(msg->safeParam(0));
	msg->decodeAndSplitPrefix(szNick, szUser, szHost);

	KviIrcUserDataBase * db = msg->connection()->userDataBase();
	KviIrcUserEntry * e = db->find(szNick);
	KviConsoleWindow * console = msg->console();

	if(e)
	{
		if(szAccount == "*")
			e->setAccountName("");
		else
			e->setAccountName(szAccount);
	}

	if(KVS_TRIGGER_EVENT_4_HALTED(KviEvent_OnAccount, console, szNick, szUser, szHost, szAccount))
		msg->setHaltOutput();
}

//
// CHGHOST
//

void KviIrcServerParser::parseLiteralChghost(KviIrcMessage * msg)
{
	// CHGHOST
	// :<mask> CHGHOST <user> <new.host>
	QString szNick, szUser, szHost;
	QString szNewUser = msg->connection()->decodeText(msg->safeParam(0));
	QString szNewHost = msg->connection()->decodeText(msg->safeParam(1));
	msg->decodeAndSplitPrefix(szNick, szUser, szHost);

	KviIrcUserDataBase * db = msg->connection()->userDataBase();
	KviIrcUserEntry * e = db->find(szNick);

	if(e)
	{
		e->setUser(szNewUser);
		e->setHost(szNewHost);
	}

	KviConsoleWindow * console = msg->console();

	if(KVS_TRIGGER_EVENT_5_HALTED(KviEvent_OnHostChange, console, szNick, szUser, szHost, szNewUser, szNewHost))
		msg->setHaltOutput();

	for(auto & c : console->connection()->channelList())
	{
		if(c->isOn(szNick))
		{
			if(!msg->haltOutput())
			{
				if(szHost == szNewHost)
				{
					c->output(KVI_OUT_NICK, __tr2qs("\r!n\r%Q\r [%Q@\r!h\r%Q\r] now has user %Q"),
					    &szNick, &szUser, &szHost, &szNewUser);
				}
				else if(szUser == szNewUser)
				{
					c->output(KVI_OUT_NICK, __tr2qs("\r!n\r%Q\r [%Q@\r!h\r%Q\r] now has host \r!h\r%Q\r"),
					    &szNick, &szUser, &szHost, &szNewHost);
				}
				else
				{
					c->output(KVI_OUT_NICK, __tr2qs("\r!n\r%Q\r [%Q@\r!h\r%Q\r] now has user@host %Q@\r!h\r%Q\r"),
					    &szNick, &szUser, &szHost, &szNewUser, &szNewHost);
				}
			}
		}
	}

	KviQueryWindow * q = console->connection()->findQuery(szNick);
	if(q)
	{
		if(szHost == szNewHost)
		{
			q->output(KVI_OUT_NICK, __tr2qs("\r!n\r%Q\r [%Q@\r!h\r%Q\r] now has user %Q"),
			    &szNick, &szUser, &szHost, &szNewUser);
		}
		else if(szUser == szNewUser)
		{
			q->output(KVI_OUT_NICK, __tr2qs("\r!n\r%Q\r [%Q@\r!h\r%Q\r] now has host \r!h\r%Q\r"),
			    &szNick, &szUser, &szHost, &szNewHost);
		}
		else
		{
			q->output(KVI_OUT_NICK, __tr2qs("\r!n\r%Q\r [%Q@\r!h\r%Q\r] now has user@host %Q@\r!h\r%Q\r"),
			    &szNick, &szUser, &szHost, &szNewUser, &szNewHost);
		}
		q->updateLabelText();
	}
}

//
// JOIN
//

void KviIrcServerParser::parseLiteralJoin(KviIrcMessage * msg)
{
	// JOIN
	// [PLAIN] :<joiner_mask> JOIN :<channel>
	// [EXTJO] :<joiner_mask> JOIN <channel> <account|*> :gecos
	QString szNick, szUser, szHost;
	msg->decodeAndSplitPrefix(szNick, szUser, szHost);
	QString channel, szAccount, szReal;

	if(msg->connection()->stateData()->enabledCaps().contains("extended-join"))
	{
		szAccount = msg->connection()->decodeText(msg->safeParam(1));
		KviCString trailing = msg->safeTrailing();
		szReal = msg->connection()->decodeText(trailing.ptr());
		channel = msg->connection()->decodeText(msg->safeParam(0));
	}
	else
	{
		const char * encodedChan = msg->safeTrailing();
		channel = msg->connection()->decodeText(encodedChan);
	}

	if(channel.isEmpty())
	{
		// This is broken....
		UNRECOGNIZED_MESSAGE(msg, __tr2qs("Missing channel parameter in join message"));
		return;
	}

	// check for extended join syntax.
	// it is used in splits only (AFAIK)
	// nick!user@host JOIN :#channel\x07[o|v]
	const QChar * pExt = channel.constData();
	char chExtMode = 0;
	while(pExt->unicode() && (pExt->unicode() != 0x07))
		pExt++;
	if(pExt->unicode())
	{
		++pExt;
		if(pExt->unicode())
		{
			chExtMode = (char)pExt->unicode();
			channel.remove(channel.length() - 2, 2); // assuming that we're at the end (we should be)
		}                                            // else { senseless 0x07 in channel name ?
	}

	// Now lookup the channel
	KviConsoleWindow * console = msg->console();
	KviChannelWindow * chan = msg->connection()->findChannel(channel);

	bool bIsMe = IS_ME(msg, szNick);

	if(!chan)
	{
		// This must be me...(or desync)
		if(bIsMe)
		{
			msg->connection()->userInfoReceived(szUser, szHost);
			chan = msg->connection()->createChannel(channel); // New channel (will resurrect an eventual dead one too!)
		}
		else
		{
			// Someone is joining a non-existent channel!!!
			UNRECOGNIZED_MESSAGE(msg, __tr("Received a join message for an unknown channel, possible desync"));
			return;
		}

		int iFlags = 0;
		iFlags = msg->connection()->serverInfo()->modeFlagFromModeChar(chExtMode);

		KviUserListEntry * it = chan->join(szNick, szUser, szHost, iFlags);
		if(iFlags)
			chan->updateCaption();

		// FIXME: #warning "Trigger also OnMeVoice and OnMeOp here ?"
		if(!(it->globalData()->avatar()))
		{
			KviAvatar * av = console->defaultAvatarFromOptions();
			if(av)
			{
				it->globalData()->setAvatar(av);
				console->avatarChanged(av, szNick, szUser, szHost, QString());
			}
		}

		if(KVS_TRIGGER_EVENT_0_HALTED(KviEvent_OnMeJoin, chan))
			msg->setHaltOutput();

		// the channel requests must be sent AFTER we have created and accessed the chan
		// since it MAY happen that a sendFmtData() call fails by detecting a disconnect
		// and thus destroys the channel window!

		// If this problem persists in other parts of the KVIrc core then
		// we should disable disconnection detection during the parsing of a single
		// message in KviIrcSocket. See the comment in KviIrcSocket::processData() for more info.

		if(_OUTPUT_VERBOSE)
			console->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs("Requesting modes and ban list for \r!c\r%Q\r..."), &channel);
		msg->connection()->requestQueue()->enqueueChannel(chan);
	}
	else
	{
		// This must be someone else...(or desync)
		int iFlags = 0;
		iFlags = msg->connection()->serverInfo()->modeFlagFromModeChar(chExtMode);

		KviUserListEntry * it = chan->join(szNick, szUser, szHost, iFlags);

		// FIXME: #warning "Trigger also OnVoice and OnOp here ?"
		// Note: checkDefaultAvatar() makes a KviRegisteredUser lookup
		//       if later it is needed, make it return a pointer
		if(!(it->globalData()->avatar()))
			console->checkDefaultAvatar(it->globalData(), szNick, szUser, szHost);

		if(KVS_TRIGGER_EVENT_3_HALTED(KviEvent_OnJoin, chan, szNick, szUser, szHost))
			msg->setHaltOutput();
		// FIXME: #warning "WE COULD OPTIONALLY REQUEST A /WHO FOR THE USERS JOINING THAT WE DON'T KNOW THE HOST OF"
	}

	// We SHOULD have a database entry for the user now. If we do, we
	// can tack on some additional info if we're using extended-join.
	if(!szAccount.isEmpty())
	{
		KviIrcUserDataBase * db = msg->connection()->userDataBase();
		KviIrcUserEntry * e = db->find(szNick);
		if(e)
		{
			if(szAccount == "*")
				e->setAccountName("");
			else
				e->setAccountName(szAccount);

			e->setRealName(szReal);
		}
	}

	// Now say it to the world
	if(!msg->haltOutput())
	{
		// FIXME: #warning "CHECK IF MESSAGES GO TO CONSOLE OR NOT"

		if(chExtMode != 0)
		{
			chan->output(KVI_OUT_JOIN,
			    __tr2qs("\r!n\r%Q\r [%Q@\r!h\r%Q\r] has joined \r!c\r%Q\r [implicit +%c umode change]"),
			    &szNick, &szUser, &szHost, &channel, chExtMode);
		}
		else
		{
			chan->output(KVI_OUT_JOIN,
			    __tr2qs("\r!n\r%Q\r [%Q@\r!h\r%Q\r] has joined \r!c\r%Q\r"),
			    &szNick, &szUser, &szHost, &channel);
		}
	}

	QString szChans;
	int iChans = msg->connection()->getCommonChannels(szNick, szChans);
	KviQueryWindow * q = console->connection()->findQuery(szNick);
	if(q)
	{
		if(KVI_OPTION_BOOL(KviOption_boolEnableQueryTracing))
		{
			q->output(KVI_OUT_QUERYTRACE,
			    __tr2qs("\r!n\r%Q\r [%Q@\r!h\r%Q\r] has just joined \r!c\r%Q\r"), &szNick, &szUser,
			    &szHost, &channel);
			q->notifyCommonChannels(szNick, szUser, szHost, iChans, szChans);
		}
		else
		{
			q->updateLabelText();
		}
	}
}

//
// PART
//

void KviIrcServerParser::parseLiteralPart(KviIrcMessage * msg)
{
	// PART
	// :<source_mask> PART <channel> :<part message>
	QString szNick, szUser, szHost;
	msg->decodeAndSplitPrefix(szNick, szUser, szHost);

	QString szChan = msg->connection()->decodeText(msg->safeParam(0));

	// Now lookup the channel
	KviConsoleWindow * console = msg->console();
	KviChannelWindow * chan = msg->connection()->findChannel(szChan);

	if(!chan)
	{
		//chan = msg->context()->findDeadChannel(msg->safeParam(0));
		UNRECOGNIZED_MESSAGE(msg, __tr("Received a part message for an unknown channel, possible desync"));
		return;
	}

	// always decode with the textEncoding of the channel
	QString partMsg = msg->paramCount() > 1 ? chan->decodeText(msg->safeTrailing()) : QString();

	if(IS_ME(msg, szNick))
	{
		if(KVS_TRIGGER_EVENT_1_HALTED(KviEvent_OnMePart, chan, partMsg))
			msg->setHaltOutput();

		KviWindow * pOut = console;

		// It's me!
		if(chan->closeOnPart() || !KVI_OPTION_BOOL(KviOption_boolKeepChannelOpenOnPart))
		{
			g_pMainWindow->closeWindow(chan); // <-- deleted path
		}
		else
		{
			chan->part(szNick); // this will trigger the action too
			chan->setDeadChan();
			pOut = chan;
		}

		if(!msg->haltOutput())
		{
			if(KVI_OPTION_BOOL(KviOption_boolShowOwnParts))
			{
				if(partMsg.isEmpty())
					pOut->output(KVI_OUT_PART, __tr2qs("You have left channel \r!c\r%Q\r"), &szChan);
				else
					pOut->output(KVI_OUT_PART, __tr2qs("You have left channel \r!c\r%Q\r: %Q"), &szChan, &partMsg);
			}
		}
	}
	else
	{
		// Someone else
		if(KVS_TRIGGER_EVENT_4_HALTED(KviEvent_OnPart, chan, szNick, szUser, szHost, partMsg))
			msg->setHaltOutput();

		chan->part(szNick);

		if(!msg->haltOutput())
		{
			if(!partMsg.isEmpty())
				chan->output(KVI_OUT_PART,
				    __tr2qs("\r!n\r%Q\r [%Q@\r!h\r%Q\r] has left \r!c\r%Q\r: %Q"), &szNick, &szUser,
				    &szHost, &szChan, &partMsg);
			else
				chan->output(KVI_OUT_PART,
				    __tr2qs("\r!n\r%Q\r [%Q@\r!h\r%Q\r] has left \r!c\r%Q\r"), &szNick, &szUser,
				    &szHost, &szChan);
		}

		if(KVI_OPTION_BOOL(KviOption_boolEnableQueryTracing))
		{
			QString szChans;
			int iChans = console->connection()->getCommonChannels(szNick, szChans);
			KviQueryWindow * q = console->connection()->findQuery(szNick);
			if(q)
			{
				if(!partMsg.isEmpty())
					q->output(KVI_OUT_QUERYTRACE,
					    __tr2qs("\r!nc\r%Q\r [%Q@\r!h\r%Q\r] has just left \r!c\r%Q\r: %Q"),
					    &szNick, &szUser, &szHost, &szChan, &partMsg);
				else
					q->output(KVI_OUT_QUERYTRACE,
					    __tr2qs("\r!nc\r%Q\r [%Q@\r!h\r%Q\r] has just left \r!c\r%Q\r"),
					    &szNick, &szUser, &szHost, &szChan);
				q->notifyCommonChannels(szNick, szUser, szHost, iChans, szChans);
			}
		}
	}
}

//
// QUIT
//

void KviIrcServerParser::parseLiteralQuit(KviIrcMessage * msg)
{
	// QUIT
	// :<source_mask> QUIT :<quit message>
	QString szNick, szUser, szHost;
	msg->decodeAndSplitPrefix(szNick, szUser, szHost);

	KviConsoleWindow * console = msg->console();

	// NETSPLIT DETECTION STUFF
	// this doesn't need to be decoded for the moment
	const char * aux = msg->safeTrailing();
	bool bWasSplit = false;
	//determine if signoff string matches "%.% %.%", and only one space (from eggdrop code)
	char * p = (char *)strchr(aux, ' ');
	if(p && (p == (char *)strrchr(aux, ' ')))
	{
		char * daSpace = p;
		// one space detected. go ahead
		char *z1, *z2;
		*p = 0;
		z1 = (char *)strchr(p + 1, '.');
		z2 = (char *)strchr(aux, '.');
		if(z1 && z2 && (*(z1 + 1) > 47) && (z1 - 1 != p) && (z2 + 1 != p) && (z2 != aux) && console->connection())
		{
			// server split, or else it looked like it anyway
			KviIrcConnectionNetsplitDetectorData * ndd = msg->connection()->netsplitDetectorData();
			*p = ' ';
			bWasSplit = true;

			time_t curTime = kvi_unixTime();
			int diff = ((unsigned int)curTime) - ((unsigned int)ndd->lastNetsplitOnQuitTime());
			bool bDuplicate = false;

			QString szReason = aux;
			if(diff < 6)
			{
				if(KviQString::equalCI(ndd->lastNetsplitOnQuitReason(), szReason))
				{
					bDuplicate = true;
				}
			}

			ndd->setLastNetsplitOnQuitTime(curTime);
			ndd->setLastNetsplitOnQuitReason(szReason);

			if(!bDuplicate)
			{
				KviCString sz1(aux, daSpace - aux);
				KviCString sz2(daSpace + 1);

				QString szD1 = msg->connection()->decodeText(sz1.ptr());
				QString szD2 = msg->connection()->decodeText(sz2.ptr());
				if(!KVS_TRIGGER_EVENT_2_HALTED(KviEvent_OnNetsplit, console, szD1, szD2))
				{
					if(!msg->haltOutput())
						console->output(KVI_OUT_SPLIT, __tr2qs("Netsplit detected: %s"), aux);
				}
			}
		}
		else
		{
			*p = ' ';
		}
	}

	// FIXME: #warning "Add a netsplit parameter ?"
	if(KviKvsEventManager::instance()->hasAppHandlers(KviEvent_OnQuit))
	{
		// compute the channel list
		QString chanlist;
		QString szReason = msg->connection()->decodeText(msg->safeTrailing());

		if(console->connection())
		{
			for(auto & c : console->connection()->channelList())
			{
				if(c->isOn(szNick))
				{
					if(chanlist.isEmpty())
						chanlist = c->windowName();
					else
					{
						chanlist.append(',');
						chanlist.append(c->windowName());
					}
				}
			}
		}

		KviKvsVariantList vList;
		vList.append(szNick);
		vList.append(szUser);
		vList.append(szHost);
		vList.append(szReason);
		vList.append(chanlist);

		if(KviKvsEventManager::instance()->trigger(KviEvent_OnQuit, console, &vList))
			msg->setHaltOutput();
	}

	for(auto & c : console->connection()->channelList())
	{
		if(c->part(szNick))
		{
			if(!msg->haltOutput())
			{
				QString quitMsg = c->decodeText(msg->safeTrailing());

				if(bWasSplit)
				{
					quitMsg.prepend("NETSPLIT ");
				}

				if(!msg->haltOutput())
					c->output(KVI_OUT_QUIT,
					    __tr2qs("\r!n\r%Q\r [%Q@\r!h\r%Q\r] has quit IRC: %Q"),
					    &szNick, &szUser, &szHost, &quitMsg);
			}
		}
	}

	if(!msg->haltOutput())
	{
		KviQueryWindow * q = msg->connection()->findQuery(szNick);
		if(q)
		{
			QString quitMsg = q->decodeText(msg->safeTrailing());
			if(bWasSplit)
			{
				quitMsg.prepend("NETSPLIT ");
			}
			q->output(KVI_OUT_QUIT, __tr2qs("\r!n\r%Q\r [%Q@\r!h\r%Q\r] has quit IRC: %Q"),
			    &szNick, &szUser, &szHost, &quitMsg);
		}
	}
}

//
// KICK
//

void KviIrcServerParser::parseLiteralKick(KviIrcMessage * msg)
{
	// KICK
	// :<source_mask> KICK <channel> <nick> :<kick message>
	QString szNick, szUser, szHost;
	msg->decodeAndSplitPrefix(szNick, szUser, szHost);

	QString szChan = msg->connection()->decodeText(msg->safeParam(0));
	QString victim = msg->connection()->decodeText(msg->safeParam(1));

	KviConsoleWindow * console = msg->console();
	KviChannelWindow * chan = msg->connection()->findChannel(szChan);

	if(!chan)
	{
		// Oops, desync with the server.
		UNRECOGNIZED_MESSAGE(msg, __tr("Received a kick message for an unknown channel, possible desync"));
		return;
	}

	QString szKickMsg = chan->decodeText(msg->safeTrailing());

	if(IS_ME(msg, victim))
	{
		// Oops...I have been kicked
		if(KVS_TRIGGER_EVENT_4_HALTED(KviEvent_OnMeKick, chan,
		       szNick, szUser, szHost, szKickMsg))
			msg->setHaltOutput();
		if(!KVI_OPTION_STRING(KviOption_stringOnMeKickedSound).isEmpty())
		{
			KviKvsVariantList soundParams{new KviKvsVariant{KVI_OPTION_STRING(KviOption_stringOnMeKickedSound)}};
			KviKvsScript::run("snd.play $0", nullptr, &soundParams);
		}

		QString szPass = chan->hasChannelMode('k') ? chan->channelModeParam('k') : "";

		if(KVI_OPTION_BOOL(KviOption_boolKeepChannelOpenOnKick))
		{
			chan->userAction(szNick, szUser, szHost, KVI_USERACTION_KICK);
			chan->part(victim);
			chan->setDeadChan();

			if(!msg->haltOutput())
			{
				// FIXME: #warning "OPTION FOR THIS TO GO TO THE CONSOLE!"
				chan->output(KVI_OUT_MEKICK,
				    __tr2qs("You have been kicked from \r!c\r%Q\r by \r!n\r%Q\r [%Q@\r!h\r%Q\r]: %Q"),
				    &szChan, &szNick, &szUser, &szHost, &szKickMsg);
			}
		}
		else
		{
			g_pMainWindow->closeWindow(chan); // <-- deleted path

			if(!msg->haltOutput())
			{
				// FIXME: #warning "This could go also to the active window!"
				console->output(KVI_OUT_MEKICK,
				    __tr2qs("You have been kicked from \r!c\r%Q\r by \r!n\r%Q\r [%Q@\r!h\r%Q\r]: %Q"),
				    &szChan, &szNick, &szUser, &szHost, &szKickMsg);
			}
		}
		if(KVI_OPTION_BOOL(KviOption_boolRejoinChannelOnKick))
		{
			if(_OUTPUT_VERBOSE)
				console->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs("Attempting to rejoin \r!c\r%Q\r..."), &szChan);
			QByteArray szC = msg->connection()->encodeText(szChan);
			if(!szPass.isEmpty())
			{
				QByteArray szP = msg->connection()->encodeText(szPass);
				msg->connection()->sendFmtData("JOIN %s %s", szC.data(), szP.data());
			}
			else
				msg->connection()->sendFmtData("JOIN %s", szC.data());
		}
	}
	else
	{
		// ok...someone else has been kicked
		if(KVS_TRIGGER_EVENT_5_HALTED(KviEvent_OnKick, chan,
		       szNick, szUser, szHost, victim, szKickMsg))
			msg->setHaltOutput();

		KviIrcUserEntry * e = msg->connection()->userDataBase()->find(victim);

		QString szVHost;
		QString szVUser;

		if(e)
		{
			szVHost = e->host();
			szVUser = e->user();
		}
		else
		{
			szVHost = "*";
			szVUser = "*";
		}

		chan->userAction(szNick, szUser, szHost, KVI_USERACTION_KICK);

		chan->part(victim);

		if(!msg->haltOutput())
		{
			// FIXME: #warning "OPTION FOR THIS TO GO TO THE CONSOLE!"
			chan->output(KVI_OUT_KICK,
			    __tr2qs("\r!n\r%Q\r [%Q@\r!h\r%Q\r] has been kicked from \r!c\r%Q\r by \r!n\r%Q\r [%Q@\r!h\r%Q\r]: %Q"),
			    &victim, &szVUser, &szVHost, &szChan, &szNick, &szUser, &szHost, &szKickMsg);
		}

		if(KVI_OPTION_BOOL(KviOption_boolEnableQueryTracing))
		{
			KviQueryWindow * q = console->connection()->findQuery(victim);
			if(q)
			{
				QString szChans;
				int iChans = console->connection()->getCommonChannels(victim, szChans);
				q->output(KVI_OUT_QUERYTRACE,
				    __tr2qs("\r!n\r%Q\r [%Q@\r!h\r%Q\r] has just been kicked from \r!c\r%Q\r by \r!n\r%Q\r [%Q@\r!h\r%Q\r]: %Q"),
				    &victim, &szVUser, &szVHost, &szChan,
				    &szNick, &szUser, &szHost, &szKickMsg);
				q->notifyCommonChannels(victim, szVUser, szVHost, iChans, szChans);
			}
		}
	}
}

#ifdef COMPILE_CRYPT_SUPPORT
#define DECRYPT_IF_NEEDED(target, txt, type, type2, buffer, retptr, retmsgtype)                                                      \
	if(KviCryptSessionInfo * cinf = target->cryptSessionInfo())                                                                      \
	{                                                                                                                                \
		if(cinf->m_bDoDecrypt)                                                                                                       \
		{                                                                                                                            \
			switch(cinf->m_pEngine->decrypt(txt, buffer))                                                                            \
			{                                                                                                                        \
				case KviCryptEngine::DecryptOkWasEncrypted:                                                                          \
					retptr = buffer.ptr();                                                                                           \
					retmsgtype = type2;                                                                                              \
					break;                                                                                                           \
				case KviCryptEngine::DecryptOkWasPlainText:                                                                          \
				case KviCryptEngine::DecryptOkWasEncoded:                                                                            \
					retptr = buffer.ptr();                                                                                           \
					retmsgtype = type;                                                                                               \
					break;                                                                                                           \
				default: /* also case KviCryptEngine::DecryptError: */                                                               \
				{                                                                                                                    \
					QString szEngineError = cinf->m_pEngine->lastError();                                                            \
					target->output(KVI_OUT_SYSTEMERROR,                                                                              \
					    __tr2qs("The following message appears to be encrypted, but the encryption engine failed to decode it: %Q"), \
					    &szEngineError);                                                                                             \
					retptr = txt + 1;                                                                                                \
					retmsgtype = type;                                                                                               \
				}                                                                                                                    \
				break;                                                                                                               \
			}                                                                                                                        \
		}                                                                                                                            \
		else                                                                                                                         \
			retptr = txt, retmsgtype = type;                                                                                         \
	}                                                                                                                                \
	else                                                                                                                             \
		retptr = txt, retmsgtype = type;
#else //COMPILE_CRYPT_SUPPORT
#define DECRYPT_IF_NEEDED(target, txt, type, type2, buffer, retptr, retmsgtype)        \
	retptr = txt;                                                                      \
	retmsgtype = type;
#endif //COMPILE_CRYPT_SUPPORT

enum PrivmsgIdentifyMsgCapState
{
	IdentifyMsgCapNotUsed,
	IdentifyMsgCapUsedNotIdentified,
	IdentifyMsgCapUsedIdentified
};

void KviIrcServerParser::parseLiteralPrivmsg(KviIrcMessage * msg)
{
	// PRIVMSG
	// :source PRIVMSG <target> :<message>
	QString szSourceNick, szSourceUser, szSourceHost;
	msg->decodeAndSplitPrefix(szSourceNick, szSourceUser, szSourceHost);

	// update the user entry in the database right away
	KviIrcUserDataBase * db = msg->connection()->userDataBase();
	KviIrcUserEntry * e = db->find(szSourceNick);
	if (e)
	{
		e->setUser(szSourceUser);
		e->setHost(szSourceHost);
	}

	QString szTarget = msg->connection()->decodeText(msg->safeParam(0));
	QString szMsg = msg->connection()->decodeText(msg->safeTrailing());

	QString szTargetNick, szTargetUser, szTargetHost;
	msg->decodeAndSplitMask(szTarget.toLatin1().data(), szTargetNick, szTargetUser, szTargetHost);

	KviConsoleWindow * console = msg->console();
	KviRegisteredUser * uSource = msg->connection()->userDataBase()->registeredUser(szSourceNick, szSourceUser, szSourceHost);
	//Highlight it?

	PrivmsgIdentifyMsgCapState eCapState = IdentifyMsgCapNotUsed;

	KviCString pTrailing = msg->trailingString();
	if(pTrailing)
	{
		if(msg->connection()->stateData()->identifyMsgCapabilityEnabled())
		{
			switch(*(pTrailing.ptr()))
			{
				case '+':
					// message from identified user
					eCapState = IdentifyMsgCapUsedIdentified;
					pTrailing.cutLeft(1); // kill the first char
					break;
				case '-':
					eCapState = IdentifyMsgCapUsedNotIdentified;
					pTrailing.cutLeft(1); // kill the first char
					break;
				default:
					// nothing interesting
					break;
			}
		}

		if(*(pTrailing.ptr()) == 0x01)
		{
			// FIXME: 	#warning "DEDICATED CTCP WINDOW ?"
			if(pTrailing.len() > 1)
			{
				if(pTrailing.lastCharIs(0x01))
					pTrailing.cutRight(1);
				pTrailing.cutLeft(1);
				KviCtcpMessage ctcp;
				ctcp.msg = msg;
				ctcp.pData = pTrailing.ptr();
				KviIrcMask mSource(szSourceNick, szSourceUser, szSourceHost); // FIXME!
				ctcp.pSource = &mSource;
				ctcp.szTarget = szTarget;
				ctcp.bIgnored = false;
				ctcp.bIsFlood = false;
				ctcp.bUnknown = false;
				parseCtcpRequest(&ctcp);
				return;
			}
		}
	}

	QString szOriginalTarget = szTarget;
	QString szPrefixes;

	// check if the channel has some leading mode prefixes
	while((szTarget.length() > 0) && console->connection()->serverInfo()->supportedStatusMsgPrefixes().contains(szTarget[0]))
	{
		szPrefixes += szTarget[0];
		szTarget.remove(0, 1);
	}

	// Query PRIVMSG
	if(msg->connection()->serverInfo()->supportedChannelTypes().indexOf(szTarget[0]) == -1)
	{
		szTarget = szOriginalTarget;

		//Ignore it?
		if(uSource)
		{
			if(uSource->isIgnoreEnabledFor(KviRegisteredUser::Query))
			{
				if(KVS_TRIGGER_EVENT_6_HALTED(KviEvent_OnIgnoredMessage, msg->console(), szSourceNick, szSourceUser, szSourceHost, szTarget, szMsg, msg->messageTagsKvsHash()))
					return;

				if(KVI_OPTION_BOOL(KviOption_boolVerboseIgnore))
					console->output(KVI_OUT_IGNORE, msg->serverTime(), __tr2qs("Ignoring query-PRIVMSG from \r!nc\r%Q\r [%Q@\r!h\r%Q\r]: %Q"), &szSourceNick, &szSourceUser, &szSourceHost, &szMsg);
				return;
			}
		}
		// FIXME: 	#warning "PROCESS MULTIMEDIA FILE REQUESTS"

		//		if(g_pOptions->m_bListenToMultimediaFileRequests)
		//		{
		//			if(*aux == '!')
		//			{
		//				const char *tmp = aux;
		//				tmp++;
		//				if(kvi_strEqualCI(tmp,m_pFrm->m_global.szCurrentNick.ptr()))
		//				{
		//					// A multimedia file request ?
		//					tmp += m_pFrm->m_global.szCurrentNick.len();
		//					if(((*tmp) == ' ') || ((*tmp) == '\t'))
		//					{
		//						while(((*tmp) == ' ') || ((*tmp) == '\t'))tmp++;
		//						if(*tmp)
		//						{
		//							KviCString file = tmp;
		//							KviCString filePath;
		//							m_pFrm->findMultimediaFileOffert(filePath,file);
		//							if(filePath.hasData())
		//							{
		//								m_pFrm->activeWindow()->output(KVI_OUT_INTERNAL,msg->serverTime(),__tr("%s requests previously offered file %s: sending (%s)"),talker.nick(),file.ptr(),filePath.ptr());
		//								KviCString cmd(KviCString::Format,"DCC SEND %s %s",talker.nick(),filePath.ptr());
		//								m_pFrm->m_pUserParser->parseUserCommand(cmd,m_pConsole);
		//								return;

		//							} else {
		//								m_pFrm->activeWindow()->output(KVI_OUT_INTERNAL,msg->serverTime(),__tr("%s requests file %s: no such file was offered, ignoring"),talker.nick(),file.ptr());
		//								return;
		//							}
		//						}
		//					}
		//				}
		//			}
		//		}

		// "znc.in/self-message" capability: Handle a replayed message from ourselves to someone else.
		bool bSelfMessage = IS_ME(msg, szSourceNick);
		const QString &szOtherNick = bSelfMessage ? szTargetNick : szSourceNick;
		const QString &szOtherUser = bSelfMessage ? szTargetUser : szSourceUser;
		const QString &szOtherHost = bSelfMessage ? szTargetHost : szSourceHost;

		// A query request
		// do we have a matching window ?
		KviQueryWindow * query = msg->connection()->findQuery(szOtherNick);

		if(!query)
		{
			// New query requested. Check if we really should create it or not

			// first of all the anti spam, if desired.
			// the antispam blocks anything else
			// Eventually we could trigger a special event to notify the user of the
			// spam message...
			if(KVI_OPTION_BOOL(KviOption_boolUseAntiSpamOnPrivmsg))
			{
				KviCString theMsg = msg->trailingString();
				if(theMsg)
				{
					KviCString spamWord;
					if(kvi_mayBeSpam(theMsg, spamWord))
					{
						// FIXME: OnSpam ?
						if(!(msg->haltOutput() || KVI_OPTION_BOOL(KviOption_boolSilentAntiSpam)))
						{
							QString szMsg = msg->connection()->decodeText(msg->safeTrailing());
							console->output(KVI_OUT_SPAM, msg->serverTime(),
							    __tr2qs("Spam PRIVMSG from \r!n\r%Q\r [%Q@\r!h\r%Q\r]: %Q (matching spamword \"%s\")"),
							    &szSourceNick, &szSourceUser, &szSourceHost, &szMsg, spamWord.ptr());
						}
						return;
					}
				}
			}

			// this is not a spam, or at least it hasn't been recognized as spam

			// user option ? (this should again override any script)
			// if the scripters want really to force the query creation they can do
			// it manually or they can set the option to true at KVIrc startup
			if(KVI_OPTION_BOOL(KviOption_boolCreateQueryOnPrivmsg))
			{
				QString szMsg = msg->connection()->decodeText(msg->safeTrailing());

				// We still want to create it
				// Give the scripter a chance to filter it out again
				if(KVS_TRIGGER_EVENT_5_HALTED(KviEvent_OnQueryWindowRequest,
				       console, szSourceNick, szSourceUser, szSourceHost, szMsg, msg->messageTagsKvsHash()))
				{
					// check if the scripter hasn't created it
					query = msg->connection()->findQuery(szOtherNick);
				}
				else
				{
					// no query yet, create it!
					// this will trigger OnQueryWindowCreated
					query = console->connection()->createQuery(szOtherNick);
					// and this will trigger OnQueryTargetAdded
					query->setTarget(szOtherNick, szOtherUser, szOtherHost);
				}

				//check for query, since the user could have halt'ed its creation
				if(query)
				{
					if(!KVI_OPTION_STRING(KviOption_stringOnNewQueryOpenedSound).isEmpty())
					{
						KviKvsVariantList soundParams{new KviKvsVariant{KVI_OPTION_STRING(KviOption_stringOnNewQueryOpenedSound)}};
						KviKvsScript::run("snd.play $0", nullptr, &soundParams);
					}
				}
			}
		}

		// ok, now we either have a query or not
		if(query)
		{
			// ok, we have the query. Trigger the user action anyway
			query->userAction(szSourceNick, szSourceUser, szSourceHost, KVI_USERACTION_PRIVMSG);

			// decrypt the message if needed
			KviCString szBuffer;
			const char * txtptr;
			int msgtype;

			DECRYPT_IF_NEEDED(query, msg->safeTrailing(), KVI_OUT_QUERYPRIVMSG, KVI_OUT_QUERYPRIVMSGCRYPTED, szBuffer, txtptr, msgtype)

			// trigger the script event and eventually kill the output
			QString szMsgText = query->decodeText(txtptr);
			if(KVS_TRIGGER_EVENT_6_HALTED(KviEvent_OnQueryMessage, query, szSourceNick, szSourceUser, szSourceHost, szMsgText, (kvs_int_t)(msgtype == KVI_OUT_QUERYPRIVMSGCRYPTED), msg->messageTagsKvsHash()))
				msg->setHaltOutput();

			if(!KVI_OPTION_STRING(KviOption_stringOnQueryMessageSound).isEmpty() && !query->hasAttention())
			{
				KviKvsVariantList soundParams(new KviKvsVariant(KVI_OPTION_STRING(KviOption_stringOnQueryMessageSound)));
				KviKvsScript::run("snd.play $0", query, &soundParams);
			}

			// spit out the message text
			if(!msg->haltOutput())
			{
				int iFlags = 0;
				if(!query->hasAttention(KviWindow::MainWindowIsVisible))
				{
					if(KVI_OPTION_BOOL(KviOption_boolFlashQueryWindowOnNewMessages))
					{
						// avoid double window flashing
						iFlags |= KviConsoleWindow::NoWindowFlashing;
						query->demandAttention();
					}
					if(KVI_OPTION_BOOL(KviOption_boolPopupNotifierOnNewQueryMessages))
					{
						// don't send the message to the notifier twice
						iFlags |= KviConsoleWindow::NoNotifier;
						QString szMsg = KviQString::toHtmlEscaped(szMsgText);
						//qDebug("KviIrcServerParser_literalHandlers.cpp:908 debug: %s",szMsgText.data());
						g_pApp->notifierMessage(query, KviIconManager::QueryPrivMsg, szMsg, KVI_OPTION_UINT(KviOption_uintNotifierAutoHideTime));
					}
				}

				// if the message is identified (identify-msg CAP) then re-add the +/- char at the beginning
				if(eCapState != IdentifyMsgCapNotUsed)
					szMsgText = QString::fromLatin1("%1%2").arg(eCapState == IdentifyMsgCapUsedIdentified ? "+" : "-").arg(szMsgText);

				console->outputPrivmsg(query, msgtype, szSourceNick, szSourceUser, szSourceHost, szMsgText, iFlags, "", "", msg->serverTime());
			}
		}
		else
		{
			// no query creation: no decryption possible
			// trigger the query message event in the console
			QString szMsgText = msg->connection()->decodeText(msg->safeTrailing());
			if(KVS_TRIGGER_EVENT_6_HALTED(KviEvent_OnQueryMessage, console, szSourceNick, szSourceUser, szSourceHost, szMsgText, (kvs_int_t)0, msg->messageTagsKvsHash()))
				msg->setHaltOutput();

			// we don't have a query here!
			if(!KVI_OPTION_STRING(KviOption_stringOnQueryMessageSound).isEmpty() && !console->hasAttention())
			{
				KviKvsVariantList soundParams{new KviKvsVariant{KVI_OPTION_STRING(KviOption_stringOnQueryMessageSound)}};
				KviKvsScript::run("snd.play $0", console, &soundParams);
			}

			// if the message is identified (identify-msg CAP) then re-add the +/- char at the beginning
			if(eCapState != IdentifyMsgCapNotUsed)
				szMsgText = QString::fromLatin1("%1%2").arg(eCapState == IdentifyMsgCapUsedIdentified ? "+" : "-").arg(szMsgText);

			// spit the message text out
			if(!msg->haltOutput())
			{
				KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolExternalMessagesToActiveWindow) ? console->activeWindow() : (KviWindow *)(console);

				if(KviIrcConnection * pConnection = console->connection())
				{
					KviWindow * aWin = console->activeWindow();
					if((aWin->type() == KviWindow::Channel) && ((KviChannelWindow *)aWin)->isOn(szOtherNick))
						pOut = aWin;
					else
					{
						for(auto & c : pConnection->channelList())
						{
							if(c->isOn(szOtherNick))
							{
								pOut = c;
								break;
							}
						}
					}
				}

				pOut->output(KVI_OUT_QUERYPRIVMSG, msg->serverTime(), "[PRIVMSG \r!nc\r%Q\r]: %Q", &szSourceNick, &szMsgText);
			}
		}
	}
	else
	{
		// Channel PRIVMSG
		KviChannelWindow * chan = msg->connection()->findChannel(szTarget);

		//Ignore it?
		if(uSource)
		{
			if(uSource->isIgnoreEnabledFor(KviRegisteredUser::Channel))
			{
				if(KVS_TRIGGER_EVENT_6_HALTED(KviEvent_OnIgnoredMessage, msg->console(), szSourceNick, szSourceUser, szSourceHost, szTarget, szMsg, msg->messageTagsKvsHash()))
					return;

				if(KVI_OPTION_BOOL(KviOption_boolVerboseIgnore))
				{
					console->output(KVI_OUT_IGNORE, msg->serverTime(), __tr2qs("Ignoring channel-PRIVMSG from \r!nc\r%Q\r [%Q@\r!h\r%Q\r]: %Q"), &szSourceNick, &szSourceUser, &szSourceHost, &szMsg);
				}
				return;
			}
		}

		if(!chan)
		{
			if(!msg->haltOutput())
			{
				QString szMsgText = msg->connection()->decodeText(msg->safeTrailing());

				// if the message is identified (identify-msg CAP) then re-add the +/- char at the beginning
				if(eCapState != IdentifyMsgCapNotUsed)
					szMsgText = QString::fromLatin1("%1%2").arg(eCapState == IdentifyMsgCapUsedIdentified ? "+" : "-").arg(szMsgText);

				KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolOperatorMessagesToActiveWindow) ? console->activeWindow() : (KviWindow *)(console);
				QString szBroad = QString("[>> %1] %2").arg(szOriginalTarget, szMsgText);
				console->outputPrivmsg(pOut, KVI_OUT_BROADCASTPRIVMSG, szSourceNick, szSourceUser, szSourceHost, szBroad, 0, "", "", msg->serverTime());
			}
		}
		else
		{
			chan->userAction(szSourceNick, szSourceUser, szSourceHost, KVI_USERACTION_PRIVMSG);

			KviCString szBuffer;
			const char * txtptr;
			int msgtype;
			DECRYPT_IF_NEEDED(chan, msg->safeTrailing(), KVI_OUT_CHANPRIVMSG, KVI_OUT_CHANPRIVMSGCRYPTED, szBuffer, txtptr, msgtype)

			QString szMsgText = chan->decodeText(txtptr);

			if(KVS_TRIGGER_EVENT_7_HALTED(KviEvent_OnChannelMessage, chan, szSourceNick, szSourceUser, szSourceHost, szMsgText, szPrefixes, (kvs_int_t)(msgtype == KVI_OUT_CHANPRIVMSGCRYPTED), msg->messageTagsKvsHash()))
				msg->setHaltOutput();

			// if the message is identified (identify-msg CAP) then re-add the +/- char at the beginning
			if(eCapState != IdentifyMsgCapNotUsed)
				szMsgText = QString::fromLatin1("%1%2").arg(eCapState == IdentifyMsgCapUsedIdentified ? "+" : "-").arg(szMsgText);

			if(!msg->haltOutput())
			{
				if(szPrefixes.length() > 0)
				{
					QString szBroad = QString("[>> %1\r!c\r%2\r] %3").arg(szPrefixes, szTarget, szMsgText);
					console->outputPrivmsg(chan, msgtype, szSourceNick, szSourceUser, szSourceHost, szBroad, 0, "", "", msg->serverTime());
				}
				else
				{
					console->outputPrivmsg(chan, msgtype, szSourceNick, szSourceUser, szSourceHost, szMsgText, 0, "", "", msg->serverTime());
				}
			}
		}
	}
}

void KviIrcServerParser::parseLiteralNotice(KviIrcMessage * msg)
{
	// NOTICE
	// :source NOTICE <target> :<message>
	QString szNick, szUser, szHost;
	msg->decodeAndSplitPrefix(szNick, szUser, szHost);

	KviConsoleWindow * console = msg->console();
	bool bIsServerNotice = false;

	//Check is it's a server notice (szNick = irc.xxx.net)
	if(szHost == "*" && szUser == "*" && szNick.indexOf('.') != -1)
		bIsServerNotice = true;

	// update the user entry in the database right away
	KviIrcUserDataBase * db = msg->connection()->userDataBase();
	KviIrcUserEntry * e = db->find(szNick);
	if (e)
	{
		e->setUser(szUser);
		e->setHost(szHost);
	}

	// FIXME: "DEDICATED CTCP WINDOW ?"

	KviCString pTrailing = msg->trailingString();
	if(pTrailing)
	{
		if(*(pTrailing.ptr()) == 0x01)
		{
			if(pTrailing.len() > 1)
			{
				if(pTrailing.lastCharIs(0x01))
					pTrailing.cutRight(1);
				pTrailing.cutLeft(1);
				KviCtcpMessage ctcp;
				ctcp.msg = msg;
				ctcp.pData = pTrailing.ptr();
				KviIrcMask talker(szNick, szUser, szHost); // FIXME
				ctcp.pSource = &talker;
				ctcp.szTarget = msg->connection()->decodeText(msg->safeParam(0));
				ctcp.bIgnored = false;
				ctcp.bIsFlood = false;
				ctcp.bUnknown = false;
				parseCtcpReply(&ctcp);
				return;
			}
		}
	}

	QString szTarget = msg->connection()->decodeText(msg->safeParam(0));

	KviRegisteredUser * u = msg->connection()->userDataBase()->registeredUser(szNick, szUser, szHost);
	//Ignore it?
	if(u)
	{
		if(u->isIgnoreEnabledFor(KviRegisteredUser::Notice))
		{
			if(KVI_OPTION_BOOL(KviOption_boolVerboseIgnore))
			{
				QString szMsg = msg->connection()->decodeText(msg->safeTrailing());
				console->output(KVI_OUT_IGNORE, msg->serverTime(), __tr2qs("Ignoring NOTICE from \r!nc\r%Q\r [%Q@\r!h\r%Q\r]: %Q"), &szNick, &szUser, &szHost, &szMsg);
			}
			return;
		}
	}

	// Normal NOTICE (directed to my nickname)
	if(IS_ME(msg, szTarget) && !bIsServerNotice)
	{
		// Nickserv nick identification routine
		QString szMsgText = msg->connection()->decodeText(msg->safeTrailing());
		KviIrcMask talker(szNick, szUser, szHost);
		KviNickServRule * rule = nullptr;

		// Figure out early if we have a query window open: if we have it then use it unconditionally
		KviQueryWindow * query = msg->connection()->findQuery(szNick);

		//check per-network nickserver settings
		KviNickServRuleSet * r = msg->connection()->target()->network()->nickServRuleSet();
		if(r)
		{
			if(r->isEnabled() && !r->isEmpty())
				rule = r->matchRule(msg->connection()->currentNickName(), &talker, szMsgText);
		}
		//check global nickserver settings (with server mask)
		if(!rule)
		{
			if(g_pNickServRuleSet->isEnabled() && !g_pNickServRuleSet->isEmpty())
				rule = g_pNickServRuleSet->matchRule(msg->connection()->currentNickName(), &talker, szMsgText, msg->connection()->currentServerName());
		}
		//check if at least one of the rules matches
		if(rule)
		{
			//kvs event triggering and text output
			if(KVS_TRIGGER_EVENT_5_HALTED(KviEvent_OnNickServNotice, console, szNick, szUser, szHost, szMsgText, msg->messageTagsKvsHash()))
				msg->setHaltOutput();
			if(!msg->haltOutput())
			{
				if(query)
					goto output_to_query_window; // use the query unconditionally

				KviWindow * pOut = (KviWindow *)(console);

				if(KVI_OPTION_BOOL(KviOption_boolServicesNoticesToActiveWindow))
				{
					KviWindow * aWin = console->activeWindow();
					if(aWin && (aWin->type() == KviWindow::Channel
					               || aWin->type() == KviWindow::Console
					               || aWin->type() == KviWindow::Query))
						pOut = aWin;
				}
				pOut->output(KVI_OUT_NICKSERV, "\r!n\r%Q\r [%Q@\r!h\r%Q\r]: %Q", &szNick, &szUser, &szHost, &szMsgText);
			}
			// exec the rule
			console->outputNoFmt(KVI_OUT_SYSTEMMESSAGE, __tr2qs("NickServ requests authentication, executing scheduled command"));
			if(!KviKvsScript::run(rule->identifyCommand(), console))
			{
				console->outputNoFmt(KVI_OUT_SYSTEMERROR, __tr2qs("The scheduled NickServ identification command appears to be broken, please change the setting"));
			}
			return;
		}

		//old style NickServ nick identification routine
		if(KviQString::equalCI(szNick, "NickServ"))
		{
			//kvs event triggering and text output
			if(KVS_TRIGGER_EVENT_5_HALTED(KviEvent_OnNickServNotice, console, szNick, szUser, szHost, szMsgText, msg->messageTagsKvsHash()))
				msg->setHaltOutput();
			if(!msg->haltOutput())
			{
				if(query)
					goto output_to_query_window; // use the query unconditionally

				KviWindow * pOut = (KviWindow *)(console);

				if(KVI_OPTION_BOOL(KviOption_boolServicesNoticesToActiveWindow))
				{
					KviWindow * aWin = console->activeWindow();
					if(aWin && (aWin->type() == KviWindow::Channel
					               || aWin->type() == KviWindow::Console
					               || aWin->type() == KviWindow::Query))
						pOut = aWin;
				}
				pOut->output(KVI_OUT_NICKSERV, "\r!n\r%Q\r [%Q@\r!h\r%Q\r]: %Q", &szNick, &szUser, &szHost, &szMsgText);
			}
			return;
		}

		// Chanserv nick identification routine
		if(KviQString::equalCI(szNick, "ChanServ"))
		{
			QString szMsgText = msg->connection()->decodeText(msg->safeTrailing());
			if(KVS_TRIGGER_EVENT_5_HALTED(KviEvent_OnChanServNotice, console, szNick, szUser, szHost, szMsgText, msg->messageTagsKvsHash()))
				msg->setHaltOutput();
			if(!msg->haltOutput())
			{
				if(query)
					goto output_to_query_window; // use the query unconditionally

				KviWindow * pOut = (KviWindow *)(console);

				if(KVI_OPTION_BOOL(KviOption_boolServicesNoticesToActiveWindow))
				{
					KviWindow * aWin = console->activeWindow();
					if(aWin && (aWin->type() == KviWindow::Channel
					               || aWin->type() == KviWindow::Console
					               || aWin->type() == KviWindow::Query))
						pOut = aWin;
				}
				pOut->output(KVI_OUT_CHANSERV, "\r!n\r%Q\r [%Q@\r!h\r%Q\r]: %Q", &szNick, &szUser, &szHost, &szMsgText);
			}
			return;
		}

		// Chanserv nick identification routine
		if(KviQString::equalCI(szNick, "MemoServ"))
		{
			QString szMsgText = msg->connection()->decodeText(msg->safeTrailing());
			if(KVS_TRIGGER_EVENT_5_HALTED(KviEvent_OnMemoServNotice, console, szNick, szUser, szHost, szMsgText, msg->messageTagsKvsHash()))
				msg->setHaltOutput();
			if(!msg->haltOutput())
			{
				if(query)
					goto output_to_query_window; // use the query unconditionally

				KviWindow * pOut = (KviWindow *)(console);

				if(KVI_OPTION_BOOL(KviOption_boolServicesNoticesToActiveWindow))
				{
					KviWindow * aWin = console->activeWindow();
					if(aWin && (aWin->type() == KviWindow::Channel
					               || aWin->type() == KviWindow::Console
					               || aWin->type() == KviWindow::Query))
						pOut = aWin;
				}

				pOut->output(KVI_OUT_MEMOSERV, "\r!n\r%Q\r [%Q@\r!h\r%Q\r]: %Q", &szNick, &szUser, &szHost, &szMsgText);
			}
			return;
		}

		// FIXME: PROCESS MULTIMEDIA FILE REQUESTS

		// A query request
		// do we have a matching window ?

		if(!query)
		{
			// New query requested. Check if we really should create it or not

			// first of all the anti spam, if desired.
			// the antispam blocks anything else
			// Eventually we could trigger a special event to notify the user of the
			// spam message...
			if(KVI_OPTION_BOOL(KviOption_boolUseAntiSpamOnNotice))
			{
				KviCString theMsg = msg->trailingString(); // FIXME
				if(theMsg)
				{
					KviCString spamWord;
					if(kvi_mayBeSpam(theMsg, spamWord))
					{
						// FIXME: OnSpam ?

						if(!(msg->haltOutput() || KVI_OPTION_BOOL(KviOption_boolSilentAntiSpam)))
						{
							QString szMsgText = msg->connection()->decodeText(msg->safeTrailing());
							QString szSpamWord = spamWord.ptr();
							console->output(KVI_OUT_SPAM, msg->serverTime(), __tr2qs("Spam notice from \r!n\r%Q\r [%Q@\r!h\r%Q\r]: %Q (matching spamword \"%Q\")"),
							    &szNick, &szUser, &szHost, &szMsgText, &szSpamWord);
						}
						return;
					}
				}
			}

			// this is not a spam, or at least it hasn't been recognized as spam

			// user option ? (this should again override any script)
			// if the scripters want really to force the query creation they can do
			// it manually or they can set the option to true at KVIrc startup
			if(KVI_OPTION_BOOL(KviOption_boolCreateQueryOnNotice))
			{
				QString szMsgText = msg->connection()->decodeText(msg->safeTrailing());
				// We still want to create it
				// Give the scripter a chance to filter it out again
				if(KVS_TRIGGER_EVENT_5_HALTED(KviEvent_OnQueryWindowRequest, console, szNick, szUser, szHost, szMsgText, msg->messageTagsKvsHash()))
				{
					// check if the scripter hasn't created it
					query = msg->connection()->findQuery(szNick);
				}
				else
				{
					// no query yet, create it!
					// this will trigger OnQueryWindowCreated
					query = console->connection()->createQuery(szNick);
					// and this will trigger OnQueryTargetAdded
					query->setTarget(szNick, szUser, szHost);
				}

				//check for query, since the user could have halt'ed its creation
				if(query)
				{
					if(!KVI_OPTION_STRING(KviOption_stringOnNewQueryOpenedSound).isEmpty())
					{
						KviKvsVariantList soundParams{new KviKvsVariant{KVI_OPTION_STRING(KviOption_stringOnNewQueryOpenedSound)}};
						KviKvsScript::run("snd.play $0", nullptr, &soundParams);
					}
				}
			}
		}

		// ok, now we either have a query or not
		if(query)
		{
		output_to_query_window:
			// ok, we have the query. Trigger the user action anyway
			query->userAction(szNick, szUser, szHost, KVI_USERACTION_NOTICE);
			// decrypt it if needed
			KviCString szBuffer;
			const char * txtptr;
			int msgtype;
			DECRYPT_IF_NEEDED(query, msg->safeTrailing(), KVI_OUT_QUERYNOTICE, KVI_OUT_QUERYNOTICECRYPTED, szBuffer, txtptr, msgtype)
			QString szMsgText = query->decodeText(txtptr);

			// trigger the script event and eventually kill the output
			if(KVS_TRIGGER_EVENT_6_HALTED(KviEvent_OnQueryNotice, query, szNick, szUser, szHost, szMsgText, (kvs_int_t)(msgtype == KVI_OUT_QUERYNOTICECRYPTED), msg->messageTagsKvsHash()))
				msg->setHaltOutput();
			// spit out the message text
			if(!msg->haltOutput())
			{
				int iFlags = 0;

				if(!query->hasAttention(KviWindow::MainWindowIsVisible))
				{
					if(KVI_OPTION_BOOL(KviOption_boolFlashQueryWindowOnNewMessages))
					{
						// avoid double window flashing
						iFlags |= KviConsoleWindow::NoWindowFlashing;
						query->demandAttention();
					}
					if(KVI_OPTION_BOOL(KviOption_boolPopupNotifierOnNewNotices))
					{
						// don't send the message twice to the notifier
						iFlags |= KviConsoleWindow::NoNotifier;
						QString szMsg = KviQString::toHtmlEscaped(szMsgText);
						//qDebug("KviIrcServerParser_literalHandlers.cpp:1262 debug: %s",szMsgText.data());
						g_pApp->notifierMessage(query, KviIconManager::QueryNotice, szMsg, KVI_OPTION_UINT(KviOption_uintNotifierAutoHideTime));
					}
				}

				console->outputPrivmsg(query, msgtype, szNick, szUser, szHost, szMsgText, iFlags, "", "", msg->serverTime());
			}
		}
		else
		{
			QString szMsgText = msg->connection()->decodeText(msg->safeTrailing());

			// no query creation: no decryption possible
			// trigger the query message event in the console
			if(KVS_TRIGGER_EVENT_6_HALTED(KviEvent_OnQueryNotice, console, szNick, szUser, szHost, szMsgText, (kvs_int_t)0, msg->messageTagsKvsHash()))
				msg->setHaltOutput();
			// spit the message text out
			if(!msg->haltOutput())
			{
				KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolExternalMessagesToActiveWindow) ? console->activeWindow() : (KviWindow *)(console);

				if(KviIrcConnection * pConnection = console->connection())
				{
					KviWindow * aWin = console->activeWindow();
					if((aWin->type() == KviWindow::Channel) && ((KviChannelWindow *)aWin)->isOn(szNick))
						pOut = aWin;
					else
					{
						for(auto & c : pConnection->channelList())
							if(c->isOn(szNick))
							{
								pOut = c;
								break;
							}
					}
				}

				pOut->output(KVI_OUT_QUERYNOTICE, msg->serverTime(), "*\r!n\r%Q\r* %Q", &szNick, &szMsgText);
			}
		}
		return;
	}

	// Channel NOTICE
	KviChannelWindow * chan = msg->connection()->findChannel(szTarget);

	QString szOriginalTarget = szTarget;
	QString szPrefixes;

	if(!chan)
	{
		// check if the channel has some leading mode prefixes
		while((szTarget.length() > 0) && console->connection()->serverInfo()->supportedStatusMsgPrefixes().contains(szTarget[0]))
		{
			szPrefixes += szTarget[0];
			szTarget.remove(0, 1);
		}
		chan = msg->connection()->findChannel(szTarget);
	}

	if(!chan)
	{
		QString szMsgText = msg->connection()->decodeText(msg->safeTrailing());

		if(bIsServerNotice)
		{
			//SERVER NOTICE DIRECTED TO A CHANNEL (EG. &servers, &kills on ircd)
			// FIXME: "Dedicated window for server notices ?"
			KviIrcConnectionServerInfo * pServerInfo = msg->connection()->serverInfo();
			QString version = pServerInfo->software();

			// OFTC replaced RPL_HOSTHIDDEN with a server notice
			if(version == "Hybrid+Oftc")
			{
				QStringList parts = szMsgText.split(" ", QString::SkipEmptyParts);
				if(parts.count() == 3)
				{
					if(parts[0] == "Activating" && parts[1] == "Cloak:")
						if(KVS_TRIGGER_EVENT_2_HALTED(KviEvent_OnMeHostChange, msg->console(), szNick, parts[2]))
							msg->setHaltOutput();
				}
			}

			if(KVS_TRIGGER_EVENT_2_HALTED(KviEvent_OnServerNotice, console, szNick, szMsgText))
				msg->setHaltOutput();

			if(!msg->haltOutput())
			{
				KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolServerNoticesToActiveWindow) ? console->activeWindow() : (KviWindow *)(console);
				pOut->output(KVI_OUT_SERVERNOTICE, msg->serverTime(), "[\r!s\r%Q\r]: %Q", &szNick, &szMsgText);
			}
			return;
		}

		//UNKNOWN NOTICE TYPE
		if(KVS_TRIGGER_EVENT_5_HALTED(KviEvent_OnBroadcastNotice, console, szNick, szUser, szHost, szOriginalTarget, szMsgText))
			msg->setHaltOutput();

		if(!msg->haltOutput())
		{
			KviWindow * pOut = (KviWindow *)(console);

			if(KVI_OPTION_BOOL(KviOption_boolExternalMessagesToActiveWindow))
			{
				KviWindow * aWin = console->activeWindow();
				if(aWin && (aWin->type() == KviWindow::Channel
				               || aWin->type() == KviWindow::Console
				               || aWin->type() == KviWindow::Query))
					pOut = aWin;
			}

			QString szBroad;
			szBroad = QString("[>> %1] %2").arg(szOriginalTarget, szMsgText);
			console->outputPrivmsg(pOut, KVI_OUT_BROADCASTNOTICE, szNick, szUser, szHost, szBroad, 0, "", "", msg->serverTime());
		}

		return;
	}

	//CHANNEL NOTICE
	chan->userAction(szNick, szUser, szHost, KVI_USERACTION_NOTICE);

	KviCString szBuffer;
	const char * txtptr;
	int msgtype;
	DECRYPT_IF_NEEDED(chan, msg->safeTrailing(), KVI_OUT_CHANNELNOTICE, KVI_OUT_CHANNELNOTICECRYPTED, szBuffer, txtptr, msgtype)
	QString szMsgText = chan->decodeText(txtptr);

	if(KVS_TRIGGER_EVENT_5_HALTED(KviEvent_OnChannelNotice, chan, szNick, szMsgText, szOriginalTarget, (kvs_int_t)(msgtype == KVI_OUT_CHANNELNOTICECRYPTED), msg->messageTagsKvsHash()))
		msg->setHaltOutput();

	if(!msg->haltOutput())
	{
		if(szPrefixes.length() > 0)
		{
			QString szBroad = QString("[>> %1\r!c\r%2\r] %3").arg(szPrefixes, szTarget, szMsgText);
			console->outputPrivmsg(chan, msgtype, szNick, szUser, szHost, szBroad, 0, "", "", msg->serverTime());
		}
		else
		{
			console->outputPrivmsg(chan, msgtype, szNick, szUser, szHost, szMsgText, 0, "", "", msg->serverTime());
		}
	}
}

//
// TOPIC
//

void KviIrcServerParser::parseLiteralTopic(KviIrcMessage * msg)
{
	// TOPIC
	// :<source_mask> TOPIC <channel> :<topic>
	QString szNick, szUser, szHost;
	msg->decodeAndSplitPrefix(szNick, szUser, szHost);
	QString szTarget = msg->connection()->decodeText(msg->safeParam(0));

	// Now lookup the channel
	KviChannelWindow * chan = msg->connection()->findChannel(szTarget);

	if(!chan)
	{
		UNRECOGNIZED_MESSAGE(msg, __tr2qs("Received a topic message for an unknown channel, possible desync"));
		return;
	}

	KviCString szBuffer;
	const char * txtptr;
	int msgtype;

	DECRYPT_IF_NEEDED(chan, msg->safeTrailing(), KVI_OUT_TOPIC, KVI_OUT_TOPICCRYPTED, szBuffer, txtptr, msgtype)

	QString szTopic = chan->decodeText(txtptr);

	if(KVS_TRIGGER_EVENT_4_HALTED(KviEvent_OnTopic, chan, szNick, szUser, szHost, szTopic))
		msg->setHaltOutput();

	chan->topicWidget()->setTopic(szTopic);
	chan->topicWidget()->setTopicSetBy(szNick);

	QString szTmp;
	QDateTime date = QDateTime::currentDateTime();
	switch(KVI_OPTION_UINT(KviOption_uintOutputDatetimeFormat))
	{
		case 0:
			// this is the equivalent to an empty date.toString() call, but it's needed
			// to ensure qt4 will use the default() locale and not the system() one
			szTmp = QLocale().toString(date, "ddd MMM d hh:mm:ss yyyy");
			break;
		case 1:
			szTmp = date.toString(Qt::ISODate);
			break;
		case 2:
			szTmp = date.toString(Qt::SystemLocaleShortDate);
			break;
	}
	chan->topicWidget()->setTopicSetAt(szTmp);

	chan->userAction(szNick, szUser, szHost, KVI_USERACTION_TOPIC);

	if(!msg->haltOutput())
	{
		chan->output(msgtype,
		    __tr2qs("\r!n\r%Q\r [%Q@\r!h\r%Q\r] has changed topic to \"%Q%c\""),
		    &szNick, &szUser, &szHost, &szTopic, KviControlCodes::Reset);
	}
}

void KviIrcServerParser::parseLiteralNick(KviIrcMessage * msg)
{
	// NICK
	// :source NICK <newnick>
	QString szNick, szUser, szHost;
	msg->decodeAndSplitPrefix(szNick, szUser, szHost);
	KviConsoleWindow * console = msg->console();
	QString szNewNick = msg->connection()->decodeText(msg->safeTrailing());

	bool bIsMe = IS_ME(msg, szNick);

	if(bIsMe)
	{
		// We have changed our nick
		msg->connection()->nickChange(szNewNick);

		if(KVS_TRIGGER_EVENT_2_HALTED(KviEvent_OnMeNickChange, console, szNick, szNewNick))
			msg->setHaltOutput();
	}
	else
	{
		if(KVS_TRIGGER_EVENT_4_HALTED(KviEvent_OnNickChange, console, szNick, szUser, szHost, szNewNick))
			msg->setHaltOutput();
	}

	// reset any cached smart nick color information
	KviIrcUserEntry * pUserEntry = msg->connection()->userDataBase()->find(szNick);
	if(pUserEntry)
		pUserEntry->setSmartNickColor(-1);

	for(auto & c : console->connection()->channelList())
	{
		if(c->nickChange(szNick, szNewNick))
		{
			if(!msg->haltOutput())
				c->output(KVI_OUT_NICK, __tr2qs("\r!n\r%Q\r [%Q@\r!h\r%Q\r] is now known as \r!n\r%Q\r"),
				    &szNick, &szUser, &szHost, &szNewNick);
			// FIXME if(bIsMe)output(YOU ARE now known as.. ?)
		}
		if(bIsMe)
			c->updateCaption();
	}

	// FIXME: #warning "NEW NICK MIGHT BE REGISTERED AND HAVE AN AVATAR!"

	if(bIsMe)
	{
		if(console->connection())
		{
			if(!msg->haltOutput())
				console->output(KVI_OUT_NICK, __tr2qs("You have changed your nickname to %Q"), &szNewNick);

			// just update all the captions : we have changed OUR nick
			for(auto & q : console->connection()->queryList())
			{
				if(!msg->haltOutput())
					q->output(KVI_OUT_NICK, __tr2qs("You have changed your nickname to %Q"), &szNewNick);
				q->updateCaption();
			}
		}
	}

	KviQueryWindow * q = console->connection()->findQuery(szNick);

	// It CAN happen that szNewNick first queries us without being
	// on any channel then he QUITS, he reconnects, he joins
	// a channel with szNick, queries us and changes nick to szNewNick : gotcha!
	// should merge the queries!

	KviQueryWindow * old = console->connection()->findQuery(szNewNick);
	if(old && (old != q))
	{
		if(KVI_OPTION_BOOL(KviOption_boolEnableQueryTracing) && (!_OUTPUT_QUIET))
		{
			old->output(KVI_OUT_QUERYTRACE,
			    __tr2qs("The target of this query was lost and has been found when \r!n\r%Q\r [%Q@\r!h\r%Q\r] changed their nickname to \r!n\r%Q\r"),
			    &szNick, &szUser, &szHost, &szNewNick);
		}

		if(q)
		{
			bool bQWasActive = (q == g_pActiveWindow);
			if(!_OUTPUT_MUTE)
			{
				old->output(KVI_OUT_SYSTEMWARNING,
				    __tr2qs("The recent nickname change from \r!n\r%Q\r to \r!n\r%Q\r caused a query collision: merging output"),
				    &szNick, &szNewNick);
			}
			old->mergeQuery(q);
			g_pMainWindow->closeWindow(q); // deleted path
			if(!msg->haltOutput())
				old->output(KVI_OUT_NICK, __tr2qs("\r!n\r%Q\r [%Q@\r!h\r%Q\r] is now known as \r!n\r%Q\r"),
				    &szNick, &szUser, &szHost, &szNewNick);
			if(!_OUTPUT_MUTE)
				old->output(KVI_OUT_SYSTEMWARNING, __tr2qs("End of merged output"));
			old->userAction(szNewNick, szUser, szHost, KVI_USERACTION_NICK);
			if(bQWasActive)
				old->delayedAutoRaise();
		}
		if(KVI_OPTION_BOOL(KviOption_boolEnableQueryTracing))
		{
			QString szChans;
			int iChans = console->connection()->getCommonChannels(szNewNick, szChans);
			old->notifyCommonChannels(szNewNick, szUser, szHost, iChans, szChans);
		}
	}
	else
	{
		if(q)
		{
			// the target SHOULD have changed his nick here
			if(!q->nickChange(szNick, szNewNick))
				qDebug("Internal error: query %s failed to change nick from %s to %s", szNick.toUtf8().data(), szNick.toUtf8().data(), szNewNick.toUtf8().data());
			if(!msg->haltOutput())
				q->output(KVI_OUT_NICK, __tr2qs("\r!n\r%Q\r [%Q@\r!h\r%Q\r] is now known as \r!n\r%Q\r"),
				    &szNick, &szUser, &szHost, &szNewNick);
			q->userAction(szNewNick, szUser, szHost, KVI_USERACTION_NICK);
		}
	}
	// FIXME: #warning "UPDATE ALL THE OTHER CONNECTION RELATED WINDOW CAPTIONS WHEN bIsMe!!"
}

void KviIrcServerParser::parseLiteralInvite(KviIrcMessage * msg)
{
	// INVITE
	// :source INVITE <target> <channel>
	QString szNick, szUser, szHost;
	msg->decodeAndSplitPrefix(szNick, szUser, szHost);

	// update the user entry in the database right away
	KviIrcUserDataBase * db = msg->connection()->userDataBase();
	KviIrcUserEntry * e = db->find(szNick);
	if (e)
	{
		e->setUser(szUser);
		e->setHost(szHost);
	}

	QString szTarget = msg->connection()->decodeText(msg->safeParam(0));
	QString szChannel = msg->connection()->decodeText(msg->safeParam(1));

	KviConsoleWindow * console = msg->console();
	KviRegisteredUser * u = msg->connection()->userDataBase()->registeredUser(szNick, szUser, szHost);
	//Ignore it?
	if(u)
	{
		if(u->isIgnoreEnabledFor(KviRegisteredUser::Invite))
		{
			if(KVI_OPTION_BOOL(KviOption_boolVerboseIgnore))
			{
				console->output(KVI_OUT_IGNORE, __tr2qs("Ignoring invite from \r!nc\r%Q\r [%Q@\r!h\r%Q\r]"), &szNick, &szUser, &szHost);
			}
			return;
		}
	}

	if(IS_ME(msg, szTarget))
	{
		if(KVS_TRIGGER_EVENT_4_HALTED(KviEvent_OnInvite, msg->console(), szNick, szUser, szHost, szChannel))
			msg->setHaltOutput();

		if(!msg->haltOutput())
		{
			KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolInvitesToActiveWindow) ? msg->console()->activeWindow() : (KviWindow *)(msg->console());
			QString szAction = KVI_OPTION_BOOL(KviOption_boolAutoJoinOnInvite) ? __tr2qs("auto joining") : __tr2qs("double-click the channel name to join");
			pOut->output(KVI_OUT_INVITE, __tr2qs("\r!n\r%Q\r [%Q@\r!h\r%Q\r] invites you to channel \r!c\r%Q\r (%Q)"),
			    &szNick, &szUser, &szHost, &szChannel, &szAction);
		}

		if(KVI_OPTION_BOOL(KviOption_boolAutoJoinOnInvite))
			msg->connection()->sendFmtData("JOIN %s", msg->safeParam(1));
	}
	else
	{
		UNRECOGNIZED_MESSAGE(msg, __tr("Received an invite message directed to another nick, possible desync"));
	}
}

void KviIrcServerParser::parseLiteralWallops(KviIrcMessage * msg)
{
	// WALLOPS
	// :source WALLOPS :msg
	QString szNick, szUser, szHost;
	msg->decodeAndSplitPrefix(szNick, szUser, szHost);

	QString szMsg = msg->connection()->decodeText(msg->safeTrailing());

	if(KVS_TRIGGER_EVENT_4_HALTED(KviEvent_OnWallops, msg->console(), szNick, szUser, szHost, szMsg))
		msg->setHaltOutput();

	if(!msg->haltOutput())
	{
		KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolOperatorMessagesToActiveWindow) ? msg->console()->activeWindow() : (KviWindow *)(msg->console());
		pOut->output(KVI_OUT_WALLOPS, __tr2qs("WALLOPS from \r!n\r%Q\r [%Q@\r!h\r%Q\r]: %Q"),
		    &szNick, &szUser, &szHost, &szMsg);
	}
}

void KviIrcServerParser::parseUserMode(KviIrcMessage * msg, const char * modeflptr)
{
	// changed my user mode
	bool bSet = true;
	while(*modeflptr)
	{
		switch(*modeflptr)
		{
			case '+':
				bSet = true;
				break;
			case '-':
				bSet = false;
				break;
			default:
				if(msg->connection()->changeUserMode(*modeflptr, bSet))
				{
					if(msg->connection()->serverInfo()->registerModeChar() == *modeflptr)
					{
						KviKvsVariantList vList;
						KviKvsEventManager::instance()->trigger(KviEvent_OnNickServAuth, msg->console(), &vList);
					}
					// There was a mode change
					if(KviKvsEventManager::instance()->hasAppHandlers(KviEvent_OnUserModeChange))
					{
						QString szModeFlag(bSet ? QChar('+') : QChar('-'));
						szModeFlag += QChar(*modeflptr);
						KviKvsVariantList vList(new KviKvsVariant(szModeFlag));
						if(KviKvsEventManager::instance()->trigger(KviEvent_OnUserModeChange, msg->console(), &vList))
							msg->setHaltOutput();
					}
				}
				break;
		}
		++modeflptr;
	}
}

void KviIrcServerParser::parseLiteralMode(KviIrcMessage * msg)
{
	// MODE
	// :source MODE target <params>
	//    :source MODE <me> +|-modeflag
	//    :source MODE <channel> +-modeflags [parameters]
	QString szNick, szUser, szHost;
	msg->decodeAndSplitPrefix(szNick, szUser, szHost);

	QString szTarget = msg->connection()->decodeText(msg->safeParam(0));
	KviCString modefl(msg->safeParam(1));

	if(IS_ME(msg, szTarget))
	{
		parseUserMode(msg, modefl.ptr());
		if(!msg->haltOutput())
		{
			KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolServerRepliesToActiveWindow) ? msg->console()->activeWindow() : (KviWindow *)(msg->console());
			pOut->output(KVI_OUT_MODE, __tr2qs("You have set user mode %s"), modefl.ptr());
		}
	}
	else
	{
		// a channel mode
		KviChannelWindow * chan = msg->connection()->findChannel(szTarget);

		if(!chan)
		{
			// Oops, desync with the server.
			UNRECOGNIZED_MESSAGE(msg, __tr("Received a mode change for an unknown channel, possible desync"));
			return;
		}

		chan->userAction(szNick, szUser, szHost, KVI_USERACTION_CHANMODE);
		parseChannelMode(szNick, szUser, szHost, chan, modefl, msg, 2);
	}
}

void KviIrcServerParser::parseChannelMode(const QString & szNick, const QString & szUser, const QString & szHost, KviChannelWindow * chan, KviCString & modefl, KviIrcMessage * msg, int curParam)
{
	bool bSet = true;

	bool bIsMultiSingleMode = false;
	bool bShowAsCompact = false;

	int iIconForCompactMode = KVI_OUT_CHANMODE;
	const char * aux = modefl.ptr();
	QString aParam;

	QString szNickBuffer, szHostBuffer;

	if(szHost != "*")
	{
		szNickBuffer = QString("\r!n\r%1\r").arg(szNick);
		szHostBuffer = QString("\r!h\r%1\r").arg(szHost);
	}
	else
	{
		if(szNick.indexOf('.') != -1)
		{
			// This looks a lot like a server!
			szNickBuffer = QString("\r!s\r%1\r").arg(szNick);
		}
		else
		{
			// Probably a service....whois should work
			szNickBuffer = QString("\r!n\r%1\r").arg(szNick);
		}
		szHostBuffer = szHost;
	}

	int curParamSave = curParam;

	bool bIsMe;

	/* pre-loop the modes char*, to understand if this is a multi-mode change that needs
	 * to be grouped or not (see bug #482)
	 */
	char cLastMode = ' ';
	int iSingleModes = 0, iTotModes = 0;

	while(*aux)
	{
		switch(*aux)
		{
			case '+':
			case '-':
				break;
			default:
				if(cLastMode != *aux)
				{
					iSingleModes++;
					cLastMode = *aux;
				}
				iTotModes++;
				break;
		}
		++aux;
	}

	// restore aux
	aux = modefl.ptr();

	if(iTotModes > 1)
	{
		bShowAsCompact = KVI_OPTION_BOOL(KviOption_boolShowCompactModeChanges);

		if(iSingleModes == 1)
			bIsMultiSingleMode = true;
	}

	while(*aux)
	{
		switch(*aux)
		{
			case '+':
				bSet = true;
				break;
			case '-':
				bSet = false;
				break;
			case 'k':
				if(bSet)
				{
					aParam = msg->safeParam(curParam++);
				}
				else
				{
					aParam = "";
					// mode k always eats a parameter, also when unset
					curParam++;
				}
				chan->setChannelModeWithParam('k', aParam);

				if(bSet)
				{
					if(KVS_TRIGGER_EVENT_4_HALTED(KviEvent_OnKeySet, chan, szNick, szUser, szHost, aParam))
						msg->setHaltOutput();
				}
				else
				{
					if(KVS_TRIGGER_EVENT_3_HALTED(KviEvent_OnKeyUnset, chan, szNick, szUser, szHost))
						msg->setHaltOutput();
				}
				if(!(msg->haltOutput() || bShowAsCompact))
				{
					if(bSet)
						chan->output(KVI_OUT_KEY,
						    __tr2qs("%Q [%Q@%Q] has set channel key to \"\r!m-k %Q\r%Q\r\""),
						    &szNickBuffer, &szUser, &szHostBuffer, &aParam, &aParam);
					else
						chan->output(KVI_OUT_KEY,
						    __tr2qs("%Q [%Q@%Q] has unset the channel key"),
						    &szNickBuffer, &szUser, &szHostBuffer);
				}

				if(bIsMultiSingleMode)
					iIconForCompactMode = KVI_OUT_KEY;
				break;
			case 'l':
				if(bSet)
					aParam = msg->safeParam(curParam++);
				else
					aParam = "";
				chan->setChannelModeWithParam('l', aParam);

				if(bSet)
				{
					if(KVS_TRIGGER_EVENT_4_HALTED(KviEvent_OnLimitSet, chan, szNick, szUser, szHost, aParam))
						msg->setHaltOutput();
				}
				else
				{
					if(KVS_TRIGGER_EVENT_3_HALTED(KviEvent_OnLimitUnset, chan, szNick, szUser, szHost))
						msg->setHaltOutput();
				}

				if(!(msg->haltOutput() || bShowAsCompact))
				{
					if(bSet)
						chan->output(KVI_OUT_LIMIT,
						    __tr2qs("%Q [%Q@%Q] has set channel \r!m-l\rlimit to %Q\r"),
						    &szNickBuffer, &szUser, &szHostBuffer, &aParam);
					else
						chan->output(KVI_OUT_LIMIT,
						    __tr2qs("%Q [%Q@%Q] has unset the channel limit"),
						    &szNickBuffer, &szUser, &szHostBuffer);
				}

				if(bIsMultiSingleMode)
					iIconForCompactMode = KVI_OUT_LIMIT;
				break;
#define CHANUSER_MODE(modechar, chanfunc, evmeset, evmeunset, evset, evunset, icomeset, icomeunset, icoset, icounset)                     \
	case modechar:                                                                                                                        \
		if(msg->connection()->serverInfo()->isSupportedModeFlag(modechar))                                                                \
		{                                                                                                                                 \
			aParam = msg->connection()->decodeText(msg->safeParam(curParam++));                                                           \
			bIsMe = IS_ME(msg, aParam);                                                                                                   \
			chan->chanfunc(aParam, bSet, bIsMe);                                                                                          \
			if(bIsMe)                                                                                                                     \
			{                                                                                                                             \
				if(KVS_TRIGGER_EVENT_3_HALTED(bSet ? evmeset : evmeunset, chan, szNick, szUser, szHost))                                  \
					msg->setHaltOutput();                                                                                                 \
				chan->updateCaption();                                                                                                    \
			}                                                                                                                             \
			else                                                                                                                          \
			{                                                                                                                             \
				if(KVS_TRIGGER_EVENT_4_HALTED(bSet ? evset : evunset, chan, szNick, szUser, szHost, aParam))                              \
					msg->setHaltOutput();                                                                                                 \
			}                                                                                                                             \
			if(!(msg->haltOutput() || bShowAsCompact))                                                                                    \
			{                                                                                                                             \
				chan->output(bSet ? (bIsMe ? icomeset : icoset) : (bIsMe ? icomeunset : icounset),                                        \
				    __tr2qs("%Q [%Q@%Q] has set mode %c%c \r!n\r%Q\r"),                                                                   \
				    &szNickBuffer, &szUser, &szHostBuffer, bSet ? '+' : '-', modechar, &aParam);                                          \
			}                                                                                                                             \
			if(bIsMultiSingleMode)                                                                                                        \
				iIconForCompactMode = (bSet ? (bIsMe ? icomeset : icoset) : (bIsMe ? icomeunset : icounset));                             \
		}                                                                                                                                 \
		else                                                                                                                              \
		{                                                                                                                                 \
			chan->setChannelMode(modechar, bSet);                                                                                         \
			if(!(msg->haltOutput() || bShowAsCompact))                                                                                    \
			{                                                                                                                             \
				chan->output(KVI_OUT_CHANMODE,                                                                                            \
				    __tr2qs("%Q [%Q@%Q] has set channel \r!m%c%c\rmode %c%c\r"),                                                          \
				    &szNickBuffer, &szUser, &szHostBuffer,                                                                                \
				    bSet ? '-' : '+', modechar, bSet ? '+' : '-', modechar);                                                              \
			}                                                                                                                             \
			if(bIsMultiSingleMode)                                                                                                        \
				iIconForCompactMode = KVI_OUT_CHANMODE;                                                                                   \
		}                                                                                                                                 \
		break;

				CHANUSER_MODE('o', setOp, KviEvent_OnMeOp, KviEvent_OnMeDeOp, KviEvent_OnOp, KviEvent_OnDeOp, KVI_OUT_MEOP, KVI_OUT_MEDEOP, KVI_OUT_OP, KVI_OUT_DEOP)
				CHANUSER_MODE('h', setHalfOp, KviEvent_OnMeHalfOp, KviEvent_OnMeDeHalfOp, KviEvent_OnHalfOp, KviEvent_OnDeHalfOp, KVI_OUT_MEHALFOP, KVI_OUT_MEDEHALFOP, KVI_OUT_HALFOP, KVI_OUT_HALFDEOP)
				CHANUSER_MODE('v', setVoice, KviEvent_OnMeVoice, KviEvent_OnMeDeVoice, KviEvent_OnVoice, KviEvent_OnDeVoice, KVI_OUT_MEVOICE, KVI_OUT_MEDEVOICE, KVI_OUT_VOICE, KVI_OUT_DEVOICE)
				CHANUSER_MODE('u', setUserOp, KviEvent_OnMeUserOp, KviEvent_OnMeDeUserOp, KviEvent_OnUserOp, KviEvent_OnDeUserOp, KVI_OUT_MEUSEROP, KVI_OUT_MEDEUSEROP, KVI_OUT_USEROP, KVI_OUT_USERDEOP)

			case 'a':
			case 'q':
				// note: unrealircd declares q,a as CHANMODES, while inspircd declares
				// them as PREFIXes, so we need to check them both here (ticket #951)
				if(msg->connection()->serverInfo()->supportedListModes().contains(*aux) || msg->connection()->serverInfo()->isSupportedModeFlag(*aux))
				{
					// (freenode) ircd-seven's quite ban (channel mode q with mask)
					// (ircq) unrealircd's channel owner (channel mode q with nickname)
					// (ircq) unrealircd's channel admin (channel mode a with nickname)
					// not existing but supported mode (channel mode a with mask)

					aParam = msg->connection()->decodeText(msg->safeParam(curParam++));
					// we call setModeInList anyway to fill the "mode q editor"
					chan->setModeInList(*aux, aParam, bSet, msg->connection()->decodeText(msg->safePrefix()), QDateTime::currentDateTime().toTime_t());
					if(aParam.contains('!'))
					{
						// it's a mask
						{
						KviIrcMask auxMask(aParam);
						bIsMe = auxMask.matchesFixed(
						    msg->connection()->userInfo()->nickName(),
						    msg->connection()->userInfo()->userName(),
						    msg->connection()->userInfo()->hostName());
						}
						if(bIsMe)
						{
							if(KVS_TRIGGER_EVENT_4_HALTED(bSet ? KviEvent_OnMeQuietBan : KviEvent_OnMeQuietUnban, chan, szNick, szUser, szHost, aParam))
								msg->setHaltOutput();
						}
						else
						{
							if(KVS_TRIGGER_EVENT_4_HALTED(bSet ? KviEvent_OnQuietBan : KviEvent_OnQuietUnban, chan, szNick, szUser, szHost, aParam))
								msg->setHaltOutput();
						}
						if(!(msg->haltOutput() || bShowAsCompact))
						{
							chan->output(bSet ? (bIsMe ? KVI_OUT_MEBAN : KVI_OUT_BAN) : (bIsMe ? KVI_OUT_MEUNBAN : KVI_OUT_UNBAN),
							    __tr2qs("%Q [%Q@%Q] has set mode %c%c \r!m%c%c %Q\r%Q\r"),
							    &szNickBuffer, &szUser, &szHostBuffer,
							    bSet ? '+' : '-', *aux, bSet ? '-' : '+', *aux, &aParam, &aParam);
						}
						if(bIsMultiSingleMode)
							iIconForCompactMode = (bSet ? (bIsMe ? KVI_OUT_MEBAN : KVI_OUT_BAN) : (bIsMe ? KVI_OUT_MEUNBAN : KVI_OUT_UNBAN));
					}
					else
					{
						// it's a nick
						if(*aux == 'q')
						{
							chan->setChanOwner(aParam, bSet);
							bIsMe = IS_ME(msg, aParam);
							if(bIsMe)
							{
								if(KVS_TRIGGER_EVENT_3_HALTED(bSet ? KviEvent_OnMeChanOwner : KviEvent_OnMeDeChanOwner, chan, szNick, szUser, szHost))
									msg->setHaltOutput();
								chan->updateCaption();
							}
							else
							{
								if(KVS_TRIGGER_EVENT_4_HALTED(bSet ? KviEvent_OnChanOwner : KviEvent_OnDeChanOwner, chan, szNick, szUser, szHost, aParam))
									msg->setHaltOutput();
							}
							if(!(msg->haltOutput() || bShowAsCompact))
							{
								chan->output(bSet ? (bIsMe ? KVI_OUT_MECHANOWNER : KVI_OUT_CHANOWNER) : (bIsMe ? KVI_OUT_MEDECHANOWNER : KVI_OUT_DECHANOWNER),
								    __tr2qs("%Q [%Q@%Q] has set mode %c%c \r!n\r%Q\r"),
								    &szNickBuffer, &szUser, &szHostBuffer, bSet ? '+' : '-', *aux, &aParam);
							}
							if(bIsMultiSingleMode)
								iIconForCompactMode = (bSet ? (bIsMe ? KVI_OUT_MECHANOWNER : KVI_OUT_CHANOWNER) : (bIsMe ? KVI_OUT_MEDECHANOWNER : KVI_OUT_DECHANOWNER));
						}
						else
						{
							//*aux==a
							chan->setChanAdmin(aParam, bSet);
							bIsMe = IS_ME(msg, aParam);
							if(bIsMe)
							{
								if(KVS_TRIGGER_EVENT_3_HALTED(bSet ? KviEvent_OnMeChanAdmin : KviEvent_OnMeDeChanAdmin, chan, szNick, szUser, szHost))
									msg->setHaltOutput();
								chan->updateCaption();
							}
							else
							{
								if(KVS_TRIGGER_EVENT_4_HALTED(bSet ? KviEvent_OnChanAdmin : KviEvent_OnDeChanAdmin, chan, szNick, szUser, szHost, aParam))
									msg->setHaltOutput();
							}
							if(!(msg->haltOutput() || bShowAsCompact))
							{
								chan->output(bSet ? (bIsMe ? KVI_OUT_MECHANADMIN : KVI_OUT_CHANADMIN) : (bIsMe ? KVI_OUT_MEDECHANADMIN : KVI_OUT_DECHANADMIN),
								    __tr2qs("%Q [%Q@%Q] has set mode %c%c \r!n\r%Q\r"),
								    &szNickBuffer, &szUser, &szHostBuffer, bSet ? '+' : '-', *aux, &aParam);
							}
							if(bIsMultiSingleMode)
								iIconForCompactMode = (bSet ? (bIsMe ? KVI_OUT_MECHANADMIN : KVI_OUT_CHANADMIN) : (bIsMe ? KVI_OUT_MEDECHANADMIN : KVI_OUT_DECHANADMIN));
						}
					}
				}
				else if(msg->connection()->serverInfo()->supportedParameterModes().contains(*aux)
				    || (msg->connection()->serverInfo()->supportedParameterWhenSetModes().contains(*aux) && bSet))
				{
					aParam = msg->connection()->decodeText(msg->safeParam(curParam++));
					chan->setChannelModeWithParam(*aux, aParam);

					if(!(msg->haltOutput() || bShowAsCompact))
					{
						if(aParam.isEmpty())
						{
							chan->output(KVI_OUT_CHANMODE,
							    __tr2qs("%Q [%Q@%Q] has set channel \r!m%c%c\rmode %c%c\r"),
							    &szNickBuffer, &szUser, &szHostBuffer,
							    bSet ? '-' : '+', *aux, bSet ? '+' : '-', *aux);
						}
						else
						{
							chan->output(KVI_OUT_CHANMODE,
							    __tr2qs("%Q [%Q@%Q] has set mode %c%c \r!m%c%c %Q\r%Q\r"),
							    &szNickBuffer, &szUser, &szHostBuffer,
							    bSet ? '+' : '-', *aux, bSet ? '-' : '+', *aux, &aParam, &aParam);
						}
					}
				}
				else
				{
					if(msg->connection()->serverInfo()->supportedParameterWhenSetModes().contains(*aux))
					{
						// a mode with parameter that does not receive one when unset
						QString szEmptyParam("");
						chan->setChannelModeWithParam(*aux, szEmptyParam);
					}
					else
					{
						chan->setChannelMode(*aux, bSet);
					}

					if(!(msg->haltOutput() || bShowAsCompact))
					{
						chan->output(KVI_OUT_CHANMODE,
						    __tr2qs("%Q [%Q@%Q] has set channel \r!m%c%c\rmode %c%c\r"),
						    &szNickBuffer, &szUser, &szHostBuffer,
						    bSet ? '-' : '+', *aux, bSet ? '+' : '-', *aux);
					}
				}
				if(bIsMultiSingleMode)
					iIconForCompactMode = KVI_OUT_CHANMODE;
				break;

#define CHANNEL_MODE(modefl, evmeset, evmeunset, evset, evunset, icomeset, icomeunset, icoset, icounset)                                    \
	case modefl:                                                                                                                            \
		aParam = msg->connection()->decodeText(msg->safeParam(curParam++));                                                                 \
		chan->setModeInList(*aux, aParam, bSet, msg->connection()->decodeText(msg->safePrefix()), QDateTime::currentDateTime().toTime_t()); \
		{                                                                                                                                   \
		KviIrcMask auxMask(aParam);                                                                                                         \
		bIsMe = auxMask.matchesFixed(                                                                                                       \
		    msg->connection()->userInfo()->nickName(),                                                                                      \
		    msg->connection()->userInfo()->userName(),                                                                                      \
		    msg->connection()->userInfo()->hostName());                                                                                     \
		}                                                                                                                                   \
		if(bIsMe)                                                                                                                           \
		{                                                                                                                                   \
			if(KVS_TRIGGER_EVENT_4_HALTED(bSet ? evmeset : evmeunset, chan, szNick, szUser, szHost, aParam))                                \
				msg->setHaltOutput();                                                                                                       \
		}                                                                                                                                   \
		else                                                                                                                                \
		{                                                                                                                                   \
			if(KVS_TRIGGER_EVENT_4_HALTED(bSet ? evset : evunset, chan, szNick, szUser, szHost, aParam))                                    \
				msg->setHaltOutput();                                                                                                       \
		}                                                                                                                                   \
		if(!(msg->haltOutput() || bShowAsCompact))                                                                                          \
		{                                                                                                                                   \
			auto aParamEscaped = aParam;                                                                                                \
			KviQString::escapeKvs(&aParamEscaped);                                                                                      \
			chan->output(bSet ? (bIsMe ? icomeset : icoset) : (bIsMe ? icomeunset : icounset),                                              \
			    __tr2qs("%Q [%Q@%Q] has set mode %c%c \r!m%c%c %Q\r%Q\r"),                                                                  \
			    &szNickBuffer, &szUser, &szHostBuffer,                                                                                      \
			    bSet ? '+' : '-', modefl, bSet ? '-' : '+', modefl, &aParamEscaped, &aParam);                                               \
		}                                                                                                                                   \
		if(bIsMultiSingleMode)                                                                                                              \
			iIconForCompactMode = (bSet ? (bIsMe ? icomeset : icoset) : (bIsMe ? icomeunset : icounset));                                   \
		break;

				CHANNEL_MODE('b', KviEvent_OnMeBan, KviEvent_OnMeUnban, KviEvent_OnBan, KviEvent_OnUnban, KVI_OUT_MEBAN, KVI_OUT_MEUNBAN, KVI_OUT_BAN, KVI_OUT_UNBAN)
				CHANNEL_MODE('I', KviEvent_OnMeInviteException, KviEvent_OnMeInviteExceptionRemove, KviEvent_OnInviteException, KviEvent_OnInviteExceptionRemove, KVI_OUT_MEINVITEEXCEPT, KVI_OUT_MEINVITEUNEXCEPT, KVI_OUT_INVITEEXCEPT, KVI_OUT_INVITEUNEXCEPT)
				CHANNEL_MODE('e', KviEvent_OnMeBanException, KviEvent_OnMeBanExceptionRemove, KviEvent_OnBanException, KviEvent_OnBanExceptionRemove, KVI_OUT_MEBANEXCEPT, KVI_OUT_MEBANUNEXCEPT, KVI_OUT_BANEXCEPT, KVI_OUT_BANUNEXCEPT)

			default:
				// check if the mode "eats" a parameter
				if(msg->connection()->serverInfo()->supportedListModes().contains(*aux))
				{
					/*
				 * Examples:
				 * spam filter with parameter like mode "g" in inspircd
				 */
					aParam = msg->connection()->decodeText(msg->safeParam(curParam++));
					chan->setModeInList(*aux, aParam, bSet, msg->connection()->decodeText(msg->safePrefix()), QDateTime::currentDateTime().toTime_t());

					if(!(msg->haltOutput() || bShowAsCompact))
					{
						if(aParam.isEmpty())
						{
							chan->output(KVI_OUT_CHANMODE,
							    __tr2qs("%Q [%Q@%Q] has set channel \r!m%c%c\rmode %c%c\r"),
							    &szNickBuffer, &szUser, &szHostBuffer,
							    bSet ? '-' : '+', *aux, bSet ? '+' : '-', *aux);
						}
						else
						{
							chan->output(KVI_OUT_CHANMODE,
							    __tr2qs("%Q [%Q@%Q] has set mode %c%c \r!m%c%c %Q\r%Q\r"),
							    &szNickBuffer, &szUser, &szHostBuffer,
							    bSet ? '+' : '-', *aux, bSet ? '-' : '+', *aux, &aParam, &aParam);
						}
					}
				}
				else if(msg->connection()->serverInfo()->supportedParameterModes().contains(*aux)
				    || (msg->connection()->serverInfo()->supportedParameterWhenSetModes().contains(*aux) && bSet))
				{
				/*
				 * Examples:
				 * flood mode with parameter like "[5m#M4]:5", see bug #505
				 * Channel join throttling like "4:5", see bug #731
				 */
					aParam = msg->connection()->decodeText(msg->safeParam(curParam++));
					chan->setChannelModeWithParam(*aux, aParam);

					if(!(msg->haltOutput() || bShowAsCompact))
					{
						if(aParam.isEmpty())
						{
							chan->output(KVI_OUT_CHANMODE,
							    __tr2qs("%Q [%Q@%Q] has set channel \r!m%c%c\rmode %c%c\r"),
							    &szNickBuffer, &szUser, &szHostBuffer,
							    bSet ? '-' : '+', *aux, bSet ? '+' : '-', *aux);
						}
						else
						{
							chan->output(KVI_OUT_CHANMODE,
							    __tr2qs("%Q [%Q@%Q] has set mode %c%c \r!m%c%c %Q\r%Q\r"),
							    &szNickBuffer, &szUser, &szHostBuffer,
							    bSet ? '+' : '-', *aux, bSet ? '-' : '+', *aux, &aParam, &aParam);
						}
					}
				}
				else if(msg->connection()->serverInfo()->isSupportedModeFlag(*aux))
				{
				/*
				 * A custom mode prefix, like inspircd's  +y => !
				 * We need to correctly echo it, and be sure it doesn't
				 * get confused as a channel mode
				 */
					aParam = msg->connection()->decodeText(msg->safeParam(curParam++));
					// TODO support custom mode prefixes
					//chan->setChannelModeWithParam(*aux,aParam);

					if(!(msg->haltOutput() || bShowAsCompact))
					{
						if(aParam.isEmpty())
						{
							chan->output(KVI_OUT_CHANMODE,
							    __tr2qs("%Q [%Q@%Q] has set channel \r!m%c%c\rmode %c%c\r"),
							    &szNickBuffer, &szUser, &szHostBuffer,
							    bSet ? '-' : '+', *aux, bSet ? '+' : '-', *aux);
						}
						else
						{
							chan->output(KVI_OUT_CHANMODE,
							    __tr2qs("%Q [%Q@%Q] has set mode %c%c \r!m%c%c %Q\r%Q\r"),
							    &szNickBuffer, &szUser, &szHostBuffer,
							    bSet ? '+' : '-', *aux, bSet ? '-' : '+', *aux, &aParam, &aParam);
						}
					}
				}
				else
				{
					if(msg->connection()->serverInfo()->supportedParameterWhenSetModes().contains(*aux))
					{
						// a mode with parameter that does not receive one when unset
						QString szEmptyParam("");
						chan->setChannelModeWithParam(*aux, szEmptyParam);
					}
					else
					{
						chan->setChannelMode(*aux, bSet);
					}

					if(!(msg->haltOutput() || bShowAsCompact))
					{
						chan->output(KVI_OUT_CHANMODE,
						    __tr2qs("%Q [%Q@%Q] has set channel \r!m%c%c\rmode %c%c\r"),
						    &szNickBuffer, &szUser, &szHostBuffer,
						    bSet ? '-' : '+', *aux, bSet ? '+' : '-', *aux);
					}
				}
				if(bIsMultiSingleMode)
					iIconForCompactMode = KVI_OUT_CHANMODE;
				break;
		}
		++aux;
	}

	QString param;
	QString params;
	param = msg->connection()->decodeText(msg->safeParam(curParamSave++));
	while(!param.isEmpty())
	{
		if(!params.isEmpty())
			params.append(' ');
		params.append(param);
		param = msg->connection()->decodeText(msg->safeParam(curParamSave++));
	}

	if(KVS_TRIGGER_EVENT_5_HALTED(KviEvent_OnChannelModeChange, chan, szNick, szUser, szHost, modefl.ptr(), params))
		msg->setHaltOutput();

	if(bShowAsCompact && (!msg->haltOutput()) && (!kvi_strEqualCS(modefl.ptr(), "+")))
	{
		if(!params.isEmpty())
		{
			chan->output(iIconForCompactMode, __tr2qs("%Q [%Q@%Q] has set mode %s %Q"),
			    &szNickBuffer, &szUser, &szHostBuffer, modefl.ptr(), &params);
		}
		else
		{
			chan->output(iIconForCompactMode, __tr2qs("%Q [%Q@%Q] has set channel mode %s"),
			    &szNickBuffer, &szUser, &szHostBuffer, modefl.ptr());
		}
	}
}

void KviIrcServerParser::parseLiteralCap(KviIrcMessage * msg)
{
	// CAP
	// CAP LS
	// :prefix CAP <nickname> <command> [*] :tls userhost-in-names multi-prefix sasl

	// Server2client subcommands:
	// LIST, LS, ACK, NAK
	// Client2server subcommands:
	// LIST, LS, REQ, CLEAR, END

	QString szPrefix = msg->connection()->decodeText(msg->safePrefix());
	QString szCmd = msg->connection()->decodeText(msg->safeParam(1));
	QString szProtocols = msg->connection()->decodeText(msg->safeTrailing());

	if(KVS_TRIGGER_EVENT_3_HALTED(KviEvent_OnCap, msg->console(), szPrefix, szCmd, szProtocols))
		msg->setHaltOutput();

	if(szCmd.compare("LS", Qt::CaseInsensitive) == 0)
	{
		// :prefix CAP <nickname> LS [*] :<cap1> <cap2> <cap3> ....
		// All but the last LS messages have the asterisk

		QString szAsterisk = msg->connection()->decodeText(msg->safeParam(2));
		bool bLast = szAsterisk != "*";

		msg->connection()->serverInfo()->addSupportedCaps(szProtocols);

		if(msg->connection()->stateData()->isInsideInitialCapLs())
		{
			// We sent the request: handle it silently.. but only if this is the last of the LS messages
			if(bLast)
				msg->connection()->handleInitialCapLs();
			return;
		}

		// We didn't send the request (the user did, or the server sent the reply spontaneously)
		if(!msg->haltOutput())
			msg->console()->output(KVI_OUT_CAP, __tr2qs("Server capabilities: %Q"), &szProtocols);
	}
	else if(szCmd.compare("ACK", Qt::CaseInsensitive) == 0)
	{
		// :prefix CAP <nickname> ACK [*] :<cap1> <cap2> <cap3> ....
		// All but the last ACK messages have the asterisk

		msg->connection()->serverInfo()->addSupportedCaps(szProtocols);
		msg->connection()->stateData()->changeEnabledCapList(szProtocols);

		QString szAsterisk = msg->connection()->decodeText(msg->safeParam(2));
		bool bLast = szAsterisk != "*";

		if(msg->connection()->stateData()->isInsideInitialCapReq())
		{
			// We sent the request: handle it silently.. but only if this is the last of the REQ messages
			if(bLast)
				msg->connection()->handleInitialCapAck();
			return;
		}

		if(!msg->haltOutput())
			msg->console()->output(KVI_OUT_CAP, __tr2qs("Capability change acknowledged: %Q"), &szProtocols);
	}
	else if(szCmd.compare("NAK", Qt::CaseInsensitive) == 0)
	{
		// :prefix CAP <nickname> NAK :<cap1> <cap2> <cap3> ....

		if(msg->connection()->stateData()->isInsideInitialCapReq())
		{
			msg->connection()->handleInitialCapNak();
			return;
		}

		if(!msg->haltOutput())
			msg->console()->output(KVI_OUT_CAP, __tr2qs("Capability change denied: %Q"), &szProtocols);
	}
	else if(szCmd.compare("LIST", Qt::CaseInsensitive) == 0)
	{
		// :prefix CAP <nickname> LIST [*] :<cap1> <cap2> <cap3> ....
		// All but the last LIST messages have the asterisk

		msg->connection()->stateData()->changeEnabledCapList(szProtocols);

		if(!msg->haltOutput())
			msg->console()->output(KVI_OUT_CAP, __tr2qs("Currently enabled capabilities: %Q"), &szProtocols);
	}
	else if(!msg->haltOutput())
		msg->console()->output(KVI_OUT_CAP, __tr2qs("Received unknown extended capability message: %Q %Q"), &szCmd, &szProtocols);
}

void KviIrcServerParser::parseLiteralAuthenticate(KviIrcMessage * msg)
{
	// :AUTHENTICATE +
	KviCString szAuth(msg->safeParam(0));
	msg->connection()->handleAuthenticate(szAuth);
}

//
// AWAY
//

void KviIrcServerParser::parseLiteralAway(KviIrcMessage * msg)
{
	// AWAY (IRCv3.1)
	// :<source_mask> AWAY :<part message>
	QString szNick, szUser, szHost;
	msg->decodeAndSplitPrefix(szNick, szUser, szHost);

	QString awayMsg = msg->paramCount() > 0 ? msg->connection()->decodeText(msg->safeTrailing()) : QString();

	// Update the user entry
	KviIrcUserDataBase * db = msg->connection()->userDataBase();
	KviIrcUserEntry * e = db->find(szNick);
	KviConsoleWindow * console = msg->console();

	if(e)
	{
		e->setAway(!awayMsg.isEmpty());
	}

	if(KVS_TRIGGER_EVENT_4_HALTED(KviEvent_OnAway, console, szNick, szUser, szHost, awayMsg))
		msg->setHaltOutput();
}
