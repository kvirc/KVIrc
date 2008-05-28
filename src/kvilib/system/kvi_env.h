#ifndef _KVI_ENV_H_
#define _KVI_ENV_H_

//=============================================================================
//
//   File : kvi_env.h
//   Creation date : Sat May 05 2002 02:15:21 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002 Szymon Stefanek (pragma@kvirc.net)
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

//=============================================================================
// Environment function wrappers
//=============================================================================

#include "kvi_settings.h"



#include <stdlib.h>

inline char * kvi_getenv(const char * name)
{
#ifdef HAVE_GETENV
	return getenv(name);
#else
	return 0;
#endif
}

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	#define kvi_setenv(__name,__value) SetEnvironmentVariable(__name,__value)
	#define kvi_unsetenv(__name) SetEnvironmentVariable(__name,NULL)
#else
	#ifndef _KVI_ENV_CPP_
		KVILIB_API extern bool kvi_setenv(const char * name,const char * value);
		KVILIB_API extern void kvi_unsetenv(const char * name);
	#endif
#endif




#endif //_KVI_ENV_H_
