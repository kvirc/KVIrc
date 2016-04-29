#ifndef _KVI_MEXLINKFILTER_H_
#define _KVI_MEXLINKFILTER_H_
//=============================================================================
//
//   File : KviMexLinkFilter.h
//   Creation date : Wed Apr 11 2003 03:07 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2010 Szymon Stefanek (pragma at kvirc dot net)
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
//=============================================================================

#include "kvi_settings.h"
#include "KviModuleExtension.h"

#include <QObject>

class KviDataBuffer;

class KVIRC_API KviMexLinkFilter : public QObject, public KviModuleExtension
{
	Q_OBJECT
public:
	KviMexLinkFilter(KviModuleExtensionDescriptor * d);
	~KviMexLinkFilter();

public:
	virtual void processData(char * buffer, int len) = 0;
	virtual bool sendPacket(KviDataBuffer * pData) = 0;
	virtual bool clearOutputQueue(bool bPrivateMessagesOnly) = 0;
	virtual unsigned int outputQueueSize() = 0;
};

#endif //!_KVI_MEXLINKFILTER_H_
