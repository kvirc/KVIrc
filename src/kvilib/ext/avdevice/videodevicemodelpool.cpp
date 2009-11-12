//=============================================================================
//
//   File : videodevicepool.cpp
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
    videodevicepool.cpp  -  Kopete Multiple Video Device handler Class

    Copyright (c) 2005-2006 by Cláudio da Silveira Pinheiro   <taupter@gmail.com>

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

#include "videodevicemodelpool.h"

namespace Kopete {

namespace AV {

VideoDeviceModelPool::VideoDeviceModelPool()
{
}


VideoDeviceModelPool::~VideoDeviceModelPool()
{
}

void VideoDeviceModelPool::clear()
{
	m_devicemodel.clear();
}

size_t VideoDeviceModelPool::size()
{
	return m_devicemodel.size();
}

size_t VideoDeviceModelPool::addModel( QString newmodel )
{
	VideoDeviceModel newdevicemodel;
	newdevicemodel.model=newmodel;
	newdevicemodel.count=0;

	if(m_devicemodel.size())
	{
		for ( int loop = 0 ; loop < m_devicemodel.size(); loop++)
		if (newmodel == m_devicemodel[loop].model)
		{
			kDebug() << "Model " << newmodel << " already exists.";
			m_devicemodel[loop].count++;
			return m_devicemodel[loop].count;
		}
	}
	m_devicemodel.push_back(newdevicemodel);
	m_devicemodel[m_devicemodel.size()-1].model = newmodel;
	m_devicemodel[m_devicemodel.size()-1].count = 0;
	return 0;
}


}

}
