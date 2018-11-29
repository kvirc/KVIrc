//=============================================================================
//
//   File : Manager.h
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
                          Manager.h -  description
                             -------------------
    begin                : Fri Aug 05 2005
    copyright            : (C) 2005 by Diederik van der Boor
    email                : vdboor --at-- codingdomain.com
 ***************************************************************************/

#ifndef UPNPMANAGER_H
#define UPNPMANAGER_H

#include <QObject>
#include <QTimer>

#include "KviPointerList.h"
#include "igdcontrolpoint.h"
#include "SsdpConnection.h"

namespace UPnP
{

	/**
 * The manager class is the public interface used by other networking classes.
 * It's implemented as singleton to provide easy access by other classes.
 * Devices are automatically detected at start-up, and maintained by this class.
 * Underneath, the actual work is done by the SsdpConnection and IgdControlPoint classes.
 *
 * @author Diederik van der Boor
 * @ingroup NetworkUPnP
 */
	class Manager : public QObject
	{
		Q_OBJECT

	public: // public methods
		// The destructor
		~Manager();

		// Return the external IP address
		QString getExternalIpAddress() const;

		// Return the instance of the manager class
		static Manager * instance();

		// Return true if a controlable gateway is available
		bool isGatewayAvailable();

		// Add a port mapping
		void addPortMapping(const QString & protocol, const QString & remoteHost, int externalPort, const QString & internalClient, int internalPort, const QString & description, bool enabled = true, int leaseDuration = 0);
		// Delete a port mapping
		void deletePortMapping(const QString & protocol, const QString & remoteHost, int externalPort);

	private slots:
		// The broadcast failed
		void slotBroadcastTimeout();
		// A device was discovered by the SSDP broadcast
		void slotDeviceFound(const QString & hostname, int port, const QString & rootUrl);

	private: // private methods
		     // The constructor  (it's a singleton)
		Manager();
		// Disable the copy constructor
		Manager(const Manager &);
		// Disable the assign operator
		Manager & operator=(const Manager &);
		// Initialize the manager, detect all devices
		void initialize();

	private:
		// The active control point we're working with
		IgdControlPoint * m_pActiveIgdControlPoint;
		// True if the broadcast failed (false during the discovery process)
		bool m_bBroadcastFailed;
		// True if the broadcast found a device (false during the discovery process)
		bool m_bBroadcastFoundIt;
		// The instance of the singleton class
		static Manager * m_pInstance;
		// A list of all detected gateway devices
		KviPointerList<IgdControlPoint> m_lIgdControlPoints;
		// The SSDP connection to find all UPnP devices
		SsdpConnection * m_pSsdpConnection;
		// The timer to detect a broadcast timeout
		QTimer * m_pSsdpTimer;
	};

} // End of namespace

#endif
