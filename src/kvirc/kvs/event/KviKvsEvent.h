#ifndef _KVI_KVS_EVENT_H_
#define _KVI_KVS_EVENT_H_
//=============================================================================
//
//   File : KviKvsEvent.h
//   Creation date : Mon 23 Feb 2004 03:00:02 by Szymon Stefanek
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
#include "KviQString.h"
#include "KviPointerList.h"

#include "KviKvsEventHandler.h"

class KVIRC_API KviKvsEvent
{
protected:
	QString m_szName;
	QString m_szParameterDescription;
	KviPointerList<KviKvsEventHandler> * m_pHandlers;

public:
	// the event name and the parameter description are NOT translated
	KviKvsEvent(const char * szEventName, const char * szParameterDescription)
	    : m_szName(szEventName), m_szParameterDescription(szParameterDescription), m_pHandlers(0){};
	~KviKvsEvent();
	void clear();
	void clearScriptHandlers();
	bool hasHandlers() { return m_pHandlers != 0; };
	KviPointerList<KviKvsEventHandler> * handlers() { return m_pHandlers; };
	void addHandler(KviKvsEventHandler * h);
	void removeHandler(KviKvsEventHandler * h);
	const QString & name() { return m_szName; };
	const QString & parameterDescription() { return m_szParameterDescription; };
};

#endif //!_KVI_KVS_EVENT_H_
