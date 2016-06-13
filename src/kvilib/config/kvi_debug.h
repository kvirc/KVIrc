#ifndef _KVI_DEBUG_H_
#define _KVI_DEBUG_H_
//=============================================================================
//
//   File : kvi_debug.h
//   Creation date : Fri Mar 19 1999 03:10:39 CEST by Szymon Stefanek
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

#include <QtGlobal>

#include "kvi_sysconfig.h"

/**
* \file kvi_debug.h
* \author Szymon Stefanek
* \brief This file contains the definition of the debug macros;
*/

#include <stdlib.h> // abort

#ifdef __GNUC__

#define kvi_debug(fmt, arg...) qDebug(fmt, ##arg)
#define kvi_warning(fmt, arg...) qWarning(fmt, ##arg)
#define kvi_fatal(fmt, arg...) \
	do                         \
	{                          \
		qFatal(fmt, ##arg);    \
		abort();               \
	} while(0)
#define KVI_PRETTY_FUNCTION __PRETTY_FUNCTION__

#else //!__GNUC__

// assume MSVC

#define kvi_debug(fmt, ...) qDebug(fmt, __VA_ARGS__)
#define kvi_warning(fmt, ...) qWarning(fmt, __VA_ARGS__)
#define kvi_fatal(fmt, ...)       \
	do                            \
	{                             \
		qFatal(fmt, __VA_ARGS__); \
		abort();                  \
	} while(0)
#define KVI_PRETTY_FUNCTION __FUNCTION__

#endif //!__GNUC__

#ifdef COMPILE_DEBUG_MODE

#define KVI_ASSERT(condition)                                                                                       \
	do                                                                                                              \
	{                                                                                                               \
		if(!(condition))                                                                                            \
			qFatal("[ASSERT FAILED] (" #condition ") in %s at %s:%u", KVI_PRETTY_FUNCTION, __FILE__, __LINE__);     \
	} while(0)

#define KVI_ASSERT_MSG(condition, message)                                                                          \
	do                                                                                                              \
	{                                                                                                               \
		if(!(condition))                                                                                            \
		{                                                                                                           \
			qFatal("[ASSERT FAILED] (" #condition ") in %s at %s:%u", KVI_PRETTY_FUNCTION, __FILE__, __LINE__);     \
			qFatal("[ASSERT FAILED] " message);                                                                     \
		}                                                                                                           \
	} while(0)

#include "KviDebugContext.h"

// The following two macros are used to create unique variable names
// by the means of the __LINE__ builtin macro.
// The ## token paste operator must be called inside a macro and must
// precede a macro parameter. This is why we can't use directly
//
// #define UNIQUEVARIABLE int name ## __LINE__
//
// We need something like
//
// #define PASTE(x,y) x ## y
// #define UNIQUEVARIABLE int PASTE(x,__LINE__)
//
// But this doesn't work since the specification of the token pasting operator is
//
// "If a formal parameter in a macro definition is preceded or followed by the token-pasting operator,
// the formal parameter is immediately replaced by the __unexpanded__ actual argument. Macro expansion
// is __not performed__ on the argument prior to replacement."
//
// So to actually have __LINE__ expanded we need another level of indirection
//
// #define PASTE(x,y) x ## y
// #define EXPAND_Y_AND_THEN_PASTE(x,y) PASTE(x,y)
// #define UNIQUEVARIABLE int EXPAND_Y_AND_THEN_PASTE(x,__LINE__)

#define KVI_TRACE_HACK_TOKENPASTE_2(x, y) x##y
#define KVI_TRACE_HACK_TOKENPASTE_1(x, y) KVI_TRACE_HACK_TOKENPASTE_2(x, y)

#ifdef __GNUC__
#define KVI_TRACE_FUNCTION \
	KviDebugContext KVI_TRACE_HACK_TOKENPASTE_1(ctx, __LINE__)(__PRETTY_FUNCTION__)

#define KVI_TRACE_BLOCK(_szBlockDescription) \
	KviDebugContext KVI_TRACE_HACK_TOKENPASTE_1(ctx, __LINE__)("%s - %s", __PRETTY_FUNCTION__, _szBlockDescription)

#define KVI_TRACE(_szFmt, arg...) KviDebugContext::trace(_szFmt, ##arg)

#else
#define KVI_TRACE_FUNCTION \
	KviDebugContext KVI_TRACE_HACK_TOKENPASTE_1(ctx, __LINE__)(__FUNCTION__)

#define KVI_TRACE_BLOCK(_szBlockDescription) \
	KviDebugContext KVI_TRACE_HACK_TOKENPASTE_1(ctx, __LINE__)("%s - %s", __FUNCTION__, _szBlockDescription)

#define KVI_TRACE(_szFmt, ...) KviDebugContext::trace(_szFmt, __VA_ARGS__)

#endif

#else //!COMPILE_DEBUG_MODE

#define KVI_ASSERT(condition)     \
	do                            \
	{                             \
	} while(0)
#define KVI_ASSERT_MSG(condition, message)         \
	do                                             \
	{                                              \
	} while(0)

#define KVI_TRACE_FUNCTION \
	do                     \
	{                      \
	} while(0)

#define KVI_TRACE_BLOCK(_szBlockDescription) \
	do                                       \
	{                                        \
	} while(0)

#ifdef __GNUC__
#define KVI_TRACE(_szFmt, arg...) \
	do                            \
	{                             \
	} while(0)
#else
#define KVI_TRACE(_szFmt, ...) \
	do                         \
	{                          \
	} while(0)
#endif

#endif //!COMPILE_DEBUG_MODE

#endif //_KVI_DEBUG_H_
