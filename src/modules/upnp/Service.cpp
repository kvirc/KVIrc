//=============================================================================
//
//   File : Service.cpp
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
                          Service.cpp  -  description
                             -------------------
    begin                : Sun Jul 24 2005
    copyright            : (C) 2005 by Diederik van der Boor
    email                : vdboor --at-- codingdomain.com
 ***************************************************************************/

#include "Service.h"
#include "XmlFunctions.h"

#include <QDebug>
#include <QByteArray>
#include <utility>

#include "KviNetworkAccessManager.h"

//   This implementation was created with the help of the following documentation:
//   http://www.upnp.org/standardizeddcps/documents/UPnP_IGD_1.0.zip
//   http://zacbowling.com/upnp/
//   http://www.knoxscape.com/Upnp/NAT.htm
//   http://www.artima.com/spontaneous/upnp_digihome2.html

namespace UPnP
{

	// The constructor for information services
	Service::Service(QString hostname, int port, const QString & informationUrl)
	    : m_iPendingRequests(0)
	    , m_szBaseXmlPrefix("s")
	    , m_szHostname(std::move(hostname))
	    , m_iPort(port)
	{
		m_szInformationUrl = informationUrl;
		qDebug() << "UPnP::Service: created information service url='" << m_szInformationUrl << "'." << endl;
	}

	// The constructor for action services
	Service::Service(const ServiceParameters & params)
	    : m_szControlUrl(params.controlUrl), m_szInformationUrl(params.scpdUrl), m_iPendingRequests(0), m_szServiceId(params.serviceId), m_szServiceType(params.serviceType), m_szBaseXmlPrefix("s"), m_szHostname(params.hostname), m_iPort(params.port)
	{
		qDebug() << "CREATED UPnP::Service: url='" << m_szControlUrl << "' id='" << m_szServiceId << "'." << endl;
	}

	// The destructor
	Service::~Service()
	{
		qDebug() << "DESTROYED UPnP::Service [url=" << m_szControlUrl << ",  id=" << m_szServiceId << "]" << endl;
	}

	// Makes a UPnP action request
	// TODO: rename to callMethod / callSoapMethod
	int Service::callAction(const QString & actionName, const QString & prefix)
	{
		return callActionInternal(actionName, nullptr, prefix);
	}

	// Makes a UPnP action request
	int Service::callAction(const QString & actionName, const QMap<QString, QString> & arguments, const QString & prefix)
	{
		return callActionInternal(actionName, &arguments, prefix);
	}

	// Makes a UPnP action request (keeps pointers from the external interface)
	int Service::callActionInternal(const QString & actionName, const QMap<QString, QString> * arguments, const QString & prefix)
	{
		qDebug() << "UPnP::Service: calling remote procedure '" << actionName << "'." << endl;

		// Create the data message
		//NOTE: we shouldm use serviceId_ instead of serviceType_, but it seems that my router
		// (and maybe others) are reporting wrong Ids, while they're gonna accepting only the
		// correct ones. This is a decoded reply from my Zyxel:

		//  Service :
		//    servicetype = urn:schemas-upnp-org:service:Layer3Forwarding:1
		//    controlurl = /UD/act?0
		//    eventsuburl = /?0
		//    scpdurl = /L3Fwd.xml
		//    serviceid = urn:upnp-org:serviceId:L3Forwarding1

		// this router wants us to use servicetype in the following requests

		QString soapMessage = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n"
		                      "<"
		    + m_szBaseXmlPrefix + ":Envelope xmlns:" + m_szBaseXmlPrefix + "=\"http://schemas.xmlsoap.org/soap/envelope/\""
		                                                                   " "
		    + m_szBaseXmlPrefix + ":encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">\n"
		                          " <"
		    + m_szBaseXmlPrefix + ":Body>\n"
		                          "  <"
		    + prefix + ":" + actionName + " xmlns:" + prefix + "=\"" + m_szServiceType + "\">\n";

		// Do we have any arguments?
		if(arguments != nullptr)
		{
			// Add the arguments
			QMap<QString, QString>::const_iterator it;
			for(it = arguments->begin(); it != arguments->end(); ++it)
			{
				QString argumentName = it.key();
				soapMessage += "<" + argumentName + ">" + it.value() + "</" + argumentName + ">";
			}
		}

		// Add the closing tags
		soapMessage += "  </" + prefix + ":" + actionName + ">\n </" + m_szBaseXmlPrefix + ":Body>\n</" + m_szBaseXmlPrefix + ":Envelope>\n";

		// Get an utf8 encoding string
		QByteArray content = soapMessage.toUtf8().data();

		// Create the HTTP header
		QNetworkRequest request;
		request.setHeader(QNetworkRequest::ContentTypeHeader, "text/xml");
		request.setHeader(QNetworkRequest::ContentLengthHeader, content.size());
		request.setRawHeader("SOAPAction", QString("\"%1#%2\"").arg(m_szServiceType, actionName).toUtf8());

		QString port;
		port.setNum(m_iPort);
		request.setRawHeader("HOST", QString("%1:%2").arg(m_szHostname, port).toUtf8());

		QUrl url;
		url.setHost(m_szHostname);
		url.setPort(m_iPort);
		request.setUrl(url);

		// Send the POST request
		m_iPendingRequests++;

		QByteArray dummy;
		QNetworkReply * pReply = KviNetworkAccessManager::getInstance()->post(request, dummy);
		connect(pReply, SIGNAL(finished()), this, SLOT(slotRequestFinished()));

		return 0;
	}

	// Makes a UPnP service request
	// TODO: rename to downloadFile()
	int Service::callInformationUrl()
	{
		qDebug() << "UPnP::Service: requesting file '" << m_szInformationUrl << "'." << endl;

		// Send the GET request
		// TODO: User-Agent: Mozilla/4.0 (compatible; UPnP/1.0; Windows NT/5.1)
		m_iPendingRequests++;

		QNetworkRequest request;
		QByteArray dummy;
		QUrl url;
		url.setHost(m_szHostname);
		url.setPort(m_iPort);
		url.setPath(m_szInformationUrl);
		request.setUrl(url);
		QNetworkReply * pReply = KviNetworkAccessManager::getInstance()->post(request, dummy);
		connect(pReply, SIGNAL(finished()), this, SLOT(slotRequestFinished()));

		return 0;
	}

	// Get the number of pending requests
	int Service::getPendingRequests() const
	{
		return m_iPendingRequests;
	}

	// The control point received an action failure indication
	void Service::gotActionErrorResponse(const QDomNode & response)
	{
		QString faultString = XmlFunctions::getNodeValue(response, "/faultstring");
		QString errorCode = XmlFunctions::getNodeValue(response, "/detail/" + faultString + "/errorCode");
		QString errorDescription = XmlFunctions::getNodeValue(response, "/detail/" + faultString + "/errorDescription");
		qWarning() << "UPnP::Service - Action failed: " << errorCode << " " << errorDescription << endl;
	}

	// The control point received a response to callAction()
	void Service::gotActionResponse(const QString & responseType, const QMap<QString, QString> & /*resultValues*/)
	{
		qWarning() << "UPnP::Service - Action response '" << responseType << "' is not handled." << endl;
	}

	// The control point received a response to callInformationUrl()
	void Service::gotInformationResponse(const QDomNode & response)
	{
		QString rootTagName = response.nodeName();
		qWarning() << "UPnP::Service - Service response (with root '" << rootTagName << "') is not handled." << endl;
	}

	// The QHttp object retrieved data.
	void Service::slotRequestFinished()
	{
		QNetworkReply * reply = qobject_cast<QNetworkReply *>(sender());

		qDebug() << "UPnP::Service: received HTTP response for request " << endl;

		if(!reply)
		{
			qWarning() << "UPnP::Service - HTTP Request failed: " << reply->errorString() << endl;
			m_iPendingRequests--;
			emit queryFinished(true);
			return;
		}

		if(reply->error() != QNetworkReply::NoError)
		{
			qWarning() << "UPnP::Service - HTTP Request failed: " << reply->errorString() << endl;
			m_iPendingRequests--;
			emit queryFinished(true);
			reply->deleteLater();
			return;
		}

		// Get the XML content
		QByteArray response = reply->readAll();
		QDomDocument xml;

		qDebug() << "Response:\n"
		         << response << "\n---\n";

		// Parse the XML
		QString errorMessage;
		bool error = !xml.setContent(response, false, &errorMessage);

		if(!error)
		{
			QString baseNamespace = xml.documentElement().tagName();

			if(baseNamespace.length() > 0)
			{
				int cutAt = baseNamespace.indexOf(':');
				if(cutAt > -1)
				{
					baseNamespace.truncate(cutAt);
					qDebug() << "Device is using " << baseNamespace << " as XML namespace" << endl;
					m_szBaseXmlPrefix = baseNamespace;
				}
			}

			// Determine how to process the data
			if(xml.namedItem(m_szBaseXmlPrefix + ":Envelope").isNull())
			{
				qDebug() << "UPnP::Service: plain XML detected, calling gotInformationResponse()." << endl;
				// No SOAP envelope found, this is a normal response to callService()
				gotInformationResponse(xml.lastChild());
			}
			else
			{
				qDebug() << xml.toString() << endl;
				// Got a SOAP message response to callAction()
				QDomNode resultNode = XmlFunctions::getNode(xml, "/" + m_szBaseXmlPrefix + ":Envelope/" + m_szBaseXmlPrefix + ":Body").firstChild();

				error = (resultNode.nodeName() == m_szBaseXmlPrefix + ":Fault");

				if(!error)
				{
					if(resultNode.nodeName().startsWith("m:") || resultNode.nodeName().startsWith("u:"))
					{
						qDebug() << "UPnP::Service: SOAP envelope detected, calling gotActionResponse()." << endl;
						// Action success, return SOAP body
						QMap<QString, QString> resultValues;

						// Parse all parameters
						// It's possible to pass the entire QDomNode object to the gotActionResponse()
						// function, but this is somewhat nicer, and reduces code boat in the subclasses
						QDomNodeList children = resultNode.childNodes();
						for(int i = 0; i < children.count(); i++)
						{
							QString key = children.item(i).nodeName();
							resultValues[key] = children.item(i).toElement().text();
						}

						// Call the gotActionResponse()
						gotActionResponse(resultNode.nodeName().mid(2), resultValues);
					}
				}
				else
				{
					qDebug() << "UPnP::Service: SOAP error detected, calling gotActionResponse()." << endl;

					// Action failed
					gotActionErrorResponse(resultNode);
				}
			}
		}
		else
		{
			qWarning() << "UPnP::Service - XML parsing failed: " << errorMessage << endl;
		}

		// Only emit when bytes>0
		m_iPendingRequests--;
		emit queryFinished(error);
	}

} // end of namespace
