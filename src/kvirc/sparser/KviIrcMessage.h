#ifndef _KVI_IRCMESSAGE_H_
#define _KVI_IRCMESSAGE_H_
//=============================================================================
//
//   File : KviIrcMessage.h
//   Creation date : Fri Aug  2 23:08:55 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include "kvi_settings.h"
#include "KviPointerList.h"
#include "KviCString.h"
#include "KviQString.h"
#include "KviConsoleWindow.h"

class KviIrcConnection;
class KviIrcContext;
class KviConneciton;


//
// This is a single IRC message received from the server.
// The message is strictly associated to the connection it came from.
//
// The IRC protocol is strictly 8 bit and thus the strings
// are all 8 bit strings. The decoding of these strings should
// be done on the targeting context (mainly channel or query...)
//

class KVIRC_API KviIrcMessage
{
public:

	KviIrcMessage(const char * message,KviIrcConnection * pConnection);

	~KviIrcMessage();

public:

	///
	/// The message flags.
	///
	enum Flags
	{
		///
		/// Some message processing step (most likely a script handler)
		/// requested to stop further message related output.
		///
		HaltOutput = 1,
		///
		/// The message is marked as unrecognized.
		///
		Unrecognized = 2
	};

private:
	const char * m_ptr;                // shallow! never null
	KviCString m_szPrefix;           // the extracted prefix string
	KviCString m_szCommand;          // the extracted command (may be numeric)
	KviPointerList<KviCString> * m_pParams;            // the list of parameters
	KviConsoleWindow * m_pConsole;           // the console we're attacched to
	KviIrcConnection * m_pConnection;        // the connection we're attacched to
	int m_iNumericCommand;    // the numeric of the command (0 if non numeric)
	int m_iFlags;             // yes.. flags :D
public:
	KviConsoleWindow * console(){ return m_pConsole; };
	KviIrcConnection * connection(){ return m_pConsole->connection(); };

	bool isNumeric(){ return (m_iNumericCommand >= 0); };
	const char * command(){ return m_szCommand.ptr(); };
	KviCString * commandPtr(){ return &m_szCommand; };
	int numeric(){ return m_iNumericCommand; };

	KviCString * prefixPtr(){ return &m_szPrefix; };
	const char * prefix(){ return m_szPrefix.ptr(); };
	const char * safePrefix();
	bool hasPrefix(){ return m_szPrefix.hasData(); };

	bool isEmpty(){ return (m_szPrefix.isEmpty() && m_szCommand.isEmpty() && m_pParams->isEmpty()); };

	int paramCount(){ return m_pParams->count(); };

	const char * param(unsigned int idx){ return (idx < m_pParams->count()) ? m_pParams->at(idx)->ptr() : 0; };

	const char * safeParam(unsigned int idx){ return (idx < m_pParams->count()) ? m_pParams->at(idx)->ptr() : KviCString::emptyString().ptr(); };

	KviCString * paramString(unsigned int idx){ return m_pParams->at(idx); };

	const char * trailing(){ KviCString * tr = m_pParams->last(); return tr ? tr->ptr() : 0; };
	KviCString * trailingString(){ return m_pParams->last(); };
	KviCString & safeTrailingString(){ KviCString * tr = m_pParams->last(); return tr ? *tr : KviCString::emptyString(); };
	const char * safeTrailing(){ KviCString * tr = m_pParams->last(); return tr ? tr->ptr() : KviCString::emptyString().ptr(); };

	const char * allParams(){ return m_ptr; };

	KviCString * firstParam(){ return m_pParams->first(); };
	KviCString * nextParam(){ return m_pParams->next(); };

	void setHaltOutput(){ m_iFlags |= HaltOutput; };
	bool haltOutput(){ return (m_iFlags & HaltOutput); };
	void setUnrecognized(){ m_iFlags |= Unrecognized; };
	bool unrecognized(){ return (m_iFlags & Unrecognized); };

	void decodeAndSplitPrefix(QString &szNick,QString &szUser,QString &szHost);
	void decodeAndSplitMask(char * mask,QString &szNick,QString &szUser,QString &szHost);
};

#endif //_KVI_IRCMESSAGE_H_
