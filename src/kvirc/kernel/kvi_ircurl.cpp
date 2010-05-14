//=============================================================================
//
//   File : kvi_ircurl.cpp
//   Creation date : Sun Mar 04 2001 14:20:12 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_string.h"
#include "kvi_inttypes.h"
#include "kvi_qstring.h"
#include "kvi_ircserver.h"
#include "kvi_kvs_script.h"
#include "kvi_msgbox.h"
#include "kvi_channel.h"
#include "kvi_app.h"
#include "kvi_locale.h"
#include "kvi_ircconnectiontarget.h"
#include "kvi_ircconnection.h"
#include "kvi_irccontext.h"
#include "kvi_console.h"
#include "kvi_frame.h"

#define _KVI_IRCURL_CPP_
#include "kvi_ircurl.h"


bool KviIrcUrl::parse(const char * url,KviStr &cmdBuffer,int contextSpec)
{
	// irc[6]://<server>[:<port>][/<channel>[?<pass>]]
	KviStr szUrl = url;
	//szUrl.replaceAll("$","\\$");
	//szUrl.replaceAll(";","\\;");
	bool bIPv6 = false;
	bool bSSL = false;
	KviStr szServer;
	kvi_u32_t uPort = 0;
	bool bGotPort = false;
	if(kvi_strEqualCIN(szUrl.ptr(),"irc://",6))
	{
		szUrl.cutLeft(6);
	} else if(kvi_strEqualCIN(szUrl.ptr(),"irc6://",7))
	{
		bIPv6 = true;
		szUrl.cutLeft(7);
	} else if(kvi_strEqualCIN(szUrl.ptr(),"ircs://",7))
	{
		bSSL = true;
		szUrl.cutLeft(7);
	} else if(kvi_strEqualCIN(szUrl.ptr(),"ircs6://",8))
	{
		bIPv6 = true;
		bSSL = true;
		szUrl.cutLeft(8);
	} else return false;

	KviStr szServerAndPort;

	int idx = szUrl.findFirstIdx('/');
	if(idx != -1)
	{
		szServerAndPort = szUrl.left(idx);
		szUrl.cutLeft(idx + 1);
	} else {
		szServerAndPort = szUrl;
		szUrl = "";
	}

	if(szServerAndPort.isEmpty())return false;

	idx = szServerAndPort.findFirstIdx(':');

	if(idx != -1)
	{
		szServer = szServerAndPort.left(idx);
		szServerAndPort.cutLeft(idx + 1);
		bool bOk;
		uPort = szServerAndPort.toUInt(&bOk);
		if(!bOk)uPort = 6667;
		bGotPort = true;
	} else {
		szServer = szServerAndPort;
	}

	cmdBuffer = "server ";
	switch(contextSpec)
	{
		case KVI_IRCURL_CONTEXT_FIRSTFREE:
			cmdBuffer.append("-u ");
			break;
		case KVI_IRCURL_CONTEXT_NEW:
			cmdBuffer.append("-n ");
			break;
	}
	if(bIPv6)cmdBuffer.append(" -i ");
	if(bSSL)cmdBuffer.append(" -s ");

	if(szUrl.hasData())
	{
		KviStr szChannel;
		KviStr szPass;

		idx = szUrl.findFirstIdx('?');
		if(idx != -1)
		{
			szChannel = szUrl.left(idx);
			szUrl.cutLeft(idx + 1);
			szPass = szUrl;
		} else {
			szChannel = szUrl;
			szPass = "";
		}

		if(!(szChannel.firstCharIs('#') || szChannel.firstCharIs('!') || szChannel.firstCharIs('&')))
				szChannel.prepend('#');

		if(szPass.isEmpty())cmdBuffer.append(KviStr::Format," -c=\"join %s\" ",szChannel.ptr());
		else cmdBuffer.append(KviStr::Format," -c=\"join %s %s\" ",szChannel.ptr(),szPass.ptr());

	}

	cmdBuffer.append(szServer);
	if(bGotPort)cmdBuffer.append(KviStr::Format," %d",uPort);

	cmdBuffer.append(';');

	return true;
}

void KviIrcUrl::split(QString url, KviIrcUrlParts& result)
{
	// irc[s][6]://<server>[:<port>][/<channel>[?<pass>]][[,<channel>[?<pass>]]

	//defaults
	result.bSsl=false;
	result.bIPv6=false;
	result.iPort = 6667;
	result.iError=0;

	int iProtoLen = url.indexOf("://");
	if(iProtoLen!=-1) {
		if(KviQString::equalCIN(url,"irc",3)) {
			// OK, seems to be a valid proto;
			url = url.right(url.length()-3);
			if(KviQString::equalCIN(url,"s",1)) {
				result.bSsl=true;
				url = url.right(url.length()-1);
			}
			if(KviQString::equalCIN(url,"6",1)) {
				result.bIPv6=true;
				url = url.right(url.length()-1);
			}
			if(!KviQString::equalCIN(url,"://",3)) {
				//irc(???):// proto??
				result.iError |= InvalidProtocol;
			}
			iProtoLen = url.indexOf("://");
			url = url.right(url.length()-iProtoLen-3);
		} else {
			result.iError |= InvalidProtocol;
		}
	}
	//Ok, we understand a protocol.. Now we shuld find a server name:)
	int iTmp;
	iTmp = url.indexOf(':');
	if(iTmp!=-1) {
		result.szHost = url.left(iTmp);
		url = url.right(url.length()-iTmp-1);
		// Accepted, now the time for the port:)
		bool bOk;
		if( (iTmp = url.indexOf('/')) != -1) { // any channels pending?
			result.iPort = url.left(iTmp).toUInt(&bOk);
			if(!bOk) {
				result.iPort = 6667;
				result.iError |= InvalidPort;
			}
			url = url.right(url.length()-iTmp-1);
		} else {
			result.iPort = url.toUInt(&bOk);
			if(!bOk) {
				result.iPort = 6667;
				result.iError |= InvalidPort;
			}
			url = "";
		}
	} else if( (iTmp = url.indexOf('/')) != -1) { // have channels??
		result.szHost = url.left(iTmp);
		url = url.right(url.length()-iTmp-1);
	} else {
		result.szHost = url;
		url = "";
	}

	//and, finally, channels:D
	result.chanList = url.isEmpty()?QStringList():url.split(',',QString::SkipEmptyParts);

}

void KviIrcUrl::join(QString &uri, KviServer* server)
{
	if(server)
	{
		uri="irc";

		if(server->useSSL()) uri.append("s");
		if(server->isIPv6()) uri.append("6");

		uri.append("://");
		uri.append(server->hostName());
		if(server->port()!=6667) uri.append(QString(":%1").arg(server->port()));
		uri.append("/");
	}
}

void KviIrcUrl::makeJoinCmd(const QStringList& chans, QString& szJoinCommand)
{
	QString szChannels,szProtectedChannels,szPasswords,szCurPass,szCurChan;
		if(chans.count()!=0)
		{

			for ( QStringList::ConstIterator it = chans.begin(); it != chans.end(); ++it ) {

				szCurPass=(*it).section('?',1);
				if(szCurPass.isEmpty())
				{
					if(!szChannels.isEmpty())
						szChannels.append(",");
					szCurChan = (*it).section('?',0,0);
					if(!(szCurChan[0]=='#' || szCurChan[0]=='&' || szCurChan[0]=='!'))
							szCurChan.prepend('#');
					szChannels.append(szCurChan);
				} else {
					if(!szProtectedChannels.isEmpty())
						szProtectedChannels.append(",");
					szCurChan = (*it).section('?',0,0);
					if(!(szCurChan[0]=='#' || szCurChan[0]=='&' || szCurChan[0]=='!'))
							szCurChan.prepend('#');
					szProtectedChannels.append(szCurChan);
					if(!szPasswords.isEmpty())
						szPasswords.append(",");
					szPasswords.append(szCurPass);
				}
			}
			szJoinCommand = "JOIN ";
			szJoinCommand.append(szProtectedChannels);
			if(!szProtectedChannels.isEmpty() && !szChannels.isEmpty())
				szJoinCommand.append(',');
			szJoinCommand.append(szChannels);
			szJoinCommand.append(" ");
			szJoinCommand.append(szPasswords);
		}
}

int KviIrcUrl::run(const QString& text,int contextSpec,KviConsole* pConsole)
{
	KviIrcUrlParts parts;
	KviIrcUrl::split(text,parts);
	QString cmdBuffer;

	if( (contextSpec & CurrentContext) && !pConsole) {
		contextSpec = FirstFreeContext;
	}

	if( (contextSpec & TryCurrentContext) && !pConsole) {
		contextSpec = FirstFreeContext;
	}

	if(contextSpec & FirstFreeContext) {
		if(pConsole) {
			if(pConsole->connectionInProgress())
			{
				pConsole = g_pFrame->firstNotConnectedConsole();
				if(!pConsole) {
					pConsole = g_pFrame->createNewConsole();
				}
			}
		} else {
			pConsole = g_pFrame->firstNotConnectedConsole();
			if(!pConsole) {
				pConsole = g_pFrame->createNewConsole();
			}
		}
	}

	if(!(parts.iError & KviIrcUrl::InvalidProtocol)) {
		g_pApp->addRecentUrl(text);

		QString szJoinCommand;
		makeJoinCmd(parts.chanList,szJoinCommand);
		QString szCommand("server ");
		if(parts.bSsl) szCommand.append("-s ");
		if(parts.bIPv6) szCommand.append("-i ");
		if(!szJoinCommand.isEmpty()){
			szCommand.append("-c=\"");
			szCommand.append(szJoinCommand);
			szCommand.append("\" ");
		}
		szCommand.append(QString("%1 %2 ").arg(parts.szHost).arg(parts.iPort));

		if(pConsole->connection()) {
			KviServer* server = pConsole->connection()->target()->server();
			if(
				( server->hostName() != parts.szHost ) ||
				( server->port() != parts.iPort ) ||
				( server->useSSL() != parts.bSsl ) ||
				( server->isIPv6() != parts.bIPv6) )
			{ // New server, try to reconnect
				KviKvsScript::run(szCommand,(contextSpec & TryCurrentContext) ? g_pFrame->createNewConsole() : pConsole);
				return parts.iError;
			} else {
				// the same server, but probably new chanlist
				QString tmp;
				QString toPart;
				for(KviChannel * c = pConsole->connection()->channelList()->first();c;c = pConsole->connection()->channelList()->next())
				{
					tmp=c->target();
					if(c->hasChannelKey()) {
						tmp.append("?");
						tmp.append(c->channelKey());
					}
					if(!parts.chanList.removeAll(tmp))
					{
						toPart.append(c->target());
						toPart.append(",");
					}
				}
				if(!(contextSpec & DoNotPartChans))
				{
					makeJoinCmd(parts.chanList,szJoinCommand);
					if(!toPart.isEmpty())
					{
						toPart.prepend("part ");
						KviKvsScript::run(toPart,pConsole);
					}
				}
				if(!szJoinCommand.isEmpty())
				{
					pConsole->connection()->sendData(pConsole->connection()->encodeText(szJoinCommand).data());
				}
				return parts.iError;
			}
		}
		// New server
		KviKvsScript::run(szCommand,pConsole);
	}
	//!invalid proto
	return parts.iError;
}
