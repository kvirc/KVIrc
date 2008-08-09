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
                          ssdpconnection.cpp -  description
                             -------------------
    begin                : Fri Jul 29 2005
    copyright            : (C) 2005 by Diederik van der Boor
    email                : vdboor --at-- codingdomain.com
 ***************************************************************************/

#include "ssdpconnection.h"

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
	socket_ = new QUdpSocket();
	connect(socket_, SIGNAL(readyRead()), this, SLOT(slotDataReceived()));
}

SsdpConnection::~SsdpConnection()
{
	if(socket_ != 0)
	{
		socket_->close();
		delete socket_;
	}
}



// Data was received by the socket
void SsdpConnection::slotDataReceived()
{
	qDebug() << "UPnP::SsdpConnection: received " << socket_->bytesAvailable() << " bytes." << endl;

	// Get the HTTP-like content
	// TODO: How to handle multiple packets arriving from different devices?
	QByteArray response = socket_->readAll();

	// Response from my Acatel router:
	//
	//   HTTP/1.1 200 OK
	//   CACHE-CONTROL:max-age=1800
	//   EXT:
	//   LOCATION:http://10.0.0.138:80/IGD.xml
	//   SERVER:SpeedTouch 510 4.0.2.0.0 UPnP/1.0 (0313QZ6S2)
	//   ST:upnp:rootdevice
	//   USN:uuid:UPnP-SpeedTouch510-1_00-90-D0-8E-A1-6F::upnp:rootdevice
	//

	QString sspdResponse = QString::fromUtf8(response.data(), response.size());

	// Find the location field manually, MimeMessage is not required
	int locationStart = sspdResponse.find("LOCATION:",0,false);   // case insensitive
	int locationEnd   = sspdResponse.find("\r\n",locationStart);

	locationStart    += 9;  // length of field name
	QString location  = sspdResponse.mid(locationStart, locationEnd - locationStart);

	// Parse the URL syntax using KURL
	QUrl url(location);

	// Emit success
	emit deviceFound(url.host(), url.port(), url.path());
}


// Send a broadcast to detect all devices
void SsdpConnection::queryDevices(int bindPort)
{
	qDebug() << "UPnP::SsdpConnection: Sending broadcast packet." << endl;

	// Send a packet to a broadcast address
	QHostAddress address("239.255.255.250");

	QString data = "M-SEARCH * HTTP/1.1\r\n"
			"Host:239.255.255.250:1900\r\n"
			"ST:urn:schemas-upnp-org:device:InternetGatewayDevice:1\r\n"
			"Man:\"ssdp:discover\"\r\n"
			"MX:3\r\n"
			"\r\n";

	// Bind the socket to a certain port
	bool success = socket_->bind(address, bindPort);
	if(! success)
	{
		qDebug() << "UPnP::SsdpConnection: Failed to bind to port " << bindPort << "." << endl;
	}

	// Send the data
	QByteArray dataBlock = data.utf8();
	int bytesWritten = socket_->writeDatagram(dataBlock.data(), dataBlock.size(), address, 1900);

	if(bytesWritten == -1)
	{
		qDebug() << "UPnP::SsdpConnection: Failed to send the UPnP broadcast packet." << endl;
	}
}



}  // end of namespace
