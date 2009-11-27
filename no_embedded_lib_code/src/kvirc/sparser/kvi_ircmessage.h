#ifndef _KVI_IRCMESSAGE_H_
#define _KVI_IRCMESSAGE_H_
//=============================================================================
//
//   File : kvi_ircmessage.h
//   Creation date : Fri Aug  2 23:08:55 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002-2008 Szymon Stefanek (pragma at kvirc dot net)
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
#include "kvi_pointerlist.h"
#include "kvi_string.h"
#include "kvi_qstring.h"
#include "kvi_console.h"

class KviIrcConnection;
class KviIrcContext;
class KviConneciton;

#define KVI_IRCMSG_FLAG_HALTOUTPUT 1
#define KVI_IRCMSG_FLAG_UNRECOGNIZED 2

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
private:
	const char          * m_ptr;                // shallow! never null
	KviStr                m_szPrefix;           // the extracted prefix string
	KviStr                m_szCommand;          // the extracted command (may be numeric)
	KviPointerList<KviStr>  * m_pParams;            // the list of parameters
	KviConsole          * m_pConsole;           // the console we're attacched to
	KviIrcConnection    * m_pConnection;        // the connection we're attacched to
	int                   m_iNumericCommand;    // the numeric of the command (0 if non numeric)
	int                   m_iFlags;             // yes.. flags :D
public:
	KviConsole       * console(){ return m_pConsole; };
	KviIrcConnection * connection(){ return m_pConsole->connection(); };

	bool               isNumeric(){ return (m_iNumericCommand >= 0); };
	const char       * command(){ return m_szCommand.ptr(); };
	KviStr           * commandPtr(){ return &m_szCommand; };
	int                numeric(){ return m_iNumericCommand; };

	KviStr           * prefixPtr(){ return &m_szPrefix; };
	const char       * prefix(){ return m_szPrefix.ptr(); };
	const char       * safePrefix();
	bool               hasPrefix(){ return m_szPrefix.hasData(); };

	bool               isEmpty(){ return (m_szPrefix.isEmpty() && m_szCommand.isEmpty() && m_pParams->isEmpty()); };

	int                paramCount(){ return m_pParams->count(); };

	const char       * param(unsigned int idx){ return (idx < m_pParams->count()) ? m_pParams->at(idx)->ptr() : 0; };

	const char       * safeParam(unsigned int idx){ return (idx < m_pParams->count()) ? m_pParams->at(idx)->ptr() : KviStr::emptyString().ptr(); };

	KviStr           * paramString(unsigned int idx){ return m_pParams->at(idx); };

	const char       * trailing(){ KviStr * tr = m_pParams->last(); return tr ? tr->ptr() : 0; };
	KviStr           * trailingString(){ return m_pParams->last(); };
	KviStr           & safeTrailingString(){ KviStr * tr = m_pParams->last(); return tr ? *tr : KviStr::emptyString(); };
	const char       * safeTrailing(){ KviStr * tr = m_pParams->last(); return tr ? tr->ptr() : KviStr::emptyString().ptr(); };

	const char       * allParams(){ return m_ptr; };

	KviStr           * firstParam(){ return m_pParams->first(); };
	KviStr           * nextParam(){ return m_pParams->next(); };

	void               setHaltOutput(){ m_iFlags |= KVI_IRCMSG_FLAG_HALTOUTPUT; };
	bool               haltOutput(){ return (m_iFlags & KVI_IRCMSG_FLAG_HALTOUTPUT); };
	void               setUnrecognized(){ m_iFlags |= KVI_IRCMSG_FLAG_UNRECOGNIZED; };
	bool               unrecognized(){ return (m_iFlags & KVI_IRCMSG_FLAG_UNRECOGNIZED); };

	void decodeAndSplitPrefix(QString &szNick,QString &szUser,QString &szHost);
	void decodeAndSplitMask(char * mask,QString &szNick,QString &szUser,QString &szHost);
};

#endif //_KVI_IRCMESSAGE_H_
