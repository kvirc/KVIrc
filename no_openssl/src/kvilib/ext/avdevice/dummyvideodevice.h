//=============================================================================
//
//   File : dummyvideodevice.h
//   Creation date : Tue Nov 10 18:08:09 2009 GMT by Fabio Bas
//
//   This file is part of the KVirc irc client distribution
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
    dummyvideodevice.h  -  Kopete Dummy Video Device

    Copyright (c) 2009 by Alan Jones <skyphyr@gmail.com>

    Kopete    (c) 2002-2009      by the Kopete developers  <kopete-devel@kde.org>

    *************************************************************************
    *                                                                       *
    * This library is free software; you can redistribute it and/or         *
    * modify it under the terms of the GNU Lesser General Public            *
    * License as published by the Free Software Foundation; either          *
    * version 2 of the License, or (at your option) any later version.      *
    *                                                                       *
    * This program is distributed in the hope that it will be useful,       *
    * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
    * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
    * GNU General Public License for more details.                          *
    *                                                                       *
    * You should have received a copy of the GNU General Public License     *
    * along with this program.  If not, see <http://www.gnu.org/licenses/>. *
    *************************************************************************
*/

#ifndef KOPETE_AVDUMMYVIDEODEVICE_H
#define KOPETE_AVDUMMYVIDEODEVICE_H

#include "videodevice.h"

namespace Kopete {

namespace AV {

/**
@author Kopete Developers
*/

class DummyVideoDevice : public VideoDevice
{

public:
	DummyVideoDevice();
	virtual ~DummyVideoDevice() {}
	
	virtual int open();
	virtual bool isOpen();
	virtual int checkDevice();
	virtual int initDevice();

	virtual int setSize( int newwidth, int newheight);

	virtual pixel_format setPixelFormat(pixel_format newformat);

	virtual int startCapturing();
	virtual int getFrame();
	virtual int getFrame(imagebuffer *imgbuffer);
	virtual int getImage(QImage *qimage);
	virtual int stopCapturing();
	virtual int close();
	
	
private:
	bool opened;
};

}

}

#endif
