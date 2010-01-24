//=============================================================================
//
//   File : libkvioptions.cpp
//   Creation date : Fri Aug 18 2000 15:04:09 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "container.h"
#include "instances.h"
#include "dialog.h"

#include "kvi_module.h"
#include "kvi_options.h"
#include "kvi_app.h"
#include "kvi_frame.h"
#include "kvi_window.h"
#include "kvi_locale.h"
#include "kvi_mirccntrl.h"
#include "kvi_out.h"
#include "kvi_splash.h"
#include "kvi_pointerhashtable.h"

#include <QSplitter>

KviPointerHashTable<QString,KviOptionsDialog> * g_pOptionsDialogDict = 0;

KviOptionsInstanceManager * g_pOptionsInstanceManager = 0;

extern int g_iOptionWidgetInstances;

/*
	@doc: options.save
	@type:
		command
	@title:
		options.save
	@short:
		Saves the options to disk
	@syntax:
		options.save
	@description:
		Saves the options to disk; this includes aliases , popups ,
		events and user preferences.
*/

static bool options_kvs_cmd_save(KviKvsModuleCommandCall *)
{
	g_pApp->saveOptions();
	return true;
}


/*
	@doc: options.dialog
	@type:
		command
	@title:
		options.dialog
	@short:
		Shows the "options dialog"
	@syntax:
		options.dialog [-t] [options_group]
	@description:
		Shows the KVIrc options dialog for the specified options group.
		If the [-t] switch is used , the dialog is opened as toplevel window,
		otherwise it is opened as part of the current frame window.[br]
		Valid values for [options_group] are "general" and "theme".
		If [options_group] is omitted, the option group "general" is assumed.
		This command is exported by the "options" module.
	@seealso:
		[fnc]$options.isDialog[/fnc]
*/


static bool options_kvs_cmd_dialog(KviKvsModuleCommandCall * c)
{
	QString szGroup;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("options_group",KVS_PT_STRING,KVS_PF_OPTIONAL,szGroup)
	KVSM_PARAMETERS_END(c)
	if(szGroup.isEmpty())szGroup = "general";
	KviOptionsDialog * d = g_pOptionsDialogDict->find(szGroup);
	if(d)
	{
		if(c->hasSwitch('t',"toplevel"))
		{
			if(d->parent())
			{
				d->setParent(0);
				d->setGeometry(0,0,d->width(),d->height());
			}
		} else {
			if(d->parent() != c->window()->frame()->splitter())
			{
				d->setParent(c->window()->frame()->splitter());
				d->setGeometry(0,0,d->width(),d->height());
				d->show();
			}
		}
	} else {
		if(c->hasSwitch('t',"toplevel"))
		{
			d = new KviOptionsDialog(0,szGroup);
		} else {
			d = new KviOptionsDialog(c->window()->frame()->splitter(),szGroup);
		}
		g_pOptionsDialogDict->insert(szGroup,d);
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


static void options_kvs_module_print_pages(KviKvsModuleCommandCall * c,KviOptionsWidgetInstanceEntry * e,const char * prefix)
{
	c->window()->output(KVI_OUT_SYSTEMMESSAGE,"%s%c%s%c  (%Q)",prefix,KVI_TEXT_BOLD,e->szClassName,KVI_TEXT_BOLD,&(e->szName));
	KviStr szPre = prefix;
	szPre.append("  ");
	if(e->pChildList)
	{
		for(KviOptionsWidgetInstanceEntry * ex = e->pChildList->first();ex;ex = e->pChildList->next())
		{
			options_kvs_module_print_pages(c,ex,szPre.ptr());
		}
	}
}

static bool options_kvs_cmd_pages(KviKvsModuleCommandCall * c)
{
	KviPointerList<KviOptionsWidgetInstanceEntry> * l = g_pOptionsInstanceManager->instanceEntryTree();

	for(KviOptionsWidgetInstanceEntry * e = l->first();e;e = l->next())
	{
		options_kvs_module_print_pages(c,e,"");
	}

	return true;
}

/*
	@doc: options.edit
	@type:
		command
	@title:
		options.edit [-m]
	@short:
		Shows a single options page
	@syntax:
		options.edit <"options page class name">
	@description:
		Shows an options page as toplevel dialog.
		The available option pages can be listed by using [cmd]options.pages[/cmd].
		If the -m switch is used, the dialog will appear as modal, blocking input
		to the main frame until it's closed.
	@seealso:
*/

static bool options_kvs_cmd_edit(KviKvsModuleCommandCall * c)
{
	QString szOption;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("option",KVS_PT_STRING,0,szOption)
	KVSM_PARAMETERS_END(c)
	KviOptionsWidgetInstanceEntry * e = g_pOptionsInstanceManager->findInstanceEntry(szOption.toUtf8().data());
	if(!e)
	{
		c->warning(__tr2qs_ctx("No such options page class name %Q","options"),&szOption);
		return true;
	}

	if(e->pWidget)
	{
		e->pWidget->raise();
		e->pWidget->activateWindow();
		e->pWidget->setFocus();
		return true;
	}

	QWidget * w;

	if(c->hasSwitch('m',"modal"))
	{
		w = g_pApp->activeModalWidget();
		if(!w)w = g_pFrame;
	} else {
		w = g_pFrame;
	}

	KviOptionsWidgetContainer * wc = new KviOptionsWidgetContainer(w,c->hasSwitch('m',"modal"));

	wc->setup(g_pOptionsInstanceManager->getInstance(e,wc));

	// a trick for the dialog covering the splash screen before the time (this is prolly a WM or Qt bug)
	if(g_pSplashScreen)
	{
		if(g_pSplashScreen->isVisible()) // another bug: this ALWAYS RETURNS TRUE, even if the splash was hidden by a mouse click...
		{
			QObject::connect(g_pSplashScreen,SIGNAL(destroyed()),wc,SLOT(show()));
			return true;
		}
	}

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
		If [options_group] is omitted then the group "general" is assumed.
		At the moment of writing the valid [options_group] values are "general" and "theme".
		This command is exported by the "options" module.
	@seealso:
		[cmd]options.dialog[/cmd]
*/

static bool options_kvs_fnc_isdialog(KviKvsModuleFunctionCall * c)
{
	QString szGroup;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("options_group",KVS_PT_STRING,KVS_PF_OPTIONAL,szGroup)
	KVSM_PARAMETERS_END(c)
	if(szGroup.isEmpty())szGroup = "general";
	c->returnValue()->setBoolean(g_pOptionsDialogDict->find(szGroup));
	return true;
}

static bool options_module_init(KviModule * m)
{
	g_pOptionsInstanceManager = new KviOptionsInstanceManager();

	KVSM_REGISTER_SIMPLE_COMMAND(m,"dialog",options_kvs_cmd_dialog);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"save",options_kvs_cmd_save);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"pages",options_kvs_cmd_pages);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"edit",options_kvs_cmd_edit);
	KVSM_REGISTER_FUNCTION(m,"isDialog",options_kvs_fnc_isdialog);

	g_pOptionsDialogDict = new KviPointerHashTable<QString,KviOptionsDialog>;
	g_pOptionsDialogDict->setAutoDelete(false);

	return true;
}

static bool options_module_cleanup(KviModule *m)
{
	KviPointerHashTableIterator<QString,KviOptionsDialog> it(*g_pOptionsDialogDict);
	KviPointerList<KviOptionsDialog> l;
	l.setAutoDelete(false);
	KviOptionsDialog * d;
	while( (d = it.current()) )
	{
		l.append(d);
		++it;
	}
	for(d = l.first();d;d = l.next())delete d;
	delete g_pOptionsDialogDict;
	g_pOptionsDialogDict = 0;

	g_pOptionsInstanceManager->cleanup(m);
	delete g_pOptionsInstanceManager;
	g_pOptionsInstanceManager = 0;

	return true;
}

static bool options_module_can_unload(KviModule *)
{
	return ((g_pOptionsDialogDict->isEmpty()) && (g_iOptionWidgetInstances == 0));
}

KVIRC_MODULE(
	"Options",                                              // module name
	"4.0.0",                                                // module version
	"Copyright (C) 2000 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
	"Options Dialog",
	options_module_init,
	options_module_can_unload,
	0,
	options_module_cleanup,
	"options"
)
