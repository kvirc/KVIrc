#ifndef _KVI_KVS_EVENTHANDLER_H_
#define _KVI_KVS_EVENTHANDLER_H_
//=============================================================================
//
//   File : KviKvsEventHandler.h
//   Creation date : Mon 23 Feb 2004 03:04:58 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
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
#include "KviKvsScript.h"
#include "KviQString.h"
#include "KviKvsModuleInterface.h"
#include "KviHeapObject.h"

class KVIRC_API KviKvsEventHandler : public KviHeapObject
{
public:
	enum Type
	{
		Script,
		Module
	};

protected:
	Type m_type;

public:
	KviKvsEventHandler(Type t);
	virtual ~KviKvsEventHandler();

public:
	Type type() { return m_type; };
};

class KVIRC_API KviKvsScriptEventHandler : public KviKvsEventHandler
{
public:
	// the event handler becomes the owned of pszCode!
	KviKvsScriptEventHandler(QString szHandlerName, const QString & szContextName, const QString & szCode, bool bEnabled = true);
	virtual ~KviKvsScriptEventHandler();

protected:
	QString m_szName;
	KviKvsScript * m_pScript;
	bool m_bEnabled;

public:
	KviKvsScript * script() { return m_pScript; };
	const QString & name() { return m_szName; };
	const QString & code() { return m_pScript->code(); };

	bool isEnabled() { return m_bEnabled; };
	void setEnabled(bool bEnabled) { m_bEnabled = bEnabled; };

	// Static allocator function.
	// This MUST be used by the modules to allocate event structures
	// instead of the new operator.
	// See KviHeapObject.cpp for an explanation.
	static KviKvsScriptEventHandler * createInstance(const QString & szHandlerName, const QString & szContextName, const QString & szCode, bool bEnabled = true);
};

class KVIRC_API KviKvsModuleEventHandler : public KviKvsEventHandler
{
protected:
	KviKvsModuleInterface * m_pModule;
	KviKvsModuleEventHandlerRoutine * m_pProc;

public:
	KviKvsModuleEventHandler(KviKvsModuleInterface * m, KviKvsModuleEventHandlerRoutine proc);
	virtual ~KviKvsModuleEventHandler();

public:
	KviKvsModuleInterface * moduleInterface() { return m_pModule; };
	KviKvsModuleEventHandlerRoutine * handlerRoutine() { return m_pProc; };
};

/*
#include "KviModule.h"

class KVIRC_API KviKvsOldModuleEventHandler : public KviKvsEventHandler
{
protected:
	KviModule               * m_pModule;
	KviModuleEventParseProc   m_proc;
public:
	KviKvsOldModuleEventHandler(KviModuleEventParseProc p,KviModule * m);
	virtual ~KviKvsOldModuleEventHandler();
public:
	KviModule * module(){ return m_pModule; };
	void setModule(KviModule * m){ m_pModule = m; };
	KviModuleEventParseProc proc(){ return m_proc; };
	void setProc(KviModuleEventParseProc p){ m_proc = p; };
};

*/
#endif //!_KVI_KVS_EVENTHANDLER_H_
