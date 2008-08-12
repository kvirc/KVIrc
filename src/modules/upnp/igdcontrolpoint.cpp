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

#include <QDebug>

#include "igdcontrolpoint.h"

#include "rootservice.h"
#include "layer3forwardingservice.h"
#include "wanconnectionservice.h"

namespace UPnP
{

#define InternetGatewayDeviceType "urn:schemas-upnp-org:device:InternetGatewayDevice:1"
#define Layer3ForwardingType      "urn:schemas-upnp-org:service:Layer3Forwarding:1"


// The constructor
IgdControlPoint::IgdControlPoint(const QString &hostname, int port, const QString &rootUrl)
: QObject()
, forwardingService_(0)
, gatewayAvailable_(false)
, igdPort_(0)
, rootService_(0)
, wanConnectionService_(0)
{
	qDebug() << "CREATED UPnP::IgdControlPoint: Created control point"
		<< " url='" << hostname << ":" << port << "/" << rootUrl << "'." << endl;

	qDebug() << "UPnP::IgdControlPoint: querying services..." << endl;

	// Store device url
	igdHostname_ = hostname;
	igdPort_     = port;

	// Query the device for it's services
	rootService_ = new RootService(igdHostname_, igdPort_, rootUrl);
	connect(rootService_, SIGNAL(queryFinished(bool)), this, SLOT(slotDeviceQueried(bool)));
}



// The destructor
IgdControlPoint::~IgdControlPoint()
{
	delete rootService_;
	delete forwardingService_;
	delete wanConnectionService_;

	qDebug() << "DESTROYED UPnP::IgdControlPoint [host=" << igdHostname_ << ", port=" << igdPort_ << "]" << endl;
}



// Return the external IP address
QString IgdControlPoint::getExternalIpAddress() const
{
	// Do not expose  wanConnectionService_;
	if(wanConnectionService_ != 0)
	{
		return wanConnectionService_->getExternalIpAddress();
	} else {
		return QString::null;
	}
}


// Initialize the control point
void IgdControlPoint::initialize()
{
	rootService_->queryDevice();
}



// Return true if a controlable gateway is available
bool IgdControlPoint::isGatewayAvailable()
{
	return gatewayAvailable_;
}



// The IGD was queried for it's services
void IgdControlPoint::slotDeviceQueried(bool error)
{
	if(! error)
	{
		// Get the Layer3ForwardingService from the retrieved service list
		ServiceParameters params = rootService_->getServiceByType(Layer3ForwardingType);

		if(! params.controlUrl.isNull())
		{
			qDebug() << "UPnP::IgdControlPoint: Services found, "
					<< "querying service '" << params.serviceId << "' for port mapping service..." << endl;

			// Call the service
			forwardingService_ = new Layer3ForwardingService(params);
			connect(forwardingService_, SIGNAL(queryFinished(bool)), this, SLOT(slotWanConnectionFound(bool)));
			forwardingService_->queryDefaultConnectionService();
		} else {
			qDebug() << "UPnP::IgdControlPoint: No Layer3Forwarding service found on device" << endl;
		}
	}
}



// A WAN connection service was found
void IgdControlPoint::slotWanConnectionFound(bool error)
{
	if(! error)
	{
		// Get the retreived service description
		QString deviceUrn = forwardingService_->getConnectionDeviceUdn();
		QString serviceId = forwardingService_->getConnectionServiceId();
		ServiceParameters params = rootService_->getServiceById(serviceId, deviceUrn);

		if(! params.controlUrl.isNull())
		{
			qDebug() << "UPnP::IgdControlPoint: wan/ipconnection service found, "
					<< "querying service '" << params.serviceId << "' for external ip address..." << endl;

			// Call the service
			wanConnectionService_ = new WanConnectionService(params);
			connect(wanConnectionService_, SIGNAL(queryFinished(bool)), this, SLOT(slotWanQueryFinished(bool)));
			wanConnectionService_->queryExternalIpAddress();
		}
	}

	// No longer need the forwarding service
	forwardingService_->deleteLater();
	forwardingService_ = 0;
}



// A WAN connection query was finished
void IgdControlPoint::slotWanQueryFinished(bool error)
{
	if(! error)
	{
		qDebug() << "IgdControlPoint: UPnP Gateway Device found." << endl;
		gatewayAvailable_ = true;
	} else {
		// Just started, the request for the external IP failed. This should succeed, abort portation
		qDebug() << "Requesting external IP address failed, leaving UPnP Gateway Device untouched." << endl;
	}
}



}  // End of namespace
