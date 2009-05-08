//
//   File : libkviterm.cpp
//   Creation date : Wed Aug 30 2000 15:29:02 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2000 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//

#include "kvi_settings.h"

#include "kvi_module.h"

#include "kvi_window.h"

#include "kvi_frame.h"

#include <qsplitter.h>
#include "kvi_pointerlist.h"

#ifdef COMPILE_KDE_SUPPORT

	#include "termwidget.h"
	#include "termwindow.h"

	#include "ktrader.h"

//	#include "kparts/part.h"
//	#include "kparts/factory.h"
//	#include "klibloader.h"

#endif

KviModule * g_pTermModule = 0;

#ifdef COMPILE_KDE_SUPPORT
	KviPointerList<KviTermWidget> * g_pTermWidgetList      = 0;
	KviPointerList<KviTermWindow> * g_pTermWindowList      = 0;
	KviStr                 g_szKonsoleLibraryName = "";
#endif

/*
	@doc: term.open
	@type:
		command
	@title:
		term.open
	@short:
		Shows a terminal emulator
	@syntax:
		term.open [-m]
	@switches:
		!sw: -m | --mdi
		creates terminal as a MDI window
	@description:
		Opens a new terminal window (If the service is available).
		if the -m switch is present, the created terminal as a MDI window,
		otherwise it is a static window.[br]
		This command is exported by the "term" module.[br]
		Note: At the time that this module was written, the
		KDE terminal emulator module was quite unstable; no extensive
		testings have been made on this.
*/

static bool term_kvs_cmd_open(KviKvsModuleCommandCall * c)
{
#ifdef COMPILE_KDE_SUPPORT
	if(g_szKonsoleLibraryName.hasData())
	{
		c->module()->lock(); // multiple locks are allowed
		if(c->hasSwitch('m',"mdi"))
		{
			KviTermWindow *w = new KviTermWindow(c->window()->frame(),"Terminal emulator");
			c->window()->frame()->addWindow(w);
		} else {
			KviTermWidget *w = new KviTermWidget(c->window()->frame()->splitter(),
				c->window()->frame(),true);
			w->show();
		}
	} else {
		c->warning("No terminal emulation service available");
	}
#else
	c->warning("Terminal emulation service not supported (non-KDE compilation)");
#endif
	return true;
}

static bool term_module_init(KviModule * m)
{
	g_pTermModule = m;

#ifdef COMPILE_KDE_SUPPORT
	g_pTermWidgetList = new KviPointerList<KviTermWidget>;
	g_pTermWidgetList->setAutoDelete(false);
	g_pTermWindowList = new KviPointerList<KviTermWindow>;
	g_pTermWindowList->setAutoDelete(false);

//	KTrader::OfferList offers = KTrader::self()->query("Browser/View","KonsolePart");
//	KTrader::OfferList offers = KTrader::self()->query("Browser/View","");
//	if(offers.count() > 0)
//	{
//		KTrader::OfferList::Iterator it = offers.begin();
//		while(it != offers.end())
//		{
//			KviStr tmp = (*it)->name();
//			KviStr tmp2 = (*it)->type();
//			KviStr tmp3 = (*it)->library();
//			debug("Got Service name:%s type:%s library:%s",tmp.ptr(),tmp2.ptr(),tmp3.ptr());
//			++it;
//		}
//	}
//		KService::Ptr service = *offers.begin();
	KService::Ptr pKonsoleService = KService::serviceByName("Terminal Emulator");
	if(!pKonsoleService)pKonsoleService = KService::serviceByName("KonsolePart");
	if(!pKonsoleService)pKonsoleService = KService::serviceByName("Terminal *");
	if(pKonsoleService)
	{
		g_szKonsoleLibraryName = pKonsoleService->library();
//		debug("KONSOLE LIB %s",g_szKonsoleLibraryName.ptr());
	} else {
		// try the default name
		g_szKonsoleLibraryName = "libkonsolepart";
	}
//	delete pKonsoleService;
#endif

//	m->registerCommand("open",term_module_cmd_open);
#ifdef COMPILE_NEW_KVS
	KVSM_REGISTER_SIMPLE_COMMAND(m,"open",term_kvs_cmd_open);
#endif
	return true;
}

static bool term_module_cleanup(KviModule *m)
{
#ifdef COMPILE_KDE_SUPPORT
	while(g_pTermWidgetList->first())delete g_pTermWidgetList->first();
	delete g_pTermWidgetList;
	g_pTermWidgetList = 0;
	while(g_pTermWindowList->first())g_pTermWindowList->first()->close();
	delete g_pTermWindowList;
	g_pTermWindowList = 0;
#endif
	return true;
}

KVIRC_MODULE(
	"Term",                                                 // module name
	"1.0.0",                                                // module version
	"Copyright (C) 2000 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
	"Terminal emulator extension based on the KonsolePart KDE class",
	term_module_init,
	0,
	0,
	term_module_cleanup
)
