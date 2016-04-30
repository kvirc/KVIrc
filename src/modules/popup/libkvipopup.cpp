//=============================================================================
//
//   File : libkvipopup.cpp
//   Creation date : Fri Sep 03 2010 15:23:00 CEST by Fabio Bas
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
#include "KviKvsPopupManager.h"

#define GET_KVS_POPUP(LOCKING_CHECK)                                                                                        \
	int iIdx = szPopupName.indexOf(QChar('.'));                                                                             \
	if(iIdx == 0)                                                                                                           \
	{                                                                                                                       \
		if(!c->hasSwitch('q', "quiet"))                                                                                     \
			c->warning(__tr2qs_ctx("Empty popup name supplied", "kvs"));                                                    \
		return true;                                                                                                        \
	}                                                                                                                       \
	if(iIdx > 0)                                                                                                            \
	{                                                                                                                       \
		if(iIdx == szPopupName.size())                                                                                      \
		{                                                                                                                   \
			if(!c->hasSwitch('q', "quiet"))                                                                                 \
				c->warning(__tr2qs_ctx("Empty subpopup name supplied", "kvs"));                                             \
			return true;                                                                                                    \
		}                                                                                                                   \
		szSubPopupName = szPopupName.mid(iIdx + 1);                                                                         \
		szPopupName.truncate(iIdx);                                                                                         \
	}                                                                                                                       \
	KviKvsPopupMenu * pPopup = KviKvsPopupManager::instance()->lookup(szPopupName);                                         \
	if(!pPopup)                                                                                                             \
	{                                                                                                                       \
		if(!c->hasSwitch('q', "quiet"))                                                                                     \
			c->warning(__tr2qs_ctx("Popup \"%Q\" doesn't exist", "kvs"), &szPopupName);                                     \
		return true;                                                                                                        \
	}                                                                                                                       \
	if(pPopup->LOCKING_CHECK())                                                                                             \
	{                                                                                                                       \
		if(!c->hasSwitch('q', "quiet"))                                                                                     \
			c->warning(__tr2qs_ctx("Popup menu self-modification is not allowed (the popup is probably open)", "kvs"));     \
		return true;                                                                                                        \
	}                                                                                                                       \
	while(!szSubPopupName.isEmpty())                                                                                        \
	{                                                                                                                       \
		iIdx = szSubPopupName.indexOf(QChar('.'));                                                                          \
		QString szTargetPopupName;                                                                                          \
		if(iIdx >= 0)                                                                                                       \
		{                                                                                                                   \
			szTargetPopupName = szSubPopupName.mid(0, iIdx);                                                                \
			szSubPopupName = szSubPopupName.mid(iIdx + 1);                                                                  \
		}                                                                                                                   \
		else                                                                                                                \
		{                                                                                                                   \
			szTargetPopupName = szSubPopupName;                                                                             \
			szSubPopupName = QString();                                                                                     \
		}                                                                                                                   \
		pPopup = pPopup->findChildPopupByName(szTargetPopupName);                                                           \
		if(!pPopup)                                                                                                         \
		{                                                                                                                   \
			if(!c->hasSwitch('q', "quiet"))                                                                                 \
				c->warning(__tr2qs_ctx("Popup \"%Q\" doesn't exist", "kvs"), &szTargetPopupName);                           \
			return true;                                                                                                    \
		}                                                                                                                   \
		if(pPopup->LOCKING_CHECK())                                                                                         \
		{                                                                                                                   \
			if(!c->hasSwitch('q', "quiet"))                                                                                 \
				c->warning(__tr2qs_ctx("Popup menu self-modification is not allowed (the popup is probably open)", "kvs")); \
			return true;                                                                                                    \
		}                                                                                                                   \
	}

#define GET_KVS_POPUP_FNC                                                                     \
	int iIdx = szPopupName.indexOf(QChar('.'));                                               \
	if(iIdx == 0)                                                                             \
	{                                                                                         \
		c->warning(__tr2qs_ctx("Empty popup name supplied", "kvs"));                          \
		return true;                                                                          \
	}                                                                                         \
	if(iIdx > 0)                                                                              \
	{                                                                                         \
		if(iIdx == szPopupName.size())                                                        \
		{                                                                                     \
			c->warning(__tr2qs_ctx("Empty subpopup name supplied", "kvs"));                   \
			return true;                                                                      \
		}                                                                                     \
		szSubPopupName = szPopupName.mid(iIdx + 1);                                           \
		szPopupName.truncate(iIdx);                                                           \
	}                                                                                         \
	KviKvsPopupMenu * pPopup = KviKvsPopupManager::instance()->lookup(szPopupName);           \
	if(!pPopup)                                                                               \
	{                                                                                         \
		c->warning(__tr2qs_ctx("Popup \"%Q\" doesn't exist", "kvs"), &szPopupName);           \
		return true;                                                                          \
	}                                                                                         \
	while(!szSubPopupName.isEmpty())                                                          \
	{                                                                                         \
		iIdx = szSubPopupName.indexOf(QChar('.'));                                            \
		QString szTargetPopupName;                                                            \
		if(iIdx >= 0)                                                                         \
		{                                                                                     \
			szTargetPopupName = szSubPopupName.mid(0, iIdx);                                  \
			szSubPopupName = szSubPopupName.mid(iIdx + 1);                                    \
		}                                                                                     \
		else                                                                                  \
		{                                                                                     \
			szTargetPopupName = szSubPopupName;                                               \
			szSubPopupName = QString();                                                       \
		}                                                                                     \
		pPopup = pPopup->findChildPopupByName(szSubPopupName);                                \
		if(!pPopup)                                                                           \
		{                                                                                     \
			c->warning(__tr2qs_ctx("Popup \"%Q\" doesn't exist", "kvs"), &szTargetPopupName); \
			return true;                                                                      \
		}                                                                                     \
	}

/*
	@doc: popup.addItem
	@type:
		command
	@title:
		popup.addItem
	@syntax:
		popup.addItem [-q] (<popupname:string>, <text:string>[, <icon:string>[, <item_id:string>[, <condition:string>]]])
		[{]
			<command>
		[}]
	@short:
		Adds an item to a popup
	@switches:
		!sw: -q | --quiet
		Run quietly: don't print warning and errors
	@description:
		Adds a menu item with visible <text> and the optional <icon> to an already existing popup or
		nested popup named <popupname>. If you want to add an item to a nested popup, use the form
		"popup.addItem", for example, "channeltextview.myNestedItem".[br]
		<text> is a string that is evaluated at [cmd]popup[/cmd] call time and may contain
		identifiers and variables.[br]
		<icon> is an optional [doc:image_id]image identifier[/doc].[br]
		<item_id> is the optional item ID - if not specified, it will be generated automatically.[br]
		If <condition> is given, it is evaluated at [cmd]popup.show[/cmd] call time and if the
		result is 0, the item is not shown in the physical popup.[br]
		If this item gets clicked, the code inside <command> will be executed.
		<command> can be a simple instruction or an instruction block delimited by curly brackets.[br]
	@seealso:
		[cmd]defpopup[/cmd], [cmd]popup.show[/cmd]
*/

static bool popup_kvs_cmd_addItem(KviKvsModuleCallbackCommandCall * c)
{
	QString szPopupName, szSubPopupName, szItemId, szText, szIcon, szCondition;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("popupname", KVS_PT_NONEMPTYSTRING, 0, szPopupName)
	KVSM_PARAMETER("text", KVS_PT_NONEMPTYSTRING, 0, szText)
	KVSM_PARAMETER("icon", KVS_PT_STRING, KVS_PF_OPTIONAL, szIcon)
	KVSM_PARAMETER("item_id", KVS_PT_STRING, KVS_PF_OPTIONAL, szItemId)
	KVSM_PARAMETER("condition", KVS_PT_STRING, KVS_PF_OPTIONAL, szCondition)
	KVSM_PARAMETERS_END(c)

	GET_KVS_POPUP(isHardLocked)

	if(c->callback()->code().trimmed().isEmpty() && !c->hasSwitch('q', "quiet"))
		c->warning(__tr2qs_ctx("Found empty instruction for popup item: maybe you need to fix the script?", "kvs"));

	pPopup->addItem(szItemId, c->callback()->code(), szText, szIcon, szCondition);

	KviKvsPopupManager::instance()->emitRefresh(szPopupName);
	return true;
}

/*
	@doc: popup.addPrologue
	@type:
		command
	@title:
		popup.addPrologue
	@syntax:
		popup.addPrologue [-q] (<popupname:string>[, <item_id:string>])
		[{]
			<command>
		[}]
	@short:
		Adds a prologue to a popup
	@switches:
		!sw: -q | --quiet
		Run quietly: don't print warning and errors
	@description:
		Adds a menu prologue to an already existing popup or nested popup named <popupname>. If you
		want to add an item to a nested popup, use the form "popup.addItem", for example,
		"channeltextview.myNestedItem".[br]
		<item_id> is the optional item ID - if not specified, it will be generated automatically.[br]
		<command> will be executed just before the popup is filled at [cmd]popup.show[/cmd] command call.
		<command> can be a simple instruction or an instruction block delimited by curly brackets.[br]
		Please note that some instructions are not allowed here, You can't delete the prologue's parent
		popup (calling [cmd]popup.clear[/cmd] or [cmd]popup.destroy[/cmd]) or modify prologues/epilogues
		(calling [cmd]popup.addPrologue[/cmd] or [cmd]popup.addEpilogue[/cmd])
	@seealso:
		[cmd]defpopup[/cmd], [cmd]popup.show[/cmd], [cmd]popup.addEpilogue[/cmd]
*/

static bool popup_kvs_cmd_addPrologue(KviKvsModuleCallbackCommandCall * c)
{
	QString szPopupName, szSubPopupName, szItemId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("popupname", KVS_PT_NONEMPTYSTRING, 0, szPopupName)
	KVSM_PARAMETER("item_id", KVS_PT_STRING, KVS_PF_OPTIONAL, szItemId)
	KVSM_PARAMETERS_END(c)

	GET_KVS_POPUP(isSoftLocked)

	if(c->callback()->code().trimmed().isEmpty() && !c->hasSwitch('q', "quiet"))
		c->warning(__tr2qs_ctx("Found empty prologue block: maybe you need to fix the script?", "kvs"));

	pPopup->addPrologue(szItemId, c->callback()->code());

	KviKvsPopupManager::instance()->emitRefresh(szPopupName);
	return true;
}

/*
	@doc: popup.addEpilogue
	@type:
		command
	@title:
		popup.addEpilogue
	@syntax:
		popup.addEpilogue [-q] (<popupname:string>[, <item_id:string>])
		[{]
			<command>
		[}]
	@short:
		Adds an epilogue to a popup
	@switches:
		!sw: -q | --quiet
		Run quietly: don't print warning and errors
	@description:
		Adds a menu epilogue to an already existing popup or nested popup named <popupname>. If you
		want to add an item to a nested popup, use the form "popup.addItem", for example,
		"channeltextview.myNestedItem".[br]
		<item_id> is the optional item ID - if not specified, it will be generated automatically.[br]
		<command> will be executed just after the popup is filled at [cmd]popup.show[/cmd] command call.
		<command> can be a simple instruction or an instruction block delimited by curly brackets.[br]
		Please note that some instructions are not allowed here, You can't delete the prologue's parent
		popup (calling [cmd]popup.clear[/cmd] or [cmd]popup.destroy[/cmd]) or modify prologues/epilogues
		(calling [cmd]popup.addPrologue[/cmd] or [cmd]popup.addEpilogue[/cmd])
	@seealso:
		[cmd]defpopup[/cmd], [cmd]popup.show[/cmd], [cmd]popup.addPrologue[/cmd]
*/

static bool popup_kvs_cmd_addEpilogue(KviKvsModuleCallbackCommandCall * c)
{
	QString szPopupName, szSubPopupName, szItemId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("popupname", KVS_PT_NONEMPTYSTRING, 0, szPopupName)
	KVSM_PARAMETER("item_id", KVS_PT_STRING, KVS_PF_OPTIONAL, szItemId)
	KVSM_PARAMETERS_END(c)

	GET_KVS_POPUP(isSoftLocked)

	if(c->callback()->code().trimmed().isEmpty() && !c->hasSwitch('q', "quiet"))
		c->warning(__tr2qs_ctx("Found empty epilogue block: maybe you need to fix the script?", "kvs"));

	pPopup->addEpilogue(szItemId, c->callback()->code());

	KviKvsPopupManager::instance()->emitRefresh(szPopupName);
	return true;
}

/*
	@doc: popup.addExtPopup
	@type:
		command
	@title:
		popup.addExtPopup
	@syntax:
		popup.addExtPopup [-q] <popupname:string> <external_popup:string> <text:string> [<icon:string> [<item_id:string> [<condition:string>]]]
	@short:
		Adds an external popup to a popup
	@switches:
		!sw: -q | --quiet
		Run quietly: don't print warning and errors
	@description:
		Adds a nested popup item with visible <text> and the optional <icon> to an already existing popup or
		nested popup named <popupname>. If you want to add an item to a nested popup, use the form
		"popup.addItem", for example, "channeltextview.myNestedItem".[br]
		<text> is a string that is evaluated at [cmd]popup[/cmd] call time and may contain
		identifiers and variables.[br]
		<icon> is an optional [doc:image_id]image identifier[/doc].[br]
		<item_id> is the optional item ID - if not specified, it will be generated automatically.[br]
		If <condition> is given, it is evaluated at [cmd]popup.show[/cmd] call time and if the
		result is 0, the item is not shown in the physical popup.[br]
		If this item is hovered over, the nested popup <external_popup> will be shown.
	@seealso:
		[cmd]defpopup[/cmd], [cmd]popup.show[/cmd], [cmd]popup.addSubPopup[/cmd]
*/

static bool popup_kvs_cmd_addExtPopup(KviKvsModuleCommandCall * c)
{
	QString szPopupName, szSubPopupName, szItemId, szExtPopupName, szText, szIcon, szCondition;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("popupname", KVS_PT_NONEMPTYSTRING, 0, szPopupName)
	KVSM_PARAMETER("external_popup", KVS_PT_NONEMPTYSTRING, 0, szExtPopupName)
	KVSM_PARAMETER("text", KVS_PT_NONEMPTYSTRING, 0, szText)
	KVSM_PARAMETER("icon", KVS_PT_STRING, KVS_PF_OPTIONAL, szIcon)
	KVSM_PARAMETER("item_id", KVS_PT_STRING, KVS_PF_OPTIONAL, szItemId)
	KVSM_PARAMETER("condition", KVS_PT_STRING, KVS_PF_OPTIONAL, szCondition)
	KVSM_PARAMETERS_END(c)

	GET_KVS_POPUP(isHardLocked)

	pPopup->addExtPopup(szItemId, szExtPopupName, szText, szIcon, szCondition);

	KviKvsPopupManager::instance()->emitRefresh(szPopupName);
	return true;
}

/*
	@doc: popup.addLabel
	@type:
		command
	@title:
		popup.addLabel
	@syntax:
		popup.addLabel [-q] <popupname:string> <text:string> [<icon:string> [<item_id:string> [<condition:string>]]]
	@short:
		Adds a label to a popup
	@switches:
		!sw: -q | --quiet
		Run quietly: don't print warning and errors
	@description:
		Adds a descriptive label with visible <text> and the optional <icon> to an already existing popup or
		nested popup named <popupname>. If you want to add an item to a nested popup, use the form
		"popup.addItem", for example, "channeltextview.myNestedItem". The label acts like a separator - it is
		not selectable or clickable.[br]
		<text> is a string that is evaluated at [cmd]popup[/cmd] call time and may contain
		identifiers and variables.[br]
		<icon> is an optional [doc:image_id]image identifier[/doc].[br]
		<item_id> is the optional item ID - if not specified, it will be generated automatically.[br]
		If <condition> is given, it is evaluated at [cmd]popup.show[/cmd] call time and if the
		result is 0, the item is not shown in the physical popup.[br]
	@seealso:
		[cmd]defpopup[/cmd], [cmd]popup.show[/cmd]
*/

static bool popup_kvs_cmd_addLabel(KviKvsModuleCommandCall * c)
{
	QString szPopupName, szSubPopupName, szItemId, szText, szIcon, szCondition;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("popupname", KVS_PT_NONEMPTYSTRING, 0, szPopupName)
	KVSM_PARAMETER("text", KVS_PT_NONEMPTYSTRING, 0, szText)
	KVSM_PARAMETER("icon", KVS_PT_STRING, KVS_PF_OPTIONAL, szIcon)
	KVSM_PARAMETER("item_id", KVS_PT_STRING, KVS_PF_OPTIONAL, szItemId)
	KVSM_PARAMETER("condition", KVS_PT_STRING, KVS_PF_OPTIONAL, szCondition)
	KVSM_PARAMETERS_END(c)

	GET_KVS_POPUP(isHardLocked)

	pPopup->addLabel(szItemId, szText, szIcon, szCondition);

	KviKvsPopupManager::instance()->emitRefresh(szPopupName);
	return true;
}

/*
	@doc: popup.addSubPopup
	@type:
		command
	@title:
		popup.addSubPopup
	@syntax:
		popup.addSubPopup [-q] (<popupname:string>, <text:string>[, <icon:string>[, <item_id:string>[, <condition:string>]]])
	@short:
		Adds a nested popup to a popup
	@switches:
		!sw: -q | --quiet
		Run quietly: don't print warning and errors
	@description:
		Adds an empty nested popup item with visible <text> and the optional <icon> to an already existing popup or
		nested popup named <popupname>. If you want to add an item to a nested popup, use the form
		"popup.addItem", for example, "channeltextview.myNestedItem".[br]
		<text> is a string that is evaluated at [cmd]popup[/cmd] call time and may contain
		identifiers and variables.[br]
		<icon> is an optional [doc:image_id]image identifier[/doc].[br]
		<item_id> is the optional item ID - if not specified, it will be generated automatically.[br]
		If <condition> is given, it is evaluated at [cmd]popup.show[/cmd] call time and if the
		result is 0, the item is not shown in the physical popup.[br]
		If this item is hovered over, the nested popup will be shown.
	@seealso:
		[cmd]defpopup[/cmd], [cmd]popup.show[/cmd], [cmd]popup.addExtPopup[/cmd]
*/

static bool popup_kvs_cmd_addSubPopup(KviKvsModuleCommandCall * c)
{
	QString szPopupName, szSubPopupName, szItemId, szText, szIcon, szCondition;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("popupname", KVS_PT_NONEMPTYSTRING, 0, szPopupName)
	KVSM_PARAMETER("text", KVS_PT_NONEMPTYSTRING, 0, szText)
	KVSM_PARAMETER("icon", KVS_PT_STRING, KVS_PF_OPTIONAL, szIcon)
	KVSM_PARAMETER("item_id", KVS_PT_STRING, KVS_PF_OPTIONAL, szItemId)
	KVSM_PARAMETER("condition", KVS_PT_STRING, KVS_PF_OPTIONAL, szCondition)
	KVSM_PARAMETERS_END(c)

	GET_KVS_POPUP(isHardLocked)

	pPopup->addPopup(szItemId, szText, szIcon, szCondition);

	KviKvsPopupManager::instance()->emitRefresh(szPopupName);
	return true;
}

/*
	@doc: popup.addSeparator
	@type:
		command
	@title:
		popup.addSeparator
	@syntax:
		popup.addSeparator [-q] <popupname:string> [<item_id:string> [<condition:string>]]
	@short:
		Adds a separator to a popup
	@switches:
		!sw: -q | --quiet
		Run quietly: don't print warning and errors
	@description:
		Adds a separator (typically drawn as a straight line) to an already existing popup or
		nested popup named <popupname>. If you want to add an item to a nested popup, use the form
		"popup.addItem", for example, "channeltextview.myNestedItem". The separator is not selectable
		or clickable.[br]
		<item_id> is the optional item ID - if not specified, it will be generated automatically.[br]
		If <condition> is given, it is evaluated at [cmd]popup.show[/cmd] call time and if the
		result is 0, the item is not shown in the physical popup.[br]
	@seealso:
		[cmd]defpopup[/cmd], [cmd]popup.show[/cmd]
*/

static bool popup_kvs_cmd_addSeparator(KviKvsModuleCommandCall * c)
{
	QString szPopupName, szSubPopupName, szItemId, szCondition;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("popupname", KVS_PT_NONEMPTYSTRING, 0, szPopupName)
	KVSM_PARAMETER("item_id", KVS_PT_STRING, KVS_PF_OPTIONAL, szItemId)
	KVSM_PARAMETER("condition", KVS_PT_STRING, KVS_PF_OPTIONAL, szCondition)
	KVSM_PARAMETERS_END(c)

	GET_KVS_POPUP(isHardLocked)

	pPopup->addSeparator(szItemId, szCondition);

	KviKvsPopupManager::instance()->emitRefresh(szPopupName);
	return true;
}

/*
	@doc: popup.clear
	@type:
		command
	@title:
		popup.clear
	@syntax:
		popup.clear [-q] <popupname:string>
	@short:
		Clears all the items of a popup
	@switches:
		!sw: -q | --quiet
		Run quietly: don't print warning and errors
	@description:
		Removes all the items from an already existing popup or nested popup named <popupname>.
		If you want to clear a nested popup, use the form "popup.delItem", for example,
		"channeltextview.myNestedItem".[br]
	@seealso:
		[cmd]defpopup[/cmd], [cmd]popup.show[/cmd], [fnc]popup.isEmpty[/fnc]
*/

static bool popup_kvs_cmd_clear(KviKvsModuleCommandCall * c)
{
	QString szPopupName, szSubPopupName;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("popupname", KVS_PT_NONEMPTYSTRING, 0, szPopupName)
	KVSM_PARAMETERS_END(c)

	GET_KVS_POPUP(isSoftLocked)

	pPopup->doClear();

	KviKvsPopupManager::instance()->emitRefresh(szPopupName);
	return true;
}

/*
	@doc: popup.create
	@type:
		command
	@title:
		popup.create
	@syntax:
		popup.create [-q] <popupname:string>
	@short:
		Creates a popup
	@switches:
		!sw: -q | --quiet
		Run quietly: don't print warning and errors
	@description:
		Creates a popup named <popupname>; if the popup already exists, a warning is printed.
		To create a nested popup, use [cmd]popup.addSubPopup[/cmd] instead.
	@seealso:
		[cmd]defpopup[/cmd], [cmd]popup.show[/cmd], [cmd]popup.destroy[/cmd], [fnc]popup.exists[/fnc]
*/

static bool popup_kvs_cmd_create(KviKvsModuleCommandCall * c)
{
	QString szPopupName, szSubPopupName;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("popupname", KVS_PT_NONEMPTYSTRING, 0, szPopupName)
	KVSM_PARAMETERS_END(c)

	if(KviKvsPopupManager::instance()->lookup(szPopupName))
	{
		if(!c->hasSwitch('q', "quiet"))
			c->warning(__tr2qs_ctx("The popup \"%Q\" already exists", "kvs"), &szPopupName);
		return true;
	}

	KviKvsPopupMenu * pMenu = new KviKvsPopupMenu(szPopupName);
	KviKvsPopupManager::instance()->add(szPopupName, pMenu);

	KviKvsPopupManager::instance()->emitRefresh(szPopupName);
	return true;
}
/*
	@doc: popup.destroy
	@type:
		command
	@title:
		popup.destroy
	@syntax:
		popup.destroy [-q] <popupname:string>
	@short:
		Destroies a popup
	@switches:
		!sw: -q | --quiet
		Run quietly: don't print warning and errors
	@description:
		Destroys a popup named <popupname>.
		To destroy a nested popup, use [cmd]popup.delItem[/cmd] instead.
	@seealso:
		[cmd]defpopup[/cmd], [cmd]popup.show[/cmd], [cmd]popup.create[/cmd], [fnc]popup.exists[/fnc]
*/

static bool popup_kvs_cmd_destroy(KviKvsModuleCommandCall * c)
{
	QString szPopupName, szSubPopupName;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("popupname", KVS_PT_NONEMPTYSTRING, 0, szPopupName)
	KVSM_PARAMETERS_END(c)

	KviKvsPopupMenu * pPopup = KviKvsPopupManager::instance()->lookup(szPopupName);
	if(!pPopup)
	{
		if(!c->hasSwitch('q', "quiet"))
			c->warning(__tr2qs_ctx("Popup \"%Q\" doesn't exist", "kvs"), &szPopupName);
		return true;
	}
	if(pPopup->isSoftLocked())
	{
		if(!c->hasSwitch('q', "quiet"))
			c->warning(__tr2qs_ctx("Popup menu self-modification is not allowed (the popup is probably open)", "kvs"));
		return true;
	}

	KviKvsPopupManager::instance()->remove(szPopupName);

	KviKvsPopupManager::instance()->emitRefresh(szPopupName);
	return true;
}
/*
	@doc: popup.delitem
	@type:
		command
	@title:
		popup.delitem
	@syntax:
		popup.delitem [-d] [-q] <popupname:string> <item_id:string>
	@short:
		Deletes an item from a popup
	@switches:
		!sw: -q | --quiet
		Run quietly
		!sw: -d | --deep
		Search the whole popup tree instead of only the first level
	@description:
		Deletes the item specified by <id> from the popup <popupname>.
		If the -d flag is specified then the item with the specified
		<id> is searched in the whole popup tree (containing submenus)
		otherwise only the first level is searched. [br]
		If the -q flag is specified, the command does not complain
		about non-existent items or popup menus.[br]
		See [cmd]defpopup[/cmd] for more information.[br]
	@seealso:
		[cmd]defpopup[/cmd], [cmd]popup.show[/cmd]
*/

static bool popup_kvs_cmd_delItem(KviKvsModuleCommandCall * c)
{
	QString szPopupName, szSubPopupName, szItemId;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("popupname", KVS_PT_NONEMPTYSTRING, 0, szPopupName)
	KVSM_PARAMETER("item_id", KVS_PT_NONEMPTYSTRING, 0, szItemId)
	KVSM_PARAMETERS_END(c)

	GET_KVS_POPUP(isHardLocked)

	if(!pPopup->removeItemByName(szItemId, c->hasSwitch('d', "deep")))
	{
		if(!c->hasSwitch('q', "quiet"))
			c->warning(__tr2qs_ctx("The menu item with ID \"%Q\" doesn't exist in popup \"%Q\"", "kvs"), &szItemId, &szPopupName);
	}

	KviKvsPopupManager::instance()->emitRefresh(szPopupName);
	return true;
}

/*
	@doc: popup.exists
	@type:
		function
	@title:
		$popup.exists
	@syntax:
		$popup.exists(<popupname:string>)
	@short:
		Returns true if a popup exists
	@description:
		Returns true if a popup exists or false otherwise.
	@seealso:
		[cmd]defpopup[/cmd], [cmd]popup.show[/cmd], [cmd]popup.create[/cmd], [cmd]popup.destroy[/cmd]
*/

static bool popup_kvs_fnc_exists(KviKvsModuleFunctionCall * c)
{
	QString szPopupName, szSubPopupName;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("popupname", KVS_PT_NONEMPTYSTRING, 0, szPopupName)
	KVSM_PARAMETERS_END(c)

	int iIdx = szPopupName.indexOf(QChar('.'));
	if(iIdx == 0)
	{
		c->warning(__tr2qs_ctx("Empty subpopup name supplied", "kvs"));
		return true;
	}
	if(iIdx > 0)
	{
		if(iIdx == szPopupName.size())
		{
			c->warning(__tr2qs_ctx("Empty subpopup name supplied", "kvs"));
			return true;
		}
		szSubPopupName = szPopupName.mid(iIdx + 1);
		szPopupName.truncate(iIdx);
	}
	KviKvsPopupMenu * pPopup = KviKvsPopupManager::instance()->lookup(szPopupName);
	if(!pPopup)
	{
		c->returnValue()->setBoolean(false);
		return true;
	}
	if(!szSubPopupName.isEmpty())
	{
		pPopup = pPopup->findChildPopupByName(szSubPopupName);
		if(!pPopup)
		{
			c->returnValue()->setBoolean(false);
			return true;
		}
	}

	c->returnValue()->setBoolean(true);
	return true;
}

/*
	@doc: popup.isEmpty
	@type:
		function
	@title:
		$popup.isEmpty
	@syntax:
		$popup.isEmpty(<popupname:string>)
	@short:
		Returns true if a popup is empty
	@description:
		Returns true if a popup is empty or false otherwise.
	@seealso:
		[cmd]defpopup[/cmd], [cmd]popup.show[/cmd], [cmd]popup.clear[/cmd]
*/

static bool popup_kvs_fnc_isEmpty(KviKvsModuleFunctionCall * c)
{
	QString szPopupName, szSubPopupName;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("popupname", KVS_PT_NONEMPTYSTRING, 0, szPopupName)
	KVSM_PARAMETERS_END(c)

	GET_KVS_POPUP_FNC

	c->returnValue()->setBoolean(pPopup->isEmpty());
	return true;
}

/*
	@doc: popup.currentItemId
	@type:
		function
	@title:
		$popup.currentItemId
	@syntax:
		$popup.currentItemId()
	@short:
		Returns the popup item ID that owns the executing code
	@description:
		When called inside a popup item's callback, this function returns the ID of the popup item.
		Outside of callback code, $null is returned.
	@examples:
		[example]
			#clean any previous popup by the same name
			setmenu test
			popup.clear test
			popup.destroy test
			#create the popup
			popup.create test
			popup.addPrologue(test)
			{
				# loop ten times
				foreach(%i,0,1,2,3,4,5,6,7,8,9)
				{
					# delete previous created item to avoid duplication
					popup.delItem test %i
					# create a test item using %i as its "item id"
					popup.addItem(test, item%i, , %i)
					{
						#get back "item id" inside the callback
						debug "chosen item $popup.currentItemId()";
					}
				}
			}
			setmenu test test
		[/example]
	@seealso:
		[cmd]popup.show[/cmd], [cmd]popup.addItem[/cmd]
*/

static bool popup_kvs_fnc_currentItemId(KviKvsModuleFunctionCall * c)
{
	QString * pPopupId = c->context()->popupId();
	if(pPopupId)
	{
		c->returnValue()->setString(pPopupId);
	}
	else
	{
		c->returnValue()->setNothing();
	}
	return true;
}

/*
	@doc: popup.show
	@type:
		command
	@title:
		popup.show
	@syntax:
		popup.show [-p=<screen_coordinates:string>] <popup_name:string> [<parameter1:variant> [<parameter2:variant> [...]]]
	@short:
		Shows a popup menu
	@switches:
		!sw: -p=<screen_coordinates:string> | --point=<screen_coordinates:string>
	@description:
		Shows the popup menu <popup_name> at the current cursor position,
		eventually passing the [parameters]. Please note that you can't
		use this command inside one of the [cmd]defpopup[/cmd] command
		handlers for <popup_name>. In other words, you can't "popup
		a popup" while it is being popped up. :) (This would
		be an endless recursive behaviour).[br]
		If the -p switch is used, then <screen_coordinates> must be
		in the form <x>,<y> and will be used as screen coordinates
		for the placement of the popup (instead of the current cursor position).[br]
	@seealso:
		[cmd]defpopup[/cmd]
*/

static bool popup_kvs_cmd_show(KviKvsModuleCommandCall * c)
{
	QString szPopupName, szSubPopupName;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("popup_name", KVS_PT_NONEMPTYSTRING, 0, szPopupName)
	KVSM_PARAMETERS_END(c)

	// copy parameters
	KviKvsVariantList * pPopupParams = new KviKvsVariantList();
	c->params()->first();
	while(KviKvsVariant * v = c->params()->next())
		pPopupParams->append(new KviKvsVariant(*v));

	KviKvsPopupMenu * pMenu = KviKvsPopupManager::instance()->lookup(szPopupName);

	if(!pMenu)
	{
		delete pPopupParams;
		pPopupParams = nullptr;
		c->error(__tr2qs_ctx("Popup %Q is not defined", "kvs"), &szPopupName);
		return false;
	}

	if(pMenu->isSoftLocked())
	{
		delete pPopupParams;
		pPopupParams = nullptr;
		c->error(__tr2qs_ctx("A popup menu can't be popped up twice", "kvs"));
		return false;
	}

	QPoint pnt = QCursor::pos();

	KviKvsVariant * pCoords = c->getSwitch('p', "point");
	if(pCoords)
	{
		QString szCoords;
		pCoords->asString(szCoords);

		int idx = szCoords.indexOf(',');
		bool bCoordsOk = true;
		if(idx == -1)
			bCoordsOk = false;
		else
		{
			QString szX = szCoords.left(idx);
			szCoords.remove(0, idx + 1);
			bool bOk1, bOk2;
			int iX = szX.toInt(&bOk1);
			int iY = szCoords.toInt(&bOk2);
			if(bOk1 && bOk2)
				pnt = QPoint(iX, iY);
			else
				bCoordsOk = false;
		}

		if(!bCoordsOk)
			c->warning(__tr2qs_ctx("Invalid syntax for screen coordinates, using cursor position", "kvs"));
	}

	pMenu->doPopup(pnt, c->window(), pPopupParams);
	return true;
}

static bool popup_module_init(KviModule * m)
{
	KVSM_REGISTER_CALLBACK_COMMAND(m, "addItem", popup_kvs_cmd_addItem);
	KVSM_REGISTER_CALLBACK_COMMAND(m, "addPrologue", popup_kvs_cmd_addPrologue);
	KVSM_REGISTER_CALLBACK_COMMAND(m, "addEpilogue", popup_kvs_cmd_addEpilogue);

	KVSM_REGISTER_SIMPLE_COMMAND(m, "addExtPopup", popup_kvs_cmd_addExtPopup);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "addLabel", popup_kvs_cmd_addLabel);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "addSubPopup", popup_kvs_cmd_addSubPopup);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "addSeparator", popup_kvs_cmd_addSeparator);

	KVSM_REGISTER_SIMPLE_COMMAND(m, "clear", popup_kvs_cmd_clear);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "create", popup_kvs_cmd_create);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "delItem", popup_kvs_cmd_delItem);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "destroy", popup_kvs_cmd_destroy);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "show", popup_kvs_cmd_show);

	KVSM_REGISTER_FUNCTION(m, "exists", popup_kvs_fnc_exists);
	KVSM_REGISTER_FUNCTION(m, "isEmpty", popup_kvs_fnc_isEmpty);
	KVSM_REGISTER_FUNCTION(m, "currentItemId", popup_kvs_fnc_currentItemId);

	return true;
}

static bool popup_module_cleanup(KviModule *)
{
	return true;
}

static bool popup_module_can_unload(KviModule *)
{
	return true;
}

KVIRC_MODULE(
    "Userlist",                                                  // module name
    "4.0.0",                                                     // module version
    "Copyright (C) 2010 Fabio Bas (ctrlaltca at gmail dot com)", // author & (C)
    "KVIrc popup management functions",
    popup_module_init,
    popup_module_can_unload,
    0,
    popup_module_cleanup,
    0)
