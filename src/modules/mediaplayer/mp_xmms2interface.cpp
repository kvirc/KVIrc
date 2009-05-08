//=============================================================================
//
//   File : mp_xmms2interface.cpp
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

#include "mp_xmms2interface.h"
#if (!defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MAC) && !defined(COMPILE_ON_MINGW))
#include "kvi_locale.h"

MP_IMPLEMENT_DESCRIPTOR(
	KviXmms2Interface,
	"xmms2",
	__tr2qs_ctx(
		"An interface to the xmms2 media player.\n" \
		"Download it from http://wiki.xmms2.xmms.se/index.php/Main_Page\n",
		"mediaplayer"
	)
)

KviXmms2Interface::KviXmms2Interface()
: KviMPRISInterface()
{
	m_szServiceName = "org.mpris.xmms2";
}

#endif //!COMPILE_ON_WINDOWS
