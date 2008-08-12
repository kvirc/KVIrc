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
                          rootservice.cpp -  description
                             -------------------
    begin                : Mon Jul 25 2005
    copyright            : (C) 2005 by Diederik van der Boor
    email                : vdboor --at-- codingdomain.com
 ***************************************************************************/

#include "rootservice.h"
#include "xmlfunctions.h"

#include <QDebug>

namespace UPnP
{

  // Example message result:
  //
  //   <root xmlns="urn:schemas-upnp-org:device-1-0" >
  //     <specVersion>
  //       <major>1</major>
  //       <minor>0</minor>
  //     </specVersion>
  //     <URLBase>http://10.0.0.138</URLBase>
  //     <device>
  //       <deviceType>urn:schemas-upnp-org:device:InternetGatewayDevice:1</deviceType>
  //       <friendlyName>SpeedTouch 510 (0313QZ6S2)</friendlyName>
  //       <manufacturer>THOMSON multimedia</manufacturer>
  //       <manufacturerURL>http://www.thomson-multimedia.com</manufacturerURL>
  //       <modelDescription>DSL Internet Gateway</modelDescription>
  //       <modelName>SpeedTouch</modelName>
  //       <modelNumber>510</modelNumber>
  //       <modelURL>http://www.speedtouch.com</modelURL>
  //       <serialNumber>0313QZ6S2</serialNumber>
  //       <UDN>uuid:UPnP-SpeedTouch510-1_00-90-D0-8E-A1-6F</UDN>
  //       <presentationURL>/index.htm</presentationURL>
  //       <serviceList>
  //         <service>
  //           <serviceType>urn:schemas-upnp-org:service:Layer3Forwarding:1</serviceType>
  //           <serviceId>urn:upnp-org:serviceId:layer3f</serviceId>
  //           <controlURL>/upnp/control/layer3f</controlURL>
  //           <eventSubURL>/upnp/event/layer3f</eventSubURL>
  //           <SCPDURL>/Layer3Forwarding.xml</SCPDURL>
  //         </service>
  //       </serviceList>
  //       <deviceList>
  //         <device>
  //           <deviceType>urn:schemas-upnp-org:device:LANDevice:1</deviceType>
  //           <friendlyName>LANDevice</friendlyName>
  //           <manufacturer>THOMSON multimedia</manufacturer>
  //           <modelName>SpeedTouch</modelName>
  //           <serialNumber>0313QZ6S2</serialNumber>
  //           <UDN>uuid:UPnP-SpeedTouch510-1_00-90-D0-8E-A1-6F_LD</UDN>
  //           <serviceList>
  //             <service>
  //               <serviceType>urn:schemas-upnp-org:service:LANHostConfigManagement:1</serviceType>
  //               <serviceId>urn:upnp-org:serviceId:lanhcm</serviceId>
  //               <controlURL>/upnp/control/lanhcm</controlURL>
  //               <eventSubURL>/upnp/event/lanhcm</eventSubURL>
  //               <SCPDURL>/LANHostConfigManagement.xml</SCPDURL>
  //             </service>
  //           </serviceList>
  //         </device>
  //         <device>
  //           <deviceType>urn:schemas-upnp-org:device:WANDevice:1</deviceType>
  //           <friendlyName>WANDevice</friendlyName>
  //           <manufacturer>THOMSON multimedia</manufacturer>
  //           <modelName>SpeedTouch</modelName>
  //           <serialNumber>0313QZ6S2</serialNumber>
  //           <UDN>uuid:UPnP-SpeedTouch510-1_00-90-D0-8E-A1-6F_WD</UDN>
  //           <serviceList>
  //             <service>
  //               <serviceType>urn:schemas-upnp-org:service:WANCommonInterfaceConfig:1</serviceType>
  //               <serviceId>urn:upnp-org:serviceId:wancic</serviceId>
  //               <controlURL>/upnp/control/wancic</controlURL>
  //               <eventSubURL>/upnp/event/wancic</eventSubURL>
  //               <SCPDURL>/WANCommonInterfaceConfig.xml</SCPDURL>
  //             </service>
  //           </serviceList>
  //           <deviceList>
  //             <device>
  //               <deviceType>urn:schemas-upnp-org:device:WANConnectionDevice:1</deviceType>
  //               <friendlyName>WANConnectionDevice</friendlyName>
  //               <manufacturer>THOMSON multimedia</manufacturer>
  //               <modelName>SpeedTouch</modelName>
  //               <serialNumber>0313QZ6S2</serialNumber>
  //               <UDN>uuid:UPnP-SpeedTouch510-1_00-90-D0-8E-A1-6F_WCDpppoa</UDN>
  //               <serviceList>
  //                 <service>
  //                   <serviceType>urn:schemas-upnp-org:service:WANDSLLinkConfig:1</serviceType>
  //                   <serviceId>urn:upnp-org:serviceId:wandsllc:pppoa</serviceId>
  //                   <controlURL>/upnp/control/wandsllcpppoa</controlURL>
  //                   <eventSubURL>/upnp/event/wandsllcpppoa</eventSubURL>
  //                   <SCPDURL>/WANDSLLinkConfig.xml</SCPDURL>
  //                 </service>
  //                 <service>
  //                   <serviceType>urn:schemas-upnp-org:service:WANPPPConnection:1</serviceType>
  //                   <serviceId>urn:upnp-org:serviceId:wanpppc:pppoa</serviceId>
  //                   <controlURL>/upnp/control/wanpppcpppoa</controlURL>
  //                   <eventSubURL>/upnp/event/wanpppcpppoa</eventSubURL>
  //                   <SCPDURL>/WANPPPConnection.xml</SCPDURL>
  //                 </service>
  //               </serviceList>
  //             </device>
  //           </deviceList>
  //         </device>
  //       </deviceList>
  //     </device>
  //   </root>


// The contructor
RootService::RootService(const QString &hostname, int port, const QString &rootUrl)
: Service(hostname, port, rootUrl)
, hostname_(hostname)
, port_(port)
{

}


// The destructor
RootService::~RootService()
{

}


// Recursively add all devices and embedded devices to the deviceServices_ map
void RootService::addDeviceServices(const QDomNode &device)
{
	qDebug() << "UPnP Discovered device " << XmlFunctions::getNodeValue(device, "/UDN") << endl;

	// Insert the given device node
	// The "key" is the device/UDN tag, the value is a list of device/serviceList/service nodes
	deviceServices_.insert(XmlFunctions::getNodeValue(device, "/UDN"),
				device.namedItem("serviceList").childNodes());

	// Find all embedded device nodes
	QDomNodeList embeddedDevices = device.namedItem("deviceList").childNodes();
	for(uint i = 0; i < embeddedDevices.count(); i++)
	{
		if(embeddedDevices.item(i).nodeName() != "device") continue;
		addDeviceServices(embeddedDevices.item(i));
	}
}



// Return the device type
QString RootService::getDeviceType() const
{
	return deviceType_;
}



// Return a service from the cached root device entry
ServiceParameters RootService::getServiceById(const QString &serviceId) const
{
	// Get a /root/device/serviceList/service/ tag
	return getServiceById(serviceId, rootUdn_);
}



// Return a service from a cached embedded device entry
ServiceParameters RootService::getServiceById(const QString &serviceId, const QString &deviceUdn) const
{
	// Get a /root/device/deviceList/device/.../serviceList/service/serviceId tag
	QDomNode service = XmlFunctions::getNodeChildByKey( deviceServices_[deviceUdn], "serviceId", serviceId );

	// Initialize a ServiceParameters struct
	ServiceParameters params;

	if(! service.isNull())
	{
		params.hostname   = hostname_;
		params.port       = port_;
		params.controlUrl = XmlFunctions::getNodeValue(service, "/controlURL");
		params.scpdUrl    = XmlFunctions::getNodeValue(service, "/SCPDURL");
		params.serviceId  = XmlFunctions::getNodeValue(service, "/serviceId");
		params.serviceType = XmlFunctions::getNodeValue(service, "/serviceType");
	} else {
	qWarning() << "UPnP::RootService::getServiceById -"
		<< " id '" << serviceId << "' not found for device '" << deviceUdn << "'." << endl;
	}

	return params;
}



// Return a service from the cached root device entry
ServiceParameters RootService::getServiceByType(const QString &serviceType) const
{
	// Get a /root/device/serviceList/service/ tag
	return getServiceByType(serviceType, rootUdn_);
}



// Return a service from a cached embedded device entry
ServiceParameters RootService::getServiceByType(const QString &serviceType, const QString &deviceUdn) const
{
	// Get a /root/device/deviceList/device/.../serviceList/service/serviceType tag
	QDomNode service = XmlFunctions::getNodeChildByKey( deviceServices_[deviceUdn], "serviceType", serviceType );

	// Initialize a ServiceParameters struct
	ServiceParameters params;

	if(! service.isNull())
	{
		params.hostname   = hostname_;
		params.port       = port_;
		params.controlUrl = XmlFunctions::getNodeValue(service, "/controlURL");
		params.scpdUrl    = XmlFunctions::getNodeValue(service, "/SCPDURL");
		params.serviceId  = XmlFunctions::getNodeValue(service, "/serviceId");
		params.serviceType = serviceType;
	} else {
	qWarning() << "UPnP::RootService::getServiceByType -"
		<< " type '" << serviceType << "' not found for device '" << deviceUdn << "'." << endl;
	}

	return params;
}



// The control point received a response to callInformationUrl()
void RootService::gotInformationResponse(const QDomNode &response)
{
	// Register all device UDN nodes for later
	deviceServices_.clear();
	addDeviceServices( XmlFunctions::getNode(response, "/device") );

	// Fetch the required data
	deviceType_ = XmlFunctions::getNodeValue(response, "/device/deviceType");
	rootUdn_    = XmlFunctions::getNodeValue(response, "/device/UDN");

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
