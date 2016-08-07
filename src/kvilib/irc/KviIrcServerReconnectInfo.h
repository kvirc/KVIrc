#ifndef _KVI_IRCSERVER_INFO_H_
#define _KVI_IRCSERVER_INFO_H_
//=============================================================================
//
//   File : KviIrcServerReconnectInfo.h
//   Creation date : Tue Jan 04 2010 21:44:11 by Elvio Basello
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2010 Elvio Basello (hellvis69 at gmail dot com)
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

/**
* \file KviIrcServerReconnectInfo.h
* \author Szymon Stefanek
* \brief Irc server handling
*
* This file was originally part of KviIrcServer.h
*/

#include "kvi_settings.h"

#include <QString>
#include <QStringList>
#include <utility>
#include <vector>

/**
* \class KviIrcServerReconnectInfo
* \brief A class for reconnecting purposes
*/
class KVILIB_API KviIrcServerReconnectInfo
{
public:
	/**
	* \brief Constructs the server reconnect info object
	* \return KviIrcServerReconnectInfo
	*/
	KviIrcServerReconnectInfo() : m_bIsAway(false) {};

public:
	QString m_szNick;
	QString m_szPass;
	QString m_szAwayReason;
	QStringList m_lOpenQueries;
	bool m_bIsAway;

	// The list of channels to join (first = channel name, second = password)
	std::vector<std::pair<QString, QString>> m_lJoinChannels;
};

#endif //_KVI_IRCSERVER_INFO_H_
