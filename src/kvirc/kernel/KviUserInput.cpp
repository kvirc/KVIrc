//=============================================================================
//
//   File : KviUserInput.cpp
//   Creation date : Sun 25 Sep 2005 05:27:57 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
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

#include "KviUserInput.h"
#include "KviKvsVariantList.h"
#include "KviConsoleWindow.h"
#include "KviKvsScript.h"
#include "KviLocale.h"
#include "KviIrcConnection.h"
#include "KviIrcConnectionUserInfo.h"
#include "kvi_out.h"
#include "KviOptions.h"
#include "KviKvsEventTriggers.h"

namespace KviUserInput
{
	bool parse(QString & szData, KviWindow * pWindow, const QString & szContext, bool bUserFriendlyCommandline)
	{
		const QChar * b = szData.constData();
		const QChar * c = b;
		if(!c)
			return true; // empty

		if(!c->unicode())
			return true; // empty

		if(c->unicode() == '\\')
		{
			c++;
			if(c->unicode() != '/')
				c--;
		}
		else
		{
			if(c->unicode() == '/')
			{
				c++;
				if(c->unicode() != '/')
				{
					szData.remove(0, c - b);
					return parseCommand(szData, pWindow, szContext, bUserFriendlyCommandline);
				}
				else
				{
					// C++ comment, probably
					c--;
				}
			}
		}

		if(KVS_TRIGGER_EVENT_1_HALTED(KviEvent_OnTextInput, pWindow, szData))
			return true; // halted

		if(c != b)
			szData.remove(0, c - b);

		parseNonCommand(szData, pWindow);
		return true;
	}

	bool parseCommand(const QString & szData, KviWindow * pWindow, const QString & szContext, bool bUserFriendlyCommandline)
	{
		if(bUserFriendlyCommandline)
		{
			static QString szUserFriendlyCommandlineContext(__tr2qs("commandline::userfriendly"));

			QString szCmd = szData;
			KviQString::escapeKvs(&szCmd, KviQString::EscapeParenthesis);

			KviKvsScript kvs(szContext.isEmpty() ? szUserFriendlyCommandlineContext : szContext, szCmd);
			return (kvs.run(pWindow, nullptr, nullptr) != KviKvsScript::Error);
		}
		else
		{
			static QString szCommandlineContext(__tr2qs("commandline::kvs"));

			KviKvsScript kvs(szContext.isEmpty() ? szCommandlineContext : szContext, szData);
			return (kvs.run(pWindow, nullptr, nullptr /*,KviKvsScript::AssumeLocals*/) != KviKvsScript::Error);
		}
	}

	void parseNonCommand(QString & szData, KviWindow * pWindow)
	{
		const QChar * aux = szData.constData();
		const QChar * beg = aux;

		if(!beg)
			return; // empty

		while(aux->unicode())
		{
			while(aux->unicode() && (aux->unicode() != '\n'))
				aux++;

			QString buf(beg, aux - beg);
			if(aux->unicode() == '\n')
				aux++;
			beg = aux;

			if(buf.isEmpty())
				buf = " "; // avoid "No text to send" (d3vah)

			switch(pWindow->type())
			{
				case KviWindow::Console:
					if(pWindow->connection())
					{
						QByteArray data = pWindow->connection()->encodeText(buf);

						if(((KviConsoleWindow *)pWindow)->connection()->sendData(data.data()))
						{
							pWindow->output(KVI_OUT_RAW, "[RAW]: %Q", &buf);
							return;
						}
					}
					pWindow->output(KVI_OUT_PARSERERROR, __tr2qs("You're not connected to a server"));
					break;
				case KviWindow::Channel:
				case KviWindow::Query:
					if(pWindow->connection())
					{
						if(KVI_OPTION_BOOL(KviOption_boolExitAwayOnInput))
						{
							if(pWindow->connection()->userInfo()->isAway())
								parseCommand("back", pWindow->console());
						}
					}
					pWindow->ownMessage(buf);
					break;
				case KviWindow::DccChat:
				case KviWindow::DccVideo:
					pWindow->ownMessage(buf);
					break;
				default:
					// FIXME: Should pass the message somewhere ?.. a KviWindow handler ?
					break;
			}
		}
	}
};
