#ifndef _KVI_IRCURL_H_
#define _KVI_IRCURL_H_
//=============================================================================
//
//   File : kvi_ircurl.h
//   Creation date : Sun Mar 04 2001 14:22:55 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2008 Szymon Stefanek (pragma at kvirc dot net)
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
#include "kvi_string.h"
#include "kvi_inttypes.h"

class KviServer;
class KviConsole;

#include <QStringList>

// Create /server -u <server> commands (first free context)
#define KVI_IRCURL_CONTEXT_FIRSTFREE 0
// Create /server -n <server> commands (new irc context)
#define KVI_IRCURL_CONTEXT_NEW 1
// Create /server <server> commands (this irc context)
#define KVI_IRCURL_CONTEXT_THIS 2

typedef  struct _KviIrcUrlParts {
		QString szHost;
		kvi_u32_t iPort;
		bool bIPv6;
		bool bSsl;
		QStringList chanList;
		int iError;
	} KviIrcUrlParts;

namespace KviIrcUrl
{

	enum KviIrcUrlParserError {
		InvalidProtocol = 1,
		InvalidPort = 2,
		NeedNewContext = 4
	};

	enum KviIrcUrlContextSpec {
		FirstFreeContext = 1,
		NewContext = 2,
		CurrentContext = 4,
		TryCurrentContext = 8,

		DoNotPartChans = 16
	};

	extern KVIRC_API bool parse(const char * url,KviStr &cmdBuffer,int contextSpec = KVI_IRCURL_CONTEXT_FIRSTFREE);

	extern KVIRC_API int  run(const QString& url,int contextSpec = FirstFreeContext,KviConsole* pConsole = 0);

	extern KVIRC_API void split(QString url, KviIrcUrlParts& parts);
	extern KVIRC_API void join(QString &url, KviServer* server);
	extern KVIRC_API void makeJoinCmd(const QStringList& chans, QString& szJoinCommand);
}

#endif // _KVI_IRCURL_H_
