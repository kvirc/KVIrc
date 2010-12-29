//=============================================================================
//
//   File : KviCryptEngine.cpp
//   Creation date : Fri Nov 03 2000 02:34:43 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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


#include "KviCryptEngine.h"
#include "KviLocale.h"
#include "KviCString.h"

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	#include "KviMemory.h"
#endif

/*
	@doc: crypt_engines
	@type:
		generic
	@keyterms:
		cryptography, privacy, private key, text transformation
	@title:
		Cryptography and text transformation
	@short:
		Crypt/text transformation engines and how to use them
	@body:
		[big]Introduction[/big][br]
		Well, there is a concrete background noise about security around the net.
		And I've thought that adding a little cryptography support to KVIrc wasn't
		a bad idea. So I've first comed out with the "crypt engines", that
		allowed to have secure conversations on channels, queries and dcc chats;
		then found out that the realized structure was perfect to be "generalized"
		into "text transformation" support.[br]
		[big]The concept[/big][br]
		In few words, the "text transformation" engine is a layer between the user and the
		IRC connection. You type some text in the input line of a query window (for example),
		the engine transforms the text in some way and then sends it to the remote target.
		The trick works also in the reverse way: some data come from the remote target,
		the engine retransforms the text in some other way and displays it to the local user.[br]
		The "incoming" transformation is usually the inverse of the "outgoing" one, but it
		is not mandatory. It will become clear in few sentences that some engines will do no "incoming"
		transformation at all. The original use of the transformation engines was to crypt the
		outgoing data and to decrypt the incoming data; anyway, the engines can perform
		other funky tasks. One of them is remapping the local charset to a "standardized" one
		when sending text to a channel (or some other target) and doing the inverse map on
		the way back. A totally "fantastic" usage of this concept could be an "on-the-fly translator";
		it could translate for example Italian to English while sending to a channel
		and English to Italian on the way back...the implementation of a such engine is left
		to the reader as exercise :) Another (maybe less interesting) usage is to
		colorize the outgoing text, or transform it
		in a way that it is still readable but has a different look. This engine
		would not require a back transformation (so no "decrypt stage"). A "symmetric"
		idea could be an engine that strips the color codes from the incoming text: this
		engine would not require a "crypting" stage.[br]

		[big]The name of this stuf[/big][br]
		Initially all this stuff was named "cryptography support".
		Then the "cryptography" comed out to be not "enough" to describe
		the framework, so "text transformation" is a more generic term.
		Anyway, both terms were used in the documentation and the source.
		Just as example, the "text transformation engine" is called
		KviCryptEngine in the sources. So actually the terms "crypt"
		and "text transformations" refer to the "same thing".
		You will often find the term "encrypt" standing for "outgoing text
		transformation" and "decrypt" standing for "incoming text transformation".[br]

		[big]Yes, but why cryptography (on IRC) ?[/big][br]
		Because it MAY be useful. More than once people have asked me to add some
		crypting support to the dcc chats. Yes, I know that there are other
		"secure communication" tools, but actually I can't find one that is able to
		implement a "secure real time conversation". And what about a MULTIPLE real
		time secure conversation ? This can be done on an IRC channel now.[br]

		[big]The working things[/big][br]
		KVIrc can use a text transformation engine on irc channels, in the queries
		and the dcc chats. At the time I am writing, only the [module:rijndael]Rijndael[/module] crypt engine
		is available: this is a private key encryption algorithm that assures
		a "pretty good" security level. More engines will be surely available
		at the time of the 3.0.0 release of KVIrc. The engines
		can be activated by the dedicated dialog that can be accessed from the "button bar"
		of the window. Once an engine has been enabled all the text that you type
		in the input line (that is not a command obviously) is encrypted
		and sent to the remote endpoint. If you want to sent a non crypted message
		while an engine is working you can use the CTRL+P escape: by placing
		that character as the FIRST CHARACTER of the line you will avoid crypting.
		Every engine has different capabilities: some can both encrypt
		and decrypt, other perform only half of the operations. Some engines
		need a key (the crypt engines obviously), or two keys (you can specify
		one for the outgoing data and one for the incoming). You can specify
		all these options in the crypt/text transformation dialog.[br]
		Obviously (with the current implementations) all the conversation endpoints
		must agree on the engine (or better algorithm) used and on the key(s).
		The key is user specified, so you have to find a secure way to negotiate
		it with your communication engpoints. If you can meet the persons in the "real life",
		this is the best way to exchange the keys, otherwise you can use mail & pgp.
		Yes, this is a "miss" of the crypt protocol: it is missing a public key handshake.[br]

		[big]The first test[/big][br]
		A cool way to test a crypting engine is to use a "self query": connect to
		any irc server, and execute [cmd]query[/cmd] <yournickname>; a query window
		with you both as source and target will popup; activate a crypt engine
		and enable both encryption and decryption; specify the same key for
		bot directions and then type some text in the input line: you will see
		the message twice: one is "your local text" and the other is the server routed one.
		Then you can try to activate encryption only and leaving decryption disabled:
		you will see how the text would appear to a possible "man in the middle".
		You can also try to use different keys for encrypting and decrypting,
		and play with the CTRL+P escape.[br]

		[big]The protocol[/big][br]
		Well, there is no protocol actually, only the existing implementations, that
		can be accessed by anyone that want to reproduce them. There are only some
		points relating to the crypting engines that need to be cleared:[br]
		The crypted text must be suitable to be sent thru an IRC connection;
		this means that some characters can not appear in the crypted text (e.g. CR,LF,NULL....).
		KVIrc solves it in a simple way: the crypted binary data is encoded,
		either as hexadecimal numeric string or in base64.[br]
		An escape character has been defined to identify messages that are "crypted" from the
		ones that are not: this character has ascii code 30 (decimal).[br]
		The encoding is used in private messages only and has the following format:[br]
		PRIVMSG <target> :<escape_char_ascii_30><encrypted message>[br]
		ASCII 30 does not correspond to any widely used escape sequence and allows
		mixing encrypted and plain text messages in a conversation...Well, this is not
		so pretty but you can exchange crypted messages with one or two friends while
		talking on a normal IRC channel: nobody else than your friends will be able
		to understand the message; others will see senseless sequences of characters.
		You will be still able to read the unencrypted messages of the other people
		on the channel.[br] The escape character is not needed if the engine
		performs non-crypting tasks: a charset mapper will produce text that
		is meant to be readed by anyone on the channel, a text colorizer will
		act in a similar way too. So the escape character is used for the "crypting"
		engines only.

		[big]An idea for the future implementations[/big][br]
		A "public key" handshake protocol could be implemented.
*/


	KviCryptEngine::KviCryptEngine()
	{
#ifdef COMPILE_CRYPT_SUPPORT
		setLastError(__tr2qs("Invalid crypt engine"));
		m_iMaxEncryptLen = -1; // unlimited
		m_deallocFunc = 0;
#endif //COMPILE_CRYPT_SUPPORT
	}

	KviCryptEngine::~KviCryptEngine()
	{
	}

#ifdef COMPILE_CRYPT_SUPPORT

	bool KviCryptEngine::init(const char *, int, const char *, int)
	{
		return false;
	}

	KviCryptEngine::EncryptResult KviCryptEngine::encrypt(const char *, KviCString &)
	{
//		qDebug("Pure virtual KviCryptEngine::encrypt() called");
		return EncryptError;
	}

	KviCryptEngine::DecryptResult KviCryptEngine::decrypt(const char *, KviCString &)
	{
//		qDebug("Pure virtual KviCryptEngine::decrypt() called");
		return DecryptError;
	}

#endif //COMPILE_CRYPT_SUPPORT
