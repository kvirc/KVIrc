#ifndef _KVI_USERACTION_H_
#define _KVI_USERACTION_H_

//=============================================================================
//
//   File : kvi_userlistview.h
//   Creation date : Tue Mar 18 2003 13:36:12 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2003 Szymon Stefanek (pragma at kvirc dot net)
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

// This file defines the user action "temperatures".
// The more "hot" is an action, the more humanity is in it

#define KVI_USERACTION_ACTION 0
#define KVI_USERACTION_PRIVMSG 1
#define KVI_USERACTION_NOTICE 2
#define KVI_USERACTION_TOPIC 3
#define KVI_USERACTION_KICK 4
#define KVI_USERACTION_NICK 5
#define KVI_USERACTION_CHANMODE 6
#define KVI_USERACTION_JOIN 7
#define KVI_USERACTION_PART 8

#define KVI_NUM_USERACTION_TYPES 9


extern KVIRC_API int kvi_getUserActionTemperature(unsigned int uIdx);


//
//  ACTION     30
//  NOTICE     25
//  PRIVMSG    25
//  TOPIC      20
//  KICK       10
//  OP         5
//  VOICE      5
//  DEOP      -5
//  DEVOICE   -5
//  UNBAN     -15
//  BAN       -20
//  CHANMODE  -25
//  NICK      -30
//  JOIN      -30
//

// These are the temperature levels for a single user on a channel

// Maximum : can't go above this
#define KVI_MAX_TEMPERATURE 150
// Hot : human
#define KVI_HOT_TEMPERATURE 100
// Half hot : signs of humanity
#define KVI_HALF_HOT_TEMPERATURE 40
// Mid : uncertain
#define KVI_MID_TEMPERATURE 0
// Half cold : automa ?
#define KVI_HALF_COLD_TEMPERATURE -40
// Cold : bot
#define KVI_COLD_TEMPERATURE -100
// Minimum : can't drop below this
#define KVI_MIN_TEMPERATURE -150

#endif // _KVI_USERACTION_H_
