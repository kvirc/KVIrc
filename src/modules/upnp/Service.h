//=============================================================================
//
//   File : Service.h
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
                          Service.h  -  description
                             -------------------
    begin                : Sun Jul 24 2005
    copyright            : (C) 2005 by Diederik van der Boor
    email                : vdboor --at-- codingdomain.com
 ***************************************************************************/

#ifndef UPNP_CONTROLPOINT_H
#define UPNP_CONTROLPOINT_H

#include <QObject>
#include <QDomNode>
#include <QMap>
#include <QString>

namespace UPnP
{

	// A datablock to make the exchange
	// of service information easier
	struct ServiceParameters
	{
		QString hostname;
		int port;
		QString scpdUrl;
		QString controlUrl;
		QString serviceId;
		QString serviceType;
	};

	/**
 * This base class to implement calls to a UPnP-enabled device.
 * In UPnP terminology, a client is called a Control Point, and a service is a UPnP device.
 *
 * This class different kind of calls.
 * An information request queries the service for data with a HTTP GET.
 * An action request issues a HTTP POST call to the given service.
 *
 * @author Diederik van der Boor
 * @ingroup NetworkUPnP
 */
	class Service : public QObject
	{
		Q_OBJECT

	public: // public methods
		// The constructor for the root service
		Service(QString hostname, int port, const QString & informationUrl);
		// The constructor for action services
		Service(const ServiceParameters & params);
		// The destructor
		~Service();

		// Get the number of pending requests
		int getPendingRequests() const;

	protected: // Protected methods
		// Makes a UPnP action request
		int callAction(const QString & actionName, const QString & prefix);
		// Makes a UPnP action request
		int callAction(const QString & actionName, const QMap<QString, QString> & arguments, const QString & prefix);
		// Makes a UPnP service request
		int callInformationUrl();

		// The control point received an action failure indication
		virtual void gotActionErrorResponse(const QDomNode & response);
		// The control point received a response to callAction()
		virtual void gotActionResponse(const QString & responseType, const QMap<QString, QString> & resultValues);
		// The control point received a response to callInformationUrl()
		virtual void gotInformationResponse(const QDomNode & response);

	private slots:
		// The QHttp object retrieved data.
		void slotRequestFinished();

	private:
		// Makes a UPnP action request (keeps pointers from the external interface)
		int callActionInternal(const QString & actionName, const QMap<QString, QString> * arguments, const QString & prefix);

	private:
		// The URL to control the service
		QString m_szControlUrl;
		// The URL to request service information
		QString m_szInformationUrl;
		// The number of pending queries/actions
		int m_iPendingRequests;
		// The ID of the service
		QString m_szServiceId;
		// The Type of the service
		QString m_szServiceType;
		// The xml prefix used by the router
		QString m_szBaseXmlPrefix;
		// hostname
		QString m_szHostname;
		//port
		int m_iPort;

	signals:
		// Called when a query completed
		void queryFinished(bool error);
	};
}

#endif
