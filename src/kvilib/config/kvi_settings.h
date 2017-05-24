#ifndef _KVI_SETTINGS_H_
#define _KVI_SETTINGS_H_
//=============================================================================
//
//   File : kvi_settings.h
//   Creation date : Fri Mar 19 1999 05:21:13 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 1999-2010 Szymon Stefanek (pragma at kvirc dot net)
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

/**
* \file kvi_settings.h
* \author Szymon Stefanek
* \brief This file contains compile time settings
*
* They are either set by configure or non-customizable defaults
* Better to not touch this
*/

#include <QtGlobal>

// assume CMake build system for all systems
#include "kvi_sysconfig.h"

/**
 * \def KVILIB_API This prefix before a class name or function enables the export of its symbols outside kvilib; needed for win32/visual studio, win32/mingw, linux/gcc.
 * \def KVILIB_API_TYPEDEF This prefix before a typedef enables the export of its symbol outside kvilib; needed for win32/visual studio, win32/mingw
 */

/**
 * \def KVIRC_API This prefix before a class name or function enables the export of its symbols outside kvirc; needed for win32/visual studio, win32/mingw, linux/gcc.
 * \def KVIRC_API_TYPEDEF This prefix before a typedef enables the export of its symbol outside kvirc; needed for win32/visual studio, win32/mingw
 */
// note: MINGW is a cmake-set variable
#ifdef MINGW

/**
	 * \def COMPILE_ON_MINGW This flag will enable specific code for windows/mingw (cross)compilation
	 */
#define COMPILE_ON_MINGW

#ifdef _WANT_KVILIB_
#define KVILIB_API __declspec(dllexport) __attribute__((visibility("default")))
#define KVILIB_API_TYPEDEF __declspec(dllexport)
#else
#define KVILIB_API __declspec(dllimport) __attribute__((visibility("default")))
#define KVILIB_API_TYPEDEF __declspec(dllimport)
#endif

#ifdef _WANT_KVIRC_
#define KVIRC_API __declspec(dllexport) __attribute__((visibility("default")))
#define KVIRC_API_TYPEDEF __declspec(dllexport)
#else
#define KVIRC_API __declspec(dllimport) __attribute__((visibility("default")))
#define KVIRC_API_TYPEDEF __declspec(dllimport)
#endif

// note: unofficial cmake ports defines OS2
#elif defined(OS2)

// os2 ports still uses mingw/gcc
#define COMPILE_ON_MINGW

// visibility attribute is not supported by gcc/os2
#ifdef _WANT_KVILIB_
#define KVILIB_API __declspec(dllexport)
#define KVILIB_API_TYPEDEF __declspec(dllexport)
#else
#define KVILIB_API __declspec(dllimport)
#define KVILIB_API_TYPEDEF __declspec(dllimport)
#endif

#ifdef _WANT_KVIRC_
#define KVIRC_API __declspec(dllexport)
#define KVIRC_API_TYPEDEF __declspec(dllexport)
#else
#define KVIRC_API __declspec(dllimport)
#define KVIRC_API_TYPEDEF __declspec(dllimport)
#endif

// note: cmake defines "WIN32" but we're not using it since it includes cygwin, too
// we use qt's Q_OS_WIN32 instead (and its variants)
#elif(defined(_OS_WIN32_) || defined(Q_OS_WIN32) || defined(Q_OS_WIN32_))

/**
	 * \def COMPILE_ON_WINDOWS This flag will enable specific code for windows/visual studio compilation
	 */
#define COMPILE_ON_WINDOWS

#ifdef _WANT_KVILIB_
#define KVILIB_API __declspec(dllexport)
#define KVILIB_API_TYPEDEF __declspec(dllexport)
#else
#define KVILIB_API __declspec(dllimport)
#define KVILIB_API_TYPEDEF __declspec(dllimport)
#endif

#ifdef _WANT_KVIRC_
#define KVIRC_API __declspec(dllexport)
#define KVIRC_API_TYPEDEF __declspec(dllexport)
#else
#define KVIRC_API __declspec(dllimport)
#define KVIRC_API_TYPEDEF __declspec(dllimport)
#endif

#else
// we failback on unix/linux variants
#define KVILIB_API __attribute__((visibility("default")))
#define KVILIB_API_TYPEDEF

#define KVIRC_API __attribute__((visibility("default")))
#define KVIRC_API_TYPEDEF

// note: cmake defines "APPLE" but we're not using it since it includes legacy mac os version (pre-X)
// we use qt's Q_OS_MACX instead
#ifdef Q_OS_MACX
/**
		* \def COMPILE_ON_MAC This flag will enable specific code for mac compilation
		*/

#define COMPILE_ON_MAC
#endif

#endif

/**
* \def KVI_VERSION Defines the current KVIrc version
* \def KVIRC_VERSION_BRANCH Defines the KVIrc version of the current branch
* \def KVI_RELEASE_NAME Defines the codename for the current KVIrc release
* \def _GNU_SOURCE Enables _GNU_SOURCE features
* \def KVI_PTR2MEMBER Cross-platform macro that returns a member from its pointer
* \def KVI_DEPRECATED Prefix for deprecated objects inside KVIrc code (currently unused)
* \def BIG_ENDIAN_MACHINE_BYTE_ORDER If defined, the current target processor is big endian, little endian otherwise
*/

#define KVI_VERSION KVIRC_VERSION_RELEASE

#ifndef KVIRC_VERSION_BRANCH
#define KVI_VERSION_BRANCH "VS_BRANCH"
#else
#define KVI_VERSION_BRANCH KVIRC_VERSION_BRANCH
#endif

#define KVI_RELEASE_NAME KVIRC_VERSION_CODENAME

// We want _GNU_SOURCE features
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#if defined(__GNUC__)
// gcc
#if __GNUC__ >= 3
#define KVI_PTR2MEMBER(__x) &__x
#else
#define KVI_PTR2MEMBER(__x) &(__x)
#endif
#elif defined(COMPILE_ON_WINDOWS)
// Visual C++
#define KVI_PTR2MEMBER(__x) &__x
#elif defined(__SUNPRO_CC)
// Sun Forte
#define KVI_PTR2MEMBER(__x) (__x)
#else
// default
#define KVI_PTR2MEMBER(__x) &(__x)
#endif

#define KVI_DEPRECATED

// Trust Qt about the current target processor endianness
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
#define BIG_ENDIAN_MACHINE_BYTE_ORDER
#else
#ifdef BIG_ENDIAN_MACHINE_BYTE_ORDER
#undef BIG_ENDIAN_MACHINE_BYTE_ORDER
#endif
#endif

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
#define UNICODE
#define _UNICODE
#endif

#endif //_KVI_SETTINGS_H_
