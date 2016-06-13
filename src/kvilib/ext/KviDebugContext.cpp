//=============================================================================
//
//   File : KviDebugContext.cpp
//   Creation date : Tue Jul 13 2010 23:27:23 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 1999-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviDebugContext.h"
#include "kvi_stdarg.h"
#include "kvi_debug.h"

static KviCString g_szIndentPrefix;

KviDebugContext::KviDebugContext(const char * pcContext, ...)
{
	kvi_va_list list;
	kvi_va_start(list, pcContext);

	m_szContext.vsprintf(pcContext, list);

	kvi_va_end(list);

	static KviCString szEnterMark = ">> %s";

	KviCString szPrefix = g_szIndentPrefix;
	szPrefix.append(szEnterMark);

	qDebug(szPrefix.ptr(), m_szContext.ptr());

	g_szIndentPrefix.append("  ");
}

KviDebugContext::~KviDebugContext()
{
	KVI_ASSERT(g_szIndentPrefix.len() >= 2);

	g_szIndentPrefix.cutRight(2);

	static KviCString szExitMark = "<< %s";

	KviCString szPrefix = g_szIndentPrefix;
	szPrefix.append(szExitMark);

	qDebug(szPrefix.ptr(), m_szContext.ptr());
}

void KviDebugContext::trace(const char * pcFmt, ...)
{
	kvi_va_list list;
	kvi_va_start(list, pcFmt);

	KviCString szOut;
	szOut.vsprintf(pcFmt, list);

	kvi_va_end(list);

	static KviCString szEnterMark = "-- %s";

	KviCString szPrefix = g_szIndentPrefix;
	szPrefix.append(szEnterMark);

	qDebug(szPrefix.ptr(), szOut.ptr());
}
