//=============================================================================
//
//   File : KviKvsCoreFunctions.cpp
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

#include "KviKvsCoreFunctions.h"
#include "KviKvsKernel.h"
#include "KviKvsObject.h"
#include "KviLocale.h"

namespace KviKvsCoreFunctions
{
	void init()
	{
		KviKvsKernel * pKern = KviKvsKernel::instance();

#define _REGFNC(__fncName, __routine)                                          \
	{                                                                          \
		KviKvsCoreFunctionExecRoutine * r = new KviKvsCoreFunctionExecRoutine; \
		r->proc = KVI_PTR2MEMBER(KviKvsCoreFunctions::__routine);              \
		pKern->registerCoreFunctionExecRoutine(QString(__fncName), r);         \
	}

		// a_f
		_REGFNC("active", active)
		_REGFNC("aliasBody", aliasBody)
		_REGFNC("array", array)
		_REGFNC("ascii", unicode)
		_REGFNC("asciiToHex", asciiToHex)
		_REGFNC("asciiToBase64", asciiToBase64)
		_REGFNC("away", away)
		_REGFNC("b", b)
		_REGFNC("base64ToAscii", base64ToAscii)
		_REGFNC("bool", boolean)
		_REGFNC("boolean", boolean)
		_REGFNC("certificate", certificate)
		_REGFNC("channel", channel)
		_REGFNC("char", charCKEYWORDWORKAROUND)
		_REGFNC("classDefined", classDefined)
		_REGFNC("console", console)
		_REGFNC("context", context)
		_REGFNC("countStatusBarItems", countStatusBarItems)
		_REGFNC("cr", cr)
		_REGFNC("date", date)
		_REGFNC("escape", escape)
		_REGFNC("false", falseCKEYWORDWORKAROUND)
		_REGFNC("features", features)
		_REGFNC("firstConnectedConsole", firstConnectedConsole)
		_REGFNC("flatten", flatten)
		_REGFNC("fmtlink", fmtlink)
		// g_l
		_REGFNC("gender", gender);
		_REGFNC("globals", globals);
		_REGFNC("hash", hash);
		//		_REGFNC("inputText",inputText);
		_REGFNC("hexToAscii", hexToAscii);
		_REGFNC("hostname", hostname);
		_REGFNC("hptimestamp", hptimestamp);
		_REGFNC("i", i);
		_REGFNC("ic", context);
		_REGFNC("icon", icon);
		_REGFNC("iconName", iconName);
		_REGFNC("insideAlias", insideAlias)
		_REGFNC("int", integer)
		_REGFNC("integer", integer)
		_REGFNC("isBot", isBot)
		_REGFNC("isEmpty", isEmpty)
		_REGFNC("isEventEnabled", isEventEnabled)
		_REGFNC("isMainWindowActive", isMainWindowActive)
		_REGFNC("isMainWindowMinimized", isMainWindowMinimized)
		_REGFNC("isNumeric", isNumeric)
		_REGFNC("isSet", isSet)
		_REGFNC("isTimer", isTimer)
		_REGFNC("isWellKnown", isWellKnown)
		_REGFNC("k", k)
		_REGFNC("keys", keys)
		_REGFNC("lag", lag)
		_REGFNC("lang", lang)
		_REGFNC("length", length)
		_REGFNC("lf", lf)
		_REGFNC("link", link)
		_REGFNC("listtimers", listtimers)
		// m_r
		_REGFNC("mask", mask)
		_REGFNC("me", me)
		_REGFNC("msgtype", msgtype)
		_REGFNC("new", newCKEYWORDWORKAROUND)
		_REGFNC("nothing", nothing)
		_REGFNC("null", nullCKEYWORDWORKAROUND)
		_REGFNC("o", o)
		_REGFNC("option", option)
		_REGFNC("query", query)
		_REGFNC("r", r)
		_REGFNC("rand", rand)
		_REGFNC("real", real)
		_REGFNC("realname", realname)
		_REGFNC("receivedBytes", receivedBytes)
		_REGFNC("rsort", rsort)
		// s_z
		_REGFNC("scriptContextName", scriptContextName)
		_REGFNC("selected", selected)
		_REGFNC("sentBytes", sentBytes)
		_REGFNC("serialize", serialize)
		_REGFNC("server", server)
		_REGFNC("sort", sort)
		_REGFNC("string", string)
		_REGFNC("sw", sw)
		_REGFNC("target", target)
		_REGFNC("this", thisCKEYWORDWORKAROUND)
		_REGFNC("tr", tr)
		_REGFNC("true", trueCKEYWORDWORKAROUND)
		_REGFNC("typeof", typeofCKEYWORDWORKAROUND)
		_REGFNC("u", u)
		_REGFNC("uflags", uflags)
		_REGFNC("unicode", unicode)
		_REGFNC("unixtime", unixtime)
		_REGFNC("unserialize", unserialize)
		_REGFNC("username", username)
		_REGFNC("version", version)
		_REGFNC("window", window)
		_REGFNC("$", thisCKEYWORDWORKAROUND)
		_REGFNC("@", strayAt)
		_REGFNC("@?", mightBeStrayAtOrThis)
#undef _REGCMD
	}

	static QString g_szStaticStrayConstantAt("@");

	KVSCF(strayAt)
	{
		KVSCF_pRetBuffer->setString(g_szStaticStrayConstantAt);
		return true;
	}

	KVSCF(mightBeStrayAtOrThis)
	{
		KviKvsObject * o = KVSCF_pContext->thisObject();
		if(o)
		{
			KVSCF_pRetBuffer->setHObject(o->handle());
		}
		else
		{
			KVSCF_pRetBuffer->setString(g_szStaticStrayConstantAt);
		}
		return true;
	}
};
