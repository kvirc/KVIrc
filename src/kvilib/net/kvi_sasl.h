#ifndef _KVI_SASL_H_
#define _KVI_SASL_H_
//=============================================================================
//
//   File : kvi_sasl.h
//   Creation date : Mon Feb 14 2010 19:36:12 CEST by Fabio Bas
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2010 Fabio Bas (ctrlaltca at gmail dot com)
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

#include "kvi_settings.h"
#include "kvi_string.h"

/**
* \namespace KviSASL
* \brief This namespace implement some SASL authentication methods.
*
* Currently implementhed methods are PLAIN and DH-BLOWFISH
*/

namespace KviSASL
{
	/**
	* \brief Create the auth message for PLAIN authentication
	* \param szIn The server-provided token
	* \param szOut A KviStr that will be filled with the authentication message
	* \param baNick The username
	* \param baPass	The password
	* \return bool 
	*/
	extern KVILIB_API bool plainMethod(KviStr & szIn, KviStr & szOut, QByteArray & baNick, QByteArray & baPass);

	/**
	* \brief Create the auth message for DH-BLOWFISH authentication
	* \param szIn The server-provided token
	* \param szOut A KviStr that will be filled with the authentication message
	* \param baNick The username
	* \param baPass	The password
	* \return bool
	*/
	extern KVILIB_API bool dh_blowfishMethod(KviStr & szIn, KviStr & szOut, QByteArray & baNick, QByteArray & baPass);

};

#endif //_KVI_SASL_H_
