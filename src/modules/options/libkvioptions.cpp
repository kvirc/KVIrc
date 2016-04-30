//=============================================================================
//
//   File : libkvioptions.cpp
//   Creation date : Fri Aug 18 2000 15:04:09 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "OptionsWidgetContainer.h"
#include "OptionsInstanceManager.h"
#include "OptionsDialog.h"

#include "KviModule.h"
#include "KviOptions.h"
#include "KviApplication.h"
#include "KviMainWindow.h"
#include "KviWindow.h"
#include "KviLocale.h"
#include "KviControlCodes.h"
#include "kvi_out.h"
#include "KviPointerHashTable.h"

#include <QSplitter>

KviPointerHashTable<QString, OptionsDialog> * g_pOptionsDialogDict = nullptr;

OptionsInstanceManager * g_pOptionsInstanceManager = nullptr;

extern int g_iOptionWidgetInstances;

/*
	@doc: options.save
	@type:
		command
	@title:
		options.save
	@short:
		Saves the configuration to disk
	@syntax:
		options.save
	@description:
		Saves the options to disk; this includes aliases, popups,
		events and all user preferences. You shouldn't need to
		call this as KVIrc [i]should[/i] save the relevant part of
		configuration when it's changed. However you might want it
		in a debugging session or when testing an unstable version
		where crashes are possible.
*/

static bool options_kvs_cmd_save(KviKvsModuleCommandCall *)
{
	//g_pApp->saveOptions();
	g_pApp->saveConfiguration();
	return true;
}

/*
	@doc: options.dialog
	@type:
		command
	@title:
		options.dialog
	@short:
		Shows the preferences dialog
	@syntax:
		options.dialog [-t] [options_group]
	@description:
		Shows the KVIrc preferences dialog and the specified options group.
		If the [-t] switch is used, the dialog is opened as toplevel window,
		otherwise it is opened as part of the current frame window.[br][br]
		Valid values for [options_group] are [b]general[/b] and [b]theme[/b].
		If [options_group] is omitted, the option group [i]general[/i] is assumed.
		This command is exported by the "options" module.
	@seealso:
		[fnc]$options.isDialog[/fnc]
*/

static bool options_kvs_cmd_dialog(KviKvsModuleCommandCall * c)
{
	QString szGroup;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("options_group", KVS_PT_STRING, KVS_PF_OPTIONAL, szGroup)
	KVSM_PARAMETERS_END(c)
	if(szGroup.isEmpty())
		szGroup = "general";
	if(szGroup != "general" && szGroup != "theme")
	{
		c->warning(__tr2qs_ctx("No such options_group %Q", "options"), &szGroup);
		return true;
	}
	OptionsDialog * d = g_pOptionsDialogDict->find(szGroup);
	if(d)
	{
		if(c->hasSwitch('t', "toplevel"))
		{
			if(d->parent())
			{
				d->setParent(nullptr);
				d->setGeometry(0, 0, d->width(), d->height());
			}
		}
		else
		{
			if(d->parent() != g_pMainWindow->splitter())
			{
				d->setParent(g_pMainWindow->splitter());
				d->setGeometry(0, 0, d->width(), d->height());
				d->show();
			}
		}
	}
	else
	{
		if(c->hasSwitch('t', "toplevel"))
		{
			d = new OptionsDialog(g_pMainWindow, szGroup, true);
		}
		else
		{
			d = new OptionsDialog(g_pMainWindow->splitter(), szGroup, false);
		}
		g_pOptionsDialogDict->insert(szGroup, d);
	}
	d->raise();
	d->show();
	d->setFocus();
	return true;
}

/*
	@doc: options.pages
	@type:
		command
	@title:
		options.pages
	@short:
		Lists the option pages
	@syntax:
		options.pages
	@description:
		Lists the option pages available for editing by the means of [cmd]options.edit[/cmd].
	@seealso:
*/

static void options_kvs_module_print_pages(KviKvsModuleCommandCall * c, OptionsWidgetInstanceEntry * e, const char * prefix)
{
	c->window()->output(KVI_OUT_SYSTEMMESSAGE, "%s%c%s%c  (%Q)", prefix, KviControlCodes::Bold, e->szClassName, KviControlCodes::Bold, &(e->szName));
	KviCString szPre = prefix;
	szPre.append("  ");
	if(e->pChildList)
	{
		for(OptionsWidgetInstanceEntry * ex = e->pChildList->first(); ex; ex = e->pChildList->next())
		{
			options_kvs_module_print_pages(c, ex, szPre.ptr());
		}
	}
}

static bool options_kvs_cmd_pages(KviKvsModuleCommandCall * c)
{
	KviPointerList<OptionsWidgetInstanceEntry> * l = g_pOptionsInstanceManager->instanceEntryTree();

	for(OptionsWidgetInstanceEntry * e = l->first(); e; e = l->next())
	{
		options_kvs_module_print_pages(c, e, "");
	}

	return true;
}

/*
	@doc: options.edit
	@type:
		command
	@title:
		options.edit [-n]
	@short:
		Shows a single options page
	@syntax:
		options.edit <options_page_class_name:string>
	@description:
		Shows an options page as toplevel dialog.
		The available option pages can be listed by using [cmd]options.pages[/cmd].
		If the -n switch is used then dialog will appear as [b]non[/b] modal (will not block input).
	@seealso:
*/

static bool options_kvs_cmd_edit(KviKvsModuleCommandCall * c)
{
	QString szOption;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("option", KVS_PT_STRING, 0, szOption)
	KVSM_PARAMETERS_END(c)

	OptionsWidgetInstanceEntry * e = g_pOptionsInstanceManager->findInstanceEntry(szOption.toUtf8().data());
	if(!e)
	{
		c->warning(__tr2qs_ctx("No such options page class name %Q", "options"), &szOption);
		return true;
	}

	if(e->pWidget)
	{
		e->pWidget->raise();
		e->pWidget->activateWindow();
		e->pWidget->setFocus();
		return true;
	}

	OptionsWidgetContainer * wc = new OptionsWidgetContainer(
	    g_pMainWindow,
	    !c->hasSwitch('n', "non-modal"));

	wc->setup(g_pOptionsInstanceManager->getInstance(e, wc));

	//wc->setModal(true);

	wc->show();
	wc->raise();
	return true;
}

/*
	@doc: options.isdialog
	@type:
		function
	@title:
		$options.isdialog
	@short:
		Returns the options dialog state
	@syntax:
		<boolean> $options.isdialog([options_group:string])
	@description:
		Returns '1' if the options dialog for the specified options_group is open, '0' otherwise.[br]
		If [options_group] is omitted then the group [i]general[/i] is assumed.
		At the moment of writing the valid [options_group] values are [b]general[/b] and [b]theme[/b].
		This command is exported by the [i]options[/i] module.
	@seealso:
		[cmd]options.dialog[/cmd]
*/

static bool options_kvs_fnc_isdialog(KviKvsModuleFunctionCall * c)
{
	QString szGroup;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("options_group", KVS_PT_STRING, KVS_PF_OPTIONAL, szGroup)
	KVSM_PARAMETERS_END(c)
	if(szGroup.isEmpty())
		szGroup = "general";
	c->returnValue()->setBoolean(g_pOptionsDialogDict->find(szGroup));
	return true;
}

/*
	@doc: options.close
	@type:
		command
	@title:
		options.close
	@short:
		Close an options dialog
	@syntax:
		options.close [options_group_or_page_class_name]
	@description:
		Closes the KVIrc options dialog for the specified options group
		or containing the specified options page.
		If [options_group] is omitted, the option group [i]general[/i] is assumed.
		This command is exported by the [i]options[/i] module.
	@seealso:
		[fnc]$options.isDialog[/fnc]
*/

static bool options_kvs_cmd_close(KviKvsModuleCommandCall * c)
{
	QString szGroup;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("options_group", KVS_PT_STRING, KVS_PF_OPTIONAL, szGroup)
	KVSM_PARAMETERS_END(c)
	if(szGroup.isEmpty())
		szGroup = "general";

	OptionsDialog * d = g_pOptionsDialogDict->find(szGroup);
	if(d)
	{
		d->close();
		return true;
	}

	OptionsWidgetInstanceEntry * e = g_pOptionsInstanceManager->findInstanceEntry(szGroup.toUtf8().data());
	if(!e)
		return true;

	if(!e->pWidget)
		return true; // not open

	QWidget * w = e->pWidget->topLevelWidget();
	if(!w)
		return true; // hm?

	w->deleteLater();

	return true;
}

static bool options_module_init(KviModule * m)
{
	g_pOptionsInstanceManager = new OptionsInstanceManager();

	KVSM_REGISTER_SIMPLE_COMMAND(m, "dialog", options_kvs_cmd_dialog);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "close", options_kvs_cmd_close);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "save", options_kvs_cmd_save);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "pages", options_kvs_cmd_pages);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "edit", options_kvs_cmd_edit);
	KVSM_REGISTER_FUNCTION(m, "isDialog", options_kvs_fnc_isdialog);

	g_pOptionsDialogDict = new KviPointerHashTable<QString, OptionsDialog>;
	g_pOptionsDialogDict->setAutoDelete(false);

	return true;
}

static bool options_module_cleanup(KviModule * m)
{
	KviPointerHashTableIterator<QString, OptionsDialog> it(*g_pOptionsDialogDict);
	KviPointerList<OptionsDialog> l;
	l.setAutoDelete(false);
	OptionsDialog * d;
	while((d = it.current()))
	{
		l.append(d);
		++it;
	}
	for(d = l.first(); d; d = l.next())
		delete d;
	delete g_pOptionsDialogDict;
	g_pOptionsDialogDict = nullptr;

	g_pOptionsInstanceManager->cleanup(m);
	delete g_pOptionsInstanceManager;
	g_pOptionsInstanceManager = nullptr;

	return true;
}

static bool options_module_can_unload(KviModule *)
{
	return ((g_pOptionsDialogDict->isEmpty()) && (g_iOptionWidgetInstances == 0));
}

KVIRC_MODULE(
    "Options",                                                      // module name
    "4.0.0",                                                        // module version
    "Copyright (C) 2000 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
    "Options Dialog",
    options_module_init,
    options_module_can_unload,
    0,
    options_module_cleanup,
    "options")
