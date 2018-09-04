#ifndef _KVI_IRCDATASTREAMMONITOR_H_
#define _KVI_IRCDATASTREAMMONITOR_H_
//=============================================================================
//
//   File : KviIrcDataStreamMonitor.h
//   Creation date : Sun 04 Jul 2004 21:27:22 by Szymon Stefanek
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
#include "KviHeapObject.h"

class KviIrcContext;

class KVIRC_API KviIrcDataStreamMonitor : public KviHeapObject
{
public:
	KviIrcDataStreamMonitor(KviIrcContext * pContext);
	virtual ~KviIrcDataStreamMonitor();

protected:
	KviIrcContext * m_pMyContext;

public:
	virtual bool incomingMessage(const char *) = 0;
	// For proxy connections it might spit out binary data!
	virtual bool outgoingMessage(const char *) = 0;
	virtual void connectionInitiated(){}
	virtual void connectionTerminated(){}
	virtual void die() { delete this; }
};

#endif //!_KVI_IRCDATASTREAMMONITOR_H_
