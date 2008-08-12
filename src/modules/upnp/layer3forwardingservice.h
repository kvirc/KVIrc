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
                          layer3forwardingservice.h -  description
                             -------------------
    begin                : Mon Jul 25 2005
    copyright            : (C) 2005 by Diederik van der Boor
    email                : vdboor --at-- codingdomain.com
 ***************************************************************************/

#ifndef UPNP_LAYER3FORWARDINGSERVICE_H
#define UPNP_LAYER3FORWARDINGSERVICE_H

#include "service.h"

namespace UPnP {

/**
 * The Layer3Forwarding service is used to query a Internet Gateway Device (router in UPnP terms)
 * for it's WanConnection service. This can be an instance of the WanIPConnection or WanPPPConnection service,
 * which is implemented by the WanConnectionService class.
 * The WanIPConnection/WanPPPConnection service can be used to configure the external connection settings and port mappings of the router.
 * The Layer3Forwarding service itself is resolved by the RootService class.
 *
 * @author Diederik van der Boor
 * @ingroup NetworkUPnP
 */
class Layer3ForwardingService : public Service
{
public:  // public methods

	// The constructor
	Layer3ForwardingService(const ServiceParameters &params);
	// The destructor
	virtual             ~Layer3ForwardingService();

	// Get the device UDN of the default connection service
	QString              getConnectionDeviceUdn() const;
	// Get the service ID of the default connection service
	QString              getConnectionServiceId() const;

	// Query the Layer3Forwarding service for the default connection service
	void                 queryDefaultConnectionService();


protected:  // protected methods

	// The control point received a response to callAction()
	virtual void         gotActionResponse(const QString &responseType, const QMap<QString,QString> &resultValues);


private:  // private attributes

	// The service ID of the default connection service
	QString              connectionServiceId_;
	// The device UDN of the default connection service
	QString              connectionDeviceUdn_;
};

}

#endif
