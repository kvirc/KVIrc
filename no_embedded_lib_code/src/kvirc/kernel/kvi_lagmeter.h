#ifndef _KVI_LAGMETER_H_
#define _KVI_LAGMETER_H_
//=============================================================================
//
//   File : kvi_lagmeter.h
//   Creation date : Fri Oct 18 13:30:26 CEST 1999 by Juanjo Álvarez
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999 Juanjo Álvarez
//   Copyright (C) 2002-2008 Szymon Stefanek (pragma at kvirc dot net)
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
#include "kvi_string.h"
#include "kvi_pointerlist.h"

#include <QObject>

class KviIrcConnection;

class KviLagCheck
{
public:
	KviStr szKey;
	long   lSecs;  // since epoch
	long   lUSecs;
	unsigned int    uReliability; // [0-100]
};

class KVIRC_API KviLagMeter : public QObject
{
	Q_OBJECT
	friend class KviIrcConnection;
protected:
	KviLagMeter(KviIrcConnection * c);
	~KviLagMeter();
protected:
	KviIrcConnection        * m_pConnection;
	unsigned int              m_uLag;                // last computed lag
	unsigned int              m_uLastEmittedLag;     // last emitted lag
	long                      m_tLastCompleted;      // time when the last lag was completed (gettimeofday!)
	unsigned int              m_uLastReliability;    // how much reliable was the last completed check ?
	KviPointerList<KviLagCheck> * m_pCheckList;
	long                      m_tFirstOwnCheck;      // time when the first ping after a completed check was sent
	long                      m_tLastOwnCheck;       // time when the last ping was sent
	bool                      m_bOnAlarm;
	bool                    * m_pDeletionSignal;     // we use this to signal our own delete
public:
	// lag checks should be done only against the user's server
	// please make SURE that the key is unique!
	void lagCheckRegister(const char * key,unsigned int uReliability = 50);
	bool lagCheckComplete(const char * key);
	void lagCheckAbort(const char * key);
	unsigned int lag(){ return m_uLag; };
	unsigned int secondsSinceLastCompleted();
protected:
	virtual void timerEvent(QTimerEvent * e);
};

#endif // _KVI_LAGMETER_H_
