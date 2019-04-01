#ifndef _KVI_IRCCONNECTIONREQUESTQUEUE_H_
#define _KVI_IRCCONNECTIONREQUESTQUEUE_H_
//=============================================================================
//
//   File : KviIrcConnectionRequestQueue.h
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

/**
* \file KviIrcConnectionRequestQueue.h
* \author Fabio Bas
* \brief IRC connection request queue
*/

#include "kvi_settings.h"

#include <QTimer>
#include <QQueue>

class KviChannelWindow;

/**
* \class KviIrcConnectionRequestQueue
* \brief Class to enqueue commands to IRC server
*
* This class is designed to delay channel requests like MODE and WHO to avoid
* excess floods on some servers
*/
class KVIRC_API KviIrcConnectionRequestQueue : public QObject
{
	Q_OBJECT
public:
	/**
	* \brief Constructs the request queue objects
	* \return KviIrcConnectionRequestQueue
	*/
	KviIrcConnectionRequestQueue();

	/**
	* \brief Destroys the request queue objects
	*/
	~KviIrcConnectionRequestQueue();

protected:
	/**
	* \enum RequestTypes
	*
	* \warning MODE %s b MUST BE THE LAST AUTOMATIC CHANNEL QUERY so we get
	* RPL_ENDOFBANLIST as the last reply and we know that the channel is in sync
	*/
	enum RequestTypes
	{
		Mode = 0,         /**< Channel modes request */
		BanException = 1, /**< Ban exceptions request */
		Invite = 2,       /**< Invites request */
		Who = 3,          /**< Who request */
		QuietBan = 4,     /**< Quiet ban request */
		Ban = 5           /**< Ban request */
	};

	QQueue<KviChannelWindow *> m_channels;
	QTimer m_timer;
	RequestTypes m_curType = Mode;

public:
	/**
	* \brief Enqueues the channel in the queue stack
	* \param pChan The channel to enqueue
	* \return void
	*/
	void enqueueChannel(KviChannelWindow * pChan);

	/**
	* \brief Removes the channel from the queue stack
	* \param pChan The channel to dequeue
	* \return void
	*/
	void dequeueChannel(KviChannelWindow * pChan);

	/**
	* \brief Checks if a channel is in the queue stack
	* \param pChan The channel to check
	* \return bool
	*/
	bool isQueued(KviChannelWindow * pChan) const { return m_channels.contains(pChan); }

	/**
	* \brief Clears the queue stack
	* \return void
	*/
	void clearAll();
private slots:
	/**
	* \brief Performs time based requests
	* \return void
	*/
	void timerSlot();
};

#endif //!_KVI_IRCCONNECTIONREQUESTQUEUE_H_
