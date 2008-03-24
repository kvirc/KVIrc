#ifndef _KVI_HEAPOBJECT_H_
#define _KVI_HEAPOBJECT_H_
//=============================================================================
//
//   File : kvi_heapobject.h
//   Created on Wed 24 Mar 2004 04:45:17 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2006 Szymon Stefanek <pragma at kvirc dot net>
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
//=============================================================================

#include "kvi_settings.h"

// See kvi_heapobject.cpp for comments on this class

#ifdef COMPILE_ON_WINDOWS
	
	class KVILIB_API KviHeapObject
	{
	public:
		void * operator new(size_t uSize);
		void operator delete(void * pData);
		void * operator new[](size_t uSize);
		void operator delete[](void * pData);
		void * operator new(size_t uSize,const char *,int);
		void operator delete(void * pData,const char *,int);
	};
#else //!COMPILE_ON_WINDOWS
	class KVILIB_API KviHeapObject
	{
		// on other platforms this crap is not necessary
	};
#endif //!COMPILE_ON_WINDOWS

#endif //!_KVI_HEAPOBJECT_H_
