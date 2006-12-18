#ifndef _KVI_BSWAP_H_
#define _KVI_BSWAP_H_

//=============================================================================
//
//   File : kvi_bswap.h
//   Creation date : Fri Mar 19 1999 03:15:21 CEST by Szymon Stefanek
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

#include "kvi_settings.h"

// Removed : kvi_settings.h includes qglobal.h for debug()
//#include <qglobal.h> // for Q_UINT* and Q_INT*

// KVILIB_API has been removed from therse two functions
// these should always go inlined

inline Q_UINT32 kvi_swap32(Q_UINT32 i)
{
	return ((i << 24) | ((i & 0xff00) << 8) | ((i >> 8) & 0xff00) | (i >> 24));
}

inline Q_UINT16 kvi_swap16(Q_UINT16 i)
{
	return ((i << 8) | (i >> 8));
}



#endif // !_KVI_BSWAP_H_
