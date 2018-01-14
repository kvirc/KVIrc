#ifndef _KVI_ACTIONMANAGER_H_
#define _KVI_ACTIONMANAGER_H_
//=============================================================================
//
//   File : KviActionManager.h
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

#include "kvi_settings.h"
#include "KviAction.h"
#include "KviPointerHashTable.h"

#include <QObject>

class KviActionDrawer;
class KviCustomToolBar;

class KVIRC_API KviActionManager : public QObject
{
	friend class KviActionDrawer;
	friend class CustomizeToolBarsDialog; // this resides in a module!
	friend class KviCustomToolBar;
	friend class KviMainWindow;
	Q_OBJECT
public:
	KviActionManager();
	~KviActionManager();

protected:
	static KviActionManager * m_pInstance;
	KviPointerHashTable<QString, KviAction> * m_pActions = nullptr;
	KviPointerHashTable<QString, KviActionCategory> * m_pCategories = nullptr;
	static bool m_bCustomizingToolBars;

	// action categories
	static KviActionCategory * m_pCategoryIrc;
	static KviActionCategory * m_pCategoryGeneric;
	static KviActionCategory * m_pCategorySettings;
	static KviActionCategory * m_pCategoryScripting;
	static KviActionCategory * m_pCategoryGUI;
	static KviActionCategory * m_pCategoryChannel;
	static KviActionCategory * m_pCategoryTools;
	// internal, current toolbar to be edited (only when customizing)
	static KviCustomToolBar * m_pCurrentToolBar;
	bool m_bCoreActionsRegistered = false;

public:
	static void init();
	static void done();
	static KviActionManager * instance() { return m_pInstance; }
	static void loadAllAvailableActions();
	static bool customizingToolBars() { return m_bCustomizingToolBars; }
	static KviActionCategory * categoryIrc() { return m_pCategoryIrc; }
	static KviActionCategory * categoryGeneric() { return m_pCategoryGeneric; }
	static KviActionCategory * categorySettings() { return m_pCategorySettings; }
	static KviActionCategory * categoryScripting() { return m_pCategoryScripting; }
	static KviActionCategory * categoryGUI() { return m_pCategoryGUI; }
	static KviActionCategory * categoryChannel() { return m_pCategoryChannel; }
	static KviActionCategory * categoryTools() { return m_pCategoryTools; }

	KviPointerHashTable<QString, KviAction> * actions() { return m_pActions; }
	KviActionCategory * category(const QString & szName);
	KviPointerHashTable<QString, KviActionCategory> * categories() { return m_pCategories; }

	void killAllKvsUserActions();

	static KviCustomToolBar * currentToolBar() { return m_pCurrentToolBar; }
	KviAction * getAction(const QString & szName);
	void listActionsByCategory(const QString & szCatName, KviPointerList<KviAction> * pBuffer);
	QString nameForAutomaticAction(const QString & szTemplate);
	bool coreActionExists(const QString & szName);

	void load(const QString & szFileName);
	void save(const QString & szFileName);

	bool registerAction(KviAction * a);
	bool unregisterAction(const QString & szName);

	void emitRemoveActionsHintRequest();

protected:
	void setCurrentToolBar(KviCustomToolBar * t);
	KviAction * findAction(const QString & szName) { return m_pActions->find(szName); }
	void customizeToolBarsDialogCreated();
	void customizeToolBarsDialogDestroyed();
	void tryFindCurrentToolBar();
	void delayedRegisterAccelerators(); // this is called ONCE by KviMainWindow, at startup
protected slots:
	void actionDestroyed();
signals:
	void beginCustomizeToolBars();
	void endCustomizeToolBars();
	void currentToolBarChanged();    // emitted only when customizing!
	void removeActionsHintRequest(); // connected by the KviCustomToolBarDialog to flash the trashcan
	                                 // when the user tries to remove an action from the toolbar
	                                 // and it fails to drag it on the trashcan
};

#define ACTION_POPUP_ITEM(__name, __popup)                               \
	{                                                                    \
		KviAction * a = KviActionManager::instance()->getAction(__name); \
		if(a)                                                            \
			a->addToPopupMenu(__popup);                                  \
	}

#endif //!_KVI_ACTIONMANAGER_H_
