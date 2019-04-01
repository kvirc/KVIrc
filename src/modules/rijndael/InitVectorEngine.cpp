//=============================================================================
//
//   File : InitVectorEngine.cpp
//   Creation date : Sun Feb 6 2011 22:25:10 CEST by Fabio Bas
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2011 Fabio Bas (ctrlaltca at gmail dot com)
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

#include "InitVectorEngine.h"

#include <cstdlib>
#include "KviTimeUtils.h"

namespace InitVectorEngine
{

	bool bDidInit = false;

	int fillRandomIV(unsigned char * pIn, int iLen)
	{
		if(!bDidInit)
		{
			srand((int)kvi_unixTime());
			bDidInit = true;
		}

		for(int i = 0; i < iLen; i++)
			pIn[i] = (unsigned char)(rand() % 256);

		return iLen;
	}
};
