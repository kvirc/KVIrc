//=============================================================================
//
//   Creation date : Fri Aug 08 18:00:00 2000 2008 GMT by Fabio Bas
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
                          wanconnectionservice.cpp -  description
                             -------------------
    begin                : Mon Jul 25 2005
    copyright            : (C) 2005 by Diederik van der Boor
    email                : vdboor --at-- codingdomain.com
 ***************************************************************************/

#include "wanconnectionservice.h"
#include <QDebug>

namespace UPnP
{


// The constructor
WanConnectionService::WanConnectionService(const ServiceParameters &params)
  : Service(params)
  , m_bNatEnabled(false)
{

}


// The destructor
WanConnectionService::~WanConnectionService()
{

}



// Add a port mapping
void WanConnectionService::addPortMapping(const QString &protocol, const QString &remoteHost, int externalPort,
                                          const QString &internalClient, int internalPort, const QString &description,
                                          bool enabled, int leaseDuration)
{
  // TODO: this still needs to be tested
  QMap<QString,QString> arguments;
  arguments["NewProtocol"]               = protocol;
  arguments["NewRemoteHost"]             = remoteHost;
  arguments["NewExternalPort"]           = QString::number(externalPort);
  arguments["NewInternalClient"]         = internalClient;
  arguments["NewInternalPort"]           = QString::number(internalPort);
  arguments["NewPortMappingDescription"] = description;
  arguments["NewEnabled"]                = QString::number(enabled ? 1 : 0);
  arguments["NewLeaseDuration"]          = QString::number(leaseDuration);
  callAction("AddPortMapping", arguments);
}



// Delete a port mapping
void WanConnectionService::deletePortMapping(const QString &protocol, const QString &remoteHost, int externalPort)
{
  // TODO: this still needs to be tested
  QMap<QString,QString> arguments;
  arguments["NewProtocol"]       = protocol;
  arguments["NewRemoteHost"]     = remoteHost;
  arguments["NewExternalPort"]   = QString::number(externalPort);
  callAction("DeletePortMapping", arguments);
}



// Return the external IP address
QString WanConnectionService::getExternalIpAddress() const
{
  return m_szExternalIpAddress;
}



// Return true if NAT is enabled
bool WanConnectionService::getNatEnabled() const
{
  return m_bNatEnabled;
}



// Return the port mappings
const KviPointerList<PortMapping>& WanConnectionService::getPortMappings() const
{
  return m_lPortMappings;
}



// The control point received a response to callAction()
void WanConnectionService::gotActionResponse(const QString &responseType, const QMap<QString,QString> &resultValues)
{
  qDebug() << "UPnP::WanConnectionService: Parsing action response:"
            << " type='" << responseType << "'." << endl;

  // Check the message type
  if(responseType == "GetExternalIPAddressResponse")
  {
    // Get the external IP address from the response
    m_szExternalIpAddress = resultValues["NewExternalIPAddress"];

    qDebug() << "UPnP::WanConnectionService: externalIp='" << m_szExternalIpAddress << "'." << endl;
  }
  else if(responseType == "GetNATRSIPStatusResponse")
  {
    // Get the nat status from the response
    m_bNatEnabled = (resultValues["NewNATEnabled"] == "1");

    qDebug() << "UPnP::WanConnectionService: natEnabled=" << m_bNatEnabled << "." << endl;
  }
  else if(responseType == "GetGenericPortMappingEntryResponse")
  {
    // Find a place to store the data
    PortMapping *map = new PortMapping;

    // Get the port mapping data from the response
    map->enabled        = (resultValues["NewEnabled"] == "1");
    map->externalPort   =  resultValues["NewExternalPort"].toInt();
    map->internalClient =  resultValues["NewInternalClient"];
    map->internalPort   =  resultValues["NewInternalPort"].toInt();
    map->leaseDuration  =  resultValues["NewLeaseDuration"].toInt();
    map->description    =  resultValues["NewPortMappingDescription"];
    map->protocol       =  resultValues["NewProtocol"];
    map->remoteHost     =  resultValues["NewRemoteHost"];

    // Register the mapping
    m_lPortMappings.append(map);

    qDebug() << "UPnP::WanConnectionService - Got mapping: " << map->protocol << " " << map->remoteHost << ":" << map->externalPort
              << " to " << map->internalClient << ":" << map->internalPort
              << "    max " << map->leaseDuration << "s '" << map->description << "' " << (map->enabled ? "enabled" : "disabled") << endl;
  }
  else
  {
    qDebug() << "UPnP::WanConnectionService - Unexpected response type"
                << " '" << responseType << "' encountered." << endl;
  }
}



// Query for the external IP address
void WanConnectionService::queryExternalIpAddress()
{
  callAction("GetExternalIPAddress");
}



// Query for the Nat status
void WanConnectionService::queryNatEnabled()
{
  callAction("GetNATRSIPStatus");
}



// Query for a port mapping entry
void WanConnectionService::queryPortMappingEntry(int index)
{
  QMap<QString,QString> arguments;
  arguments["NewPortMappingIndex"] = QString::number(index);
  callAction("GetGenericPortMappingEntry", arguments);
}



} // End of namespace
