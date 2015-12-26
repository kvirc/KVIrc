#ifndef _KVI_USERINPUT_H_
#define _KVI_USERINPUT_H_
//=============================================================================
//
//   File : KviUserInput.h
//   Creation date : Sun 25 Sep 2005 05:27:57 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2005-2010 Szymon Stefanek <pragma at kvirc dot net>
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
* \file KviUserInput.h
* \author Szymon Stefanek
* \brief User input parsing
*/

#include "kvi_settings.h"
#include "KviQString.h"
#include "KviWindow.h"

/**
* \namespace KviUserInput
* \brief Namespace that contains parsing function for the user input
*/
namespace KviUserInput
{
	/**
	* \brief Returns false if szData is a command and it fails to run
	* \param szData The input data to parse
	* \param pWindow The window associated to the input
	* \param szContext The context associated to the input
	* \param bUserFriendlyCommandline Whether the commandline is in user friendly mode
	* \return bool
	* \warning May destroy szData
	*/
	KVIRC_API bool parse(QString & szData, KviWindow * pWindow, const QString & szContext = KviQString::Empty, bool bUserFriendlyCommandline = false);

	/**
	* \brief Returns true if the command run
	* \param szData The input data to parse
	* \param pWindow The window associated to the input
	* \param szContext The context associated to the input
	* \param bUserFriendlyCommandline Whether the commandline is in user friendly mode
	* \return bool
	*/
	KVIRC_API bool parseCommand(const QString & szData, KviWindow * pWindow, const QString & szContext = KviQString::Empty, bool bUserFriendlyCommandline = false);

	/**
	* \brief Parses the non command input data
	* \param szData The input data to parse
	* \param pWindow The window associated to the input
	* \return void
	*/
	KVIRC_API void parseNonCommand(QString & szData, KviWindow * pWindow);

	/**
	* \brief Escapes any kvs special character from a string
	* \param szData The string to escape
	* \return void
	*/
	KVIRC_API void escapeString(QString & szData);

	//bool parseCommandWithSingleArgument(const QString & szData, KviWindow * pWindow, const QString & szContext = KviQString::Empty);
}

#endif //_KVI_USERINPUT_H_
