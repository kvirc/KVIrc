#ifndef _KVI_LAGMETER_H_
#define _KVI_LAGMETER_H_
//=============================================================================
//
//   File : KviLagMeter.h
//   Creation date : Fri Oct 18 13:30:26 CEST 1999 by Juanjo Álvarez
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 1999 Juanjo Álvarez
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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
#include "KviCString.h"

#include <QObject>
#include <QList>

class KviIrcConnection;

struct KviLagCheck
{
	KviCString szKey;
	long lSecs; // since epoch
	long lUSecs;
	unsigned int uReliability; // [0-100]
};

class KVIRC_API KviLagMeter : public QObject
{
	friend class KviIrcConnection;
	Q_OBJECT

protected:
	KviLagMeter(KviIrcConnection * c);
	~KviLagMeter();

protected:
	KviIrcConnection * m_pConnection = nullptr;
	unsigned int m_uLag = 0;             // last computed lag
	unsigned int m_uLastEmittedLag = 0;  // last emitted lag
	long m_tLastCompleted = 0;           // time when the last lag was completed (gettimeofday!)
	unsigned int m_uLastReliability = 0; // how much reliable was the last completed check ?
	QList<KviLagCheck *> m_lCheckList;
	long m_tFirstOwnCheck = 0; // time when the first ping after a completed check was sent
	long m_tLastOwnCheck = 0;  // time when the last ping was sent
	bool m_bOnAlarm = false;
	bool * m_pDeletionSignal = nullptr; // we use this to signal our own delete
public:
	// lag checks should be done only against the user's server
	// please make SURE that the key is unique!
	void lagCheckRegister(const char * key, unsigned int uReliability = 50);
	bool lagCheckComplete(const char * key);
	void lagCheckAbort(const char * key);
	unsigned int lag() const { return m_uLag; }
	unsigned int secondsSinceLastCompleted();

protected:
	void timerEvent(QTimerEvent * e) override;
};

#endif // _KVI_LAGMETER_H_
