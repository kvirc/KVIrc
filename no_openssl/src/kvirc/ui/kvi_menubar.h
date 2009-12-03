#ifndef _KVI_MENUBAR_H_
#define _KVI_MENUBAR_H_

//=============================================================================
//
//   File : kvi_menubar.h
//   Creation date : Wen Jun 21 2000 13:11:24 by Szymon Stefanek
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

#include "kvi_settings.h"
#include "kvi_pointerlist.h"
#include "kvi_string.h"
#include "kvi_tal_menubar.h"

class KviFrame;
class KviTalPopupMenu;
class KviKvsPopupMenu;

typedef struct _KviScriptMenuBarItem
{
	int               id;
	KviStr            szPopupName;
	KviKvsPopupMenu * pPopup;
	KviStr            szText;
} KviScriptMenuBarItem;


class KVIRC_API KviMenuBar : public KviTalMenuBar
{
	Q_OBJECT
public:
	KviMenuBar(KviFrame * par,const char * name);
	~KviMenuBar();
protected:
	KviTalPopupMenu                       * m_pToolbarsPopup;
	KviTalPopupMenu                       * m_pRecentServersPopup;
	KviFrame                         * m_pFrm;
	KviPointerList<KviScriptMenuBarItem> * m_pScriptItemList;
	int                                m_iNumDefaultItems;
	int                              * m_pDefaultItemId;
protected:
	KviScriptMenuBarItem * findMenu(const QString &text);
	KviScriptMenuBarItem * findMenu(KviKvsPopupMenu * p);
	void addDefaultItem(const QString &text,KviTalPopupMenu * pop);
public:
	int getDefaultItemRealIndex(int iDefaultIndex);
	void addMenu(const QString &text,KviKvsPopupMenu * p,int index);
	bool removeMenu(const QString &text);
protected slots:
	void menuDestroyed();
	void setupMainPopup();
	void setupSettingsPopup();
	void setupHelpPopup();
	void setupRecentServersPopup();
	void setupScriptingPopup();
	void newConnectionToServer(int id);
	void setupToolbarsPopup();
	void setupToolsPopup();
	void toolsPopupSelected(int id);
	void actionTriggered(int id);
};

#endif //_KVI_MENUBAR_H_
