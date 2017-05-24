//=============================================================================
//
//   File : RootService.cpp
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
                          RootService.cpp -  description
                             -------------------
    begin                : Mon Jul 25 2005
    copyright            : (C) 2005 by Diederik van der Boor
    email                : vdboor --at-- codingdomain.com
 ***************************************************************************/

#include "RootService.h"
#include "XmlFunctions.h"
#include "KviLocale.h"
#include "kvi_out.h"
#include "KviApplication.h"
#include "KviConsoleWindow.h"

#include <QDebug>

namespace UPnP
{
	// The constructor
	RootService::RootService(const QString & hostname, int port, const QString & rootUrl)
	    : Service(hostname, port, rootUrl), m_szHostname(hostname), m_iPort(port)
	{
	}

	// The destructor
	RootService::~RootService()
	    = default;

	// Recursively add all devices and embedded devices to the deviceServices_ map
	void RootService::addDeviceServices(const QDomNode & device)
	{
		qDebug() << "UPnP discovered device " << XmlFunctions::getNodeValue(device, "/UDN") << endl;

		if(XmlFunctions::getNodeValue(device, "/deviceType") == InternetGatewayDeviceType)
		{
			QString description;
			description = XmlFunctions::getNodeValue(device, "/friendlyName");
			if(description.isNull())
				description = XmlFunctions::getNodeValue(device, "/modelDescription");
			if(description.isNull())
				description = XmlFunctions::getNodeValue(device, "/modelName") + " " + XmlFunctions::getNodeValue(device, "/modelNumber");
			if(description.isNull())
				description = __tr2qs("Unknown");

			qDebug() << "Model: " << description << endl;

			g_pApp->activeConsole()->output(KVI_OUT_GENERICSTATUS, __tr2qs_ctx("[UPNP]: found gateway device: %s", "upnp"), description.toUtf8().data());
		}
		// Insert the given device node
		// The "key" is the device/UDN tag, the value is a list of device/serviceList/service nodes
		m_deviceServices.insert(XmlFunctions::getNodeValue(device, "/UDN"),
		    device.namedItem("serviceList").childNodes());

		// Find all embedded device nodes
		QDomNodeList embeddedDevices = device.namedItem("deviceList").childNodes();
		for(int i = 0; i < embeddedDevices.count(); i++)
		{
			if(embeddedDevices.item(i).nodeName() != "device")
				continue;
			addDeviceServices(embeddedDevices.item(i));
		}
	}

	// Return the device type
	QString RootService::getDeviceType() const
	{
		return m_szDeviceType;
	}

	// Return a service from the cached root device entry
	ServiceParameters RootService::getServiceById(const QString & serviceId) const
	{
		// Get a /root/device/serviceList/service/ tag

		ServiceParameters params;
		QMap<QString, QDomNodeList>::const_iterator i = m_deviceServices.constBegin();
		while(i != m_deviceServices.constEnd())
		{
			if(getServiceById(serviceId, i.key(), params))
				return params;
			++i;
		}

		//calling function check this field to understand if the struct is null
		params.controlUrl = QString();
		return params;
	}

	// Return a service from a cached embedded device entry
	bool RootService::getServiceById(const QString & serviceId, const QString & deviceUdn, ServiceParameters & params) const
	{
		// Get a /root/device/deviceList/device/.../serviceList/service/serviceId tag
		QDomNode service = XmlFunctions::getNodeChildByKey(m_deviceServices[deviceUdn], "serviceId", serviceId);

		if(!service.isNull())
		{
			params.hostname = m_szHostname;
			params.port = m_iPort;
			params.controlUrl = XmlFunctions::getNodeValue(service, "/controlURL");
			params.scpdUrl = XmlFunctions::getNodeValue(service, "/SCPDURL");
			params.serviceId = XmlFunctions::getNodeValue(service, "/serviceId");
			params.serviceType = XmlFunctions::getNodeValue(service, "/serviceType");

			return true;
		}
		else
		{
			return false;
		}
	}

	// Return a service from the cached root device entry
	ServiceParameters RootService::getServiceByType(const QString & serviceType) const
	{
		// Get a /root/device/serviceList/service/ tag

		ServiceParameters params;
		QMap<QString, QDomNodeList>::const_iterator i = m_deviceServices.constBegin();
		while(i != m_deviceServices.constEnd())
		{
			if(getServiceByType(serviceType, i.key(), params))
				return params;
			++i;
		}

		//calling function check this field to understand if the struct is null
		params.controlUrl = QString();
		return params;
	}

	// Return a service from a cached embedded device entry
	bool RootService::getServiceByType(const QString & serviceType, const QString & deviceUdn, ServiceParameters & params) const
	{
		// Get a /root/device/deviceList/device/.../serviceList/service/serviceType tag
		QDomNode service = XmlFunctions::getNodeChildByKey(m_deviceServices[deviceUdn], "serviceType", serviceType);

		if(!service.isNull())
		{
			params.hostname = m_szHostname;
			params.port = m_iPort;
			params.controlUrl = XmlFunctions::getNodeValue(service, "/controlURL");
			params.scpdUrl = XmlFunctions::getNodeValue(service, "/SCPDURL");
			params.serviceId = XmlFunctions::getNodeValue(service, "/serviceId");
			params.serviceType = serviceType;

			return true;
		}
		else
		{
			qWarning() << "UPnP::RootService::getServiceByType -"
			           << " type '" << serviceType << "' not found for device '" << deviceUdn << "'." << endl;

			return false;
		}
	}

	// The control point received a response to callInformationUrl()
	void RootService::gotInformationResponse(const QDomNode & response)
	{
		// Register all device UDN nodes for later
		m_deviceServices.clear();
		addDeviceServices(XmlFunctions::getNode(response, "/device"));

		// Fetch the required data
		m_szDeviceType = XmlFunctions::getNodeValue(response, "/device/deviceType");
		m_szRootUdn = XmlFunctions::getNodeValue(response, "/device/UDN");

		// The rootUdn_ is used to retrieve
		// the /root/device/serviceList/service
		// nodes from the deviceServices_ map
	}

	// Query the device for its service list
	void RootService::queryDevice()
	{
		callInformationUrl();
	}

} // end of namespae
