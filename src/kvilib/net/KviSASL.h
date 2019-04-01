#ifndef _KVI_SASL_H_
#define _KVI_SASL_H_
//=============================================================================
//
//   File : KviSASL.h
//   Creation date : Mon Feb 14 2010 19:36:12 CEST by Fabio Bas
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2010 Fabio Bas (ctrlaltca at gmail dot com)
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

class KviCString;
class QByteArray;
class QStringList;

/**
* \namespace KviSASL
* \brief This namespace implement some SASL authentication methods.
*
* Currently implementhed methods are PLAIN and EXTERNAL
*/

namespace KviSASL
{
	/**
	* \brief Returns a list of the supported SASL methods
	* \return QStringList
	*/
	extern KVILIB_API QStringList supportedMethods();

	/**
	* \brief Create the auth message for PLAIN authentication
	* \param szIn The server-provided token
	* \param szOut A KviCString that will be filled with the authentication message
	* \param baNick The username
	* \param baPass	The password
	* \return bool
	*/
	extern KVILIB_API bool plainMethod(const KviCString & szIn, KviCString & szOut, const QByteArray & baNick, const QByteArray & baPass);

	/**
	* \brief Create the auth message for EXTERNAL authentication
	* \param szIn The server-provided token
	* \param szOut A KviCString that will be filled with the authentication message
	* \return bool
	*/
	extern KVILIB_API bool externalMethod(const KviCString & szIn, KviCString & szOut);
};

#endif //_KVI_SASL_H_
