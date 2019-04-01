//===========================================================================
//
//   File : KviIrcView_getTextLine.cpp
//   Creation date : Wed Oct 1 2008 17:18:20 by Fabio Bas
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2008 Fabio Bas (ctrlaltca at gmail dot com)
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
//===========================================================================

//
// This file was originally part of KviIrcView.cpp
//


#include "KviChannelWindow.h"
#include "KviIrcView.h"
#include "KviIrcView_private.h"
#include "KviKvsEventTriggers.h"
#include "KviMemory.h"
#include "KviControlCodes.h"
#include "KviOptions.h"
#include "kvi_out.h"
#include "KviTextIconManager.h"
#include "KviWindow.h"

#include <QDateTime>
#include <QChar>

#define WSTRINGCONFIG_SAFE_TO_MEMCPY_QCHAR 1

void kvi_appendWCharToQStringWithLength(QString * qstrptr, const kvi_wchar_t * ptr, kvi_wslen_t len)
{
	KVI_ASSERT(qstrptr);
	kvi_wslen_t oldLen = qstrptr->length();
	qstrptr->resize(oldLen + len);
#ifdef WSTRINGCONFIG_SAFE_TO_MEMCPY_QCHAR
	KviMemory::copy((void *)(qstrptr->unicode() + oldLen), ptr, sizeof(kvi_wchar_t) * len);
#else  // !WSTRINGCONFIG_SAFE_TO_MEMCPY_QCHAR
	QChar * c = (QChar *)(qstrptr->unicode() + oldLen);
	while(*ptr)
	{
		c->unicode() = *ptr;
		ptr++;
		c++;
	}
#endif // !WSTRINGCONFIG_SAFE_TO_MEMCPY_QCHAR
}

static const kvi_wchar_t * skip_to_end_of_url(const kvi_wchar_t * p)
{
	// p here points somewhere inside an URL.

	// Now the question is what characters are and which aren't allowed inside an URL.

	// RFC1738 says that:
	//
	// ...only alphanumerics, the special characters "$-_.+!*'(),", and reserved characters [...] "/:?@&=;" may be used unencoded within a URL...

	// However, in many cases this isn't respected. Since the browsers will automatically encode the invalid characters
	// then the users will tend to write the links without the special encoding. ed2k links also use the | character
	// and it's common for the file names to appear partially unencoded.

	// There is also a very common case of URLs being enclosed inside parentheses: (http://url.here).
	// In this case the rightmost ')' is shouldn't be included in the URL. On the other hand there are many links that
	// actually contain the ')' character and have it exactly at the end. Wikipedia, for instance, has a lot of such links.

	// So in the end, we just can't have an algorithm that pleases everybody. If we follow exactly the RFC1738
	// then some links that would actually work inside a browser will not be recognized as links and some other links that
	// a human would recognize being inside parentheses will contain too many chars.

	// The following algorithm, then, is a compromise that works in most cases.

	int iSquareParenthesisLevel = 0;
	int iRoundParenthesisLevel = 0;

	for(;;)
	{
		if(*p <= 32)
			return p; // no spaces and control characters below 32

		if((*p == '{') || (*p == '}') || (*p == '<') || (*p == '>') || (*p == '"')) // || (*p == '\''))
			return p;                                                               // never valid inside an URL

		if(*p == '[')
		{
			iSquareParenthesisLevel++;
			p++;
			continue;
		}

		if(*p == ']')
		{
			if(iSquareParenthesisLevel <= 0)
				return p;
			// balanced square parenthesis
			iSquareParenthesisLevel--;
			p++;
			continue;
		}

		if(*p == '(')
		{
			iRoundParenthesisLevel++;
			p++;
			continue;
		}

		if(*p == ')')
		{
			if(iRoundParenthesisLevel <= 0)
				return p;
			// balanced round parenthesis
			iRoundParenthesisLevel--;
			p++;
			continue;
		}

		p++;
	}

	return p;
}

static inline bool url_compare_helper(const kvi_wchar_t * pData1, const kvi_wchar_t * pData2, int iData2Len)
{
	KVI_ASSERT(pData1);
	KVI_ASSERT(pData2);

	// pData1 is null-terminated and may be shorter than pData2
	// pData2 is NOT null terminated and is lower case

	const kvi_wchar_t * pData2End = pData2 + iData2Len;

	while(pData2 < pData2End)
	{
		if(!*pData1)
			return false; // not equal
		if(QChar::toLower(*pData1) != *pData2)
			return false; // not equal
		pData1++;
		pData2++;
	}
	return true; // all equal up to iData2Len
}

const kvi_wchar_t * KviIrcView::getTextLine(
		int iMsgType,
		const kvi_wchar_t * data_ptr,
		KviIrcViewLine * line_ptr,
		bool bEnableTimeStamp,
		const QDateTime & datetime_param
	)
{
	const kvi_wchar_t * pUnEscapeAt = nullptr;

	// Splits the text data in lines (separated by '\n')

	// NOTE: This function may be NOT reentrant
	//   ... no function in this file is supposed to be thread safe anyway

	int iTextIdx = 0; //we're at the beginning in the buffer
	int iCurChunk = 0;
	int blockLen;

	const kvi_wchar_t * p = data_ptr;

	//Alloc the first attribute
	line_ptr->uChunkCount = 1;
	line_ptr->pChunks = (KviIrcViewLineChunk *)KviMemory::allocate(sizeof(KviIrcViewLineChunk));
	//And fill it up
	line_ptr->pChunks[0].type = KviControlCodes::Color;
	line_ptr->pChunks[0].iTextStart = 0;
	line_ptr->pChunks[0].colors.back = KVI_OPTION_MSGTYPE(iMsgType).back();
	line_ptr->pChunks[0].colors.fore = KVI_OPTION_MSGTYPE(iMsgType).fore();
	//line_ptr->pChunks[0].customFore = QColor();

	// print a nice timestamp at the begin of the first line
	if(bEnableTimeStamp && KVI_OPTION_BOOL(KviOption_boolIrcViewTimestamp))
	{
		QString szTimestamp;
		QDateTime datetime = datetime_param;
		if(!datetime.isValid())
			datetime = QDateTime::currentDateTime();
		datetime = datetime.toTimeSpec(KVI_OPTION_BOOL(KviOption_boolIrcViewTimestampUTC) ? Qt::UTC : Qt::LocalTime);
		szTimestamp = datetime.toString(KVI_OPTION_STRING(KviOption_stringIrcViewTimestampFormat));
		szTimestamp.append(' ');
		int iTimeStampLength = szTimestamp.length();

		if(KVI_OPTION_BOOL(KviOption_boolUseSpecialColorForTimestamp))
		{
			// we need three chunks: the first one uses the default colors
			// for the message type, the second one the special colors
			// of the timestamp and the third one goes back to the defaults
			line_ptr->pChunks[0].iTextLen = 0;

			line_ptr->uChunkCount = 3;
			line_ptr->pChunks = (KviIrcViewLineChunk *)KviMemory::reallocate((void *)line_ptr->pChunks, 3 * sizeof(KviIrcViewLineChunk));

			line_ptr->pChunks[1].type = KviControlCodes::Color;
			line_ptr->pChunks[1].iTextStart = 0;
			line_ptr->pChunks[1].iTextLen = iTimeStampLength - 1;
			line_ptr->pChunks[1].colors.back = KVI_OPTION_UINT(KviOption_uintTimeStampBackground);
			line_ptr->pChunks[1].colors.fore = KVI_OPTION_UINT(KviOption_uintTimeStampForeground);

			line_ptr->pChunks[2].type = KviControlCodes::Color;
			line_ptr->pChunks[2].iTextStart = iTimeStampLength - 1;
			line_ptr->pChunks[2].iTextLen = 1;
			line_ptr->pChunks[2].colors.back = KVI_OPTION_MSGTYPE(iMsgType).back();
			line_ptr->pChunks[2].colors.fore = KVI_OPTION_MSGTYPE(iMsgType).fore();
			//line_ptr->pChunks[2].customFore = QColor();
			iCurChunk += 2;
		}
		else
		{
			// only one chunk
			line_ptr->pChunks[0].iTextLen = iTimeStampLength;
		}

		// We need the timestamp string to be added
		// alloc the necessary space
		line_ptr->szText.resize(iTimeStampLength);

		iTextIdx = iTimeStampLength; // the rest of the string will begin 11 chars later

		// throw away const: we WANT to set the chars :D
		QChar * data_ptr_aux = (QChar *)line_ptr->szText.unicode();
		QChar * stamp_ptr_aux = (QChar *)szTimestamp.unicode();

		//copy the timestamp into line_ptr->szText.unicode()
		for(int i = 0; i < iTimeStampLength; i++)
			*data_ptr_aux++ = *stamp_ptr_aux++;
	}
	else
	{
		// Timestamp not needed... but we don't want null strings floating around
		line_ptr->szText = "";
		line_ptr->pChunks[0].iTextLen = 0;
	}

	//
	// Ok... a couple of macros that occur really frequently
	// in the following code...
	// these could work well as functions too...but the macros are a lot faster :)
	//
	
	/*
	 * Profane description: this adds a block of text of known length to a already created chunk inside this line.
	 */
	#define APPEND_LAST_TEXT_BLOCK(__data_ptr, __data_len)                               \
		blockLen = (__data_len);                                                         \
		line_ptr->pChunks[iCurChunk].iTextLen += blockLen;                               \
		kvi_appendWCharToQStringWithLength(&(line_ptr->szText), __data_ptr, __data_len); \
		iTextIdx += blockLen;
	
	/*
	 * Profane description: this adds a block of text of known length to a already created chunk inside this line.
	 * text is hidden (e.g. we want to display an emoticon instead of the ":)" text, so we insert it hidden)
	 */
	
	#define APPEND_LAST_TEXT_BLOCK_HIDDEN_FROM_NOW(__data_ptr, __data_len)               \
		blockLen = (__data_len);                                                         \
		kvi_appendWCharToQStringWithLength(&(line_ptr->szText), __data_ptr, __data_len); \
		iTextIdx += blockLen;
	
	/*
	 * Profane description: this is dummy
	 */
	
	#define APPEND_ZERO_LENGTH_BLOCK(__data_ptr) /* does nothing */
	
	/*
	 * Profane description: this adds a new chunk to the current line of the specified type. A chunk is a block of text
	 * with similar style properties (mainly with the same color)
	 */
	
	#define NEW_LINE_CHUNK(_chunk_type)                                                             \
		line_ptr->uChunkCount++;                                                                    \
		line_ptr->pChunks = (KviIrcViewLineChunk *)KviMemory::reallocate((void *)line_ptr->pChunks, \
		    line_ptr->uChunkCount * sizeof(KviIrcViewLineChunk));                                   \
		iCurChunk++;                                                                                \
		line_ptr->pChunks[iCurChunk].type = _chunk_type;                                            \
		line_ptr->pChunks[iCurChunk].iTextStart = iTextIdx;                                         \
		line_ptr->pChunks[iCurChunk].iTextLen = 0;                                                  \
		if(iCurChunk > 0)                                                                           \
			line_ptr->pChunks[iCurChunk].customFore = line_ptr->pChunks[iCurChunk - 1].customFore;

		// EOF Macros
	
	int partLen;
	
	/*
	 * Some additional description for the profanes: we want a fast way to check the presence of "active objects we have to process" in lines of text;
	 * such objects can be: EOF, URLs, mIRC control characters, emoticons, and so on. We implemented a jump table to accomplish this task very fast.
	 * This jump table is an array[256] containing label addresses (imagine them as functions). So something like "goto array[4];" is valid construct
	 * in C, that is equivalent to a function call to a function that starts on that label's line of code.
	 * Imagine to parse the input line one character at once and match it (as a switch can do) against this big array. Every 1-byte character corresponds
	 * to an ASCII integer between 0 and 255. If the array value for that integer key is defined and !=0, we jump to the corresponding label address.
	 * Example, if we find a "H" (72) we'll "goto char_to_check_jump_table[72]", aka "goto check_http_url".
	 * There exists two different versions of this tricky code, we switch them depending on the compiler abilities to accept our bad code :)
	 */
	
#ifdef COMPILE_USE_DYNAMIC_LABELS

	// Heresy :)

	// This is not only usage of the *Evil Goto(tm)*
	// This is also a *rather unclear* use of the *Really Evil Goto(tm)*
	// char_to_check_jump_table is a table of dynamic label addresses...
	// we use it to jump to the proper check
	// loop_begin is a dynamic label, and we use it to
	// return to the appropriate loop
	// This is again BAD PROGRAMMING(TM) :).... but it is faster than
	// the version with no dynamic gotos, and really faster
	// that any version without gotos that came into my mind...
	//
	// This code will prolly work only with GCC...(and even needs a "smart" one)

	// Again did two versions... the first was:
	//
	//  if(void * jmp_address = char_to_check_jump_table[*((unsigned char *)p)])goto *jmp_address;
	//    18a3:	8b 55 f0             	movl   0xfffffff0(%ebp),%edx
	//    18a6:	31 c0                	xorl   %eax,%eax
	//    18a8:	8a 02                	movb   (%edx),%al
	//    18aa:	8b 04 85 20 00 00 00 	movl   0x20(,%eax,4),%eax
	//    18b1:	85 c0                	testl  %eax,%eax
	//    18b3:	74 02                	je     18b7 <KviIrcView::getTextLine(int, char const *, KviIrcViewLine *)+0x1f3>
	//    18b5:	ff e0                	jmp    *%eax
	//
	// I even had a nicer version:
	//
	//  goto *(char_to_check_jump_table[*((unsigned char *)p)]);
	//    18a3:	8b 55 f0             	movl   0xfffffff0(%ebp),%edx
	//    18a6:	31 c0                	xorl   %eax,%eax
	//    18a8:	8a 02                	movb   (%edx),%al
	//    18aa:	ff 24 85 20 00 00 00 	jmp    *0x20(,%eax,4)
	//
	// but sth tells me that "jmp *0x20(,%eax,4)" takes a loooooot of clock ticks...
	// ...we have less instructions, but the code takes longer to execute (7-8% longer)
	// it might be also due to pipeline tricks, jump "next instruction precalculation" stuff...

	// So we end up using the first version here

	void * loop_begin;

	static void * char_to_check_jump_table[256] = {
		// clang-format off
		&&found_end_of_buffer        ,nullptr                      ,&&found_mirc_escape          ,&&found_color_escape         ,
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      ,
		nullptr                      ,&&found_tab                  ,&&found_end_of_line          ,nullptr                      ,
		nullptr                      ,&&found_command_escape       ,nullptr                      ,&&found_mirc_escape          ,
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      ,
		nullptr                      ,nullptr                      ,&&found_mirc_escape          ,nullptr                      ,
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      ,
		&&found_icon_escape          ,&&found_mirc_escape          ,nullptr                      ,&&found_mirc_escape          , // 000-031
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      ,
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      ,
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      ,
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      , // 032-047
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      ,
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      ,
		nullptr                      ,nullptr                      ,&&check_emoticon_char        ,&&check_emoticon_char        ,
		nullptr                      ,&&check_emoticon_char        ,nullptr                      ,nullptr                      , // 048-063 // 61='=' , 59=';' , 58=':'
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      ,
		nullptr                      ,&&check_e2k_url              ,&&check_file_or_ftp_url      ,nullptr                      ,
		&&check_http_url             ,&&check_irc_url              ,nullptr                      ,nullptr                      ,
		nullptr                      ,&&check_mailto_or_magnet_url ,nullptr                      ,nullptr                      , // 064-079  // 070==F 072==H 073==I  077==M
		nullptr                      ,nullptr                      ,nullptr                      ,&&check_spotify_or_sftp_url  ,
		nullptr                      ,nullptr                      ,nullptr                      ,&&check_www_url              ,
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      ,
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      , // 080-095  // 083==S 087==W
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      ,
		nullptr                      ,&&check_e2k_url              ,&&check_file_or_ftp_url      ,nullptr                      ,
		&&check_http_url             ,&&check_irc_url              ,nullptr                      ,nullptr                      ,
		nullptr                      ,&&check_mailto_or_magnet_url ,nullptr                      ,nullptr                      , // 096-111  // 101=e 102=f 104=h 105=i 109==m
		nullptr                      ,nullptr                      ,nullptr                      ,&&check_spotify_or_sftp_url  ,
		nullptr                      ,nullptr                      ,nullptr                      ,&&check_www_url              ,
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      ,
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      , // 112-127  // 115==s 119==w
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      ,
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      ,
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      ,
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      , // 128-133
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      ,
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      ,
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      ,
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      , // 134-159
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      ,
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      ,
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      ,
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      , // 160-175
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      ,
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      ,
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      ,
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      , // 176-191
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      ,
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      ,
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      ,
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      , // 192-207
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      ,
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      ,
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      ,
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      , // 208-223
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      ,
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      ,
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      ,
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      , // 224-239
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      ,
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      ,
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                      ,
		nullptr                      ,nullptr                      ,nullptr                      ,nullptr                        // 240-255
		// clang-format on
	};

	if(KVI_OPTION_BOOL(KviOption_boolIrcViewUrlHighlighting) || KVI_OPTION_BOOL(KviOption_boolDrawEmoticons))
	{
		loop_begin = &&highlighting_check_loop; // get the address of the return label
	                                            // forever loop
	highlighting_check_loop:
		// yet more optimized
		if(*((unsigned short *)p) < 0xff)
			if(void * jmp_address = char_to_check_jump_table[*((unsigned short *)p)])
				goto * jmp_address;
		// goto *(char_to_check_jump_table[*((unsigned char *)p)]); <--- replace 0 with &nothing_found
		//nothing_found:
		p++;
		goto highlighting_check_loop;
		// never here
	}
	else
	{
		loop_begin = &&escape_check_loop; // get the address of the return label
	                                      // forever loop
	escape_check_loop:
		while(*((unsigned short *)p) > 31)
			p++;
		goto check_escape_switch; // returns to escape_check_loop or returns from the function at all
		                          // never here
	}
// never here

#else // !COMPILE_USE_DYNAMIC_LABELS

	// No way to have a jump table, nor a dynamic return jump
	// Anyway...we can have sth similar to a jump table...
	// Note: this could be substituted by a compiler generated jump table
	//       for a switch command... but this is STILL faster

	static char char_to_check_table[256] = {
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 000-015
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 016-031
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 032-047
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 0, 7, 0, 0, // 048-063
		0, 0, 0, 0, 0, 8, 3, 0, 2, 5, 0, 0, 0, 6, 0, 0, // 064-079  // 070==F 072==H 073==I 077==M
		0, 0, 0, 9, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, // 080-095  // 083==S 087==W
		0, 0, 0, 0, 0, 8, 3, 0, 2, 5, 0, 0, 0, 6, 0, 0, // 096-111  // 102==f 104==h 105==i 109==m
		0, 0, 0, 9, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, // 112-127  // 115==s 119==w
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 128-133
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 134-159
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 160-175
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 176-191
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 192-207
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 208-223
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 224-239
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  // 240-255
	};

check_char_loop:
	if(KVI_OPTION_BOOL(KviOption_boolIrcViewUrlHighlighting) || KVI_OPTION_BOOL(KviOption_boolDrawEmoticons))
	{
		for(;;)
		{
			if(*((unsigned short *)p) < 0xff)
				if(unsigned int chk = char_to_check_table[*((unsigned short *)p)])
				{
					switch(chk)
					{
						case 1:
							goto check_escape_switch;
							break; // returns to check_char_loop or returns from the function at all
						case 2:
							goto check_http_url;
							break; // returns to check_char_loop
						case 3:
							goto check_file_or_ftp_url;
							break; // returns to check_char_loop
						case 4:
							goto check_www_url;
							break; // returns to check_char_loop
						case 5:
							goto check_irc_url;
							break; // returns to check_char_loop
						case 6:
							goto check_mailto_or_magnet_url;
							break; // returns to check_char_loop
						case 7:
							goto check_emoticon_char;
							break; // returns to check_char_loop
						case 8:
							goto check_e2k_url;
							break;
						case 9:
							goto check_spotify_or_sftp_url;
							break;
					}
				}
			p++;
		}
	}
	else
	{
		while(((unsigned short)*p) > 31)
			p++;
		goto check_escape_switch; // returns to check_char_loop
	}

#endif // !COMPILE_USE_DYNAMIC_LABELS

/*
 * Profane description:
 * Here the two different approaches to the jump table ends. Following there's the list of all the possible
 * codes found. The "check table" approach needs an additional switch to discriminate between different control codes,
 * while the "jump table" approach directly jumps to the right case.
 */

check_escape_switch:
	switch(*p)
	{
		case '\0':
#ifdef COMPILE_USE_DYNAMIC_LABELS
found_end_of_buffer:
#endif //COMPILE_USE_DYNAMIC_LABELS
			APPEND_LAST_TEXT_BLOCK(data_ptr, p - data_ptr)
			if(pUnEscapeAt)
			{
				// empty unescape block needed
				NEW_LINE_CHUNK(KviControlCodes::UnEscape);
				// no need to append more data
			}
			return p;
			break;
		case '\n':
#ifdef COMPILE_USE_DYNAMIC_LABELS
found_end_of_line:
#endif //COMPILE_USE_DYNAMIC_LABELS
			// Found the end of a line
			APPEND_LAST_TEXT_BLOCK(data_ptr, p - data_ptr);
			if(pUnEscapeAt)
			{
				// empty unescape block needed
				NEW_LINE_CHUNK(KviControlCodes::UnEscape);
				// no need to append more data
			}
			// terminate the string
			// move the current pointer to the next character...
			// if it is '\0' we will simply stop
			p++;
			return p;
			break;
		case '\t':
#ifdef COMPILE_USE_DYNAMIC_LABELS
found_tab:
#endif //COMPILE_USE_DYNAMIC_LABELS
			// Found tab. Artificial end of block.
			APPEND_LAST_TEXT_BLOCK(data_ptr, p - data_ptr);
			// Artificial block with a single tab
			NEW_LINE_CHUNK(KviControlCodes::ArbitraryBreak);
			data_ptr = p;
			p++;
			APPEND_LAST_TEXT_BLOCK(data_ptr, p - data_ptr);
			NEW_LINE_CHUNK(KviControlCodes::ArbitraryBreak);
			data_ptr = p;
			p++;
		break;
		case '\r':
#ifdef COMPILE_USE_DYNAMIC_LABELS
found_command_escape:
#endif //COMPILE_USE_DYNAMIC_LABELS

			if(p == pUnEscapeAt)
			{
				// This is the terminator of an escape
				APPEND_LAST_TEXT_BLOCK(data_ptr, p - data_ptr);
				NEW_LINE_CHUNK(KviControlCodes::UnEscape);
				pUnEscapeAt = nullptr;
				p++;
				data_ptr = p;
				break;
			}

			// Command escape sequence
			// \r!<escape_cmd>\r<visible parameters string>\r
			p++;
			if(*p == '!')
			{
				const kvi_wchar_t * next_cr = p;
				// lookup the next carriage return
				while(*next_cr && (*next_cr != '\r'))
					next_cr++;

				if(*next_cr)
				{
					const kvi_wchar_t * term_cr = next_cr;
					term_cr++;
					while(*term_cr && (*term_cr != '\r'))
						term_cr++;

					if(*term_cr)
					{
						// ok....the format is right:
						//  \r!    ... \r ...    \r
						//    ^p        ^next_cr  ^term_cr
						p--;
						//  \r!    ... \r ...    \r
						//   ^p         ^next_cr  ^term_cr
						APPEND_LAST_TEXT_BLOCK(data_ptr, p - data_ptr)
						NEW_LINE_CHUNK(KviControlCodes::Escape)

						p += 2; //point after \r!

						blockLen = (next_cr - p);
						line_ptr->pChunks[iCurChunk].szPayload = (kvi_wchar_t *)KviMemory::allocate(((next_cr - p) + 1) * sizeof(kvi_wchar_t));
						KviMemory::copy((void *)(line_ptr->pChunks[iCurChunk].szPayload), p, blockLen * sizeof(kvi_wchar_t));

						line_ptr->pChunks[iCurChunk].szPayload[blockLen] = 0;

						++next_cr; //point after the middle \r

						bool bColorSet = false;
						if((line_ptr->pChunks[iCurChunk].szPayload[0] == 'n') && KVI_OPTION_BOOL(KviOption_boolUseUserListColorsAsNickColors) && (!KVI_OPTION_BOOL(KviOption_boolColorNicks)))
						{
							if(m_pKviWindow->type() == KviWindow::Channel && m_pKviWindow)
							{
								if(line_ptr->pChunks[iCurChunk].szPayload[1] == 'c' && ((KviChannelWindow *)m_pKviWindow)->userListView())
								{
									KviUserListEntry * e = ((KviChannelWindow *)m_pKviWindow)->userListView()->findEntry(QString((QChar *)next_cr, term_cr - next_cr));
									if(e)
									{
										line_ptr->pChunks[iCurChunk].colors.fore = KVI_COLOR_CUSTOM;
										e->color(line_ptr->pChunks[iCurChunk].customFore);
										bColorSet = true;
									}
								}
							}
							else if(m_pKviWindow->type() == KviWindow::Query && m_pKviWindow && line_ptr->pChunks[iCurChunk].szPayload[1] == 'c')
							{
								QString m_szNick = QString((QChar *)next_cr, term_cr - next_cr);
								if(m_szNick == m_pKviWindow->connection()->currentNickName())
								{
									line_ptr->pChunks[iCurChunk].colors.fore = KVI_COLOR_OWN;
									bColorSet = true;
								}
							}
						}
						if(!bColorSet)
							line_ptr->pChunks[iCurChunk].colors.fore = KviControlCodes::NoChange;

#if 0
						APPEND_LAST_TEXT_BLOCK(next_cr,term_cr - next_cr)
						NEW_LINE_CHUNK(KVI_TEXT_UNESCAPE)

						p=term_cr;
#else
						pUnEscapeAt = term_cr;

						//APPEND_LAST_TEXT_BLOCK(next_cr,term_cr - next_cr)
						//NEW_LINE_CHUNK(KVI_TEXT_UNESCAPE)

						p = next_cr;
#endif
						data_ptr = p;
					}
				}
			}
			break;
		case KviControlCodes::Color:
#ifdef COMPILE_USE_DYNAMIC_LABELS
found_color_escape:
#endif //COMPILE_USE_DYNAMIC_LABELS
			//Color control code...need a new attribute struct
			APPEND_LAST_TEXT_BLOCK(data_ptr, p - data_ptr)
			NEW_LINE_CHUNK(*p)
			p++;
			p = KviControlCodes::getColorBytesW(p, &(line_ptr->pChunks[iCurChunk].colors.fore), &(line_ptr->pChunks[iCurChunk].colors.back));
			data_ptr = p;
			break;
		case KviControlCodes::Icon:
#ifdef COMPILE_USE_DYNAMIC_LABELS
found_icon_escape:
#endif //COMPILE_USE_DYNAMIC_LABELS
			p++;
			if(KVI_OPTION_BOOL(KviOption_boolDrawEmoticons))
			{
				if(*p > 32)
				{
					// Icon control word... need a new attribute struct
					const kvi_wchar_t * beginPtr = p - 1;
					const kvi_wchar_t * icon_name = p;
					while(*p > 32)
						p++;
					int datalen = p - icon_name;
					kvi_wchar_t save = *p;
					// throw away constness!
					*((kvi_wchar_t *)p) = 0;
					// FIXME: this has to be changed! : lookupTextIcon must use wide characters!
					QString tmpQ;
					tmpQ.setUtf16(icon_name, datalen);
					KviTextIcon * icon = g_pTextIconManager->lookupTextIcon(tmpQ);
					// throw away constness!
					*((kvi_wchar_t *)p) = save;
					//if(*p == KVI_TEXT_ICON)p++; // ending delimiter
					if(icon)
					{
						APPEND_LAST_TEXT_BLOCK(data_ptr, beginPtr - data_ptr)
						NEW_LINE_CHUNK(KviControlCodes::Icon)
						line_ptr->pChunks[iCurChunk].szPayload = (kvi_wchar_t *)KviMemory::allocate((datalen + 1) * sizeof(kvi_wchar_t));
						KviMemory::copy((void *)(line_ptr->pChunks[iCurChunk].szPayload), icon_name, datalen * sizeof(kvi_wchar_t));
						line_ptr->pChunks[iCurChunk].szPayload[datalen] = 0;
						line_ptr->pChunks[iCurChunk].szSmileId = line_ptr->pChunks[iCurChunk].szPayload;

						APPEND_LAST_TEXT_BLOCK_HIDDEN_FROM_NOW(icon_name, datalen)

						if(icon->animatedPixmap())
						{
							//FIXME: that's ugly
							disconnect(icon->animatedPixmap(), SIGNAL(frameChanged()), this, SLOT(animatedIconChange()));
							connect(icon->animatedPixmap(), SIGNAL(frameChanged()), this, SLOT(animatedIconChange()));
							m_hAnimatedSmiles.insert(line_ptr, icon->animatedPixmap());
						}
						data_ptr = p;
						NEW_LINE_CHUNK(KviControlCodes::UnIcon)
					}
				}
			}
			break;
		case KviControlCodes::Bold:
		case KviControlCodes::Italic:
		case KviControlCodes::Underline:
		case KviControlCodes::Reverse:
		case KviControlCodes::Reset:
#ifdef COMPILE_USE_DYNAMIC_LABELS
found_mirc_escape:
#endif //COMPILE_USE_DYNAMIC_LABELS
			APPEND_LAST_TEXT_BLOCK(data_ptr, p - data_ptr)
			NEW_LINE_CHUNK(*p)
			data_ptr = ++p;
			break;
		default:
			p++;
			break;
	}

/*
 * Profane description: end of the additional switch needed in the "check table" approach;
 * the next instruction re-starts the loop
 */

#ifdef COMPILE_USE_DYNAMIC_LABELS
	goto * loop_begin;
#else  // !COMPILE_USE_DYNAMIC_LABELS
	goto check_char_loop;
#endif // !COMPILE_USE_DYNAMIC_LABELS

/*
 * Profane description: More codes. The difference from the previous block is that these codes doesn't need the additional switch in the
 * "check table" approach as before.
 */

check_http_url:
	p++;
	if(KVI_OPTION_BOOL(KviOption_boolIrcViewUrlHighlighting))
	{
		/*
		 * Profane description: we found an 'h' using the "jump/check table", now check for a 't' (we don't want to search directly for the
		 * "http://" tag, it takes us more CPU time)
		 */

		//
		if((*p == 't') || (*p == 'T'))
		{
			/*
			 * Profane description: we found it! now there's an high probability we're in front of an http URL. Relax, rewind the last
			 * character and try to match the complete URL protocol tag
	 		 */
			p--;

			static kvi_wchar_t aHttpUrl[] = { 'h', 't', 't', 'p', ':', '/', '/' };

			if(url_compare_helper(p, aHttpUrl, 7))
			{
				partLen = 7;
				/*
				* Profane description: we got it; save in partLen the length of the tag and jump to the got_url label.
				*/
				goto got_url;
			}

			static kvi_wchar_t aHttpsUrl[] = { 'h', 't', 't', 'p', 's', ':', '/', '/' };

			if(url_compare_helper(p, aHttpsUrl, 8))
			{
				partLen = 8;
				goto got_url;
			}
			p++;
		}
	}
#ifdef COMPILE_USE_DYNAMIC_LABELS
	goto * loop_begin;
#else  // !COMPILE_USE_DYNAMIC_LABELS
	goto check_char_loop;
#endif // !COMPILE_USE_DYNAMIC_LABELS

check_file_or_ftp_url:
	p++;
	if(KVI_OPTION_BOOL(KviOption_boolIrcViewUrlHighlighting))
	{
		if((*p == 'i') || (*p == 'I'))
		{
			p--;

			static kvi_wchar_t aFileUrl[] = { 'f', 'i', 'l', 'e', ':', '/', '/' };

			if(url_compare_helper(p, aFileUrl, 7))
			{
				partLen = 7;
				goto got_url;
			}
			p++;
		}
		else if((*p == 't') || (*p == 'T'))
		{
			p--;

			static kvi_wchar_t aFtp1Url[] = { 'f', 't', 'p', ':', '/', '/' };

			if(url_compare_helper(p, aFtp1Url, 6))
			{
				partLen = 6;
				goto got_url;
			}

			static kvi_wchar_t aFtpsUrl[] = { 'f', 't', 'p', 's', ':', '/', '/' };

			if(url_compare_helper(p, aFtpsUrl, 7))
			{
				partLen = 7;
				goto got_url;
			}

			static kvi_wchar_t aFtpesUrl[] = { 'f', 't', 'p', 'e', 's', ':', '/', '/' };

			if(url_compare_helper(p, aFtpesUrl, 8))
			{
				partLen = 8;
				goto got_url;
			}

			static kvi_wchar_t aFtp2Url[] = { 'f', 't', 'p', '.' };

			if(url_compare_helper(p, aFtp2Url, 4))
			{
				partLen = 4;
				goto got_url;
			}
			p++;
		}
	}
#ifdef COMPILE_USE_DYNAMIC_LABELS
	goto * loop_begin;
#else  // !COMPILE_USE_DYNAMIC_LABELS
	goto check_char_loop;
#endif // !COMPILE_USE_DYNAMIC_LABELS

check_e2k_url:
	p++;
	if(KVI_OPTION_BOOL(KviOption_boolIrcViewUrlHighlighting))
	{
		if((*p == 'd') || (*p == 'D'))
		{
			p--;

			static kvi_wchar_t aEd2kUrl[] = { 'e', 'd', '2', 'k', ':', '/', '/' };

			if(url_compare_helper(p, aEd2kUrl, 7))
			{
				partLen = 7;
				goto got_url;
			}
			p++;
		}
	}
#ifdef COMPILE_USE_DYNAMIC_LABELS
	goto * loop_begin;
#else  // !COMPILE_USE_DYNAMIC_LABELS
	goto check_char_loop;
#endif // !COMPILE_USE_DYNAMIC_LABELS

check_www_url:
	p++;
	if(KVI_OPTION_BOOL(KviOption_boolIrcViewUrlHighlighting))
	{
		if((*p == 'w') || (*p == 'W'))
		{
			p--;

			static kvi_wchar_t aWwwUrl[] = { 'w', 'w', 'w', '.' };

			if(url_compare_helper(p, aWwwUrl, 4))
			{
				partLen = 4;
				goto got_url;
			}
			p++;
		}
	}
#ifdef COMPILE_USE_DYNAMIC_LABELS
	goto * loop_begin;
#else  // !COMPILE_USE_DYNAMIC_LABELS
	goto check_char_loop;
#endif // !COMPILE_USE_DYNAMIC_LABELS

check_irc_url:
	p++;
	if(KVI_OPTION_BOOL(KviOption_boolIrcViewUrlHighlighting))
	{
		if((*p == 'r') || (*p == 'R'))
		{
			p--;

			static kvi_wchar_t aIrcUrl[] = { 'i', 'r', 'c', ':', '/', '/' };

			if(url_compare_helper(p, aIrcUrl, 6))
			{
				partLen = 6;
				goto got_url;
			}

			static kvi_wchar_t aIrc6Url[] = { 'i', 'r', 'c', '6', ':', '/', '/' };

			if(url_compare_helper(p, aIrc6Url, 7))
			{
				partLen = 7;
				goto got_url;
			}

			static kvi_wchar_t aIrcsUrl[] = { 'i', 'r', 'c', 's', ':', '/', '/' };

			if(url_compare_helper(p, aIrcsUrl, 7))
			{
				partLen = 7;
				goto got_url;
			}

			static kvi_wchar_t aIrcs6Url[] = { 'i', 'r', 'c', 's', '6', ':', '/', '/' };

			if(url_compare_helper(p, aIrcs6Url, 8))
			{
				partLen = 8;
				goto got_url;
			}

			p++;
		}
	}
#ifdef COMPILE_USE_DYNAMIC_LABELS
	goto * loop_begin;
#else  // !COMPILE_USE_DYNAMIC_LABELS
	goto check_char_loop;
#endif // !COMPILE_USE_DYNAMIC_LABELS

check_mailto_or_magnet_url:
	p++;
	if(KVI_OPTION_BOOL(KviOption_boolIrcViewUrlHighlighting))
	{
		if((*p == 'a') || (*p == 'A'))
		{
			p--;

			static kvi_wchar_t aMailtoUrl[] = { 'm', 'a', 'i', 'l', 't', 'o', ':' };
			static kvi_wchar_t aMagnetUrl[] = { 'm', 'a', 'g', 'n', 'e', 't', ':' };

			if(url_compare_helper(p, aMailtoUrl, 7))
			{
				partLen = 7;
				goto got_url;
			}

			if(url_compare_helper(p, aMagnetUrl, 7))
			{
				partLen = 7;
				goto got_url;
			}
			p++;
		}
	}
#ifdef COMPILE_USE_DYNAMIC_LABELS
	goto * loop_begin;
#else  // !COMPILE_USE_DYNAMIC_LABELS
	goto check_char_loop;
#endif // !COMPILE_USE_DYNAMIC_LABELS

check_spotify_or_sftp_url:
	p++;
	if(KVI_OPTION_BOOL(KviOption_boolIrcViewUrlHighlighting))
	{
		if((*p == 'f') || (*p == 'F'))
		{
			p--;

			static kvi_wchar_t aSftpUrl[] = { 's', 'f', 't', 'p', ':', '/', '/' };

			if(url_compare_helper(p, aSftpUrl, 7))
			{
				partLen = 7;
				goto got_url;
			}
			p++;
		}
		else if((*p == 'p') || (*p == 'P'))
		{
			p--;

			static kvi_wchar_t aSpotifyUrl[] = { 's', 'p', 'o', 't', 'i', 'f', 'y', ':' };

			if(url_compare_helper(p, aSpotifyUrl, 8))
			{
				partLen = 8;
				goto got_url;
			}
			p++;
		}
	}
#ifdef COMPILE_USE_DYNAMIC_LABELS
	goto * loop_begin;
#else  // !COMPILE_USE_DYNAMIC_LABELS
	goto check_char_loop;
#endif // !COMPILE_USE_DYNAMIC_LABELS

got_url:
	//URL highlighting block

	/*
	 * Profane description: we just found a tag that we suppose to be the start of a URL.
	 * p is the address of the start of our text buffer, partLen the length of the tag (e.g. http:// = 7)
	 * We want to check if it's valid and highlight it creating an ad-hoc chunk for it in this line.
	 * The ASCII value of the first character after the tag have to be >= 47, or we assume it as invalid
	 */

	if(*(p + partLen) < 47)
	{
		//invalid: append all the text up to the end of the false URL tag
		p += partLen;
		APPEND_LAST_TEXT_BLOCK(data_ptr, p - data_ptr)
	}
	else
	{
		//valid: append all the text before the start of the URL tag
		APPEND_LAST_TEXT_BLOCK(data_ptr, p - data_ptr)
		//create a new chunk
		NEW_LINE_CHUNK(KviControlCodes::Escape)
		// FIXME: #warning "Option for the URL escape...double click and right button!!!"

		//	int urlLen = KVI_OPTION_STRING(KviOption_stringUrlLinkCommand).len() + 1;

		//write into null-terminated char* szPayload an 'u' that means that this chunk represents an URL
		line_ptr->pChunks[iCurChunk].szPayload = (kvi_wchar_t *)KviMemory::allocate(2 * sizeof(kvi_wchar_t));
		line_ptr->pChunks[iCurChunk].szPayload[0] = 'u';
		line_ptr->pChunks[iCurChunk].szPayload[1] = 0x0;
		//set the color for this chunk
		line_ptr->pChunks[iCurChunk].colors.fore = KVI_OPTION_MSGTYPE(KVI_OUT_URL).fore();
		//now run until the presumed end of the url
		data_ptr = p;
		p += partLen;

		p = skip_to_end_of_url(p);

		if(m_pKviWindow)
		{
			QString tmp;
			tmp.setUtf16(data_ptr, p - data_ptr);
			KVS_TRIGGER_EVENT_1(KviEvent_OnURL, m_pKviWindow, tmp);
		}

		//add all the text till the end of the URL, then create a new "clean" chunk for the next cycle loop
		APPEND_LAST_TEXT_BLOCK(data_ptr, p - data_ptr)
		NEW_LINE_CHUNK(KviControlCodes::UnEscape)
	}
	//update the value of the main counter, then restart the loop
	data_ptr = p;

#ifdef COMPILE_USE_DYNAMIC_LABELS
	goto * loop_begin;
#else  // !COMPILE_USE_DYNAMIC_LABELS
	goto check_char_loop;
#endif // !COMPILE_USE_DYNAMIC_LABELS

//FIXME #warning: Add more emoticons, and more intelligent code to detect when they're not really emoticons

check_emoticon_char:
	// What about this ?

	const kvi_wchar_t * begin = p;
	p++;
	if(KVI_OPTION_BOOL(KviOption_boolDrawEmoticons))
		switch(iMsgType)
		{
			case KVI_OUT_CHANPRIVMSG:
			case KVI_OUT_ACTION:
			case KVI_OUT_ACTIONCRYPTED:
			case KVI_OUT_OWNACTION:
			case KVI_OUT_OWNACTIONCRYPTED:
			case KVI_OUT_OWNPRIVMSG:
			case KVI_OUT_QUERYPRIVMSG:
			case KVI_OUT_QUERYPRIVMSGCRYPTED:
			case KVI_OUT_QUERYNOTICE:
			case KVI_OUT_QUERYNOTICECRYPTED:
			case KVI_OUT_CHANPRIVMSGCRYPTED:
			case KVI_OUT_CHANNELNOTICE:
			case KVI_OUT_CHANNELNOTICECRYPTED:
			case KVI_OUT_OWNPRIVMSGCRYPTED:
			case KVI_OUT_HIGHLIGHT:
			case KVI_OUT_DCCCHATMSG:
			{
				// Pragma: 31.05.2002 : I had to kill the 8 prefix
				// It happens really too often to have an 8 followed by a parenthesis
				// that is not an emoticon

				// *begin can be one of ':', ';', '='
				if(*p == '-')
					p++; // FIXME: we could handle also 'o' as a nose ??? (extreme: also '+' ?)
				// FIXME: use a "jump-like-check-table" here ? .... it would be surely faster
				// FIXME: handle also '[',']','\\','p','@','#','<','>','|' ???
				// alt + 39 is the common key for tear in emoticon not alt+ 176 also fix issue #1614
				switch(*p)
				{
					case ')':
					case '(':
					case '/':
					case 'D':
					case 'P':
					case 'S':
					case 'O':
					case 'p':
					case 's':
					case 'o':
					case '*':
					case '|':
					case 'B':
					case 39: //  -> alt 39 : teardrop
					{
						const kvi_wchar_t * item = p;
						const kvi_wchar_t * item2 = nullptr;
						p++;
						while(*p == *item)
							p++;
						int count = (p - item) - 1;
						if(*item == 39)
						{
							if(*p == ')')
							{
								item2 = p;
								p++;
							}
						}
						if(!*p || (*p == ' '))
						{
							// OK! this is an emoticon (sequence) !
							// We lookup simplified versions of the emoticons...

							// FIXME: this should become UNICODE!!!
							QString lookupstring;
							kvi_wchar_t ng[3];
							ng[0] = *begin;
							ng[1] = *item;
							if(item2)
								ng[2] = *item2;
							lookupstring.setUtf16(ng, item2 ? 3 : 2);

							KviTextIcon * icon = g_pTextIconManager->lookupTextIcon(lookupstring);
							// do we have that emoticon-icon association ?
							if(icon)
							{

								if(icon->animatedPixmap())
								{
									//FIXME: that's ugly
									disconnect(icon->animatedPixmap(), SIGNAL(frameChanged()), this, SLOT(animatedIconChange()));
									connect(icon->animatedPixmap(), SIGNAL(frameChanged()), this, SLOT(animatedIconChange()));
									m_hAnimatedSmiles.insert(line_ptr, icon->animatedPixmap());
								}

								// we got an icon for this emoticon
								// the tooltip will carry the original emoticon source text
								APPEND_LAST_TEXT_BLOCK(data_ptr, begin - data_ptr)
								NEW_LINE_CHUNK(KviControlCodes::Icon)

								int emolen = p - begin;
								int reallen = item2 ? 3 : 2;

								line_ptr->pChunks[iCurChunk].szPayload = (kvi_wchar_t *)KviMemory::allocate((emolen + 1) * sizeof(kvi_wchar_t));
								KviMemory::copy(line_ptr->pChunks[iCurChunk].szPayload, begin, emolen * sizeof(kvi_wchar_t));
								line_ptr->pChunks[iCurChunk].szPayload[emolen] = 0;

								line_ptr->pChunks[iCurChunk].szSmileId = (kvi_wchar_t *)KviMemory::allocate((reallen + 1) * sizeof(kvi_wchar_t));
								KviMemory::copy(line_ptr->pChunks[iCurChunk].szSmileId, ng, reallen * sizeof(kvi_wchar_t));
								line_ptr->pChunks[iCurChunk].szSmileId[reallen] = 0;

								APPEND_LAST_TEXT_BLOCK_HIDDEN_FROM_NOW(begin, emolen)
								data_ptr = p;
								// let's also handle thingies like :DDDD
								item++;
								while(count > 0)
								{
									NEW_LINE_CHUNK(KviControlCodes::Icon)
									line_ptr->pChunks[iCurChunk].szPayload = (kvi_wchar_t *)KviMemory::allocate((emolen + 1) * sizeof(kvi_wchar_t));
									KviMemory::copy(line_ptr->pChunks[iCurChunk].szPayload, begin, emolen * sizeof(kvi_wchar_t));
									line_ptr->pChunks[iCurChunk].szPayload[emolen] = 0;

									line_ptr->pChunks[iCurChunk].szSmileId = (kvi_wchar_t *)KviMemory::allocate((reallen + 1) * sizeof(kvi_wchar_t));
									KviMemory::copy(line_ptr->pChunks[iCurChunk].szSmileId, ng, reallen * sizeof(kvi_wchar_t));
									line_ptr->pChunks[iCurChunk].szSmileId[reallen] = 0;

									APPEND_ZERO_LENGTH_BLOCK(data_ptr)
									count--;
								}
								NEW_LINE_CHUNK(KviControlCodes::UnIcon)
							} // we don't even need to skip back... the text eventually parsed is ok to be in a single block for sure
						}     // we don't even need to skip back... the text eventually parsed is ok to be in a single block for sure
					}         // we don't even need to skip back... the text eventually parsed is ok to be in a single block for sure
					break;
				} // switch(*p)
			}
			break;
		}

#ifdef COMPILE_USE_DYNAMIC_LABELS
	goto * loop_begin;
#else  // !COMPILE_USE_DYNAMIC_LABELS
	goto check_char_loop;
#endif // !COMPILE_USE_DYNAMIC_LABELS

	// never here
	return p;
}

void KviIrcView::reapplyMessageColors()
{
	// This function is usually called when the theme is changed.
	// It re-applies the colors for all the messages (which may have been changed).

	KviIrcViewLine * pLine = m_pFirstLine;
	while(pLine)
	{
		pLine->iMaxLineWidth = -1; // force recomputation of blocks

		if(pLine->uChunkCount > 0) // always true?
		{
			unsigned char oldBack = pLine->pChunks[0].colors.back;
			unsigned char oldFore = pLine->pChunks[0].colors.fore;

			for(unsigned int u = 0; u < pLine->uChunkCount; u++)
			{
				if((pLine->pChunks[u].colors.back == oldBack) && (pLine->pChunks[u].colors.fore == oldFore))
				{
					pLine->pChunks[u].colors.back = KVI_OPTION_MSGTYPE(pLine->iMsgType).back();
					pLine->pChunks[u].colors.fore = KVI_OPTION_MSGTYPE(pLine->iMsgType).fore();
				}
			}
		}

		pLine = pLine->pNext;
	}
}

void KviIrcView::appendText(int iMsgType, const kvi_wchar_t * data_ptr, int iFlags, const QDateTime & datetime)
{
	//appends a text string to the buffer list
	//splits the lines
	KVI_ASSERT(data_ptr);
	m_pLastLinkUnderMouse = nullptr;

	if(!KVI_OPTION_BOOL(KviOption_boolStripControlCodesInLogs))
	{
		// Looks like the user wants to keep the control codes in the log file: we just dump everything inside (including newlines...)
		if(m_pLogFile && KVI_OPTION_MSGTYPE(iMsgType).logEnabled())
		{
			add2Log(QString::fromUtf16(data_ptr), datetime, iMsgType, true);
		}
		else if(m_pMasterView)
		{
			if(m_pMasterView->m_pLogFile && KVI_OPTION_MSGTYPE(iMsgType).logEnabled())
				m_pMasterView->add2Log(QString::fromUtf16(data_ptr), datetime, iMsgType, true);
		}
	}

	while(*data_ptr)
	{
		// have more data to process

		KviIrcViewLine * line_ptr = new KviIrcViewLine; //create a line struct

		line_ptr->iMsgType = iMsgType;
		line_ptr->iMaxLineWidth = -1;
		line_ptr->iBlockCount = 0;
		line_ptr->uLineWraps = 0;

		data_ptr = getTextLine(iMsgType, data_ptr, line_ptr, !(iFlags & NoTimestamp), datetime);

		appendLine(line_ptr, datetime, !(iFlags & NoRepaint));

		if(iFlags & SetLineMark)
		{
			if(KVI_OPTION_BOOL(KviOption_boolTrackLastReadTextViewLine))
			{
				m_uLineMarkLineIndex = line_ptr->uIndex;
				iFlags &= ~SetLineMark;
			}
		}

		if(iFlags & TriggersNotification)
		{
			if(!m_bHaveUnreadedHighlightedMessages && iMsgType == KVI_OUT_HIGHLIGHT)
				m_bHaveUnreadedHighlightedMessages = true;
			if(!m_bHaveUnreadedMessages && (iMsgType == KVI_OUT_CHANPRIVMSG || iMsgType == KVI_OUT_CHANPRIVMSGCRYPTED || iMsgType == KVI_OUT_CHANNELNOTICE || iMsgType == KVI_OUT_CHANNELNOTICECRYPTED || iMsgType == KVI_OUT_ACTION || iMsgType == KVI_OUT_ACTIONCRYPTED || iMsgType == KVI_OUT_QUERYPRIVMSG || iMsgType == KVI_OUT_QUERYPRIVMSGCRYPTED || iMsgType == KVI_OUT_DCCCHATMSG || iMsgType == KVI_OUT_DCCCHATMSGCRYPTED || iMsgType == KVI_OUT_HIGHLIGHT))
				m_bHaveUnreadedMessages = true;
		}
	}
}
