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
                          manager.h -  description
                             -------------------
    begin                : Fri Aug 05 2005
    copyright            : (C) 2005 by Diederik van der Boor
    email                : vdboor --at-- codingdomain.com
 ***************************************************************************/

#ifndef UPNPMANAGER_H
#define UPNPMANAGER_H

#include <QObject>
#include "kvi_pointerlist.h"

class QTimer;

namespace UPnP
{

class IgdControlPoint;
class SsdpConnection;


/**
 * The manager class is the public interface used by other networking classes.
 * It's implemented as singleton to provide easy access by other classes.
 * Devices are automatically detected at start-up, and maintained by this class.
 * Underneath, the actual work is done by the SsdpConnection and IgdControlPoint classes.
 *
 * @author Diederik van der Boor
 * @ingroup NetworkUPnP
 */
class Manager : public QObject
{
	Q_OBJECT

public:  // public methods

	// The destructor
	virtual             ~Manager();

	// Return the external IP address
	QString              getExternalIpAddress() const;

	// Return the instance of the manager class
	static Manager *     instance();

	// Return true if a controlable gateway is available
	bool                 isGatewayAvailable();

private slots:
	// The broadcast failed
	void                 slotBroadcastTimeout();
	// A device was discovered by the SSDP broadcast
	void                 slotDeviceFound(const QString &hostname, int port, const QString &rootUrl);


private:  // private methods
	// The constructor  (it's a singleton)
				Manager();
	// Disable the copy constructor
				Manager(const Manager &);
	// Disable the assign operator
	Manager&             operator=(const Manager&);
	// Initialize the manager, detect all devices
	void                 initialize();

private:
	// The active control point we're working with
	IgdControlPoint     *activeIgdControlPoint_;
	// True if the broadcast failed (false during the discovery process)
	bool                 broadcastFailed_;
	// True if the broadcast found a device (false during the discovery process)
	bool                 broadcastFoundIt_;
	// The instance of the singleton class
	static Manager      *instance_;
	// A list of all detected gateway devices
	KviPointerList<IgdControlPoint> igdControlPoints_;
	// The SSDP connection to find all UPnP devices
	SsdpConnection      *ssdpConnection_;
	// The timer to detect a broadcast timeout
	QTimer              *ssdpTimer_;
};


}  // End of namespace

#endif
