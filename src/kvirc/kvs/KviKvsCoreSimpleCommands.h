#ifndef _KVI_KVS_CORESIMPLECOMMANDS_H_
#define _KVI_KVS_CORESIMPLECOMMANDS_H_
//=============================================================================
//
//   File : KviKvsCoreSimpleCommands.h
//   Creation date : Fri 31 Oct 2003 00:04:25 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "KviMainWindow.h"
#include "KviConsoleWindow.h"
#include "kvi_settings.h"
#include "KviKvsRunTimeContext.h"
#include "KviKvsVariantList.h"
#include "KviKvsSwitchList.h"
#include "KviKvsParameterProcessor.h"

#define KVSCSC(_name) bool _name([[maybe_unused]] KviKvsRunTimeContext * __pContext, [[maybe_unused]] KviKvsVariantList * __pParams, [[maybe_unused]] KviKvsSwitchList * __pSwitches)

#define KVSCSC_pContext __pContext
#define KVSCSC_pParams __pParams
#define KVSCSC_pSwitches __pSwitches

#define KVSCSC_PARAMETER(a, b, c, d) KVS_PARAMETER(a, b, c, d)

#define KVSCSC_PARAMETERS_BEGIN \
	KVS_PARAMETERS_BEGIN(parameter_format_list)

#define KVSCSC_PARAMETERS_END                                                                      \
	KVS_PARAMETERS_END                                                                             \
	if(!KviKvsParameterProcessor::process(KVSCSC_pParams, KVSCSC_pContext, parameter_format_list)) \
		return false;

#define KVSCSC_REQUIRE_CONNECTION                    \
	if(!KVSCSC_pContext->window()->context())        \
		return KVSCSC_pContext->errorNoIrcContext(); \
	if(!KVSCSC_pContext->window()->connection())     \
		return KVSCSC_pContext->warningNoIrcConnection();

#define KVSCSC_pWindow __pContext->window()
#define KVSCSC_pConnection __pContext->window()->connection()
#define KVSCSC_pIrcContext __pContext->context()

namespace KviKvsCoreSimpleCommands
{
	// a_f
	KVSCSC(away);
	KVSCSC(back);
	KVSCSC(ban);
	KVSCSC(beep);
	KVSCSC(buttonctl);
	KVSCSC(cap);
	KVSCSC(chanadmin);
	KVSCSC(chanowner);
	KVSCSC(ctcp);
	KVSCSC(debugCKEYWORDWORKAROUND);
	KVSCSC(dechanadmin);
	KVSCSC(dechanowner);
	KVSCSC(dehalfop);
	KVSCSC(deleteCKEYWORDWORKAROUND);
	KVSCSC(delpopupitem);
	KVSCSC(deop);
	KVSCSC(deuserop);
	KVSCSC(devoice);
	KVSCSC(echoprivmsg);
	KVSCSC(echo);
	KVSCSC(error);
	KVSCSC(eval);
	KVSCSC(eventctl);
	KVSCSC(exit);
	// g_l
	KVSCSC(halfop);
	KVSCSC(halt);
	KVSCSC(help);
	KVSCSC(host);
	KVSCSC(inject);
	KVSCSC(join);
	KVSCSC(kick);
	KVSCSC(killtimer);
	KVSCSC(listtimers);
	// m_r
	KVSCSC(me);
	KVSCSC(mode);
	KVSCSC(nick);
	KVSCSC(notice);
	KVSCSC(op);
	KVSCSC(openurl);
	KVSCSC(operwall);
	KVSCSC(option);
	KVSCSC(parse);
	KVSCSC(part);
	KVSCSC(play);
	KVSCSC(popup);
	KVSCSC(privmsg);
	KVSCSC(query);
	KVSCSC(quit);
	KVSCSC(raise);
	KVSCSC(raw);
	KVSCSC(returnCKEYWORDWORKAROUND);
	KVSCSC(rebind);
	KVSCSC(rfc2812wrapper);
	KVSCSC(run);
	// s_z
	KVSCSC(say);
	KVSCSC(server);
	KVSCSC(setmenu);
	KVSCSC(setreturn);
	KVSCSC(squery);
	KVSCSC(srand);
	KVSCSC(topic);
	KVSCSC(trload);
	KVSCSC(trunload);
	KVSCSC(unban);
	KVSCSC(userop);
	KVSCSC(voice);
	KVSCSC(warning);
	KVSCSC(wallops);
	KVSCSC(who);
	KVSCSC(whois);
	KVSCSC(whowas);

	bool multipleModeCommand(KviKvsRunTimeContext * __pContext, KviKvsVariantList * __pParams, KviKvsSwitchList * __pSwitches, char plusminus, char flag);

	void init();
};

#endif //!_KVI_KVS_CORESIMPLECOMMANDS_H_
