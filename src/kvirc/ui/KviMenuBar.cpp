//=============================================================================
//
//   File : KviMenuBar.cpp
//   Creation date : Wen Jun 21 2000 13:12:11 by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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


#include "KviMenuBar.h"
#include "KviApplication.h"
#include "KviLocale.h"
#include "KviMainWindow.h"
#include "KviMdiManager.h"
#include "KviIconManager.h"
#include "KviInternalCommand.h"
#include "kvi_settings.h"
#include "KviIrcUrl.h"
#include "KviConsoleWindow.h"
#include "KviKvsPopupMenu.h"
#include "KviMemory.h"
#include "KviModuleExtension.h"
#include "KviActionManager.h"
#include "KviCoreActionNames.h"
#include "KviKvsScript.h"
#include "KviTalPopupMenu.h"

KviMenuBar::KviMenuBar(KviMainWindow * par,const char * name)
      : KviTalMenuBar(par,name)
{
	setAutoFillBackground(false);
	m_pFrm = par;

	KviTalPopupMenu * pop = new KviTalPopupMenu(this,"KVIrc");
	connect(pop,SIGNAL(aboutToShow()),this,SLOT(setupMainPopup()));
#ifndef COMPILE_ON_MAC
	addDefaultItem("&KVIrc",pop);
#else
	// Qt/Mac creates already a "KVirc" menu item on its own, and we don't like double entries ;-)
	addDefaultItem("&IRC",pop);
#endif //COMPILE_ON_MAC
	m_pRecentServersPopup = new KviTalPopupMenu(this,"recentservers");
	connect(m_pRecentServersPopup,SIGNAL(aboutToShow()),this,SLOT(setupRecentServersPopup()));
	connect(m_pRecentServersPopup,SIGNAL(activated(int)),this,SLOT(newConnectionToServer(int)));

	m_pScriptItemList = 0;

	pop = new KviTalPopupMenu(this,"scripting");
	connect(pop,SIGNAL(aboutToShow()),this,SLOT(setupScriptingPopup()));
	addDefaultItem(__tr2qs("Scri&pting"),pop);

	pop = new KviTalPopupMenu(this,"tools");
	connect(pop,SIGNAL(aboutToShow()),this,SLOT(setupToolsPopup()));
	connect(pop,SIGNAL(activated(int)),this,SLOT(toolsPopupSelected(int)));
	addDefaultItem(__tr2qs("&Tools"),pop);

	m_pToolbarsPopup = new KviTalPopupMenu(this,"toolbars");
	connect(m_pToolbarsPopup,SIGNAL(aboutToShow()),this,SLOT(setupToolbarsPopup()));

	pop = new KviTalPopupMenu(this,"settings");
	connect(pop,SIGNAL(aboutToShow()),this,SLOT(setupSettingsPopup()));
	addDefaultItem(__tr2qs("&Settings"),pop);

	addDefaultItem(__tr2qs("&Window"),par->mdiManager()->windowPopup());

	pop = new KviTalPopupMenu(this,"help");
	connect(pop,SIGNAL(aboutToShow()),this,SLOT(setupHelpPopup()));
	connect(pop,SIGNAL(activated(int)),this,SLOT(actionTriggered(int)));
	addDefaultItem(__tr2qs("&Help"),pop);
}

KviMenuBar::~KviMenuBar()
{
	if(m_pScriptItemList)delete m_pScriptItemList;
}

void KviMenuBar::addDefaultItem(const QString &text,KviTalPopupMenu * pop)
{
	pop->menuAction()->setText(text);
	addAction(pop->menuAction());
}

void KviMenuBar::setupHelpPopup()
{
	KviTalPopupMenu * help = (KviTalPopupMenu *)sender();
	help->clear();

	ACTION_POPUP_ITEM(KVI_COREACTION_HELPINDEX,help)

	int id = help->insertItem(*(g_pIconManager->getSmallIcon(KviIconManager::Help)),__tr2qs("&Help Browser (Panel)"));
	help->setItemParameter(id,KVI_INTERNALCOMMAND_HELP_NEWSTATICWINDOW);


	id = help->insertItem(*(g_pIconManager->getSmallIcon(KviIconManager::Idea)),__tr2qs("&Tip of the Day"));
	help->setItemParameter(id,KVI_INTERNALCOMMAND_TIP_OPEN);
	help->insertSeparator();

	id = help->insertItem(*(g_pIconManager->getSmallIcon(KviIconManager::KVIrc)),__tr2qs("About &KVIrc"));

	help->setItemParameter(id,KVI_INTERNALCOMMAND_ABOUT_ABOUTKVIRC);
	help->insertSeparator();
	id = help->insertItem(*(g_pIconManager->getSmallIcon(KviIconManager::HomePage)),__tr2qs("KVIrc Home&page"));
	help->setItemParameter(id,KVI_INTERNALCOMMAND_KVIRC_HOMEPAGE);
	if(kvi_strEqualCIN(KviLocale::localeName(),"ru",2))
	{
		id = help->insertItem(*(g_pIconManager->getSmallIcon(KviIconManager::HomePage)),__tr2qs("KVIrc Russian Home&page"));
		help->setItemParameter(id,KVI_INTERNALCOMMAND_KVIRC_HOMEPAGE_RU);
	}
	if(kvi_strEqualCIN(KviLocale::localeName(),"fr",2))
	{
		id = help->insertItem(*(g_pIconManager->getSmallIcon(KviIconManager::HomePage)),__tr2qs("KVIrc French Home&page"));
		help->setItemParameter(id,KVI_INTERNALCOMMAND_KVIRC_HOMEPAGE_FR);
	}
	help->insertSeparator();
	id = help->insertItem(*(g_pIconManager->getSmallIcon(KviIconManager::Message)),__tr2qs("Subscribe to the Mailing List"));
	help->setItemParameter(id,KVI_INTERNALCOMMAND_OPENURL_KVIRC_MAILINGLIST);
	id = help->insertItem(*(g_pIconManager->getSmallIcon(KviIconManager::Bomb)),__tr2qs("Report a Bug / Propose Improvements"));
	help->setItemParameter(id,KVI_INTERNALCOMMAND_OPENURL_KVIRC_BUGTRACK);
	help->insertSeparator();
	id = help->insertItem(*(g_pIconManager->getSmallIcon(KviIconManager::Channel)),__tr2qs("Join KVIrc International Channel on Freenode"));
	help->setItemParameter(id,KVI_INTERNALCOMMAND_OPENURL_KVIRC_ON_FREENODE);
	id = help->insertItem(*(g_pIconManager->getSmallIcon(KviIconManager::Channel)),__tr2qs("Join KVIrc International Channel on IRCNet"));
	help->setItemParameter(id,KVI_INTERNALCOMMAND_OPENURL_KVIRC_ON_IRCNET);
	if(kvi_strEqualCIN(KviLocale::localeName(),"it",2))
	{
		// join #kvirc.net on azzurra
		id = help->insertItem(*(g_pIconManager->getSmallIcon(KviIconManager::Channel)),__tr2qs("Join KVIrc Italian Channel on AzzurraNet"));
		help->setItemParameter(id,KVI_INTERNALCOMMAND_OPENURL_KVIRC_IT_ON_AZZURRA);
	}
	if(kvi_strEqualCIN(KviLocale::localeName(),"fr",2))
	{
		// join #kvirc-fr on freenode
		id = help->insertItem(*(g_pIconManager->getSmallIcon(KviIconManager::Channel)),__tr2qs("Join KVIrc French Channel on Freenode"));
		help->setItemParameter(id,KVI_INTERNALCOMMAND_OPENURL_KVIRC_FR_ON_FREENODE);
		// join #kvirc on europnet
		id = help->insertItem(*(g_pIconManager->getSmallIcon(KviIconManager::Channel)),__tr2qs("Join KVIrc French Channel on EuropNet"));
		help->setItemParameter(id,KVI_INTERNALCOMMAND_OPENURL_KVIRC_FR_ON_EUROPNET);
	}
}

void KviMenuBar::actionTriggered(int id)
{
	KviTalPopupMenu * help = (KviTalPopupMenu *)sender();
	m_pFrm->executeInternalCommand(help->itemParameter(id));
}

void KviMenuBar::setupSettingsPopup()
{

	KviTalPopupMenu * opt = (KviTalPopupMenu *)sender();
	opt->clear();

	opt->insertItem(__tr2qs("Toolbars"),m_pToolbarsPopup);

	int id = opt->insertItem(*(g_pIconManager->getSmallIcon(KviIconManager::StatusBar)),__tr2qs("Show StatusBar"),m_pFrm,SLOT(toggleStatusBar()));
	opt->setItemChecked(id,m_pFrm->mainStatusBar());


	opt->insertSeparator();
	// FIXME: #warning "Toggle these items on the fly ?"
	ACTION_POPUP_ITEM(KVI_COREACTION_GENERALOPTIONS,opt)
	ACTION_POPUP_ITEM(KVI_COREACTION_THEMEOPTIONS,opt)
	ACTION_POPUP_ITEM(KVI_COREACTION_EDITREGUSERS,opt)
	ACTION_POPUP_ITEM(KVI_COREACTION_TOOLBAREDITOR,opt)
	opt->insertSeparator();
	ACTION_POPUP_ITEM(KVI_COREACTION_SERVEROPTIONS,opt)
	opt->insertSeparator();
	ACTION_POPUP_ITEM(KVI_COREACTION_MANAGETHEMES,opt)
	ACTION_POPUP_ITEM(KVI_COREACTION_MANAGEADDONS,opt)

	// In 2010 this is not professional :D
	// The app must take care of saving user options whenever they are changed.
	//
	// If you're playing with unstable stuff then use /options.save to obtain
	// the same effect.
	// 
	//opt->insertSeparator();
	//opt->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_FLOPPY)),__tr2qs("&Save Configuration"),g_pApp,SLOT(saveConfiguration()));
}

void KviMenuBar::setupScriptingPopup()
{
	KviTalPopupMenu * script = (KviTalPopupMenu *)sender();
	script->clear();

	ACTION_POPUP_ITEM(KVI_COREACTION_ACTIONEDITOR,script)
	ACTION_POPUP_ITEM(KVI_COREACTION_ALIASEDITOR,script)
	ACTION_POPUP_ITEM(KVI_COREACTION_CLASSEDITOR,script)
	ACTION_POPUP_ITEM(KVI_COREACTION_EVENTEDITOR,script)
	ACTION_POPUP_ITEM(KVI_COREACTION_POPUPEDITOR,script)
	ACTION_POPUP_ITEM(KVI_COREACTION_RAWEDITOR,script)
	script->insertSeparator();
	ACTION_POPUP_ITEM(KVI_COREACTION_CODETESTER,script)
	script->insertSeparator();
	ACTION_POPUP_ITEM(KVI_COREACTION_EXECUTEKVS,script)

	script->insertSeparator();
	script->insertItem(*(g_pIconManager->getSmallIcon(KviIconManager::TextExclamative)),__tr2qs("Restore &Default Script..."),g_pApp,SLOT(restoreDefaultScript()));
}

void KviMenuBar::setupMainPopup()
{
	KviTalPopupMenu * main = (KviTalPopupMenu *)sender();
	main->clear();

	ACTION_POPUP_ITEM(KVI_COREACTION_NEWIRCCONTEXT,main)

	main->insertItem(*(g_pIconManager->getSmallIcon(KviIconManager::World)),__tr2qs("New &Connection To"),m_pRecentServersPopup);

	if(m_pFrm->activeContext())
		if(m_pFrm->activeContext()->state()==KviIrcContext::Connected)
		{
			int id = main->insertItem(*(g_pIconManager->getSmallIcon(KviIconManager::Quit)),__tr2qs("Disconnect"),m_pFrm,SLOT(executeInternalCommand(int)));
			main->setItemParameter(id,KVI_INTERNALCOMMAND_QUIT);
		}

	main->insertSeparator();

	// FIXME: Add a "Dock to tray" icon if the tray is not visible (or show tray icon or whatever)

// Qt/Mac creates a Quit item on its own <= this is bad
#ifndef COMPILE_ON_MAC
	main->insertSeparator();

	main->insertItem(*(g_pIconManager->getSmallIcon(KviIconManager::QuitApp)),__tr2qs("&Quit"),g_pMainWindow,SLOT(close()));
#endif //COMPILE_ON_MAC
}

void KviMenuBar::setupRecentServersPopup()
{
	KviTalPopupMenu * m = (KviTalPopupMenu *)sender();
	g_pApp->fillRecentServersPopup(m);
	m->insertSeparator();
	m->insertItem(*(g_pIconManager->getSmallIcon(KviIconManager::Server)),__tr2qs("&Other..."));
}

void KviMenuBar::newConnectionToServer(int id)
{
	QString text = m_pRecentServersPopup->text(id);
	if(!text.isEmpty())
	{
		if(text == __tr2qs("&Other..."))
		{
			KviKvsScript::run("options.edit OptionsWidget_servers",m_pFrm->firstConsole());
		} else {
			text.replace(QString("&"),QString(""));
			KviCString szCommand;
			if(KviIrcUrl::parse(text.toUtf8().data(),szCommand,KVI_IRCURL_CONTEXT_FIRSTFREE))
			{
				KviKvsScript::run(szCommand.ptr(),g_pApp->activeConsole());
			}
		}
	}
}

void KviMenuBar::setupToolsPopup()
{
	KviTalPopupMenu * m = (KviTalPopupMenu *)sender();
	if(!m)return;

	m->clear();

	KviModuleExtensionDescriptorList * l = g_pModuleExtensionManager->getExtensionList("tool");
	if(l)
	{
		for(KviModuleExtensionDescriptor * d = l->first();d;d = l->next())
		{
			int id;
			if(d->icon())
			{
				id = m->insertItem(*(d->icon()),d->visibleName());
			} else {
				id = m->insertItem(d->visibleName());
			}
			m->setItemParameter(id,d->id());
		}
	}
	m->insertSeparator();
	ACTION_POPUP_ITEM(KVI_COREACTION_SOCKETSPY,m)
	ACTION_POPUP_ITEM(KVI_COREACTION_NETWORKLINKS,m)
	ACTION_POPUP_ITEM(KVI_COREACTION_CHANNELLIST,m)
	m->insertSeparator();

	ACTION_POPUP_ITEM(KVI_COREACTION_JOINCHANNELS,m)

	m->insertSeparator();

	ACTION_POPUP_ITEM(KVI_COREACTION_SCREENSHOT,m)

	// moved the old tools here
	m->insertItem(*(g_pIconManager->getSmallIcon(KviIconManager::IconManager)),__tr2qs("Show &Icon Table"),g_pIconManager,SLOT(showIconWidget()));
#ifdef COMPILE_KDE_SUPPORT
	int id;
	id = m->insertItem(*(g_pIconManager->getSmallIcon(KviIconManager::Terminal)),__tr2qs("Open &Terminal"),m_pFrm,SLOT(executeInternalCommand(int)));
	m->setItemParameter(id,KVI_INTERNALCOMMAND_TERM_OPEN);
#endif

	KviPointerList<KviAction> list;
	KviActionManager::instance()->listActionsByCategory("tools",&list);
	if(!list.isEmpty())
	{
		m->insertSeparator();
		for(KviAction * ac = list.first();ac;ac = list.next())
			ac->addToPopupMenu(m);
	}
}

void KviMenuBar::toolsPopupSelected(int id)
{
	KviTalPopupMenu * m = (KviTalPopupMenu *)sender();
	if(!m) return;
	int idext = m->itemParameter(id);
	g_pModuleExtensionManager->allocateExtension("tool",idext,m_pFrm->firstConsole());
}

void KviMenuBar::setupToolbarsPopup()
{
	m_pFrm->fillToolBarsPopup(m_pToolbarsPopup);
}

//
// Script items
//

int KviMenuBar::getDefaultItemRealIndex(int iDefaultIndex)
{
	if(iDefaultIndex < 0)
		iDefaultIndex = 0;
	if(iDefaultIndex > actions().count())
		iDefaultIndex = actions().count();

	return iDefaultIndex;
}

KviScriptMenuBarItem * KviMenuBar::findMenu(const QString &text)
{
	if(!m_pScriptItemList)
		return 0;
	for(KviScriptMenuBarItem * i = m_pScriptItemList->first();i;i = m_pScriptItemList->next())
	{
		if(KviQString::equalCI(text,i->szText))
			return i;
	}
	return 0;
}

KviScriptMenuBarItem * KviMenuBar::findMenu(KviKvsPopupMenu * p)
{
	if(!m_pScriptItemList)return 0;
	for(KviScriptMenuBarItem * i = m_pScriptItemList->first();i;i = m_pScriptItemList->next())
	{
		if(i->pPopup == p) return i;
	}
	return 0;
}

bool KviMenuBar::removeMenu(const QString &text)
{
	KviScriptMenuBarItem * i = findMenu(text);
	if(i)
	{
		disconnect(i->pPopup,SIGNAL(destroyed()),this,SLOT(menuDestroyed()));
		removeAction(i->pPopup->menuAction());
		m_pScriptItemList->removeRef(i);
		return true;
	}
	return false;
}

void KviMenuBar::menuDestroyed()
{
	KviScriptMenuBarItem * i = findMenu(((KviKvsPopupMenu *)sender()));
	if(i)
	{
		// No need to remove the associated action: qt already did it (ticket #931)
		m_pScriptItemList->removeRef(i);
	}
}

void KviMenuBar::addMenu(const QString &text,KviKvsPopupMenu * p,int index)
{
	if(!m_pScriptItemList)
	{
		m_pScriptItemList = new KviPointerList<KviScriptMenuBarItem>;
		m_pScriptItemList->setAutoDelete(true);
	}

	KviScriptMenuBarItem * it = new KviScriptMenuBarItem;
	it->szText = text;
	it->szPopupName = p->objectName();
	it->pPopup = p;
	it->pPopup->menuAction()->setText(text);
	if(index == -1 || index >= actions().count())
	{
		addAction(it->pPopup->menuAction());
	} else {
		insertAction(actions().value(index), it->pPopup->menuAction());
	}

	connect(p,SIGNAL(destroyed()),this,SLOT(menuDestroyed()));
	m_pScriptItemList->append(it);
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "KviMenuBar.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
