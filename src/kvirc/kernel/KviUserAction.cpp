//=============================================================================
//
//   File : KviUserAction.cpp
//   Creation date : Tue Mar 18 2003 13:36:12 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviUserAction.h"

static int useraction_table[KVI_NUM_USERACTION_TYPES] = {
	25,  // ACTION (typically human)
	25,  // PRIVMSG
	25,  // NOTICE
	20,  // TOPIC
	5,   // KICK
	-5,  // NICK
	-20, // CHANMODE
	-25, // JOIN
	-25  // PART (this works only for channels and not for single users on channels)
};

int kvi_getUserActionTemperature(unsigned int uIdx)
{
	return useraction_table[uIdx];
}
