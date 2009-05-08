#ifndef _KVI_LIBRARY_H_
#define _KVI_LIBRARY_H_

//=====================================================================================
//
//   File : kvi_library.h
//   Creation date : Tue Sep 25 16:20:40 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=====================================================================================


//=====================================================================================
// System dynamic linker interface abstraction
//=====================================================================================


#include "kvi_settings.h"


#ifdef COMPILE_ON_WINDOWS

	//#include <windows.h>
	#include <winsock2.h> // this will pull in windows.h

	typedef HMODULE kvi_library_t;

	inline kvi_library_t kvi_library_open(const char * path)
	{
#ifndef DEBUG
		// this is to avoid the ugly message boxes when the dll has
		// ... but do it only in release mode
		UINT nOldErrorMode = SetErrorMode(SEM_FAILCRITICALERRORS);
#endif
		kvi_library_t ret = LoadLibrary(path);
#ifndef DEBUG
		SetErrorMode(nOldErrorMode);
#endif
		return ret;
	};

	inline void kvi_library_close(kvi_library_t lib)
	{
		FreeLibrary(lib);
	};

	inline void * kvi_library_symbol(kvi_library_t lib,const char * symName)
	{
		return GetProcAddress(lib,symName);
	};

	inline const char * kvi_library_error()
	{
		return "Windoze-like error";
	};

#else

	#include <dlfcn.h>

	// sparc-unknown-openbsd3.0 (At least) has only RTLD_LAZY
	#ifndef RTLD_NOW
		#define RTLD_NOW RTLD_LAZY
	#endif
	#ifndef RTLD_GLOBAL
		#define RTLD_GLOBAL 0
	#endif

	typedef void * kvi_library_t;

	inline kvi_library_t kvi_library_open(const char * path)
	{
		return dlopen(path,RTLD_GLOBAL | RTLD_NOW);
	};

	inline void kvi_library_close(kvi_library_t lib)
	{
		dlclose(lib);
	};


	inline void * kvi_library_symbol(kvi_library_t lib,const char * symName)
	{
		return dlsym(lib,symName);
	};

	inline const char * kvi_library_error()
	{
		return dlerror();
	};


#endif //!COMPILE_ON_WINDOWS


#define kvi_library_load kvi_library_open
#define kvi_library_unload kvi_library_close

#endif //_KVI_LIBRARY_H_
