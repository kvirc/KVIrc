//=============================================================================
//
//   File : KviKvsCallbackObject.cpp
//   Creation date : Mon 21 Nov 2005 01:46:51 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
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

#include "KviKvsCallbackObject.h"
#include "KviConsoleWindow.h"
#include "KviApplication.h"

KviKvsCallbackObject::KviKvsCallbackObject(const QString & szCallbackName, KviWindow * pTargetWindow, const QString & szCallbackCode, KviKvsVariantList * pMagicParams, unsigned int uFlags)
    : KviHeapObject()
{
	m_uFlags = uFlags;
	m_pTargetWindow = pTargetWindow;
	if(pTargetWindow)
		m_pTargetConsole = pTargetWindow->console();
	else
		m_pTargetConsole = nullptr;
	m_pMagicParams = new KviKvsVariantList();
	if(pMagicParams)
		for(KviKvsVariant * v = pMagicParams->first(); v; v = pMagicParams->next())
			m_pMagicParams->append(new KviKvsVariant(*v));
	QString szName;
	szName = szCallbackName;
	szName += "::";
	szName += "callback";
	m_pScript = new KviKvsScript(szName, szCallbackCode);
}

KviKvsCallbackObject::~KviKvsCallbackObject()
{
	delete m_pScript;
	delete m_pMagicParams;
}

KviKvsCallbackObject::CallbackStatus KviKvsCallbackObject::execute(KviKvsVariantList * pParams, KviKvsVariant * pRetVal)
{
	// first of all check the window
	if(!m_pTargetWindow)
		m_pTargetWindow = g_pApp->activeConsole();
	if(!g_pApp->windowExists(m_pTargetWindow))
	{
		// target window dead
		if(m_uFlags & OnlyIfWindowExists)
			return TargetWindowDead;
		// rebind
		m_pTargetWindow = m_pTargetConsole;
		if(!m_pTargetWindow)
		{
			m_pTargetWindow = g_pApp->activeConsole();
			if(!m_pTargetWindow)
				return Error; // ooops...
		}
		else
		{
			if(!g_pApp->windowExists(m_pTargetWindow))
			{
				m_pTargetWindow = g_pApp->activeConsole();
				if(!m_pTargetWindow)
					return Error; // ooops...
			}
		}
	}
	// ok, got the window
	// now take care of the params
	KviKvsVariantList params;
	params.setAutoDelete(false);
	KviKvsVariant * v;

	if(pParams)
		for(v = pParams->first(); v; v = pParams->next())
			params.append(v);

	for(v = m_pMagicParams->first(); v; v = m_pMagicParams->next())
		params.append(v);

	unsigned int uRet = m_pScript->run(m_pTargetWindow, &params, pRetVal, KviKvsScript::PreserveParams);

	if(uRet)
	{
		if(uRet & KviKvsScript::HaltEncountered)
			return HaltEncountered;
		return Success;
	}

	// ooops... error
	return Error;
}
