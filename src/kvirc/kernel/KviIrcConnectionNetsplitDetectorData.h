#ifndef _KVI_IRCCONNECTIONNETSPLITDETECTORDATA_H_
#define _KVI_IRCCONNECTIONNETSPLITDETECTORDATA_H_
//=============================================================================
//
//   File : KviIrcConnectionNetsplitDetectorData.h
//   Creation date : Sat 26 Jun 2004 18:20:58 by Szymon Stefanek
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
#include "KviTimeUtils.h"

#include <QString>

class KVIRC_API KviIrcConnectionNetsplitDetectorData
{
public:
	KviIrcConnectionNetsplitDetectorData();
	~KviIrcConnectionNetsplitDetectorData();

protected:
	QString m_szLastNetsplitOnQuitReason;
	kvi_time_t m_tLastNetsplitOnQuit = 0;

public:
	const QString & lastNetsplitOnQuitReason() const { return m_szLastNetsplitOnQuitReason; }
	void setLastNetsplitOnQuitReason(const QString & szReason) { m_szLastNetsplitOnQuitReason = szReason; }
	kvi_time_t lastNetsplitOnQuitTime() const { return m_tLastNetsplitOnQuit; }
	void setLastNetsplitOnQuitTime(kvi_time_t t) { m_tLastNetsplitOnQuit = t; }
};

#endif //!_KVI_IRCCONNECTIONNETSPLITDETECTORDATA_H_
