//=============================================================================
//
//   File : KviKvsTimerManager.cpp
//   Creation date : Fri 19 Dec 2003 01:29:22 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "KviKvsTimerManager.h"

#include "KviKvsScript.h"
#include "KviKvsVariantList.h"
#include "KviKvsRunTimeContext.h"

#include "KviApplication.h"
#include "KviWindow.h"
#include "KviConsoleWindow.h"
#include "KviOptions.h"
#include "KviLocale.h"
#include "kvi_out.h"

KviKvsTimer::KviKvsTimer(const QString & szName, Lifetime l, KviWindow * pWnd, int iDelay, int iId, KviKvsScript * pCallback, KviKvsVariantList * pParams)
{
	m_szName = szName;
	m_eLifetime = l;
	m_pWnd = pWnd;
	m_iDelay = iDelay;
	m_iId = iId;
	m_pCallback = pCallback;
	//m_pVariables = new KviKvsHash();
	m_pRunTimeData = new KviKvsExtendedRunTimeData(new KviKvsHash(), true);
	m_pParameterList = pParams;
}

KviKvsTimer::~KviKvsTimer()
{
	delete m_pRunTimeData;
	delete m_pParameterList;
	delete m_pCallback;
}

KviKvsTimerManager * KviKvsTimerManager::m_pInstance = nullptr;

KviKvsTimerManager::KviKvsTimerManager()
    : QObject()
{
	m_pTimerDictById = new KviPointerHashTable<int, KviKvsTimer>(17);
	m_pTimerDictById->setAutoDelete(false);
	m_pTimerDictByName = new KviPointerHashTable<QString, KviKvsTimer>(17, false);
	m_pTimerDictByName->setAutoDelete(false);
	m_pKilledTimerList = nullptr;
	m_iAssassinTimer = 0;
	m_iCurrentTimer = 0;
}

KviKvsTimerManager::~KviKvsTimerManager()
{
	delete m_pTimerDictById;
	m_pTimerDictByName->setAutoDelete(true);
	delete m_pTimerDictByName;
	if(m_pKilledTimerList)
		delete m_pKilledTimerList;
}

void KviKvsTimerManager::init()
{
	if(KviKvsTimerManager::m_pInstance)
	{
		qDebug("Trying to double init() the timer manager!");
		return;
	}
	KviKvsTimerManager::m_pInstance = new KviKvsTimerManager();
}

void KviKvsTimerManager::done()
{
	if(!KviKvsTimerManager::m_pInstance)
	{
		qDebug("Trying to call done() on a non existing timer manager!");
		return;
	}
	delete KviKvsTimerManager::m_pInstance;
	KviKvsTimerManager::m_pInstance = nullptr;
}

bool KviKvsTimerManager::addTimer(const QString & szName, KviKvsTimer::Lifetime l, KviWindow * pWnd, int iDelay, KviKvsScript * pCallback, KviKvsVariantList * pParams)
{
	int iId = startTimer(iDelay);

	if(iId <= 0)
	{
		delete pCallback;
		pCallback = nullptr;
		delete pParams;
		pParams = nullptr;
		return false;
	}

	KviKvsTimer * t = new KviKvsTimer(szName, l, pWnd, iDelay, iId, pCallback, pParams);
	KviKvsTimer * old = m_pTimerDictByName->find(szName);
	if(old)
		deleteTimer(old->id());
	m_pTimerDictByName->insert(szName, t);
	m_pTimerDictById->insert(t->id(), t);
	return true;
}

bool KviKvsTimerManager::deleteTimer(const QString & szName)
{
	KviKvsTimer * t = m_pTimerDictByName->find(szName);
	if(!t)
		return false;
	killTimer(t->id());
	m_pTimerDictById->remove(t->id());
	m_pTimerDictByName->remove(szName);
	scheduleKill(t);
	return true;
}

bool KviKvsTimerManager::deleteTimer(int iId)
{
	KviKvsTimer * t = m_pTimerDictById->find(iId);
	if(!t)
		return false;
	killTimer(t->id());
	m_pTimerDictById->remove(t->id());
	m_pTimerDictByName->remove(t->name());
	scheduleKill(t);
	return true;
}

bool KviKvsTimerManager::deleteCurrentTimer()
{
	if(!m_iCurrentTimer)
		return false;
	deleteTimer(m_iCurrentTimer);
	m_iCurrentTimer = 0;
	return true;
}

void KviKvsTimerManager::deleteAllTimers()
{
	if(m_pTimerDictById->isEmpty())
		return;
	KviPointerHashTableIterator<int, KviKvsTimer> it(*m_pTimerDictById);
	KviPointerList<KviKvsTimer> tl;
	tl.setAutoDelete(false);
	while(KviKvsTimer * t = it.current())
	{
		tl.append(t);
		++it;
	}
	for(KviKvsTimer * dying = tl.first(); dying; dying = tl.next())
	{
		deleteTimer(dying->id());
	}
}

void KviKvsTimerManager::scheduleKill(KviKvsTimer * t)
{
	if(!m_pKilledTimerList)
	{
		m_pKilledTimerList = new KviPointerList<KviKvsTimer>;
		m_pKilledTimerList->setAutoDelete(true);
	}
	m_pKilledTimerList->append(t);

	if(!m_iAssassinTimer)
		m_iAssassinTimer = startTimer(0);
}

void KviKvsTimerManager::timerEvent(QTimerEvent * e)
{
	int iId = e->timerId();

	if(iId == m_iAssassinTimer)
	{
		if(!m_pKilledTimerList)
		{
			qDebug("Oops! Assassinating timer with no victims?");
		}
		else
		{
			m_pKilledTimerList->clear();
		}
		killTimer(m_iAssassinTimer);
		m_iAssassinTimer = 0;
		return;
	}

	KviKvsTimer * t = m_pTimerDictById->find(iId);
	if(!t)
	{
		qDebug("Internal error: received a non existent timer event");
		return; // HUH ?
	}

	if(!g_pApp->windowExists(t->window()))
	{
		if(t->lifetime() != KviKvsTimer::Persistent)
		{
			deleteTimer(t->id());
			return;
		}

		// rebind to an existing console
		t->setWindow(g_pApp->activeConsole());
	}

	KviKvsScript copy(*(t->callback()));

	m_iCurrentTimer = t->id();
	bool bRet = copy.run(t->window(),
	    t->parameterList(),
	    nullptr,
	    KviKvsScript::PreserveParams,
	    t->runTimeData());

	m_iCurrentTimer = 0;

	if(!bRet)
	{
		// the timer may already have been scheduled for killing!
		if(KVI_OPTION_BOOL(KviOption_boolKillBrokenTimers))
		{
			t->window()->output(KVI_OUT_PARSERERROR, __tr2qs_ctx("Timer '%Q' has a broken callback handler: killing the timer", "kvs"), &(t->name()));
			deleteTimer(t->id());
		}
		return;
	}

	// the timer may already have been scheduled for killing!

	if(t->lifetime() == KviKvsTimer::SingleShot)
	{
		deleteTimer(t->id());
	}
}
