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
//#define Layer3ForwardingType      "urn:schemas-upnp-org:service:Layer3Forwarding:1"

#define WanIpConnectionType		"urn:schemas-upnp-org:service:WANIPConnection:1"
#define WanPPPConnectionType		"urn:schemas-upnp-org:service:WANPPPConnection:1"

// The constructor
IgdControlPoint::IgdControlPoint(const QString &hostname, int port, const QString &rootUrl)
: QObject()
, m_pForwardingService(0)
, m_bGatewayAvailable(false)
, m_iIgdPort(0)
, m_pRootService(0)
, m_pWanConnectionService(0)
{
	qDebug() << "CREATED UPnP::IgdControlPoint: Created control point"
		<< " url='" << hostname << ":" << port << "/" << rootUrl << "'." << endl;

	qDebug() << "UPnP::IgdControlPoint: querying services..." << endl;

	// Store device url
	m_szIgdHostname = hostname;
	m_iIgdPort     = port;

	// Query the device for it's services
	m_pRootService = new RootService(m_szIgdHostname, m_iIgdPort, rootUrl);
	connect(m_pRootService, SIGNAL(queryFinished(bool)), this, SLOT(slotDeviceQueried(bool)));
}



// The destructor
IgdControlPoint::~IgdControlPoint()
{
	delete m_pRootService;
	delete m_pForwardingService;
	delete m_pWanConnectionService;

	qDebug() << "DESTROYED UPnP::IgdControlPoint [host=" << m_szIgdHostname << ", port=" << m_iIgdPort << "]" << endl;
}



// Return the external IP address
QString IgdControlPoint::getExternalIpAddress() const
{
	// Do not expose  wanConnectionService_;
	if(m_pWanConnectionService != 0)
	{
		return m_pWanConnectionService->getExternalIpAddress();
	} else {
		return QString::null;
	}
}


// Initialize the control point
void IgdControlPoint::initialize()
{
	m_pRootService->queryDevice();
}



// Return true if a controlable gateway is available
bool IgdControlPoint::isGatewayAvailable()
{
	return m_bGatewayAvailable;
}



// The IGD was queried for it's services
void IgdControlPoint::slotDeviceQueried(bool error)
{
	if(! error)
	{
		ServiceParameters params = m_pRootService->getServiceByType(WanIpConnectionType);

		if(params.controlUrl.isNull())
			params = m_pRootService->getServiceByType(WanPPPConnectionType);

		if(! params.controlUrl.isNull())
		{
			qDebug() << "UPnP::IgdControlPoint: wan/ipconnection service found, "
					<< "querying service '" << params.serviceId << "' for external ip address..." << endl;

			// Call the service
			m_pWanConnectionService = new WanConnectionService(params);
			connect(m_pWanConnectionService, SIGNAL(queryFinished(bool)), this, SLOT(slotWanQueryFinished(bool)));
			m_pWanConnectionService->queryExternalIpAddress();
		} else {
			qDebug() << "UPnP::IgdControlPoint: no ppp/ipconnection service found :(" << endl;
		}
	}
}


/*
// A WAN connection service was found
void IgdControlPoint::slotWanConnectionFound(bool error)
{
	if(! error)
	{
		// Get the retreived service description
		QString deviceUrn = m_pForwardingService->getConnectionDeviceUdn();
		QString serviceId = m_pForwardingService->getConnectionServiceId();
		ServiceParameters params = m_pRootService->getServiceById(serviceId, deviceUrn);

		if(! params.controlUrl.isNull())
		{
			qDebug() << "UPnP::IgdControlPoint: wan/ipconnection service found, "
					<< "querying service '" << params.serviceId << "' for external ip address..." << endl;

			// Call the service
			m_pWanConnectionService = new WanConnectionService(params);
			connect(m_pWanConnectionService, SIGNAL(queryFinished(bool)), this, SLOT(slotWanQueryFinished(bool)));
			m_pWanConnectionService->queryExternalIpAddress();
		}
	}

	// No longer need the forwarding service
	m_pForwardingService->deleteLater();
	m_pForwardingService = 0;
}
*/


// A WAN connection query was finished
void IgdControlPoint::slotWanQueryFinished(bool error)
{
	if(! error)
	{
		qDebug() << "IgdControlPoint: UPnP Gateway Device found." << endl;
		m_bGatewayAvailable = true;
	} else {
		// Just started, the request for the external IP failed. This should succeed, abort portation
		qDebug() << "Requesting external IP address failed, leaving UPnP Gateway Device untouched." << endl;
	}
}



}  // End of namespace
