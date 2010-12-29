//=============================================================================
//
//   File : KviAntiSpam.cpp
//   Creation date : Sun Apr 18 1999 05:36:55 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 1999-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#define _KVI_ANTISPAM_CPP_

#include "KviAntiSpam.h"
#include "KviOptions.h"

//[05:10:48] <Nataly19> :2http://www.extrababes.com5Go ahead punk, make my day !
//[05:11:04] <treeman>  :-ææ¸`ææ¸`ææ¸ -= for mp3's go to http://members.xoom.com/treemansmp3/start.htm 
//						=- `ææ¸`ææ¸`ææ- 
//[05:10:51] <Suzi48498>:P*** **** squirms like CRAZY when **** licks between her ...
//[05:12:11] <dea_>     :Hi my name is **** and I just posted pics of myself and 
//						other hot girls on my website at http://133.16.114.32/~guest/
//						tell me what you think!
//[15:18:37] <JuL|aN^bZ> Join #Teens4eva...Have Fun PPl!!!
//[15:19:37] <jkhnbn> Ciao, vai sul sito  www.sitromba.3000.it ci sono un casino di
//						immagini e video porno gratisssssssssssssssss!!!!!
//[15:19:46] <emjot``> zapraszam na kanal #poke super zabawa i opa za free !!!!!
//[15:23:20] <AlExXxX> IMPORTANTE: Se cerchi foto e video HARD andate quì:
//						http://216.221.175.58/cgi-bin/affiliate/ad.cgi?id=m4rines [automsg]
//[15:23:36] <Jane> hi..join #hard sex channel --Venom ScRipT³-- SToRM iN THe DaRK!!
//[15:23:47] <lovghgh> VISIT>> http://go.to/mp3andwarez/ << VISIT[15:23:47] <lovghgh> VISIT>> http://go.to/mp3andwarez/ << VISIT
//[15:23:56] <PIMPOLO> HO le prove certe che si puo' far soldi dovendo solo ricevere sms pubblicitari sul cellulare.
//						Mi sono arrivati sms pubblicitari!!! Ti interessa ? ISCRIVITI CLICCANDO DIRETTAMENTE SU QUESTO SITO:
//						http://www.smscash.it/index.php3?pagid=reg&p_form=P&p_pres=0&cpre=95620&p_desc=1 (Messaggio Automatico) IR©Simpson


// Yes...it is really annoying when I join a channel and
// i am flooded with all that messages.
// On large-newbie-high-traffic channels I get lagged
// with the Query windows popping up with all that stuff.
// What I can do ? Ignore queries ?
// mmmmh
//
// kvi_mayBeSpam : try to guess if text may be a spam message.
// Ideas:
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

// Just a minor change. This allows to easily add more "banned" words.
// Maybe there should be a config dialog for that ? Some people like to get
// porno pics, but don't like mp3's for example ... ;-)))
// -- Kristoff

// Ok...made this completely configurable....
// The default spam words get "installed" by the default script
// 09-01-2001: Pragma

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
		containing one or more "special words", and that user has no open query window,
		so the message is treated as spam and is redirected to the console window or is ignored completely
		(if you enable that option).[br]
		The spam words to check for, can be set by the means of [cmd]option[/cmd] stringlistSpamWords.[br]
		The anti-spam engine can be enabled by setting [cmd]option[/cmd] boolUseAntiSpamOnPrivmsg and
		[cmd]option[/cmd] boolUseAntiSpamOnNotice to 1.[br]
	@examples:
		[example]
			[comment]# This will cause "new-query private messages" containing[/comment]
			[comment]# words "http:","#" (channel name),"ftp:".... to be redirected to the console[/comment]
			[cmd]option[/cmd] stringlistSpamWords http:,#,ftp:,sex,free,chat,join
			[cmd]option[/cmd] boolUseAntiSpamOnPrivmsg 1
			[comment]# This will cause these messages to be completely ignored[/comment]
			[cmd]option[/cmd] boolSilentAntiSpam 1
		[/example]
*/

bool kvi_mayBeSpam(KviCString * msg,KviCString &spamWord)
{
	for(QStringList::Iterator it = KVI_OPTION_STRINGLIST(KviOption_stringlistSpamWords).begin();
			it != KVI_OPTION_STRINGLIST(KviOption_stringlistSpamWords).end() ; ++it)
	{
		// FIXME : This is SLOOOOOOOOW (QString -> ascii translation!!)

		const char * aux = (*it).toAscii();
		if(aux)
		{
			if(msg->findFirstIdx(aux,false) != -1)
			{
				spamWord = aux;
				return true;
			}
		}
	}
	return false;
}
