#ifndef _KVI_HTMLGENERATOR_H_
#define _KVI_HTMLGENERATOR_H_
//=============================================================================
//
//   File : kvi_htmlgenerator.h
//   Creation date : Mon 04 Jan 2010 22:09:18 by Fabio Bas
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2010 Fabio Bas < ctrlaltca at gmail dot com >
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

/**
* \file kvi_htmlgenerator.h
* \author Fabio Bas
* \brief Irc => Html format converter; this should better live in kdelibs, but uses kvirc options
*/

#include "kvi_settings.h"

/**
* \namespace KviHtmlGenerator
* \brief A namespace to handle html-related functions
*/
namespace KviHtmlGenerator
{
	/**
	* \brief Converts a raw irc message to its html equivalent text
	* \param szText const reference to a message text
	* \return QString
	*/
	extern KVIRC_API QString convertToHtml(const QString &szText);
};

#endif //_KVI_HTMLGENERATOR_H_