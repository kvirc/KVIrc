//=============================================================================
//
//   File : KviCommandFormatter.cpp
//   Creation date : Tue Jun 12 2001 03:08:12 CEST by Szymon Stefanek
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

#include "KviCommandFormatter.h"
#include "KviCString.h"
#include "KviQString.h"

#include <QStringList>

namespace KviCommandFormatter
{
	bool hasLeadingChars(KviCString ** pszArray, char c)
	{
		if(!(*pszArray))
			return false; // can't have more leading chars

		bool bGotIt = false;
		while(*pszArray)
		{
			if(*((*pszArray)->ptr()) == c)
			{
				// found at least one such leading char
				bGotIt = true;
			}
			else
			{
				// we pretend this line to be empty
				KviCString szTmp = *(*pszArray);
				szTmp.trim();
				if(szTmp.hasData())
					return false;
				*(*pszArray) = ""; // set it to empty also in the main buffer
			}
			pszArray++;
		}
		return bGotIt;
	}

	bool hasLeadingChars(QStringList & list, const QChar & c)
	{
		bool bGotIt = false;
		for(auto & it : list)
		{
			if(it.length() < 1)
				continue;
			if(it.at(0) == c)
			{
				// found at least one such leading char
				bGotIt = true;
			}
			else
			{
				// we pretend this line to be empty
				QString szTmp = it;
				szTmp = szTmp.trimmed();

				if(!szTmp.isEmpty())
					return false;
				it = ""; // set it to empty also in the main buffer
			}
		}
		return bGotIt;
	}

	void trimLeading(KviCString ** pszArray)
	{
		while(*pszArray)
		{
			if((*pszArray)->hasData())
				(*pszArray)->cutLeft(1);
			pszArray++;
		}
	}

	void trimLeading(QStringList & list)
	{
		for(auto & it : list)
		{
			it.remove(0, 1);
		}
	}

	void addLeading(KviCString ** pszArray, char c)
	{
		while(*pszArray)
		{
			if((*pszArray)->hasData())
				(*pszArray)->prepend(c);
			pszArray++;
		}
	}

	void addLeading(QStringList & list, const QChar & c)
	{
		for(auto & it : list)
		{
			it.prepend(c);
		}
	}

	void unindent(KviCString & szBuffer)
	{
		// we can format correctly up to 65536 lines (that's really enough)
		int iRealLen;
		KviCString ** pszArray = szBuffer.splitToArray('\n', 65536, &iRealLen);
		if(pszArray)
		{
			while(hasLeadingChars(pszArray, '\t') || hasLeadingChars(pszArray, ' '))
				trimLeading(pszArray);
			szBuffer.joinFromArray(pszArray, "\n", true);
			KviCString::freeArray(pszArray);
		}
	}

	void unindent(QString & szBuffer)
	{
		QStringList list = szBuffer.split("\n", QString::KeepEmptyParts);

		while(hasLeadingChars(list, QChar('\t')) || hasLeadingChars(list, QChar(' ')))
			trimLeading(list);
		//szBuffer = list.join("\n"); join implementation sux :D
		// we WANT the last newline
		szBuffer = "";
		for(auto & it : list)
		{
			szBuffer.append(it);
			szBuffer.append(QChar('\n'));
		}
	}

	void bufferFromBlock(KviCString & szBuffer)
	{
		szBuffer.trim();

		if((*(szBuffer.ptr()) == '{') && szBuffer.lastCharIs('}'))
		{
			// leading and trailing { must be stripped
			szBuffer.cutLeft(1);
			szBuffer.cutRight(1);
		}

		unindent(szBuffer);

		szBuffer.trim();
	}

	void bufferFromBlock(QString & szBuffer)
	{
		szBuffer = szBuffer.trimmed();

		if(szBuffer.isEmpty())
			return;

		if((szBuffer.at(0) == QChar('{')) && szBuffer.endsWith(QChar('}')))
		{
			szBuffer.remove(0, 1);
			szBuffer.remove(szBuffer.length() - 1, 1);
			while((szBuffer.length() > 0) && ((szBuffer.at(0) == QChar('\n')) || (szBuffer.at(0) == QChar('\r'))))
				szBuffer.remove(0, 1);
		}

		unindent(szBuffer);
		szBuffer = szBuffer.trimmed();
	}

	void indent(KviCString & szBuffer)
	{
		// we can format correctly up to 65536 lines (that's really enough)
		int iRealLen;
		KviCString ** pszArray = szBuffer.splitToArray('\n', 65536, &iRealLen);
		if(pszArray)
		{
			addLeading(pszArray, '\t');
			szBuffer.joinFromArray(pszArray, "\n", true);
			KviCString::freeArray(pszArray);
		}
	}

	void indent(QString & szBuffer)
	{
		QStringList list = szBuffer.split("\n", QString::KeepEmptyParts);

		addLeading(list, QChar('\t'));
		//szBuffer = list.join("\n"); join implementation sux :D
		// we WANT the last newline
		szBuffer = "";
		for(auto & it : list)
		{
			szBuffer.append(it);
			szBuffer.append(QChar('\n'));
		}
	}

	void blockFromBuffer(KviCString & szBuffer)
	{
		indent(szBuffer);
		szBuffer.prepend("{\n");
		szBuffer.stripRightWhiteSpace();
		szBuffer.ensureLastCharIs('\n');
		szBuffer.append("}\n");
	}

	void blockFromBuffer(QString & szBuffer)
	{
		indent(szBuffer);
		szBuffer.prepend("{\n");
		KviQString::stripRightWhiteSpace(szBuffer);
		KviQString::ensureLastCharIs(szBuffer, '\n');
		szBuffer.append("}\n");
	}
}

// FIXME: #warning "move popups and events to this formatting stuff!"
