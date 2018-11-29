#ifndef _KVI_KVS_TIMERMANAGER_H_
#define _KVI_KVS_TIMERMANAGER_H_
//=============================================================================
//
//   File : KviKvsTimerManager.h
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

#include "kvi_settings.h"
#include "KviQString.h"

#include "KviPointerHashTable.h"
#include "KviPointerList.h"

#include <QObject>

class KviKvsTimerManager;
class KviKvsScript;
class KviKvsHash;
class KviKvsVariantList;
class KviKvsExtendedRunTimeData;

class KviWindow;

// FIXME: This stuff could be moved to a module
//        called timer
//        timer.start(){}
//        timer.stop
//        timer.exists
//        timer.timeout
//        timer.suspend
//        etc...
//        Then we could remap timer.start to the old /timer call
//        by hardcoding it...

class KVIRC_API KviKvsTimer
{
	friend class KviKvsTimerManager;

public:
	enum Lifetime
	{
		SingleShot,
		WindowLifetime,
		Persistent
	};

protected:
	KviKvsTimer(const QString & szName, Lifetime l, KviWindow * pWnd, int iDelay, int iId, KviKvsScript * pCallback, KviKvsVariantList * pParams);

public:
	~KviKvsTimer();

protected:
	Lifetime m_eLifetime;                       // the type of this timer
	KviWindow * m_pWnd;                         // the window that this timer is (currently) bound to
	QString m_szName;                           // this timer name
	KviKvsScript * m_pCallback;                 // callback to be executed at timer shots
	int m_iDelay;                               // the timer delay in msecs
	int m_iId;                                  // the system id of this timer
	KviKvsExtendedRunTimeData * m_pRunTimeData; // ext run time data for this timer object
	KviKvsVariantList * m_pParameterList;       // parameter list
public:
	KviWindow * window() { return m_pWnd; };
	const QString & name() { return m_szName; };
	const KviKvsScript * callback() { return m_pCallback; };
	Lifetime lifetime() { return m_eLifetime; };
	int delay() { return m_iDelay; };
	int id() { return m_iId; };
	//KviKvsHash * variables(){ return m_pVariables; };
	KviKvsExtendedRunTimeData * runTimeData() { return m_pRunTimeData; };
	KviKvsVariantList * parameterList() { return m_pParameterList; };
protected:
	void setWindow(KviWindow * pWnd) { m_pWnd = pWnd; };
};

class KVIRC_API KviKvsTimerManager : public QObject
{
	Q_OBJECT
protected: // it only can be created and destroyed by KviKvsTimerManager::init()/done()
	KviKvsTimerManager();
	~KviKvsTimerManager();

private:
	KviPointerHashTable<int, KviKvsTimer> * m_pTimerDictById;       // stored by id
	KviPointerHashTable<QString, KviKvsTimer> * m_pTimerDictByName; // stored by name
	static KviKvsTimerManager * m_pInstance;                        // the one and only timer manager instance
	KviPointerList<KviKvsTimer> * m_pKilledTimerList;               // list of timers for that killing has been scheduled
	int m_iAssassinTimer;                                           // assassin timer id
	int m_iCurrentTimer;                                            // the timer currently executed
public:
	static KviKvsTimerManager * instance() { return m_pInstance; };
	static void init();
	static void done();
	// the pCallback and pParams are owned by the timer: they WILL be deleted
	bool addTimer(const QString & szName, KviKvsTimer::Lifetime l, KviWindow * pWnd, int iDelay, KviKvsScript * pCallback, KviKvsVariantList * pParams);
	bool deleteTimer(const QString & szName);
	bool deleteTimer(int iId);
	// the timer manager does not trigger timers concurrently
	// this means that if this is called from a timer handler
	// the current timer will be unique
	bool deleteCurrentTimer();
	void deleteAllTimers();
	bool timerExists(const QString & szName) { return m_pTimerDictByName->find(szName); };
	KviPointerHashTable<QString, KviKvsTimer> * timerDict()
	{
		return m_pTimerDictByName;
	};

protected:
	void scheduleKill(KviKvsTimer * t);
	void timerEvent(QTimerEvent * e) override;
};

#endif //!_KVI_KVS_TIMERMANAGER_H_
