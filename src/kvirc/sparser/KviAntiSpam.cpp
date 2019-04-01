//=============================================================================
//
//   File : KviAntiSpam.cpp
//   Creation date : Sun Apr 18 1999 05:36:55 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 1999-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#define _KVI_ANTISPAM_CPP_

#include "KviAntiSpam.h"
#include "KviCString.h"
#include "KviOptions.h"

// - A spam message is generally a single PRIVMSG <mynick> :<text>
//		so this function should be (and is) called when
//		a PRIVMSG is received from a person that has no QUERY
//		window open yet.
// - 95% of spam messages contain an URL inside (http,ftp,www) or a channel name.
// - The other 5% contain words like auto-msg, msg me or query <nickname>
// - There are some really common words:
//   free, mp3, sex, teen, porn, pics, girls, babe, pass, user..., hard, join
//
// In this way we can get rid of a good 70% of spam msgs.

/*
	@doc: antispam
	@title:
		Anti-spam protection
	@keyterms:
		spam
	@body:
		KVIrc contains a small spam detector, and can help you in avoiding that annoying flood
		of queries popping up when you join large channels full of lamers.[br]
		The spam detector works in a rather simple way: when someone sends you a private message
		containing one or more [i]special words[/i], and that user has no open query window,
		so the message is treated as spam and is redirected to the console window or is ignored completely
		(if you enable that option).[br]
		The spam words to check for, can be set by the means of [cmd]option[/cmd] stringlistSpamWords.[br]
		The anti-spam engine can be enabled by setting [cmd]option[/cmd] boolUseAntiSpamOnPrivmsg and
		[cmd]option[/cmd] boolUseAntiSpamOnNotice to 1.[br]
	@examples:
		[example]
			[comment]# This will cause new-query private messages containing[/comment]
			[comment]# words http:, #(channel name), or ftp: to be redirected to the console[/comment]
			[cmd]option[/cmd] stringlistSpamWords http:, #, ftp:, sex, free, chat, join
			[cmd]option[/cmd] boolUseAntiSpamOnPrivmsg 1
			[comment]# This will cause these messages to be completely ignored[/comment]
			[cmd]option[/cmd] boolSilentAntiSpam 1
		[/example]
*/

bool kvi_mayBeSpam(KviCString msg, KviCString & spamWord)
{
	for(auto & it : KVI_OPTION_STRINGLIST(KviOption_stringlistSpamWords))
	{
		// FIXME : This is SLOOOOOOOOW (QString -> ascii translation!!)
		QByteArray szLatin1 = it.toLatin1();
		const char * aux = szLatin1.data();
		if(*aux)
		{
			if(msg.findFirstIdx(aux, false) != -1)
			{
				spamWord = aux;
				return true;
			}
		}
	}
	return false;
}
