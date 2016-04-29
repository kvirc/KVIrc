#ifndef _KVI_KVS_PARSER_MACROS_H_
#define _KVI_KVS_PARSER_MACROS_H_
//=============================================================================
//
//   File : KviKvsParserMacros.h
//   Creation date : Sun 5 Ocr 2003 20.25 CEST by Szymon Stefanek
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

#define KVSP_skipChar m_ptr++
#define KVSP_skipNChars(n) m_ptr += n
#define KVSP_backChar m_ptr--
#define KVSP_backNChars(n) m_ptr -= n

#define KVSP_curCharUnicode (m_ptr->unicode())
#define KVSP_curCharPointer m_ptr

#define KVSP_curCharIsLetter (m_ptr->isLetter())
#define KVSP_curCharIsNumber (m_ptr->isNumber())
#define KVSP_curCharIsLetterOrNumber (m_ptr->isLetterOrNumber())
#define KVSP_curCharIsFunctionStart (m_ptr->isLetterOrNumber() || (m_ptr->unicode() == '(') || (m_ptr->unicode() == '{') || (m_ptr->unicode() == '$') || (m_ptr->unicode() == '#'))
#define KVSP_curCharIsEndOfCommand ((m_ptr->unicode() == ';') || (m_ptr->unicode() == 0) || (m_ptr->unicode() == '\n') || (m_ptr->unicode() == '\r'))
#define KVSP_curCharIsEndOfBuffer (m_ptr->unicode() == 0)

#define KVSP_setCurCharPointer(_ptr) m_ptr = _ptr

#define KVSP_ASSERT(_x) \
	if(!(_x))           \
		qDebug("WARNING : ASSERT FAILED: (%s) IS false AT %s:%d", #_x, __FILE__, __LINE__);

#endif //!_KVI_KVS_PARSER_MACROS_H_
