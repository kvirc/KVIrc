//=============================================================================
//
//   File : igdcontrolpoint.cpp
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

#include <QDebug>

#include "igdcontrolpoint.h"

#include "RootService.h"
#include "WanConnectionService.h"

namespace UPnP
{

#define WanIpConnectionType "urn:schemas-upnp-org:service:WANIPConnection:1"
#define WanPPPConnectionType "urn:schemas-upnp-org:service:WANPPPConnection:1"

	// The constructor
	IgdControlPoint::IgdControlPoint(const QString & hostname, int port, const QString & rootUrl)
	    : QObject(), m_bGatewayAvailable(false), m_iIgdPort(0), m_pRootService(nullptr), m_pWanConnectionService(nullptr)
	{
		qDebug() << "CREATED UPnP::IgdControlPoint: created control point"
		         << " url='" << hostname << ":" << port << "/" << rootUrl << "'." << endl;

		qDebug() << "UPnP::IgdControlPoint: querying services..." << endl;

		// Store device url
		m_szIgdHostname = hostname;
		m_iIgdPort = port;

		// Query the device for it's services
		m_pRootService = new RootService(m_szIgdHostname, m_iIgdPort, rootUrl);
		connect(m_pRootService, SIGNAL(queryFinished(bool)), this, SLOT(slotDeviceQueried(bool)));
	}

	// The destructor
	IgdControlPoint::~IgdControlPoint()
	{
		delete m_pRootService;
		delete m_pWanConnectionService;

		qDebug() << "DESTROYED UPnP::IgdControlPoint [host=" << m_szIgdHostname << ", port=" << m_iIgdPort << "]" << endl;
	}

	// Return the external IP address
	QString IgdControlPoint::getExternalIpAddress() const
	{
		// Do not expose  wanConnectionService_;
		if(m_pWanConnectionService != nullptr)
		{
			return m_pWanConnectionService->getExternalIpAddress();
		}
		else
		{
			return QString();
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
		if(!error)
		{
			ServiceParameters params = m_pRootService->getServiceByType(WanIpConnectionType);

			if(params.controlUrl.isNull())
				params = m_pRootService->getServiceByType(WanPPPConnectionType);

			if(!params.controlUrl.isNull())
			{

				m_bGatewayAvailable = true;

				qDebug() << "UPnP::IgdControlPoint: WAN/IP connection service found, "
				         << "querying service '" << params.serviceId << "' for external IP address..." << endl;

				// Call the service
				m_pWanConnectionService = new WanConnectionService(params);
				connect(m_pWanConnectionService, SIGNAL(queryFinished(bool)), this, SLOT(slotWanQueryFinished(bool)));
				m_pWanConnectionService->queryExternalIpAddress();
			}
			else
			{
				qDebug() << "UPnP::IgdControlPoint: no PPP/IP connection service found :(" << endl;
			}
		}
	}

	// A WAN connection query was finished
	void IgdControlPoint::slotWanQueryFinished(bool error)
	{
		if(!error)
		{
			qDebug() << "IgdControlPoint: UPnP gateway device found." << endl;
		}
		else
		{
			// Just started, the request for the external IP failed. This should succeed, abort portation
			qDebug() << "Requesting external IP address failed, leaving UPnP gateway device untouched." << endl;
		}
	}

	// Return the external IP address
	void IgdControlPoint::addPortMapping(const QString & protocol, const QString & remoteHost, int externalPort, const QString & internalClient, int internalPort, const QString & description, bool enabled, int leaseDuration)
	{
		// Do not expose  wanConnectionService_;
		if(m_pWanConnectionService != nullptr)
		{
			m_pWanConnectionService->addPortMapping(protocol, remoteHost, externalPort, internalClient, internalPort, description, enabled, leaseDuration);
		}
	}

	// Delete a port mapping
	void IgdControlPoint::deletePortMapping(const QString & protocol, const QString & remoteHost, int externalPort)
	{
		// Do not expose  wanConnectionService_;
		if(m_pWanConnectionService != nullptr)
		{
			m_pWanConnectionService->deletePortMapping(protocol, remoteHost, externalPort);
		}
	}

} // End of namespace
