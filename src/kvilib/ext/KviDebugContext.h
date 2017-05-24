#ifndef _KVIDEBUGCONTEXT_H_
#define _KVIDEBUGCONTEXT_H_
//=============================================================================
//
//   File : KviDebugContext.h
//   Creation date : Tue Jul 13 2010 23:27:23 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2010 Szymon Stefanek (pragma at kvirc dot net)
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
* \file KviDebugContext.h
* \author Szymon Stefanek
* \brief Debug context
*/

#include "kvi_settings.h"
#include "KviCString.h"

/**
* \class KviDebugContext
* \brief The KviDebugContext class
*
* This class...
*/
class KVILIB_API KviDebugContext
{
public:
	/**
	* \brief Creates an instance of KviDebugContext
	* \param pcContext  The format list argument
	* \param ... The succeeding arguments
	* \return KviDebugContext
	*/
	KviDebugContext(const char * pcContext, ...);

	/**
	* \brief Destroys the instance of KviDebugContext and frees all the relevant resources
	*/
	~KviDebugContext();

private:
	KviCString m_szContext;

public:
	/**
	* \brief
	* \param pcFmt The format list argument
	* \param ... The succeeding arguments
	* \return void
	*/
	static void trace(const char * pcFmt, ...);

private:
	const KviDebugContext & operator=(const KviDebugContext &) { return *this; };
};

#endif // _KVIDEBUGCONTEXT_H_
