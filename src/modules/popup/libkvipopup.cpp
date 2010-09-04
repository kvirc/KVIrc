//=============================================================================
//
//   File : libkvipopup.cpp
//   Creation date : Fri Sep 03 2010 15:23:00 CEST by Fabio Bas
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2008 Szymon Stefanek (pragma at kvirc dot net)
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
#include "kvi_locale.h"
#include "kvi_kvs_popupmanager.h"

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
		Deletes the item specified by <id> from the poup <popupname>.
		If the -d flag is specified then the item with the specified
		<id> is searched in the whole popup tree (containing submenus)
		otherwise only the first level is searched. [br]
		If the -q flag is specified, the command does not complain
		about non-existent items or popup menus.[br]
		See [cmd]defpopup[/cmd] for more information.[br]
	@seealso:
		[cmd]defpopup[/cmd], [cmd]popup[/cmd]
*/


static bool popup_kvs_cmd_delitem(KviKvsModuleCommandCall * c)
{
	QString szPopupName,szItemId;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("popupname",KVS_PT_NONEMPTYSTRING,0,szPopupName)
		KVSM_PARAMETER("item_id",KVS_PT_NONEMPTYSTRING,0,szItemId)
	KVSM_PARAMETERS_END(c)

	KviKvsPopupMenu * p = KviKvsPopupManager::instance()->lookup(szPopupName);
	if(!p)
	{
		if(!c->hasSwitch('q',"quiet"))
			c->warning(__tr2qs_ctx("Inexisting popup \"%Q\"","kvs"),&szPopupName);
		return true;
	}

	if(p->isLocked())
	{
		c->error(__tr2qs_ctx("Popup menu self-modification is not allowed (the popup is probably open)","kvs"));
		return false;
	}

	if(!p->removeItemByName(szItemId,c->hasSwitch('d',"deep")))
	{
		if(!c->hasSwitch('q',"quiet"))
			c->warning(__tr2qs_ctx("The menu item with id \"%Q\" does not exist in popup \"%Q\"","kvs"),&szItemId,&szPopupName);
	}

	KviKvsPopupManager::instance()->emitRefresh(szPopupName);
	return true;
}

	/*
		@doc: popup
		@type:
			command
		@title:
			popup
		@syntax:
			popup [-p=<screen_coordinates:string>] <popup_name:string> [<parameter1:variant> [<parameter2:variant> [...]]]
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
	QString szPopupName;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("popup_name",KVS_PT_NONEMPTYSTRING,0,szPopupName)
	KVSM_PARAMETERS_END(c)

	// copy parameters
	KviKvsVariantList * pPopupParams = new KviKvsVariantList();
	c->params()->first();
	while(KviKvsVariant * v = c->params()->next())pPopupParams->append(new KviKvsVariant(*v));

	KviKvsPopupMenu * pMenu = KviKvsPopupManager::instance()->lookup(szPopupName);

	if(!pMenu)
	{
		delete pPopupParams;
		pPopupParams = 0;
		c->error(__tr2qs_ctx("Popup %Q is not defined","kvs"),&szPopupName);
		return false;
	}

	if(pMenu->isLocked())
	{
		delete pPopupParams;
		pPopupParams = 0;
		c->error(__tr2qs_ctx("A popup menu cannot be popped up twice","kvs"));
		return false;
	}

	QPoint pnt = QCursor::pos();

	KviKvsVariant * pCoords = c->getSwitch('p',"point");
	if(pCoords)
	{
		QString szCoords;
		pCoords->asString(szCoords);

		int idx = szCoords.indexOf(',');
		bool bCoordsOk = true;
		if(idx == -1)bCoordsOk = false;
		else {
			QString szX = szCoords.left(idx);
			szCoords.remove(0,idx + 1);
			bool bOk1,bOk2;
			int iX = szX.toInt(&bOk1);
			int iY = szCoords.toInt(&bOk2);
			if(bOk1 && bOk2)pnt = QPoint(iX,iY);
			else bCoordsOk = false;
		}

		if(!bCoordsOk)c->warning(__tr2qs_ctx("Invalid syntax for screen coordinates, using cursor position","kvs"));
	}

	pMenu->doPopup(pnt,c->window(),pPopupParams);
	return true;
}

static bool popup_module_init(KviModule *m)
{
	KVSM_REGISTER_SIMPLE_COMMAND(m,"delitem",popup_kvs_cmd_delitem);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"show",popup_kvs_cmd_show);
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
	"Userlist",                                               // module name
	"4.0.0",                                                // module version
	"Copyright (C) 2010 Fabio Bas (ctrlaltca at gmail dot com)", // author & (C)
	"KVIrc popup management functions",
	popup_module_init,
	popup_module_can_unload,
	0,
	popup_module_cleanup,
	0
)
