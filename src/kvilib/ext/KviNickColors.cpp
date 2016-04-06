//=============================================================================
//
//   File : KviNickColors.cpp
//   Creation date : Sun Apr 11 2010 15:00:01 CEST by Fabio Bas
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2009 Fabio Bas (ctrlaltca at libero dot it)
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

#include "KviNickColors.h"

#include "KviMemory.h"
#include <stdio.h>

namespace KviNickColors
{

	#define KVI_NUM_NICK_COLORS 11

	// FIXME: Maybe make this table settable via options?
	//        Or maybe a kvc file...
	static const char * g_nickColors[KVI_NUM_NICK_COLORS]=
	{
		"2", "3", "4", "5", "6", "7", "8", "9", "10", "12", "13",
	};

	int getSmartColorForNick(QString *szNick)
	{
		int sum = 0;
		int i = szNick->length();
		const QChar * aux = szNick->unicode();
		// FIXME: Shouldn't this be case insensitive ?
		while(i > 0)
		{
			sum += aux->unicode();
			aux++;
			i--;
		}
		return sum;
	}

	const char * getSmartColor(int iPos)
	{
		return g_nickColors[iPos % KVI_NUM_NICK_COLORS];
	}

	int getSmartColorIntByMircColor(unsigned char iFore,unsigned char iBack)
	{
		int iBestMatch=-1;
#if 0
		char* comb= (char*)KviMemory::allocate(6);
		int numm=0;
		// TODO handle KVI_TRANSPARENT and KVI_NOCHANGE (ticket #812)
#if defined(COMPILE_ON_WINDOWS) && !(defined(MINGW))
		_snprintf(comb, 6, "%d,%d", iFore % 16, iBack % 16);
#else
		snprintf(comb, 6, "%d,%d", iFore % 16, iBack % 16);
#endif
// 		qDebug("Nick color %s",comb);
		for(int i=0; i<KVI_NUM_NICK_COLORS;++i)
		{
			int numc=0;
			// strcmp
			while(g_nickColors[i][numc] && comb[numc])
			{
				if(g_nickColors[i][numc] != comb[numc])
					break;
				numc++;
			}
			if(numc>0)
			{
				// any match
				if(!g_nickColors[i][numc] && !comb[numc])
				{
					//exact match
// 					qDebug("Exact match %s",g_nickColors[i]);
					KviMemory::free(comb);
					return i;
				} else {
					//partial match
					if(numc > numm)
					{
						//better than any previous one
						iBestMatch=i;
						numm=numc;
					}
				}
			}
		}
		KviMemory::free(comb);
		//no exact match
// 		qDebug("Best match %s",g_nickColors[iBestMatch]);
#endif
		return iBestMatch;
	}
}
