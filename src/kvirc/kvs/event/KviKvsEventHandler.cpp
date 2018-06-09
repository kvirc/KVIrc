//=============================================================================
//
//   File : KviKvsEventHandler.cpp
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

#include "KviKvsEventHandler.h"

#include <utility>

KviKvsEventHandler::KviKvsEventHandler(Type t)
    : KviHeapObject(), m_type(t)
{
}

KviKvsEventHandler::~KviKvsEventHandler()
    = default;

KviKvsScriptEventHandler::KviKvsScriptEventHandler(QString szHandlerName, const QString & szContextName, const QString & szCode, bool bEnabled)
    : KviKvsEventHandler(KviKvsEventHandler::Script)
    , m_szName(std::move(szHandlerName))
    , m_bEnabled(bEnabled)
{
	m_pScript = new KviKvsScript(szContextName, szCode);
}

KviKvsScriptEventHandler::~KviKvsScriptEventHandler()
{
	delete m_pScript;
}

KviKvsScriptEventHandler * KviKvsScriptEventHandler::createInstance(const QString & szHandlerName, const QString & szContextName, const QString & szCode, bool bEnabled)
{
	return new KviKvsScriptEventHandler(szHandlerName, szContextName, szCode, bEnabled);
}

KviKvsModuleEventHandler::KviKvsModuleEventHandler(KviKvsModuleInterface * m, KviKvsModuleEventHandlerRoutine proc)
    : KviKvsEventHandler(KviKvsEventHandler::Module), m_pModule(m)
{
	m_pProc = new KviKvsModuleEventHandlerRoutine(proc);
}

KviKvsModuleEventHandler::~KviKvsModuleEventHandler()
{
	delete m_pProc;
}
