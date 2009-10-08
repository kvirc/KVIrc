//===========================================================================
//
//   File : kvi_hstrEqualCIN.h
//   Creation date : Wed Oct 1 2008 17:18:20 by Fabio Bas
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2008 Fabio Bas (ctrlaltca at gmail dot com)
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
//===========================================================================
//
// This file was originally part of kvi_ircview.h
//
//===========================================================================

/**
* \file kvi_hstrEqualCIN.h
* \author Fabio Bas
* \brief Comparison function kvi_hstrEqualCIN
*/

#include "kvi_string.h"

/**
* \brief Returns true if the string are equals casa insensitive
* \param pcStr1 The source string
* \param pcStr2 The target string
* \param iLen The length of the string
* \return bool
*/
__KVI_EXTERN KVILIB_API bool kvi_hstrEqualCIN(const kvi_wchar_t * pcStr1, const char * pcStr2, int iLen);
