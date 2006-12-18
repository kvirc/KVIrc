#ifndef _KVI_OSINFO_H_
#define _KVI_OSINFO_H_

///////////////////////////////////////////////////////////////////////////////
//
//   File : kvi_osinfo.h
//   Creation date : 19  Jan 2006 GMT by Alexey Uzhva
//
//   This toolbar is part of the KVirc irc client distribution
//   Copyright (C) 2006 Alexey Uzhva
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
///////////////////////////////////////////////////////////////////////////////

#include "kvi_settings.h"

#include <qstring.h>

namespace KviOsInfo
{
	extern KVILIB_API QString type();
	extern KVILIB_API QString name();
	extern KVILIB_API QString version();
	extern KVILIB_API QString release();
	extern KVILIB_API QString machine();
	extern KVILIB_API QString nodename();
	extern KVILIB_API QString hostname();
};

#endif