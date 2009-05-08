#ifndef _MP_XMMS2INTERFACE_H_
#define _MP_XMMS2INTERFACE_H_
//=============================================================================
//
//   File : mp_xmms2interface.h
//   Creation date : Thu 06 Dec 2007 14:20:02 by Elvio Basello
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2008 Elvio Basello <hellvis69 at netsons dot org>
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
#include "mp_mprisinterface.h"

#if (!defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MAC) && !defined(COMPILE_ON_MINGW))
	#include <QtDBus/QtDBus>

	class KviXmms2Interface : public KviMPRISInterface
	{
	public:
		KviXmms2Interface();
	};

	MP_DECLARE_DESCRIPTOR(KviXmms2Interface)

#endif //!COMPILE_ON_WINDOWS

#endif //!_MP_XMMS2INTERFACE_H_
