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

#include <cstdio>
#include <QString>

namespace KviNickColors
{
	// FIXME: Maybe make this table settable via options?
	//        Or maybe a kvc file...
	static const int g_numNickColors = 95;
	static const char * g_nickColors[g_numNickColors] = {
		"0,1", "0,2", "0,3", "0,4", "0,5", "0,6", "0,10", "0,12", "0,14",        //9
		"1,0", "1,4", "1,7", "1,8", "1,9", "1,11", "1,15",                       //7
		"2,0", "2,4", "2,7", "2,8", "2,9", "2,11", "2,15",                       //7
		"3,8", "3,9", "3,0", "3,15",                                             //4
		"4,0", "4,1", "4,8", "4,9", "4,11", "4,15",                              //6
		"5,0", "5,7", "5,8", "5,15",                                             //4
		"6,0", "6,7", "6,8", "6,9", "6,10", "6,11", "6,15",                      //7
		"7,1", "7,2", "7,5", "7,6", "7,14",                                      //5
		"8,1", "8,2", "8,3", "8,4", "8,5", "8,6", "8,7", "8,10", "8,12", "8,14", //10
		"9,1", "9,2", "9,3", "9,5", "9,6", "9,14",                               //6
		"10,1", "10,2",                                                          //2
		"11,1", "11,2", "11,3", "11,5", "11,6", "11,14",                         //6
		"12,0", "12,7", "12,8", "12,9", "12,10", "12,11", "12,15",               //7
		"13,0", "13,1", "13,6", "13,8", "13,11", "13,15",                        //6
		"14,0", "14,8", "14,11", "14,15",                                        //4
		"15,1", "15,2", "15,3", "15,6", "15,14"                                  //5
	};
	static const int g_numNickColorsNoBg = 8;
	static const char * g_nickColorsNoBg[g_numNickColorsNoBg] = {
		"2", "3", "4", "5", "6", "7", "10", "12"                                 //8
	};

	int getSmartColorForNick(QString * szNick)
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

	const char * getSmartColor(int iPos, bool bWithBg)
	{
		if(bWithBg)
			return g_nickColors[iPos % g_numNickColors];
		else
			return g_nickColorsNoBg[iPos % g_numNickColorsNoBg];
	}

	int getSmartColorIntByMircColor(unsigned char iFore, unsigned char iBack)
	{
		int iBestMatch = -1;
		char * comb = (char *)KviMemory::allocate(6);
		int numm = 0;
// TODO handle KVI_TRANSPARENT and KVI_NOCHANGE (ticket #812)
#if defined(COMPILE_ON_WINDOWS) && !(defined(MINGW))
		_snprintf(comb, 6, "%d,%d", iFore % 16, iBack % 16);
#else
		snprintf(comb, 6, "%d,%d", iFore % 16, iBack % 16);
#endif
		// 		qDebug("Nick color %s",comb);
		for(int i = 0; i < g_numNickColors; ++i)
		{
			int numc = 0;
			// strcmp
			while(g_nickColors[i][numc] && comb[numc])
			{
				if(g_nickColors[i][numc] != comb[numc])
					break;
				numc++;
			}
			if(numc > 0)
			{
				// any match
				if(!g_nickColors[i][numc] && !comb[numc])
				{
					//exact match
					//qDebug("Exact match %s",g_nickColors[i]);
					KviMemory::free(comb);
					return i;
				}
				else
				{
					//partial match
					if(numc > numm)
					{
						//better than any previous one
						iBestMatch = i;
						numm = numc;
					}
				}
			}
		}
		KviMemory::free(comb);
		//no exact match
		//qDebug("Best match %s",g_nickColors[iBestMatch]);
		return iBestMatch;
	}
}
