//=============================================================================
//
//   File : Manager.cpp
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
                          Manager.cpp -  description
                             -------------------
    begin                : Fri Aug 05 2005
    copyright            : (C) 2005 by Diederik van der Boor
    email                : vdboor --at-- codingdomain.com
 ***************************************************************************/

#include "Manager.h"

#include "igdcontrolpoint.h"
#include "SsdpConnection.h"

#include <QDebug>
#include <QTimer>

namespace UPnP
{

	// Set the static variable
	Manager * Manager::m_pInstance(nullptr);

	// The constructor
	Manager::Manager()
	    : m_pActiveIgdControlPoint(nullptr), m_bBroadcastFailed(false), m_bBroadcastFoundIt(false), m_pSsdpConnection(nullptr), m_pSsdpTimer(nullptr)
	{
	}

	// The destructor
	Manager::~Manager()
	{
		delete m_pSsdpTimer;
		delete m_pSsdpConnection;
		m_pInstance = nullptr; // Unregister the instance
	}

	// Initialize the manager, detect all devices
	void Manager::initialize()
	{
		qDebug() << "UPnP::Manager: initiating a broadcast to detect UPnP devices..." << endl;

		// Create the SSDP object to detect devices
		m_pSsdpConnection = new SsdpConnection();
		connect(m_pSsdpConnection, SIGNAL(deviceFound(const QString &, int, const QString &)),
		    this, SLOT(slotDeviceFound(const QString &, int, const QString &)));

		// Create a timer
		m_pSsdpTimer = new QTimer(this);
		connect(m_pSsdpTimer, SIGNAL(timeout()), this, SLOT(slotBroadcastTimeout()));

		// Start a UPnP broadcast
		m_bBroadcastFailed = false;
		m_bBroadcastFoundIt = false;
		m_pSsdpConnection->queryDevices();
		m_pSsdpTimer->setSingleShot(true);
		m_pSsdpTimer->start(2000);
	}

	// Return the instance of the manager class
	Manager * Manager::instance()
	{
		// Create when it's required
		if(m_pInstance == nullptr)
		{
			m_pInstance = new Manager();
			m_pInstance->initialize();
		}

		return m_pInstance;
	}

	// Return the external IP address
	QString Manager::getExternalIpAddress() const
	{
		// Do not expose activeIgd_;
		return (m_pActiveIgdControlPoint != nullptr ? m_pActiveIgdControlPoint->getExternalIpAddress() : QString());
	}

	// Return true if a controlable gateway is available
	bool Manager::isGatewayAvailable()
	{
		return (m_pActiveIgdControlPoint != nullptr && m_pActiveIgdControlPoint->isGatewayAvailable());
	}

	// Add a port mapping
	void Manager::addPortMapping(const QString & protocol, const QString & remoteHost, int externalPort, const QString & internalClient, int internalPort, const QString & description, bool enabled, int leaseDuration)
	{
		if(m_pActiveIgdControlPoint != nullptr)
			m_pActiveIgdControlPoint->addPortMapping(protocol, remoteHost, externalPort, internalClient, internalPort, description, enabled, leaseDuration);
	}

	// Delete a port mapping
	void Manager::deletePortMapping(const QString & protocol, const QString & remoteHost, int externalPort)
	{
		if(m_pActiveIgdControlPoint != nullptr)
			m_pActiveIgdControlPoint->deletePortMapping(protocol, remoteHost, externalPort);
	}

	// The broadcast failed
	void Manager::slotBroadcastTimeout()
	{
		if(!m_bBroadcastFoundIt)
		{
			qDebug() << "UPnP::Manager: timeout, no broadcast response received!" << endl;

			m_bBroadcastFailed = true;
		}
	}

	// A device was discovered by the SSDP broadcast
	void Manager::slotDeviceFound(const QString & hostname, int port, const QString & rootUrl)
	{
		qDebug() << "UPnP::Manager: device found, initializing IgdControlPoint to query it." << endl;

		// this blocks the action of our timeout timer
		m_bBroadcastFoundIt = true;

		IgdControlPoint * controlPoint = new IgdControlPoint(hostname, port, rootUrl);
		m_lIgdControlPoints.append(controlPoint);

		if(m_pActiveIgdControlPoint == nullptr)
		{
			m_pActiveIgdControlPoint = controlPoint;
			m_pActiveIgdControlPoint->initialize();
		}
	}

} // end of namespace
