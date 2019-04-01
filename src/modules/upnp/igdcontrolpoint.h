//=============================================================================
//
//   File : igdcontrolpoint.h
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
                          igdcontrolpoint.cpp -  description
                             -------------------
    begin                : Mon Jul 25 2005
    copyright            : (C) 2005 by Diederik van der Boor
    email                : vdboor --at-- codingdomain.com
 ***************************************************************************/

#ifndef UPNP_IGDCONTROLPOINT_H
#define UPNP_IGDCONTROLPOINT_H

#include <QObject>

namespace UPnP
{

	class SsdpConnection;
	class RootService;
	class WanConnectionService;

	/**
 * A control point is a UPnP term for "client".
 * It's the host that controls the UPnP device.
 * This control point specifically handles Internet Gateway Devices (routers in UPnP terminology).
 * It queries the device for its port mapping service (an instance of a WanIPConnection or WanPPPConnection service).
 * Once the service is found, it can be used from the Manager class to configure port mappings.
 *
 * @author Diederik van der Boor
 * @ingroup NetworkUPnP
 */
	class IgdControlPoint : public QObject
	{
		Q_OBJECT

	public: // public methods
		// The constructor
		IgdControlPoint(const QString & hostname, int port, const QString & rootUrl);
		// The destructor
		~IgdControlPoint();

		// Return the external IP address
		QString getExternalIpAddress() const;
		// Initialize the control point
		void initialize();
		// Return true if a controlable gateway is available
		bool isGatewayAvailable();

		// Add a port mapping
		void addPortMapping(const QString & protocol, const QString & remoteHost, int externalPort, const QString & internalClient, int internalPort, const QString & description, bool enabled = true, int leaseDuration = 0);
		// Delete a port mapping
		void deletePortMapping(const QString & protocol, const QString & remoteHost, int externalPort);

	private slots:
		// The IGD was queried for it's services
		void slotDeviceQueried(bool error);
		// A WAN connection query was finished
		void slotWanQueryFinished(bool error);

	private: // private attributes
		// Is a gateway available?
		bool m_bGatewayAvailable;
		// The host of the gateway
		QString m_szIgdHostname;
		// The port of the gateway
		int m_iIgdPort;
		// The root service
		RootService * m_pRootService;
		// The wan connection service
		WanConnectionService * m_pWanConnectionService;
	};
}

#endif
