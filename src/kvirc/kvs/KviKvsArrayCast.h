#ifndef _KVI_KVS_ARRAYCAST_H_
#define _KVI_KVS_ARRAYCAST_H_
//=============================================================================
//
//   File : KviKvsArrayCast.h
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

#include "kvi_settings.h"
#include "KviKvsArray.h"

class KVIRC_API KviKvsArrayCast
{
protected:
	KviKvsArray * m_pArray = nullptr;
	bool m_bOwned = false;

public:
	KviKvsArrayCast() = default;
	~KviKvsArrayCast();

public:
	void clear();
	KviKvsArray * array()
	{
		return m_pArray;
	};
	void release()
	{
		m_bOwned = false;
	};
	void set(KviKvsArray * pArry, bool bOwned)
	{
		m_pArray = pArry;
		m_bOwned = bOwned;
	};
};

#endif //!_KVI_KVS_ARRAYCAST_H_
