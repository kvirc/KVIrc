//=============================================================================
//
//   Creation date : Fri Aug 08 18:00:00 2008 GMT by Fabio Bas
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include <qobject.h>

namespace UPnP
{

class SsdpConnection;
class RootService;
class Layer3ForwardingService;
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

public:  // public methods

	// The constructor
	IgdControlPoint(const QString &hostname, int port, const QString &rootUrl);
	// The destructor
	virtual             ~IgdControlPoint();

	// Return the external IP address
	QString              getExternalIpAddress() const;
	// Initialize the control point
	void                 initialize();
	// Return true if a controlable gateway is available
	bool                 isGatewayAvailable();

private slots:
	// The IGD was queried for it's services
	void                 slotDeviceQueried(bool error);
	// A WAN connection service was found
	void                 slotWanConnectionFound(bool error);
	// A WAN connection query was finished
	void                 slotWanQueryFinished(bool error);

private:  // private attibutes
	// The forwarding service
	Layer3ForwardingService *forwardingService_;
	// Is a gateway available?
	bool                 gatewayAvailable_;
	// The host of the gateway
	QString              igdHostname_;
	// The port of the gateway
	int                  igdPort_;
	// The root service
	RootService         *rootService_;
	// The wan connection service
	WanConnectionService *wanConnectionService_;
};


}

#endif
