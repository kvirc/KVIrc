//=============================================================================
//
//   File : KviIrcUrl.cpp
//   Creation date : Sun Mar 04 2001 14:20:12 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviCString.h"
#include "kvi_inttypes.h"
#include "KviQString.h"
#include "KviIrcServer.h"
#include "KviKvsScript.h"
#include "KviMessageBox.h"
#include "KviChannelWindow.h"
#include "KviApplication.h"
#include "KviLocale.h"
#include "KviIrcConnectionTarget.h"
#include "KviIrcConnection.h"
#include "KviIrcContext.h"
#include "KviConsoleWindow.h"
#include "KviMainWindow.h"

#include <map>

#define _KVI_IRCURL_CPP_
#include "KviIrcUrl.h"

// KviApplication.cpp
extern KVIRC_API std::map<QString, KviWindow *> g_pGlobalWindowDict;

bool KviIrcUrl::parse(const char * url, KviCString & cmdBuffer, int contextSpec)
{
	// irc[6]://<server>[:<port>][/<channel>[?<pass>]]

	KviIrcUrlParts urlParts;
	KviIrcUrl::split(url, urlParts);

	if(urlParts.iError & InvalidUrl)
		return false;

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
	if(urlParts.bIPv6)
		cmdBuffer.append(" -i ");
	if(urlParts.bSsl)
		cmdBuffer.append(" -s ");

	QString channels, passwords;

	if(urlParts.chanList.size())
	{
		for(int i = 0; i < urlParts.chanList.size(); ++i)
		{
			QStringList splitted = urlParts.chanList[i].split("?");
			if(i)
				channels.append(',');
			if(!(splitted[0].startsWith('#') || splitted[0].startsWith('!') || splitted[0].startsWith('&')))
				channels.append('#');
			channels.append(splitted[0]);

			if(splitted.size() > 1)
			{
				if(i)
					passwords.append(',');
				passwords.append(splitted[1]);
			}
		}

		cmdBuffer.append(KviCString::Format, " -c=\"join %s %s\" ", channels.data(), passwords.data());
	}

	cmdBuffer.append(urlParts.szHost);
	cmdBuffer.append(KviCString::Format, " %d", urlParts.iPort);

	cmdBuffer.append(';');

	return true;
}

void KviIrcUrl::split(const QString & url, KviIrcUrlParts & result)
{
	// irc[s][6]://<server>[:<port>][/<channel>[?<pass>]][[,<channel>[?<pass>]]

	//defaults
	result.bSsl = false;
	result.bIPv6 = false;
	result.iPort = 6667;
	result.iError = 0;

	QRegExp rx(R"(^(irc(s)?(6)?://)?\[?([\w\d\.-]*|[\d:a-f]*)\]?(:(\d*))?(/(.*))?$)");

	if(rx.indexIn(url) < 0)
	{
		result.iError |= InvalidUrl;
		return; // doesn't match? then it's not even a url...
	}

	if(!rx.cap(1).isEmpty())
	{
		result.bSsl = rx.cap(2) == "s";
		result.bIPv6 = rx.cap(3) == "6";
	}

	result.bIPv6 = result.bIPv6 || rx.cap(3).contains(":");

	if(rx.cap(6).toUInt() > 65535 || rx.cap(6).toUInt() < 1)
	{
		result.iError |= InvalidPort;
		if(result.bSsl)
			result.iPort = 6697;
	}
	else
		result.iPort = rx.cap(6).toUInt();

	result.szHost = rx.cap(4);

	if(result.szHost.isEmpty())
		result.iError |= InvalidUrl;

	result.chanList = rx.cap(8).isEmpty() ? QStringList() : rx.cap(8).split(',', QString::SkipEmptyParts);
}

void KviIrcUrl::join(QString & uri, KviIrcServer * server)
{
	if(server)
	{
		uri = "irc";

		if(server->useSSL())
			uri.append('s');
		if(server->isIPv6())
			uri.append('6');

		uri.append("://");
		if(server->isIPv6() && server->hostName().contains(':'))
			uri.append('[');
		uri.append(server->hostName());
		if(server->isIPv6() && server->hostName().contains(':'))
			uri.append(']');
		if(server->port() != 6667)
			uri.append(QString(":%1").arg(server->port()));
		uri.append('/');
	}
}

void KviIrcUrl::makeJoinCmd(const QStringList & chans, QString & szJoinCommand)
{
	QString szChannels, szProtectedChannels, szPasswords, szCurPass, szCurChan;
	if(!chans.isEmpty())
	{

		for(const auto & chan : chans)
		{

			szCurPass = chan.section('?', 1);
			if(szCurPass.isEmpty())
			{
				if(!szChannels.isEmpty())
					szChannels.append(',');
				szCurChan = chan.section('?', 0, 0);
				if(!(szCurChan[0] == '#' || szCurChan[0] == '&' || szCurChan[0] == '!'))
					szCurChan.prepend('#');
				szChannels.append(szCurChan);
			}
			else
			{
				if(!szProtectedChannels.isEmpty())
					szProtectedChannels.append(',');
				szCurChan = chan.section('?', 0, 0);
				if(!(szCurChan[0] == '#' || szCurChan[0] == '&' || szCurChan[0] == '!'))
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

int KviIrcUrl::run(const QString & text, int contextSpec, KviConsoleWindow * pConsole)
{
	// first, sanity check the url
	QString cmdBuffer;
	QString szJoinCommand;
	KviIrcUrlParts parts;
	bool bAlreadyConnected = false;

	KviIrcUrl::split(text, parts);
	if(parts.iError & KviIrcUrl::InvalidProtocol || parts.iError & KviIrcUrl::InvalidUrl)
	{
		// invalid proto
		return parts.iError;
	}

	g_pApp->addRecentUrl(text);
	makeJoinCmd(parts.chanList, szJoinCommand);

	// then, get a context
	if((contextSpec & CurrentContext) && !pConsole)
		contextSpec = FirstFreeContext;

	if(contextSpec & TryCurrentContext)
	{
		if(pConsole)
		{
			KviConsoleWindow * pTmpConsole = pConsole;
			pConsole = nullptr;

			if(pTmpConsole->connection())
			{
				KviIrcServer * server = pTmpConsole->connection()->target()->server();
				if(
				    (server->hostName() == parts.szHost) && (server->port() == parts.iPort) && (server->useSSL() == parts.bSsl) && (server->isIPv6() == parts.bIPv6))
				{
					pConsole = pTmpConsole;
					bAlreadyConnected = true;
				}
			}

			if(!pConsole)
				contextSpec = FirstFreeContext;
		}
		else
		{
			contextSpec = FirstFreeContext;
		}
	}

	if(contextSpec & TryEveryContext)
	{
		KviConsoleWindow * pTmpConsole = nullptr;
		pConsole = nullptr;

		for(auto & wnd : g_pGlobalWindowDict)
		{
			if(wnd.second->type() == KviWindow::Console)
			{
				pTmpConsole = (KviConsoleWindow *)wnd.second;

				if(pTmpConsole->connection())
				{
					KviIrcServer * server = pTmpConsole->connection()->target()->server();
					if(
					    (server->hostName() == parts.szHost) && (server->port() == parts.iPort) && (server->useSSL() == parts.bSsl) && (server->isIPv6() == parts.bIPv6))
					{
						pConsole = pTmpConsole;
						bAlreadyConnected = true;
						break;
					}
				}
			}
		}

		if(!pConsole)
			contextSpec = FirstFreeContext;
	}

	if(contextSpec & FirstFreeContext)
	{
		if(pConsole)
		{
			if(pConsole->connectionInProgress())
			{
				pConsole = g_pMainWindow->firstNotConnectedConsole();
				if(!pConsole)
				{
					pConsole = g_pMainWindow->createNewConsole();
				}
			}
		}
		else
		{
			pConsole = g_pMainWindow->firstNotConnectedConsole();
			if(!pConsole)
			{
				pConsole = g_pMainWindow->createNewConsole();
			}
		}
	}

	if(bAlreadyConnected)
	{
		// we already have a connected context to the right server on pConsole
		// just check if thr user want us to join or part any channel
		QString tmp;
		QString toPart;
		for(auto & c : pConsole->connection()->channelList())
		{
			tmp = c->target();
			if(c->hasChannelMode('k'))
			{
				tmp.append("?");
				tmp.append(c->channelModeParam('k'));
			}
			if(!parts.chanList.removeAll(tmp))
			{
				toPart.append(c->target());
				toPart.append(",");
			}
		}
		if(!(contextSpec & DoNotPartChans))
		{
			makeJoinCmd(parts.chanList, szJoinCommand);
			if(!toPart.isEmpty())
			{
				toPart.prepend("part ");
				KviKvsScript::run(toPart, pConsole);
			}
		}
		if(!szJoinCommand.isEmpty())
		{
			pConsole->connection()->sendData(pConsole->connection()->encodeText(szJoinCommand).data());
		}
		return parts.iError;
	}
	else
	{
		// we need to create a new connection on the context of pConsole
		QString szCommand("server ");
		if(parts.bSsl)
			szCommand.append("-s ");
		if(parts.bIPv6)
			szCommand.append("-i ");
		if(!szJoinCommand.isEmpty())
		{
			szCommand.append("-c=\"");
			szCommand.append(szJoinCommand);
			szCommand.append("\" ");
		}
		szCommand.append(QString("%1 %2 ").arg(parts.szHost).arg(parts.iPort));

		KviKvsScript::run(szCommand, (contextSpec & TryCurrentContext) ? g_pMainWindow->createNewConsole() : pConsole);
		return parts.iError;
	}
}
