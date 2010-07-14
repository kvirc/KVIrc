//=============================================================================
//
// File : kvi_debugcontext.cpp
// Creation date : Tue 13 Jul 2010 23:27:23
// Project : KVIrc Irc client
// Author : Szymon Stefanek <pragma at kvirc dot net>
//
//=============================================================================

#include "kvi_debugcontext.h"

#include "kvi_stdarg.h"
#include "kvi_string.h"
#include "kvi_debug.h"


static KviStr g_szIndentPrefix;

KviDebugContext::KviDebugContext(const char * szContext,...)
{
	kvi_va_list list;
	kvi_va_start(list,szContext);

	m_szContext.vsprintf(szContext,list);

	kvi_va_end(list);
	
	static KviStr szEnterMark = ">> %s";
	
	KviStr szPrefix = g_szIndentPrefix;
	szPrefix.append(szEnterMark);
	
	qDebug(szPrefix.ptr(),m_szContext.ptr());
	
	g_szIndentPrefix.append("  ");
}


KviDebugContext::~KviDebugContext()
{
	KVI_ASSERT(g_szIndentPrefix.len() >= 2);

	g_szIndentPrefix.cutRight(2);

	static KviStr szExitMark = "<< %s";

	KviStr szPrefix = g_szIndentPrefix;
	szPrefix.append(szExitMark);

	qDebug(szPrefix.ptr(),m_szContext.ptr());
}

void KviDebugContext::trace(const char * szFmt,...)
{
	kvi_va_list list;
	kvi_va_start(list,szFmt);

	KviStr szOut;
	szOut.vsprintf(szFmt,list);

	kvi_va_end(list);
	
	static KviStr szEnterMark = "-- %s";
	
	KviStr szPrefix = g_szIndentPrefix;
	szPrefix.append(szEnterMark);
	
	qDebug(szPrefix.ptr(),szOut.ptr());
}

