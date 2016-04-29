//=============================================================================
//
//   File : KviKvsRunTimeCall.cpp
//   Creation date : Sat 23 Apr 2005 18:19:38 by Szymon Stefanek
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

#include "KviKvsRunTimeCall.h"

void KviKvsRunTimeCall::warning(QString szFmt, ...)
{
	kvi_va_list va;
	kvi_va_start(va, szFmt);
	m_pContext->report(false, m_pContext->defaultReportLocation(), szFmt, va);
	kvi_va_end(va);
}

bool KviKvsRunTimeCall::error(QString szFmt, ...)
{
	kvi_va_list va;
	kvi_va_start(va, szFmt);
	m_pContext->report(true, m_pContext->defaultReportLocation(), szFmt, va);
	kvi_va_end(va);
	return false;
}

bool KviKvsRunTimeCall::getParameterCode(unsigned int /*uParamIdx*/, QString & /*szParamBuffer*/)
{
	return false;
}
