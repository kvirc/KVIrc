#ifndef _KVI_KVS_COREFUNCTIONS_H_
#define _KVI_KVS_COREFUNCTIONS_H_
//=============================================================================
//
//   File : KviKvsCoreFunctions.h
//   Creation date : Fri 31 Oct 2003 01:52:04 by Szymon Stefanek
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

#include "kvi_settings.h"
#include "KviKvsRunTimeContext.h"
#include "KviKvsVariantList.h"
#include "KviKvsVariant.h"
#include "KviKvsParameterProcessor.h"

#define KVSCF(_name) bool _name([[maybe_unused]] KviKvsRunTimeContext * __pContext, [[maybe_unused]] KviKvsVariantList * __pParams, [[maybe_unused]] KviKvsVariant * __pRetBuffer)

#define KVSCF_pContext __pContext
#define KVSCF_pParams __pParams
#define KVSCF_pRetBuffer __pRetBuffer

#define KVSCF_PARAMETER(a, b, c, d) KVS_PARAMETER(a, b, c, d)

#define KVSCF_PARAMETERS_BEGIN \
	KVS_PARAMETERS_BEGIN(parameter_format_list)

#define KVSCF_PARAMETERS_END                                                                     \
	KVS_PARAMETERS_END                                                                           \
	if(!KviKvsParameterProcessor::process(KVSCF_pParams, KVSCF_pContext, parameter_format_list)) \
		return false;

namespace KviKvsCoreFunctions
{
	// a_f
	KVSCF(active);
	KVSCF(aliasBody);
	KVSCF(array);
	KVSCF(asciiToBase64);
	KVSCF(asciiToHex);
	KVSCF(avatar);
	KVSCF(away);
	KVSCF(b);
	KVSCF(base64ToAscii);
	KVSCF(boolean);
	KVSCF(certificate);
	KVSCF(channel);
	KVSCF(charCKEYWORDWORKAROUND);
	KVSCF(classDefined);
	KVSCF(console);
	KVSCF(context);
	KVSCF(countStatusBarItems);
	KVSCF(cr);
	KVSCF(date);
	KVSCF(escape);
	KVSCF(falseCKEYWORDWORKAROUND);
	KVSCF(features);
	KVSCF(firstConnectedConsole);
	KVSCF(flatten);
	KVSCF(fmtlink);
	// g_l
	KVSCF(gender);
	KVSCF(globals);
	KVSCF(hash);
	KVSCF(inputText);
	KVSCF(hexToAscii);
	KVSCF(hostname);
	KVSCF(hptimestamp);
	KVSCF(i);
	KVSCF(icon);
	KVSCF(iconName);
	KVSCF(insideAlias);
	KVSCF(integer);
	KVSCF(isAnyConsoleConnected);
	KVSCF(isBot);
	KVSCF(isEmpty);
	KVSCF(isEventEnabled);
	KVSCF(isNumeric);
	KVSCF(isMeOp);
	KVSCF(isMeUserOp);
	KVSCF(isMeHalfOp);
	KVSCF(isMeVoice);
	KVSCF(isMainWindowActive);
	KVSCF(isMainWindowMinimized);
	KVSCF(isSet);
	KVSCF(isTimer);
	KVSCF(isWellKnown);
	KVSCF(k);
	KVSCF(keys);
	KVSCF(lag);
	KVSCF(lang);
	KVSCF(length);
	KVSCF(lf);
	KVSCF(link);
	KVSCF(listtimers);
	// m_r
	KVSCF(mask);
	KVSCF(me);
	KVSCF(msgtype);
	KVSCF(newCKEYWORDWORKAROUND);
	KVSCF(nothing);
	KVSCF(nullCKEYWORDWORKAROUND);
	KVSCF(o);
	KVSCF(option);
	KVSCF(query);
	KVSCF(r);
	KVSCF(rand);
	KVSCF(real);
	KVSCF(realname);
	KVSCF(receivedBytes);
	KVSCF(rsort);
	// s_z
	KVSCF(scriptContextName);
	KVSCF(selected);
	KVSCF(sentBytes);
	KVSCF(serialize);
	KVSCF(server);
	KVSCF(sort);
	KVSCF(split);
	KVSCF(string);
	KVSCF(sw);
	KVSCF(target);
	KVSCF(thisCKEYWORDWORKAROUND);
	KVSCF(timeCFUNCTIONWORKAROUND);
	KVSCF(tr);
	KVSCF(trueCKEYWORDWORKAROUND);
	KVSCF(typeofCKEYWORDWORKAROUND);
	KVSCF(u);
	KVSCF(uflags);
	KVSCF(unicode);
	KVSCF(unixtime);
	KVSCF(unserialize);
	KVSCF(username);
	KVSCF(version);
	KVSCF(window);
	// specials
	KVSCF(strayAt);
	KVSCF(mightBeStrayAtOrThis);
	//bool redirectToModule(const QString &szFrom,const QString &szModName,const QString &szFuncName

	void init();
};

#endif //!_KVI_KVS_COREFUNCTIONS_H_
