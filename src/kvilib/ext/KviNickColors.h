#ifndef _KVI_NICKCOLORS_H_
#define _KVI_NICKCOLORS_H_
//=============================================================================
//
//   File : KviNickColors.h
//   Creation date : Sun Apr 11 2010 15:00:01 CEST by Fabio Bas
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2009 Fabio Bas (ctrlaltca at libero dot it)
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

class QString;

namespace KviNickColors
{
	extern KVILIB_API int getSmartColorForNick(QString * szNick);

	extern KVILIB_API const char * getSmartColor(int iPos, bool bWithBg);

	extern KVILIB_API int getSmartColorIntByMircColor(unsigned char iFore, unsigned char iBack);
}
#endif // _KVI_NICKCOLORS_H_
