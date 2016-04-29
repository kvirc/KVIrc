//=============================================================================
//
//   File : KviKvs.cpp
//   Creation date : Wed 1 Oct 2003 01.38 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviKvs.h"
#include "KviKvsKernel.h"
#include "KviKvsAliasManager.h"
#include "KviKvsDnsManager.h"
#include "KviKvsTimerManager.h"
#include "KviKvsPopupManager.h"
#include "KviKvsEventManager.h"
#include "KviKvsScriptAddonManager.h"
#include "KviKvsObjectController.h"

namespace KviKvs
{
	void init()
	{
		KviKvsKernel::init();
		KviKvsAliasManager::init();
		KviKvsPopupManager::init();
		KviKvsEventManager::init();
		// FIXME: Could we delay the init of these objects ?
		//        It could be delayed to the first *::instance() call
		KviKvsScriptAddonManager::init();
		KviKvsTimerManager::init();
		KviKvsDnsManager::init();
	}

	void done()
	{
		//KviKvsScriptManager::done();
		KviKvsEventManager::done();
		KviKvsPopupManager::done();
		KviKvsAliasManager::done();
		KviKvsScriptAddonManager::done();
		KviKvsTimerManager::done();
		KviKvsDnsManager::done();
		KviKvsKernel::done();
	}

	void loadPopups(const QString & szFileName)
	{
		KviKvsPopupManager::instance()->load(szFileName);
	}

	void savePopups(const QString & szFileName)
	{
		KviKvsPopupManager::instance()->save(szFileName);
	}

	void clearPopups()
	{
		KviKvsPopupManager::instance()->clear();
	}

	void loadAliases(const QString & szFileName)
	{
		KviKvsAliasManager::instance()->load(szFileName);
	}

	void saveAliases(const QString & szFileName)
	{
		KviKvsAliasManager::instance()->save(szFileName);
	}

	void clearAliases()
	{
		KviKvsAliasManager::instance()->clear();
	}

	void loadAppEvents(const QString & szFileName)
	{
		KviKvsEventManager::instance()->loadAppEvents(szFileName);
	}

	void saveAppEvents(const QString & szFileName)
	{
		KviKvsEventManager::instance()->saveAppEvents(szFileName);
	}

	void clearAppEvents()
	{
		KviKvsEventManager::instance()->removeAllScriptAppHandlers();
	}

	void loadRawEvents(const QString & szFileName)
	{
		KviKvsEventManager::instance()->loadRawEvents(szFileName);
	}

	void saveRawEvents(const QString & szFileName)
	{
		KviKvsEventManager::instance()->saveRawEvents(szFileName);
	}

	void clearRawEvents()
	{
		KviKvsEventManager::instance()->removeAllScriptRawHandlers();
	}

	void loadScriptAddons(const QString & szFileName)
	{
		KviKvsScriptAddonManager::instance()->load(szFileName);
	}

	void saveScriptAddons(const QString & szFileName)
	{
		KviKvsScriptAddonManager::instance()->save(szFileName);
	}

	void clearScriptAddons()
	{
		KviKvsScriptAddonManager::instance()->clear();
	}

	void flushUserClasses()
	{
		KviKvsKernel::instance()->objectController()->flushUserClasses();
	}
};
