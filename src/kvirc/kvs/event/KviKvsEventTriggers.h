#ifndef _KVI_KVS_EVENTTRIGGERS_H_
#define _KVI_KVS_EVENTTRIGGERS_H_
//=============================================================================
//
//   File : KviKvsEventTriggers.h
//   Creation date : Thu Oct 22 2005 04:57:12 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "KviKvsEventTable.h"
#include "KviKvsEventManager.h"
#include "KviKvsVariantList.h"

//
// KVS Macros for triggering events
//
// KviKvsVariantList vParams;
// vParams.append(bSomething);
// vParams.append(iSomething);
// vParams.append(szSomething);
// KVS_TRIGGER_EVENT(KviEvent_OnSomething,pSomeWindow,&vParams)
//
// if(KVS_TRIGGER_EVENT_HALTED(KviEvent_OnSomething,pSomeWindow,&vParams))
//     msg->haltOutput()
//
//

// These two allow reusing the parameter lists (but may require more code)
#define KVS_TRIGGER_EVENT(__idx, __wnd, __parms)              \
	if(KviKvsEventManager::instance()->hasAppHandlers(__idx)) \
		KviKvsEventManager::instance()->trigger(__idx, __wnd, __parms);

#define KVS_TRIGGER_EVENT_HALTED(__idx, __wnd, __parms) \
	(KviKvsEventManager::instance()->hasAppHandlers(__idx) ? KviKvsEventManager::instance()->trigger(__idx, __wnd, __parms) : false)

// These require less code (but param lists can't be reused)
#define KVS_TRIGGER_EVENT_0(__idx, __wnd)                                         \
	if(KviKvsEventManager::instance()->hasAppHandlers(__idx))                     \
	{                                                                             \
		KviKvsVariantList _vLocalParamList;                                       \
		KviKvsEventManager::instance()->trigger(__idx, __wnd, &_vLocalParamList); \
	}

#define KVS_TRIGGER_EVENT_1(__idx, __wnd, __param1)                               \
	if(KviKvsEventManager::instance()->hasAppHandlers(__idx))                     \
	{                                                                             \
		KviKvsVariantList _vLocalParamList(                                       \
		    new KviKvsVariant(__param1));                                         \
		KviKvsEventManager::instance()->trigger(__idx, __wnd, &_vLocalParamList); \
	}

#define KVS_TRIGGER_EVENT_2(__idx, __wnd, __param1, __param2)                     \
	if(KviKvsEventManager::instance()->hasAppHandlers(__idx))                     \
	{                                                                             \
		KviKvsVariantList _vLocalParamList(                                       \
		    new KviKvsVariant(__param1),                                          \
		    new KviKvsVariant(__param2));                                         \
		KviKvsEventManager::instance()->trigger(__idx, __wnd, &_vLocalParamList); \
	}

#define KVS_TRIGGER_EVENT_3(__idx, __wnd, __param1, __param2, __param3)           \
	if(KviKvsEventManager::instance()->hasAppHandlers(__idx))                     \
	{                                                                             \
		KviKvsVariantList _vLocalParamList(                                       \
		    new KviKvsVariant(__param1),                                          \
		    new KviKvsVariant(__param2),                                          \
		    new KviKvsVariant(__param3));                                         \
		KviKvsEventManager::instance()->trigger(__idx, __wnd, &_vLocalParamList); \
	}

#define KVS_TRIGGER_EVENT_4(__idx, __wnd, __param1, __param2, __param3, __param4) \
	if(KviKvsEventManager::instance()->hasAppHandlers(__idx))                     \
	{                                                                             \
		KviKvsVariantList _vLocalParamList(                                       \
		    new KviKvsVariant(__param1),                                          \
		    new KviKvsVariant(__param2),                                          \
		    new KviKvsVariant(__param3),                                          \
		    new KviKvsVariant(__param4));                                         \
		KviKvsEventManager::instance()->trigger(__idx, __wnd, &_vLocalParamList); \
	}

#define KVS_TRIGGER_EVENT_5(__idx, __wnd, __param1, __param2, __param3, __param4, __param5) \
	if(KviKvsEventManager::instance()->hasAppHandlers(__idx))                               \
	{                                                                                       \
		KviKvsVariantList _vLocalParamList(                                                 \
		    new KviKvsVariant(__param1),                                                    \
		    new KviKvsVariant(__param2),                                                    \
		    new KviKvsVariant(__param3),                                                    \
		    new KviKvsVariant(__param4),                                                    \
		    new KviKvsVariant(__param5));                                                   \
		KviKvsEventManager::instance()->trigger(__idx, __wnd, &_vLocalParamList);           \
	}

#define KVS_TRIGGER_EVENT_6(__idx, __wnd, __param1, __param2, __param3, __param4, __param5, __param6) \
	if(KviKvsEventManager::instance()->hasAppHandlers(__idx))                                         \
	{                                                                                                 \
		KviKvsVariantList _vLocalParamList(                                                           \
		    new KviKvsVariant(__param1),                                                              \
		    new KviKvsVariant(__param2),                                                              \
		    new KviKvsVariant(__param3),                                                              \
		    new KviKvsVariant(__param4),                                                              \
		    new KviKvsVariant(__param5),                                                              \
		    new KviKvsVariant(__param6));                                                             \
		KviKvsEventManager::instance()->trigger(__idx, __wnd, &_vLocalParamList);                     \
	}

#define KVS_TRIGGER_EVENT_7(__idx, __wnd, __param1, __param2, __param3, __param4, __param5, __param6, __param7) \
	if(KviKvsEventManager::instance()->hasAppHandlers(__idx))                                                   \
	{                                                                                                           \
		KviKvsVariantList _vLocalParamList(                                                                     \
		    new KviKvsVariant(__param1),                                                                        \
		    new KviKvsVariant(__param2),                                                                        \
		    new KviKvsVariant(__param3),                                                                        \
		    new KviKvsVariant(__param4),                                                                        \
		    new KviKvsVariant(__param5),                                                                        \
		    new KviKvsVariant(__param6),                                                                        \
		    new KviKvsVariant(__param7));                                                                       \
		KviKvsEventManager::instance()->trigger(__idx, __wnd, &_vLocalParamList);                               \
	}

#define KVS_TRIGGER_EVENT_0_HALTED(__idx, __wnd)                   \
	(                                                              \
	    KviKvsEventManager::instance()->hasAppHandlers(__idx)      \
	        ? KviKvsEventManager::instance()->triggerDeleteParams( \
	              __idx,                                           \
	              __wnd,                                           \
	              new KviKvsVariantList())                         \
	        : false)

#define KVS_TRIGGER_EVENT_1_HALTED(__idx, __wnd, __param1)         \
	(                                                              \
	    KviKvsEventManager::instance()->hasAppHandlers(__idx)      \
	        ? KviKvsEventManager::instance()->triggerDeleteParams( \
	              __idx,                                           \
	              __wnd,                                           \
	              new KviKvsVariantList(                           \
	                  new KviKvsVariant(__param1)))                \
	        : false)

#define KVS_TRIGGER_EVENT_2_HALTED(__idx, __wnd, __param1, __param2) \
	(                                                                \
	    KviKvsEventManager::instance()->hasAppHandlers(__idx)        \
	        ? KviKvsEventManager::instance()->triggerDeleteParams(   \
	              __idx,                                             \
	              __wnd,                                             \
	              new KviKvsVariantList(                             \
	                  new KviKvsVariant(__param1),                   \
	                  new KviKvsVariant(__param2)))                  \
	        : false)

#define KVS_TRIGGER_EVENT_3_HALTED(__idx, __wnd, __param1, __param2, __param3) \
	(                                                                          \
	    KviKvsEventManager::instance()->hasAppHandlers(__idx)                  \
	        ? KviKvsEventManager::instance()->triggerDeleteParams(             \
	              __idx,                                                       \
	              __wnd,                                                       \
	              new KviKvsVariantList(                                       \
	                  new KviKvsVariant(__param1),                             \
	                  new KviKvsVariant(__param2),                             \
	                  new KviKvsVariant(__param3)))                            \
	        : false)

#define KVS_TRIGGER_EVENT_4_HALTED(__idx, __wnd, __param1, __param2, __param3, __param4) \
	(                                                                                    \
	    KviKvsEventManager::instance()->hasAppHandlers(__idx)                            \
	        ? KviKvsEventManager::instance()->triggerDeleteParams(                       \
	              __idx,                                                                 \
	              __wnd,                                                                 \
	              new KviKvsVariantList(                                                 \
	                  new KviKvsVariant(__param1),                                       \
	                  new KviKvsVariant(__param2),                                       \
	                  new KviKvsVariant(__param3),                                       \
	                  new KviKvsVariant(__param4)))                                      \
	        : false)

#define KVS_TRIGGER_EVENT_5_HALTED(__idx, __wnd, __param1, __param2, __param3, __param4, __param5) \
	(                                                                                              \
	    KviKvsEventManager::instance()->hasAppHandlers(__idx)                                      \
	        ? KviKvsEventManager::instance()->triggerDeleteParams(                                 \
	              __idx,                                                                           \
	              __wnd,                                                                           \
	              new KviKvsVariantList(                                                           \
	                  new KviKvsVariant(__param1),                                                 \
	                  new KviKvsVariant(__param2),                                                 \
	                  new KviKvsVariant(__param3),                                                 \
	                  new KviKvsVariant(__param4),                                                 \
	                  new KviKvsVariant(__param5)))                                                \
	        : false)

#define KVS_TRIGGER_EVENT_6_HALTED(__idx, __wnd, __param1, __param2, __param3, __param4, __param5, __param6) \
	(                                                                                                        \
	    KviKvsEventManager::instance()->hasAppHandlers(__idx)                                                \
	        ? KviKvsEventManager::instance()->triggerDeleteParams(                                           \
	              __idx,                                                                                     \
	              __wnd,                                                                                     \
	              new KviKvsVariantList(                                                                     \
	                  new KviKvsVariant(__param1),                                                           \
	                  new KviKvsVariant(__param2),                                                           \
	                  new KviKvsVariant(__param3),                                                           \
	                  new KviKvsVariant(__param4),                                                           \
	                  new KviKvsVariant(__param5),                                                           \
	                  new KviKvsVariant(__param6)))                                                          \
	        : false)

#define KVS_TRIGGER_EVENT_7_HALTED(__idx, __wnd, __param1, __param2, __param3, __param4, __param5, __param6, __param7) \
	(                                                                                                                  \
	    KviKvsEventManager::instance()->hasAppHandlers(__idx)                                                          \
	        ? KviKvsEventManager::instance()->triggerDeleteParams(                                                     \
	              __idx,                                                                                               \
	              __wnd,                                                                                               \
	              new KviKvsVariantList(                                                                               \
	                  new KviKvsVariant(__param1),                                                                     \
	                  new KviKvsVariant(__param2),                                                                     \
	                  new KviKvsVariant(__param3),                                                                     \
	                  new KviKvsVariant(__param4),                                                                     \
	                  new KviKvsVariant(__param5),                                                                     \
	                  new KviKvsVariant(__param6),                                                                     \
	                  new KviKvsVariant(__param7)))                                                                    \
	        : false)

#endif //!_KVI_KVS_EVENTTRIGGERS_H_
