#ifndef _KVI_MIRCCNTRL_H_
#define _KVI_MIRCCNTRL_H_

//=============================================================================
//
//   File : kvi_mirccntrl.h
//   Creation date : Thu Jun 29 2000 21:06:55 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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

//=============================================================================
// This file contains non-customizable standards
//
// Better do not touch this :)
//=============================================================================

#include "kvi_settings.h"
#include "kvi_string.h"

//
// mIrc color codes
//

#define KVI_WHITE 0
#define KVI_BLACK 1
#define KVI_DARKBLUE 2
#define KVI_DARKGREEN 3
#define KVI_RED 4
#define KVI_DARKRED 5
#define KVI_DARKVIOLET 6
#define KVI_ORANGE 7
#define KVI_YELLOW 8
#define KVI_LIGHTGREEN 9
#define KVI_BLUEMARINE 10
#define KVI_LIGHTBLUE 11
#define KVI_BLUE 12
#define KVI_LIGHTVIOLET 13
#define KVI_DARKGRAY 14
#define KVI_LIGHTGRAY 15

#define KVI_MIRCCOLOR_MAX_FOREGROUND 15
#define KVI_MIRCCOLOR_MAX_BACKGROUND 15

//
// Non-standard color codes for KviIrcView
//
#define KVI_TRANSPARENT 100
#define KVI_NOCHANGE 101
//
// Internal control codes for KviIrcView
//
// (Totally artificial and internal to KviIrcView)
#define KVI_TEXT_ESCAPE 0x04
#define KVI_TEXT_UNESCAPE 0x05
#define KVI_TEXT_UNICON 0x06
//#define KVI_TEXT_EMOTICON 0x07

// ASCII Stuff: the following defines are meant to be escape sequences
//              that can go thru an IRC connection

// The following table is a 30-minute analysis of the escape characters commonly used over the IRC protocol...
// created when looking for a good placement for the CRYPT escape char in KVirc.
// I guess that the best chars to be used were FS,GS,RS,US,DC1,DC2,DC3,DC4...they have the "less defined"
// meaning as ASCII control chars.
// mmmh... :) 

// ASCII                              IRC Meaning
// 000 NUL Null                       (Cannot be assigned)
// 001 SOH Start of heading           ( CTCP Escape: only beginning of the message )
// 002 STX Start of text              ( Bold text )
// 003 ETX End of text                ( Color text escape sequence )
// 004 EOT End of transmission        ( Assigned internally (escape) )
// 005 ENQ Enquiry (WRU: Who are you) ( Assigned internally (unescape) )
// 006 ACK Acknowledge                (Not so good, but can be used as last resource)
// 007 BEL Bell                       ( Recognized as bell by terminals and IRCII ) (Used also by some IRC servers)
// 008 BS  Backspace                  (Should not be assigned: terminal control)
// 009 HT  Horizontal tabulation      (Should not be assigned: terminal control)
// 010 LF  Line feed                  (Should not be assigned: terminal control)
// 011 VT  Vertical tabulation        (Should not be assigned: terminal control)
// 012 FF  Form feed                  (Should not be assigned: terminal control)
// 013 CR  Carriage return            (Should not be assigned: terminal control)
// 014 SO  Shift out                  (Should not be assigned: terminal control)
// 015 SI  Shift in                   ( Resets Bold,Color,Underline and Reverse ) (Conflicting with terminal control)
// 016 DLE Data link escape           (Decent , can be assigned)
// 017 DC1 Device control 1           (Good to be assigned)
// 018 DC2 Device control 2           (Good to be assigned)
// 019 DC3 Device control 3           (Good to be assigned)
// 020 DC4 Device control 4           (Good to be assigned)
// 021 NAK Negative acknowledge       (Not so good, but could be used as last resource)
// 022 SYN Synchronous idle           ( Reverse text )
// 023 ETB End of transmission block  (Decent , can be assigned)
// 024 CAN Cancel                     (Should not be assigned: terminal control)
// 025 EM  End of medium              (Decent , can be assigned)
// 026 SUB Substitute                 (Should not be assigned: terminal control)
// 027 ESC Escape                     (Should not be assigned: terminal control)
// 028 FS  File separator             (Excellent , should be used as first candidate)
// 029 GS  Group separator            ( ICONS Escape: beginning of a word )
// 030 RS  Record separator           ( CRYPT Escape: only beginning of the message )
// 031 US  Unit separator             ( Underline text )


//
// mIrc control codes
//

//31 (0001 1111) US (Unit separator)
#define KVI_TEXT_UNDERLINE 0x1f
//2  (0000 0010) STX (Start of text)
#define KVI_TEXT_BOLD 0x02
//15 (0000 1111) SI (Shift in)
#define KVI_TEXT_RESET 0x0f
//22 (0001 0110) SYN (Synchronous idle)
#define KVI_TEXT_REVERSE 0x16
//3  (0000 0011) ETX (End of text)
#define KVI_TEXT_COLOR 0x03

//
// Irc control codes
//
//1 (0000 0001) SOH (Start of heading)
#define KVI_TEXT_CTCP 0x01

//
// KVirc added control codes
//
//30 (0001 1110) RS (Record separator)
#define KVI_TEXT_CRYPTESCAPE 0x1e
//29 (0001 1101) GS (Group separator)
#define KVI_TEXT_ICON 0x1d

#ifndef _KVI_MIRCCNTRL_CPP_
	extern KVILIB_API const char * getColorBytes(const char *data_ptr,unsigned char *byte_1,unsigned char *byte_2);
	extern KVILIB_API const kvi_wchar_t * getColorBytesW(const kvi_wchar_t *data_ptr,unsigned char *byte_1,unsigned char *byte_2);
	extern KVILIB_API unsigned int getUnicodeColorBytes(const QString &szData,unsigned int charIdx,unsigned char *byte_1,unsigned char *byte_2);
	inline const QChar * getUnicodeColorBytes(const QChar *pData,unsigned char *byte_1,unsigned char *byte_2)
		{ return (QChar *)getColorBytesW((const kvi_wchar_t *)pData,byte_1,byte_2); }

#endif

namespace KviMircCntrl
{
	KVILIB_API QString stripControlBytes(const QString &szData);
}


#endif //_KVI_MIRCCNTRL_H_
