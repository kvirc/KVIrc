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

#include "kvi_ircconnectionrequestqueue.h"

#include "kvi_channel.h"
#include "kvi_options.h"
#include "kvi_lagmeter.h"
#include "kvi_ircconnectionstatedata.h"

#include <QByteArray>

KviRequestQueue::KviRequestQueue()
{
	curType=ModeRequest;
	connect(&timer, SIGNAL(timeout()), this, SLOT(timerSlot()));
}

KviRequestQueue::~KviRequestQueue()
{
	disconnect(&timer, SIGNAL(timeout()), this, SLOT(timerSlot()));
}

void KviRequestQueue::enqueueChannel(KviChannel *pChan)
{
	if(!channels.contains(pChan))
	{
		channels.enqueue(pChan);
		if(!timer.isActive()) timer.start(KVI_OPTION_UINT(KviOption_uintOnJoinRequestsDelay)*1000);
	}
}

void KviRequestQueue::dequeueChannel(KviChannel *pChan)
{
	if(channels.contains(pChan))
	{
		channels.removeOne(pChan);
		if(channels.isEmpty())
			timer.stop();
	}
}

void KviRequestQueue::clearAll()
{
	channels.clear();
	timer.stop();
	curType=ModeRequest;
}

void KviRequestQueue::timerSlot()
{
	if(channels.isEmpty())
	{
		timer.stop();
	} else {
		KviChannel* chan = channels.head();
		QByteArray encodedChan = chan->connection()->encodeText(chan->target()).data();
		/* The following switch will let the execution flow pass-trought if any request type
		 * is currently disabled (or not available on the server). Channel's "MODE" request is
		 * the only mandatory request.
		 */
		switch(curType)
		{
			case BanERequest:
				if(chan->connection()->serverInfo()->supportsModesIe() &&
					!KVI_OPTION_BOOL(KviOption_boolDisableBanExceptionListRequestOnJoin))
				{
					if(!chan->connection()->sendFmtData("MODE %s e",encodedChan.data()))
						clearAll(); // disconnected
					else chan->setSentBanExceptionListRequest();
					curType=InviRequest;
					break;
				}
			case InviRequest:
				if(chan->connection()->serverInfo()->supportsModesIe() &&
					!KVI_OPTION_BOOL(KviOption_boolDisableInviteListRequestOnJoin))
				{
					if(!chan->connection()->sendFmtData("MODE %s I",encodedChan.data()))
						clearAll(); // disconnected
					else chan->setSentInviteListRequest();
					curType=QuietBanRequest;
					break;
				}
			case QuietBanRequest:
				if(chan->connection()->serverInfo()->supportsModeq() &&
					!KVI_OPTION_BOOL(KviOption_boolDisableQuietBanListRequestOnJoin))
				{
					if(!chan->connection()->sendFmtData("MODE %s q",encodedChan.data()))
						clearAll(); // disconnected
					else chan->setSentQuietBanListRequest();
					curType=WhoRequest;
					break;
				}
			case WhoRequest:
				if(!KVI_OPTION_BOOL(KviOption_boolDisableWhoRequestOnJoin))
				{
					chan->connection()->stateData()->setLastSentChannelWhoRequest(kvi_unixTime());
					if(chan->connection()->lagMeter())
					{
						KviStr tmp;
						tmp.sprintf("WHO %s",encodedChan.data());
						chan->connection()->lagMeter()->lagCheckRegister(tmp.ptr(),60);
					}
					if(!chan->connection()->sendFmtData("WHO %s",encodedChan.data()))
						clearAll(); // disconnected
					else chan->setSentWhoRequest();
					curType=BanRequest;
					break;
				}
			case BanRequest:
				if(!KVI_OPTION_BOOL(KviOption_boolDisableBanListRequestOnJoin))
				{
					if(!chan->connection()->sendFmtData("MODE %s b",encodedChan.data()))
						clearAll(); // disconnected
					else chan->setSentBanListRequest();
					channels.dequeue();
					curType=ModeRequest;
					break;
				}
			default:
				//we're at the end of the list
				channels.dequeue();
				if(channels.isEmpty())
				{
					timer.stop();
					return;
				}
				chan = channels.head();
				encodedChan = chan->connection()->encodeText(chan->target());
				curType=ModeRequest;
			case ModeRequest:
				if(!chan->connection()->sendFmtData("MODE %s",encodedChan.data()))
				{
					clearAll(); // disconnected
					break;
				}
				curType=BanERequest;
				break;
		}
	}
}