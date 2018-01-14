//=============================================================================
//
//   File : KviLagMeter.cpp
//   Creation date : Fri Oct 18 13:31:36 CEST 1999 by Juanjo Álvarez
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 1999 Juanjo Álvarez
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviLagMeter.h"
#include "KviOptions.h"
#include "KviKvsEventTriggers.h"
#include "KviIrcConnection.h"
#include "KviIrcContext.h"
#include "KviMainWindow.h"
#include "KviConsoleWindow.h"
#include "KviTimeUtils.h"
#include "KviIrcConnectionUserInfo.h"
#include "KviIrcConnectionServerInfo.h"
#include "kvi_out.h"
#include "KviLocale.h"

#include <algorithm>

KviLagMeter::KviLagMeter(KviIrcConnection * c)
    : QObject(), m_pConnection(c)
{
	// FIXME: We could use the KviIrcConnection::heartbeat() here!
	if(KVI_OPTION_UINT(KviOption_uintLagMeterHeartbeat) < 2000)
		KVI_OPTION_UINT(KviOption_uintLagMeterHeartbeat) = 2000; // kinda absurd

	if(KVI_OPTION_UINT(KviOption_uintLagMeterHeartbeat) > 10000)
		KVI_OPTION_UINT(KviOption_uintLagMeterHeartbeat) = 10000; // kinda absurd

	startTimer(KVI_OPTION_UINT(KviOption_uintLagMeterHeartbeat)); // 5 seconds by default
}

KviLagMeter::~KviLagMeter()
{
	if(m_pDeletionSignal)
		*m_pDeletionSignal = true;
		
	qDeleteAll(m_lCheckList);
}

unsigned int KviLagMeter::secondsSinceLastCompleted()
{
	struct timeval tv;
	kvi_gettimeofday(&tv);
	return tv.tv_sec - m_tLastCompleted;
}

void KviLagMeter::timerEvent(QTimerEvent *)
{
	if(m_pConnection->state() != KviIrcConnection::Connected)
		return; // do nothing atm

	// If the lag has changed emit our signals
	if((m_uLag / 10) != (m_uLastEmittedLag / 10))
	{
		m_uLastEmittedLag = m_uLag;
		g_pMainWindow->childConnectionLagChange(m_pConnection);

		KviCString szLag(KviCString::Format, "%u", m_uLag);

		bool bDeletionSignal = false;
		m_pDeletionSignal = &bDeletionSignal;

		if((!m_bOnAlarm) && (m_uLag > KVI_OPTION_UINT(KviOption_uintLagAlarmTime)))
		{
			KVS_TRIGGER_EVENT_2_HALTED(KviEvent_OnLagAlarmTimeUp,
			    m_pConnection->console(), m_pConnection->serverInfo()->name(), QString(szLag.ptr()));
			if(bDeletionSignal)
				return; // killed, probably by a quit -f -u
			m_bOnAlarm = true;
		}
		else if(m_bOnAlarm)
		{
			KVS_TRIGGER_EVENT_2_HALTED(KviEvent_OnLagAlarmTimeDown,
			    m_pConnection->console(), m_pConnection->serverInfo()->name(), QString(szLag.ptr()));
			if(bDeletionSignal)
				return; // killed, probably by a quit -f -u
			m_bOnAlarm = false;
		}

		KVS_TRIGGER_EVENT_2_HALTED(KviEvent_OnLagCheck,
		    m_pConnection->console(), m_pConnection->serverInfo()->name(), QString(szLag.ptr()));
		if(bDeletionSignal)
			return; // killed, probably by a quit -f -u

		m_pDeletionSignal = nullptr;
	}

	// get current time
	struct timeval tv;
	kvi_gettimeofday(&tv);
	unsigned int uDiff = tv.tv_sec - m_tLastCompleted;
	unsigned int uHeartbeat = KVI_OPTION_UINT(KviOption_uintLagMeterHeartbeat) / 1000;
	if(uHeartbeat < 2)
		uHeartbeat = 2;
	// we keep the last lag value for an amount of time
	// depending on its reliability.
	// Since reliability ranges from 10 to 100 we keep the lags
	// for (hrtbt * 4) + (reliability / 2) seconds (which means from 25 to 70 seconds by default)
	if(uDiff <= ((uHeartbeat * 4) + (m_uLastReliability / 2)))
		return; // nothing to do, the actual value is accurate

	// the last completed check has been completed a lot of time ago
	// do we have some checks on the queue ?
	if(m_lCheckList.count() > 0)
	{
		// if the first registered check is not too outdated
		// we wait a little more for it to return
		KviLagCheck * c = m_lCheckList.first();
		if(c)
		{
			if((tv.tv_sec - c->lSecs) <= 10)
				return;
		}
		// the first check was registered more than 10 secs before
		if(m_tLastOwnCheck > 0)
		{
			// hm.. we have already sent our own (reliable) check after the last completed
			// make the lag grow (we're pretty sure it's growing)
			uDiff = (tv.tv_sec - m_tFirstOwnCheck) * 1000;
			if(m_uLag < uDiff)
				m_uLag = uDiff; // the lag grows for sure
			uDiff = tv.tv_sec - m_tLastOwnCheck;
			if(uDiff < (uHeartbeat * 4))
				return; // wait a bit...send own checks only every 20 secs (by default) at this point
		}
	}
	// or we have no checks in the queue at all
	// or it's really time to do something...
	if(m_tFirstOwnCheck == 0)
	{
		if(_OUTPUT_PARANOIC)
			m_pConnection->console()->output(KVI_OUT_VERBOSE, __tr2qs("Sending out PING based lag probe"));

		// this is the first our own lag check since the last successful one: use the ping
		lagCheckRegister("@ping@", 70); // the ping may be fooled easily
		m_pConnection->sendFmtData("PING %s %s",
		    m_pConnection->encodeText(m_pConnection->userInfo()->nickName()).data(),
		    m_pConnection->encodeText(m_pConnection->serverInfo()->name()).data());
		m_tFirstOwnCheck = tv.tv_sec;
	}
	else
	{
		if(_OUTPUT_PARANOIC)
			m_pConnection->console()->output(KVI_OUT_VERBOSE, __tr2qs("Sending out CTCP based lag probe"));

		// we have already sent a ping but we got no reply
		// try with another method... even if this will reset our idle time
		KviCString tmp(KviCString::Format, "%d%d-yeah-:)", tv.tv_sec, tv.tv_usec);
		lagCheckRegister(tmp.ptr(), 100); // almost impossible to fool
		m_pConnection->sendFmtData("NOTICE %s :%cLAGCHECK %s%c",
		    m_pConnection->encodeText(m_pConnection->userInfo()->nickName()).data(),
		    0x01,
		    tmp.ptr(),
		    0x01);
	}
	m_tLastOwnCheck = tv.tv_sec;
}

void KviLagMeter::lagCheckRegister(const char * key, unsigned int uReliability)
{
	if(uReliability < 10)
		return; // what the heck of a lag check is this ?
	// store the lagcheck structure and just return

	if(_OUTPUT_PARANOIC)
		m_pConnection->console()->output(KVI_OUT_VERBOSE, __tr2qs("Registered lag check with reliability %u (%s)"), uReliability, key);

	KviLagCheck * c = new KviLagCheck();
	c->szKey = key;
	struct timeval tv;
	kvi_gettimeofday(&tv);
	c->lSecs = tv.tv_sec;
	c->lUSecs = tv.tv_usec;
	c->uReliability = uReliability <= 100 ? uReliability : 100;
	m_lCheckList.append(c);
	while(m_lCheckList.size() > 30)
	{
		// we're fried :/
		// either our ping mechanism is not working
		// or the server is stoned...
		delete m_lCheckList.takeFirst();
	}
}

bool KviLagMeter::lagCheckComplete(const char * key)
{
	// find this lag check
	KviLagCheck * c = nullptr;
	for(auto cc : m_lCheckList)
	{
		if(kvi_strEqualCS(cc->szKey.ptr(), key))
		{
			c = cc;
			break;
		}
	}
	if(!c)
		return false; // not found

	// kill any earlier lag checks (IRC is a sequential proto)
	while(m_lCheckList.first() != c)
		delete m_lCheckList.takeFirst();

	if(_OUTPUT_PARANOIC)
		m_pConnection->console()->output(KVI_OUT_VERBOSE, __tr2qs("Lag check completed (%s)"), key);

	struct timeval tv;
	kvi_gettimeofday(&tv);

	unsigned int uLag = ((tv.tv_sec - c->lSecs) * 1000);
	if(tv.tv_usec < c->lUSecs)
		uLag -= ((c->lUSecs - tv.tv_usec) / 1000);
	else
		uLag += ((tv.tv_usec - c->lUSecs) / 1000);

	// now check the reliability

	if(m_uLastReliability > c->uReliability)
	{
		// the actual data is more reliable than the new one :/
		// change the real lag only by a certain amount
		// c->uRel : 100 = uLag : m_uLag
		m_uLag = ((uLag * c->uReliability) + (m_uLag * m_uLastReliability)) / (c->uReliability + m_uLastReliability);
	}
	else
	{
		// the actual data is less reliable than the new one
		m_uLag = uLag;
	}

	m_tLastCompleted = tv.tv_sec; // now
	m_tLastOwnCheck = 0;
	m_tFirstOwnCheck = 0;
	m_uLastReliability = c->uReliability;

	delete m_lCheckList.takeFirst();

	return true;
}

void KviLagMeter::lagCheckAbort(const char * key)
{
	if(_OUTPUT_PARANOIC)
		m_pConnection->console()->output(KVI_OUT_VERBOSE, __tr2qs("Lag check aborted (%s)"), key);

	QList<KviLagCheck *> lAborted;

	for(auto c : m_lCheckList)
	{
		if(kvi_strEqualCS(c->szKey.ptr(), key))
			lAborted.append(c);
	}
	
	qDeleteAll(lAborted);
}
