#ifndef _KVI_USERACTION_H_
#define _KVI_USERACTION_H_
//=============================================================================
//
//   File : KviUserAction.h
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

/**
* \file KviUserAction.h
* \author Szymon Stefanek
* \brief This file defines the user action "temperatures"
*
* The more "hot" is an action, the more humanity is in it
*
* \def KVI_USERACTION_ACTION The action 'action'
* \def KVI_USERACTION_PRIVMSG The action 'message' (channel message)
* \def KVI_USERACTION_NOTICE The action 'notice'
* \def KVI_USERACTION_TOPIC The action 'topic'
* \def KVI_USERACTION_KICK The action 'kick'
* \def KVI_USERACTION_NICK The action 'nick'
* \def KVI_USERACTION_CHANMODE The action 'mode' (channel mode)
* \def KVI_USERACTION_JOIN The action 'join'
* \def KVI_USERACTION_PART The action 'part'
* \def KVI_NUM_USERACTION_TYPES The number of actions' type
*
* These are the temperature levels for a single user on a channel
*
* \def KVI_MAX_TEMPERATURE Maximum: can't go above this
* \def KVI_HOT_TEMPERATURE Hot: human
* \def KVI_HALF_HOT_TEMPERATURE Half hot: signs of humanity
* \def KVI_MID_TEMPERATURE Mid: uncertain
* \def KVI_HALF_COLD_TEMPERATURE Half cold: automa ?
* \def KVI_COLD_TEMPERATURE Cold: bot
* \def KVI_MIN_TEMPERATURE Minimum: can't drop below this
*/

#include "kvi_settings.h"

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

#define KVI_MAX_TEMPERATURE 150
#define KVI_HOT_TEMPERATURE 100
#define KVI_HALF_HOT_TEMPERATURE 40
#define KVI_MID_TEMPERATURE 0
#define KVI_HALF_COLD_TEMPERATURE -40
#define KVI_COLD_TEMPERATURE -100
#define KVI_MIN_TEMPERATURE -150

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

extern KVIRC_API int kvi_getUserActionTemperature(unsigned int uIdx);

#endif // _KVI_USERACTION_H_
