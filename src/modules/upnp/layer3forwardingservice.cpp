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
                          layer3forwardingservice.cpp -  description
                             -------------------
    begin                : Mon Jul 25 2005
    copyright            : (C) 2005 by Diederik van der Boor
    email                : vdboor --at-- codingdomain.com
 ***************************************************************************/

#include <QDebug>
#include <QStringList>

#include "layer3forwardingservice.h"

namespace UPnP
{

// The constructor
Layer3ForwardingService::Layer3ForwardingService(const ServiceParameters &params)
  : Service(params)
{

}


// The destructor
Layer3ForwardingService::~Layer3ForwardingService()
{

}



// Get the device UDN of the default connection service
QString Layer3ForwardingService::getConnectionDeviceUdn() const
{
	return m_szConnectionDeviceUdn;
}



// Get the service ID of the default connection service
QString Layer3ForwardingService::getConnectionServiceId() const
{
	return m_szConnectionServiceId;
}



// The control point received a response to callAction()
void Layer3ForwardingService::gotActionResponse(const QString &responseType, const QMap<QString,QString> &resultValues)
{
	qDebug() << "UPnP::Layer3ForwardingService: Got action response"
		<< " type='" << responseType << "'." << endl;

	// Example:
	//
	//   <m:GetDefaultConnectionServiceResponse xmlns:m="urn:schemas-upnp-org:service:Layer3Forwarding:1" >
	//     <NewDefaultConnectionService>
	//       (there is no white space between these parts!)
	//       uuid:UPnP-SpeedTouch510-1_00-90-D0-8E-A1-6F_WCDpppoa:WANConnectionDevice:1,
	//       urn:upnp-org:serviceId:wanpppc:pppoa
	//     </NewDefaultConnectionService>
	//   </m:GetDefaultConnectionServiceResponse>


	if(responseType == "GetDefaultConnectionServiceResponse" )
	{
		QString     newService   = resultValues["NewDefaultConnectionService"];
		QStringList serviceItems = QStringList::split(',', newService);
		QString     uuid;
		QString     urn;

		// Extract the uuid and urn from the NewDefaultConnectionService value
		for(uint i = 0; i < serviceItems.count(); i++)
		{
			QString type = serviceItems[i].section(':', 0, 0);
			if(type == "uuid")
			{
				// format: uuid:<id>:<class>:<ver>
				m_szConnectionDeviceUdn = serviceItems[i].section(':', 0, 1);
			} else if(type == "urn") {
				m_szConnectionServiceId = serviceItems[i];
			} else {
				qDebug() << "UPnP::Layer3ForwardingService - Unexpected section"
					<< " '" << type << "' encountered in NewDefaultConnectionService value." << endl;
			}
		}

		qDebug() << "UPnP::Layer3ForwardingService:"
			<< " udn='"       << m_szConnectionDeviceUdn << "'"
			<< " serviceid='" << m_szConnectionServiceId << "'." << endl;
	} else {
		qDebug() << "UPnP::Layer3ForwardingService - Unexpected response type"
			<< " '" << responseType << "' encountered." << endl;
	}
}



// Query the Layer3Forwarding service for the default connection service
void Layer3ForwardingService::queryDefaultConnectionService()
{
	callAction("GetDefaultConnectionService");
}


}
