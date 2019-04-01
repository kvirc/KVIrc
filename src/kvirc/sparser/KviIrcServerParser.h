#ifndef _KVI_SPARSER_H_
#define _KVI_SPARSER_H_
//=============================================================================
//
//   File : KviIrcServerParser.h
//   Creation date : Sat Jun 29 2000 14:23:11 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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
#include "KviConsoleWindow.h"

#include <QObject>

#include <time.h>

class KviChannelWindow;
class KviIrcConnection;
class KviIrcMessage;
class KviIrcServerParser;
class KviMainWindow;
class KviWindow;
class QByteArray;

#define UNRECOGNIZED_MESSAGE(_msg, _err) \
	_msg->setUnrecognized();             \
	m_szLastParserError = _err;

#define IS_ME(_msg, _nick) KviQString::equalCI(_msg->connection()->currentNickName(), _nick)

typedef void (KviIrcServerParser::*messageParseProc)(KviIrcMessage *);

struct KviLiteralMessageParseStruct
{
	const char * msgName;
	messageParseProc proc;
};

class KviIrcMask;

struct KviCtcpMessage
{
	KviIrcMessage * msg;
	const char * pData;
	QString szTarget;
	KviIrcMask * pSource;
	bool bIgnored;
	bool bIsFlood;
	bool bUnknown;

	QString szTag;
};

struct KviDccRequest
{
	KviCString szType;
	KviCString szParam1;
	KviCString szParam2;
	KviCString szParam3;
	KviCString szParam4;
	KviCString szParam5;
	bool bIPv6;
	KviCtcpMessage * ctcpMsg;
	KviConsoleWindow * pConsole;
};

typedef void (KviIrcServerParser::*ctcpParseProc)(KviCtcpMessage *);

#define KVI_CTCP_MESSAGE_PARSE_TRIGGERNOEVENT 1

struct KviCtcpMessageParseStruct
{
	const char * msgName;
	ctcpParseProc req;
	ctcpParseProc rpl;
	int iFlags;
};

#define EXTERNAL_SERVER_DATA_PARSER_CONTROL_RESET 0
#define EXTERNAL_SERVER_DATA_PARSER_CONTROL_STARTOFDATA 1
#define EXTERNAL_SERVER_DATA_PARSER_CONTROL_ENDOFDATA 2

class KVIRC_API KviExternalServerDataParser
{
public:
	KviExternalServerDataParser(){};
	virtual ~KviExternalServerDataParser(){};

public:
	virtual void processData(KviIrcMessage *){};
	virtual void control(int){};
	virtual void die() { delete this; };
};

class KVIRC_API KviIrcServerParser : public QObject
{
	Q_OBJECT
public:
	KviIrcServerParser();
	~KviIrcServerParser();

private:
	static messageParseProc m_numericParseProcTable[1000];
	static KviLiteralMessageParseStruct m_literalParseProcTable[];
	static KviCtcpMessageParseStruct m_ctcpParseProcTable[];
	KviCString m_szLastParserError;

	//	KviCString                          m_szNoAwayNick; //<-- moved to KviConsoleWindow.h in KviConnectionInfo
public:
	void parseMessage(const char * message, KviIrcConnection * pConnection);

private:
	void parseNumeric001(KviIrcMessage * msg);
	void parseNumeric002(KviIrcMessage * msg);
	void parseNumeric003(KviIrcMessage * msg);
	void parseNumeric004(KviIrcMessage * msg);
	void parseNumeric005(KviIrcMessage * msg);
	void parseNumeric020(KviIrcMessage * msg);
	void parseNumeric344(KviIrcMessage * msg);
	void parseNumeric345(KviIrcMessage * msg);
	void parseNumeric367(KviIrcMessage * msg);
	void parseNumeric368(KviIrcMessage * msg);
	void parseNumeric480(KviIrcMessage * msg);
	void parseNumeric728(KviIrcMessage * msg);
	void parseNumeric729(KviIrcMessage * msg);
	void parseNumeric742(KviIrcMessage * msg);
	void parseNumeric486(KviIrcMessage * msg);

	void parseNumericSnomask(KviIrcMessage * msg);
	void parseNumericYourUID(KviIrcMessage * msg);
	void parseNumericMotd(KviIrcMessage * msg);
	void parseNumericNames(KviIrcMessage * msg);
	void parseNumericEndOfNames(KviIrcMessage * msg);
	void parseNumericTopic(KviIrcMessage * msg);
	void parseNumericNoTopic(KviIrcMessage * msg);
	void parseNumericTopicWhoTime(KviIrcMessage * msg);
	void parseNumericChannelModeIs(KviIrcMessage * msg);
	void parseNumericInviteList(KviIrcMessage * msg);
	void parseNumericEndOfInviteList(KviIrcMessage * msg);
	void parseNumericExceptList(KviIrcMessage * msg);
	void parseNumericEndOfExceptList(KviIrcMessage * msg);
	void parseNumericQList(KviIrcMessage * msg);
	void parseNumericEndOfQList(KviIrcMessage * msg);
	void parseNumericAList(KviIrcMessage * msg);
	void parseNumericEndOfAList(KviIrcMessage * msg);
	void parseNumericReopList(KviIrcMessage * msg);
	void parseNumericEndOfReopList(KviIrcMessage * msg);
	void parseNumericChanAccessList(KviIrcMessage * msg);
	void parseNumericEndOfChanAccessList(KviIrcMessage * msg);
	void parseNumericExemptChanOpList(KviIrcMessage * msg);
	void parseNumericEndOfExemptChanOpList(KviIrcMessage * msg);
	void parseNumericInvited(KviIrcMessage * msg);
	void parseNumericSpamFilterList(KviIrcMessage * msg);
	void parseNumericEndOfSpamFilterList(KviIrcMessage * msg);
	void parseNumericWhoReply(KviIrcMessage * msg);
	void parseNumericEndOfWho(KviIrcMessage * msg);
	void parseNumericNicknameProblem(KviIrcMessage * msg);
	void parseNumericWhoisAway(KviIrcMessage *);
	void parseNumericWhoisUser(KviIrcMessage * msg);
	void parseNumericWhoisChannels(KviIrcMessage * msg);
	void parseNumericWhoisIdle(KviIrcMessage * msg);
	void parseNumericWhoisServer(KviIrcMessage * msg);
	void parseNumericWhoisActually(KviIrcMessage * msg);
	void parseNumericWhoisOther(KviIrcMessage * msg);
	void parseNumericWhoisAuth(KviIrcMessage * msg);
	void parseNumericWhoisRegistered(KviIrcMessage * msg);
	void parseNumericWhowasUser(KviIrcMessage * msg);
	void parseNumericEndOfWhois(KviIrcMessage * msg);
	void parseNumericEndOfWhowas(KviIrcMessage * msg);
	void parseNumericNoSuchNick(KviIrcMessage * msg);
	void parseNumericChanUrl(KviIrcMessage * msg);
	void parseNumericCreationTime(KviIrcMessage * msg);
	void parseNumericIsOn(KviIrcMessage * msg);
	void parseNumericBanOnChan(KviIrcMessage * msg);
	void parseNumericUnavailResource(KviIrcMessage * msg);
	void parseNumericUserInChan(KviIrcMessage * msg);
	void parseNumericLinks(KviIrcMessage * msg);
	void parseNumericEndOfLinks(KviIrcMessage * msg);
	void parseNumericUserhost(KviIrcMessage * msg);
	void parseLoginNicknameProblem(KviIrcMessage * msg);
	void parseNumericBackFromAway(KviIrcMessage *);
	void parseNumericAway(KviIrcMessage *);
	void parseNumericUsersDontMatch(KviIrcMessage * msg);
	void parseNumericWatch(KviIrcMessage * msg);
	void parseNumericList(KviIrcMessage * msg);
	void parseNumericListStart(KviIrcMessage * msg);
	void parseNumericListEnd(KviIrcMessage * msg);
	void parseNumericCantJoinChannel(KviIrcMessage * msg);
	void parseNumericStats(KviIrcMessage * msg);
	void parseNumericUserMode(KviIrcMessage * msg);
	void parseNumericCodePageSet(KviIrcMessage * msg);
	void parseNumericCodePageScheme(KviIrcMessage * msg);
	void parseNumericCannotSend(KviIrcMessage * msg);
	void parseNumericNoSuchChannel(KviIrcMessage * msg);
	void parseNumericNoSuchServer(KviIrcMessage * msg);
	void parseNumericTime(KviIrcMessage * msg);
	void parseNumericHiddenHost(KviIrcMessage * msg);
	void parseNumericInfoEnd(KviIrcMessage * msg);
	void parseNumericInfoStart(KviIrcMessage * msg);
	void parseNumericInfo(KviIrcMessage * msg);
	void parseNumericYoureOper(KviIrcMessage * msg);
	void parseNumericNotEnoughParams(KviIrcMessage * msg);
	void parseNumericAlreadyRegistered(KviIrcMessage * msg);
	void parseNumericPasswordIncorrect(KviIrcMessage * msg);
	void parseNumericInviting(KviIrcMessage * msg);
	void parseNumericCommandSyntax(KviIrcMessage * msg);
	void parseNumericServerAdminInfoAdminContact(KviIrcMessage * msg);
	void parseNumericServerAdminInfoAdminName(KviIrcMessage * msg);
	void parseNumericServerAdminInfoServerName(KviIrcMessage * msg);
	void parseNumericServerAdminInfoTitle(KviIrcMessage * msg);
	void parseNumericCannotSendColor(KviIrcMessage * msg);
	void parseNumericEndOfStats(KviIrcMessage * msg);
	void parseNumericForward(KviIrcMessage * msg);
	void parseNumericNoPrivs(KviIrcMessage * msg);
	void parseNumericNotifyGeneric(KviIrcMessage * msg);
	void parseNumericYouHaveCallerID(KviIrcMessage * msg);
	void otherChannelError(KviIrcMessage * msg);
	void parseNumericUnknownCommand(KviIrcMessage * msg);
	void parseNumericMotdMissing(KviIrcMessage * msg);
	void parseCommandSyntaxHelp(KviIrcMessage * msg);
	void parseCommandHelp(KviIrcMessage * msg);
	void parseCommandEndOfHelp(KviIrcMessage * msg);
	void parseChannelHelp(KviIrcMessage * msg);
	void parseNumericNeedSSL(KviIrcMessage * msg);
	void parseNumericOftcQuietList(KviIrcMessage * msg);
	void parseNumericOftcEndOfQuietList(KviIrcMessage * msg);
	void parseNumericQuietList(KviIrcMessage * msg);
	void parseNumericEndOfQuietList(KviIrcMessage * msg);
	void parseNumericWhospcrpl(KviIrcMessage * msg);
	void parseNumericTryAgain(KviIrcMessage * msg);

	void parseLiteralPing(KviIrcMessage * msg);
	void parseLiteralJoin(KviIrcMessage * msg);
	void parseLiteralTopic(KviIrcMessage * msg);
	void parseLiteralPart(KviIrcMessage * msg);
	void parseLiteralPrivmsg(KviIrcMessage * msg);
	void parseLiteralNotice(KviIrcMessage * msg);
	void parseLiteralQuit(KviIrcMessage * msg);
	void parseLiteralNick(KviIrcMessage * msg);
	void parseLiteralMode(KviIrcMessage * msg);
	void parseLiteralKick(KviIrcMessage * msg);
	void parseLiteralInvite(KviIrcMessage * msg);
	void parseLiteralWallops(KviIrcMessage * msg);
	void parseLiteralPong(KviIrcMessage * msg);
	void parseLiteralError(KviIrcMessage * msg);
	void parseChannelMode(const QString & szNick, const QString & szUser, const QString & szHost, KviChannelWindow * chan, KviCString & modefl, KviIrcMessage * msg, int curParam);
	void parseUserMode(KviIrcMessage * msg, const char * modeflptr);
	void parseLiteralCap(KviIrcMessage * msg);
	void parseLiteralAuthenticate(KviIrcMessage * msg);
	void parseLiteralAway(KviIrcMessage * msg);

	void parseCtcpRequest(KviCtcpMessage * msg);
	void parseCtcpReply(KviCtcpMessage * msg);
	void echoCtcpRequest(KviCtcpMessage * msg);
	void echoCtcpReply(KviCtcpMessage * msg);
	void replyCtcp(KviCtcpMessage * msg, const QString & data);
	bool checkCtcpFlood(KviCtcpMessage * msg);
	void parseCtcpRequestPing(KviCtcpMessage * msg);
	void parseCtcpRequestVersion(KviCtcpMessage * msg);
	void parseCtcpRequestUserinfo(KviCtcpMessage * msg);
	void parseCtcpRequestClientinfo(KviCtcpMessage * msg);
	void parseCtcpRequestFinger(KviCtcpMessage * msg);
	void parseCtcpRequestSource(KviCtcpMessage * msg);
	void parseCtcpRequestAction(KviCtcpMessage * msg);
	void parseCtcpRequestTime(KviCtcpMessage * msg);
	void parseCtcpRequestAvatar(KviCtcpMessage * msg);
	void parseCtcpRequestDcc(KviCtcpMessage * msg);
	void parseCtcpRequestPage(KviCtcpMessage * msg);
	void parseCtcpReplyAvatar(KviCtcpMessage * msg);
	void parseCtcpReplyUserinfo(KviCtcpMessage * msg);
	void parseCtcpReplyGeneric(KviCtcpMessage * msg);
	void parseCtcpReplyPing(KviCtcpMessage * msg);
	void parseCtcpReplyLagcheck(KviCtcpMessage * msg);
	//	void parseCtcpReply(const char * msg_ptr,KviIrcUser &source,const char * target);
	// STARTTLS support
	void parseNumericStartTls(KviIrcMessage * msg);
	void parseNumericNotRegistered(KviIrcMessage * msg);
	//sasl
	void parseNumericSaslSuccess(KviIrcMessage * msg);
	void parseNumericSaslLogin(KviIrcMessage * msg);
	void parseNumericSaslFail(KviIrcMessage * msg);
	// IRCv3 stuffs
	void parseLiteralAccount(KviIrcMessage * msg);
	void parseLiteralChghost(KviIrcMessage * msg);

public:
	static void encodeCtcpParameter(const char * param, KviCString & buffer, bool bSpaceBreaks = true);
	static void encodeCtcpParameter(const char * param, QString & buffer, bool bSpaceBreaks = true);
	static const char * decodeCtcpEscape(const char * msg_ptr, KviCString & buffer);
	static const char * decodeCtcpEscape(const char * msg_ptr, QByteArray & buffer);
	static const char * extractCtcpParameter(const char * msg_ptr, KviCString & buffer, bool bSpaceBreaks = true, bool bSafeOnly = false);
	static const char * extractCtcpParameter(const char * msg_ptr, QString & buffer, bool bSpaceBreaks = true, bool bSafeOnly = false);
};

#ifndef _KVI_SPARSER_CPP_
extern KVIRC_API KviIrcServerParser * g_pServerParser;
#endif

#endif //_KVI_SPARSER_H_
