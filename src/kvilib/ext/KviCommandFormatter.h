#ifndef _KVI_CMDFORMATTER_H_
#define _KVI_CMDFORMATTER_H_
//=============================================================================
//
//   File : KviCommandFormatter.h
//   Creation date : Tue Jun 12 2001 03:04:05 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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

class KviCString;
class QChar;
class QStringList;

namespace KviCommandFormatter
{
	extern KVILIB_API bool hasLeadingChars(KviCString * pszArray, char c);
	extern KVILIB_API bool hasLeadingChars(QStringList & list, const QChar & c);

	extern KVILIB_API void trimLeading(KviCString ** pszArray);
	extern KVILIB_API void trimLeading(QStringList & list);

	extern KVILIB_API void addLeading(KviCString ** pszArray, char c);
	extern KVILIB_API void addLeading(QStringList & list, const QChar & c);

	extern KVILIB_API void trimBlockBraces(KviCString & szBuffer);
	extern KVILIB_API void trimBlockBraces(QString & szBuffer);

	extern KVILIB_API void unindent(KviCString & szBuffer);
	extern KVILIB_API void unindent(QString & szBuffer);

	extern KVILIB_API void bufferFromBlock(KviCString & szBuffer);
	extern KVILIB_API void bufferFromBlock(QString & szBuffer);

	extern KVILIB_API void addBlockBraces(KviCString & szBuffer);
	extern KVILIB_API void addBlockBraces(QString & szBuffer);

	extern KVILIB_API void indent(KviCString & szBuffer);
	extern KVILIB_API void indent(QString & szBuffer);

	extern KVILIB_API void blockFromBuffer(KviCString & szBuffer);
	extern KVILIB_API void blockFromBuffer(QString & szBuffer);
}

#endif //_KVI_CMDFORMATTER_H_
