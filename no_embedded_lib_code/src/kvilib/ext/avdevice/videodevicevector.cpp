//=============================================================================
//
//   File : videodevicevector.cpp
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
    videodevicevector.cpp  -  Kopete Video Device List Abstration

    Copyright (c) 2009 by Alan Jones <skyphyr@gmail.com>

    Kopete    (c) 2002-2009      by the Kopete developers  <kopete-devel@kde.org>

    *************************************************************************
    *                                                                       *
    * This library is free software; you can redistribute it and/or         *
    * modify it under the terms of the GNU General Public                   *
    * License as published by the Free Software Foundation; either          *
    * version 3 of the License, or (at your option) any later version.      *
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

#include "videodevicevector.h"

namespace Kopete {

namespace AV {

VideoDevice& VideoDeviceVector::operator[] (int i)
	{
	if (i>=size())
		{
		//we're outside the range so let's return a default camera not available VideoDevice
		return fallback;
		}
	
	//if we've made it here then we're within range
	return QVector<VideoDevice>::operator[](i);
	}
	
const VideoDevice& VideoDeviceVector::operator[] (int i) const
	{
	return at(i);
	}

const VideoDevice& VideoDeviceVector::at(int i) const
	{
	if (i>=size())
		{
		//we're outside the range so let's return a default camera not available VideoDevice
		return fallback;
		}
	
	//if we've made it here then we're within range
	return QVector<VideoDevice>::operator[](i);
	}

}

}
