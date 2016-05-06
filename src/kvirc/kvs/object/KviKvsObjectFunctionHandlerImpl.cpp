//=============================================================================
//
//   File : KviKvsObjectFunctionHandlerImpl.cpp
//   Creation date : Sun 24 Apr 2005 04:01:14 by Szymon Stefanek
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

#include "KviKvsObjectFunctionHandlerImpl.h"
#include "KviKvsObjectFunctionCall.h"
#include "KviKvsScript.h"

KviKvsObjectFunctionHandler * KviKvsObjectStandardTrueReturnFunctionHandler::clone()
{
	return new KviKvsObjectStandardTrueReturnFunctionHandler();
}

bool KviKvsObjectStandardTrueReturnFunctionHandler::call(KviKvsObject *, KviKvsObjectFunctionCall * pCall)
{
	pCall->returnValue()->setBoolean(true);
	return true;
}

KviKvsObjectFunctionHandler * KviKvsObjectStandardFalseReturnFunctionHandler::clone()
{
	return new KviKvsObjectStandardFalseReturnFunctionHandler();
}

bool KviKvsObjectStandardFalseReturnFunctionHandler::call(KviKvsObject *, KviKvsObjectFunctionCall * pCall)
{
	pCall->returnValue()->setBoolean(false);
	return true;
}

KviKvsObjectFunctionHandler * KviKvsObjectStandardNothingReturnFunctionHandler::clone()
{
	return new KviKvsObjectStandardNothingReturnFunctionHandler();
}

bool KviKvsObjectStandardNothingReturnFunctionHandler::call(KviKvsObject *, KviKvsObjectFunctionCall * pCall)
{
	pCall->returnValue()->setNothing();
	return true;
}

KviKvsObjectCoreCallFunctionHandler::KviKvsObjectCoreCallFunctionHandler(KviKvsObjectFunctionHandlerProc pProc, unsigned int uFlags)
    : KviKvsObjectFunctionHandler(uFlags)
{
	m_pProc = pProc;
}

KviKvsObjectCoreCallFunctionHandler::KviKvsObjectCoreCallFunctionHandler(const KviKvsObjectCoreCallFunctionHandler & h)
    : KviKvsObjectFunctionHandler(h.m_uFlags)
{
	m_pProc = h.m_pProc;
}

KviKvsObjectCoreCallFunctionHandler::~KviKvsObjectCoreCallFunctionHandler()
    = default;

KviKvsObjectFunctionHandler * KviKvsObjectCoreCallFunctionHandler::clone()
{
	return new KviKvsObjectCoreCallFunctionHandler(*this);
}

bool KviKvsObjectCoreCallFunctionHandler::call(KviKvsObject * pObject, KviKvsObjectFunctionCall * pCall)
{
	// Actually m_pProc can't be 0: if it's zero then it's a BUG and should be traced down: we just crash
	return (pObject->*m_pProc)(pCall);
}

KviKvsObjectScriptFunctionHandler::KviKvsObjectScriptFunctionHandler(const QString & szScriptContext, const QString & szCode, const QString & szReminder, unsigned int uFlags)
    : KviKvsObjectFunctionHandler(uFlags)
{
	m_pHandler = new KviKvsScript(szScriptContext, szCode, KviKvsScript::InstructionList);
	m_szReminder = szReminder;
}

KviKvsObjectScriptFunctionHandler::KviKvsObjectScriptFunctionHandler(const KviKvsObjectScriptFunctionHandler & h)
    : KviKvsObjectFunctionHandler(h.m_uFlags)
{
	// ref
	m_pHandler = new KviKvsScript(*(h.m_pHandler));
}

KviKvsObjectScriptFunctionHandler::~KviKvsObjectScriptFunctionHandler()
{
	delete m_pHandler;
}

bool KviKvsObjectScriptFunctionHandler::isScriptHandler()
{
	return true;
}

QString KviKvsObjectScriptFunctionHandler::scriptHandlerCode()
{
	return m_pHandler->code();
}
QString KviKvsObjectScriptFunctionHandler::reminder()
{
	return m_szReminder;
}

KviKvsObjectFunctionHandler * KviKvsObjectScriptFunctionHandler::clone()
{
	return new KviKvsObjectScriptFunctionHandler(*this);
}

bool KviKvsObjectScriptFunctionHandler::call(KviKvsObject * pObject, KviKvsObjectFunctionCall * pCall)
{
	KviKvsExtendedRunTimeData rd(pObject);
	return m_pHandler->run(pCall->window(), pCall->params(), pCall->returnValue(), KviKvsScript::PreserveParams, &rd);
}
