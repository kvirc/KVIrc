//=============================================================================
//
//   File : libkviregchan.cpp
//   Creation date : Sat Jun 29 02:55:18 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

// incomplete
#include "KviModule.h"
#include "KviRegisteredChannelDataBase.h"
#include "KviLocale.h"
#include "kvi_out.h"
#include "KviControlCodes.h"
#include "KviWindow.h"
#include "kvi_debug.h"

// KviApplication.cpp
extern KVIRC_API KviRegisteredChannelDataBase * g_pRegisteredChannelDataBase;

/*
	@doc: regchan.add
	@type:
		command
	@title:
		regchan.add
	@keyterms:
		registering channels
	@short:
		Registers a channel
	@syntax:
		regchan.add <channel:string> [netmask:string]
	@description:
		Registers the <channel> with the specified [netmask].
		[netmask] can be a full network name or a wildcard string
		that will be matched against network names.
		For example, if you want to register #kvirc on IRCNet
		you can use regchan.add #kvirc IRCNet.
		If you want to handle #kvirc on all the IRC networks then
		you can use regchan.add #kvirc *[br]
		Obviously if you register both #kvirc/IRCNet and #kvirc/ *
		then #kvirc/IRCNet will be looked first.
		If netmask is not passed a netmask of * is assumed.[br]
	@examples:
		[example]
			regchan.add #kvirc openprojects
		[/example]
	@seealso:
*/

static bool regchan_kvs_cmd_add(KviKvsModuleCommandCall * c)
{
	QString szChan, szNetmask;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("channel name", KVS_PT_NONEMPTYSTRING, 0, szChan)
	KVSM_PARAMETER("netmask", KVS_PT_STRING, KVS_PF_OPTIONAL, szNetmask)
	KVSM_PARAMETERS_END(c)
	if(szNetmask.isEmpty())
		szNetmask = "*";
	g_pRegisteredChannelDataBase->add(new KviRegisteredChannel(szChan, szNetmask));
	return true;
}

/*
	@doc: regchan.remove
	@type:
		command
	@title:
		regchan.remove
	@keyterms:
		unregistering channels
	@short:
		Unregisters a channel
	@syntax:
		regchan.remove [-q|--quiet] [-e|--exactly] <channel:string> <network:string>
	@description:
		Unregisters the channel named <channel> that best matches <network>
		with its netmask.[br]
		If no match for <channel>/<network> is found in the database then
		a warning is printed unless -q is used.[br]
		If -e is specified then <network> is not matched against the
		netmasks in the database but is compared directly. This means
		that <network> must be exactly the netmask used to register a channel.
	@examples:
		[example]
			regchan.remove #kvirc openprojects
		[/example]
	@seealso:
		[cmd]regchan.add[/cmd]
*/
static bool regchan_kvs_cmd_remove(KviKvsModuleCommandCall * c)
{
	QString szChan, szNetwork;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("channel name", KVS_PT_NONEMPTYSTRING, 0, szChan)
	KVSM_PARAMETER("network", KVS_PT_NONEMPTYSTRING, 0, szNetwork)
	KVSM_PARAMETERS_END(c)
	KviRegisteredChannel * ch;
	if(c->hasSwitch('e', "exactly"))
		ch = g_pRegisteredChannelDataBase->findExact(szChan, szNetwork);
	else
		ch = g_pRegisteredChannelDataBase->find(szChan, szNetwork);
	if(ch)
	{
		g_pRegisteredChannelDataBase->remove(ch);
	}
	else
	{
		if(!c->hasSwitch('q', "quiet"))
			c->warning(__tr2qs_ctx("No such channel/netmask entry in the database", "register"));
	}
	return true;
}

/*
	@doc: regchan.setproperty
	@type:
		command
	@title:
		regchan.setproperty
	@short:
		Sets a property for a registered channel
	@syntax:
		regchan.setproperty [-e|--exactly] [-q|--quiet] <channel:string> <network:string> <property:string> <value:string>
	@description:
		Sets the <property> for the registered <channel> that best
		matches <network> to <value>.[br]
		If <value> is an empty string the <property> is removed.[br]
		If no match for <channel>/<network> is found in the database then
		a warning is printed unless -q is used.[br]
	@switches:
		!fn: -q|--quiet
		No warning on screen.
		!fn: -e|--exactly
		If -e is specified then <network> is not matched against the
		netmasks in the database but is compared directly. This means
		that <network> must be exactly the netmask used to register a channel.
	@examples:
		[example]
			regchan.setproperty #kvirc openprojects autojoin true
		[/example]
	@seealso:
		[cmd]regchan.add[/cmd]
*/

static bool regchan_kvs_cmd_setProperty(KviKvsModuleCommandCall * c)
{
	QString szChan, szNetwork, szProperty, szValue;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("channel name", KVS_PT_NONEMPTYSTRING, 0, szChan)
	KVSM_PARAMETER("network", KVS_PT_NONEMPTYSTRING, 0, szNetwork)
	KVSM_PARAMETER("property name", KVS_PT_NONEMPTYSTRING, 0, szProperty)
	KVSM_PARAMETER("value", KVS_PT_STRING, 0, szValue)
	KVSM_PARAMETERS_END(c)
	KviRegisteredChannel * ch;
	if(c->hasSwitch('e', "exactly"))
		ch = g_pRegisteredChannelDataBase->findExact(szChan, szNetwork);
	else
		ch = g_pRegisteredChannelDataBase->find(szChan, szNetwork);
	if(ch)
	{
		if(!szValue.isEmpty())
		{
			ch->setProperty(szProperty, szValue);
		}
		else
		{
			ch->removeProperty(szProperty);
		}
	}
	else
	{
		if(!c->hasSwitch('q', "quiet"))
			c->warning(__tr2qs_ctx("No such channel/netmask entry in the database", "register"));
	}
	return true;
}
/*
	@doc: regchan.showlist
	@type:
		command
	@title:
		regchan.showlist
	@short:
		Lists the registered channels
	@syntax:
		regchan.showlist
	@description:
		Lists the registered channels with their description
*/

static bool regchan_kvs_cmd_showlist(KviKvsModuleCommandCall * c)
{
	c->window()->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs_ctx("Registered channel list:", "register"));

	int tot = 0;

	QHash<QString, KviRegisteredChannelList *>::Iterator it;
	for(it = g_pRegisteredChannelDataBase->channelDict()->begin(); it != g_pRegisteredChannelDataBase->channelDict()->end(); ++it)
	{
		KviRegisteredChannelList * l = it.value();
		KVI_ASSERT(l);

		for(KviRegisteredChannel * ch = l->first(); ch; ch = l->next())
		{
			tot++;
			c->window()->outputNoFmt(
			    KVI_OUT_SYSTEMMESSAGE,
			    __tr2qs_ctx("Channel: %1%2@%3", "register")
			        .arg(QChar(KviControlCodes::Bold))
			        .arg(ch->name())
			        .arg(ch->netMask()));

			for(QHash<QString, QString>::Iterator it2 = ch->propertyDict()->begin(); it2 != ch->propertyDict()->end(); ++it2)
			{
				c->window()->outputNoFmt(
				    KVI_OUT_SYSTEMMESSAGE,
				    __tr2qs_ctx("    Property: %1=%2", "register")
				        .arg(it2.key())
				        .arg(it2.value()));
			}
		}
	}

	c->window()->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs_ctx("Total %d channels", "register"), tot);
	return true;
}

/*
	@doc: regchan.list
	@type:
		function
	@title:
		$regchan.list
	@short:
		Get a list of registered channels
	@syntax:
		<array> $regchan.list([<channel_mask:string>],[<network_mask:string>])
	@description:
		Returns an array of the channel@netmask entries in the registered
		channels database.[br]
		Please note that <network_mask> is a wildcard string that will match
		wildcard strings.[br]
	@examples:
		[example]
			echo $regchan.list(#kvirc*,[fnc]$my.network[/fnc])
		[/example]
*/

static bool regchan_kvs_fnc_list(KviKvsModuleFunctionCall * c)
{
	QString szChan, szNetmask;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("channel name", KVS_PT_STRING, KVS_PF_OPTIONAL, szChan)
	KVSM_PARAMETER("netmask", KVS_PT_STRING, KVS_PF_OPTIONAL, szNetmask)
	KVSM_PARAMETERS_END(c)

	KviKvsArray * pArray = new KviKvsArray();

	int aid = 0;

	if(szChan.isEmpty())
		szChan = "*";
	if(szNetmask.isEmpty())
		szNetmask = "*";

	QHash<QString, KviRegisteredChannelList *>::Iterator it;
	for(it = g_pRegisteredChannelDataBase->channelDict()->begin(); it != g_pRegisteredChannelDataBase->channelDict()->end(); ++it)
	{
		KviRegisteredChannelList * l = it.value();
		KVI_ASSERT(l);

		for(KviRegisteredChannel * ch = l->first(); ch; ch = l->next())
		{
			if(KviQString::matchWildExpressions(ch->name(), szChan) && KviQString::matchWildExpressions(ch->netMask(), szNetmask))
			{
				pArray->set(aid, new KviKvsVariant(QString::fromUtf8("%1@%2").arg(ch->name(), ch->netMask())));
				aid++;
			}
		}
	}
	c->returnValue()->setArray(pArray);
	return true;
}

/*
	@doc: regchan.property
	@type:
		function
	@title:
		$regchan.property
	@short:
		Returns a property of an entry in the registered channels database
	@syntax:
		<string> $regchan.property(<channel_name:string>,<network:string>,<property_name:string>)
	@description:
		Returns the value of the property identified by <property_name> and
		bound to the channel identified by <channel_name> and best
		matched by <network>.[br]
		If the property is not set or if there is no matching channel entry an empty string is returned.[br]
	@examples:
		[example]
			echo $regchan.property(#kvirc,[fnc]$my.network[/fnc],autojoin)
		[/example]
*/

static bool regchan_kvs_fnc_property(KviKvsModuleFunctionCall * c)
{
	QString szChan, szNetwork, szPropertyName;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("channel name", KVS_PT_STRING, 0, szChan)
	KVSM_PARAMETER("network", KVS_PT_STRING, 0, szNetwork)
	KVSM_PARAMETER("property name", KVS_PT_NONEMPTYSTRING, 0, szPropertyName)
	KVSM_PARAMETERS_END(c)
	KviRegisteredChannel * ch = g_pRegisteredChannelDataBase->find(szChan, szNetwork);
	if(ch)
	{
		QString szProp = ch->property(szPropertyName);
		if(!szProp.isEmpty())
			c->returnValue()->setString(szProp);
	}
	return true;
}
/*
	@doc: regchan.match
	@type:
		function
	@title:
		$regchan.match
	@short:
		Checks if a channel is registered
	@syntax:
		<boolean> $regchan.match(<channel_name:string>,<network:string>)
	@description:
		Returns 1 if <channel_name> and <network> match a registered channel entry
		and [b]0[/b] otherwise.
	@examples:
		[example]
			echo $regchan.match(#kvirc,IRCNet)
			echo $regchan.match(#kvirc,$my.network)
		[/example]
	@seealso:
		[fnc]$regchan.find[/fnc]
*/

static bool regchan_kvs_fnc_match(KviKvsModuleFunctionCall * c)
{
	QString szChan, szNetwork;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("channel name", KVS_PT_STRING, 0, szChan)
	KVSM_PARAMETER("network", KVS_PT_STRING, 0, szNetwork)
	KVSM_PARAMETERS_END(c)
	KviRegisteredChannel * ch = g_pRegisteredChannelDataBase->find(szChan, szNetwork);
	c->returnValue()->setBoolean(ch);
	return true;
}

/*
	@doc: regchan.find
	@type:
		function
	@title:
		$regchan.find
	@short:
		Checks if a channel is registered
	@syntax:
		<boolean> $regchan.find(<channel_name:string>,<netmask:string>)
	@description:
		Returns [b]1[/b] if the channel identified by <channel_name> and <netmask>
		is registered and [b]0[/b] otherwise. This function is similar to $regchan.match
		but for $regchan.find the second parameter is the exact netmask
		of the registered channel entry.
	@examples:
		[example]
			echo $regchan.find(#kvirc,IRCNet)
			echo $regchan.find(#kvirc,*)
		[/example]
	@seealso:
		[fnc]$regchan.match[/fnc]
*/

static bool regchan_kvs_fnc_find(KviKvsModuleFunctionCall * c)
{
	QString szChan, szNetmask;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("channel name", KVS_PT_STRING, 0, szChan)
	KVSM_PARAMETER("netmask", KVS_PT_STRING, 0, szNetmask)
	KVSM_PARAMETERS_END(c)
	KviRegisteredChannel * ch = g_pRegisteredChannelDataBase->find(szChan, szNetmask);
	c->returnValue()->setBoolean(ch);
	return true;
}

static bool regchan_module_init(KviModule * m)
{
	KVSM_REGISTER_SIMPLE_COMMAND(m, "add", regchan_kvs_cmd_add);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "showlist", regchan_kvs_cmd_showlist);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "remove", regchan_kvs_cmd_remove);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "setProperty", regchan_kvs_cmd_setProperty);
	KVSM_REGISTER_FUNCTION(m, "property", regchan_kvs_fnc_property);
	KVSM_REGISTER_FUNCTION(m, "match", regchan_kvs_fnc_match);
	KVSM_REGISTER_FUNCTION(m, "find", regchan_kvs_fnc_find);
	KVSM_REGISTER_FUNCTION(m, "list", regchan_kvs_fnc_list);

	return true;
}

static bool regchan_module_cleanup(KviModule *)
{
	return true;
}

static bool regchan_module_can_unload(KviModule *)
{
	return true;
}

KVIRC_MODULE(
    "RegChan",                                                      // module name
    "4.0.0",                                                        // module version
    "Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
    "Script interface to the registered channels database",
    regchan_module_init,
    regchan_module_can_unload,
    0,
    regchan_module_cleanup,
    "register")
