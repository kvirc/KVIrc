#ifndef _NOTIFIER_SETTINGS_H_
#define _NOTIFIER_SETTINGS_H_
//=============================================================================
//
//   File : NotifierSettings.h
//   Creation date : Thu 30 Dev 2004 21:21:33 by Iacopo Palazzi
//
//   This file is part of the Notifier - KVIrc Distribution distribution
//   Copyright (C) 2004-2008 Iacopo Palazzi < iakko(at)siena(dot)linux(dot)it >
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

// Global settings for KviNotifier

#define WDG_MIN_WIDTH 370
#define WDG_MIN_HEIGHT 160
#define WDG_BORDER_THICKNESS 5
#define SPACING 2

#define WDG_ICON_OUT 0
#define WDG_ICON_OVER 1
#define WDG_ICON_CLICKED 2
#define WDG_ICON_ON 3
#define WDG_ICON_OFF 4

#define NTF_TABS_FONT_BASELINE 8

#define MAX_MESSAGES_IN_WINDOW 20
// keep hidden messages for 600 seconds : 10 min
#define MAX_MESSAGE_LIFETIME 600

#define OPACITY_STEP 0.07

#define NUM_OLD_COLORS 6

#define WDG_UPSX 1
#define WDG_UP 2
#define WDG_UPDX 3
#define WDG_DWNSX 4
#define WDG_DWN 5
#define WDG_DWNDX 6
#define WDG_SX 7
#define WDG_DX 8

#define m_mac_bkgColor QColor(236, 233, 216) // Light-brown color of notifier background widget

enum State
{
	Hidden,
	Showing,
	Visible,
	Hiding,
	FocusingOff,
	FocusingOn
};
enum TabState
{
	Normal,
	Highlighted,
	Changed
};

#endif //_NOTIFIER_SETTINGS_H_
