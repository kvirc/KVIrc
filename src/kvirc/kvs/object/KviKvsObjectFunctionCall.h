#ifndef _KVI_KVS_OBJECT_FUNCTIONCALL_H_
#define _KVI_KVS_OBJECT_FUNCTIONCALL_H_
//=============================================================================
//
//   File : KviKvsObjectFunctionCall.h
//   Creation date : Sun 24 Apr 2005 04:05:54 by Szymon Stefanek
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

#include "KviKvsRunTimeCall.h"
#include "KviKvsVariantList.h"
#include "KviKvsVariant.h"

class KVIRC_API KviKvsObjectFunctionCall : public KviKvsRunTimeCall
{
protected:
	KviKvsVariant * m_pResult;

public:
	KviKvsObjectFunctionCall(KviKvsRunTimeContext * pContext,
	    KviKvsVariantList * pParams,
	    KviKvsVariant * pResult)
	    : KviKvsRunTimeCall(pContext, pParams), m_pResult(pResult){};
	~KviKvsObjectFunctionCall(){};

public:
	KviKvsVariant * returnValue() { return m_pResult; };
};

#endif //!_KVI_KVS_OBJECT_FUNCTIONCALL_H_
