#ifndef _KVI_KVS_REPORT_H_
#define _KVI_KVS_REPORT_H_
//=============================================================================
//
//   File : KviKvsReport.h
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

#include "kvi_settings.h"
#include "KviQString.h"
#include "KviPointerList.h"

class KviWindow;

class KVIRC_API KviKvsReport
{
public:
	enum Type
	{
		RunTimeError,
		RunTimeWarning,
		ParserError,
		ParserWarning
	};

public:
	KviKvsReport(Type t, QString szContext, QString szMessage, QString szLocation, KviWindow * pWindow);
	~KviKvsReport();

protected:
	Type m_eType;

	// mandatory
	QString m_szContext;  // context name (script name, usually)
	QString m_szMessage;  // report message, always present
	QString m_szLocation; // line and col description (may be some thing like "at end of input")

	KviWindow * m_pWindow; // the window that the script was attacched to

	// optional
	KviPointerList<QString> * m_pCodeListing; // code listing, if present, it is owned
	KviPointerList<QString> * m_pCallStack;   // call stack, if present, it is owned
public:
	Type type() const { return m_eType; };

	KviWindow * window() { return m_pWindow; };
	const QString & context() { return m_szContext; };
	const QString & message() { return m_szMessage; };
	const QString & location() { return m_szLocation; };
	KviPointerList<QString> * codeListing() { return m_pCodeListing; };
	KviPointerList<QString> * callStack() { return m_pCallStack; };

	void setContext(const QString & szContext) { m_szContext = szContext; };
	void setMessage(const QString & szMessage) { m_szMessage = szMessage; };
	void setLocation(const QString & szLocation) { m_szLocation = szLocation; };
	void setCodeListing(KviPointerList<QString> * pListing) { m_pCodeListing = pListing; };
	void setCallStack(KviPointerList<QString> * pStack) { m_pCallStack = pStack; };

	static void findLineAndCol(const QChar * pBegin, const QChar * pPoint, int & iLine, int & iCol);
	static void findLineColAndListing(const QChar * pBegin, const QChar * pPoint, int & iLine, int & iCol, KviPointerList<QString> * pListing);

	static void report(KviKvsReport * r, KviWindow * pOutput);
};

#endif //!_KVI_KVS_REPORT_H_
