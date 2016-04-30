//=============================================================================
//
//   File : SsdpConnection.cpp
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
                          SsdpConnection.cpp -  description
                             -------------------
    begin                : Fri Jul 29 2005
    copyright            : (C) 2005 by Diederik van der Boor
    email                : vdboor --at-- codingdomain.com
 ***************************************************************************/

#include "SsdpConnection.h"

#include <QHostAddress>
#include <QUdpSocket>

#include <QByteArray>
#include <QDebug>
#include <QUrl>

namespace UPnP
{

	SsdpConnection::SsdpConnection()
	    : QObject()
	{
		m_pSocket = new QUdpSocket();
		connect(m_pSocket, SIGNAL(readyRead()), this, SLOT(slotDataReceived()));
	}

	SsdpConnection::~SsdpConnection()
	{
		if(m_pSocket != nullptr)
		{
			m_pSocket->close();
			delete m_pSocket;
		}
	}

	// Data was received by the socket
	void SsdpConnection::slotDataReceived()
	{
		qDebug() << "UPnP::SsdpConnection: received " << m_pSocket->bytesAvailable() << " bytes." << endl;

		// Response from Diederik's Acatel router:
		//
		//   HTTP/1.1 200 OK
		//   CACHE-CONTROL:max-age=1800
		//   EXT:
		//   LOCATION:http://10.0.0.138:80/IGD.xml
		//   SERVER:SpeedTouch 510 4.0.2.0.0 UPnP/1.0 (0313QZ6S2)
		//   ST:upnp:rootdevice
		//   USN:uuid:UPnP-SpeedTouch510-1_00-90-D0-8E-A1-6F::upnp:rootdevice
		//

		//  This is from a Zyxel router:
		//
		//   HTTP/1.1 200 OK
		//   ST:urn:schemas-upnp-org:device:InternetGatewayDevice:1
		//   USN:uuid:11111111-%04x-c0a8-fcb30002cf618c::urn:schemas-upnp-org:device:InternetGatewayDevice:1
		//   Location:http://192.168.252.179:80/DeviceDescription.xml
		//   Cache-Control: max-age=480
		//   Server: ZyXEL-RomPlug/4.51 UPnP/1.0 IGD/1.00
		//   Ext:

		while(m_pSocket->hasPendingDatagrams())
		{
			QByteArray datagram;
			datagram.resize(m_pSocket->pendingDatagramSize());
			m_pSocket->readDatagram(datagram.data(), datagram.size());

			qDebug("Received datagram: %s\n", datagram.data());

			QString sspdResponse = QString::fromUtf8(datagram.data(), datagram.size());

			// Find the location field manually, MimeMessage is not required
			int locationStart = sspdResponse.indexOf("LOCATION:", 0, Qt::CaseInsensitive);
			int locationEnd = sspdResponse.indexOf("\r\n", locationStart, Qt::CaseSensitive);

			locationStart += 9; // length of field name
			QString location = sspdResponse.mid(locationStart, locationEnd - locationStart);

			// Parse the URL syntax using KURL
			QUrl url(location.trimmed());

			qDebug("Found internet gateway: %s\n", location.toUtf8().data());
			// Emit success
			emit deviceFound(url.host(), url.port(), url.path());
		}
	}

	// Send a broadcast to detect all devices
	void SsdpConnection::queryDevices(int bindPort)
	{
		qDebug() << "UPnP::SsdpConnection: sending broadcast packet." << endl;

		// Send a packet to a broadcast address
		QHostAddress address("239.255.255.250");

		QString data = "M-SEARCH * HTTP/1.1\r\n"
		               "Host:239.255.255.250:1900\r\n"
		               "ST:urn:schemas-upnp-org:device:InternetGatewayDevice:1\r\n"
		               "Man:\"ssdp:discover\"\r\n"
		               "MX:3\r\n"
		               "\r\n";

		// Bind the socket to a certain port
		bool success = m_pSocket->bind(bindPort);
		if(!success)
		{
			qDebug() << "UPnP::SsdpConnection: failed to bind to port " << bindPort << "." << endl;
		}

		// Send the data
		QByteArray dataBlock = data.toUtf8();
		int bytesWritten = m_pSocket->writeDatagram(dataBlock.data(), dataBlock.size(), address, 1900);

		if(bytesWritten == -1)
		{
			qDebug() << "UPnP::SsdpConnection: failed to send the UPnP broadcast packet." << endl;
		}
	}

} // end of namespace
