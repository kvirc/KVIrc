//=============================================================================
//
//   File : KviActionManager.cpp
//   Creation date : Sun 21 Nov 2004 03:37:57 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2004-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "KviActionManager.h"
#include "KviModuleManager.h"
#include "KviCoreActions.h"
#include "KviCustomToolBar.h"
#include "KviCustomToolBarManager.h"
#include "KviApplication.h"
#include "KviLocale.h"
#include "KviKvsUserAction.h"
#include "KviConfigurationFile.h"
#include "KviMainWindow.h"

KviActionManager * KviActionManager::m_pInstance = nullptr;
KviActionCategory * KviActionManager::m_pCategoryIrc = nullptr;
KviActionCategory * KviActionManager::m_pCategoryGeneric = nullptr;
KviActionCategory * KviActionManager::m_pCategorySettings = nullptr;
KviActionCategory * KviActionManager::m_pCategoryScripting = nullptr;
KviActionCategory * KviActionManager::m_pCategoryGUI = nullptr;
KviActionCategory * KviActionManager::m_pCategoryChannel = nullptr;
KviActionCategory * KviActionManager::m_pCategoryTools = nullptr;
bool KviActionManager::m_bCustomizingToolBars = false;
KviCustomToolBar * KviActionManager::m_pCurrentToolBar = nullptr;
// KviCoreActions.cpp
extern void register_core_actions(KviActionManager *);

KviActionManager::KviActionManager()
    : QObject()
{
	m_pActions = new KviPointerHashTable<QString, KviAction>(101);
	m_pActions->setAutoDelete(false);

	m_pCategories = new KviPointerHashTable<QString, KviActionCategory>(17, false);
	m_pCategories->setAutoDelete(true);

#define CATEGORY(__var, __name, __vname, __descr)            \
	__var = new KviActionCategory(__name, __vname, __descr); \
	m_pCategories->replace(__name, __var)

	CATEGORY(m_pCategoryIrc, "irc", __tr2qs("IRC"), __tr2qs("IRC context related actions"));
	CATEGORY(m_pCategoryGeneric, "generic", __tr2qs("Generic"), __tr2qs("Generic actions"));
	CATEGORY(m_pCategorySettings, "settings", __tr2qs("Settings"), __tr2qs("Actions related to settings"));
	CATEGORY(m_pCategoryScripting, "scripting", __tr2qs("Scripting"), __tr2qs("Scripting related actions"));
	CATEGORY(m_pCategoryGUI, "gui", __tr2qs("GUI"), __tr2qs("Actions related to the Graphic User Interface"));
	CATEGORY(m_pCategoryChannel, "channel", __tr2qs("Channel"), __tr2qs("IRC channel related actions"));
	CATEGORY(m_pCategoryTools, "tools", __tr2qs("Tools"), __tr2qs("Actions that will appear in the \"Tools\" menu"));

	m_bCustomizingToolBars = false;
}

KviActionManager::~KviActionManager()
{
	// the customizeToolBars dialog has been already
	// destroyed since the module manager has already
	// killed all the modules at this point...

	KviPointerHashTableIterator<QString, KviAction> it(*m_pActions);
	while(KviAction * a = it.current())
	{
		disconnect(a, SIGNAL(destroyed()), this, SLOT(actionDestroyed()));
		++it;
	}
	delete m_pActions;

	delete m_pCategories;
}

void KviActionManager::load(const QString & szFileName)
{
	KviConfigurationFile cfg(szFileName, KviConfigurationFile::Read);

	KviConfigurationFileIterator it(*(cfg.dict()));
	while(it.current())
	{
		cfg.setGroup(it.currentKey());
		KviKvsUserAction * a = new KviKvsUserAction(this);
		if(a->load(&cfg))
			registerAction(a);
		else
			delete a;
		++it;
	}
}

void KviActionManager::save(const QString & szFileName)
{
	KviConfigurationFile cfg(szFileName, KviConfigurationFile::Write);
	cfg.clear();

	KviPointerHashTableIterator<QString, KviAction> it(*m_pActions);
	while(KviAction * a = it.current())
	{
		if(a->isKviUserActionNeverOverrideThis())
		{
			cfg.setGroup(a->name());
			((KviKvsUserAction *)a)->save(&cfg);
		}
		++it;
	}
}

void KviActionManager::killAllKvsUserActions()
{
	KviPointerList<KviKvsUserAction> dying;
	dying.setAutoDelete(true);

	KviPointerHashTableIterator<QString, KviAction> it(*m_pActions);
	while(KviAction * a = it.current())
	{
		if(a->isKviUserActionNeverOverrideThis())
		{
			dying.append(((KviKvsUserAction *)a));
		}
		++it;
	}

	dying.clear(); // bye :)
}

bool KviActionManager::coreActionExists(const QString & szName)
{
	KviAction * a = m_pActions->find(szName);
	if(a)
		return (!a->isKviUserActionNeverOverrideThis());
	return false;
}

QString KviActionManager::nameForAutomaticAction(const QString & szTemplate)
{
	QString szRet;

	int i = 1;
	do
	{
		szRet = QString("%1%2").arg(szTemplate).arg(i);
		i++;
	} while(m_pActions->find(szRet));

	return szRet;
}

void KviActionManager::emitRemoveActionsHintRequest()
{
	emit removeActionsHintRequest();
}

KviActionCategory * KviActionManager::category(const QString & szName)
{
	if(!szName.isEmpty())
	{
		KviActionCategory * c = m_pCategories->find(szName);
		if(c)
			return c;
	}
	return m_pCategoryGeneric;
}

void KviActionManager::customizeToolBarsDialogCreated()
{
	m_bCustomizingToolBars = true;
	m_pCurrentToolBar = KviCustomToolBarManager::instance()->firstExistingToolBar();
	if(m_pCurrentToolBar)
		m_pCurrentToolBar->update();
	emit beginCustomizeToolBars();
}

void KviActionManager::customizeToolBarsDialogDestroyed()
{
	m_bCustomizingToolBars = false;
	emit endCustomizeToolBars();
	if(m_pCurrentToolBar)
	{
		m_pCurrentToolBar->update();
		m_pCurrentToolBar = nullptr;
	}
	g_pApp->saveToolBars();
}

void KviActionManager::setCurrentToolBar(KviCustomToolBar * t)
{
	if(m_pCurrentToolBar == t)
		return;
	KviCustomToolBar * old = m_pCurrentToolBar;
	m_pCurrentToolBar = t;
	if(old)
		old->update();
	if(!m_pCurrentToolBar && m_bCustomizingToolBars)
		m_pCurrentToolBar = KviCustomToolBarManager::instance()->firstExistingToolBar();
	if(m_pCurrentToolBar)
		m_pCurrentToolBar->update();
	emit currentToolBarChanged();
}

void KviActionManager::loadAllAvailableActions()
{
	// make sure that the core actions are registered now
	if(!KviActionManager::instance()->m_bCoreActionsRegistered)
	{
		register_core_actions(KviActionManager::instance());
		KviActionManager::instance()->m_bCoreActionsRegistered = true;
	}
	g_pModuleManager->loadModulesByCaps("action");
}

void KviActionManager::init()
{
	if(!m_pInstance)
		m_pInstance = new KviActionManager();
}

void KviActionManager::done()
{
	if(m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

void KviActionManager::delayedRegisterAccelerators()
{
	KviPointerHashTableIterator<QString, KviAction> it(*m_pActions);
	while(KviAction * a = it.current())
	{
		a->registerAccelerator();
		++it;
	}
}

bool KviActionManager::registerAction(KviAction * a)
{
	if(m_pActions->find(a->name()))
		return false;
	connect(a, SIGNAL(destroyed()), this, SLOT(actionDestroyed()));
	m_pActions->insert(a->name(), a);
	if(g_pMainWindow)
		a->registerAccelerator(); // otherwise it is delayed!
	return true;
}

void KviActionManager::actionDestroyed()
{
	KviAction * a = (KviAction *)sender();
	m_pActions->remove(a->name());
}

bool KviActionManager::unregisterAction(const QString & szName)
{
	KviAction * a = m_pActions->find(szName);
	if(!a)
		return false;
	disconnect(a, SIGNAL(destroyed()), this, SLOT(actionDestroyed()));
	a->unregisterAccelerator();
	return m_pActions->remove(szName);
}

KviAction * KviActionManager::getAction(const QString & szName)
{
	KviAction * a = m_pActions->find(szName);
	if(a)
		return a;
	int idx = szName.indexOf('.');
	if(idx < 0)
	{
		// backward compatibility: try to lookup the name with the kvirc. prefix
		QString s = "kvirc.";
		s += szName;
		return m_pActions->find(s);
	}
	if((idx == 5) && (!m_bCoreActionsRegistered))
	{
		// the core actions are all like kvirc.name
		// so the dot is at position 5 (6th char)
		// the first requested core action will trigger this
		// the nice thing is that we will probably already have a frame when
		// the core actions are registered thus stuff like windowStack() can be accessed...
		if(szName.left(5) == "kvirc")
		{
			register_core_actions(this);
			m_bCoreActionsRegistered = true;
			a = m_pActions->find(szName);
			return a;
		}
	}
	// try to preload the module that might register this action...
	QString szModule = szName.left(idx);
	if(!g_pModuleManager->getModule(szModule))
		return nullptr;
	return m_pActions->find(szName);
}

void KviActionManager::listActionsByCategory(const QString & szCatName, KviPointerList<KviAction> * pBuffer)
{
	loadAllAvailableActions();
	KviActionCategory * pCat = category(szCatName);
	pBuffer->setAutoDelete(false);
	pBuffer->clear();
	if(!pCat)
		return;
	KviPointerHashTableIterator<QString, KviAction> it(*m_pActions);
	while(KviAction * a = it.current())
	{
		if(a->category() == pCat)
			pBuffer->append(a);
		++it;
	}
}
