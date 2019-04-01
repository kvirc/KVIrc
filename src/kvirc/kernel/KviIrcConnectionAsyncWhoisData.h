#ifndef _KVI_IRCCONNECTIONASYNCWHOISDATA_H_
#define _KVI_IRCCONNECTIONASYNCWHOISDATA_H_
//=============================================================================
//
//   File : KviIrcConnectionAsyncWhoisData.h
//   Creation date : Sat 26 Jun 2004 19:40:20 by Szymon Stefanek
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
#include "KviIrcConnectionAsyncData.h"

#include <QString>

class KviKvsScript;
class KviKvsVariant;
class KviWindow;

class KVIRC_API KviAsyncWhoisInfo
{
public:
	KviAsyncWhoisInfo();
	~KviAsyncWhoisInfo();

public:
	QString szNick;
	QString szUser;
	QString szHost;
	QString szReal;
	QString szServer;
	QString szIdle;
	QString szSignon;
	QString szChannels;
	QString szAway;
	QString szAuth;
	QString szAdditional;
	KviKvsScript * pCallback = nullptr;
	KviKvsVariant * pMagic = nullptr;
	KviWindow * pWindow = nullptr;
};

// KviIrcConnectionAsyncWhoisData is now recreated using a template
// I'm using inheritance as i haven't been able to use a typedef
// This line didn't build
// 	typedef KviIrcConnectionAsyncData<KviAsyncWhoisInfo> KviIrcConnectionAsyncWhoisData
class KVIRC_API KviIrcConnectionAsyncWhoisData : public KviIrcConnectionAsyncData<KviAsyncWhoisInfo>
{
};

#endif //!_KVI_IRCCONNECTIONASYNCWHOISDATA_H_
