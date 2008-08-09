//=============================================================================
//
//   Creation date : Fri Aug 08 18:00:00 2000 GMT by Szymon Stefanek 
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
// Original Copyright following:
//=============================================================================

/***************************************************************************
                          service.cpp  -  description
                             -------------------
    begin                : Sun Jul 24 2005
    copyright            : (C) 2005 by Diederik van der Boor
    email                : vdboor --at-- codingdomain.com
 ***************************************************************************/

#include "service.h"
#include "xmlfunctions.h"

#include <QDebug>
#include <QHttp>
#include <QByteArray>

// This implementation was created with the help of the following documentation:
//   http://www.upnp.org/standardizeddcps/documents/UPnP_IGD_1.0.zip
//   http://zacbowling.com/upnp/
//   http://www.knoxscape.com/Upnp/NAT.htm
//   http://www.artima.com/spontaneous/upnp_digihome2.html


namespace UPnP
{


// The constructor for information services
Service::Service(const QString &hostname, int port, const QString &informationUrl)
  : informationUrl_(informationUrl)
  , pendingRequests_(0)
{
  http_ = new QHttp(hostname, port);
  connect(http_, SIGNAL(     requestFinished(int,bool) ) ,
          this,    SLOT( slotRequestFinished(int,bool) ) );

qDebug() << "UPnP::Service: Created information service url='" << informationUrl_ << "'." << endl;
}


// The constructor for action services
Service::Service(const ServiceParameters &params)
  : controlUrl_(params.controlUrl)
  , informationUrl_(params.scdpUrl)
  , pendingRequests_(0)
  , serviceId_(params.serviceId)
{
  http_ = new QHttp(params.hostname, params.port);
  connect(http_, SIGNAL(     requestFinished(int,bool) ) ,
          this,    SLOT( slotRequestFinished(int,bool) ) );

  qDebug() << "CREATED UPnP::Service: url='" << controlUrl_ << "' id='" << serviceId_ << "'." << endl;
}



// The destructor
Service::~Service()
{
  qDebug() << "DESTROYED UPnP::Service [url=" << controlUrl_ << ",  id=" << serviceId_ << "]" << endl;

  delete http_;
}



// Makes a UPnP action request
// TODO: rename to callMethod / callSoapMethod
int Service::callAction(const QString &actionName)
{
  return callActionInternal(actionName, 0);
}



// Makes a UPnP action request
int Service::callAction(const QString &actionName, const QMap<QString,QString> &arguments)
{
  return callActionInternal(actionName, &arguments);
}



// Makes a UPnP action request (keeps pointers from the external interface)
int Service::callActionInternal(const QString &actionName, const QMap<QString,QString> *arguments)
{
  qDebug() << "UPnP::Service: calling remote prodecure '" << actionName << "'." << endl;

  // Create the data message
  QString soapMessage = "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\""
                        " s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
                          "<s:Body>"
                            "<u:" + actionName + " xmlns:u=\"" + serviceId_ + "\">";

  // Do we have any arguments?
  if(arguments != 0)
  {
    // Add the arguments
    QMap<QString,QString>::const_iterator it;
    for(it = arguments->begin(); it != arguments->end(); ++it)
    {
      QString argumentName = it.key();
      soapMessage += "<" + argumentName + ">" + it.data() + "</" + argumentName + ">";
    }
  }

  // Add the closing tags
  soapMessage += "</u:" + actionName + "></s:Body></s:Envelope>";

  // Get an utf8 encoding string
  QByteArray content = soapMessage.utf8();

  // Create the HTTP header
  QHttpRequestHeader header("POST", controlUrl_);
  header.setContentType("text/xml; charset=\"utf-8\"");
  header.setContentLength(content.size());
  header.setValue("SoapAction", serviceId_ + "#" + actionName);

  // Send the POST request
  pendingRequests_++;
  return http_->request(header, content);
}



// Makes a UPnP service request
// TODO: rename to downloadFile()
int Service::callInformationUrl()
{
  qDebug() << "UPnP::Service: requesting file '" << informationUrl_ << "'." << endl;

  // Send the GET request
  // TODO: User-Agent: Mozilla/4.0 (compatible; UPnP/1.0; Windows NT/5.1)
  pendingRequests_++;
  return http_->get(informationUrl_);
}



// Get the number of pending requests
int Service::getPendingRequests() const
{
  return pendingRequests_;
}


// The control point received an action failure indication
void Service::gotActionErrorResponse(const QDomNode &response)
{
  QString  faultString      = XmlFunctions::getNodeValue(response, "/faultstring");
  QString  errorCode        = XmlFunctions::getNodeValue(response, "/detail/" + faultString + "/errorCode");
  QString  errorDescription = XmlFunctions::getNodeValue(response, "/detail/" + faultString + "/errorDescription");
  qWarning() << "UPnP::Service - Action failed: " << errorCode << " " << errorDescription << endl;
}



// The control point received a response to callAction()
void Service::gotActionResponse(const QString &responseType, const QMap<QString,QString> &/*resultValues*/)
{
  qWarning() << "UPnP::Service - Action response '" << responseType << "' is not handled." << endl;
}



// The control point received a response to callInformationUrl()
void Service::gotInformationResponse(const QDomNode &response)
{
  QString rootTagName = response.nodeName();
  qWarning() << "UPnP::Service - Service response (with root '" << rootTagName << "') is not handled." << endl;
}



// The QHttp object retrieved data.
void Service::slotRequestFinished(int /*id*/, bool error)
{
  qDebug() << "UPnP::Service: Got HTTP response." << endl;

  if(! error)
  {
    // Not sure why this happens
    if(http_->bytesAvailable() > 0)
    {
      // Get the XML content
      QByteArray   response = http_->readAll();
      QDomDocument xml;

      // TODO: handle 401 Authorisation required messages

      // Parse the XML
      QString errorMessage;
      error = ! xml.setContent(response, false, &errorMessage);

      if(! error)
      {
        // Determine how to process the data
        if(xml.namedItem("s:Envelope").isNull())
        {
          qDebug() << "UPnP::Service: Plain XML detected, calling gotInformationResponse()." << endl;
          // No SOAP envelope found, this is a normal response to callService()
          gotInformationResponse( xml.lastChild() );
        }
        else
        {
          qDebug() << xml.toString() << endl;
          // Got a SOAP message response to callAction()
          QDomNode resultNode = XmlFunctions::getNode(xml, "/s:Envelope/s:Body").firstChild();

          error = (resultNode.nodeName() == "s:Fault");

          if(! error)
          {
            if(resultNode.nodeName().startsWith("m:"))
            {
              qDebug() << "UPnP::Service: SOAP Envelope detected, calling gotActionResponse()." << endl;
              // Action success, return SOAP body
              QMap<QString,QString> resultValues;

              // Parse all parameters
              // It's possible to pass the entire QDomNode object to the gotActionResponse()
              // function, but this is somewhat nicer, and reduces code boat in the subclasses
              QDomNodeList children = resultNode.childNodes();
              for(uint i = 0; i < children.count(); i++)
              {
                QString key = children.item(i).nodeName();
                resultValues[ key ] = children.item(i).toElement().text();
              }

              // Call the gotActionResponse()
              gotActionResponse(resultNode.nodeName().mid(2), resultValues);
            }
          }
          else
          {
            qDebug() << "UPnP::Service: SOAP Error detected, calling gotActionResponse()." << endl;

            // Action failed
            gotActionErrorResponse(resultNode);
          }
        }
      }
      else
      {
        qWarning() << "UPnP::Service - XML Parsing failed: " << errorMessage << endl;
      }

      // Only emit when bytes>0
      pendingRequests_--;
      emit queryFinished(error);
    }
  }
  else
  {
    qWarning() << "UPnP::Service - HTTP Request failed: " << http_->errorString() << endl;
    pendingRequests_--;
    emit queryFinished(error);
  }

}


} // end of namespace
