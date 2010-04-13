#ifndef _KVI_DEBUG_H_
#define _KVI_DEBUG_H_

//=============================================================================
//
//   File : kvi_debug.h
//   Creation date : Fri Mar 19 1999 03:10:39 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2009 Szymon Stefanek (pragma at kvirc dot net)
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
//=============================================================================

#include <QtGlobal>

/**
* \file kvi_debug.h
* \author Szymon Stefanek
* \brief This file contains the definition of the debug macros;
*        You can enable ALL the debugging output by uncommenting the line that defines _KVI_DEBUG_CHECK_RANGE_
*/

//=============================================================================
//
// You can enable ALL the debugging output by uncommenting the next line
// #define _KVI_DEBUG_CHECK_RANGE_
//
//=============================================================================

/**
* \brief Debug macros
*
* \def __range_valid Assert that ensures that its parameter is true
* \def __range_invalid Assert that ensures that its parameter is false
* \def __ASSERT Assert that ensures that its parameter is true; enabled only if _KVI_DEBUG_or __KVI_DEBUG__ is defined
*/


#ifdef _KVI_DEBUG_CHECK_RANGE_
	#define __range_valid(_expr) if(!(_expr))debug("[kvirc]: ASSERT FAILED: \"%s\" is false in %s (%d)",#_expr,__FILE__,__LINE__)
	#define __range_invalid(_expr) if(_expr)debug("[kvirc]: ASSERT FAILED: \"%s\" is true in %s (%d)",#_expr,__FILE__,__LINE__)
#else
	#define __range_valid(_expr)
	#define __range_invalid(_expr)
#endif

#if defined(_KVI_DEBUG_) || defined(__KVI_DEBUG__)
	#define __ASSERT(_expr) if(!(_expr))debug("[kvirc]: ASSERT FAILED: \"%s\" is false in %s (%d)",#_expr,__FILE__,__LINE__)
#else
	#define __ASSERT(_expr)
#endif

#endif //_KVI_DEBUG_H_
