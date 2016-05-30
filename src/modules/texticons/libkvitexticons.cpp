//=============================================================================
//
//   File : libkvitexticons.cpp
//   Creation date : Wed May 10 14:00:12 2006 GMT by Alexey Uzhva
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2006-2008 Alexey Uzhva (wizard at opendoor dot ru)
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
#include "KviIconManager.h"
#include "KviTextIconManager.h"

/*
	@doc: texticons.get
	@type:
		function
	@title:
		$texticons.get
	@short:
		Retreives texticon data
	@syntax:
		<hash> $texticons.get()
		<string>  $texticons.get(<iconName:string>)
		<integer> $texticons.get(<iconName:string>)
	@description:
		if <iconName:string> is set returns integer icon ID or string filename, associated with <iconName:string>[br]
		if <iconName:string> isn't set returns hash where keys are the icon names, and values are the icon id's or filenames
	@seealso:
		[fnc]$texticons.get[/fnc]
*/

static bool texticons_kvs_fnc_get(KviKvsModuleFunctionCall * c)
{
	QString szIcon;
	KviTextIcon * pIcon = nullptr;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("iconName", KVS_PT_NONEMPTYSTRING, KVS_PF_OPTIONAL, szIcon)
	KVSM_PARAMETERS_END(c)
	if(!szIcon.isNull())
	{
		pIcon = g_pTextIconManager->lookupTextIcon(szIcon);
		if(!pIcon)
		{
			c->warning("Icon '%s' not found", szIcon.toUtf8().data());
		}
		else
		{
			if(pIcon->id() != KviIconManager::None)
				c->returnValue()->setInteger(pIcon->id());
			else
				c->returnValue()->setString(pIcon->filename());
		}
	}
	else
	{
		KviKvsHash * pHash = new KviKvsHash();

		KviPointerHashTableIterator<QString, KviTextIcon> it(*(g_pTextIconManager->textIconDict()));

		while(KviTextIcon * i = it.current())
		{
			if(i->id() != KviIconManager::None)
				pHash->set(it.currentKey(), new KviKvsVariant((kvs_int_t)(i->id())));
			else
				pHash->set(it.currentKey(), new KviKvsVariant(i->filename()));
			++it;
		}
		c->returnValue()->setHash(pHash);
	}
	return true;
}

/*
	@doc: texticons.set
	@type:
		command
	@title:
		texticons.set
	@short:
		Sets texticon data
	@syntax:
		texticons.set <iconName:string>
		texticons.set <iconName:string> <iconFile:string>
		texticons.set <iconName:string> <iconId:integer>
	@description:
		if command gets only 1 argument it removes <iconName:string> from list of texticons[br]
		if it gets an integer second parameters, command associates <iconName:string> icon with builtin icon, having ID <iconId:integer>[br]
		if it gets a string second parameters, command associates <iconName:string> icon with the filename <iconFile:string>
	@seealso:
		[fnc]$texticons.get[/fnc]
*/

static bool texticons_kvs_cmd_set(KviKvsModuleCommandCall * c)
{
	QString szName, szIcon;
	KviTextIcon * pIcon = nullptr;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("iconName", KVS_PT_NONEMPTYSTRING, 0, szName)
	KVSM_PARAMETER("iconIdOrFile", KVS_PT_STRING, KVS_PF_OPTIONAL, szIcon)
	KVSM_PARAMETERS_END(c)
	if(szIcon.isNull())
	{
		g_pTextIconManager->textIconDict()->remove(szName);
	}
	else
	{
		pIcon = g_pTextIconManager->lookupTextIcon(szName);
		if(!pIcon)
		{
			KviTextIcon * pTmpIcon = new KviTextIcon(KviIconManager::None);
			g_pTextIconManager->insert(szName, *pTmpIcon);
			delete pTmpIcon;
			pIcon = g_pTextIconManager->lookupTextIcon(szName);
			if(!pIcon)
				return false;
		}

		bool bOk;
		unsigned int uResult = szIcon.toUInt(&bOk);
		if(bOk)
			pIcon->setId(uResult);
		else
			pIcon->setFilename(szIcon);
	}
	return true;
}

static bool texticons_module_init(KviModule * m)
{
	KVSM_REGISTER_SIMPLE_COMMAND(m, "set", texticons_kvs_cmd_set);

	KVSM_REGISTER_FUNCTION(m, "get", texticons_kvs_fnc_get);
	return true;
}

static bool texticons_module_cleanup(KviModule *)
{
	return true;
}

KVIRC_MODULE(
    "Texticons", // module name
    "4.0.0",     // module version
    "Copyright (C) 2006 Alexey Uzhva (wizard at opendoor dot ru)",
    "Texticons handling functions module",
    texticons_module_init,
    0,
    0,
    texticons_module_cleanup,
    0)

/*
	@doc: texticons
	@type:
		generic
	@title:
		The KVIrc TextIcons extension
	@short:
		The KVIrc TextIcons extension
	@body:
		Starting from version 3.0.0 KVIrc supports the TextIcon extension
		to the standard IRC protocol. It is a mean for sending text enriched
		of small images without sending the images themselves.[br]
		The idea is quite simple: the IRC client (and it's user) associates
		some small images to text strings (called icon tokens) and the strings are sent
		in place of the images preceded by a special escape character.[br]
		The chosen escape character is 29 (hex 0x1c) which corresponds
		to the ASCII group separator.[br]
		So for example if a client has the association of the icon token "rose" with a small
		icon containing a red rose flower then KVIrc could send the string
		"&lt;0x1c&gt;rose" in the message stream to ask the remote parties to
		display such an icon. If the remote parties don't have this association
		then they will simply strip the control code and display the string "rose",
		(eventually showing it in some enhanced way).[br]
		The icon tokens can't contain spaces
		so the receiving clients stop the extraction of the icon strings
		when a space, an icon escape or the message termination is encountered.
		[br]
		&lt;icon escape&gt; := character 0x1c (ASCII group separator)[br]
		&lt;icon token&gt; := any character with the exception of 0x1c, CR,LF and SPACE.[br]
		[br]
		Please note that this is a KVIrc extension and the remote clients
		that don't support this feature will not display the icon (and will
		eventually show the 0x1c character in the data stream).[br]
		If you like this feature please either convince the remote users
		to try KVIrc or tell them to write to their client developers asking
		for this simple feature to be implemented.[br]
*/
