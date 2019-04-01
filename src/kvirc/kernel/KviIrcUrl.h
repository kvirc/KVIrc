#ifndef _KVI_IRCURL_H_
#define _KVI_IRCURL_H_
//=============================================================================
//
//   File : KviIrcUrl.h
//   Creation date : Sun Mar 04 2001 14:22:55 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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
#include "KviCString.h"
#include "kvi_inttypes.h"

class KviIrcServer;
class KviConsoleWindow;

#include <QStringList>

// Create /server -u <server> commands (first free context)
#define KVI_IRCURL_CONTEXT_FIRSTFREE 0
// Create /server -n <server> commands (new irc context)
#define KVI_IRCURL_CONTEXT_NEW 1
// Create /server <server> commands (this irc context)
#define KVI_IRCURL_CONTEXT_THIS 2

struct KviIrcUrlParts
{
	QString szHost;
	kvi_u32_t iPort;
	bool bIPv6;
	bool bSsl;
	QStringList chanList;
	int iError;
};

namespace KviIrcUrl
{

	enum KviIrcUrlParserError
	{
		InvalidProtocol = 1,
		InvalidPort = 2,
		NeedNewContext = 4,
		InvalidUrl = 8
	};

	enum KviIrcUrlContextSpec
	{
		FirstFreeContext = 1,
		NewContext = 2,
		CurrentContext = 4,
		TryCurrentContext = 8,
		TryEveryContext = 32,

		DoNotPartChans = 16
	};

	extern KVIRC_API bool parse(const char * url, KviCString & cmdBuffer, int contextSpec = KVI_IRCURL_CONTEXT_FIRSTFREE);

	extern KVIRC_API int run(const QString & url, int contextSpec = FirstFreeContext, KviConsoleWindow * pConsole = nullptr);

	extern KVIRC_API void split(const QString & url, KviIrcUrlParts & parts);
	extern KVIRC_API void join(QString & url, KviIrcServer * server);
	extern KVIRC_API void makeJoinCmd(const QStringList & chans, QString & szJoinCommand);
}

#endif // _KVI_IRCURL_H_
