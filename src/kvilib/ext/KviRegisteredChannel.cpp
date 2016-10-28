//=============================================================================
//
//   File : KviRegisteredChannel.cpp
//   Creation date : Sat Jun 29 01:01:16 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
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

#include "kvi_debug.h"
#include "KviRegisteredChannel.h"

#include <QString>

KviRegisteredChannel::KviRegisteredChannel(const QString & szName, const QString & szNetMask)
{
	m_szName = szName;
	m_szNetMask = szNetMask;
	m_pPropertyDict = new QHash<QString, QString>();
}

KviRegisteredChannel::~KviRegisteredChannel()
{
	delete m_pPropertyDict;
}
