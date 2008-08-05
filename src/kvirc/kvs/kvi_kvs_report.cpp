//=============================================================================
//
//   File : kvi_kvs_report.cpp
//   Creation date : Thu 25 Sep 2003 05.12 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2003 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your opinion) any later version.
//
//   This program is distributed in the HOPE that it will be USEFUL,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program. If not, write to the Free Software Foundation,
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#include "kvi_kvs_report.h"
#include "kvi_mirccntrl.h"
#include "kvi_window.h"
#include "kvi_out.h"
#include "kvi_app.h"
#include "kvi_locale.h"
#include "kvi_debugwindow.h"
#include "kvi_options.h"

KviKvsReport::KviKvsReport(Type t,const QString &szContext,const QString &szMessage,const QString &szLocation,KviWindow * pWindow)
: m_eType(t), m_szContext(szContext), m_szMessage(szMessage), m_szLocation(szLocation), m_pWindow(pWindow)
{
	m_pCallStack = 0;
	m_pCodeListing = 0;
}

KviKvsReport::~KviKvsReport()
{
	if(m_pCallStack)delete m_pCallStack;
	if(m_pCodeListing)delete m_pCodeListing;
}

void KviKvsReport::findLineAndCol(const QChar * pBegin,const QChar * pPoint,int &iLine,int &iCol)
{
	iLine = 1;

	const QChar * pBufferBegin = pBegin;
	const QChar * pPrevLine = 0;
	const QChar * pLineBegin = pBegin;

	unsigned short us = pBegin->unicode();

	while(us && (pBegin < pPoint))
	{
		if(us == '\n')
		{
			pPrevLine = pLineBegin;
			pBegin++;
			pLineBegin = pBegin;
			iLine++;
		} else {
			pBegin++;
		}
		us = pBegin->unicode();
	}
	
	iCol = (pBegin - pLineBegin) + 1;
}



void KviKvsReport::findLineColAndListing(const QChar * pBegin,const QChar * pPoint,int &iLine,int &iCol,KviPointerList<QString> * pListing)
{
	iLine = 1;

	const QChar * pBufferBegin = pBegin;
	const QChar * pPrevLine = 0;
	const QChar * pLineBegin = pBegin;

	unsigned short us = pBegin->unicode();

	while(us && (pBegin < pPoint))
	{
		if(us == '\n')
		{
			pPrevLine = pLineBegin;
			pBegin++;
			pLineBegin = pBegin;
			iLine++;
		} else {
			pBegin++;
		}
		us = pBegin->unicode();
	}
	
	iCol = (pBegin - pLineBegin) + 1;
	
	// previous line
	if(pPrevLine)
	{
		// there would be yet another line before
		if(pPrevLine > pBufferBegin)
		{
			QString * pListingStrZ = new QString();
			KviQString::sprintf(*pListingStrZ,"%d ...",iLine - 2);
			pListing->append(pListingStrZ);
		}

		QString * pListingStr = new QString();
		KviQString::sprintf(*pListingStr,"%d ",iLine - 1);
		*pListingStr += QString(pPrevLine,pLineBegin - pPrevLine);
		pListingStr->replace("\n","");
		pListing->append(pListingStr);
	}
	
	// current line
	pBegin = pLineBegin;

	us = pBegin->unicode();
	while(us && (us != '\n'))
	{
		pBegin++;
		us = pBegin->unicode();
	}
	if(us)pBegin++;

	{
		QString * pListingStr = new QString();
		KviQString::sprintf(*pListingStr,"%c%d ",KVI_TEXT_BOLD,iLine);
		*pListingStr += QString(pLineBegin,pBegin - pLineBegin);
		pListingStr->replace("\n","");
		pListing->append(pListingStr);
	}

	if(us)
	{
		// next line
		pLineBegin = pBegin;
	
		us = pBegin->unicode();
		while(us && (us != '\n'))
		{
			pBegin++;
			us = pBegin->unicode();
		}
		if(us)pBegin++;
	
		{
			QString * pListingStr = new QString();
			KviQString::sprintf(*pListingStr,"%d ",iLine + 1);
			*pListingStr += QString(pLineBegin,pBegin - pLineBegin);
			pListingStr->replace("\n","");
			pListing->append(pListingStr);
		}
	
		// there would be yet another line
		if(us)
		{
			QString * pListingStr = new QString();
			KviQString::sprintf(*pListingStr,"%d ...",iLine + 2);
			pListing->append(pListingStr);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// ERROR REPORTING

void KviKvsReport::report(KviKvsReport * r,KviWindow * pOutput)
{
	if(!pOutput)return; // ?
	if(!g_pApp->windowExists(pOutput))
	{
		if(KVI_OPTION_BOOL(KviOption_boolScriptErrorsToDebugWindow))
		{
			// rethrow to the debug window
			report(r,KviDebugWindow::getInstance());
		} // else window lost: unrecoverable
		return;
	}

	// make sure that the output window still exists!

	int out;
		
	switch(r->type())
	{
		case KviKvsReport::ParserWarning:
			out = KVI_OUT_PARSERWARNING;
			pOutput->output(out,__tr2qs("[KVS]%c Warning: %Q"),KVI_TEXT_BOLD,&(r->message()));
		break;
		case KviKvsReport::ParserError:
			out = KVI_OUT_PARSERERROR;
			pOutput->output(out,__tr2qs("[KVS]%c Compilation Error: %Q"),KVI_TEXT_BOLD,&(r->message()));
		break;
		case KviKvsReport::RunTimeWarning:
			out = KVI_OUT_PARSERWARNING;
			pOutput->output(out,__tr2qs("[KVS]%c Warning: %Q"),KVI_TEXT_BOLD,&(r->message()));
		break;
		case KviKvsReport::RunTimeError:
			out = KVI_OUT_PARSERERROR;
			pOutput->output(out,__tr2qs("[KVS]%c Runtime Error: %Q"),KVI_TEXT_BOLD,&(r->message()));
		break;
	}
	
	if(r->location().isEmpty())
		pOutput->output(out,__tr2qs("[KVS]   in script context \"%Q\""),&(r->context()));
	else
		pOutput->output(out,__tr2qs("[KVS]   in script context \"%Q\", %Q"),&(r->context()),&(r->location()));
	
	if(pOutput == KviDebugWindow::instance())
	{
		KviPointerList<QString> * l;
		if(l = r->codeListing())
		{
			pOutput->outputNoFmt(out,__tr2qs("[KVS] Code listing:"));
			for(QString * s = l->first();s;s = l->next())
				pOutput->output(out,"[KVS]   %Q",s);
		}
	
		pOutput->output(out,__tr2qs("[KVS] Window:"));
			if(g_pApp->windowExists(r->window()))
				pOutput->output(out,"[KVS]   %Q [id: %u]",&(r->window()->windowName()),r->window()->numericId());
			else
				pOutput->output(out,__tr2qs("[KVS]   Destroyed window with pointer %x"),r->window());

		if(l = r->callStack())
		{
			pOutput->outputNoFmt(out,__tr2qs("[KVS] Call stack:"));
			for(QString * s = l->first();s;s = l->next())
				pOutput->output(out,"[KVS]   %Q",s);
		}

		pOutput->outputNoFmt(out,"[KVS]");
	} else {
		if(KVI_OPTION_BOOL(KviOption_boolScriptErrorsToDebugWindow))
		{
			// rethrow to the debug window
			if(pOutput != KviDebugWindow::getInstance())
				report(r,KviDebugWindow::getInstance());
		}
	}
}
