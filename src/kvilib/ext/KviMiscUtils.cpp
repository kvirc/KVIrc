//=============================================================================
//
//   File : KviMiscUtils.cpp
//   Creation date : Mon 08 Jan 2007 04:07:31 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2007-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "KviMiscUtils.h"

#include <QString>
#include <QStringList>

namespace KviMiscUtils
{
	int compareVersions(const QString & szVersion1, const QString & szVersion2)
	{
		QStringList sl1 = szVersion1.split(".");
		QStringList sl2 = szVersion2.split(".");

		QStringList::Iterator it1 = sl1.begin();
		QStringList::Iterator it2 = sl2.begin();
		while((it1 != sl1.end()) && (it2 != sl2.end()))
		{
			bool bOk;
			int i1 = (*it1).toInt(&bOk);
			if(!bOk)
				return 1;
			int i2 = (*it2).toInt(&bOk);
			if(!bOk)
				return -1;
			if(i1 != i2)
			{
				// field not equal
				if(i1 > i2)
					return -1;
				else
					return 1;
			}
			it1++;
			it2++;
		}
		// both are equal until now
		if(it1 != sl1.end())
			return -1; // 1 has at least one field more
		if(it2 != sl2.end())
			return 1; // 2 has at least one field more
		// both are equal also in length
		return 0;
	}

	bool isValidVersionString(const QString & szVersion)
	{
		QStringList sl = szVersion.split(".");

		if(sl.isEmpty())
			return false;
		// must all be numbers
		for(auto & it : sl)
		{
			bool bOk;
			int i = it.toInt(&bOk);
			if(!bOk)
				return false;
			if(i < 0)
				return false;
		}
		return true;
	}
}
