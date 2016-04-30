//=============================================================================
//
//   File : KviKvsParser_comment.cpp
//   Creation date : Thu 03 Nov 2003 13.23 CEST by Szymon Stefanek
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

#include "KviKvsParser.h"
#include "KviKvsTreeNode.h"
#include "KviKvsReport.h"
#include "KviKvsKernel.h"
#include "KviKvsParserMacros.h"
#include "KviLocale.h"

KviKvsTreeNode * KviKvsParser::parseComment()
{
	KVSP_ASSERT((KVSP_curCharUnicode == '#') || (KVSP_curCharUnicode == '/'));

	switch(KVSP_curCharUnicode)
	{
		case '#': // bash style
			skipToNextLine();
			break;
		case '/':
			KVSP_skipChar;
			switch(KVSP_curCharUnicode)
			{
				case '/':
					// c++ style
					skipToNextLine();
					break;
				case '*':
				{
					const QChar * pBegin = KVSP_curCharPointer;
					// c style, multiline
					KVSP_skipChar;
					for(;;)
					{
						switch(KVSP_curCharUnicode)
						{
							case 0:
								warning(pBegin, __tr2qs_ctx("Unterminated c-style multi-line comment", "kvs"));
								error(KVSP_curCharPointer, __tr2qs_ctx("Unexpected end of script in multi-line comment", "kvs"));
								return nullptr;
								break;
							case '*':
								while(KVSP_curCharUnicode == '*')
									KVSP_skipChar;
								if(KVSP_curCharUnicode == '/')
								{
									KVSP_skipChar;
									return nullptr;
								}
								break;
						}
						KVSP_skipChar;
					}
				}
				break;
				default:
					error(KVSP_curCharPointer, __tr2qs_ctx("Unexpected character '%q' (Unicode %x) after a slash (is it a typo or a malformed comment begin?)", "kvs"), KVSP_curCharPointer, KVSP_curCharUnicode);
					return nullptr;
					break;
			}
			break;
		default:
			// shouldn't be here :/
			KVSP_ASSERT(false);
			break;
	}
	return nullptr;
}
