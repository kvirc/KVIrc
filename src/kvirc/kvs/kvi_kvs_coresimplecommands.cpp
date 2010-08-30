//=============================================================================
//
//   File : kvi_kvs_coresimplecommands.cpp
//   Creation date : Fri 31 Oct 2003 00:04:25 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2008 Szymon Stefanek <pragma at kvirc dot net>
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

#include "kvi_ircconnectionserverinfo.h"
#include "kvi_kvs_coresimplecommands.h"

#include "kvi_kvs_kernel.h"
#include "kvi_options.h"
#include "kvi_ircconnection.h"
#include "kvi_locale.h"
#include "kvi_kvs_treenode_command.h"

#include <QRegExp>
#include <QStringList>

namespace KviKvsCoreSimpleCommands
{
	void init()
	{
		KviKvsKernel * pKern = KviKvsKernel::instance();

#define _REGCMD(__cmdName,__routine) \
		{ \
			KviKvsCoreSimpleCommandExecRoutine * r = new KviKvsCoreSimpleCommandExecRoutine; \
			r->proc = KVI_PTR2MEMBER(KviKvsCoreSimpleCommands::__routine); \
			pKern->registerCoreSimpleCommandExecRoutine(QString(__cmdName),r); \
		}
		// a_f
		_REGCMD("admin",rfc2812wrapper)
		_REGCMD("away",away)
		_REGCMD("back",back)
		_REGCMD("ban",ban)
		_REGCMD("beep",beep)
		_REGCMD("buttonctl",buttonctl)
		_REGCMD("cap",cap)
		_REGCMD("chanadmin",chanadmin)
		_REGCMD("chanowner",chanowner)
		_REGCMD("codepage",rfc2812wrapper)
		_REGCMD("ctcp",ctcp)
		_REGCMD("debug",debugCKEYWORDWORKAROUND)
		_REGCMD("dechanadmin",dechanadmin)
		_REGCMD("dechanowner",dechanowner)
		_REGCMD("dehalfop",dehalfop)
		_REGCMD("delete",deleteCKEYWORDWORKAROUND)
		_REGCMD("delpopupitem",delpopupitem)
		_REGCMD("destroy",deleteCKEYWORDWORKAROUND)
		_REGCMD("deop",deop)
		_REGCMD("deuserop",deuserop)
		_REGCMD("devoice",devoice)
		_REGCMD("die",error)
		_REGCMD("echoprivmsg",echoprivmsg)
		_REGCMD("echo",echo)
		_REGCMD("error",error)
		_REGCMD("eval",eval)
		_REGCMD("eventctl",eventctl)
		_REGCMD("exit",exit)
		// g_l
		_REGCMD("halfop",halfop)
		_REGCMD("halt",halt)
		_REGCMD("help",help)
		_REGCMD("host",host)
		_REGCMD("include",parse)
		_REGCMD("info",rfc2812wrapper)
		_REGCMD("inject",inject)
		_REGCMD("invite",rfc2812wrapper)
		_REGCMD("join",join)
		_REGCMD("kick",kick)
		_REGCMD("killtimer",killtimer)
		_REGCMD("leave",part)
		_REGCMD("links",rfc2812wrapper)
		_REGCMD("list",rfc2812wrapper)
		_REGCMD("listtimers",listtimers)
		_REGCMD("lusers",rfc2812wrapper)
		// m_r
		_REGCMD("me",me)
		_REGCMD("mode",mode)
		_REGCMD("motd",rfc2812wrapper)
		_REGCMD("msg",privmsg)
		_REGCMD("nick",nick)
		_REGCMD("notice",notice)
		_REGCMD("op",op)
		_REGCMD("openurl",openurl)
		_REGCMD("oper",rfc2812wrapper)
		_REGCMD("option",option)
		_REGCMD("parse",parse)
		_REGCMD("part",part)
		_REGCMD("pass",rfc2812wrapper)
		_REGCMD("play",play)
		_REGCMD("popup",popup)
		_REGCMD("privmsg",privmsg)
		_REGCMD("query",query)
		_REGCMD("quit",quit)
		_REGCMD("quote",raw)
		_REGCMD("raise",raise)
		_REGCMD("raw",raw)
		_REGCMD("return",returnCKEYWORDWORKAROUND)
		_REGCMD("rebind",rebind)
		// s_z
		_REGCMD("run",run)
		_REGCMD("say",say)
		_REGCMD("server",server)
		_REGCMD("service",rfc2812wrapper)
		_REGCMD("setmenu",setmenu)
		_REGCMD("setreturn",setreturn)
		_REGCMD("sockets",rfc2812wrapper)
		_REGCMD("squery",rfc2812wrapper)
		_REGCMD("squit",rfc2812wrapper)
		_REGCMD("srand",srand)
		_REGCMD("stats",rfc2812wrapper)
		_REGCMD("time",rfc2812wrapper)
		_REGCMD("topic",topic)
		_REGCMD("trace",rfc2812wrapper)
		_REGCMD("trload",trload)
		_REGCMD("trunload",trunload)
		_REGCMD("unban",unban)
		_REGCMD("userop",userop)
		_REGCMD("version",rfc2812wrapper)
		_REGCMD("voice",voice)
		_REGCMD("warning",warning)
		_REGCMD("who",who)
		_REGCMD("whois",whois)
		_REGCMD("whowas",whowas)

#undef _REGCMD
	}


	bool multipleModeCommand(KviKvsRunTimeContext * __pContext,KviKvsVariantList * __pParams,KviKvsSwitchList * __pSwitches,char plusminus,char flag)
	{
		Q_UNUSED(__pSwitches);

		QString szTokens;
		KVSCSC_PARAMETERS_BEGIN
			KVSCSC_PARAMETER("tokens",KVS_PT_STRING,KVS_PF_APPENDREMAINING,szTokens)
		KVSCSC_PARAMETERS_END

		KVSCSC_REQUIRE_CONNECTION

		if(KVSCSC_pWindow->type() != KVI_WINDOW_TYPE_CHANNEL)
		{
			KVSCSC_pContext->warning(__tr2qs_ctx("This is not a channel","kvs"));
			return true;
		}

		QStringList sl = szTokens.split(QRegExp("[, ]+"),QString::SkipEmptyParts);

		QByteArray szFlags;
		QByteArray szTarget = KVSCSC_pConnection->encodeText(KVSCSC_pWindow->windowName());
		QByteArray szMessage;

		int i = 0;
		for(QStringList::Iterator it = sl.begin();it != sl.end();++it)
		{
			QByteArray szTxt = KVSCSC_pConnection->encodeText(*it);
			if(!szTxt.isEmpty())
			{
				szFlags += flag;
				if(szMessage.length() > 0)szMessage += " ";
				szMessage += szTxt;
				i++;
				if(i >= KVSCSC_pConnection->serverInfo()->maxModeChanges())
				{
					if(!KVSCSC_pConnection->sendFmtData("MODE %s %c%s %s",szTarget.data(),plusminus,szFlags.data(),szMessage.data()))
						return KVSCSC_pContext->warningNoIrcConnection();
					i = 0;
					szFlags = "";
					szMessage = "";
				}
			}
		}

		if(i > 0)
		{
			if(!KVSCSC_pConnection->sendFmtData("MODE %s %c%s %s",szTarget.data(),plusminus,szFlags.data(),szMessage.data()))
				return KVSCSC_pContext->warningNoIrcConnection();
		}

		return true;
	}

	/*
		@doc: rfc2821wrappers
		@title:
			RFC2821 Wrappers
		@type:
			generic
		@short:
			Wrappers for rfc2821 commands
		@body:
			Some less used KVIrc commands are implemented as simple wrappers
			around the standard [doc]rfc2821[/doc] commands.[br]
			For example, the /[cmd:oper]OPER[/cmd] command will be used only by server operators
			and probably only once per connection.[br]
			These commands should only be needed by experienced IRC users.
			Due to this fact, these commands have no special kind of parsing performed by KVIrc;
			the parameters are extracted, the identifiers are parsed normally
			and are sent to the server without any semantic check.[br]
			This means that while [cmd:oper]OPER[/cmd] requires two parameters, KVIrc will not
			complain if you send a single parameter only or use ten parameters.[br]
			The exact parameter syntax/semantic checking is left to you.[br]
			This also means that if some of these commands accept parameters with spaces,
			it is left to you to add the leading ':' before the last parameter.[br]
			You should refer to [doc]rfc2812[/doc] if any of these commands do not work properly for you.[br]
	*/

	KVSCSC(rfc2812wrapper)
	{
		Q_UNUSED(__pSwitches);

		QString szText;
		KVSCSC_PARAMETERS_BEGIN
			KVSCSC_PARAMETER("text",KVS_PT_STRING,KVS_PF_OPTIONAL | KVS_PF_APPENDREMAINING,szText)
		KVSCSC_PARAMETERS_END

		KVSCSC_REQUIRE_CONNECTION

		QByteArray txt = KVSCSC_pConnection->encodeText(szText);
		QByteArray cmd = KVSCSC_pConnection->encodeText(((KviKvsTreeNodeCommand *)(KVSCSC_pContext->defaultReportLocation()))->commandName());

		if(!KVSCSC_pConnection->sendFmtData("%s %s",cmd.data(),txt.data() ? txt.data() : ""))
			return KVSCSC_pContext->warningNoIrcConnection();

		return true;
	}
};
