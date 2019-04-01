//=============================================================================
//
//   File : libkvinotifier.cpp
//   Creation date : Tue Jul 7 2004 20:21:12 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2005-2008 Iacopo Palazzi < iakko(at)siena(dot)linux(dot)it >
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

#include "NotifierWindow.h"
#include "NotifierMessage.h"

#include "KviModule.h"
#include "KviKvsVariant.h"
#include "KviApplication.h"
#include "KviMainWindow.h"
#include "KviWindow.h"
#include "KviLocale.h"
#include "KviIconManager.h"
#include "KviTimeUtils.h"
#include "KviOptions.h"

NotifierWindow * g_pNotifierWindow = nullptr;
kvi_time_t g_tNotifierDisabledUntil = 0;

/*
	@doc: notifier.message
	@type:
		command
	@title:
		notifier.message
	@short:
		Adds a message to the notifier window
	@syntax:
		notifier.message [-q|quiet] [-n|noanim] [-w[=<window_id:string>]!] [-i=<image_id:string>] [-t=<timeout:integer>] <messag:string>
	@description:
		Adds a message to the notifier window.
		The notifier window is shown (if not already visible)
		unless the -q switch is present.
		The new message becomes the current message of the notifier
		unless the user is already typing in the input window
		and the typed message would be directed to a different window.
		In that case the message is appended at the end of the
		message queue and the user will be able to scroll to it
		by using the proper notifier buttons.[br]
		The &lt;message&gt; text can contain simple HTML tags: basically you are
		allowed to use &lt;b&gt; and &lt;i&gt;. The usage of other
		tags is possible but is discouraged since it tends to
		mess up the message display. In particular you should avoid
		any color and/or font specification since the notifier is
		skinnable and you don't know which color will result in a visible text.
		[b]Please note that the user can forcibly disable the notifier
		for a limited period of time (a sort of [i]don't bug me[/i] option).[/b]
	@switches:
		!sw: -n | --noanim
		Do not animate
		!sw: -w | --windowid
		Causes the message gets attached to the specified window and
		the user is able to type commands in that window after
		showing up the notifier input. If the "=&lt;window_id&gt;" part
		is omitted then the current window is used.[br]
		!sw: -i | --icon
		If the -i=&lt;image_id&gt; switch is present then the
		message has the specified image displayed.
		See the [doc:image_id]documentation on the image identifier[/doc]
		for more information about the image_id parameter.[br]
		!sw: -q | --quiet
		If you use -q then you must explicitly call [cmd]notifier.show[/cmd] to
		show the notifier. If the -n switch is present then
		the show action will not be animated (the notifier
		will be shown immediately instead of fading in).
		Obviously -n has no meaning if -q is used.[br]
		!sw: -t | --timeout
		Set the message lifetime to <timeout>
		Obviously this option has no meaning if the window is not going to be shown.
		The timeout may be overridden by new messages but only in the future.
		If the timeout expires and is not overridden by any new message
		then the window will be automatically hidden.
		A zero timeout disables auto-hiding.
	@seealso:
		[cmd]notifier.show[/cmd] [cmd]notifier.hide[/cmd] [fnc]$notifier.isEnabled[/fnc]()
	@examples:
		[example]
			notifier.message Hello world!
			[cmd]notifier.hide[/cmd]
			notifier.message -q This is a hidden message!
			notifier.message -q -i=14 This is a second hidden message with an icon
			[cmd]notifier.show[/cmd]
			notifier.message -w This message has the current window associated
			notifier.message -w=[fnc]$window[/fnc] This is equivalent to the above
			notifier.message &lt;b&gt;Bold text&lt;/b&gt; and normal text
			[cmd]notifier.hide[/cmd]
			notifier.message -t=10 This message will be shown only for 10 seconds
		[/example]
*/

static bool notifier_kvs_cmd_message(KviKvsModuleCommandCall * c)
{
	QString szMessage;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("message", KVS_PT_STRING, 0, szMessage)
	KVSM_PARAMETERS_END(c)

	if(!g_pNotifierWindow)
		g_pNotifierWindow = new NotifierWindow();

	QString szIco = "";
	QString szWnd = "";

	KviWindow * pWnd = c->window();

	if(c->hasSwitch('w', "window_id"))
	{
		c->switches()->getAsStringIfExisting('w', "window_id", szWnd);
		if(!szWnd.isEmpty())
		{
			pWnd = g_pApp->findWindow(szWnd);
			if(!pWnd)
				c->warning(__tr2qs_ctx("The specified window does not exist", "notifier"));
		}
	}
	c->switches()->getAsStringIfExisting('i', "icon", szIco);
	kvs_int_t uTime = KVI_OPTION_UINT(KviOption_uintNotifierAutoHideTime);
	if(c->hasSwitch('t', "timeout"))
	{
		KviKvsVariant * pTime = c->getSwitch('t', "timeout");
		if(pTime)
		{
			bool bOk = pTime->asInteger(uTime);
			if(!bOk)
			{
				uTime = 0;
				c->warning(__tr2qs_ctx("The specified timeout is not valid, assuming 0", "notifier"));
			}
		}
		else
		{
			c->warning(__tr2qs_ctx("The -t switch expects a timeout in seconds", "notifier"));
		}
	}

	g_pNotifierWindow->addMessage(pWnd, szIco, szMessage, uTime);

	if(!c->hasSwitch('q', "quiet"))
		g_pNotifierWindow->doShow(!(c->hasSwitch('n', "new")));
	return true;
}

/*
	@doc: notifier.hide
	@type:
		command
	@title:
		notifier.hide
	@short:
		Hides the notifier window
	@syntax:
		notifier.hide [-n|--noanim]
	@switches:
		!sw: -n
		Causes the hide operation is not animated.
	@description:
		Hide the notifier window
		[b]Please note that the user can forcibly disable the notifier
		for a limited period of time (a sort of [i]don't bug me[/i] option).[/b]
	@seealso:
		[cmd]notifier.show[/cmd] [cmd]notifier.message[/cmd] [fnc]$notifier.isEnabled[/fnc]
*/

static bool notifier_kvs_cmd_hide(KviKvsModuleCommandCall * c)
{
	if(g_pNotifierWindow)
		g_pNotifierWindow->doHide(!(c->hasSwitch('n', "notanimated")));
	return true;
}

/*
	@doc: notifier.show
	@type:
		command
	@title:
		notifier.show
	@short:
		Shows the notifier window
	@syntax:
		notifier.show [-n|--noanim]
	@switches:
		!sw: -n
		Disables the animation
	@description:
		Shows the notifier window if it is not already visible
		If the -n switch is present then the show operation is
		not animated.[br]
		The notifier is shown [b]only[/b] if it contains some messages.
		[b]Please note that the user can forcibly disable the notifier
		for a limited period of time (a sort of [i]don't bug me[/i] option).[/b]
	@seealso:
		[cmd]notifier.hide[/cmd] [cmd]notifier.message[/cmd] [fnc]$notifier.isEnabled[/fnc]
*/

static bool notifier_kvs_cmd_show(KviKvsModuleCommandCall * c)
{
	if(!g_pNotifierWindow)
		return true;
	if(!g_pNotifierWindow->countTabs())
		return true;

	g_pNotifierWindow->setDisableHideOnMainWindowGotAttention(true);
	g_pNotifierWindow->doShow(!(c->hasSwitch('n', "noanim")));

	return true;
}

/*
	@doc: notifier.isEnabled
	@type:
		function
	@title:
		$notifier.isEnabled
	@short:
		Returns [b]1[/b] if the notifier window is enabled
	@syntax:
		<boolean> $notifier.isEnabled
	@description:
		Returns [b]1[/b] if the notifier window is enabled and [b]0[/b] otherwise.
		The user can forcibly disable the notifier as a sort of [i]don't bug me[/i]
		feature for a limited period of time. When the notifier
		is disabled the messages sent to it will not be shown.[br]
		The only method that you (the scripter) can use to forcibly
		re-enable the notifier is to unload the module and
		reload it, but [b]don't do it[/b] :)[br]
		There is also a global option that allows forcibly disabling
		the notifier forever, this option could be overridden with [cmd]option[/cmd]
		instead, but again [b]don't do it[/b] :)[br]

*/

static bool notifier_kvs_fnc_isEnabled(KviKvsModuleFunctionCall * c)
{
	bool bCheck = false;
	if(KVI_OPTION_BOOL(KviOption_boolEnableNotifier))
		bCheck = g_tNotifierDisabledUntil < kvi_unixTime();
	c->returnValue()->setBoolean(bCheck);
	return true;
}

static bool notifier_module_init(KviModule * m)
{
	KVSM_REGISTER_SIMPLE_COMMAND(m, "message", notifier_kvs_cmd_message);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "show", notifier_kvs_cmd_show);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "hide", notifier_kvs_cmd_hide);
	KVSM_REGISTER_FUNCTION(m, "isEnabled", notifier_kvs_fnc_isEnabled);

	return true;
}

static bool notifier_module_cleanup(KviModule *)
{
	if(g_pNotifierWindow)
	{
		delete g_pNotifierWindow;
		g_pNotifierWindow = nullptr;
	}
	return true;
}

static bool notifier_module_can_unload(KviModule *)
{
	return (!g_pNotifierWindow);
}

static bool notifier_module_ctrl(KviModule *, const char * pcOperation, void * pParam)
{
	if(!kvi_strEqualCI("notifier::message", pcOperation))
		return false;

	KviNotifierMessageParam * p = (KviNotifierMessageParam *)pParam;
	if(!p)
		return false;

	if(!g_pNotifierWindow)
		g_pNotifierWindow = new NotifierWindow();

	g_pNotifierWindow->addMessage(p->pWindow, p->szIcon, p->szMessage, p->uMessageLifetime);
	g_pNotifierWindow->doShow(KVI_OPTION_BOOL(KviOption_boolNotifierFading) ? true : false);

	return true;
}

KVIRC_MODULE(
    "Notifier",
    "4.0.0",
    "Copyright (C) 2005 Iacopo Palazzi (iakko at siena dot linux dot it)\n"
    "              2010 Elvio Basello (hell at hellvis69 dot netsons dot org)",
    "KVIrc Client - Taskbar Notifier",
    notifier_module_init,
    notifier_module_can_unload,
    notifier_module_ctrl,
    notifier_module_cleanup,
    "notifier")
