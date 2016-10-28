//=============================================================================
//
//   File : KviIrcServerParser.cpp
//   Creation date : Sun Jun 30 2000 03:25:17 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#define _KVI_SPARSER_CPP_

#include "KviIrcServerParser.h"
#include "KviWindow.h"
#include "kvi_out.h"
#include "KviLocale.h"
#include "KviIrcSocket.h"
#include "KviOptions.h"
#include "KviKvsEventManager.h"
#include "KviKvsEventTriggers.h"
#include "KviIrcConnectionStateData.h"
#include "KviIrcMessage.h"

KviIrcServerParser * g_pServerParser = nullptr;

KviIrcServerParser::KviIrcServerParser()
    : QObject(nullptr)
{
	setObjectName("server_parser");
}

KviIrcServerParser::~KviIrcServerParser()
    = default;

void KviIrcServerParser::parseMessage(const char * message, KviIrcConnection * pConnection)
{
	if(message == nullptr || message[0] == '\0')
		return;

	KviIrcMessage msg(message, pConnection);

	if(msg.isNumeric())
	{
		if(KviKvsEventManager::instance()->hasRawHandlers(msg.numeric()))
		{
			KviKvsVariantList parms;
			parms.append(pConnection->decodeText(msg.safePrefix()));
			parms.append(pConnection->decodeText(msg.command()));

			for(auto & str : msg.params())
				parms.append(pConnection->console()->decodeText(str.ptr()));

			if(KviKvsEventManager::instance()->triggerRaw(msg.numeric(), pConnection->console(), &parms))
				msg.setHaltOutput();
		}

		messageParseProc proc = m_numericParseProcTable[msg.numeric()];
		if(proc)
		{
			(this->*proc)(&msg);
			if(!msg.unrecognized())
				return; // parsed
		}
		else
		{
			// we don't have a proc for this

			// special handling of unknown RPL_WHOIS* messages
			// if
			//      - we're in the middle of a RPL_WHOIS* sequence (i.e. have received a RPL_WHOIS* message since less than 10 seconds)
			//      - we have not received RPL_ENDOFWHOIS yet (the time of the last RPL_WHOIS* is reset to zero when a RPL_ENDOFWHOIS is received)
			//      - this message is unrecognized and looks like a RPL_WHOIS*
			// then pass it to the WhoisOther handler.
			//
			// Thnx Elephantman :)

			if(msg.paramCount() >= 3) // might look like :prefix RPL_WHOIS* <target> <nick> [?] :<something>
			{
				kvi_time_t tNow = kvi_unixTime();

				if((tNow - pConnection->stateData()->lastReceivedWhoisReply()) < 10)
				{
					// we're in the middle of a RPL_WHOIS* sequence and haven't
					// received a RPL_ENDOFWHOIS yet.
					parseNumericWhoisOther(&msg);
					if(!msg.unrecognized())
						return;
				}
			}
		}
	}
	else
	{
		for(int i = 0; m_literalParseProcTable[i].msgName; i++)
			if(kvi_strEqualCS(m_literalParseProcTable[i].msgName, msg.command()))
			{
				(this->*(m_literalParseProcTable[i].proc))(&msg);
				if(!msg.unrecognized())
					return; // parsed
			}

		if(KviKvsEventManager::instance()->hasAppHandlers(KviEvent_OnUnhandledLiteral))
		{
			KviKvsVariantList parms;
			parms.append(pConnection->decodeText(msg.safePrefix()));
			parms.append(pConnection->decodeText(msg.command()));

			for(auto & str : msg.params())
				parms.append(pConnection->console()->decodeText(str.ptr()));

			if(KviKvsEventManager::instance()->trigger(KviEvent_OnUnhandledLiteral, pConnection->console(), &parms))
				msg.setHaltOutput();
		}
	}

	// unhandled || unrecognized
	if(!msg.haltOutput() && !_OUTPUT_MUTE)
	{
		QString szWText = pConnection->decodeText(msg.allParams());
		if(msg.unrecognized())
		{
			pConnection->console()->output(KVI_OUT_UNRECOGNIZED,
			    __tr2qs("[Server parser]: encountered problems while parsing the following message:"));
			pConnection->console()->output(KVI_OUT_UNRECOGNIZED,
			    __tr2qs("[Server parser]: [%s][%s] %Q"), msg.prefix(), msg.command(), &szWText);
			pConnection->console()->output(KVI_OUT_UNRECOGNIZED,
			    __tr2qs("[Server parser]: %s"), m_szLastParserError.ptr());
		}
		else
		{
			// ignore spurious CRLF pairs (some servers send them a lot) unless we want PARANOID output
			if((!msg.isEmpty()) || _OUTPUT_PARANOIC)
				pConnection->console()->output(KVI_OUT_UNHANDLED,
				    "[%s][%s] %Q", msg.prefix(), msg.command(), &szWText);
		}
	}
}
