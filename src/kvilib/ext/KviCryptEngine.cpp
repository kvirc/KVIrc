//=============================================================================
//
//   File : KviCryptEngine.cpp
//   Creation date : Fri Nov 03 2000 02:34:43 CEST by Szymon Stefanek
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

#include "KviCryptEngine.h"
#include "KviCString.h"
#include "KviLocale.h"

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
#include "KviMemory.h"
#endif

/*
	@doc: crypt_engines
	@type:
		generic
	@keyterms:
		cryptography, encryption, privacy, private key, text transformation
	@title:
		Cryptography and text transformation
	@short:
		Crypt/text transformation engines and how to use them
	@body:
		[big]Introduction[/big]
		Well, there is a concrete background noise about security around the net.
		And I've thought that adding a little cryptography support to KVIrc wasn't
		a bad idea. So I first came up with the [i]crypt engines[/i], that
		allowed secure conversations in channels, queries and DCC chats;
		then found out that the realized structure was perfect to be [i]generalized[/i]
		into [i]text transformation[/i] support.
		[big]The concept[/big]
		In few words, the [i]text transformation[/i] engine is a layer between the user and the
		IRC connection. You type some text in the input line of a query window (for example),
		the engine transforms the text in some way and then sends it to the remote target.
		The trick works also in the reverse way: some data comes from the remote target,
		the engine re-transforms the text in some other way and displays it to the local user.[br][br]
		The [i]incoming[/i] transformation is usually the inverse of the [i]outgoing[/i] one, but it
		is not mandatory. It will become clear in few sentences that some engines will do no [i]incoming[/i]
		transformation at all. The original use of the transformation engines were to encrypt the
		outgoing data and to decrypt the incoming data; anyway, the engines can perform
		other funky tasks. One of them is remapping the local charset to a standardized one
		when sending text to a channel (or some other target) and doing the inverse map on
		the way back. A totally [i]fantastic[/i] usage of this concept could be an [i]on-the-fly translator[/i];
		it could translate for example Italian to English while sending to a channel
		and English to Italian on the way back... the implementation of a such engine is left
		to the reader as exercise :) Another (maybe less interesting) usage is to
		colorize the outgoing text, or transform it
		in a way that it is still readable but has a different look. This engine
		would not require a back transformation (so no [i]decrypt stage[/i]). A [i]symmetric[/i]
		idea could be an engine that strips the color codes from the incoming text: this
		engine would not require an encrypting stage.
		[big]The name of this stuff[/big]
		Initially all this was named [i]cryptography support[/i].
		Then [i]cryptography[/i] was no longer enough to describe
		the framework, so [i]text transformation[/i] is a more generic term.
		Anyway, both terms were used in the documentation and the source.
		Just as example, the [i]text transformation engine[/i] is called
		KviCryptEngine in the sources. So actually the terms [i]crypt[/i]
		and text transformation refer to the same thing.
		You will often find the term [i]encrypt[/i] standing for [i]outgoing text
		transformation[/i] and [i]decrypt[/i] standing for [i]incoming text transformation[/i].
		[big]Yes, but why cryptography (on IRC)?[/big]
		Because it MAY be useful. More than once people have asked me to add some
		encryption support to the DCC chats. Yes, I know that there are other
		[i]secure communication[/i] tools, but actually I can't find one that is able to
		implement a [i]secure real time conversation[/i]. And what about a MULTIPLE real
		time secure conversation? This can be done on an IRC channel now.
		[big]The working things[/big]
		KVIrc can use a text transformation engine on IRC channels, in queries
		and in DCC chats. At the time I am writing, only the [module:rijndael]Rijndael[/module] crypt engine
		is available: this is a private key encryption algorithm that assures
		a [i]pretty good[/i] security level. More engines will be surely available
		at the time of the 3.0.0 release of KVIrc. The engines
		can be activated by the dedicated dialog that can be accessed from the [i]button bar[/i]
		of the window. Once an engine has been enabled, all the text that you type
		in the input line (that is not a command obviously) is encrypted
		and sent to the remote endpoint. If you want to send a non-encrypted message
		while an engine is working, you can use the Ctrl+P escape: by placing
		that character as the FIRST CHARACTER of the line you will avoid encrypting.
		Every engine has different capabilities: some can both encrypt
		and decrypt, others perform only half of the operations. Some engines
		need a key (the crypt engines obviously), or two keys (you can specify
		one for the outgoing data and one for the incoming). You can specify
		all these options in the crypt/text transformation dialog.[br][br]
		Obviously (with the current implementations) all the conversation endpoints
		must agree on the engine (or better algorithm) used and on the key(s).
		The key is user specified, so you have to find a secure way to negotiate
		it with your communication endpoints. If you can meet these people in real life,
		this is the best way to exchange the keys, otherwise you can use mail & PGP.
		Yes, this is a shortcoming of the crypt protocol: it is missing a public key handshake.
		[big]The first test[/big]
		A cool way to test an encryption engine is to use a [i]self query[/i]: connect to
		any IRC server, and execute [cmd]query[/cmd] <yournickname>; a query window
		with you both as source and target will popup; activate a crypt engine
		and enable both encryption and decryption; specify the same key for
		both directions and then type some text in the input line: you will see
		the message twice: one is [i]your local text[/i] and the other is the server routed one.
		Then you can try to activate encryption only and leaving decryption disabled:
		you will see how the text would appear to a possible [i]man in the middle[/i].
		You can also try to use different keys for encrypting and decrypting,
		and play with the Ctrl+P escape.
		[big]The protocol[/big]
		Well, there is no protocol actually, only the existing implementations, that
		can be accessed by anyone that wants to reproduce them. There are only some
		points relating to the encryption engines that need to be cleared:[br][br]
		The encrypted text must be suitable to be sent through an IRC connection;
		this means that some characters can not appear in the encrypted text (e.g. CR, LF, NULL).
		KVIrc solves it in a simple way: the encrypted binary data is encoded,
		either as a hexadecimal numeric string or in base64.[br][br]
		An escape character has been defined to identify messages that are [i]encrypted[/i] from the
		ones that are not: this character has ASCII code 30 (decimal).[br]
		The encoding is used in private messages only and has the following format:[br][br]
		PRIVMSG <target> :<escape_char_ascii_30><encrypted message>[br][br]
		ASCII 30 does not correspond to any widely used escape sequence and allows
		mixing encrypted and plain text messages in a conversation, well, this is not
		so pretty but you can exchange encrypted messages with one or two friends while
		talking on a normal IRC channel, nobody else other than your friends will be able
		to understand the message, others will see senseless sequences of characters.
		You will be still able to read the unencrypted messages of the other people
		on the channel.[br][br]
		The escape character is not needed if the engine performs non-encrypting
		tasks: a charset mapper will produce text that is meant to be read by
		anyone on the channel, a text colorizer will act in a similar way too.
		So the escape character is used for the encryption engines only.
		[big]An idea for future implementations[/big]
		A [i]public key[/i] handshake protocol could be implemented.
*/

KviCryptEngine::KviCryptEngine()
{
#ifdef COMPILE_CRYPT_SUPPORT
	setLastError(__tr2qs("Invalid encryption engine"));
	m_iMaxEncryptLen = -1; // unlimited
	m_deallocFunc = nullptr;
#endif //COMPILE_CRYPT_SUPPORT
}

KviCryptEngine::~KviCryptEngine()
    = default;

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
