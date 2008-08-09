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
                          manager.cpp -  description
                             -------------------
    begin                : Fri Aug 05 2005
    copyright            : (C) 2005 by Diederik van der Boor
    email                : vdboor --at-- codingdomain.com
 ***************************************************************************/

#include "manager.h"

#include "igdcontrolpoint.h"
#include "ssdpconnection.h"

#include <QDebug>
#include <QTimer>


namespace UPnP
{

// Set the static variable
Manager* Manager::instance_(0);


// The constructor
Manager::Manager()
  : activeIgdControlPoint_(0)
  , broadcastFailed_(false)
  , ssdpConnection_(0)
  , ssdpTimer_(0)
{

}



// The destructor
Manager::~Manager()
{
  delete ssdpTimer_;
  delete ssdpConnection_;
  instance_ = 0;  // Unregister the instance
}



// Initialize the manager, detect all devices
void Manager::initialize()
{
  qDebug() << "UPnP::Manager: Initiating a broadcast to detect UPnP devices..." << endl;


  // Create the SSDP object to detect devices
  ssdpConnection_ = new SsdpConnection();
  connect(ssdpConnection_, SIGNAL(     deviceFound(const QString&,int,const QString&) ) ,
          this,              SLOT( slotDeviceFound(const QString&,int,const QString&) ) );

  // Create a timer
  ssdpTimer_      = new QTimer(this);
  connect(ssdpTimer_, SIGNAL(timeout()), this, SLOT(slotBroadcastTimeout()));

  // Start a UPnP broadcast
  broadcastFailed_ = false;
  ssdpConnection_->queryDevices();
  ssdpTimer_->start(2000, true);
}



// Return the instance of the manager class
Manager * Manager::instance()
{
  // Create when it's required
  if(instance_ == 0)
  {
    instance_ = new Manager();
    instance_->initialize();
  }

  return instance_;
}



// Return the external IP address
QString Manager::getExternalIpAddress() const
{
  // Do not expose activeIgd_;
  return (activeIgdControlPoint_ != 0 ? activeIgdControlPoint_->getExternalIpAddress() : QString::null);
}



// Return true if a controlable gateway is available
bool Manager::isGatewayAvailable()
{
  return (activeIgdControlPoint_ != 0 &&
          activeIgdControlPoint_->isGatewayAvailable());
}



// The broadcast failed
void Manager::slotBroadcastTimeout()
{
qDebug() << "UPnP::Manager: Timeout, no broadcast response received!" << endl;

  broadcastFailed_ = true;
}



// A device was discovered by the SSDP broadcast
void Manager::slotDeviceFound(const QString &hostname, int port, const QString &rootUrl)
{
qDebug() << "UPnP::Manager: Device found, initializing IgdControlPoint to query it." << endl;

  IgdControlPoint *controlPoint = new IgdControlPoint(hostname, port, rootUrl);
  igdControlPoints_.append(controlPoint);

  if(activeIgdControlPoint_ == 0)
  {
    activeIgdControlPoint_ = controlPoint;
    activeIgdControlPoint_->initialize();
  }
}



}  // end of namespace
