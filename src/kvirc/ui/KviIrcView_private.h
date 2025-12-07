#ifndef _KVI_IRCVIEWPRIVATE_H_
#define _KVI_IRCVIEWPRIVATE_H_
//=============================================================================
//
//   File : KviIrcView_private.h
//   Creation date : Sat Oct 9 2004 16:29:01 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"

#include <QString>

//
// Internal data structures
//

// Force the structs to be packed...
#ifdef COMPILE_ON_WINDOWS
#pragma pack(push, old_packing, 1)
#define _KVI_PACKED
#else //!COMPILE_ON_WIDNOWS
// this works only on gcc
#ifdef __GNUC__
#define _KVI_PACKED __attribute__((__packed__))
#else
#define _KVI_PACKED
#endif
#endif //!COMPILE_ON_WINDOWS

// Borders...just do not set it to 0
#define KVI_IRCVIEW_HORIZONTAL_BORDER 4
#define KVI_IRCVIEW_VERTICAL_BORDER 4

//
// The LineChunk structure contains information about
// an attribute change, an icon or a link
//
// type can be one of:
//
//  KVI_TEXT_ICON:
//     the uIconId member is the icon to be shown
//     szPayload contains the text that triggered this icon was triggered by
//  KVI_TEXT_UNICON:
//     the text block after an icon
//  KVI_TEXT_ESCAPE:
//     szPayload contains the encoded escape command
//     colors.fore contains the new text color
//  KVI_TEXT_UNESCAPE:
//     the text block after an escape
//  KVI_TEXT_COLOR:
//     the colors.back and colors.fore members indicate the color change
//  KVI_TEXT_BOLD:
//     toggles the bold flag
//  KVI_TEXT_UNDERLINE:
//     toggles the underline flag
//  KVI_TEXT_REVERSE:
//     inverts the current fore and back colors
//  KVI_TEXT_RESET:
//     resets the color, bold and underline flags
//

struct KviIrcViewLineChunk
{
	unsigned char type;      // chunk type
	int iTextStart;          // index in the szText string of the beginning of the block
	int iTextLen;            // length in chars of the block (excluding the terminator)
	kvi_wchar_t * szPayload; // KVI_TEXT_ESCAPE attribute command buffer and KVI_TEXT_ICON icon name (non zeroed for other attributes!!!)
	kvi_wchar_t * szSmileId;
	struct
	{
		unsigned char back; // optional background color for KVI_TEXT_COLOR attribute
		unsigned char fore; // optional foreground color for KVI_TEXT_COLOR attribute (used also for KVI_TEXT_ESCAPE!!!)
	} _KVI_PACKED colors;   // anonymous
	                        //	QColor customBack;
	QColor customFore;
};

//
// The wrapped paintable data block
//

struct KviIrcViewWrappedBlock
{
	KviIrcViewLineChunk * pChunk; // pointer to real line chunk or 0 for word wraps
	int block_start;              // this is generally different than pAttribute->block_idx!
	int block_len;                // length if the block in characters
	int block_width;              // width of the block in pixels
} _KVI_PACKED;

struct KviIrcViewLine
{
	// this is a text line in the IrcView's memory
	unsigned int uIndex; // index of the text line (needed for find and splitting)
	QString szText;      // data string without color codes nor escapes...
	int iMsgType;        // type of the line (defines icon and colors)

	// At line insert time the szData text is split in parts which
	// signal attribute changes (or icons)
	unsigned int uChunkCount;      // number of allocated chunks
	KviIrcViewLineChunk * pChunks; // pointer to the allocated structures

	// At paint time the data is re-split in drawable chunks which
	// are either real data chunks or line wraps.
	// The algorightm that does this is lazy and computes it
	// only once for a given widget width (iMaxLineWidth)
	unsigned int uLineWraps;          // number of line wraps (lines - 1)
	int iMaxLineWidth;                // width that the blocks were calculated for (lazy calculation)
	int iBlockCount;                  // number of allocated paintable blocks
	KviIrcViewWrappedBlock * pBlocks; // pointer to the re-split paintable blocks

	// next and previous line
	KviIrcViewLine * pPrev;
	KviIrcViewLine * pNext;
};

struct KviIrcViewWrappedBlockSelectionInfo
{
	int selection_type;
	int part_1_length;
	int part_1_width;
	int part_2_length;
	int part_2_width;
	int part_3_length;
	int part_3_width;
};

#ifdef COMPILE_ON_WINDOWS
#pragma pack(pop, old_packing)
#else //!COMPILE_ON_WINDOWS
#undef _KVI_PACKED
#endif //!COMPILE_ON_WINDOWS

//
// Screen layout
//

//FIRST LINE (prev_line = 0) <---m_pFirstLine
//LINE
//--------------------SCREEN--
//LINE
//LINE
//LINE
//LINE <-------------------------m_pCurLine
//--------------------SCREEN--
//LAST LINE (next_line = 0) <----m_pLastLine

#endif //!_KVI_IRCVIEWPRIVATE_H_
