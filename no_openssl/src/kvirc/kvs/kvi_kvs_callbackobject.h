#ifndef _KVI_KVS_CALLBACKOBJECT_H_
#define _KVI_KVS_CALLBACKOBJECT_H_
//=============================================================================
//
//   File : kvi_kvs_callbackobject.h
//   Creation date : Mon 21 Nov 2005 01:46:51 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2005-2008 Szymon Stefanek <pragma at kvirc dot net>
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
#include "kvi_heapobject.h"

#include "kvi_window.h"
#include "kvi_qstring.h"
#include "kvi_kvs_variantlist.h"
#include "kvi_kvs_script.h"


//
// This is a simple callback object useful in stuff like dialog.message(){}
// You derive a class from KviKvsCallbackObject, pass the callback data (code,window and magic params)
// and when the time comes ask it to execute the callback (maybe by passing some special parameters)
//
//

class KVIRC_API KviKvsCallbackObject : public KviHeapObject
{
public:
	enum CallbackFlags {
		// Execute the callback only if the target window exists
		// If this flag is not used then if the target window dies
		// the callback is executed in another window (the associated console, if possible, otherwise the first available console)
		OnlyIfWindowExists = 1
	};
	enum CallbackStatus {
		// succesfull execution
		Success,
		// error in command
		Error,
		// succesfull execution, but halt encountered
		HaltEncountered,
		// execution aborted since OnlyIfWindowExists was passed and the target window is dead
		TargetWindowDead
	};
public:
	KviKvsCallbackObject(
			const QString      &szCallbackSource, // the name of this callback (should identify its source, like "dialog.message"): displayed in error messages
			KviWindow         * pTargetWindow,    // the window that this callback should be triggered in (can be zero: to execute in the first available console)
			const QString      &szCallbackCode,   // the callback code (may be empty!)
			KviKvsVariantList * pMagicParams,     // magic parameters that should be passed to the callback (this class makes a copy of them)
			unsigned int        uFlags = 0        // a combination of CallbackFlags
		);
	virtual ~KviKvsCallbackObject();
protected:
	KviKvsScript      * m_pScript;       // the callback script
	KviWindow         * m_pTargetWindow;
	KviWindow         * m_pTargetConsole;
	KviKvsVariantList * m_pMagicParams;
	unsigned int        m_uFlags;
protected:
	// the parameter list is always shallow!
	CallbackStatus execute(KviKvsVariantList * pParams = 0,KviKvsVariant * pRetVal = 0);
};



#endif //!_KVI_KVS_CALLBACKOBJECT_H_
