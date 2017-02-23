//=============================================================================
//
//   File : KviMenuBar.cpp
//   Creation date : Wen Jun 21 2000 13:12:11 by Szymon Stefanek
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

#include "KviMenuBar.h"
#include "KviApplication.h"
#include "KviLocale.h"
#include "KviMainWindow.h"
#include "KviWindowStack.h"
#include "KviIconManager.h"
#include "KviInternalCommand.h"
#include "KviIrcUrl.h"
#include "KviConsoleWindow.h"
#include "KviKvsPopupMenu.h"
#include "KviMemory.h"
#include "KviModuleExtension.h"
#include "KviOptions.h"
#include "KviActionManager.h"
#include "KviCoreActionNames.h"
#include "KviKvsScript.h"
#include "KviShortcut.h"
#include "KviOptions.h"

#include <QKeySequence>
#include <QToolButton>
#include <QMenu>

KviMenuBar::KviMenuBar(KviMainWindow * par, const char * name)
    : KviTalMenuBar(par, name)
{
	setAutoFillBackground(false);
	m_pFrm = par;

	m_pRecentServersPopup = new QMenu("recentservers", this);
	connect(m_pRecentServersPopup, SIGNAL(aboutToShow()), this, SLOT(updateRecentServersPopup()));
	connect(m_pRecentServersPopup, SIGNAL(triggered(QAction *)), this, SLOT(newConnectionToServer(QAction *)));

	m_pModulesToolsPopup = new QMenu("modulestools", this);
	connect(m_pModulesToolsPopup, SIGNAL(aboutToShow()), this, SLOT(updateModulesToolsPopup()));
	connect(m_pModulesToolsPopup, SIGNAL(triggered(QAction *)), this, SLOT(modulesToolsTriggered(QAction *)));

	m_pActionsToolsPopup = new QMenu("actionstools", this);
	connect(m_pActionsToolsPopup, SIGNAL(aboutToShow()), this, SLOT(updateActionsToolsPopup()));

	QMenu * pop = new QMenu("KVIrc", this);
	setupMainPopup(pop);
	connect(pop, SIGNAL(aboutToShow()), this, SLOT(updateMainPopup()));
#ifndef COMPILE_ON_MAC
	addDefaultItem("&KVIrc", pop);
#else
	// Qt/Mac creates already a "KVirc" menu item on its own, and we don't like double entries ;-)
	addDefaultItem("&IRC", pop);
#endif //COMPILE_ON_MAC

	m_pScriptItemList = nullptr;

	pop = new QMenu("scripting", this);
	setupScriptingPopup(pop);
	addDefaultItem(__tr2qs("Scri&pting"), pop);

	pop = new QMenu("tools", this);
	setupToolsPopup(pop);
	addDefaultItem(__tr2qs("&Tools"), pop);
	connect(pop, SIGNAL(aboutToShow()), this, SLOT(updateToolsPopup()));

	m_pToolbarsPopup = new QMenu("toolbars", this);
	connect(m_pToolbarsPopup, SIGNAL(aboutToShow()), this, SLOT(updateToolbarsPopup()));

	pop = new QMenu("settings", this);
	setupSettingsPopup(pop);
	connect(pop, SIGNAL(aboutToShow()), this, SLOT(updateSettingsPopup()));
	addDefaultItem(__tr2qs("&Settings"), pop);

	addDefaultItem(__tr2qs("&Window"), par->windowStack()->windowPopup());

	pop = new QMenu("help", this);
	setupHelpPopup(pop);
	connect(pop, SIGNAL(triggered(QAction *)), this, SLOT(actionTriggered(QAction *)));
	addDefaultItem(__tr2qs("&Help"), pop);

	m_pCloseActiveWindowButton = new QToolButton(this);
	m_pCloseActiveWindowButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_TitleBarCloseButton));
	m_pCloseActiveWindowButton->setAutoRaise(true);
	m_pCloseActiveWindowButton->setToolTip(__tr2qs("Close"));
	connect(m_pCloseActiveWindowButton, SIGNAL(clicked()), g_pMainWindow, SLOT(closeActiveWindow()));
	setCornerWidget(m_pCloseActiveWindowButton);
}


KviMenuBar::~KviMenuBar()
{
	if(m_pScriptItemList)
		delete m_pScriptItemList;
}

void KviMenuBar::addDefaultItem(const QString & text, QMenu * pop)
{
	pop->menuAction()->setText(text);
	addAction(pop->menuAction());
}

void KviMenuBar::setupHelpPopup(QMenu * pop)
{
	QMenu * help = pop ? pop : (QMenu *)sender();
	help->clear();

	ACTION_POPUP_ITEM(KVI_COREACTION_HELPINDEX, help)

	QAction * pAction = help->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Idea)), __tr2qs("&Tip of the Day"));
	pAction->setData(KVI_INTERNALCOMMAND_TIP_OPEN);
	help->addSeparator();

	pAction = help->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Info)), __tr2qs("About &KVIrc"));
	pAction->setData(KVI_INTERNALCOMMAND_ABOUT_ABOUTKVIRC);
	pAction->setMenuRole(QAction::AboutRole);

	help->addSeparator();
	pAction = help->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::HomePage)), __tr2qs("KVIrc Home&page"));
	pAction->setData(KVI_INTERNALCOMMAND_KVIRC_HOMEPAGE);
	if(kvi_strEqualCIN(KviLocale::instance()->localeName(), "ru", 2))
	{
		pAction = help->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::HomePage)), __tr2qs("KVIrc Russian Home&page"));
		pAction->setData(KVI_INTERNALCOMMAND_KVIRC_HOMEPAGE_RU);
	}
	help->addSeparator();
	pAction = help->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Message)), __tr2qs("Subscribe to the Mailing List"));
	pAction->setData(KVI_INTERNALCOMMAND_OPENURL_KVIRC_MAILINGLIST);
	pAction = help->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Bug)), __tr2qs("Report a Bug / Propose Improvements"));
	pAction->setData(KVI_INTERNALCOMMAND_OPENURL_KVIRC_BUGTRACK);
	help->addSeparator();
	pAction = help->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Channel)), __tr2qs("Join KVIrc's Channel on freenode"));
	pAction->setData(KVI_INTERNALCOMMAND_JOIN_KVIRC_ON_FREENODE);
}

void KviMenuBar::actionTriggered(QAction * pAction)
{
	bool bOk = false;
	int id = pAction->data().toInt(&bOk);
	if(bOk)
		m_pFrm->executeInternalCommand(id);
}

void KviMenuBar::actionTriggered(bool)
{
	QAction * pAction = (QAction *)sender();
	if(!pAction)
		return;

	actionTriggered(pAction);
}

void KviMenuBar::updateSettingsPopup()
{
	m_pStatusBarAction->setChecked(m_pFrm->mainStatusBar());
#ifndef COMPILE_ON_MAC
	m_pMenuBarAction->setChecked(true);
#endif
	m_pWindowListAction->setChecked(KVI_OPTION_BOOL(KviOption_boolUseTreeWindowList));
}

void KviMenuBar::setupSettingsPopup(QMenu * pop)
{
	QMenu * opt = pop ? pop : (QMenu *)sender();
	opt->clear();

	QAction * pAction = opt->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Toolbar)), __tr2qs("Toolbars"));
	pAction->setMenu(m_pToolbarsPopup);

#ifndef COMPILE_ON_MAC
	m_pMenuBarAction = opt->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::MenuBar)), __tr2qs("Show Menu Bar"), m_pFrm, SLOT(toggleMenuBar()));
	// must be added to the main window or it stops working when the menu is hidden :/
	g_pMainWindow->addAction(m_pMenuBarAction);
	m_pMenuBarAction->setCheckable(true);
	m_pMenuBarAction->setShortcut(QKeySequence::fromString(KVI_SHORTCUTS_TOGGLE_MENU_BAR));
#endif

	m_pStatusBarAction = opt->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::StatusBar)), __tr2qs("Show Status Bar"), m_pFrm, SLOT(toggleStatusBar()));
	m_pStatusBarAction->setCheckable(true);

	m_pWindowListAction = opt->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::TreeWindowList)), __tr2qs("Show Tree Window List"), m_pFrm, SLOT(toggleWindowList()));
	g_pMainWindow->addAction(m_pWindowListAction);
	m_pWindowListAction->setCheckable(true);
	m_pWindowListAction->setShortcut(QKeySequence::fromString(KVI_SHORTCUTS_TOGGLE_TREE_LIST));

	opt->addSeparator();
	// FIXME: #warning "Toggle these items on the fly ?"
	ACTION_POPUP_ITEM(KVI_COREACTION_GENERALOPTIONS, opt)
	ACTION_POPUP_ITEM(KVI_COREACTION_THEMEOPTIONS, opt)
	ACTION_POPUP_ITEM(KVI_COREACTION_EDITREGUSERS, opt)
	ACTION_POPUP_ITEM(KVI_COREACTION_TOOLBAREDITOR, opt)
	opt->addSeparator();
	ACTION_POPUP_ITEM(KVI_COREACTION_SERVEROPTIONS, opt)
	opt->addSeparator();
	ACTION_POPUP_ITEM(KVI_COREACTION_MANAGETHEMES, opt)
	ACTION_POPUP_ITEM(KVI_COREACTION_MANAGEADDONS, opt)
}

void KviMenuBar::setupScriptingPopup(QMenu * pop)
{
	QMenu * script = pop ? pop : (QMenu *)sender();
	script->clear();

	ACTION_POPUP_ITEM(KVI_COREACTION_ACTIONEDITOR, script)
	ACTION_POPUP_ITEM(KVI_COREACTION_ALIASEDITOR, script)
	ACTION_POPUP_ITEM(KVI_COREACTION_CLASSEDITOR, script)
	ACTION_POPUP_ITEM(KVI_COREACTION_EVENTEDITOR, script)
	ACTION_POPUP_ITEM(KVI_COREACTION_POPUPEDITOR, script)
	ACTION_POPUP_ITEM(KVI_COREACTION_RAWEDITOR, script)
	script->addSeparator();
	ACTION_POPUP_ITEM(KVI_COREACTION_CODETESTER, script)
	script->addSeparator();
	ACTION_POPUP_ITEM(KVI_COREACTION_EXECUTEKVS, script)

	script->addSeparator();
	script->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Warning)), __tr2qs("Restore &Default Script..."), g_pApp, SLOT(restoreDefaultScript()));
}

void KviMenuBar::updateMainPopup()
{
	m_pDisconnectAction->setEnabled(
	    m_pFrm->activeContext() && m_pFrm->activeContext()->state() == KviIrcContext::Connected);
}

void KviMenuBar::setupMainPopup(QMenu * pop)
{
	QMenu * main = pop ? pop : (QMenu *)sender();
	main->clear();

	ACTION_POPUP_ITEM(KVI_COREACTION_NEWIRCCONTEXT, main)

	QAction * pAction = main->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::World)), __tr2qs("New &Connection to"));
	pAction->setMenu(m_pRecentServersPopup);

	m_pDisconnectAction = main->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Quit)), __tr2qs("Disconnect"), this, SLOT(actionTriggered(bool)));
	m_pDisconnectAction->setData(KVI_INTERNALCOMMAND_QUIT);

// FIXME: Add a "Dock to tray" icon if the tray is not visible (or show tray icon or whatever)

// Qt/Mac creates a Quit item on its own <= this is bad
#ifndef COMPILE_ON_MAC
	main->addSeparator();

	ACTION_POPUP_ITEM(KVI_COREACTION_QUITKVIRC, main)
#endif //COMPILE_ON_MAC
}

void KviMenuBar::updateRecentServersPopup()
{
	QMenu * m = (QMenu *)sender();
	g_pApp->fillRecentServersPopup(m);
	m->addSeparator();
	m->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Server)), __tr2qs("&Other..."));
}

void KviMenuBar::newConnectionToServer(QAction * pAction)
{
	QString text = pAction->text();
	if(!text.isEmpty())
	{
		if(text == __tr2qs("&Other..."))
		{
			KviKvsScript::run("options.edit OptionsWidget_servers", m_pFrm->firstConsole());
		}
		else
		{
			text.replace(QString("&"), QString(""));
			KviCString szCommand;
			if(KviIrcUrl::parse(text.toUtf8().data(), szCommand, KVI_IRCURL_CONTEXT_FIRSTFREE))
			{
				KviKvsScript::run(szCommand.ptr(), g_pApp->activeConsole());
			}
		}
	}
}

void KviMenuBar::updateToolsPopup()
{
	KviModuleExtensionDescriptorList * l = g_pModuleExtensionManager->getExtensionList("tool");
	m_pModulesToolsAction->setVisible(l && !(l->isEmpty()));

	KviPointerList<KviAction> list;
	KviActionManager::instance()->listActionsByCategory("tools", &list);
	m_pActionsToolsAction->setVisible(!list.isEmpty());
}

void KviMenuBar::updateModulesToolsPopup()
{
	m_pModulesToolsPopup->clear();

	QAction * pAction = nullptr;
	KviModuleExtensionDescriptorList * l = g_pModuleExtensionManager->getExtensionList("tool");
	if(l)
	{
		for(KviModuleExtensionDescriptor * d = l->first(); d; d = l->next())
		{
			if(d->icon())
			{
				pAction = m_pModulesToolsPopup->addAction(*(d->icon()), d->visibleName());
			}
			else
			{
				pAction = m_pModulesToolsPopup->addAction(d->visibleName());
			}
			pAction->setData(d->id());
		}
	}
}

void KviMenuBar::updateActionsToolsPopup()
{
	m_pActionsToolsPopup->clear();

	KviPointerList<KviAction> list;
	KviActionManager::instance()->listActionsByCategory("tools", &list);
	if(!list.isEmpty())
	{
		m_pActionsToolsPopup->addSeparator();
		for(KviAction * ac = list.first(); ac; ac = list.next())
			ac->addToPopupMenu(m_pActionsToolsPopup);
	}
}

void KviMenuBar::setupToolsPopup(QMenu * pop)
{
	QMenu * m = pop ? pop : (QMenu *)sender();
	if(!m)
		return;

	m->clear();

	ACTION_POPUP_ITEM(KVI_COREACTION_FILETRANSFER, m)
	ACTION_POPUP_ITEM(KVI_COREACTION_IOGRAPH, m)
	ACTION_POPUP_ITEM(KVI_COREACTION_LOGVIEWER, m)
	ACTION_POPUP_ITEM(KVI_COREACTION_SHAREDFILES, m)
	ACTION_POPUP_ITEM(KVI_COREACTION_URLLIST, m)

	m->addSeparator();

	ACTION_POPUP_ITEM(KVI_COREACTION_SOCKETSPY, m)
	ACTION_POPUP_ITEM(KVI_COREACTION_NETWORKLINKS, m)
	ACTION_POPUP_ITEM(KVI_COREACTION_CHANNELLIST, m)
	m->addSeparator();

	ACTION_POPUP_ITEM(KVI_COREACTION_JOINCHANNELS, m)

	m->addSeparator();

	ACTION_POPUP_ITEM(KVI_COREACTION_SCREENSHOT, m)

	// moved the old tools here
	m->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::IconManager)), __tr2qs("Show &Icon Table"), g_pIconManager, SLOT(showIconWidget()));
#ifdef COMPILE_KDE4_SUPPORT
	QAction * pAction = nullptr;
	pAction = m->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Terminal)), __tr2qs("Open &Terminal"), this, SLOT(actionTriggered(bool)));
	pAction->setData(KVI_INTERNALCOMMAND_TERM_OPEN);
#endif

	m->addSeparator();

	m_pModulesToolsAction = m->addAction(__tr2qs("Module Tools"));
	m_pModulesToolsAction->setMenu(m_pModulesToolsPopup);

	m_pActionsToolsAction = m->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Action)), __tr2qs("Action Tools"));
	m_pActionsToolsAction->setMenu(m_pActionsToolsPopup);
}

void KviMenuBar::modulesToolsTriggered(QAction * pAction)
{
	bool bOk = false;
	int idext = pAction->data().toInt(&bOk);
	if(bOk)
		g_pModuleExtensionManager->allocateExtension("tool", idext, m_pFrm->firstConsole());
}

void KviMenuBar::updateToolbarsPopup()
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

KviScriptMenuBarItem * KviMenuBar::findMenu(const QString & text)
{
	if(!m_pScriptItemList)
		return nullptr;
	for(KviScriptMenuBarItem * i = m_pScriptItemList->first(); i; i = m_pScriptItemList->next())
	{
		if(KviQString::equalCI(text, i->szText))
			return i;
	}
	return nullptr;
}

KviScriptMenuBarItem * KviMenuBar::findMenu(KviKvsPopupMenu * p)
{
	if(!m_pScriptItemList)
		return nullptr;
	for(KviScriptMenuBarItem * i = m_pScriptItemList->first(); i; i = m_pScriptItemList->next())
	{
		if(i->pPopup == p)
			return i;
	}
	return nullptr;
}

bool KviMenuBar::removeMenu(const QString & text)
{
	KviScriptMenuBarItem * i = findMenu(text);
	if(i)
	{
		disconnect(i->pPopup, SIGNAL(destroyed()), this, SLOT(menuDestroyed()));
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

void KviMenuBar::addMenu(const QString & text, KviKvsPopupMenu * p, int index)
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
	}
	else
	{
		insertAction(actions().value(index), it->pPopup->menuAction());
	}

	connect(p, SIGNAL(destroyed()), this, SLOT(menuDestroyed()));
	m_pScriptItemList->append(it);
}
