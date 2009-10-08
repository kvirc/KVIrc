//=============================================================================
//
//   File : libkvitexticons.cpp
//   Creation date : Wed May 10 14:00:12 2006 GMT by Alexey Uzhva
//
//   This math is part of the KVirc irc client distribution
//   Copyright (C) 2006-2008 Alexey Uzhva (wizard at opendoor dot ru)
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

#include "kvi_module.h"
#include "kvi_texticonmanager.h"

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
		if <iconName:string> is setted returns integer icon id or string filename, associated with <iconName:string>[br]
		if <iconName:string> isn't set returns hash where keys are the icon names, and values are the icon id's or filenames
	@seealso:
		[fnc]$texticons.get[/fnc]
*/

static bool texticons_kvs_fnc_get(KviKvsModuleFunctionCall * c)
{
	QString szIcon;
	KviTextIcon* pIcon=0;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("iconName",KVS_PT_NONEMPTYSTRING,KVS_PF_OPTIONAL,szIcon)
	KVSM_PARAMETERS_END(c)
	if(!szIcon.isNull())
	{
		pIcon=g_pTextIconManager->lookupTextIcon(szIcon);
		if(!pIcon)
		{
			c->warning("Icon '%s' not found",szIcon.toUtf8().data());
		} else {
			if(pIcon->id()!=-1)
				c->returnValue()->setInteger(pIcon->id());
			else
				c->returnValue()->setString(pIcon->filename());
		}
	} else {
		KviKvsHash* hash = new KviKvsHash();

		KviPointerHashTableIterator<QString,KviTextIcon> it(*(g_pTextIconManager->textIconDict()));

		while(KviTextIcon * i = it.current())
		{
			if(i->id()!=-1)
				hash->set(it.currentKey(),new KviKvsVariant( (kvs_int_t)(i->id()) ));
			else
				hash->set(it.currentKey(),new KviKvsVariant(i->filename()));
			++it;
		}
		c->returnValue()->setHash(hash);
	}
	return true;
}

/*
	@doc: texticons.set
	@type:
		command
	@title:
		$texticons.set
	@short:
		Sets texticon data
	@syntax:
		texticons.set <iconName:string>
		texticons.set <iconName:string> <iconFile:string>
		texticons.set <iconName:string> <iconId:integer>
	@description:
		if command gets only 1 argument it removes <iconName:string> from list of texticons[br]
		if it gets an integer second parameters, command associates <iconName:string> icon with builin icon, having id <iconId:integer>[br]
		if it gets a string second parameters, command associates <iconName:string> icon with the filename <iconFile:string>
	@seealso:
		[fnc]$texticons.get[/fnc]
*/

static bool texticons_kvs_cmd_set(KviKvsModuleCommandCall * c)
{
	QString szName,szIcon;
	KviTextIcon* pIcon=0;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("iconName",KVS_PT_NONEMPTYSTRING,0,szName)
		KVSM_PARAMETER("iconIdOrFile",KVS_PT_STRING,KVS_PF_OPTIONAL,szIcon)
	KVSM_PARAMETERS_END(c)
	if(szIcon.isNull())
	{
		g_pTextIconManager->textIconDict()->remove(szName);
	} else {
		pIcon=g_pTextIconManager->lookupTextIcon(szName);
		if(!pIcon)
		{
			KviTextIcon* pTmpIcon=new KviTextIcon(-1);
			g_pTextIconManager->insert(szName,*pTmpIcon);
			delete pTmpIcon;
			pIcon=g_pTextIconManager->lookupTextIcon(szName);
			if(!pIcon) return false;
		}

		bool bOk;
		unsigned int iResult=szIcon.toUInt(&bOk);
		if(bOk)
			pIcon->setId(iResult);
		else
			pIcon->setFilename(szIcon);

	}
	g_pTextIconManager->checkDefaultAssociations();
	return true;
}

static bool texticons_module_init(KviModule * m)
{
	KVSM_REGISTER_SIMPLE_COMMAND(m,"set",texticons_kvs_cmd_set);

	KVSM_REGISTER_FUNCTION(m,"get",texticons_kvs_fnc_get);
	return true;
}

static bool texticons_module_cleanup(KviModule *)
{
	return true;
}

KVIRC_MODULE(
	"Texticons",                                                 // module name
	"4.0.0",                                                // module version
	"Copyright (C) 2006 Alexey Uzhva (wizard at opendoor dot ru)",
	"Texticons handling functions module",
	texticons_module_init,
	0,
	0,
	texticons_module_cleanup
)
