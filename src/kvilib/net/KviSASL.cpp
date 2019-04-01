//=============================================================================
//
//   File : KviSASL.cpp
//   Creation date : Mon Feb 14 2010 19:36:12 CEST by Fabio Bas
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2010 Fabio Bas (ctrlaltca at gmail dot com)
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

#include "KviSASL.h"

#include "KviCString.h"
#include "KviMemory.h"

#include <QByteArray>
#include <QStringList>

namespace KviSASL
{
	QStringList supportedMethods()
	{
		return {
			QStringLiteral("PLAIN"),
			QStringLiteral("EXTERNAL")
		};
	}

	bool plainMethod(const KviCString & szIn, KviCString & szOut, const QByteArray & baNick, const QByteArray & baPass)
	{
		if(szIn == "+")
		{
			int answerLen = 2 + (2 * baNick.size()) + baPass.size();
			char * answer = (char *)KviMemory::allocate(answerLen);
			char * answer2 = answer;

			memcpy(answer, baNick.data(), baNick.size());
			answer += baNick.size();
			memset(answer, 0, 1);
			answer++;

			memcpy(answer, baNick.data(), baNick.size());
			answer += baNick.size();
			memset(answer, 0, 1);
			answer++;

			memcpy(answer, baPass.data(), baPass.size());

			szOut.bufferToBase64(answer2, answerLen);
			KviMemory::free(answer2);

			return true;
		}
		return false;
	}

	bool externalMethod(const KviCString & szIn, KviCString & szOut)
	{
		if(szIn == "+")
		{
			szOut = szIn;

			return true;
		}
		return false;
	}
}
