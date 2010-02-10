#ifndef _KVI_IRCCONNECTIONREQUESTQUEUE_H_
#define _KVI_IRCCONNECTIONREQUESTQUEUE_H_
//=============================================================================
//
//   File : kvi_ircconnectionrequestqueue.h
//   Creation date : Tue 09 Feb 2010 10:24:32 by Fabio Bas
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2010 Fabio Bas <ctrlaltca at gmail dot com>
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

#include <QTimer>
#include <QQueue>

class KviChannel;

class KVIRC_API KviRequestQueue: public QObject
{
	Q_OBJECT
protected:
	QQueue<KviChannel *> channels;
	QTimer timer;
	// MODE %s b MUST BE THE LAST AUTOMATIC CHANNEL QUERY
	// so we get RPL_ENDOFBANLIST as the last reply
	// and we know that the channel is in sync
	enum requestTypes {
		ModeRequest = 0,
		BanERequest = 1,
		InviRequest = 2,
		WhoRequest = 3,
		QuietBanRequest = 4,
		BanRequest = 5
	};
	requestTypes curType;
public:
	KviRequestQueue();
	virtual ~KviRequestQueue();
public:
	void enqueueChannel(KviChannel *pChan);
	void dequeueChannel(KviChannel *pChan);
	void clearAll();
private slots:
	void timerSlot();
};

#endif //!_KVI_IRCCONNECTIONREQUESTQUEUE_H_
