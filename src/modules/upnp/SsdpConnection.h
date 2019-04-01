//=============================================================================
//
//   File : SsdpConnection.h
//   Creation date : Fri Aug 08 18:00:00 2008 GMT by Fabio Bas
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2008 Fabio Bas (ctrlaltca at gmail dot com)
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
//   Original Copyright follows:
//=============================================================================

/***************************************************************************
                          SsdpConnection.h -  description
                             -------------------
    begin                : Fri Jul 29 2005
    copyright            : (C) 2005 by Diederik van der Boor
    email                : vdboor --at-- codingdomain.com
 ***************************************************************************/

#ifndef UPNP_SSDPCONNECTION_H
#define UPNP_SSDPCONNECTION_H

#include <QObject>

class QUdpSocket;

namespace UPnP
{

	/**
 * The Simple Service Discovery Protocol allows UPnP clients
 * to discover UPnP devices on a network.
 * This is achieved by broadcasting a HTTP-like message over UDP.
 * Devices can respond with their location and root service name.
 * The RootService class uses this information to query the device for
 * it's meta information and service list.
 *
 * @author Diederik van der Boor
 * @ingroup NetworkUPnP
 */
	class SsdpConnection : public QObject
	{
		Q_OBJECT

	public:
		SsdpConnection();
		~SsdpConnection();

		void queryDevices(int bindPort = 1500);

	private slots:
		// Data was received by the socket
		void slotDataReceived();

	private:
		QUdpSocket * m_pSocket;
	signals:
		// Called when a query completed
		void deviceFound(const QString & hostname, int port, const QString & rootUrl);
	};
}

#endif
