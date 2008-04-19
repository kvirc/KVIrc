//=============================================================================
//
//   File : kvi_buildinfo.cpp
//   Created on Sat 19 Apr 2008 17:01:57 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2008 Szymon Stefanek <s.stefanek at libero dot it>
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

#include "kvi_buildinfo.h"

#if defined(_OS_WIN32_) || defined(Q_OS_WIN32) || defined(Q_OS_WIN32_)
	#include "kvi_wincfg.h"
#else
	#include "kvi_sysbuildinfo.h"
#endif

namespace KviBuildInfo
{
	QString buildDate()
	{
		return QString(KVIRC_BUILD_DATE);
	}
};
