//=============================================================================
//
//   File : KviIrcConnectionRequestQueue.cpp
//   Creation date : Tue 09 Feb 2010 10:24:32 by Fabio Bas
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2010 Fabio Bas <ctrlaltca at gmail dot com>
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

#include "KviIrcConnectionRequestQueue.h"

#include "KviChannelWindow.h"
#include "KviOptions.h"
#include "KviLagMeter.h"
#include "KviIrcConnectionStateData.h"
#include "KviIrcConnectionServerInfo.h"

#include <QByteArray>

KviIrcConnectionRequestQueue::KviIrcConnectionRequestQueue()
{
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(timerSlot()));
}

KviIrcConnectionRequestQueue::~KviIrcConnectionRequestQueue()
{
	disconnect(&m_timer, SIGNAL(timeout()), this, SLOT(timerSlot()));
}

void KviIrcConnectionRequestQueue::enqueueChannel(KviChannelWindow * pChan)
{
	if(!m_channels.contains(pChan))
	{
		m_channels.enqueue(pChan);
		if(!m_timer.isActive())
			m_timer.start(KVI_OPTION_UINT(KviOption_uintOnJoinRequestsDelay) * 1000);
	}
}

void KviIrcConnectionRequestQueue::dequeueChannel(KviChannelWindow * pChan)
{
	if(m_channels.contains(pChan))
	{
		m_channels.removeOne(pChan);
		if(m_channels.isEmpty())
			m_timer.stop();
	}
}

void KviIrcConnectionRequestQueue::clearAll()
{
	m_timer.stop();
	m_channels.clear();
	m_curType = Mode;
}

void KviIrcConnectionRequestQueue::timerSlot()
{
	if(m_channels.isEmpty())
	{
		m_timer.stop();
	}
	else
	{
		KviChannelWindow * pChan = m_channels.head();
		QByteArray encodedChan = pChan->connection()->encodeText(pChan->target()).data();
		/* The following switch will let the execution flow pass-through if any request type
		 * is currently disabled (or not available on the server). Channel's "MODE" request is
		 * the only mandatory request.
		 */
		switch(m_curType)
		{
			case BanException:
				if(pChan->serverInfo()->supportedListModes().contains('e') && !KVI_OPTION_BOOL(KviOption_boolDisableBanExceptionListRequestOnJoin) && !(pChan->serverInfo()->getNeedsOpToListModeseI() && !pChan->isMeOp()))
				{
					if(!pChan->connection()->sendFmtData("MODE %s e", encodedChan.data()))
						clearAll(); // disconnected
					else
						pChan->setSentListRequest('e');
					m_curType = Invite;
					break;
				}
			case Invite:
				if(pChan->serverInfo()->supportedListModes().contains('I') && !KVI_OPTION_BOOL(KviOption_boolDisableInviteListRequestOnJoin) && !(pChan->serverInfo()->getNeedsOpToListModeseI() && !pChan->isMeOp()))
				{
					if(!pChan->connection()->sendFmtData("MODE %s I", encodedChan.data()))
						clearAll(); // disconnected
					else
						pChan->setSentListRequest('I');
					m_curType = QuietBan;
					break;
				}
			case QuietBan:
				if(pChan->serverInfo()->supportedListModes().contains('q') && !KVI_OPTION_BOOL(KviOption_boolDisableQuietBanListRequestOnJoin))
				{
					if(!pChan->connection()->sendFmtData("MODE %s q", encodedChan.data()))
						clearAll(); // disconnected
					else
						pChan->setSentListRequest('q');
					m_curType = Who;
					break;
				}
			case Who:
				if(!KVI_OPTION_BOOL(KviOption_boolDisableWhoRequestOnJoin))
				{
					// TODO: cleanup
					pChan->connection()->stateData()->setLastSentChannelWhoRequest(kvi_unixTime());
					if(pChan->connection()->lagMeter())
					{
						KviCString tmp;
						if(pChan->serverInfo()->supportsWhox())
							tmp.sprintf("WHO %s %acdfhlnrsu", encodedChan.data());
						else
							tmp.sprintf("WHO %s", encodedChan.data());
						pChan->connection()->lagMeter()->lagCheckRegister(tmp.ptr(), 60);
					}
					if(pChan->serverInfo()->supportsWhox())
					{
						if(!pChan->connection()->sendFmtData("WHO %s %acdfhlnrsu", encodedChan.data()))
							clearAll(); // disconnected
						else
							pChan->setSentWhoRequest();
					}
					else
					{
						if(!pChan->connection()->sendFmtData("WHO %s", encodedChan.data()))
							clearAll(); // disconnected
						else
							pChan->setSentWhoRequest();
					}
					m_curType = Ban;
					break;
				}
			case Ban:
				if(!KVI_OPTION_BOOL(KviOption_boolDisableBanListRequestOnJoin))
				{
					if(!pChan->connection()->sendFmtData("MODE %s b", encodedChan.data()))
					{
						clearAll(); // disconnected
					}
					else
					{
						pChan->setSentListRequest('b');
						m_channels.dequeue();
					}
					m_curType = Mode;
					break;
				}
			default:
				// we're at the end of the list
				m_channels.dequeue();
				pChan->checkChannelSync();
				m_curType = Mode;
				if(m_channels.isEmpty())
				{
					m_timer.stop();
					return;
				}
				pChan = m_channels.head();
				encodedChan = pChan->connection()->encodeText(pChan->target());
			case Mode:
				if(!pChan->connection()->sendFmtData("MODE %s", encodedChan.data()))
				{
					clearAll(); // disconnected
					break;
				}
				m_curType = BanException;
				break;
		}
	}
}
