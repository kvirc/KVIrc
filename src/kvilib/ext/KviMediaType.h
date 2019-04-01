#ifndef _KVI_MEDIATYPE_H_
#define _KVI_MEDIATYPE_H_
//=============================================================================
//
//   File : KviMediaType.h
//   Creation date : Mon Aug 21 2000 17:19:56 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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
#include "KviHeapObject.h"
#include "KviCString.h"

//
// IANA media-types matching subsystem
//
// WARNING: This class is used in multiple threads
//          Thus every usage of the classes and structures defined
//          in this file MUST be protected by calls
//          to KviMediaManager::lock() and KviMediaManager::unlock()
//

struct KviDefaultMediaType
{
	const char * filemask;
	const char * magicbytes;
	const char * ianatype;
	const char * description;
	const char * commandline;
};

class KVILIB_API KviMediaType : public KviHeapObject
{
public:
	KviMediaType(){};
	~KviMediaType(){};

public:
	KviCString szFileMask;
	KviCString szMagicBytes;
	KviCString szIanaType;
	KviCString szDescription;
	KviCString szSavePath;
	KviCString szCommandline;
	KviCString szRemoteExecCommandline;
	KviCString szIcon;
};

#endif //_KVI_MEDIATYPE_H_
