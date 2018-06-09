//=============================================================================
//
//   File : KviKvsReport.cpp
//   Creation date : Thu 25 Sep 2003 05.12 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviKvsReport.h"

#include <utility>
#include "KviControlCodes.h"
#include "KviWindow.h"
#include "kvi_out.h"
#include "KviApplication.h"
#include "KviLocale.h"
#include "KviDebugWindow.h"
#include "KviOptions.h"

KviKvsReport::KviKvsReport(Type t, QString szContext, QString szMessage, QString szLocation, KviWindow * pWindow)
    : m_eType(t)
    , m_szContext(std::move(szContext))
    , m_szMessage(std::move(szMessage))
    , m_szLocation(std::move(szLocation))
    , m_pWindow(pWindow)
{
	m_pCallStack = nullptr;
	m_pCodeListing = nullptr;
}

KviKvsReport::~KviKvsReport()
{
	if(m_pCallStack)
		delete m_pCallStack;
	if(m_pCodeListing)
		delete m_pCodeListing;
}

void KviKvsReport::findLineAndCol(const QChar * pBegin, const QChar * pPoint, int & iLine, int & iCol)
{
	iLine = 1;

	const QChar * pLineBegin = pBegin;

	unsigned short us = pBegin->unicode();

	while(us && (pBegin < pPoint))
	{
		if(us == '\n')
		{
			pBegin++;
			pLineBegin = pBegin;
			iLine++;
		}
		else
		{
			pBegin++;
		}
		us = pBegin->unicode();
	}

	iCol = (pBegin - pLineBegin) + 1;
}

void KviKvsReport::findLineColAndListing(const QChar * pBegin, const QChar * pPoint, int & iLine, int & iCol, KviPointerList<QString> * pListing)
{
	iLine = 1;

	const QChar * pBufferBegin = pBegin;
	const QChar * pPrevLine = nullptr;
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
		}
		else
		{
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
			QString * pListingStrZ = new QString(QString("%1 ...").arg(iLine - 2));
			pListing->append(pListingStrZ);
		}

		QString * pListingStr = new QString(QString("%1 ").arg(iLine - 1));
		*pListingStr += QString(pPrevLine, pLineBegin - pPrevLine);
		pListingStr->replace("\n", "");
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
	if(us)
		pBegin++;

	{
		QString * pListingStr = new QString(QString("%1%2 ").arg(QChar(KviControlCodes::Bold)).arg(iLine));
		*pListingStr += QString(pLineBegin, pBegin - pLineBegin);
		pListingStr->replace("\n", "");
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
		if(us)
			pBegin++;

		{
			QString * pListingStr = new QString(QString("%1 ").arg(iLine + 1));
			*pListingStr += QString(pLineBegin, pBegin - pLineBegin);
			pListingStr->replace("\n", "");
			pListing->append(pListingStr);
		}

		// there would be yet another line
		if(us)
		{
			QString * pListingStr = new QString(QString("%1 ...").arg(iLine + 2));
			pListing->append(pListingStr);
		}
	}
}

//
// ERROR REPORTING
//

void KviKvsReport::report(KviKvsReport * r, KviWindow * pOutput)
{
	if(!pOutput)
		return; // ?

	if(!g_pApp->windowExists(pOutput))
	{
		if(KVI_OPTION_BOOL(KviOption_boolScriptErrorsToDebugWindow))
		{
			// rethrow to the debug window
			report(r, KviDebugWindow::getInstance());
		} // else window lost: unrecoverable
		return;
	}

	// make sure that the output window still exists!

	int out = 0;

	switch(r->type())
	{
		case KviKvsReport::ParserWarning:
			out = KVI_OUT_PARSERWARNING;
			pOutput->output(out, __tr2qs_ctx("[KVS]%c Warning: %Q", "kvs"), KviControlCodes::Bold, &(r->message()));
			break;
		case KviKvsReport::ParserError:
			out = KVI_OUT_PARSERERROR;
			pOutput->output(out, __tr2qs_ctx("[KVS]%c Compilation error: %Q", "kvs"), KviControlCodes::Bold, &(r->message()));
			break;
		case KviKvsReport::RunTimeWarning:
			out = KVI_OUT_PARSERWARNING;
			pOutput->output(out, __tr2qs_ctx("[KVS]%c Warning: %Q", "kvs"), KviControlCodes::Bold, &(r->message()));
			break;
		case KviKvsReport::RunTimeError:
			out = KVI_OUT_PARSERERROR;
			pOutput->output(out, __tr2qs_ctx("[KVS]%c Runtime error: %Q", "kvs"), KviControlCodes::Bold, &(r->message()));
			break;
	}

	if(r->location().isEmpty())
		pOutput->output(out, __tr2qs_ctx("[KVS]   In script context \"%Q\"", "kvs"), &(r->context()));
	else
		pOutput->output(out, __tr2qs_ctx("[KVS]   In script context \"%Q\", %Q", "kvs"), &(r->context()), &(r->location()));

	if(pOutput == KviDebugWindow::instance())
	{
		KviPointerList<QString> * l;
		if((l = r->codeListing()))
		{
			pOutput->outputNoFmt(out, __tr2qs_ctx("[KVS] Code listing:", "kvs"));
			for(QString * s = l->first(); s; s = l->next())
				pOutput->output(out, "[KVS]   %Q", s);
		}

		pOutput->output(out, __tr2qs_ctx("[KVS] Window:", "kvs"));
		if(g_pApp->windowExists(r->window()))
			pOutput->output(out, "[KVS]   %Q [ID: %u]", &(r->window()->windowName()), r->window()->numericId());
		else
			pOutput->output(out, __tr2qs_ctx("[KVS]   Destroyed window with pointer %x", "kvs"), r->window());

		if((l = r->callStack()))
		{
			pOutput->outputNoFmt(out, __tr2qs_ctx("[KVS] Call stack:", "kvs"));
			for(QString * s = l->first(); s; s = l->next())
				pOutput->output(out, "[KVS]   %Q", s);
		}

		pOutput->outputNoFmt(out, "[KVS]");
	}
	else
	{
		if(KVI_OPTION_BOOL(KviOption_boolScriptErrorsToDebugWindow))
		{
			// rethrow to the debug window
			if(pOutput != KviDebugWindow::getInstance())
				report(r, KviDebugWindow::getInstance());
		}
	}
}
