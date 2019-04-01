//===========================================================================
//
//   File : KviIrcServerParser_numericHandlers.cpp
//   Creation date : Thu Aug 3 2000 01:30:45 by Szymon Stefanek
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
//===========================================================================

#include "KviIrcServerParser.h"

#include "KviWindow.h"
#include "KviQueryWindow.h"
#include "kvi_out.h"
#include "KviLocale.h"
#include "KviIrcSocket.h"
#include "KviOptions.h"
#include "KviChannelWindow.h"
#include "KviTopicWidget.h"
#include "KviIrcUserDataBase.h"
#include "kvi_defaults.h"
#include "KviControlCodes.h"
#include "KviMainWindow.h"
#include "KviApplication.h"
#include "KviNotifyList.h"
#include "KviIrcNumericCodes.h"
#include "KviIrcConnection.h"
#include "KviIrcConnectionStateData.h"
#include "KviIrcConnectionUserInfo.h"
#include "KviIrcConnectionServerInfo.h"
#include "KviIrcConnectionAsyncWhoisData.h"
#include "KviIrcConnectionTarget.h"
#include "KviTimeUtils.h"
#include "KviLagMeter.h"
#include "KviKvsEventTriggers.h"
#include "KviKvsScript.h"
#include "KviKvsVariantList.h"
#include "KviIdentityProfileSet.h"
#include "KviIrcMessage.h"

#ifdef COMPILE_CRYPT_SUPPORT
#include "KviCryptEngine.h"
#include "KviCryptController.h"
#endif

#include <QPixmap>
#include <QDateTime>
#include <QTextCodec>
#include <QRegExp>
#include <QByteArray>
#include <QLocale>

// #define IS_CHANNEL_TYPE_FLAG(_str) ((*(_str) == '#') || (*(_str) == '&') || (*(_str) == '!'))
#define IS_CHANNEL_TYPE_FLAG(_qchar) (msg->connection()->serverInfo()->supportedChannelTypes().indexOf(_qchar) != -1)
#define IS_USER_MODE_PREFIX(_qchar) (msg->connection()->serverInfo()->supportedModePrefixes().indexOf(_qchar) != -1)

// Numeric message handlers

// FIXME: #warning "IN ALL OUTPUT ADD ESCAPE SEQUENCES!!!!"
// FIXME: #warning "parseErrorUnknownModeChar() for modes e and I, parseErrorUnknownCommand for WATCH"

void KviIrcServerParser::parseNumeric001(KviIrcMessage * msg)
{
	// 001: RPL_WELCOME
	// :prefix 001 target :Welcome to the Internet Relay Network <usermask>
	// FIXME: #warning "SET THE USERMASK FROM SERVER"
	QString szText = msg->connection()->decodeText(msg->safeTrailing());
	QRegExp rx(" ([^ ]+)!([^ ]+)@([^ ]+)$");
	if(rx.indexIn(szText) != -1)
	{
		msg->connection()->userInfo()->setUnmaskedHostName(rx.cap(3));
		msg->connection()->userInfo()->setNickName(rx.cap(1));
		msg->connection()->userInfoReceived(rx.cap(2), rx.cap(3));
	}
	if(msg->connection()->context()->state() != KviIrcContext::Connected)
		msg->connection()->loginComplete(msg->connection()->decodeText(msg->param(0)));
	if(!msg->haltOutput())
		msg->console()->outputNoFmt(KVI_OUT_SERVERINFO, szText);
}

void KviIrcServerParser::parseNumeric002(KviIrcMessage * msg)
{
	// 002: RPL_YOURHOST [I,E,U,D]
	// :prefix 002 target :Your host is <server name>, running version <server version>
	if(msg->connection()->context()->state() != KviIrcContext::Connected)
		msg->connection()->loginComplete(msg->connection()->decodeText(msg->param(0)));
	if(!msg->haltOutput())
		msg->console()->outputNoFmt(KVI_OUT_SERVERINFO, msg->connection()->decodeText(msg->safeTrailing()));
}

void KviIrcServerParser::parseNumeric003(KviIrcMessage * msg)
{
	// 003: RPL_CREATED [I,E,U,D]
	// :prefix 003 target :This server was created <date>
	if(msg->connection()->context()->state() != KviIrcContext::Connected)
		msg->connection()->loginComplete(msg->connection()->decodeText(msg->param(0)));
	if(!msg->haltOutput())
		msg->console()->outputNoFmt(KVI_OUT_SERVERINFO, msg->connection()->decodeText(msg->safeTrailing()));
}

void KviIrcServerParser::parseNumeric004(KviIrcMessage * msg)
{
	// 004: RPL_MYINFO [I,E,U,D]
	// :prefix 004 target <server_name> <srv_version> <u_modes> <ch_modes>
	if(msg->connection()->context()->state() != KviIrcContext::Connected)
		msg->connection()->loginComplete(msg->connection()->decodeText(msg->param(0)));

	int uParams = msg->paramCount();
	int uModeParam = 3;

	if(uParams < 2)
		uParams = 2;

	KviCString version = msg->safeParam(2);
	msg->connection()->serverInfo()->setServerVersion(msg->safeParam(2));

	KviCString umodes;
	// skip version number (great, thanks WEBMASTER INCORPORATED -_-)
	do
	{
		umodes = msg->safeParam(uModeParam);
	} while(((umodes.contains('.')) || (umodes.contains('-'))) && uModeParam++ < uParams);

	KviCString chanmodes = msg->safeParam(uModeParam + 1);

	if(uModeParam > 3)
	{
		version.append(' ');
		version.append(msg->safeParam(3));
	}

	if((umodes.occurrences('o') != 1) || (chanmodes.occurrences('o') != 1) || (chanmodes.occurrences('b') != 1) || (chanmodes.occurrences('v') != 1) || (chanmodes.occurrences('t') != 1) || (chanmodes.occurrences('n') != 1) || (chanmodes.contains('.')) || (chanmodes.contains('-')) || (chanmodes.contains('(')))
	{
		if(!_OUTPUT_QUIET)
		{
			msg->console()->output(KVI_OUT_SYSTEMWARNING, __tr2qs(
			                                                  "One or more standard mode flags are missing in the server available modes.\n"
			                                                  "This is caused either by a non RFC1459-compliant IRC daemon or a broken server reply.\n"
			                                                  "Server umodes seem to be '%s' and channel modes seem to be '%s'.\n"
			                                                  "Ignoring this reply and assuming that the basic set of modes is available.\n"
			                                                  "If you have strange problems, try changing the server."),
			    umodes.ptr(), chanmodes.ptr());
		}
		umodes = "oiws";         // standard support
		chanmodes = "obtkmlvsn"; // standard support
	}

	if(KVI_OPTION_BOOL(KviOption_boolShowExtendedServerInfo) && (!msg->haltOutput()))
	{
		if(umodes.hasData())
			msg->console()->outputNoFmt(KVI_OUT_SERVERINFO, __tr2qs("Available user modes:"));

		const char * aux = umodes.ptr();
		QString tmp;

		while(*aux)
		{
			tmp = msg->connection()->serverInfo()->getUserModeDescription(*aux);
			if(tmp.isEmpty())
			{
				QString tmp2 = __tr2qs(": Unknown user mode");
				tmp = QString("%1: %2").arg(*aux).arg(tmp2);
			}

			msg->console()->outputNoFmt(KVI_OUT_SERVERINFO, tmp);
			aux++;
		}

		if(chanmodes.hasData())
			msg->console()->outputNoFmt(KVI_OUT_SERVERINFO, __tr2qs("Available channel modes:"));

		aux = chanmodes.ptr();

		while(*aux)
		{
			tmp = msg->connection()->serverInfo()->getChannelModeDescription(*aux);
			if(tmp.isEmpty())
			{
				QString tmp2 = __tr2qs(": Unknown channel mode");
				tmp = QString("%1: %2").arg(*aux).arg(tmp2);
			}
			else
			{
				tmp = QString("%1: %2").arg(*aux).arg(tmp);
			}

			msg->console()->outputNoFmt(KVI_OUT_SERVERINFO, tmp);
			aux++;
		}
	}

	QString szServer = msg->connection()->decodeText(msg->safeParam(1));

	msg->connection()->serverInfoReceived(szServer, umodes.ptr(), chanmodes.ptr());

	// FIXME: #warning "TO ACTIVE ? OR TO CONSOLE ?"
	if(!_OUTPUT_MUTE && !msg->haltOutput())
		msg->console()->output(KVI_OUT_SERVERINFO,
		    __tr2qs("Server %Q version %S supporting user modes '%S' and channel modes '%S'"),
		    &szServer, &version, &umodes, &chanmodes);
}

void KviIrcServerParser::parseNumeric005(KviIrcMessage * msg)
{
	// 005: RPL_PROTOCTL [D]
	// :prefix 005 target <proto> <proto> .... :are available/supported on this server
	// 005: RPL_BOUNCE [?]
	// :prefix 005 target :Try server <server>, port <port>
	// 005: RPL_ISUPPORT
	if(msg->connection()->context()->state() != KviIrcContext::Connected)
		msg->connection()->loginComplete(msg->connection()->decodeText(msg->param(0)));

	bool bUhNames = false;
	bool bNamesx = false;

	unsigned int count = msg->paramCount();
	if(count > 2)
	{
		count--;
		for(unsigned int i = 1; i < count; i++)
		{
			const char * p = msg->param(i);
			/* Strings that we don't parse:
			 * EXCEPTS -> supports channels mode e (ban exception): we already use CHANMODES to handle that
			 * INVEX -> supports channels mode I (invite exception): we already use CHANMODES to handle that
			 * MAXCHANNELS -> Maximum number of channels allowed to join. (deprecated by CHANLIMIT, e.g. MAXCHANNELS=10)
			 * CHANLIMIT -> Maximum number of channels allowed to join by channel prefix (e.g. CHANLIMIT=#&!+:10)
			 * NICKLEN -> Maximum nickname length
			 * MAXBANS -> Maximum number of bans per channel (deprecated by MAXLIST, e.g. MAXBANS=30)
			 * MAXLIST -> Maximum number entries in the list per mode (e.g. MAXLIST=beI:30)
			 * WALLCHOPS -> The server supports messaging channel operators (deprecated by STATUSMSG, e.g. usage: NOTICE @#channel)
			 * WALLVOICES -> The server supports messaging channel voiced users (deprecated by STATUSMSG, e.g. usage: NOTICE +#channel)
			 * CASEMAPPING -> Case mapping used for nick- and channel name comparing (e.g. CASEMAPPING=rfc1459)
			 * ELIST -> search extensions to list modes, like mask search, topic search, creation time search (e.g. ELIST=MNUCT)
			 * KICKLEN -> Maximum kick comment length (e.g. KICKLEN=80)
			 * CHANNELLEN -> Maximum channel name length (e.g. CHANNELLEN=50)
			 * CHIDLEN -> Channel ID length for !channels (deprecated by IDCHAN, 5 by default, e.g. CHIDLEN=5)
			 * IDCHAN -> The ID length for channels with an ID (e.g. IDCHAN=!:5)
			 * SILENCE -> Max entries for the SILENCE command (e.g. SILENCE=15)
			 * PENALTY -> Server gives extra penalty to some commands instead of the normal 2 seconds per message and 1 second for every 120 bytes in a message.
			 * FNC -> Forced nick change: the server could change the client nickname
			 * SAFELIST -> The LIST reply won't kill the client for excess flood.
			 * AWAYLEN -> Maximum away message length (e.g. AWAYLEN=160)
			 * NOQUIT -> Server 2 server feature, no need to expose it to clients, but whatever..
			 * USERIP -> USERIP command supported
			 * CPRIVMSG -> CPRIVMSG mass-message command exists (e.g. usage: CPRIVMSG channel nick,nick2,... :text)
			 * CNOTICE -> CNOTICE mass-notice command exists (e.g. usage: CNOTICE channel nick,nick2,... :text)
			 * MAXNICKLEN -> Max length of nick for other users (like NICKLEN, but ensures the server won't overflow it for other users)
			 * MAXTARGETS -> Maximum targets allowed for PRIVMSG and NOTICE commands (e.g. MAXTARGETS=4)
			 * KNOCK -> KNOCK command supported
			 * VCHANS -> Virtual channels support
			 * WHOX -> The WHO command uses WHOX protocol.
			 * CALLERID -> The server supports server side ignores via the +g user mode
			 * ACCEPT -> The server supports server side ignore (deprecated by CALLERID)
			 * LANGUAGE -> The server supports the LANGUAGE command (experimental, e.g. LANGUAGE=2,en,i-klingon)
			 */
			if(kvi_strEqualCIN("PREFIX=(", p, 8))
			{
				p += 8;
				const char * pModes = p;
				while(*p && (*p != ')'))
					p++;
				KviCString szModeFlags(pModes, p - pModes);
				if(*p)
					p++;
				KviCString szModePrefixes = p;
				if(szModePrefixes.hasData() && (szModePrefixes.len() == szModeFlags.len()))
					msg->connection()->serverInfo()->setSupportedModePrefixes(szModePrefixes.ptr(), szModeFlags.ptr());
			}
			else if(kvi_strEqualCIN("STATUSMSG=", p, 10))
			{
				p += 10;
				KviCString tmp = p;
				if(tmp.hasData())
					msg->connection()->serverInfo()->setSupportedStatusMsgPrefixes(tmp.ptr());
			}
			else if(kvi_strEqualCIN("CHANTYPES=", p, 10))
			{
				p += 10;
				KviCString tmp = p;
				if(tmp.hasData())
					msg->connection()->serverInfo()->setSupportedChannelTypes(tmp.ptr());
			}
			else if(kvi_strEqualCI("WATCH", p) || kvi_strEqualCIN("WATCH=", p, 6))
			{
				msg->connection()->serverInfo()->setSupportsWatchList(true);
				if((!_OUTPUT_MUTE) && (!msg->haltOutput()) && KVI_OPTION_BOOL(KviOption_boolShowExtendedServerInfo))
					msg->console()->outputNoFmt(KVI_OUT_SERVERINFO, __tr2qs("This server supports the WATCH notify list method, it will be used"));
			}
			else if(kvi_strEqualCIN("TOPICLEN=", p, 9))
			{
				p += 9;
				QString tmp = p;
				if(!tmp.isEmpty())
				{
					bool ok;
					int len = tmp.toInt(&ok);
					if(ok)
						msg->connection()->serverInfo()->setMaxTopicLen(len);
				}
			}
			else if(kvi_strEqualCIN("NETWORK=", p, 8))
			{
				p += 8;
				QString tmp = p;

				if(!tmp.isEmpty())
					msg->console()->connection()->serverInfo()->setNetworkName(tmp);

				if((!_OUTPUT_MUTE) && (!msg->haltOutput()) && KVI_OPTION_BOOL(KviOption_boolShowExtendedServerInfo))
					msg->console()->output(KVI_OUT_SERVERINFO, __tr2qs("The current network is %Q"), &tmp);
			}
			else if(kvi_strEqualCI("CODEPAGES", p))
			{
				msg->connection()->serverInfo()->setSupportsCodePages(true);
				if((!_OUTPUT_MUTE) && (!msg->haltOutput()) && KVI_OPTION_BOOL(KviOption_boolShowExtendedServerInfo))
					msg->console()->outputNoFmt(KVI_OUT_SERVERINFO, __tr2qs("This server supports the CODEPAGE command, it will be used"));

			}
			else if(kvi_strEqualCIN("CHANMODES=", p, 10))
			{
				p += 10;
				QString tmp = p;
				msg->connection()->serverInfo()->setSupportedChannelModes(tmp);
			}
			else if(kvi_strEqualCIN("MODES=", p, 6))
			{
				p += 6;
				QString tmp = p;
				bool bok;
				int num = tmp.toUInt(&bok);
				if(bok)
					msg->connection()->serverInfo()->setMaxModeChanges(num);
			}
			else if(kvi_strEqualCIN("NAMESX", p, 6))
			{
				p += 6;
				bNamesx = true;
			}
			else if(kvi_strEqualCIN("UHNAMES", p, 7))
			{
				p += 7;
				bUhNames = true;
			}
			else if(kvi_strEqualCIN("CHARSET=", p, 8))
			{
				// This is commonly present, but some implementations are surely incompatible
				p += 8;
				QString tmp = p;
				msg->connection()->serverInfo()->setSupportsCodePages(true);

				if((!_OUTPUT_MUTE) && (!msg->haltOutput()) && KVI_OPTION_BOOL(KviOption_boolShowExtendedServerInfo))
					msg->console()->outputNoFmt(KVI_OUT_SERVERINFO, __tr2qs("This server supports the CODEPAGE command, it will be used"));

			}
			else if(kvi_strEqualCIN("WHOX", p, 4))
			{
				msg->connection()->serverInfo()->setSupportsWhox(true);
				if((!_OUTPUT_MUTE) && (!msg->haltOutput()) && KVI_OPTION_BOOL(KviOption_boolShowExtendedServerInfo))
					msg->console()->outputNoFmt(KVI_OUT_SERVERINFO, __tr2qs("This server supports the WHOX, extra information will be retrieved"));
			}
		}
		if((!_OUTPUT_MUTE) && (!msg->haltOutput()))
		{
			const char * aux = msg->allParams();
			while(*aux == ' ')
				aux++;
			while(*aux && (*aux != ' '))
				aux++;
			while(*aux == ' ')
				aux++;
			if(*aux == ':')
				aux++;
			if(!msg->haltOutput())
				msg->console()->output(KVI_OUT_SERVERINFO, __tr2qs("This server supports: %s"), msg->connection()->decodeText(aux).toUtf8().data());
			if(bNamesx || bUhNames)
				msg->connection()->sendFmtData("PROTOCTL %s %s", bNamesx ? "NAMESX" : "", bUhNames ? "UHNAMES" : "");
		}
	}
	else
	{
		QString inf = msg->connection()->decodeText(msg->safeTrailing());
		if(!msg->haltOutput())
			msg->console()->outputNoFmt(KVI_OUT_SERVERINFO, inf);
	}
}

void KviIrcServerParser::parseNumericSnomask(KviIrcMessage * msg)
{
	// 008: RPL_SNOMASK
	// :prefix 008 <target> <snomask>
	if(!msg->haltOutput())
	{
		KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolServerRepliesToActiveWindow) ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console());
		pOut->output(KVI_OUT_MODE, __tr2qs("You have set snomask %s"), msg->safeParam(1));
	}
}

void KviIrcServerParser::parseNumericYourUID(KviIrcMessage * msg)
{
	// 042: RPL_YOURID
	// :prefix 042 <target> <UID> :your unique ID
	if(!msg->haltOutput())
	{
		QString szUID = msg->connection()->decodeText(msg->safeParam(1));

		// Not important to us, just pass it off as server info.
		msg->console()->output(KVI_OUT_SERVERINFO, __tr2qs("%Q is your unique ID"), &szUID);
	}
}

void KviIrcServerParser::parseNumericMotd(KviIrcMessage * msg)
{
	// 372: RPL_MOTD [I,E,U,D]
	// :prefix 372 target : - <motd>
	// 377: RPL_MOTD2 [?]
	// :prefix 377 target : - <motd>
	// 375: RPL_MOTDSTART [I,E,U,D]
	// :prefix 375 target : - <server> Message of the Day -
	// 372: RPL_ENDOFMOTD [I,E,U,D]
	// :prefix 376 target :End of /MOTD command.
	// FIXME: #warning "SKIP MOTD, MOTD IN A SEPARATE WINDOW, SILENT ENDOFMOTD, MOTD IN ACTIVE WINDOW"
	if(!msg->haltOutput())
		msg->console()->outputNoFmt(KVI_OUT_MOTD, msg->connection()->decodeText(msg->safeTrailing()), 0, msg->serverTime());

	if(msg->numeric() == RPL_ENDOFMOTD)
		msg->connection()->endOfMotdReceived();
}

void KviIrcServerParser::parseNumericEndOfNames(KviIrcMessage * msg)
{
	// 366: RPL_ENDOFNAMES [I,E,U,D]
	// :prefix 366 target <channel> :End of /NAMES list.
	QString szChan = msg->connection()->decodeText(msg->safeParam(1));
	KviChannelWindow * chan = msg->connection()->findChannel(szChan);
	if(chan && !chan->hasAllNames())
	{
		chan->setHasAllNames();
		return;
	}

	if(!msg->haltOutput() && !_OUTPUT_MUTE)
	{
		// FIXME: #warning "KVI_OUT_NAMES missing"
		KviWindow * pOut = chan ? chan : KVI_OPTION_BOOL(KviOption_boolServerRepliesToActiveWindow) ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console());
		pOut->output(KVI_OUT_UNHANDLED, __tr2qs("End of NAMES for \r!c\r%Q\r"), &szChan);
	}
}

void KviIrcServerParser::parseNumeric020(KviIrcMessage * msg)
{
	// 020: RPL_CONNECTING
	//:irc.dotsrc.org 020 * :Please wait while we process your connection.
	QString szServer;
	if(!msg->haltOutput())
	{
		QString szWText = msg->console()->decodeText(msg->safeTrailing());
		msg->console()->output(
		    KVI_OUT_CONNECTION, "%c\r!s\r%s\r%c: %Q", KviControlCodes::Bold,
		    msg->safePrefix(), KviControlCodes::Bold, &szWText);
	}
}

void KviIrcServerParser::parseNumericNames(KviIrcMessage * msg)
{
	// 353: RPL_NAMREPLY [I,E,U,D]
	// :prefix 353 target [=|*|@] <channel> :<space_separated_list_of_nicks>

	// [=|*|@] is the type of the channel:
	// = --> public  * --> private   @ --> secret
	// ...but we ignore it
	QString szChan = msg->connection()->decodeText(msg->safeParam(2));
	KviChannelWindow * chan = msg->connection()->findChannel(szChan);
	// and run to the first nickname
	char * aux = msg->safeTrailingString().ptr();
	while((*aux) && (*aux == ' '))
		aux++;
	// now check if we have that channel

	char * trailing = aux;

	bool bHalt = msg->haltOutput();

	if(chan)
	{
		bHalt = bHalt || (!chan->hasAllNames());

		// K...time to parse a lot of data
		chan->enableUserListUpdates(false);

		int iPrevFlags = chan->myFlags();

		KviIrcConnectionServerInfo * pServerInfo = msg->connection()->serverInfo();

		while(*aux)
		{
			int iFlags = 0;
			// @ = op (+o), + = voiced (+v), % = halfop (+h), - = userop (+u), ^ = protected (+a?), * = chan owner (+q), !, & = channel admin (+a?)
			// ^  +a is a weird mode: it also breaks nicknames on some networks!
			// not a valid first char(s) of nickname, must be a mode prefix

			while(pServerInfo->isSupportedModePrefix(static_cast<unsigned char>(*aux)))
			{
				// leading umode flag(s)
				iFlags |= pServerInfo->modeFlagFromPrefixChar(*aux);
				aux++;
			}

			char * begin = aux;
			while(*aux && (*aux != ' '))
				aux++;
			char save = *aux;
			*aux = 0;
			// NAMESX + UHNAMES support
			KviIrcMask mask(msg->connection()->decodeText(begin));
			// and make it join
			if(!mask.nick().isEmpty())
				chan->join(mask.nick(),
				    mask.hasUser() ? mask.user() : QString(),
				    mask.hasHost() ? mask.host() : QString(),
				    iFlags);
			*aux = save;
			// run to the next nick (or the end)
			while((*aux) && (*aux == ' '))
				aux++;
		}

		if(iPrevFlags != chan->myFlags())
			chan->updateCaption();

		chan->enableUserListUpdates(true);
		// finished a block
	}

	// So it is a result of a /NAMES command or a local desync
	// We handle it in a cool way.

	if(!bHalt)
	{
		// FIXME: #warning "KVI_OUT_NAMES missing"
		KviWindow * pOut = chan ? chan : KVI_OPTION_BOOL(KviOption_boolServerRepliesToActiveWindow) ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console());
		QString szTrailing = trailing ? msg->connection()->decodeText(trailing) : QString("");
		pOut->output(KVI_OUT_UNHANDLED, __tr2qs("NAMES for \r!c\r%Q\r: %Q"), &szChan, &szTrailing);
	}
}

#ifdef COMPILE_CRYPT_SUPPORT
#define DECRYPT_IF_NEEDED(_target, _txt, _type, _type2, _buffer, _retptr, _retmsgtype)                                           \
	if(KviCryptSessionInfo * cinf = _target->cryptSessionInfo())                                                                 \
	{                                                                                                                            \
		if(cinf->m_bDoDecrypt)                                                                                                   \
		{                                                                                                                        \
			switch(cinf->m_pEngine->decrypt(_txt, _buffer))                                                                      \
			{                                                                                                                    \
				case KviCryptEngine::DecryptOkWasEncrypted:                                                                      \
					_retptr = _buffer.ptr();                                                                                     \
					_retmsgtype = _type2;                                                                                        \
					break;                                                                                                       \
				case KviCryptEngine::DecryptOkWasPlainText:                                                                      \
				case KviCryptEngine::DecryptOkWasEncoded:                                                                        \
					_retptr = _buffer.ptr();                                                                                     \
					_retmsgtype = _type;                                                                                         \
					break;                                                                                                       \
				default: /* also case KviCryptEngine::DecryptError: */                                                           \
				{                                                                                                                \
					QString szEngineError = cinf->m_pEngine->lastError();                                                        \
					_target->output(KVI_OUT_SYSTEMERROR,                                                                         \
					    __tr2qs("The following message appears to be encrypted, but the crypto engine failed to decode it: %Q"), \
					    &szEngineError);                                                                                         \
					_retptr = _txt + 1;                                                                                          \
					_retmsgtype = _type;                                                                                         \
				}                                                                                                                \
				break;                                                                                                           \
			}                                                                                                                    \
		}                                                                                                                        \
		else                                                                                                                     \
			_retptr = _txt, _retmsgtype = _type;                                                                                 \
	}                                                                                                                            \
	else                                                                                                                         \
		_retptr = _txt, _retmsgtype = _type;
#else //COMPILE_CRYPT_SUPPORT
#define DECRYPT_IF_NEEDED(_target, _txt, _type, _type2, _buffer, _retptr, _retmsgtype) \
	_retptr = _txt;                                                                    \
	_retmsgtype = _type;
#endif //COMPILE_CRYPT_SUPPORT

void KviIrcServerParser::parseNumericTopic(KviIrcMessage * msg)
{
	// 332: RPL_TOPIC [I,E,U,D]
	// :prefix 332 target <channel> :<topic>
	QString szChan = msg->connection()->decodeText(msg->safeParam(1));
	KviChannelWindow * chan = msg->connection()->findChannel(szChan);
	if(chan)
	{
		KviCString szBuffer;
		const char * txtptr;
		int msgtype;

		DECRYPT_IF_NEEDED(chan, msg->safeTrailing(), KVI_OUT_TOPIC, KVI_OUT_TOPICCRYPTED, szBuffer, txtptr, msgtype)

		QString szTopic = chan->decodeText(txtptr);

		chan->topicWidget()->setTopic(szTopic);
		chan->topicWidget()->setTopicSetBy(__tr2qs("(unknown)"));
		if(KVI_OPTION_BOOL(KviOption_boolEchoNumericTopic) && !msg->haltOutput())
			chan->output(msgtype, __tr2qs("Channel topic is: %Q"), &szTopic);
	}
	else
	{
		KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolServerRepliesToActiveWindow) ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console());

		QString szTopic = msg->connection()->decodeText(msg->safeTrailing());
		pOut->output(KVI_OUT_TOPIC, __tr2qs("Topic for \r!c\r%Q\r is: %Q"),
		    &szChan, &szTopic);
	}
}

void KviIrcServerParser::parseNumericNoTopic(KviIrcMessage * msg)
{
	// 331: RPL_NOTOPIC [I,E,U,D]
	// :prefix 331 target <channel> :No topic is set
	QString szChan = msg->connection()->decodeText(msg->safeParam(1));
	KviChannelWindow * chan = msg->connection()->findChannel(szChan);
	if(chan)
	{
		chan->topicWidget()->setTopic("");
		if(KVI_OPTION_BOOL(KviOption_boolEchoNumericTopic) && !msg->haltOutput())
			chan->outputNoFmt(KVI_OUT_TOPIC, __tr2qs("No channel topic is set"));
	}
	else
	{
		KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolServerRepliesToActiveWindow) ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console());
		pOut->output(KVI_OUT_TOPIC, __tr2qs("No topic is set for channel \r!c\r%Q\r"),
		    &szChan);
	}
}

void KviIrcServerParser::parseNumericTopicWhoTime(KviIrcMessage * msg)
{
	// 333: RPL_TOPICWHOTIME [e,U,D]
	// :prefix 333 target <channel> <whoset> <time>

	QString szChan = msg->connection()->decodeText(msg->safeParam(1));
	KviChannelWindow * chan = msg->connection()->findChannel(szChan);

	KviCString tmp = msg->safeParam(3);
	bool bOk = false;
	unsigned long t = 0;
	if(tmp.hasData())
		t = tmp.toUInt(&bOk);

	QString szDate;
	QDateTime date;
	date.setTime_t(t);

	switch(KVI_OPTION_UINT(KviOption_uintOutputDatetimeFormat))
	{
		case 0:
			// this is the equivalent to an empty date.toString() call, but it's needed
			// to ensure qt4 will use the default() locale and not the system() one
			szDate = QLocale().toString(date, "ddd MMM d hh:mm:ss yyyy");
			break;
		case 1:
			szDate = date.toString(Qt::ISODate);
			break;
		case 2:
			szDate = date.toString(Qt::SystemLocaleShortDate);
			break;
	}

	QString szWho = msg->connection()->decodeText(msg->safeParam(2));
	KviIrcMask who(szWho);
	QString szDisplayableWho;
	if(!(who.hasUser() && who.hasHost()))
		szDisplayableWho = "\r!n\r" + szWho + "\r";
	else
		szDisplayableWho = QString("\r!n\r%1\r!%2@\r!h\r%3\r").arg(who.nick(), who.user(), who.host());

	if(chan)
	{
		chan->topicWidget()->setTopicSetBy(szWho);
		if(bOk)
			chan->topicWidget()->setTopicSetAt(szDate);
		if(KVI_OPTION_BOOL(KviOption_boolEchoNumericTopic))
		{
			if(!msg->haltOutput())
			{
				if(bOk)
					chan->output(KVI_OUT_TOPIC, __tr2qs("Topic was set by %Q on %Q"), &szDisplayableWho, &szDate);
				else
					chan->output(KVI_OUT_TOPIC, __tr2qs("Topic was set by %Q"), &szDisplayableWho);
			}
		}
	}
	else
	{
		KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolServerRepliesToActiveWindow) ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console());
		if(bOk)
		{
			pOut->output(KVI_OUT_TOPIC, __tr2qs("Topic for \r!c\r%Q\r was set by %Q on %Q"),
			    &szChan, &szDisplayableWho, &szDate);
		}
		else
		{
			pOut->output(KVI_OUT_TOPIC, __tr2qs("Topic for \r!c\r%Q\r was set by %Q"),
			    &szChan, &szDisplayableWho);
		}
	}
}

void KviIrcServerParser::parseNumericChannelModeIs(KviIrcMessage * msg)
{
	// 324: RPL_CHANNELMODEIS [I,E,U,D]
	// :prefix 324 target <channel> +<chanmode>
	QString szSource = msg->connection()->decodeText(msg->safePrefix());
	QString szChan = msg->connection()->decodeText(msg->safeParam(1));
	KviChannelWindow * chan = msg->connection()->findChannel(szChan);
	KviCString modefl = msg->safeParam(2);
	if(chan)
		parseChannelMode(szSource, "*", "*", chan, modefl, msg, 3);
	else
	{
		KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolServerRepliesToActiveWindow) ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console());
		if((!szChan.isEmpty()) && IS_CHANNEL_TYPE_FLAG(szChan[0]))
		{
			pOut->output(KVI_OUT_CHANMODE, __tr2qs("Channel mode for \r!c\r%Q\r is %s"),
			    &szChan, msg->safeParam(2));
		}
		else
		{
			pOut->output(KVI_OUT_MODE, __tr2qs("User mode for \r!n\r%Q\r is %s"),
			    &szChan, msg->safeParam(2));
		}
	}
}

void getDateTimeStringFromCharTimeT(QString & szBuffer, const char * time_t_string)
{
	KviCString szTmp = time_t_string;
	bool bOk = false;
	unsigned int uTime = szTmp.toUInt(&bOk);
	if(bOk)
	{
		QDateTime date;
		date.setTime_t(uTime);

		switch(KVI_OPTION_UINT(KviOption_uintOutputDatetimeFormat))
		{
			case 0:
				// this is the equivalent to an empty date.toString() call, but it's needed
				// to ensure qt4 will use the default() locale and not the system() one
				szBuffer = QLocale().toString(date, "ddd MMM d hh:mm:ss yyyy");
				break;
			case 1:
				szBuffer = date.toString(Qt::ISODate);
				break;
			case 2:
				szBuffer = date.toString(Qt::SystemLocaleShortDate);
				break;
		}
	}
	else
		szBuffer = __tr2qs("(Unknown)");
}

void KviIrcServerParser::parseNumeric367(KviIrcMessage * msg)
{
	// 367: RPL_BANLIST [I,E,U,D]
	// :prefix 367 target <channel> <banmask> [bansetby] [bansetat]
	QString szChan = msg->connection()->decodeText(msg->safeParam(1));
	QString banmask = msg->connection()->decodeText(msg->safeParam(2));
	QString bansetby = msg->connection()->decodeText(msg->safeParam(3));
	QString bansetat;
	getDateTimeStringFromCharTimeT(bansetat, msg->safeParam(4));
	if(bansetby.isEmpty())
		bansetby = __tr2qs("(Unknown)");
	KviChannelWindow * chan = msg->connection()->findChannel(szChan);
	if(chan && chan->sentListRequest('b'))
	{
		chan->setModeInList('b', banmask, true, bansetby, QString(msg->safeParam(4)).toUInt());
		return;
	}
	if(!msg->haltOutput())
	{
		KviWindow * pOut = chan ? chan : KVI_OPTION_BOOL(KviOption_boolServerRepliesToActiveWindow) ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console());
		pOut->output(KVI_OUT_BAN, __tr2qs("%Q for \r!c\r%Q\r: \r!m-%c\r%Q\r (set by %Q on %Q)"),
		    &(__tr2qs("Ban listing")), &szChan, 'b', &banmask, &bansetby, &bansetat);
	}
}

void KviIrcServerParser::parseNumeric368(KviIrcMessage * msg)
{
	QString szChan = msg->connection()->decodeText(msg->safeParam(1));
	KviChannelWindow * chan = msg->connection()->findChannel(szChan);
	if(chan && chan->sentListRequest('b'))
	{
		chan->setListRequestDone('b');
		return;
	}
	if(!msg->haltOutput())
	{
		KviWindow * pOut = chan ? chan : KVI_OPTION_BOOL(KviOption_boolServerRepliesToActiveWindow) ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console());
		pOut->output(KVI_OUT_BAN, __tr2qs("End of channel %Q for \r!c\r%Q\r"), &(__tr2qs("ban list")), &szChan);
	}
}

#define PARSE_NUMERIC_ENDOFLIST(__funcname, __modechar, __daicon, __szWhatQString)                                                                                                   \
	void KviIrcServerParser::__funcname(KviIrcMessage * msg)                                                                                                                         \
	{                                                                                                                                                                                \
		QString szChan = msg->connection()->decodeText(msg->safeParam(1));                                                                                                           \
		KviChannelWindow * chan = msg->connection()->findChannel(szChan);                                                                                                            \
		if(chan)                                                                                                                                                                     \
		{                                                                                                                                                                            \
			if(chan->sentListRequest(__modechar))                                                                                                                                    \
			{                                                                                                                                                                        \
				chan->setListRequestDone(__modechar);                                                                                                                                \
				return;                                                                                                                                                              \
			}                                                                                                                                                                        \
		}                                                                                                                                                                            \
		if(!msg->haltOutput())                                                                                                                                                       \
		{                                                                                                                                                                            \
			KviWindow * pOut = chan ? chan : KVI_OPTION_BOOL(KviOption_boolServerRepliesToActiveWindow) ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console()); \
			pOut->output(__daicon, __tr2qs("End of channel %Q for \r!c\r%Q\r"), &(__szWhatQString), &szChan);                                                                        \
		}                                                                                                                                                                            \
	}

PARSE_NUMERIC_ENDOFLIST(parseNumericEndOfInviteList, 'I', KVI_OUT_INVITEEXCEPT, __tr2qs("invite list"))
PARSE_NUMERIC_ENDOFLIST(parseNumericEndOfExceptList, 'e', KVI_OUT_BANEXCEPT, __tr2qs("ban exception list"))

PARSE_NUMERIC_ENDOFLIST(parseNumericEndOfQList, 'q', KVI_OUT_BAN, __tr2qs("owner list"))
PARSE_NUMERIC_ENDOFLIST(parseNumericEndOfAList, 'a', KVI_OUT_BAN, __tr2qs("protected/admin list"))
PARSE_NUMERIC_ENDOFLIST(parseNumericEndOfReopList, 'R', KVI_OUT_BAN, __tr2qs("reop list"))
PARSE_NUMERIC_ENDOFLIST(parseNumericEndOfSpamFilterList, 'g', KVI_OUT_BAN, __tr2qs("spam filter list"))
PARSE_NUMERIC_ENDOFLIST(parseNumericEndOfChanAccessList, 'w', KVI_OUT_INVITEEXCEPT, __tr2qs("channel access list"))
PARSE_NUMERIC_ENDOFLIST(parseNumericEndOfExemptChanOpList, 'X', KVI_OUT_INVITEEXCEPT, __tr2qs("channel exemptions list"))

#define PARSE_NUMERIC_LIST(__funcname, __modechar, __ico, __szWhatQString)                                                                                                           \
	void KviIrcServerParser::__funcname(KviIrcMessage * msg)                                                                                                                         \
	{                                                                                                                                                                                \
		QString szChan = msg->connection()->decodeText(msg->safeParam(1));                                                                                                           \
		QString banmask = msg->connection()->decodeText(msg->safeParam(2));                                                                                                          \
		QString bansetby = msg->connection()->decodeText(msg->safeParam(3));                                                                                                         \
		QString bansetat;                                                                                                                                                            \
		getDateTimeStringFromCharTimeT(bansetat, msg->safeParam(4));                                                                                                                 \
		if(bansetby.isEmpty())                                                                                                                                                       \
			bansetby = __tr2qs("(Unknown)");                                                                                                                                         \
		KviChannelWindow * chan = msg->connection()->findChannel(szChan);                                                                                                            \
		if(chan)                                                                                                                                                                     \
		{                                                                                                                                                                            \
			chan->setModeInList(__modechar, banmask, true, bansetby, QString(msg->safeParam(4)).toUInt());                                                                           \
			if(chan->sentListRequest(__modechar))                                                                                                                                    \
				return;                                                                                                                                                              \
		}                                                                                                                                                                            \
		if(!msg->haltOutput())                                                                                                                                                       \
		{                                                                                                                                                                            \
			KviWindow * pOut = chan ? chan : KVI_OPTION_BOOL(KviOption_boolServerRepliesToActiveWindow) ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console()); \
			pOut->output(__ico, __tr2qs("%Q for \r!c\r%Q\r: \r!m-%c\r%Q\r (set by %Q on %Q)"),                                                                                       \
			    &(__szWhatQString), &szChan, __modechar, &banmask, &bansetby, &bansetat);                                                                                            \
		}                                                                                                                                                                            \
	}

// 346: RPL_EXCEPTLIST [I,E,U,D]
// :prefix 346 target <channel> <banmask> [bansetby] [bansetat]
PARSE_NUMERIC_LIST(parseNumericInviteList, 'I', KVI_OUT_INVITEEXCEPT, __tr2qs("Invite listing"))
PARSE_NUMERIC_LIST(parseNumericExceptList, 'e', KVI_OUT_BANEXCEPT, __tr2qs("Ban exception listing"));

PARSE_NUMERIC_LIST(parseNumericQList, 'q', KVI_OUT_BAN, __tr2qs("Owner listing"));
PARSE_NUMERIC_LIST(parseNumericAList, 'a', KVI_OUT_BAN, __tr2qs("Admin/protected nicks listing"));
PARSE_NUMERIC_LIST(parseNumericReopList, 'R', KVI_OUT_BAN, __tr2qs("Reop masks listing"));
PARSE_NUMERIC_LIST(parseNumericSpamFilterList, 'g', KVI_OUT_BAN, __tr2qs("Spam filter listing"));
PARSE_NUMERIC_LIST(parseNumericChanAccessList, 'w', KVI_OUT_INVITEEXCEPT, __tr2qs("Channel access listing"));
PARSE_NUMERIC_LIST(parseNumericExemptChanOpList, 'X', KVI_OUT_INVITEEXCEPT, __tr2qs("Channel exemptions listing"));

void KviIrcServerParser::parseNumericWhoReply(KviIrcMessage * msg)
{
	// 352: RPL_WHOREPLY [I,E,U,D]
	// :prefix 352 target <chan> <usr> <hst> <srv> <nck> <stat> :<hops> <real>

	QString szChan = msg->connection()->decodeText(msg->safeParam(1));
	QString szUser = msg->connection()->decodeText(msg->safeParam(2));
	QString szHost = msg->connection()->decodeText(msg->safeParam(3));
	QString szServ = msg->connection()->decodeText(msg->safeParam(4));
	QString szNick = msg->connection()->decodeText(msg->safeParam(5));
	QString szFlag = msg->connection()->decodeText(msg->safeParam(6));
	bool bAway = szFlag.indexOf('G') != -1;
	bool bIrcOp = szFlag.indexOf('*') != -1;

	KviCString trailing = msg->safeTrailing();
	KviCString hops = trailing.getToken(' ');
	bool bHopsOk = false;
	int iHops = hops.toInt(&bHopsOk);

	QString szReal = msg->connection()->decodeText(trailing.ptr());

	// Update the user entry
	KviIrcUserDataBase * db = msg->connection()->userDataBase();
	KviIrcUserEntry * e = db->find(szNick);
	if(e)
	{
		if(bHopsOk)
			e->setHops(iHops);
		e->setUser(szUser);
		e->setHost(szHost);
		e->setServer(szServ);
		e->setAway(bAway);
		e->setIrcOp(bIrcOp);
		e->setUserFlags(szFlag);

		KviQueryWindow * q = msg->connection()->findQuery(szNick);
		if(q)
			q->updateLabelText();

		// Check for the avatar unless the entry refers to the local user (in which case
		// the avatar should never be cached nor requested).
		if(!IS_ME(msg, szNick))
		{
			//no avatar? check for a cached one
			if(!e->avatar())
			{
				// FIXME: #warning "THE AVATAR SHOULD BE RESIZED TO MATCH THE MAX WIDTH/HEIGHT"
				// maybe now we can match this user ?
				msg->console()->checkDefaultAvatar(e, szNick, szUser, szHost);
			}
			//still no avatar? check if the user is exposing the fact that he's got one
			if(!e->avatar())
			{
				if((szReal[0].unicode() == KviControlCodes::Color) && (szReal[1].unicode() & 4) && (szReal[2].unicode() == KviControlCodes::Reset))
				{
					if(KVI_OPTION_BOOL(KviOption_boolRequestMissingAvatars) && !e->avatarRequested())
					{
						QByteArray d = msg->connection()->encodeText(szNick);
						msg->connection()->sendFmtData("%s %s :%c%s%c", "PRIVMSG", d.data(), 0x01, "AVATAR", 0x01);
						e->setAvatarRequested();
					}
				}
			}
		}

		//this has to be done after the avatar part
		e->setRealName(szReal);
	}

	KviChannelWindow * chan = msg->connection()->findChannel(szChan);
	if(chan)
	{
		if(!chan->hasWhoList())
		{
			// FIXME: #warning "IF VERBOSE && SHOW INTERNAL WHO REPLIES...."
			return;
		}
		if(chan->sentSyncWhoRequest())
		{
			// FIXME: #warning "IF VERBOSE && SHOW INTERNAL WHO REPLIES...."
			return;
		}
	}

	// FIXME: #warning "SYNC OP/VOICE on channel!!!"

	if(!msg->haltOutput())
	{
		KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolWhoRepliesToActiveWindow) && chan ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console());

		QString szAway = bAway ? __tr2qs("Yes") : __tr2qs("No");

		pOut->output(KVI_OUT_WHO,
		    __tr2qs("WHO entry for %c\r!n\r%Q\r%c [%Q@\r!h\r%Q\r]: %cChannel%c: \r!c\r%Q\r, %cServer%c: \r!s\r%Q\r, %cHops%c: %d, %cFlags%c: %Q, %cAway%c: %Q, %cReal name%c: %Q"),
		    KviControlCodes::Bold, &szNick, KviControlCodes::Bold,
		    &szUser, &szHost, KviControlCodes::Underline,
		    KviControlCodes::Underline, &szChan, KviControlCodes::Underline,
		    KviControlCodes::Underline, &szServ, KviControlCodes::Underline,
		    KviControlCodes::Underline, iHops, KviControlCodes::Underline, KviControlCodes::Underline,
		    &szFlag, KviControlCodes::Underline, KviControlCodes::Underline,
		    &szAway, KviControlCodes::Underline,
		    KviControlCodes::Underline, &szReal);
	}
}

void KviIrcServerParser::parseNumericWhospcrpl(KviIrcMessage * msg)
{
	// 354: RPL_WHOSPCRPL
	// :prefix 354 target <chan> <params>
	// :prefix 354 target <chan> <user> <host> <server> <nick> <flags> <hops> <idle> <account> :<gecos>
	// NOTE: Because you can arbitrarily send parameters to query extended WHOX,
	// we will not parse anything that was not generated by the client itself.
	QString szChan = msg->connection()->decodeText(msg->safeParam(1));

	// TODO: Send a templated response to the user.
	// We could add logic to determine what parameters they requested and try to
	// further parse it based on the response from there, but for now we'll just
	// send it as unparsed as it normally does. Also this block is sort of hacky
	// but it will do for the now thing. --BlindSight/staticfox
	KviChannelWindow * chan = msg->connection()->findChannel(szChan);
	if(chan)
	{
		if(chan->hasWhoList() && !chan->sentSyncWhoRequest())
		{
			QString szWText = msg->connection()->decodeText(msg->allParams());
			KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolWhoRepliesToActiveWindow) && chan ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console());

			pOut->output(KVI_OUT_UNHANDLED,
			    "[%s][%s] %Q", msg->prefix(), msg->command(), &szWText);
			return;
		}
	}
	else
	{
		return;
	}

	if(!chan->hasWhoList() || chan->sentSyncWhoRequest())
	{
		QString szUser = msg->connection()->decodeText(msg->safeParam(2));
		QString szHost = msg->connection()->decodeText(msg->safeParam(3));
		QString szServ = msg->connection()->decodeText(msg->safeParam(4));
		QString szNick = msg->connection()->decodeText(msg->safeParam(5));
		QString szFlag = msg->connection()->decodeText(msg->safeParam(6));
		KviCString iHops = msg->safeParam(7);
		// KviCString szIdle = msg->safeParam(8);
		QString szAcct = msg->connection()->decodeText(msg->safeParam(9));
		QString szReal = msg->connection()->decodeText(msg->safeTrailing());
		bool bAway = szFlag.indexOf('G') != -1;
		bool bIrcOp = szFlag.indexOf('*') != -1;

		bool bHops = false;
		// bool bIdle = false;
		// unsigned long idle = 0;
		int hops = 0;
		if(iHops.hasData())
			hops = iHops.toInt(&bHops);
		// if(szIdle.hasData())idle = szIdle.toUInt(&bIdle);

		// Update the user entry
		KviIrcUserDataBase * db = msg->connection()->userDataBase();
		KviIrcUserEntry * e = db->find(szNick);
		if(e)
		{
			if(bHops)
				e->setHops(hops);
			e->setUser(szUser);
			e->setHost(szHost);
			e->setServer(szServ);
			e->setAway(bAway);
			e->setIrcOp(bIrcOp);
			e->setUserFlags(szFlag);
			e->setAccountName(szAcct == "0" ? "" : szAcct);

			KviQueryWindow * q = msg->connection()->findQuery(szNick);
			if(q)
				q->updateLabelText();

			// Check for the avatar unless the entry refers to the local user (in which case
			// the avatar should never be cached nor requested).
			if(!IS_ME(msg, szNick))
			{
				//no avatar? check for a cached one
				if(!e->avatar())
				{
					// FIXME: #warning "THE AVATAR SHOULD BE RESIZED TO MATCH THE MAX WIDTH/HEIGHT"
					// maybe now we can match this user ?
					msg->console()->checkDefaultAvatar(e, szNick, szUser, szHost);
				}
				//still no avatar? check if the user is exposing the fact that he's got one
				if(!e->avatar())
				{
					if((szReal[0].unicode() == KviControlCodes::Color) && (szReal[1].unicode() & 4) && (szReal[2].unicode() == KviControlCodes::Reset))
					{
						if(KVI_OPTION_BOOL(KviOption_boolRequestMissingAvatars) && !e->avatarRequested())
						{
							QByteArray d = msg->connection()->encodeText(szNick);
							msg->connection()->sendFmtData("%s %s :%c%s%c", "PRIVMSG", d.data(), 0x01, "AVATAR", 0x01);
							e->setAvatarRequested();
						}
					}
				}
			}
			//this has to be done after the avatar part
			e->setRealName(szReal);
		}
	}
}

void KviIrcServerParser::parseNumericEndOfWho(KviIrcMessage * msg)
{
	// 315: RPL_ENDOFWHO [I,E,U,D]
	// :prefix 315 target <channel/nick> :End of /WHO List.
	QString szChan = msg->connection()->decodeText(msg->safeParam(1));
	KviChannelWindow * chan = msg->connection()->findChannel(szChan);
	if(chan)
	{
		chan->userListView()->updateArea();
		kvi_time_t tNow = kvi_unixTime();
		msg->connection()->stateData()->setLastReceivedChannelWhoReply(tNow);
		chan->setLastReceivedWhoReply(tNow);
		if(msg->connection()->lagMeter())
		{
			KviCString tmp(KviCString::Format, "WHO %s", msg->safeParam(1));
			msg->connection()->lagMeter()->lagCheckComplete(tmp.ptr());
		}

		if(!chan->hasWhoList())
		{
			// FIXME: #warning "IF VERBOSE && SHOW INTERNAL WHO REPLIES...."
			chan->setHasWhoList();
			return;
		}

		if(chan->sentSyncWhoRequest())
		{
			// FIXME: #warning "IF VERBOSE && SHOW INTERNAL WHO REPLIES...."
			chan->clearSentSyncWhoRequest();
			return;
		}
	}

	if(!msg->haltOutput())
	{
		KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolWhoRepliesToActiveWindow) && chan ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console());
		QString whoTarget = msg->connection()->decodeText(msg->safeParam(1));
		if(IS_CHANNEL_TYPE_FLAG(whoTarget[0]))
			whoTarget.prepend("\r!c\r");
		else
			whoTarget.prepend("\r!n\r");
		whoTarget.append("\r");
		pOut->output(KVI_OUT_WHO, __tr2qs("End of WHO list for %Q"), &whoTarget);
	}
}

void KviIrcServerParser::parseLoginNicknameProblem(KviIrcMessage * msg)
{
	// Oops...not logged in yet...

	QString szChoiceDescriptionBuffer;

	QString szNextNick = msg->connection()->pickNextLoginNickName(
	    false, // false = fallback to random choices, then give up with an empty string
	    msg->connection()->decodeText(msg->safeParam(1)),
	    szChoiceDescriptionBuffer);

	if(szNextNick.isEmpty())
	{
		msg->console()->output(KVI_OUT_NICKNAMEPROBLEM,
		    __tr2qs("The server is refusing all the login nicknames: giving up, you must send the nickname manually"));
		return;
	}

	QString szOldNick = msg->connection()->userInfo()->nickName();
	msg->console()->notifyListView()->nickChange(szOldNick, szNextNick);

	msg->connection()->userInfo()->setNickName(szNextNick);

	if(!msg->haltOutput())
	{
		msg->console()->output(
		    KVI_OUT_NICKNAMEPROBLEM,
		    __tr2qs("No way to login as \r!n\r%1\r: the server said '%2: %3'")
		        .arg(msg->connection()->decodeText(msg->safeParam(1)))
		        .arg(msg->numeric())
		        .arg(msg->connection()->decodeText(msg->safeTrailing())));

		QString szOut = __tr2qs("Trying to use %1 as nickname").arg(szNextNick);
		if(_OUTPUT_VERBOSE)
			szOut += QString::fromLatin1(" (%1)").arg(szChoiceDescriptionBuffer);

		msg->console()->outputNoFmt(KVI_OUT_NICKNAMEPROBLEM, szOut);
	}

	QByteArray d = msg->connection()->encodeText(szNextNick);
	msg->connection()->sendFmtData("NICK %s", d.data());
}

void KviIrcServerParser::parseNumericUnknownCommand(KviIrcMessage * msg)
{
	// 421: ERR_UNKNOWNCOMMAND
	// :prefix 421 <target> <command> :Unknown command
	if(!msg->haltOutput())
	{
		KviWindow * pOut = msg->console()->activeWindow();
		QString szCmd = msg->connection()->decodeText(msg->safeParam(1));
		pOut->output(KVI_OUT_GENERICERROR, __tr2qs("%Q is an unknown server command"), &szCmd);
	}
}

void KviIrcServerParser::parseNumericMotdMissing(KviIrcMessage * msg)
{
	// 422: ERR_NOMOTD
	// :prefix 422 <target> :- MOTD file not found!  Please contact your IRC administrator.
	if(!msg->haltOutput())
	{
		QString szText = msg->connection()->decodeText(msg->safeTrailing());
		msg->console()->output(KVI_OUT_GENERICERROR, szText);
	}
}

void KviIrcServerParser::parseNumericBanOnChan(KviIrcMessage * msg)
{
	// 435: ERR_BANONCHAN
	// :prefix 435 <target> <newnick> <channel> :Cannot change nickname while banned on channel
	if(!msg->haltOutput())
	{
		KviWindow * pOut = static_cast<KviWindow *>(msg->connection()->findChannel(msg->safeParam(2)));
		if(!pOut)
			pOut = static_cast<KviWindow *>(msg->console());
		QString szChannel = msg->connection()->decodeText(msg->safeParam(2));
		QString szWText = msg->connection()->decodeText(msg->safeTrailing());
		pOut->output(KVI_OUT_JOINERROR,
		    "\r!c\r%Q\r: %Q", &szChannel, &szWText);
	}
}

void KviIrcServerParser::parseNumericUnavailResource(KviIrcMessage * msg)
{
	// 437: ERR_UNAVAILRESOURCE [I]
	// :prefix 437 <target> <nick/channel> :Nick/Channel is temporairly unavailable
	if(!(msg->console()->isConnected()))
	{
		parseLoginNicknameProblem(msg);
	}
	else
	{
		// already connected... just say that we have problems
		if(!msg->haltOutput())
		{
			QString szNk = msg->connection()->decodeText(msg->safeParam(1));
			QString szWText = msg->connection()->decodeText(msg->safeTrailing());
			msg->console()->output(KVI_OUT_NICKNAMEPROBLEM, "\r!n\r%Q\r: %Q", &szNk, &szWText);
		}
	}
}

void KviIrcServerParser::parseNumericUserInChan(KviIrcMessage * msg)
{
	// 443: ERR_USERONCHANNEL
	// :prefix 443 <target> <nick> <channel> :is already on channel
	if(!msg->haltOutput())
	{
		KviWindow * pOut = static_cast<KviWindow *>(msg->connection()->findChannel(msg->safeParam(2)));
		if(!pOut)
			pOut = static_cast<KviWindow *>(msg->console()); // This would be interesting...
		QString szNick = msg->connection()->decodeText(msg->safeParam(1));
		QString szChan = msg->connection()->decodeText(msg->safeParam(2));
		pOut->output(KVI_OUT_GENERICERROR, __tr2qs("%Q is already in %Q"), &szNick, &szChan);
	}
}

void KviIrcServerParser::parseNumericForward(KviIrcMessage * msg)
{
	// 470: ERR_LINKCHANNEL
	// :prefix 470 target <oldchan> <newchan> :Forwarding to another channel
	if(!msg->haltOutput())
	{
		QString pref = msg->connection()->decodeText(msg->safePrefix());
		QString szOldChan = msg->connection()->decodeText(msg->safeParam(1));
		QString szNewChan = msg->connection()->decodeText(msg->safeParam(2));
		KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolServerNoticesToActiveWindow) ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console());
		// This technically isn't a notice, yet it is fairly useful information.
		// The server gives us good data with a pretty unusable description. So
		// what we are doing is concatenating the generic server notice prefix
		// with a more useful message so gettext can parse our final output easier.
		pOut->output(KVI_OUT_SERVERNOTICE, msg->serverTime(),
		    "[\r!s\r%Q\r]: " + __tr2qs("You are being forwarded from %Q to %Q"), &pref, &szOldChan, &szNewChan);
	}
}

void KviIrcServerParser::parseNumericCantJoinChannel(KviIrcMessage * msg)
{
	// 471: ERR_CHANNELISFULL [I,E,U,D]
	// 473: ERR_INVITEONLYCHAN [I,E,U,D]
	// 474: ERR_BANNEDFROMCHAN [I,E,U,D]
	// 475: ERR_BADCHANNELKEY [I,E,U,D]
	// :prefix 47* <target> <channel> :Can't join channel (+l/i/b/k)
	if(!msg->haltOutput())
	{
		KviWindow * pOut = static_cast<KviWindow *>(msg->connection()->findChannel(msg->safeParam(1)));
		if(!pOut)
			pOut = static_cast<KviWindow *>(msg->console());
		QString szChannel = msg->connection()->decodeText(msg->safeParam(1));
		QString szWText = msg->connection()->decodeText(msg->safeTrailing());
		pOut->output(KVI_OUT_JOINERROR,
		    "\r!c\r%Q\r: %Q", &szChannel, &szWText);
	}
}

// Keep the source ordered: this should be named "parseOtherChannelError"

void KviIrcServerParser::parseNumericNoPrivs(KviIrcMessage * msg)
{
	// 481: ERR_NOPRIVILEGES
	// :prefix 481 <target> :Permission Denied - You're not an IRC operator
	if(!msg->haltOutput())
	{
		KviWindow * pOut = static_cast<KviWindow *>(msg->console());
		QString szText = msg->connection()->decodeText(msg->safeTrailing());
		pOut->output(KVI_OUT_GENERICERROR, szText);
	}
}

void KviIrcServerParser::otherChannelError(KviIrcMessage * msg)
{
	// 482: ERR_CHANOPRIVSNEEDED
	// 467: ERR_KEYSET
	// 472: ERR_UNKNOWNMODE
	// :prefix 4?? <target> <channel> :error text
	if(!msg->haltOutput())
	{
		KviWindow * pOut = static_cast<KviWindow *>(msg->connection()->findChannel(msg->safeParam(1)));
		if(!pOut)
			pOut = static_cast<KviWindow *>(msg->console());
		QString szChannel = msg->connection()->decodeText(msg->safeParam(1));
		QString szWText = msg->connection()->decodeText(msg->safeTrailing());
		pOut->output(KVI_OUT_GENERICERROR, "\r!c\r%Q\r: %Q", &szChannel, &szWText);
	}
}

void KviIrcServerParser::parseNumeric486(KviIrcMessage * msg)
{
	// 486 ERR_NONONREG
	// :prefix 486 <target> <remotenick> :You must identify to a registered nick to private message that person

	// Unreal 3.2 (ERR_HTMDISABLED)
	// :prefix 486 <target> :CMD is currently disabled, please try again later.
	if(!msg->haltOutput())
	{
		QString szNick = msg->connection()->decodeText(msg->safeParam(1));
		QString szText = msg->connection()->decodeText(msg->safeTrailing());
		KviIrcConnectionServerInfo * pServerInfo = msg->connection()->serverInfo();
		QString version = pServerInfo->software();

		// HTM disables the command, so send the message to their active window
		if(version == "Unreal32")
		{
			KviWindow * pOut = static_cast<KviWindow *>(msg->console()->activeWindow());
			pOut->output(KVI_OUT_GENERICERROR, szText);
		}
		else
		{
			// We're trying to message them, so send it to the query window
			KviWindow * pOut = static_cast<KviWindow *>(msg->connection()->findQuery(szNick));
			if(!pOut)
				pOut = static_cast<KviWindow *>(msg->console());
			pOut->output(KVI_OUT_HELP, szText);
		}
	}
}

void KviIrcServerParser::parseCommandSyntaxHelp(KviIrcMessage * msg)
{
	// 704 RPL_COMMANDSYNTAX
	// :prefix 704 <target> <command> :text
	if(!msg->haltOutput())
	{
		KviWindow * pOut = static_cast<KviWindow *>(msg->console());
		QString szCommand = msg->connection()->decodeText(msg->safeParam(1));
		QString szWText = msg->connection()->decodeText(msg->safeTrailing());
		pOut->output(KVI_OUT_HELP,
		    __tr2qs("Command syntax %Q: %Q"), &szCommand, &szWText); // Pragma: wheee..... that should be in English :D
	}
}

void KviIrcServerParser::parseCommandHelp(KviIrcMessage * msg)
{
	// 705 RPL_COMMANDHELP
	// :prefix 705 <target> <command> :text
	if(!msg->haltOutput())
	{
		KviWindow * pOut = static_cast<KviWindow *>(msg->console());
		QString szCommand = msg->connection()->decodeText(msg->safeParam(1));
		QString szWText = msg->connection()->decodeText(msg->safeTrailing());
		pOut->outputNoFmt(KVI_OUT_HELP, szWText);
	}
}

void KviIrcServerParser::parseNumericNotifyGeneric(KviIrcMessage * msg)
{
	// 716 RPL_TARGUMODEG
	// 717 RPL_TARGNOTIFY
	// :prefix 71? target <nick> :has been informed that you messaged them.
	if(!msg->haltOutput())
	{
		QString szNick = msg->connection()->decodeText(msg->safeParam(1));
		QString szText = msg->connection()->decodeText(msg->safeTrailing());

		// Send this to the active query, since if they are trying to message this
		// user then it should appear directly within the window. Otherwise, just
		// send it to console.
		KviWindow * pOut = static_cast<KviWindow *>(msg->connection()->findQuery(szNick));

		if(!pOut)
			pOut = static_cast<KviWindow *>(msg->console());

		pOut->output(KVI_OUT_HELP, "%Q %Q", &szNick, &szText);
	}
}

void KviIrcServerParser::parseNumericYouHaveCallerID(KviIrcMessage * msg)
{
	// 718 RPL_UMODEGMSG
	// IRC is one of the most inconsistent enigmas on the face of the earth.
	// :prefix 718 <target> <remote nick[ [user@host]]> :is messaging you, and you are umode +g or +G.
	if(!msg->haltOutput())
	{
		QString szRemoteUser = msg->connection()->decodeText(msg->safeParam(1));
		QString szRemoteHost = msg->connection()->decodeText(msg->safeParam(2));
		QString szText = msg->connection()->decodeText(msg->safeTrailing());

		// This would be ironic if it hit, but you never know...
		KviWindow * pOut = static_cast<KviWindow *>(msg->connection()->findQuery(szRemoteUser));

		if(!pOut)
			pOut = static_cast<KviWindow *>(msg->console());

		// Because some IRCds return the host jammed together with the nick in an awkward way, making one
		// less parameter returned in RPL_UMODEGMSG.
		if(szRemoteHost == szText)
			pOut->output(KVI_OUT_HELP, "%Q %Q", &szRemoteUser, &szText);
		else
			pOut->output(KVI_OUT_HELP, "%Q!%Q %Q", &szRemoteUser, &szRemoteHost, &szText);
	}
}

void KviIrcServerParser::parseChannelHelp(KviIrcMessage * msg)
{
	// 477 RPL_CHANNELHELP (freenode)
	// :prefix 477 <target> <channel> :text
	if(!msg->haltOutput())
	{
		QString szChan = msg->connection()->decodeText(msg->safeParam(1));
		QString szText = msg->connection()->decodeText(msg->safeTrailing());
		KviWindow * pOut = msg->connection()->findChannel(szChan);
		if(pOut)
		{
			pOut->output(KVI_OUT_HELP, __tr2qs("Tip: %Q"), &szText);
		}
		else
		{
			pOut = static_cast<KviWindow *>(msg->console());
			pOut->output(KVI_OUT_HELP, __tr2qs("Tip for \r!c\r%Q\r: %Q"), &szChan, &szText);
		}
	}
}

void KviIrcServerParser::parseNumericNeedSSL(KviIrcMessage * msg)
{
	//  480 ERR_SSLONLYCHAN (hybrid forks + others)
	// :prefix 480 <target> <channel> :<text>
	if(!msg->haltOutput())
	{
		QString szChan = msg->connection()->decodeText(msg->safeParam(1));
		KviWindow * pOut = msg->connection()->findChannel(szChan);
		if(pOut)
		{
			pOut->output(KVI_OUT_HELP, __tr2qs("%Q requires SSL to join"), &szChan);
		}
		else
		{
			pOut = static_cast<KviWindow *>(msg->console());
			pOut->output(KVI_OUT_HELP, __tr2qs("%Q requires SSL to join"), &szChan);
		}
	}
}

void KviIrcServerParser::parseCommandEndOfHelp(KviIrcMessage * msg)
{
	// 704 RPL_COMMANDSYNTAX
	// 705 RPL_COMMANDHELP
	// :prefix 706 <target> <command> :End of /HELP.
	if(!msg->haltOutput())
	{
		KviWindow * pOut = static_cast<KviWindow *>(msg->console());
		QString szCommand = msg->connection()->decodeText(msg->safeParam(1));
		pOut->output(KVI_OUT_HELP,
		    __tr2qs("End of help about %Q"), &szCommand);
	}
}

void KviIrcServerParser::parseNumericNicknameProblem(KviIrcMessage * msg)
{
	// 433: ERR_NICKNAMEINUSE [I,E,U,D]
	// :prefix 433 <target> <nick> :Nickname is already in use.
	// 432: ERR_ERRONEUSNICKNAME [I,E,U,D]
	// :prefix 433 <target> <nick> :Erroneous nickname

	if(!(msg->console()->isConnected()))
	{
		parseLoginNicknameProblem(msg);
	}
	else
	{
		// already connected... just say that we have problems
		if(!msg->haltOutput())
		{
			QString szNk = msg->connection()->decodeText(msg->safeParam(1));
			QString szWText = msg->connection()->decodeText(msg->safeTrailing());
			msg->console()->output(KVI_OUT_NICKNAMEPROBLEM,
			    "\r!n\r%Q\r: %Q", &szNk, &szWText);
		}
	}
}

void KviIrcServerParser::parseNumericWhoisAway(KviIrcMessage * msg)
{
	// FIXME: #warning "Need an icon here too: sth like KVI_OUT_WHOISSERVER, but with 'A' letter"
	msg->connection()->stateData()->setLastReceivedWhoisReply(kvi_unixTime());

	QString szNk = msg->connection()->decodeText(msg->safeParam(1));
	KviIrcUserDataBase * db = msg->connection()->userDataBase();
	KviIrcUserEntry * e = db->find(szNk);
	if(e)
		e->setAway(true);
	KviQueryWindow * q = msg->connection()->findQuery(szNk);
	if(q)
		q->updateLabelText();
	QString szWText = msg->connection()->decodeText(msg->safeTrailing());

	KviAsyncWhoisInfo * i = msg->connection()->asyncWhoisData()->lookup(szNk);
	if(i)
	{
		// The szAway (previously szSpecial) is only modified by parseNumericWhoisAway
		// So no use appending here
		i->szAway = szWText;
		return;
	}

	if(!msg->haltOutput())
	{
		KviWindow * pOut = static_cast<KviWindow *>(msg->connection()->findQuery(szNk));

		if(!pOut)
			pOut = KVI_OPTION_BOOL(KviOption_boolWhoisRepliesToActiveWindow) ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console());
		pOut->output(KVI_OUT_WHOISUSER, __tr2qs("%c\r!n\r%Q\r%c is away: %Q"),
		    KviControlCodes::Bold, &szNk, KviControlCodes::Bold, &szWText);
	}
}

void KviIrcServerParser::parseNumericWhoisUser(KviIrcMessage * msg)
{
	// 311: RPL_WHOISUSER [I,E,U,D]
	// :prefix 311 <target> <nick> <user> <host> * :<real_name>
	msg->connection()->stateData()->setLastReceivedWhoisReply(kvi_unixTime());

	QString szNick = msg->connection()->decodeText(msg->safeParam(1));
	QString szUser = msg->connection()->decodeText(msg->safeParam(2));
	QString szHost = msg->connection()->decodeText(msg->safeParam(3));
	QString szReal = msg->connection()->decodeText(msg->safeTrailing());
	KviIrcUserDataBase * db = msg->connection()->userDataBase();
	KviIrcUserEntry * e = db->find(szNick);
	if(e)
	{
		e->setUser(szUser);
		e->setHost(szHost);
		e->setRealName(szReal);
		if(e->gender() != KviIrcUserEntry::Unknown)
		{
			// hum... this is ugly
			if(KviQString::equalCS(g_pActiveWindow->metaObject()->className(), QString("KviChannelWindow")))
				(static_cast<KviChannelWindow *>(g_pActiveWindow))->userListView()->updateArea();
		}

		KviQueryWindow * q = msg->connection()->findQuery(szNick);
		if(q)
			q->updateLabelText();

		// Check for the avatar unless the entry refers to the local user (in which case
		// the avatar should never be cached nor requested).
		if(!IS_ME(msg, szNick) && !e->avatar())
		{
			// FIXME: #warning "THE AVATAR SHOULD BE RESIZED TO MATCH THE MAX WIDTH/HEIGHT"
			// maybe now we can match this user ?
			msg->console()->checkDefaultAvatar(e, szNick, szUser, szHost);
		}
	}

	KviAsyncWhoisInfo * i = msg->connection()->asyncWhoisData()->lookup(szNick);
	if(i)
	{
		i->szNick = szNick;
		i->szUser = szUser;
		i->szHost = szHost;
		i->szReal = szReal;
		return;
	}

	if(!msg->haltOutput())
	{
		KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolWhoisRepliesToActiveWindow) ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console());
		pOut->output(
		    KVI_OUT_WHOISUSER, __tr2qs("%c\r!n\r%Q\r%c is %c\r!n\r%Q\r!%Q@\r!h\r%Q\r%c"), KviControlCodes::Bold,
		    &szNick, KviControlCodes::Bold, KviControlCodes::Underline, &szNick,
		    &szUser, &szHost, KviControlCodes::Underline);

		pOut->output(
		    KVI_OUT_WHOISUSER, __tr2qs("%c\r!n\r%Q\r%c's real name: %Q"), KviControlCodes::Bold,
		    &szNick, KviControlCodes::Bold, &szReal);
	}
}

void KviIrcServerParser::parseNumericWhowasUser(KviIrcMessage * msg)
{
	// 314: RPL_WHOWASUSER [I,E,U,D]
	// :prefix 314 <target> <nick> <user> <host> * :<real_name>

	msg->connection()->stateData()->setLastReceivedWhoisReply(kvi_unixTime());

	if(!msg->haltOutput())
	{
		QString szNick = msg->connection()->decodeText(msg->safeParam(1));
		QString szUser = msg->connection()->decodeText(msg->safeParam(2));
		QString szHost = msg->connection()->decodeText(msg->safeParam(3));
		QString szReal = msg->connection()->decodeText(msg->safeTrailing());

		KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolWhoisRepliesToActiveWindow) ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console());
		pOut->output(
		    KVI_OUT_WHOISUSER, __tr2qs("%c\r!n\r%Q\r%c was %c\r!n\r%Q\r!%Q@\r!h\r%Q\r%c"), KviControlCodes::Bold,
		    &szNick, KviControlCodes::Bold, KviControlCodes::Underline, &szNick,
		    &szUser, &szHost, KviControlCodes::Underline);
		pOut->output(
		    KVI_OUT_WHOISUSER, __tr2qs("%c\r!n\r%Q\r%c's real name was: %Q"), KviControlCodes::Bold,
		    &szNick, KviControlCodes::Bold, &szReal);
	}
}

void KviIrcServerParser::parseNumericWhoisChannels(KviIrcMessage * msg)
{
	// 319: RPL_WHOISCHANNELS [I,E,U,D]
	// :prefix 319 <target> <nick> :<channel list>

	msg->connection()->stateData()->setLastReceivedWhoisReply(kvi_unixTime());

	QString szNick = msg->connection()->decodeText(msg->safeParam(1));
	QString szChans = msg->connection()->decodeText(msg->safeTrailing());

	KviAsyncWhoisInfo * i = msg->connection()->asyncWhoisData()->lookup(szNick);
	if(i)
	{
		i->szChannels = szChans;
		return;
	}

	if(!msg->haltOutput())
	{
		KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolWhoisRepliesToActiveWindow) ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console());

		QStringList sl = szChans.split(" ", QString::SkipEmptyParts);
		QString szChanList;

		for(auto szCur : sl)
		{
			// deals with <flag>[#channel] and [##channel]
			int len = szCur.length();
			int i = 0;
			while(i < len)
			{

				if(IS_CHANNEL_TYPE_FLAG(szCur[i]) && (!IS_USER_MODE_PREFIX(szCur[i])))
					break;
				i++;
			}
			if(i < len)
			{
				if(i > 0)
				{
					len = szCur.length() - i;
					if(szChanList.length() > 0)
						szChanList.append(", ");
					szChanList += szCur.left(i);
					QString szR = szCur.right(len);
					KviQString::appendFormatted(szChanList, "\r!c\r%Q\r", &szR);
				}
				else
				{
					if(szChanList.length() > 0)
						szChanList.append(", ");
					KviQString::appendFormatted(szChanList, "\r!c\r%Q\r", &szCur);
				}
			}
			else
			{
				// we dunno what is this.. just append
				if(szChanList.length() > 0)
					szChanList.append(", ");
				szChanList.append(szCur);
			}
		}

		pOut->output(
		    KVI_OUT_WHOISCHANNELS, __tr2qs("%c\r!n\r%Q\r%c's channels: %Q"), KviControlCodes::Bold,
		    &szNick, KviControlCodes::Bold, &szChanList);
	}
}

void KviIrcServerParser::parseNumericWhoisIdle(KviIrcMessage * msg)
{
	// 317: RPL_WHOISIDLE [I,E,U,D]
	// :prefix 317 <target> <nick> <number> <number> :seconds idle, signon time

	// FIXME: #warning "and NICK LINKS"
	msg->connection()->stateData()->setLastReceivedWhoisReply(kvi_unixTime());

	QString szNick = msg->connection()->decodeText(msg->safeParam(1));

	KviAsyncWhoisInfo * i = msg->connection()->asyncWhoisData()->lookup(szNick);
	if(i)
	{
		i->szIdle = msg->safeParam(2);
		i->szSignon = msg->safeParam(3);
		bool bOk = false;
		i->szSignon.toUInt(&bOk);
		if(!bOk)
			i->szSignon = "";
		return;
	}

	if(!msg->haltOutput())
	{
		KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolWhoisRepliesToActiveWindow) ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console());
		KviCString idle = msg->safeParam(2); // shouldn't be encoded
		KviCString sign = msg->safeParam(3); // shouldn't be encoded

		bool bOk;
		unsigned int uTime = idle.toUInt(&bOk);
		if(!bOk)
		{
			UNRECOGNIZED_MESSAGE(msg, __tr2qs("Received a broken RPL_WHOISIDLE, can't evaluate the idle time"));
			return;
		}
		unsigned int uDays = uTime / 86400;
		uTime = uTime % 86400;
		unsigned int uHours = uTime / 3600;
		uTime = uTime % 3600;
		unsigned int uMins = uTime / 60;
		uTime = uTime % 60;
		pOut->output(
		    KVI_OUT_WHOISIDLE, __tr2qs("%c\r!n\r%Q\r%c's idle time: %ud %uh %um %us"), KviControlCodes::Bold,
		    &szNick, KviControlCodes::Bold, uDays, uHours, uMins, uTime);

		uTime = sign.toUInt(&bOk);
		if(bOk)
		{
			QString szTmp;
			QDateTime date;
			date.setTime_t((time_t)uTime);

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

			pOut->output(
			    KVI_OUT_WHOISIDLE, __tr2qs("%c\r!n\r%Q\r%c's signon time: %Q"), KviControlCodes::Bold,
			    &szNick, KviControlCodes::Bold, &szTmp);
		}
	}
}

void KviIrcServerParser::parseNumericWhoisServer(KviIrcMessage * msg)
{
	// 312: RPL_WHOISSERVER [I,E,U,D] (sent also in response to WHOWAS)
	// :prefix 312 <target> <nick> <server> :<server description / last whowas date>
	msg->connection()->stateData()->setLastReceivedWhoisReply(kvi_unixTime());

	QString szNick = msg->connection()->decodeText(msg->safeParam(1));
	QString szServ = msg->connection()->decodeText(msg->safeParam(2));

	KviIrcUserDataBase * db = msg->connection()->userDataBase();
	KviIrcUserEntry * e = db->find(szNick);
	if(e)
		e->setServer(szServ);
	KviQueryWindow * q = msg->connection()->findQuery(szNick);
	if(q)
		q->updateLabelText();
	KviAsyncWhoisInfo * i = msg->connection()->asyncWhoisData()->lookup(msg->safeParam(1));
	if(i)
	{
		// assigning to i->szServer erases everything that was previously appended in parseNumericWhoisOther
		// this causes the lost of "is registered nick" info on unrealircd servers
		// prepending solves the problem
		if(!(i->szServer.isEmpty()))
			i->szServer.prepend(',');
		i->szServer.prepend(szServ);
		return;
	}

	// FIXME: #warning "AWHOIS HERE.... and NICK LINKS"
	if(!msg->haltOutput())
	{
		KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolWhoisRepliesToActiveWindow) ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console());
		QString szWText = pOut->decodeText(msg->safeTrailing());
		pOut->output(
		    KVI_OUT_WHOISSERVER, __tr2qs("%c\r!n\r%Q\r%c's server: \r!s\r%Q\r - %Q"), KviControlCodes::Bold,
		    &szNick, KviControlCodes::Bold, &szServ, &szWText);
	}
}

void KviIrcServerParser::parseNumericWhoisAuth(KviIrcMessage * msg)
{
	// 330 RPL_WHOISAUTH
	// :prefix RPL_WHOISAUTH <target> <nick> <nick>:is authed as

	msg->connection()->stateData()->setLastReceivedWhoisReply(kvi_unixTime());

	QString szNick = msg->connection()->decodeText(msg->safeParam(1));
	QString szAuth = msg->connection()->decodeText(msg->safeParam(2));

	KviAsyncWhoisInfo * pInfo = msg->connection()->asyncWhoisData()->lookup(szNick);
	if(pInfo)
	{
		pInfo->szAuth = szAuth;
		return;
	}

	if(!msg->haltOutput())
	{
		KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolWhoisRepliesToActiveWindow) ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console());
		pOut->output(
		    KVI_OUT_WHOISOTHER, __tr2qs("%c\r!n\r%Q\r%c is authenticated as %Q"), KviControlCodes::Bold,
		    &szNick, KviControlCodes::Bold, &szAuth);
	}
}

void KviIrcServerParser::parseNumericWhoisRegistered(KviIrcMessage * msg)
{
	// 307 RPL_WHOISREGNICK
	// :prefix RPL_WHOISREGNICK <target> <nick> :is a registered nick

	msg->connection()->stateData()->setLastReceivedWhoisReply(kvi_unixTime());

	QString szNick = msg->connection()->decodeText(msg->safeParam(1));

	if(!msg->haltOutput())
	{
		KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolWhoisRepliesToActiveWindow) ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console());
		pOut->output(
		    KVI_OUT_WHOISOTHER, __tr2qs("%c\r!n\r%Q\r%c is a registered nickname"), KviControlCodes::Bold,
		    &szNick, KviControlCodes::Bold);
	}
}

void KviIrcServerParser::parseNumericWhoisActually(KviIrcMessage * msg)
{
	// 338: RPL_WHOISACTUALLY
	// u2 (irc-hispano)
	// :prefix 338 <target> <nick> <user@host> <ip address> :Actual user@host, Actual IP
	// ratbox (efnet)
	// :prefix 338 <target> <nick> <host/ip address> :actually using host

	// FIXME: #warning "and NICK LINKS"
	msg->connection()->stateData()->setLastReceivedWhoisReply(kvi_unixTime());

	QString szNick = msg->connection()->decodeText(msg->safeParam(1));
	QString szUserHost = msg->connection()->decodeText(msg->safeParam(2));
	QString szIpAddr = msg->connection()->decodeText(msg->safeParam(3));
	QString szOth = msg->connection()->decodeText(msg->safeTrailing());

	KviAsyncWhoisInfo * i = msg->connection()->asyncWhoisData()->lookup(szNick);
	if(i)
	{
		if(!(i->szAdditional.isEmpty()))
			i->szAdditional.append(',');
		i->szAdditional.append(szOth + ": " + szUserHost + " " + szIpAddr);
		return;
	}

	if(!msg->haltOutput())
	{
		KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolWhoisRepliesToActiveWindow) ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console());
		if(szOth.contains(QChar(',')))
		{
			pOut->output(
			    KVI_OUT_WHOISOTHER, __tr2qs("%c\r!n\r%Q\r%c's actual user@host: %Q, actual IP: %Q"), KviControlCodes::Bold,
			    &szNick, KviControlCodes::Bold, &szUserHost, &szIpAddr);
		}
		else
		{
			pOut->output(
			    KVI_OUT_WHOISOTHER, __tr2qs("%c\r!n\r%Q\r%c's actual host: %Q"), KviControlCodes::Bold,
			    &szNick, KviControlCodes::Bold, &szUserHost);
		}
	}
}

void KviIrcServerParser::parseNumericWhoisOther(KviIrcMessage * msg)
{
	// *: RPL_WHOIS* [?]
	// :prefix * <target> <nick> :<description>
	// used for RPL_WHOISCHANOP,RPL_WHOISADMIN,
	// RPL_WHOISSADMIN,RPL_WHOISOPERATOR,RPL_WHOISREGNICK,RPL_WHOISSSL
	// and all the other unrecognized codes that look really like a RPL_WHOIS*

	msg->connection()->stateData()->setLastReceivedWhoisReply(kvi_unixTime());

	QString szNick = msg->connection()->decodeText(msg->safeParam(1));
	QString szOth = msg->connection()->decodeText(msg->safeTrailing());

	KviAsyncWhoisInfo * i = msg->connection()->asyncWhoisData()->lookup(szNick);
	if(i)
	{
		if(!(i->szAdditional.isEmpty()))
			i->szAdditional.append(',');
		i->szAdditional.append(szOth);
		return;
	}

	// FIXME: #warning "NICK LINKS"
	if(!msg->haltOutput())
	{
		KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolWhoisRepliesToActiveWindow) ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console());
		pOut->output(
		    KVI_OUT_WHOISOTHER, __tr2qs("%c\r!n\r%Q\r%c's info: %Q"), KviControlCodes::Bold,
		    &szNick, KviControlCodes::Bold, &szOth);
	}
}

// FIXME: #warning "WHOWAS MISSING"

void KviIrcServerParser::parseNumericEndOfWhois(KviIrcMessage * msg)
{
	// 318: RPL_ENDOFWHOIS [I,E,U,D]
	// :prefix 318 <target> <nick> :End of /WHOIS list

	msg->connection()->stateData()->setLastReceivedWhoisReply(0);

	QString szNick = msg->connection()->decodeText(msg->safeParam(1));

	KviAsyncWhoisInfo * i = msg->connection()->asyncWhoisData()->lookup(szNick);
	if(i)
	{
		if(!g_pApp->windowExists(i->pWindow))
			i->pWindow = msg->console();

		// that's the new KVS engine!
		KviKvsVariantList vl;
		vl.setAutoDelete(true);
		vl.append(new KviKvsVariant(i->szNick));
		vl.append(new KviKvsVariant(i->szUser));
		vl.append(new KviKvsVariant(i->szHost));
		vl.append(new KviKvsVariant(i->szReal));
		vl.append(new KviKvsVariant(i->szServer));
		vl.append(new KviKvsVariant(i->szIdle));
		vl.append(new KviKvsVariant(i->szSignon));
		vl.append(new KviKvsVariant(i->szChannels));
		vl.append(new KviKvsVariant(QString(msg->safePrefix())));
		vl.append(new KviKvsVariant(i->szAway)); // szSpecial is renamed szAway
		vl.append(new KviKvsVariant(*(i->pMagic)));
		vl.append(new KviKvsVariant(i->szAuth));
		vl.append(new KviKvsVariant(i->szAdditional));
		i->pCallback->run(i->pWindow, &vl, nullptr, KviKvsScript::PreserveParams);
		msg->connection()->asyncWhoisData()->remove(i);
		return;
	}

	// FIXME: #warning "NICK LINKS"
	if(!msg->haltOutput())
	{
		KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolWhoisRepliesToActiveWindow) ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console());
		QString pref = msg->connection()->decodeText(msg->safePrefix());
		pOut->output(
		    KVI_OUT_WHOISOTHER, __tr2qs("%c\r!n\r%Q\r%c WHOIS info from \r!s\r%Q\r"), KviControlCodes::Bold,
		    &szNick, KviControlCodes::Bold, &pref);
	}
}

void KviIrcServerParser::parseNumericEndOfWhowas(KviIrcMessage * msg)
{
	// 369: RPL_ENDOFWHOWAS [I,E,U,D]
	// :prefix 369 <target> <nick> :End of /WHOWAS list

	msg->connection()->stateData()->setLastReceivedWhoisReply(0);

	if(!msg->haltOutput())
	{
		QString szNick = msg->connection()->decodeText(msg->safeParam(1));
		KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolWhoisRepliesToActiveWindow) ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console());
		QString pref = msg->connection()->decodeText(msg->safePrefix());
		pOut->output(
		    KVI_OUT_WHOISOTHER, __tr2qs("%c\r!n\r%Q\r%c WHOWAS info from \r!s\r%Q\r"), KviControlCodes::Bold,
		    &szNick, KviControlCodes::Bold, &pref);
	}
}

void KviIrcServerParser::parseNumericNoSuchNick(KviIrcMessage * msg)
{
	// 401: ERR_NOSUCHNICK [I,E,U,D]
	// 406: ERR_WASNOSUCHNICK [I,E,U,D]
	// :prefix 401 <target> <nick> :No such nick/channel
	// :prefix 406 <target> <nick> :There was no such nickname
	QString szNick = msg->connection()->decodeText(msg->safeParam(1));

	if(msg->numeric() == ERR_NOSUCHNICK)
	{
		KviAsyncWhoisInfo * i = msg->connection()->asyncWhoisData()->lookup(szNick);
		if(i)
		{
			if(!g_pApp->windowExists(i->pWindow))
				i->pWindow = msg->console();
			// that's the new KVS engine!
			KviKvsVariantList vl;
			vl.setAutoDelete(true);
			vl.append(new KviKvsVariant(i->szNick));
			for(unsigned int ii = 0; ii < 9; ii++)
				vl.append(new KviKvsVariant());
			vl.append(new KviKvsVariant(*(i->pMagic)));
			i->pCallback->run(i->pWindow, &vl, nullptr, KviKvsScript::PreserveParams);
			msg->connection()->asyncWhoisData()->remove(i);
			return;
		}
	}
	// FIXME: #warning "KVI_OUT_NOSUCHNICKCHANNEL ?"
	// FIXME: #warning "QUERIES SHOULD REPORT NO TARGET HERE! (?)"
	if(!msg->haltOutput())
	{
		KviWindow * pOut = static_cast<KviWindow *>(msg->connection()->findQuery(szNick));
		if(!pOut)
		{
			pOut = static_cast<KviWindow *>(msg->console()->activeWindow());
		}
		//} else {
		//	(static_cast<KviQueryWindow *>(pOut))->removeTarget(msg->safeParam(1));
		//}
		QString szWText = pOut->decodeText(msg->safeTrailing());
		pOut->output(KVI_OUT_NICKNAMEPROBLEM, "\r!n\r%Q\r: %Q",
		    &szNick, &szWText);
	}
}

void KviIrcServerParser::parseNumericChanUrl(KviIrcMessage * msg)
{
	// 328: RPL_CHANURL
	// :prefix 328 target <channel> :<url>
	if(msg->haltOutput()) return;

	QString szChan = msg->connection()->decodeText(msg->safeParam(1));
	KviChannelWindow * chan = msg->connection()->findChannel(szChan);

	QString szUrl;

	if(chan)
	{
		szUrl = chan->decodeText(msg->safeTrailing());
		chan->output(KVI_OUT_CHANURL, __tr2qs("This channel's website is: %Q"), &szUrl);
	}
	else
	{
		szUrl = msg->console()->decodeText(msg->safeTrailing());
		KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolServerRepliesToActiveWindow) ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console());
		pOut->output(KVI_OUT_CHANURL, __tr2qs("The website for \r!c\r%Q\r is: %Q"), &szChan, &szUrl);
	}
}

void KviIrcServerParser::parseNumericCreationTime(KviIrcMessage * msg)
{
	// 329: RPL_CREATIONTIME
	// :prefix 329 <target> <channel> <creation_time>
	QString szChan = msg->connection()->decodeText(msg->safeParam(1));
	KviChannelWindow * chan = msg->connection()->findChannel(szChan);
	KviCString tmstr = msg->safeParam(2);
	QDateTime date;
	date.setTime_t((time_t)tmstr.toUInt());

	if(!tmstr.isUnsignedNum())
	{
		UNRECOGNIZED_MESSAGE(msg, __tr2qs("Can't evaluate creation time"));
		return;
	}

	QString szDate;
	switch(KVI_OPTION_UINT(KviOption_uintOutputDatetimeFormat))
	{
		case 0:
			// this is the equivalent to an empty date.toString() call, but it's needed
			// to ensure qt4 will use the default() locale and not the system() one
			szDate = QLocale().toString(date, "ddd MMM d hh:mm:ss yyyy");
			break;
		case 1:
			szDate = date.toString(Qt::ISODate);
			break;
		case 2:
			szDate = date.toString(Qt::SystemLocaleShortDate);
			break;
	}

	if(chan)
	{
		if(!msg->haltOutput())
		{
			chan->output(KVI_OUT_CREATIONTIME, __tr2qs("Channel was created at %Q"), &szDate);
		}
	}
	else
	{
		KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolServerRepliesToActiveWindow) ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console());
		pOut->output(KVI_OUT_CREATIONTIME, __tr2qs("Channel \r!c\r%Q\r was created at %Q"),
		    &szChan, &szDate);
	}
}

void KviIrcServerParser::parseNumericIsOn(KviIrcMessage * msg)
{
	// 303: RPL_ISON
	// :prefix 303 <target> :<ison replies>
	if(msg->connection()->notifyListManager())
	{
		if(msg->connection()->notifyListManager()->handleIsOn(msg))
			return;
	}
	// not handled...output it

	if(!msg->haltOutput())
	{
		KviWindow * pOut = msg->console()->activeWindow();
		QString szPrefix = msg->connection()->decodeText(msg->safePrefix());
		QString szUser = msg->connection()->decodeText(msg->safeTrailing());
		QString szOutput = szUser.isEmpty() ? "That user is not online" : szUser + "is online";
		pOut->output(KVI_OUT_HELP, szOutput);
	}
}

void KviIrcServerParser::parseNumericUserhost(KviIrcMessage * msg)
{
	// 302: RPL_USERHOST
	// :prefix 302 <target> :<userhost replies>
	if(msg->connection()->notifyListManager())
	{
		if(msg->connection()->notifyListManager()->handleUserhost(msg))
			return;
	}
	// not handled...output it
	if(!msg->haltOutput())
	{
		KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolServerRepliesToActiveWindow) ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console());
		QString szUser = msg->connection()->decodeText(msg->safeTrailing());
		pOut->output(KVI_OUT_WHOISUSER, __tr2qs("USERHOST info: %Q"), &szUser);
	}
}

void KviIrcServerParser::parseNumericListStart(KviIrcMessage * msg)
{
	// 321: RPL_LISTSTART [I,E,U,D]
	// :prefix 321 <target> :Channel users name
	if(msg->haltOutput())
		return; // stopped by raw

	if(!(msg->console()->context()->listWindow()))
	{
		// attempt to load the module...
		msg->console()->context()->createListWindow();
	}

	if(msg->console()->context()->listWindow())
	{
		// module loaded
		msg->console()->context()->listWindow()->control(EXTERNAL_SERVER_DATA_PARSER_CONTROL_STARTOFDATA);
	}
	else
	{
		msg->console()->output(KVI_OUT_LIST, __tr2qs("Channel list begin: channel, users, topic"));
	}
}

void KviIrcServerParser::parseNumericList(KviIrcMessage * msg)
{
	// 322: RPL_LIST [I,E,U,D]
	// :prefix 364 <target> <channel> <users> :<topic>
	if(msg->haltOutput())
		return; // stopped by raw

	if(!(msg->console()->context()->listWindow()))
	{
		// attempt to load the module...
		msg->console()->context()->createListWindow();
		if(msg->console()->context()->listWindow())
		{
			msg->console()->context()->listWindow()->control(EXTERNAL_SERVER_DATA_PARSER_CONTROL_STARTOFDATA);
		}
	}

	if(msg->console()->context()->listWindow())
	{
		// module loaded
		msg->console()->context()->listWindow()->processData(msg);
	}
	else
	{
		// Oops...can't load the module...
		QString szList = msg->connection()->decodeText(msg->allParams());
		msg->console()->output(KVI_OUT_LIST, __tr2qs("List: %Q"), &szList);
	}
}

void KviIrcServerParser::parseNumericListEnd(KviIrcMessage * msg)
{
	// 323: RPL_LISTEND [I,E,U,D]
	// :prefix 323 <target> :End of /LIST
	if(msg->haltOutput())
		return; // stopped by raw

	if(msg->console()->context()->listWindow())
	{
		msg->console()->context()->listWindow()->control(EXTERNAL_SERVER_DATA_PARSER_CONTROL_ENDOFDATA);
	}
	else
	{
		msg->console()->output(KVI_OUT_LIST, __tr2qs("End of list"));
	}
}

void KviIrcServerParser::parseNumericLinks(KviIrcMessage * msg)
{
	// 364: RPL_LINKS [I,E,U,D]
	// :prefix 364 <target> <host> <parent> :<hops> <description>
	if(!(msg->console()->context()->linksWindow()))
	{
		// attempt to load the module...
		msg->console()->context()->createLinksWindow();
	}

	if(msg->console()->context()->linksWindow())
	{
		// module loaded
		msg->console()->context()->linksWindow()->processData(msg);
	}
	else
	{
		// Oops...can't load the module... or the event halted the window creation
		if(!msg->haltOutput())
		{
			QString szList = msg->connection()->decodeText(msg->allParams());
			msg->console()->output(KVI_OUT_LINKS, __tr2qs("Link: %Q"), &szList);
		}
	}
}

void KviIrcServerParser::parseNumericEndOfLinks(KviIrcMessage * msg)
{
	// 365: RPL_ENDOFLINKS [I,E,U,D]
	// :prefix 365 <target> :End of /LINKS
	if(msg->console()->context()->linksWindow())
	{
		msg->console()->context()->linksWindow()->control(EXTERNAL_SERVER_DATA_PARSER_CONTROL_ENDOFDATA);
	}
	else
	{
		if(!msg->haltOutput())
		{
			msg->console()->output(KVI_OUT_LINKS, __tr2qs("End of links"));
		}
	}
}

void KviIrcServerParser::parseNumericBackFromAway(KviIrcMessage * msg)
{
	// 305: RPL_UNAWAY [I,E,U,D]
	// :prefix 305 <target> :You are no longer away
	bool bWasAway = msg->connection()->userInfo()->isAway();
	QString szNickBeforeAway;
	QString szWText = msg->connection()->decodeText(msg->safeTrailing());

	if(bWasAway)
		szNickBeforeAway = msg->connection()->userInfo()->nickNameBeforeAway();
	msg->connection()->changeAwayState(false);

	// trigger the event
	QString tmp = QString("%1").arg(bWasAway ? static_cast<unsigned int>(msg->connection()->userInfo()->awayTime()) : 0);
	if(KVS_TRIGGER_EVENT_2_HALTED(KviEvent_OnMeBack, msg->console(), tmp, szWText))
		msg->setHaltOutput();
	if(!msg->haltOutput())
	{
		KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolServerRepliesToActiveWindow) ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console());

		if(bWasAway)
		{
			int uTimeDiff = kvi_unixTime() - msg->connection()->userInfo()->awayTime();
			pOut->output(KVI_OUT_AWAY, __tr2qs("[Leaving away status after %ud %uh %um %us]: %Q"),
			    uTimeDiff / 86400, (uTimeDiff % 86400) / 3600, (uTimeDiff % 3600) / 60, uTimeDiff % 60,
			    &szWText);
		}
		else
		{
			pOut->output(KVI_OUT_AWAY, __tr2qs("[Leaving away status]: %Q"), &szWText);
		}
	}

	if(KVI_OPTION_BOOL(KviOption_boolChangeNickAway) && bWasAway && (!(szNickBeforeAway.isEmpty())))
	{
		if(_OUTPUT_PARANOIC)
			msg->console()->output(KVI_OUT_AWAY, __tr2qs("Restoring pre-away nickname (%Q)"), &szNickBeforeAway);
		QByteArray szDat = msg->connection()->encodeText(szNickBeforeAway);
		msg->connection()->sendFmtData("NICK %s", szDat.data());
	}
}

void KviIrcServerParser::parseNumericAway(KviIrcMessage * msg)
{
	// 306: RPL_NOWAWAY [I,E,U,D]
	// :prefix 305 <target> :You're away man
	msg->connection()->changeAwayState(true);
	QString szWText = msg->connection()->decodeText(msg->safeTrailing());

	if(KVS_TRIGGER_EVENT_1_HALTED(KviEvent_OnMeAway, msg->console(), szWText))
		msg->setHaltOutput();

	if(!msg->haltOutput())
	{
		KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolServerRepliesToActiveWindow) ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console());
		pOut->output(KVI_OUT_AWAY, __tr2qs("[Entering away status]: %Q"), &szWText);
	}

	if(KVI_OPTION_BOOL(KviOption_boolChangeNickAway))
	{
		QString nick = msg->connection()->decodeText(msg->safeParam(0));
		QString szNewNick;
		if(KVI_OPTION_BOOL(KviOption_boolAutoGeneratedAwayNick))
		{
			if(nick.length() > 5)
				szNewNick = nick.left(5);
			else
				szNewNick = nick;
			szNewNick.append("AWAY");
		}
		else
		{
			szNewNick = KVI_OPTION_STRING(KviOption_stringCustomAwayNick);
			szNewNick.replace("%nick%", nick);
		}

		if(_OUTPUT_PARANOIC)
			msg->console()->output(KVI_OUT_AWAY, __tr2qs("Setting away nickname (%Q)"), &szNewNick);
		QByteArray dat = msg->connection()->encodeText(szNewNick);
		msg->connection()->sendFmtData("NICK %s", dat.data());
	}
}

void KviIrcServerParser::parseNumericUsersDontMatch(KviIrcMessage * msg)
{
	// 502: ERR_USERSDONTMATCH
	// :prefix 502 <target> :Can't change mode for other users
	if(!msg->haltOutput())
	{
		KviWindow * pOut = msg->console()->activeWindow();
		QString szMsgText = msg->connection()->decodeText(msg->safeTrailing());
		pOut->output(KVI_OUT_GENERICERROR, szMsgText);
	}
}

void KviIrcServerParser::parseNumericWatch(KviIrcMessage * msg)
{
	// 600: RPL_LOGON
	// :prefix 600 <target> <nick> <user> <host> <logintime> :logged online
	// 601: RPL_LOGON
	// :prefix 601 <target> <nick> <user> <host> <logintime> :logged offline
	// 602: RPL_WATCHOFF
	// :prefix 602 <target> <nick> <user> <host> <logintime> :stopped watching
	// 604: PRL_NOWON
	// :prefix 604 <target> <nick> <user> <host> <logintime> :is online
	// 605: PRL_NOWOFF
	// :prefix 605 <target> <nick> <user> <host> 0 :is offline

	if(msg->connection()->notifyListManager())
	{
		if(msg->connection()->notifyListManager()->handleWatchReply(msg))
			return;
	}
	// not handled...output it

	// FIXME: #warning "OUTPUT IT! (In a suitable way) (And handle 602, 603, 606 and 607 gracefully)"
	if(!msg->haltOutput())
	{
		KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolServerRepliesToActiveWindow) ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console());
		pOut->output(KVI_OUT_UNHANDLED,
		    "[%s][%s] %s", msg->prefix(), msg->command(), msg->allParams());
	}
}

void KviIrcServerParser::parseNumericStats(KviIrcMessage * msg)
{
	if(!msg->haltOutput())
	{
		KviWindow * pOut = static_cast<KviWindow *>(msg->console());
		if(msg->paramCount() > 2)
		{
			KviCString szParms;
			for(int i = 1; i < msg->paramCount(); ++i)
			{
				if(szParms.hasData())
					szParms.append(' ');
				szParms.append(msg->params()[i]);
			}
			pOut->outputNoFmt(KVI_OUT_STATS, msg->connection()->decodeText(szParms).toUtf8().data());
		}
		else
		{
			pOut->outputNoFmt(KVI_OUT_STATS, msg->connection()->decodeText(msg->safeTrailing()).toUtf8().data());
		}
	}
}

void KviIrcServerParser::parseNumericServerAdminInfoTitle(KviIrcMessage * msg)
{
	//RPL_ADMINME          256
	if(!msg->haltOutput())
	{
		KviWindow * pOut = static_cast<KviWindow *>(msg->console());
		pOut->outputNoFmt(KVI_OUT_SERVERINFO, msg->connection()->decodeText(msg->safeTrailing()).toUtf8().data());
	}
}
void KviIrcServerParser::parseNumericServerAdminInfoServerName(KviIrcMessage * msg)
{
	//RPL_ADMINLOC1        257
	if(!msg->haltOutput())
	{
		KviWindow * pOut = static_cast<KviWindow *>(msg->console());
		QString szInfo = msg->connection()->decodeText(msg->safeTrailing());
		pOut->output(KVI_OUT_SERVERINFO, __tr2qs("%c\r!s\r%s\r%c's server info: %s"), KviControlCodes::Bold, msg->prefix(), KviControlCodes::Bold, szInfo.toUtf8().data());
	}
}

void KviIrcServerParser::parseNumericServerAdminInfoAdminName(KviIrcMessage * msg)
{
	//RPL_ADMINLOC2        258
	if(!msg->haltOutput())
	{
		KviWindow * pOut = static_cast<KviWindow *>(msg->console());
		QString szInfo = msg->connection()->decodeText(msg->safeTrailing());
		pOut->output(KVI_OUT_SERVERINFO, __tr2qs("%c\r!s\r%s\r%c's administrator is %s"), KviControlCodes::Bold, msg->prefix(), KviControlCodes::Bold, szInfo.toUtf8().data());
	}
}

void KviIrcServerParser::parseNumericServerAdminInfoAdminContact(KviIrcMessage * msg)
{
	//RPL_ADMINEMAIL       259
	if(!msg->haltOutput())
	{
		KviWindow * pOut = static_cast<KviWindow *>(msg->console());
		QString szInfo = msg->connection()->decodeText(msg->safeTrailing());
		pOut->output(KVI_OUT_SERVERINFO, __tr2qs("%c\r!s\r%s\r%c's contact address is %s"), KviControlCodes::Bold, msg->prefix(), KviControlCodes::Bold, szInfo.toUtf8().data());
	}
}

void KviIrcServerParser::parseNumericTryAgain(KviIrcMessage * msg)
{
	// RPL_TRYAGAIN        263
	// :prefix 263 <target> <command> :This command could not be completed because it has been used recently, and is rate-limited.
	if(!msg->haltOutput())
	{
		QString szCmd = msg->connection()->decodeText(msg->safeParam(1));
		QString szComment = msg->connection()->decodeText(msg->safeTrailing());
		KviWindow * pOut = msg->console()->activeWindow();

		// Bank on the IRCd providing a useful explanation.
		pOut->output(KVI_OUT_GENERICERROR, __tr2qs("Unable to use command %Q. %Q"),
		    &szCmd, &szComment);
	}
}

void KviIrcServerParser::parseNumericCommandSyntax(KviIrcMessage * msg)
{
	//RPL_COMMANDSYNTAX    334
	if(!msg->haltOutput())
	{
		KviWindow * pOut = static_cast<KviWindow *>(msg->console());
		pOut->outputNoFmt(KVI_OUT_STATS, msg->connection()->decodeText(msg->safeTrailing()));
	}
}

void KviIrcServerParser::parseNumericInviting(KviIrcMessage * msg)
{
	//RPL_INVITING         341
	if(!msg->haltOutput())
	{
		QString szWho = msg->connection()->decodeText(msg->safeParam(0));
		QString szTarget = msg->connection()->decodeText(msg->safeParam(1));
		QString szChan = msg->connection()->decodeText(msg->safeParam(2));
		KviChannelWindow * chan = msg->connection()->findChannel(szChan);
		if(chan)
		{
			chan->output(KVI_OUT_INVITE, __tr2qs("\r!n\r%Q\r invited %Q into channel %Q"), &szWho, &szTarget, &szChan);
		}
		else
		{
			KviWindow * pOut = static_cast<KviWindow *>(msg->console());
			pOut->output(KVI_OUT_INVITE, __tr2qs("\r!n\r%Q\r invited %Q into channel %Q"), &szWho, &szTarget, &szChan);
		}
	}
}

void KviIrcServerParser::parseNumericInvited(KviIrcMessage * msg)
{
	//RPL_INVITED          345
	if(!msg->haltOutput())
	{
		QString szWho = msg->connection()->decodeText(msg->safeParam(2));
		QString szTarget = msg->connection()->decodeText(msg->safeParam(1));
		QString szChan = msg->connection()->decodeText(msg->safeParam(0));
		KviChannelWindow * chan = msg->connection()->findChannel(szChan);
		if(chan)
		{
			chan->output(KVI_OUT_INVITE, __tr2qs("\r!n\r%Q\r invited %Q into channel %Q"), &szWho, &szTarget, &szChan);
		}
		else
		{
			KviWindow * pOut = static_cast<KviWindow *>(msg->console());
			pOut->output(KVI_OUT_INVITE, __tr2qs("\r!n\r%Q\r invited %Q into channel %Q"), &szWho, &szTarget, &szChan);
		}
	}
}

void KviIrcServerParser::parseNumeric344(KviIrcMessage * msg)
{
	// Determine whether this is inteded for RPL_REOPLIST or
	// RPL_QUIETLIST
	KviIrcConnectionServerInfo * pServerInfo = msg->connection()->serverInfo();

	QString version = pServerInfo->software();
	if(version == "Hybrid+Oftc")
		parseNumericOftcQuietList(msg);
	else
		parseNumericReopList(msg);
}

void KviIrcServerParser::parseNumeric345(KviIrcMessage * msg)
{
	// Determine whether this is inteded for RPL_ENDOFREOPLIST,
	// RPL_QUIETLISTEND, or RPL_INVITED
	KviIrcConnectionServerInfo * pServerInfo = msg->connection()->serverInfo();

	QString version = pServerInfo->software();
	if(version == "Snircd" || version == "Ircu" || version == "Ircu+Darenet")
		parseNumericInvited(msg);
	else if(version == "Hybrid+Oftc")
		parseNumericOftcEndOfQuietList(msg);
	else
		parseNumericEndOfReopList(msg);
}

void KviIrcServerParser::parseNumeric480(KviIrcMessage * msg)
{
	// Determine if this is a generic "cannot join" string or
	// if it is an SSL requirement. Note that hyrbid forks do
	// not provide detailed information as they just print
	// "Cannot join channel (+S)". This gives more of an explanation
	// for non-versed IRC users.
	KviIrcConnectionServerInfo * pServerInfo = msg->connection()->serverInfo();

	QString version = pServerInfo->software();
	if(version == "Plexus" || version == "Hybrid+Oftc")
		parseNumericNeedSSL(msg);
	else
		parseNumericCantJoinChannel(msg);
}

void KviIrcServerParser::parseNumeric728(KviIrcMessage * msg)
{
	// Determine whether this is a freenode style quiet or an
	// oftc style quiet
	KviIrcConnectionServerInfo * pServerInfo = msg->connection()->serverInfo();

	QString version = pServerInfo->software();
	if(version == "Ircu+Darenet")
		parseNumericOftcQuietList(msg);
	else
		parseNumericQuietList(msg);
}

void KviIrcServerParser::parseNumeric729(KviIrcMessage * msg)
{
	// Determine whether this is a freenode style End of Quiet list
	// or an oftc style End Of Quiet list
	KviIrcConnectionServerInfo * pServerInfo = msg->connection()->serverInfo();

	QString version = pServerInfo->software();
	if(version == "Ircu+Darenet")
		parseNumericOftcEndOfQuietList(msg);
	else
		parseNumericEndOfQuietList(msg);
}

void KviIrcServerParser::parseNumeric742(KviIrcMessage * msg)
{
	// ERR_MLOCKRESTRICTED 742
	// :<prefix> 742 <target> <channel> <mode> <mlocked modes> :MODE cannot be set due to channel having an active MLOCK restriction policy
	QString szChan = msg->connection()->decodeText(msg->safeParam(1));
	QString mode = msg->connection()->decodeText(msg->safeParam(2));
	QString lock = msg->connection()->decodeText(msg->safeParam(3));
	KviChannelWindow * chan = msg->connection()->findChannel(szChan);
	if(chan)
	{
		chan->output(KVI_OUT_GENERICERROR, __tr2qs("Can't set mode %Q on %Q due to it having an active MLOCK policy, including modes %Q."),
		    &mode, &szChan, &lock);
	}
	else
	{
		KviWindow * pOut = static_cast<KviWindow *>(msg->console());
		pOut->output(KVI_OUT_GENERICERROR, __tr2qs("Can't set mode %Q on %Q due to it having an active MLOCK policy, including modes %Q."),
		    &mode, &szChan, &lock);
	}
}

void KviIrcServerParser::parseNumericInfo(KviIrcMessage * msg)
{
	//RPL_INFO             371
	if(!msg->haltOutput())
	{
		KviWindow * pOut = static_cast<KviWindow *>(msg->console());
		QString szInfo = msg->connection()->decodeText(msg->safeTrailing());
		pOut->outputNoFmt(KVI_OUT_SERVERINFO, szInfo);
	}
}

void KviIrcServerParser::parseNumericInfoStart(KviIrcMessage * msg)
{
	//RPL_INFOSTART        373
	if(!msg->haltOutput())
	{
		KviWindow * pOut = static_cast<KviWindow *>(msg->console());
		pOut->output(KVI_OUT_SERVERINFO, __tr2qs("%c\r!s\r%s\r%c's information:"), KviControlCodes::Bold, msg->prefix(), KviControlCodes::Bold);
	}
}

void KviIrcServerParser::parseNumericInfoEnd(KviIrcMessage * msg)
{
	//RPL_ENDOFINFO        374
	if(!msg->haltOutput())
	{
		KviWindow * pOut = static_cast<KviWindow *>(msg->console());
		pOut->output(KVI_OUT_SERVERINFO, __tr2qs("End of %c\r!s\r%s\r%c's information"), KviControlCodes::Bold, msg->prefix(), KviControlCodes::Bold);
	}
}

void KviIrcServerParser::parseNumericTime(KviIrcMessage * msg)
{
	//RPL_TIME             391
	if(!msg->haltOutput())
	{
		KviWindow * pOut = static_cast<KviWindow *>(msg->console());
		QString szInfo = msg->connection()->decodeText(msg->safeTrailing());
		pOut->output(KVI_OUT_SERVERINFO, __tr2qs("%c\r!s\r%s\r%c's time is %Q"), KviControlCodes::Bold, msg->prefix(), KviControlCodes::Bold, &szInfo);
	}
}

void KviIrcServerParser::parseNumericHiddenHost(KviIrcMessage * msg)
{
	//RPL_HOSTHIDDEN       396
	//<prefix> 396 target <[user@]host> :<message>
	QString pref = msg->connection()->decodeText(msg->safePrefix());
	QString szHost = msg->connection()->decodeText(msg->safeParam(1));
	QString szMsgText = msg->connection()->decodeText(msg->safeTrailing());

	if(KVS_TRIGGER_EVENT_2_HALTED(KviEvent_OnMeHostChange, msg->console(), pref, szHost))
		msg->setHaltOutput();

	if(!msg->haltOutput())
	{
		KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolServerNoticesToActiveWindow) ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console());
		pOut->output(KVI_OUT_SERVERNOTICE, msg->serverTime(), "[\r!s\r%Q\r]: %Q %Q", &pref, &szHost, &szMsgText);
	}
}

void KviIrcServerParser::parseNumericNoSuchServer(KviIrcMessage * msg)
{
	//ERR_NOSUCHSERVER     402
	//this can be an "awhois -i" reply for a nickname that's not connected
	QString szNick = msg->connection()->decodeText(msg->safeParam(1));

	KviAsyncWhoisInfo * i = msg->connection()->asyncWhoisData()->lookup(szNick);
	if(i)
	{
		if(!g_pApp->windowExists(i->pWindow))
			i->pWindow = msg->console();
		// that's the new KVS engine!
		KviKvsVariantList vl;
		vl.setAutoDelete(true);
		vl.append(new KviKvsVariant(i->szNick));
		for(unsigned int ii = 0; ii < 9; ii++)
			vl.append(new KviKvsVariant());
		vl.append(new KviKvsVariant(*(i->pMagic)));
		i->pCallback->run(i->pWindow, &vl, nullptr, KviKvsScript::PreserveParams);
		msg->connection()->asyncWhoisData()->remove(i);
		return;
	}

	if(!msg->haltOutput())
	{
		KviWindow * pOut = static_cast<KviWindow *>(msg->console());
		QString szWhat = msg->connection()->decodeText(msg->safeParam(1));
		pOut->output(KVI_OUT_GENERICERROR, __tr2qs("%Q: no such server"), &szWhat);
	}
}

void KviIrcServerParser::parseNumericNoSuchChannel(KviIrcMessage * msg)
{
	// ERR_NOSUCHCHANNEL    403
	if(!msg->haltOutput())
	{
		KviWindow * pOut = static_cast<KviWindow *>(msg->console());
		QString szWhat = msg->connection()->decodeText(msg->safeParam(1));
		pOut->output(KVI_OUT_GENERICERROR, __tr2qs("%Q: no such channel"), &szWhat);
	}
}

void KviIrcServerParser::parseNumericCannotSendColor(KviIrcMessage * msg)
{
	// ERR_NOCOLORSONCHAN   408
	if(!msg->haltOutput())
	{
		QString szChan = msg->connection()->decodeText(msg->safeParam(1));
		QString szInfo = msg->connection()->decodeText(msg->safeTrailing());
		KviChannelWindow * chan = msg->connection()->findChannel(szChan);
		if(chan)
		{
			chan->output(KVI_OUT_GENERICERROR, __tr2qs("Can't send to channel: %Q"), &szInfo);
		}
		else
		{
			KviWindow * pOut = static_cast<KviWindow *>(msg->console());
			pOut->output(KVI_OUT_GENERICERROR, __tr2qs("Can't send text to channel %Q: %Q"), &szChan, &szInfo);
		}
	}
}

void KviIrcServerParser::parseNumericCannotSend(KviIrcMessage * msg)
{
	// ERR_CANNOTSENDTOCHAN 404
	if(!msg->haltOutput())
	{
		QString szChan = msg->connection()->decodeText(msg->safeParam(1));
		QString szInfo = msg->connection()->decodeText(msg->safeTrailing());
		KviChannelWindow * chan = msg->connection()->findChannel(szChan);
		if(chan)
		{
			chan->output(KVI_OUT_GENERICERROR, __tr2qs("Can't send to channel"));
		}
		else
		{
			KviWindow * pOut = static_cast<KviWindow *>(msg->console());
			pOut->output(KVI_OUT_GENERICERROR, __tr2qs("Can't send text to channel %Q"), &szChan);
		}
	}
}

void KviIrcServerParser::parseNumericCodePageSet(KviIrcMessage * msg)
{
	// a nice extension for irc.wenet.ru
	// 700: RPL_CODEPAGESET
	// :prefix 700 target <encoding> :is now your translation scheme

	QString encoding = msg->connection()->decodeText(msg->safeParam(1));
	if(msg->connection()->serverInfo()->supportsCodePages())
	{
		if(encoding == "NONE")
			encoding = "KOI8-R"; //RusNet default codepage
		msg->console()->output(KVI_OUT_TEXTENCODING, __tr2qs("Your encoding is now %Q"), &encoding);
		msg->console()->setTextEncoding(encoding);
		msg->connection()->setEncoding(encoding);
	}
	else
	{
		QString szMe = msg->connection()->decodeText(msg->safeParam(0));
		if((szMe == msg->connection()->currentNickName() || szMe == "*") //fix for pre-login codepage message
		    && KviLocale::instance()->codecForName(encoding.toUtf8().data()))
		{
			msg->console()->output(KVI_OUT_TEXTENCODING, __tr2qs("Your encoding is now %Q"), &encoding);
			msg->console()->setTextEncoding(encoding);
			msg->connection()->setEncoding(encoding);
		}
		else if(!msg->haltOutput()) // simply unhandled
		{
			QString szWText = msg->connection()->decodeText(msg->allParams());
			msg->connection()->console()->output(KVI_OUT_UNHANDLED,
			    "[%s][%s] %Q", msg->prefix(), msg->command(), &szWText);
		}
	}
}

void KviIrcServerParser::parseNumericCodePageScheme(KviIrcMessage * msg)
{
	// a nice extension for irc.wenet.ru
	// 703: RPL_WHOISSCHEME
	// :prefix 703 <mynick> <nick> <encoding> :translation scheme

	msg->connection()->stateData()->setLastReceivedWhoisReply(kvi_unixTime());

	if(msg->connection()->serverInfo()->supportsCodePages())
	{
		QString szNick = msg->connection()->decodeText(msg->safeParam(1));
		QString szCodepage = msg->connection()->decodeText(msg->safeParam(2));

		if(!msg->haltOutput())
		{
			KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolWhoisRepliesToActiveWindow) ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console());
			QString szWText = pOut->decodeText(msg->safeTrailing());
			pOut->output(
			    KVI_OUT_WHOISOTHER, __tr2qs("%c\r!n\r%Q\r%c's codepage is %Q: %Q"), KviControlCodes::Bold,
			    &szNick, KviControlCodes::Bold, &szCodepage, &szWText);
		}
	}
	else
	{
		// simply unhandled
		if(!msg->haltOutput())
		{
			QString szWText = msg->connection()->decodeText(msg->allParams());
			msg->connection()->console()->output(KVI_OUT_UNHANDLED,
			    "[%s][%s] %Q", msg->prefix(), msg->command(), &szWText);
		}
	}
}

void KviIrcServerParser::parseNumericUserMode(KviIrcMessage * msg)
{
	// 321: RPL_UMODEIS [I,E,U,D]
	// :prefix 221 <target> <modeflags>
	parseUserMode(msg, msg->safeParam(1));

	if(!msg->haltOutput())
	{
		KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolServerRepliesToActiveWindow) ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console());
		pOut->output(KVI_OUT_MODE, __tr2qs("Your user mode is %s"), msg->safeParam(1));
	}
}

void KviIrcServerParser::parseNumericEndOfStats(KviIrcMessage * msg)
{
	// 219: RPL_ENDOFSTATS [I,E,U,D]
	if(!msg->haltOutput())
	{
		KviWindow * pOut = static_cast<KviWindow *>(msg->console());
		QString szText = msg->connection()->decodeText(msg->safeTrailing());
		pOut->outputNoFmt(KVI_OUT_STATS, szText);
	}
}

void KviIrcServerParser::parseNumericYoureOper(KviIrcMessage * msg)
{
	// 381: RPL_YOUREOPER
	// :prefix 381 <target> :You are now an IRC Operator
	if(!msg->haltOutput())
	{
		KviWindow * pOut = static_cast<KviWindow *>(msg->console());
		QString szPrefix = msg->connection()->decodeText(msg->safePrefix());
		QString szText = msg->connection()->decodeText(msg->safeTrailing());
		pOut->output(KVI_OUT_HELP, "%Q on server %Q", &szText, &szPrefix);
	}
}

void KviIrcServerParser::parseNumericNotEnoughParams(KviIrcMessage * msg)
{
	// 461: ERR_NEEDMOREPARAMS
	// :prefix 461 <target> <param> :Not enough parameters
	if(!msg->haltOutput())
	{
		KviWindow * pOut = msg->console()->activeWindow();
		QString szParam = msg->connection()->decodeText(msg->safeParam(1));
		pOut->output(KVI_OUT_GENERICERROR, __tr2qs("%Q requires more parameters"), &szParam);
	}
}

void KviIrcServerParser::parseNumericAlreadyRegistered(KviIrcMessage * msg)
{
	// 462: ERR_ALREADYREGISTERED
	// :prefix 462 <target> :You may not reregister
	if(!msg->haltOutput())
	{
		KviWindow * pOut = static_cast<KviWindow *>(msg->console());
		QString szText = msg->connection()->decodeText(msg->safeTrailing());
		pOut->output(KVI_OUT_GENERICERROR, szText);
	}
}

void KviIrcServerParser::parseNumericPasswordIncorrect(KviIrcMessage * msg)
{
	// 464: ERR_PASSWDMISMATCH
	// :prefix 464 <target> :Password Incorrect
	if(!msg->haltOutput())
	{
		KviWindow * pOut = msg->console()->activeWindow();
		QString szCmd = msg->connection()->decodeText(msg->safeParam(1));
		pOut->output(KVI_OUT_GENERICERROR, szCmd);
	}
}

// STARTTLS support
void KviIrcServerParser::parseNumericStartTls(KviIrcMessage * msg)
{
	// 670: RPL_STARTTLSOK
	// :prefix 670 <nickname> :STARTTLS successful, go ahead with TLS handshake
	// 691: RPL_STARTTLSFAIL
	// :prefix 691 <nickname> :STARTTLS failure

	bool bEnable = false;

	switch(msg->numeric())
	{
		case RPL_STARTTLSOK:
			//670 is used on some ircd as a whois reply: these ircds will not be able to
			//support starttls anyway.. see ticket #682
			if(!msg->connection()->stateData()->sentStartTls())
			{
				parseNumericWhoisOther(msg);
				return;
			}
			bEnable = true;
			break;
		case RPL_STARTTLSFAIL:
			bEnable = false;
			break;
	}

#ifdef COMPILE_SSL_SUPPORT
	msg->connection()->enableStartTlsSupport(bEnable);
#else  //!COMPILE_SSL_SUPPORT
	if(!msg->haltOutput())
		msg->console()->output(KVI_OUT_GENERICERROR, __tr2qs("STARTTLS reply received but SSL support is not compiled in: ignoring"));
#endif //!COMPILE_SSL_SUPPORT
}

void KviIrcServerParser::parseNumericNotRegistered(KviIrcMessage * msg)
{
	// 451: ERR_NOTREGISTERED
	// :prefix 451 PING :You have not registered

	if(msg->connection()->stateData()->isInsideInitialCapLs())
	{
		// CAP LS wasn't answered correctly.
		msg->connection()->handleFailedInitialCapLs();
		return;
	}

#ifdef COMPILE_SSL_SUPPORT
	if(msg->connection()->stateData()->isInsideInitialStartTls())
	{
		// The forced STARTTLS wasn't answered correctly.
		msg->connection()->handleFailedInitialStartTls();
		return;
	}
#endif //COMPILE_SSL_SUPPORT

	// If not registered yet and a CAP LS or STARTTLS request was sent out then
	// hide it (as WE have triggered the error).
	if(msg->connection()->stateData()->ignoreOneYouHaveNotRegisteredError())
	{
		// We DID send a CAP LS or a forced STARTTLS.
		// The request was answered properly and this is probably the error related to the following PING.
		// Eat it once, silently.
		msg->connection()->stateData()->setIgnoreOneYouHaveNotRegisteredError(false);
		return;
	}

	// We didn't send CAP LS so better show this to the user
	if(!msg->haltOutput())
	{
		QString szCmd = msg->connection()->decodeText(msg->safeParam(0));
		QString szErrorText = msg->connection()->decodeText(msg->safeTrailing());
		msg->console()->output(KVI_OUT_GENERICERROR, "%Q: %Q", &szCmd, &szErrorText);
	}
}

// SASL support
void KviIrcServerParser::parseNumericSaslLogin(KviIrcMessage * msg)
{
	// 900: RPL_SASLLOGIN
	// :prefix 900 <nickname> <usermask> <authuser>: You are now logged in as <authuser>

	if(!msg->haltOutput())
	{
		KviWindow * pOut = static_cast<KviWindow *>(msg->console());
		QString szParam = msg->connection()->decodeText(msg->safeParam(2));
		pOut->output(KVI_OUT_SERVERINFO, __tr2qs("Authenticated as %Q"), &szParam);
	}

	// This is the InspIRCd version of "You are now logged in as <nick>", but from
	// services only. i.e. not SASL.
	KviIrcConnectionServerInfo * pServerInfo = msg->connection()->serverInfo();
	QString version = pServerInfo->software();
	if(version == "InspIRCd")
		return;

	if(msg->connection()->stateData()->isInsideAuthenticate())
		msg->connection()->endInitialCapNegotiation();
}

void KviIrcServerParser::parseNumericSaslSuccess(KviIrcMessage * msg)
{
	// 903: RPL_SASLSUCCESS
	// :prefix 903 <nickname> :SASL authentication successful

	if(!msg->haltOutput())
	{
		KviWindow * pOut = static_cast<KviWindow *>(msg->console());
		pOut->outputNoFmt(KVI_OUT_SERVERINFO, __tr2qs("SASL authentication successful"));
	}

	if(msg->connection()->stateData()->isInsideAuthenticate())
		msg->connection()->endInitialCapNegotiation();
}

void KviIrcServerParser::parseNumericSaslFail(KviIrcMessage * msg)
{
	// 904: RPL_SASLFAILED
	// :prefix 904 * :SASL authentication failed

	// 906: RPL_SASLBORTED
	// :prefix 906 <nickname> :SASL authentication aborted

	// 907: RPL_SASLALREADYAUTH
	// :prefix 907 <nick> :You have already completed SASL authentication

	// 908: RPL_SASLMECHS
	// :server 908 <nick> <mechanisms> :are available SASL mechanisms

	if(!msg->haltOutput())
	{
		KviWindow * pOut = static_cast<KviWindow *>(msg->console());
		QString szParam = msg->connection()->decodeText(msg->safeTrailing());
		pOut->output(KVI_OUT_SERVERINFO, __tr2qs("SASL authentication error: %Q"), &szParam);
	}

	// Handle fallback if possible for SASL auth failure or dump if user mandates SASL
	// and no fallback is available
	if(msg->numeric() == 904)
	{
		if(msg->connection()->stateData()->sentSaslMethod() == QStringLiteral("EXTERNAL"))
		{
			if(!msg->connection()->target()->server()->saslNick().isEmpty() && !msg->connection()->target()->server()->saslPass().isEmpty())
			{
				KviWindow * pOut = static_cast<KviWindow *>(msg->console());
				pOut->output(KVI_OUT_SERVERINFO, __tr2qs("Attempting fallback due to SASL failure."));
				msg->connection()->sendFmtData("AUTHENTICATE PLAIN");
				msg->connection()->stateData()->setSentSaslMethod(QStringLiteral("PLAIN"));
				return;
			}
		}

		if(KVI_OPTION_BOOL(KviOption_boolDropConnectionOnSaslFailure))
		{
			KviWindow * pOut = static_cast<KviWindow *>(msg->console());
			pOut->output(KVI_OUT_SERVERINFO, __tr2qs("SASL auth failed. Dropping the connection."));
			msg->connection()->sendFmtData("QUIT");
			msg->connection()->abort();
			return;
		}
	}

	if(msg->connection()->stateData()->isInsideAuthenticate())
		msg->connection()->endInitialCapNegotiation();
}

void KviIrcServerParser::parseNumericOftcQuietList(KviIrcMessage * msg)
{
	// 344: RPL_QUIETLIST (oftc)
	// :prefix 344 target <channel> <banmask> [bansetby] [bansetat]
	QString szChan = msg->connection()->decodeText(msg->safeParam(1));
	// chMode is hard coded here, they do not give us any indication of what
	// it is in the reply. Another IRCd (u2+ircd-darenet) uses this same format
	// and uses +q for quiets as well.
	char chMode = 'q';
	QString banmask = msg->connection()->decodeText(msg->safeParam(2));
	QString bansetby = msg->connection()->decodeText(msg->safeParam(3));
	QString bansetat;
	getDateTimeStringFromCharTimeT(bansetat, msg->safeParam(4));
	if(bansetby.isEmpty())
		bansetby = __tr2qs("(Unknown)");
	KviChannelWindow * chan = msg->connection()->findChannel(szChan);
	if(chan && chan->sentListRequest(chMode))
	{
		chan->setModeInList(chMode, banmask, true, bansetby, QString(msg->safeParam(4)).toUInt());
		return;
	}
	if(!msg->haltOutput())
	{
		KviWindow * pOut = chan ? chan : KVI_OPTION_BOOL(KviOption_boolServerRepliesToActiveWindow) ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console());
		pOut->output(KVI_OUT_BAN, __tr2qs("%Q for \r!c\r%Q\r: \r!m-%c\r%Q\r (set by %Q on %Q)"),
		    &(__tr2qs("mode listing")), &szChan, chMode, &banmask, &bansetby, &bansetat);
	}
}

void KviIrcServerParser::parseNumericQuietList(KviIrcMessage * msg)
{
	// 728: RPL_QUIETLIST (freenode)
	// :prefix 728 target <channel> <mode> <banmask> [bansetby] [bansetat]
	QString szChan = msg->connection()->decodeText(msg->safeParam(1));
	// chMode is supposed to be a hardcoded 'q', but they could add other flags in the future
	char chMode = msg->connection()->decodeText(msg->safeParam(2)).at(0).toLatin1();
	QString banmask = msg->connection()->decodeText(msg->safeParam(3));
	QString bansetby = msg->connection()->decodeText(msg->safeParam(4));
	QString bansetat;
	getDateTimeStringFromCharTimeT(bansetat, msg->safeParam(5));
	if(bansetby.isEmpty())
		bansetby = __tr2qs("(Unknown)");
	KviChannelWindow * chan = msg->connection()->findChannel(szChan);
	if(chan && chan->sentListRequest(chMode))
	{
		chan->setModeInList(chMode, banmask, true, bansetby, QString(msg->safeParam(5)).toUInt());
		return;
	}
	if(!msg->haltOutput())
	{
		KviWindow * pOut = chan ? chan : KVI_OPTION_BOOL(KviOption_boolServerRepliesToActiveWindow) ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console());
		pOut->output(KVI_OUT_BAN, __tr2qs("%Q for \r!c\r%Q\r: \r!m-%c\r%Q\r (set by %Q on %Q)"),
		    &(__tr2qs("mode listing")), &szChan, chMode, &banmask, &bansetby, &bansetat);
	}
}

void KviIrcServerParser::parseNumericOftcEndOfQuietList(KviIrcMessage * msg)
{
	// 729: RPL_QUIETLISTEND (oftc)
	// :prefix 729 target <channel> :End of Channel Quiet List
	QString szChan = msg->connection()->decodeText(msg->safeParam(1));
	// because this is fork specific, we can be assured that the mode will
	// always be +q
	char chMode = 'q';
	KviChannelWindow * chan = msg->connection()->findChannel(szChan);
	if(chan && chan->sentListRequest(chMode))
	{
		chan->setListRequestDone(chMode);
		return;
	}
	if(!msg->haltOutput())
	{
		KviWindow * pOut = chan ? chan : KVI_OPTION_BOOL(KviOption_boolServerRepliesToActiveWindow) ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console());
		pOut->output(KVI_OUT_BAN, __tr2qs("End of channel \"%c\" %Q for \r!c\r%Q\r"), chMode, &(__tr2qs("mode list")), &szChan);
	}
}

void KviIrcServerParser::parseNumericEndOfQuietList(KviIrcMessage * msg)
{
	// 729: RPL_QUIETLISTEND (freenode)
	// :prefix 729 target <channel> <mode> :End of Channel Quiet List
	QString szChan = msg->connection()->decodeText(msg->safeParam(1));
	// chMode is supposed to be a hardcoded 'q', but they could add other flags in the future
	char chMode = msg->connection()->decodeText(msg->safeParam(2)).at(0).toLatin1();
	KviChannelWindow * chan = msg->connection()->findChannel(szChan);
	if(chan && chan->sentListRequest(chMode))
	{
		chan->setListRequestDone(chMode);
		return;
	}
	if(!msg->haltOutput())
	{
		KviWindow * pOut = chan ? chan : KVI_OPTION_BOOL(KviOption_boolServerRepliesToActiveWindow) ? msg->console()->activeWindow() : static_cast<KviWindow *>(msg->console());
		pOut->output(KVI_OUT_BAN, __tr2qs("End of channel \"%c\" %Q for \r!c\r%Q\r"), chMode, &(__tr2qs("mode list")), &szChan);
	}
}
