#ifndef _KVI_KVS_EVENTMANAGER_H_
#define _KVI_KVS_EVENTMANAGER_H_
//=============================================================================
//
//   File : kvi_kvs_eventmanager.h
//   Creation date : Thu Aug 17 2000 13:59:12 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek <pragma at kvirc dot net>
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

#include "kvi_kvs_event.h"
#include "kvi_pointerlist.h"
#include "kvi_kvs_eventtable.h"

class KviWindow;
class KviKvsModuleInterface;
class KviKvsVariantList;

#define KVI_KVS_NUM_RAW_EVENTS 1000

class KVIRC_API KviKvsEventManager
{
	friend class KviModule; // compatibility only!
protected: // it only can be created and destroyed by KviKvsAliasManager::init()/done()
	KviKvsEventManager();
	~KviKvsEventManager();
protected:
	static KviKvsEventManager * m_pInstance;

	static KviKvsEvent               m_appEventTable[KVI_KVS_NUM_APP_EVENTS];
	KviPointerList<KviKvsEventHandler> * m_rawEventTable[KVI_KVS_NUM_RAW_EVENTS];
public:
	static KviKvsEventManager * instance(){ return m_pInstance; };
	static void init(); // called by KviKvs::init()
	static void done(); // called by KviKvs::done()

	KviKvsEvent * appEvent(unsigned int uEvIdx){ return &(m_appEventTable[uEvIdx]); };

	bool hasAppHandlers(unsigned int uEvIdx){ return m_appEventTable[uEvIdx].hasHandlers(); };
	KviPointerList<KviKvsEventHandler> * appHandlers(unsigned int uEvIdx){ return m_appEventTable[uEvIdx].handlers(); };

	bool hasRawHandlers(unsigned int uEvIdx){ return m_rawEventTable[uEvIdx]; };
	KviPointerList<KviKvsEventHandler> * rawHandlers(unsigned int uEvIdx){ return m_rawEventTable[uEvIdx]; };

	KviKvsEvent * findAppEventByName(const QString &szName);
	unsigned int findAppEventIndexByName(const QString &szName);
	bool isValidAppEvent(unsigned int uEvIdx){ return (uEvIdx < KVI_KVS_NUM_APP_EVENTS); };

	bool isValidRawEvent(unsigned int uEvIdx){ return (uEvIdx < KVI_KVS_NUM_RAW_EVENTS); };

	bool addAppHandler(unsigned int uEvIdx,KviKvsEventHandler * h);
	bool addRawHandler(unsigned int uRawIdx,KviKvsEventHandler * h);

	bool removeScriptAppHandler(unsigned int uEvIdx,const QString &szName);
	bool removeScriptRawHandler(unsigned int uEvIdx,const QString &szName);

	bool enableScriptAppHandler(unsigned int uEvIdx,const QString &szName,bool bEnable);
	bool enableScriptRawHandler(unsigned int uEvIdx,const QString &szName,bool bEnable);

	bool removeModuleAppHandler(unsigned int uEvIdx,KviKvsModuleInterface *i);
	bool removeModuleRawHandler(unsigned int uRawIdx,KviKvsModuleInterface *i);

	void removeAllModuleAppHandlers(KviKvsModuleInterface *i);
	void removeAllModuleRawHandlers(KviKvsModuleInterface *i);

	void removeAllModuleHandlers(KviKvsModuleInterface * i);

	KviKvsScriptEventHandler * findScriptRawHandler(unsigned int uEvIdx,const QString &szName);
	KviKvsScriptEventHandler * findScriptAppHandler(unsigned int uEvIdx,const QString &szName);

	// returns true if further processing should be stopped
	// none of these functions takes params ownership, so be sure to delete them !
	bool triggerHandlers(KviPointerList<KviKvsEventHandler> * pHandlers,KviWindow *pWnd,KviKvsVariantList *pParams);
	bool trigger(unsigned int uEvIdx,KviWindow * pWnd,KviKvsVariantList * pParams)
		{ return triggerHandlers(m_appEventTable[uEvIdx].handlers(),pWnd,pParams); };
	bool triggerRaw(unsigned int uEvIdx,KviWindow *pWnd,KviKvsVariantList * pParams)
		{ return triggerHandlers(m_rawEventTable[uEvIdx],pWnd,pParams); };

	// this is the only that takes parameter ownership and deletes them
	bool triggerDeleteParams(unsigned int uEvIdx,KviWindow * pWnd,KviKvsVariantList * pParams)
	{
		bool bRet = triggerHandlers(m_appEventTable[uEvIdx].handlers(),pWnd,pParams);
		delete pParams;
		return bRet;
	};

	void removeAllScriptAppHandlers();
	void removeAllScriptRawHandlers();

	void clearAppEvents();
	void clearRawEvents();
	void clear();

	void loadAppEvents(const QString &szFileName);
	void saveAppEvents(const QString &szFileName);
	void loadRawEvents(const QString &szFileName);
	void saveRawEvents(const QString &szFileName);
};



#endif //!_KVI_KVS_EVENTMANAGER_H_
