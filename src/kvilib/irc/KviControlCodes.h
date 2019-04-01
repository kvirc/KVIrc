#ifndef _KVI_CONTROLCODES_H_
#define _KVI_CONTROLCODES_H_
//=============================================================================
//
//   File : KviControlCodes.h
//   Creation date : Thu Jun 29 2000 21:06:55 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2011 Elvio Basello (hellvis69 at gmail dot com)
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

//
// This file contains non-customizable standards
//
// Better to not touch this :)
//

/**
* \file KviControlCodes.h
* \author Szymon Stefanek
* \brief This file holds the text control codes
*
* It was originally named KviMircCntrl
*/

#include "kvi_settings.h"
#include "KviCString.h"

#define KVI_MIRCCOLOR_MAX 15
#define KVI_EXTCOLOR_MAX 98

// ASCII Stuff: the following defines are meant to be escape sequences
//              that can go through an IRC connection

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
// 016 DLE Data link escape           (Decent, can be assigned)
// 017 DC1 Device control 1           (Good to be assigned)
// 018 DC2 Device control 2           (Good to be assigned)
// 019 DC3 Device control 3           (Good to be assigned)
// 020 DC4 Device control 4           (Good to be assigned)
// 021 NAK Negative acknowledge       (Not so good, but could be used as last resource)
// 022 SYN Synchronous idle           ( Reverse text )
// 023 ETB End of transmission block  (Decent, can be assigned)
// 024 CAN Cancel                     (Should not be assigned: terminal control)
// 025 EM  End of medium              (Decent, can be assigned)
// 026 SUB Substitute                 (Should not be assigned: terminal control)
// 027 ESC Escape                     (Should not be assigned: terminal control)
// 028 FS  File separator             (Excellent, should be used as first candidate)
// 029 GS  Group separator            ( ICONS Escape: beginning of a word )
// 030 RS  Record separator           ( CRYPT Escape: only beginning of the message )
// 031 US  Unit separator             ( Underline text )

//
// mIrc control codes
//
//31 (0001 1111) US (Unit separator)
//2  (0000 0010) STX (Start of text)
//15 (0000 1111) SI (Shift in)
//22 (0001 0110) SYN (Synchronous idle)
//3  (0000 0011) ETX (End of text)

// FIXME: Recent versions of mIrc use 29 as Italic text escape.
//        This collides with our icon escape and creates a backward compatibility problem if we want to follow mIrc.

//
// Irc control codes
//
//1 (0000 0001) SOH (Start of heading)

//
// KVirc added control codes
//
//30 (0001 1110) RS (Record separator)
//29 (0001 1101) GS (Group separator)

/**
* \namespace KviControlCodes
* \brief Holds all text control code KVIrc uses
*/
namespace KviControlCodes
{
	/**
	* \enum Color
	* \brief Contains mIRC color codes
	*/
	enum Color
	{
		White = 0,         /**< White */
		Black = 1,         /**< Black */
		DarkBlue = 2,      /**< Dark blue */
		DarkGreen = 3,     /**< Dark green */
		Red = 4,           /**< Red */
		DarkRed = 5,       /**< Dark red */
		DarkViolet = 6,    /**< Dark violet */
		Orange = 7,        /**< Orange */
		Yellow = 8,        /**< Yellow */
		LightGreen = 9,    /**< Light green */
		BlueMarine = 10,   /**< Blue marine */
		LightBlue = 11,    /**< Light blue */
		Blue = 12,         /**< Blue */
		LightViolet = 13,  /**< Light violet */
		DarkGray = 14,     /**< Dark gray */
		LightGray = 15,    /**< Light gray */
		Transparent = 100, /**< Transparent, non standard color code for KviIrcView */
		NoChange = 101     /**< No change, non standard color code for KviIrcView */
	};

	/**
	* \enum Control
	* \brief Contains mIRC control codes
	*/
	enum Control
	{
		CTCP = 0x01,        /**< CTCP, IRC control code */
		Bold = 0x02,        /**< Bold */
		Color = 0x03,       /**< Color */
		Escape = 0x04,      /**< Escape, totally artificial and internal to KviIrcView */
		UnEscape = 0x05,    /**< Unescape, totally artificial and internal to KviIrcView */
		UnIcon = 0x06,      /**< Unicon, totally artificial and internal to KviIrcView */
		ArbitraryBreak = UnIcon, /**< Arbitrary block break, totally artificial and internal to KviIrcView */
		Reset = 0x0f,       /**< Reset */
		Reverse = 0x16,     /**< Reverse */
		Icon = 0x1c,        /**< Icon, KVIrc control code */
		Italic = 0x1d,      /**< Italic */
		CryptEscape = 0x1e, /**< Crypt escape, KVIrc control code */
		Underline = 0x1f    /**< Underline */
	};

	/**
	* \brief Removes control bytes from the given string
	* \param szData The string to clean
	* \return QString
	*/
	KVILIB_API QString stripControlBytes(const QString & szData);

	KVILIB_API const kvi_wchar_t * getColorBytesW(const kvi_wchar_t * pwData, unsigned char * pcByte1, unsigned char * pcByte2);

	KVILIB_API unsigned int getUnicodeColorBytes(const QString & szData, unsigned int iChar, unsigned char * pcByte1, unsigned char * pcByte2);
#if 0
	extern KVILIB_API const char * getColorBytes(const char * pcData, unsigned char * pcByte1, unsigned char * pcByte2);
	inline const QChar * getUnicodeColorBytes(const QChar * pData, unsigned char * pcByte1, unsigned char * pcByte2)
		{ return (QChar *)getColorBytesW((const kvi_wchar_t *)pData,pcByte1,pcByte2); }
#endif

	KVILIB_API kvi_u32_t getExtendedColor(int index);
}

#endif //_KVI_CONTROLCODES_H_
