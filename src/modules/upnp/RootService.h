//=============================================================================
//
//   File : RootService.h
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
                          RootService.h -  description
                             -------------------
    begin                : Mon Jul 25 2005
    copyright            : (C) 2005 by Diederik van der Boor
    email                : vdboor --at-- codingdomain.com
 ***************************************************************************/

#ifndef UPNP_ROOTSERVICE_H
#define UPNP_ROOTSERVICE_H

#include "Service.h"

namespace UPnP
{

#define InternetGatewayDeviceType "urn:schemas-upnp-org:device:InternetGatewayDevice:1"

	/**
 * The services of a device can be retrieved using the device root service.
 * The URL of the root service is returned by an SSDP broadcast.
 * The root service returns the meta information and list of services the device supports.
 *
 * @author Diederik van der Boor
 * @ingroup NetworkUPnP
 */
	class RootService : public Service
	{
	public: // public methods
		// The constructor
		RootService(const QString & hostname, int port, const QString & rootUrl);
		// The destructor
		~RootService();

		// Return the device type
		QString getDeviceType() const;

		// Return a service from the cached root device entry
		ServiceParameters getServiceById(const QString & serviceId) const;
		// Return a service from a cached embedded device entry
		bool getServiceById(const QString & serviceId, const QString & deviceUdn, ServiceParameters & params) const;
		// Return a service from the cached root device entry
		ServiceParameters getServiceByType(const QString & serviceType) const;
		// Return a service from a cached embedded device entry
		bool getServiceByType(const QString & serviceType, const QString & deviceUdn, ServiceParameters & params) const;

		// Query the device for its service list
		void queryDevice();

	protected: // Protected methods
		// The control point received a response to callInformationUrl()
		void gotInformationResponse(const QDomNode & response) override;

	private: // Private methods
		// Recursively add all devices and embedded devices to the deviceServices_ map
		void addDeviceServices(const QDomNode & device);

	private:
		// The device type
		QString m_szDeviceType;
		// A collection of all services provided by the device
		QMap<QString, QDomNodeList> m_deviceServices;
		// The hostname of the device
		QString m_szHostname;
		// The port of the device
		int m_iPort;
		// The udn of the root device
		QString m_szRootUdn;
	};
}

#endif
