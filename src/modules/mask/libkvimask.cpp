//=============================================================================
//
//   File : libkvimask.cpp
//   Creation date : Thu Dec 27 2001 17:13:12 GMT by Szymon Stefanek
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

#include "KviModule.h"
#include "KviLocale.h"
#include "KviCString.h"
#include "KviIrcMask.h"

/*
	@doc: mask.match
	@type:
		function
	@title:
		$mask.match
	@short:
		Matches a mask against a wildcarded one
	@syntax:
		<boolean> $mask.match(<wildcard_mask:string>,<fixed_mask:string>)
	@description:
		Returns [b]1[/b] if the <wildcard_mask> matches <fixed_mask> and [b]0[/b] otherwise.
		<wildcard_mask> can obviously contain wildcards ([b]*[/b] and [b]?[/b]).
	@example:
		[example]
			[cmd]if[/cmd]($mask.match(*!*@*.linux.it,$0))
			[cmd]op[/cmd] [fnc]$mask.nick[/fnc]($0)
		[/example]
*/

static bool mask_kvs_fnc_match(KviKvsModuleFunctionCall * c)
{
	QString wildmask, fixedmask;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("wildcard mask", KVS_PT_STRING, 0, wildmask)
	KVSM_PARAMETER("fixed mask", KVS_PT_STRING, 0, fixedmask)
	KVSM_PARAMETERS_END(c)
	KviIrcMask mk1(wildmask);
	KviIrcMask mk2(fixedmask);
	c->returnValue()->setBoolean(mk1.matchesFixed(mk2.nick(), mk2.user(), mk2.host()));
	return true;
}

/*
	@doc: mask.nick
	@type:
		function
	@title:
		$mask.nick
	@short:
		Returns the nick part of an IRC mask
	@syntax:
		<string> $mask.nick(<mask:string>)
	@description:
		Returns the nickname part of an IRC mask.
*/

static bool mask_kvs_fnc_nick(KviKvsModuleFunctionCall * c)
{
	QString mask;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("mask", KVS_PT_STRING, 0, mask)
	KVSM_PARAMETERS_END(c)
	KviIrcMask mk(mask);
	c->returnValue()->setString(mk.nick());
	return true;
}

/*
	@doc: mask.user
	@type:
		function
	@title:
		$mask.user
	@short:
		Returns the username part of an IRC mask
	@syntax:
		<string> $mask.user(<mask:string>)
	@description:
		Returns the username part of an IRC mask.
*/

static bool mask_kvs_fnc_user(KviKvsModuleFunctionCall * c)
{
	QString mask;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("mask", KVS_PT_STRING, 0, mask)
	KVSM_PARAMETERS_END(c)
	KviIrcMask mk(mask);
	c->returnValue()->setString(mk.user());
	return true;
}

/*
	@doc: mask.host
	@type:
		function
	@title:
		$mask.host
	@short:
		Returns the hostname part of an IRC mask
	@syntax:
		<string> $mask.host(<mask:string>)
	@description:
		Returns the hostname part of an IRC mask.
*/

static bool mask_kvs_fnc_host(KviKvsModuleFunctionCall * c)
{
	QString mask;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("mask", KVS_PT_STRING, 0, mask)
	KVSM_PARAMETERS_END(c)
	KviIrcMask mk(mask);
	c->returnValue()->setString(mk.host());
	return true;
}

/*
	@doc: mask.hasnumerichost
	@type:
		function
	@title:
		$mask.hasNumericHost
	@short:
		Checks if a host part of a mask is numeric
	@syntax:
		<boolean> $mask.hasNumericHost(<mask: string>)
	@description:
		Returns [b]1[/b] if the hostname part of the mask is numeric (e.g. unresolved IPv4 or IPv6 address).
*/

static bool mask_kvs_fnc_hasnumerichost(KviKvsModuleFunctionCall * c)
{
	QString mask;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("mask", KVS_PT_STRING, 0, mask)
	KVSM_PARAMETERS_END(c)
	KviIrcMask mk(mask);
	c->returnValue()->setBoolean(mk.hasNumericHost());
	return true;
}

//
// Module stuff
//

static bool mask_module_init(KviModule * m)
{
	KVSM_REGISTER_FUNCTION(m, "match", mask_kvs_fnc_match);
	KVSM_REGISTER_FUNCTION(m, "nick", mask_kvs_fnc_nick);
	KVSM_REGISTER_FUNCTION(m, "user", mask_kvs_fnc_user);
	KVSM_REGISTER_FUNCTION(m, "host", mask_kvs_fnc_host);
	KVSM_REGISTER_FUNCTION(m, "hasnumerichost", mask_kvs_fnc_hasnumerichost);

	return true;
}

static bool mask_module_cleanup(KviModule *)
{
	return true;
}

KVIRC_MODULE(
    "File",  // module name
    "4.0.0", // module version
    "Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)",
    "Mask manipulation functions",
    mask_module_init,
    0,
    0,
    mask_module_cleanup,
    0)
