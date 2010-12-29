//=============================================================================
//
//   File : videodevicepool.h
//   Creation date : Tue Nov 10 18:08:09 2009 GMT by Fabio Bas
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2009 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//   This file has been taken from the "Kopete, The KDE Instant Messenger"
//   sourcecode. Original copyright of the file follows:
//=============================================================================

/*
    videodevicepool.h  -  Kopete Multiple Video Device handler Class

    Copyright (c) 2005-2006 by Cláudio da Silveira Pinheiro   <taupter@gmail.com>
    Copyright (c) 2010      by Frank Schaefer                 <fschaefer.oss@googlemail.com>

    Kopete    (c) 2002-2003      by the Kopete developers  <kopete-devel@kde.org>

    *************************************************************************
    *                                                                       *
    * This library is free software; you can redistribute it and/or         *
    * modify it under the terms of the GNU Lesser General Public            *
    * License as published by the Free Software Foundation; either          *
    * version 2 of the License, or (at your option) any later version.      *
    *                                                                       *
    *************************************************************************
*/

#ifndef VIDEODEVICEPOOL_H
#define VIDEODEVICEPOOL_H

#include <iostream>
#include "kvi_settings.h"


#include "videoinput.h"
#include <QString>
#include <QImage>
#include <QComboBox>
#include "videodevice.h"

#ifdef COMPILE_KDE_SUPPORT
	#include <solid/device.h>
#endif

namespace Kopete {

namespace AV {

/**
This class allows kopete to check for the existence, open, configure, test, set parameters, grab frames from and close a given video capture card using the Video4Linux API.

@author Cláudio da Silveira Pinheiro
*/

class KVILIB_API VideoDevicePool : public QObject
{
Q_OBJECT
public:
	static VideoDevicePool* self();
	int open(int device = -1);
	bool isOpen();
	int getFrame();
	int width();
	int minWidth();
	int maxWidth();
	int height();
	int minHeight();
	int maxHeight();
	int setSize( int newwidth, int newheight);
	int close();
	int startCapturing();
	int stopCapturing();
	int readFrame();
	int getImage(QImage *qimage);
	int selectInput(int newinput);
	int size();
	~VideoDevicePool();
	void fillDeviceQComboBox(QComboBox *combobox);
	void fillInputQComboBox(QComboBox *combobox);
	void fillStandardQComboBox(QComboBox *combobox);
	QString currentDeviceUdi();
	int currentDevice();
	int currentInput();
	int inputs();

	QList<NumericVideoControl> getSupportedNumericControls();
	QList<BooleanVideoControl> getSupportedBooleanControls();
	QList<MenuVideoControl> getSupportedMenuControls();
	QList<ActionVideoControl> getSupportedActionControls();

	int getControlValue(quint32 ctrl_id, qint32 * value);
	int setControlValue(quint32 ctrl_id, qint32 value);

	void saveCurrentDeviceConfig();

signals:
	/**
	 * Provisional signatures, probably more useful to indicate which device was registered
	 */
	void deviceRegistered( const QString & udi );
	void deviceUnregistered( const QString & udi );

protected slots:
	/**
	 * Slot called when a new device is added to the system
	 */
	void deviceAdded( const QString & udi );
	void deviceRemoved( const QString & udi );

protected:
	int xioctl(int request, void *arg);
	int errnoReturn(const char* s);
#ifdef COMPILE_KDE_SUPPORT
	bool registerDevice( Solid::Device & dev );
#endif
	int showDeviceCapabilities(int device = -1);
	int getSavedDevice();
	void loadDeviceConfig(); // Load configuration parameters;

	int m_current_device;
	QVector<VideoDevice*> m_videodevices;	/*!< Vector of pointers to the available video devices */
	struct imagebuffer m_buffer; // only used when no devices were found

private:
	VideoDevicePool();
	static VideoDevicePool* s_self;
	static __u64 m_clients; // Number of instances

};

}

}

#endif // VIDEODEVICEPOOL_H
