#ifndef _KVI_SETTINGS_H_
#define _KVI_SETTINGS_H_

//=============================================================================
//
//   File : kvi_settings.h
//   Creation date : Fri Mar 19 1999 05:21:13 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2001 Szymon Stefanek (pragma at kvirc dot net)
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
// This file contains compile time settings, either set by configure or
// non-customizable defaults
//
// Better do not touch this
//=============================================================================

#include <qglobal.h>

// FIXME: Once we have a stable CMake build system, this section needs a cleanup.
#if defined(_OS_WIN32_) || defined(Q_OS_WIN32) || defined(Q_OS_WIN32_)

	#define FEEL_LIKE_I_AM_COMPILING_UNDER_WINDOZE
	#define COMPILE_ON_WINDOWS

	// Windows has no config.h

	#include "kvi_wincfg.h"

	#ifdef __KVILIB__
		#define KVILIB_API __declspec(dllexport)
	#else
		#define KVILIB_API __declspec(dllimport)
	#endif

	#ifdef __KVIRC__
		#define KVIRC_API __declspec(dllexport)
	#else
		#define KVIRC_API __declspec(dllimport)
	#endif

#else
	#ifdef Q_OS_MACX
		#define COMPILE_ON_MAC
	#endif

	#ifdef KVIRC_EXTERNAL_MODULE
		// when compiling an external module
		// include the last configuration
		#include "kvi_configstatus.h"
	#else
		#ifdef HAVE_CONFIG_H
			#include "config.h"
			// temporary fix
			#ifndef COMPILE_NO_X
				#ifndef COMPILE_X11_SUPPORT
					#define COMPILE_X11_SUPPORT
				#endif
			#endif

			#ifndef VERSION
				#define VERSION "?.?.?"
			#endif

		#else
			// assume CMake build system
			#include "kvi_sysconfig.h"

			#define VERSION KVIRC_VERSION_RELEASE
			#define VERSION_BRANCH KVIRC_VERSION_BRANCH
		#endif
	#endif

	#define KVILIB_API
	#define KVIRC_API

#endif

#ifndef COMPILE_USE_QT4
	// It's now a requirement! (until we cleanup the code and will be able to remove this conditional at all)
	#define COMPILE_USE_QT4
#endif

#define KVI_VERSION VERSION
#define KVI_VERSION_BRANCH VERSION_BRANCH

#define KVI_RELEASE_NAME "Insomnia"

/* HACK: this is an hack to get dynamic labels while porting.
         this line MUST be removed when the new Qt4 implementation
         is working
*/
#define COMPILE_USE_DYNAMIC_LABELS

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

#ifdef COMPILE_NO_X
	#ifndef COMPILE_NO_X_BELL
		#define COMPILE_NO_X_BELL
	#endif
#endif

/*
#if __GNUC__ - 0 > 3 || (__GNUC__ - 0 == 3 && __GNUC_MINOR__ - 0 >= 2)
	#define KVI_DEPRECATED __attribute__ ((deprecated))
#else
	#define KVI_DEPRECATED
#endif
*/
#define KVI_DEPRECATED
#define debug qDebug


// Trust Qt about the current target processor endianness
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
	#define BIG_ENDIAN_MACHINE_BYTE_ORDER
#else
	#ifdef BIG_ENDIAN_MACHINE_BYTE_ORDER
		#undef BIG_ENDIAN_MACHINE_BYTE_ORDER
	#endif
#endif

#endif //_KVI_SETTINGS_H_
