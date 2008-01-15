#ifndef _KVI_IRCCONNECTIONASYNCWHOISDATA_H_
#define _KVI_IRCCONNECTIONASYNCWHOISDATA_H_
//=============================================================================
//
//   File : kvi_ircconnectionasyncwhoisdata.h
//   Created on Sat 26 Jun 2004 19:40:20 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004 Szymon Stefanek <pragma at kvirc dot net>
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#include "kvi_settings.h"
#include "kvi_qstring.h"
#include "kvi_pointerlist.h"

class KviWindow;
class KviKvsScript;
class KviKvsVariant;

class KVIRC_API KviAsyncWhoisInfo
{
public:
	QString szNick;
	QString szUser;
	QString szHost;
	QString szReal;
	QString szServer;
	QString szIdle;
	QString szSignon;
	QString szChannels;
	QString szSpecial;
	KviKvsScript * pCallback;
	KviKvsVariant * pMagic;
	QString szCode; // <-- kill this when KVS is hardwired!
	QString szMagic; // <-- kill this when KVS is hardwired!
	KviWindow * pWindow;
public:
	KviAsyncWhoisInfo();
	~KviAsyncWhoisInfo();
};



class KVIRC_API KviIrcConnectionAsyncWhoisData
{
public:
	KviIrcConnectionAsyncWhoisData();
	~KviIrcConnectionAsyncWhoisData();
protected:
	KviPointerList<KviAsyncWhoisInfo> * m_pWhoisInfoList; // awhois pending stuff
public:
	void add(KviAsyncWhoisInfo * i);
	KviAsyncWhoisInfo * lookup(const QString &nick);
	void remove(KviAsyncWhoisInfo * i);
};

#endif //!_KVI_IRCCONNECTIONASYNCWHOISDATA_H_
