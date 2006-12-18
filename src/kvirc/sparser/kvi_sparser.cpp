//=============================================================================
//
//   File : kvi_sparser.cpp
//   Creation date : Sun Jun 30 2000 03:25:17 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2000 Szymon Stefanek (pragma at kvirc dot net)
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
#define __KVIRC__
#define _KVI_SPARSER_CPP_
 
#include "kvi_sparser.h"
#include "kvi_window.h"
#include "kvi_out.h"
#include "kvi_locale.h"
#include "kvi_ircsocket.h"
#include "kvi_options.h"
#include "kvi_kvs_eventmanager.h"
#include "kvi_kvs_eventtriggers.h"

KviServerParser * g_pServerParser = 0;


KviServerParser::KviServerParser()
: QObject(0,"server_parser")
{
}

KviServerParser::~KviServerParser()
{
}

void KviServerParser::parseMessage(const char * message,KviIrcConnection * pConnection)
{
	if(*message != 0 )
	{
		KviIrcMessage msg(message,pConnection);
		if(msg.isNumeric())
		{
			if(KviKvsEventManager::instance()->hasRawHandlers(msg.numeric()))
			{
				KviKvsVariantList parms;
				parms.append(pConnection->decodeText(msg.safePrefix()));
				parms.append(pConnection->decodeText(msg.command()));
	
				for(KviStr * str = msg.firstParam();str;str = msg.nextParam())
					parms.append(pConnection->console()->decodeText(str->ptr()));

				if(KviKvsEventManager::instance()->triggerRaw(msg.numeric(),pConnection->console(),&parms))
					msg.setHaltOutput();
			}
	
			messageParseProc proc = m_numericParseProcTable[msg.numeric()];
			if(proc)
			{
				(this->*proc)(&msg);
				if(!msg.unrecognized())return; // parsed
			}
		} else {
			for(int i=0;m_literalParseProcTable[i].msgName;i++)
			{
				if(kvi_strEqualCS(m_literalParseProcTable[i].msgName,msg.command()))
				{
					(this->*(m_literalParseProcTable[i].proc))(&msg);
					if(!msg.unrecognized())return; // parsed
				}
			}
	
	
			if(KviKvsEventManager::instance()->hasAppHandlers(KviEvent_OnUnhandledLiteral))
			{
				KviKvsVariantList parms;
				parms.append(pConnection->decodeText(msg.safePrefix()));
				parms.append(pConnection->decodeText(msg.command()));
	
				for(KviStr * str = msg.firstParam();str;str = msg.nextParam())
					parms.append(pConnection->console()->decodeText(str->ptr()));
					
				if(KviKvsEventManager::instance()->trigger(KviEvent_OnUnhandledLiteral,pConnection->console(),&parms))
					msg.setHaltOutput();
			}
	
		}
	
		if(!msg.haltOutput() && !_OUTPUT_MUTE)
		{
			QString szWText = pConnection->decodeText(msg.allParams());
			if(msg.unrecognized())
			{
				pConnection->console()->output(KVI_OUT_UNRECOGNIZED,
					__tr2qs("[Server parser]: Encountered problems while parsing the following message:"));
				pConnection->console()->output(KVI_OUT_UNRECOGNIZED,
					__tr2qs("[Server parser]: [%s][%s] %Q"),msg.prefix(),msg.command(),&szWText);
				pConnection->console()->output(KVI_OUT_UNRECOGNIZED,
					__tr2qs("[Server parser]: %s"),m_szLastParserError.ptr());
	
			} else {
				// ignore spurious CRLF pairs (some servers send them a lot) unless we want PARANOID output
				if((!msg.isEmpty()) || _OUTPUT_PARANOIC)
					pConnection->console()->output(KVI_OUT_UNHANDLED,
						"[%s][%s] %Q",msg.prefix(),msg.command(),&szWText);
			}
		}
	}
}




#include "kvi_sparser.moc"
