//=============================================================================
//
//   File : KviKvsArrayCast.cpp
//   Creation date : Tue 10 May 2005 00:05:50 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "KviKvsArrayCast.h"

KviKvsArrayCast::~KviKvsArrayCast()
{
	if(m_pArray)
		if(m_bOwned)
			delete m_pArray;
}

void KviKvsArrayCast::clear()
{
	if(m_pArray)
	{
		if(m_bOwned)
			delete m_pArray;
		m_pArray = nullptr;
	}
}
