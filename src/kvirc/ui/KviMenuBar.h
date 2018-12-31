#ifndef _KVI_MENUBAR_H_
#define _KVI_MENUBAR_H_
//=============================================================================
//
//   File : KviMenuBar.h
//   Creation date : Wen Jun 21 2000 13:11:24 by Szymon Stefanek
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

#include "kvi_settings.h"
#include "KviCString.h"
#include "KviPointerList.h"
#include "KviTalMenuBar.h"

class KviKvsPopupMenu;
class KviMainWindow;
class QAction;
class QMenu;

struct KviScriptMenuBarItem
{
	KviCString szPopupName;
	KviKvsPopupMenu * pPopup;
	KviCString szText;
};

class KVIRC_API KviMenuBar : public KviTalMenuBar
{
	Q_OBJECT
public:
	KviMenuBar(KviMainWindow * par, const char * name);
	~KviMenuBar();

protected:
	KviMainWindow * m_pFrm;
	QMenu * m_pRecentServersPopup;
	QMenu * m_pModulesToolsPopup;
	QMenu * m_pActionsToolsPopup;
	QMenu * m_pToolbarsPopup;
	KviPointerList<KviScriptMenuBarItem> * m_pScriptItemList = nullptr;
	// Dynamic actions
#ifndef COMPILE_ON_MAC
	QAction * m_pMenuBarAction;
#endif
	QAction * m_pStatusBarAction;
	QAction * m_pWindowListAction;
	QAction * m_pDisconnectAction;
	QAction * m_pModulesToolsAction;
	QAction * m_pActionsToolsAction;

protected:
	KviScriptMenuBarItem * findMenu(const QString & text);
	KviScriptMenuBarItem * findMenu(KviKvsPopupMenu * p);
	void addDefaultItem(const QString & text, QMenu * pop);

public:
	int getDefaultItemRealIndex(int iDefaultIndex);
	void addMenu(const QString & text, KviKvsPopupMenu * p, int index);
	bool removeMenu(const QString & text);
protected slots:
	void menuDestroyed();

	void setupMainPopup(QMenu * pop = nullptr);
	void setupSettingsPopup(QMenu * pop = nullptr);
	void setupHelpPopup(QMenu * pop = nullptr);
	void setupScriptingPopup(QMenu * pop = nullptr);
	void setupToolsPopup(QMenu * pop = nullptr);

	void updateMainPopup();
	void updateSettingsPopup();
	void updateRecentServersPopup();
	void updateToolbarsPopup();
	void updateModulesToolsPopup();
	void updateActionsToolsPopup();
	void updateToolsPopup();

	void newConnectionToServer(QAction * pAction);
	void modulesToolsTriggered(QAction * pAction);
	void actionTriggered(QAction * pAction);
	void actionTriggeredBool(bool);
};

#endif //_KVI_MENUBAR_H_
