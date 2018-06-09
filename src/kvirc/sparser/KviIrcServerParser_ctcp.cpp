//=============================================================================
//
//   File : KviIrcServerParser_ctcp.cpp
//   Creation date : Thu Aug 16 2000 13:34:42 by Szymon Stefanek
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

// FIXME: #warning "CTCP BEEP == WAKEUP == AWAKE"
// FIXME: #warning "CTCP AVATARREQ or QUERYAVATAR"

#include "KviControlCodes.h"
#include "KviRuntimeInfo.h"
#include "KviApplication.h"
#include "KviIrcServerParser.h"
#include "KviWindow.h"
#include "kvi_out.h"
#include "KviLocale.h"
#include "KviIrcSocket.h"
#include "KviChannelWindow.h"
#include "kvi_defaults.h"
#include "KviQueryWindow.h"
#include "KviIrcUserDataBase.h"
#include "KviIconManager.h"
#include "KviModuleManager.h"
#include "KviSharedFilesManager.h"
#include "KviTimeUtils.h"
#include "KviFileUtils.h"
#include "KviCtcpPageDialog.h"
#include "KviUserAction.h"
#include "KviOptions.h"
#include "KviIrcConnection.h"
#include "KviIrcConnectionUserInfo.h"
#include "KviIrcConnectionAntiCtcpFloodData.h"
#include "KviLagMeter.h"
#include "KviKvsEventTriggers.h"
#include "KviKvsScript.h"
#include "kvi_sourcesdate.h"
#include "KviRegisteredUserDataBase.h"
#include "KviBuildInfo.h"
#include "KviIrcConnectionServerInfo.h"
#include "KviIrcMessage.h"

#ifdef COMPILE_CRYPT_SUPPORT
#include "KviCryptEngine.h"
#include "KviCryptController.h"
#endif //COMPILE_CRYPT_SUPPORT

#include <cstdlib>

#include <QDateTime>
#include <QLocale>

extern KVIRC_API KviSharedFilesManager * g_pSharedFilesManager;
extern KVIRC_API KviCtcpPageDialog * g_pCtcpPageDialog;

/*
	@doc: ctcp_handling
	@title:
		KVIrc and CTCP
	@short:
		For developers: Client-To-Client Protocol handling in KVIrc
	@body:
		[big]Introduction[/big]
		Personally, I think that the CTCP specification is to
		be symbolically printed & burned. It is really too complex
		(you can go mad with the quoting specifications)
		and NO IRC CLIENT supports it completely.
		Here is my personal point of view on the CTCP protocol.
		[big]What is CTCP?[/big]
		CTCP stands for Client-to-Client Protocol. It is designed
		for exchanging almost arbitrary data between IRC clients;
		the data is embedded into text messages of the underlying
		IRC protocol.
		[big]Basic concepts[/big]
		A CTCP message is sent as the <text> part of the PRIVMSG and
		NOTICE IRC commands.[br]
		To differentiate the CTCP message from a normal IRC message
		text we use a delimiter character (ASCII char 1); we will
		use the symbol <0x01> for this delimiter.
		You may receive a CTCP message from server in one of the
		following two ways:[br]
		[b]:<source_mask> PRIVMSG <target> :<0x01><ctcp message><0x01>[/b][br]
		[b]:<source_mask> NOTICE <target>:<0x01><ctcp message><0x01>[/b][br]
		The PRIVMSG is used for CTCP REQUESTS, the NOTICE for CTCP REPLIES.
		The NOTICE form should never generate an automatic reply.[br]
		The two delimiters were used to begin and terminate the
		CTCP message; The original protocol allowed more than one CTCP
		message inside a single IRC message. [b]Nobody sends more than
		one message at once, no client can recognize it (since it
		complicates the message parsing), it could be even dangerous (see below)[/b].
		It makes no real sense unless we wanted to use the CTCP protocol to embed escape sequences
		into IRC messages, which is not the case.[br]
		Furthermore, sending more CTCP messages in a single IRC message could
		be easily used to flood a client. Assuming 450 characters available for the IRC message
		text part, you could include 50 CTCP messages containing "<0x01>VERSION<0x01>".[br]
		Since the VERSION replies are usually long (there can be 3 or 4 replies per IRC message),
		a client that has no CTCP flood protection (or has it disabled) will surely
		be disconnected while sending the replies, after only
		receiving a single IRC message (no flood for the sender).
		From my personal point of view, only [b]one CTCP message per IRC message[/b]
		should be allowed and theoretically the trailing <0x01> delimiter can be optional.
		[big]How to extract the CTCP message[/big]
		The IRC messages do not allow the following characters to be sent:[br]
		<NUL> (ASCII character 0), <CR> (Carriage return), <LF> (Line feed).[br]
		So finally we have four characters that [b]cannot appear literally into a
		CTCP message[/b]: <NUL>,<CR>,<LF>,<0x01>.[br]
		To extract a <ctcp_message> from an IRC PRIVMSG or NOTICE command you
		have to perform the following actions:[br]
		Find the <trailing> part of the IRC message (the one just after the ':'
		delimiter, or the last message token).[br]
		Check if the first character of the <trailing> is a <0x01>, if it is
		we have a <ctcp_message> beginning just after <0x01>.
		The trailing (optional) <0x01> can be removed in this phase
		or later, assuming that it is not a valid char in the <ctcp message>.[br]
		In this document I will assume that you have stripped the trailing <0x01>
		and thus from now on we will deal only with the <ctcp message> part.
		[big]Parsing a CTCP message: The quoting dilemma[/big]
		Since there are characters that cannot appear in a <ctcp message>,
		theoretically we should have to use a quoting mechanism.
		Well, in fact, no actual CTCP message uses the quoting: there
		is no need to include a <NUL>, a <CR> or <LF> inside the actually
		defined messages (The only one could be CTCP SED, but I have never
		seen it in action... is there any client that implements it?).
		We could also leave the [i]quoting[/i] to the [i]single message type semantic[/i]:
		a message that needs to include [i]any character[/i] could have its own
		encoding method (Base64 for example). With the "one CTCP per IRC message"
		convention we could even allow <0x01> inside messages. Only the leading
		(and eventually trailing) <0x01> would be the delimiter, the other ones
		would be valid characters. Finally, is there any CTCP type that needs
		<0x01> inside a message? <0x01> is not printable (as well as <CR>,<LF> and <NUL>),
		so only encoded messages (and again we can stick to the single message semantic)
		messages or the ones including special parameters. Some machines might
		allow <0x01> in filenames... well, a file with <0x01> in its name has something
		broken inside, or the creator is a sort of [i]hacker[/i] (so he also
		knows how to rename a file...) :).[br]
		Anyway, let's be pedantic, and define this quoting method.
		Let's use the most intuitive method, adopted all around the world:[br]
		The backslash character ('\') as escape.[br]
		An escape sequence is formed by the backslash character and a number
		of following ASCII characters. We define the following two types of escape sequences:[br]
		[b]'\XXX'[/b] (where XXX is an [b]octal number[/b] formed by three digits)
		that indicates the ASCII character with code that corresponds to the number.[br]
		[b]'\C'[/b] (where C is a [b]CTCP valid ASCII non digit character[/b]) that corresponds
		literally to the character C discarding any other semantic that might be associated
		with it (This will become clear later).
		I've chosen the octal representation just to follow a bit the old specification:
		the authors seemed to like it. This point could be discussed in
		some mailing list or sth. The '\C' sequence is useful to include the backslash
		character (escape sequence '\\').
		[big]Let's mess a little more[/big]
		A CTCP message is made of [b]space separated parameters[/b].[br]
		The natural way of separating parameters is to use the space character.
		We define a [i]token[/i] as a sequence of valid CTCP characters not including literal space.
		A <ctcp parameter> is usually a token, but not always;
		filenames can contain spaces inside names (and it happens very often!).
		So one of the parameters of CTCP DCC is not a space separated token.
		How do we handle it? Again a standard is missing. Some clients simply change
		the filename placing underscores instead of spaces, this is a reasonable solution if used with care.
		Other clients attempt to [i]isolate[/i] the filename token by surrounding it with some kind
		of quotes, usually the [b]"[/b] or [b]'[/b] characters. This is also a good solution.
		Another one that naturally comes into my mind is to use the previously defined
		quoting to define a [i]non-breaking space[/i] character, because a space after a backslash
		could lose its original semantic. Better yet, use the backslash followed by
		the octal representation of the space character ('\040').
		Anyway, to maintain compatibility with other popular IRC clients (such as mIRC),
		let's include the [b]"[/b] quotes in our standard: literal (unescaped) [b]"[/b] quotes
		define a single token string. To include a literal [b]"[/b] character, escape it.
		Additionally, the last parameter of a <ctcp message> may be made of multiple tokens.
		[big]A CTCP parameter extracting example[/big]
		A trivial example of a C [i]CTCP parameter extracting routine[/i] follows.[br]
		An IRC message is made of up to 510 usable characters.
		When a CTCP is sent there is a PRIVMSG or NOTICE token that uses at least 6 characters,
		at least two spaces and a target token (that can not be empty, so it is at least one character)
		and finally one <0x01> escape character. This gives 500 characters as maximum size
		for a complete <ctcp message> and thus for a <ctcp token>.
		In fact, the <ctcp message> is always smaller than 500 characters; there are usually two
		<0x01> chars, there is a message source part at the beginning of the IRC message
		that is 10-15 characters long, and there is a [b]:[/b] character before the trailing parameter.
		Anyway, to really be on the [i]safe side[/i], we use a 512 character buffer for each
		<ctcp token>. Finally, I'll assume that you have already ensured that
		the <ctcp message> that we are extracting from is shorter than 511 characters in all,
		and have provided a buffer big enough to avoid this code segfaulting.
		I'm assuming that msg_ptr points somewhere in the <ctcp message> and is null-terminated.[br]
		(There are C++ style comments, you might want to remove them)
		[example]
		const char * decode_escape(const char * msg_ptr,char * buffer)
		{
			// This one decodes an escape sequence
			// and returns the pointer "just after it"
			// and should be called when *msg_ptr points
			// just after a backslash
			char c;
			if((*msg_ptr >= '0') && (*msg_ptr < '8'))
			{
				// a digit follows the backslash
				c = *msg_ptr - '0';
				msg_ptr++;
				if(*msg_ptr >= '0') && (*msg_ptr < '8'))
				{
					c = ((c << 3) + (*msg_ptr - '0'));
					msg_ptr++;
					if(*msg_ptr >= '0') && (*msg_ptr < '8'))
					{
						c = ((c << 3) + (*msg_ptr - '0'));
						msg_ptr++;
					} // else broken message, but let's be flexible
				} // else it is broken, but let's be flexible
				// append the character and return
				*buffer = c;
				return msg_ptr;
			} else {
				// simple escape: just append the following
				// character (thus discarding its semantic)
				*buffer = *msg_ptr;
				return ++msg_ptr;
			}
		}

		const char * extract_ctcp_parameter(const char * msg_ptr,char * buffer,int spaceBreaks)
		{
			// this one extracts the "next" ctcp parameter in msg_ptr
			// it skips the leading and trailing spaces.
			// spaceBreaks should be set to 0 if (and only if) the
			// extracted parameter is the last in the CTCP message.
			int inString = 0;
			while(*msg_ptr == ' ')msg_ptr++;
			while(*msg_ptr)
			{
				switch(*msg_ptr)
				{
					case '\\':
						// backslash : escape sequence
						msg_ptr++;
						if(*msg_ptr)msg_ptr = decode_escape(msg_ptr,buffer);
						else return msg_ptr; // senseless backslash
					break;
					case ' ':
						// space : separate tokens?
						if(inString || (!spaceBreaks))*buffer++ = *msg_ptr++;
						else {
							// not in string and space breaks: end of token
							// skip trailing white space (this could be avoided)
							// and return
							while(*msg_ptr == ' ')msg_ptr++;
							return msg_ptr;
						}
					break;
					case '"':
						// a string begin or end
						inString = !inString;
						msg_ptr++;
					break;
					default:
						// any other char
						*buffer++ = *msg_ptr++;
					break;
				}
			}
			return msg_ptr;
		}
		[/example]
		[big]CTCP parameter semantics[/big]
		The first <ctcp parameter> of a <ctcp message> is the <ctcp tag>: it defines
		the semantic of the rest of the message.[br]
		Although it is a convention to specify the <ctcp tag> as uppercase letters,
		and the original specification says that the whole <ctcp message> is
		case sensitive, I'd prefer to follow the IRC message semantic (just to
		have less "special cases") and treat the whole message as [b]case insensitive[/b].[br]
		The remaining tokens depend on the <ctcp tag>. A description of known <ctcp tags>
		and thus <ctcp messages> follows.
		[big]PING[/big]
		[b]Syntax: <0x01>PING <data><0x01>[/b][br]
		The PING request is used to check the round trip time from one client to another.
		The receiving client should reply with exactly the same message but sent
		through a NOTICE instead of a PRIVMSG. The <data> usually contains an unsigned
		integer but not necessarily; it is not even mandatory for <data> to be a single token.
		The receiver should ignore the semantic of <data>.[br]
		The reply is intended to be processed by IRC clients.
		[big]VERSION[/big]
		[b]Syntax: <0x01>VERSION<0x01>[/b][br]
		The VERSION request asks for information about another user's IRC client program.
		The reply should be sent through a NOTICE with the following syntax:[br]
		<0x01>VERSION <client_version_data><0x01>[br]
		The preferred form for <client_version_data> is
		[i]<client_name>:<client_version>:<client_enviroinement>[/i], but historically
		clients (and users) send a generic reply describing the client name, version
		and eventually the used script name. This CTCP reply is intended to be human
		readable, so any form is accepted.
		[big]USERINFO[/big]
		[b]Syntax: <0x01>USERINFO<0x01>[/b][br]
		The USERINFO request asks for information about another user.
		The reply should be sent through a NOTICE with the following syntax:[br]
		<0x01>USERINFO <user_info_data><0x01>[br]
		The <user_info_data> should be a human readable [i]user defined[/i] string;
		[big]CLIENTINFO[/big]
		[b]Syntax: <0x01>CLIENTINFO<0x01>[/b][br]
		The CLIENTINFO request asks for information about another user's IRC client program.
		While VERSION requests the client program name and version, CLIENTINFO requests
		information about CTCP capabilities.[br]
		The reply should be sent through a NOTICE with the following syntax:[br]
		<0x01>CLIENTINFO <client_info_data><0x01>[br]
		The <client_info_data> should contain a list of supported CTCP request tags.
		The CLIENTINFO reply is intended to be human readable.
		[big]FINGER[/big]
		[b]Syntax: <0x01>FINGER<0x01>[/b][br]
		The FINGER request asks for information about another IRC user.
		The reply should be sent through a NOTICE with the following syntax:[br]
		<0x01>FINGER <user_info_data><0x01>[br]
		The <user_info_data> should be a human readable string containing
		the system username and possibly the system idle time;
		[big]SOURCE[/big]
		[b]Syntax: <0x01>SOURCE<0x01>[/b][br]
		The SOURCE request asks for the client homepage or ftp site information.
		The reply should be sent through a NOTICE with the following syntax:[br]
		<0x01>VERSION <homepage_url_data><0x01>[br]
		This CTCP reply is intended to be human readable, so any form is accepted.
		[big]TIME[/big]
		[b]Syntax: <0x01>TIME<0x01>[/b][br]
		The TIME request asks for the user local time.
		The reply should be sent through a NOTICE with the following syntax:[br]
		<0x01>TIME <time and date string><0x01>[br]
		This CTCP reply is intended to be human readable, so any form is accepted.
		[big]ACTION[/big]
		[b]Syntax: <0x01>ACTION<0x01>[/b][br]
		The ACTION tag is used to describe an action.[br]
		It should be sent through a NOTICE message and never generate a reply.
		[big]AVATAR (equivalent to ICON or FACE)[/big]
		[b]Syntax: <0x01>AVATAR<0x01>[/b][br]
		The AVATAR tag is used to query a user's avatar.
		[big]MULTIMEDIA (equivalent to MM or SOUND)[/big]
		[b]Syntax: <0x01>MULTIMEDIA <filename><0x01>[/b][br]
		The MULTIMEDIA tag is used to play a multimedia file on the receiver's side.[br]
		The receiving client should locate the file associated to <filename>,
		and play it. If the file can not be located
		by the receiving client, and the MULTIMEDIA tag was sent through a PRIVMSG format CTCP,
		the receiving client CAN request a [doc:dcc_connection]DCC GET[/doc] <filename> from the source user.
		If the MULTIMEDIA tag was sent through a NOTICE message, the receiving client
		should not generate any reply: the message should be notified to the receiving
		client's user and then be discarded. The <filename> should never contain a leading
		path. If any part of the <filename> appears to be a path component, it should be discarded.
		The client may decide to drop the entire message too. Older clients (including
		older releases of KVIrc) used to request the missing filenames by a particular
		non-standard private message syntax. This convention should be dropped.
		[big]DCC[/big]
		[b]Syntax: <0x01>DCC <type> <type dependent parameters><0x01>[/b][br]
		The DCC tag is used to initiate a Direct Client Connection.
		The known DCC types are:[br]
		[pre]
			CHAT
			SEND
			SSEND
			TSEND
			GET
			TGET
			ACCEPT
			RESUME
		[/pre]
*/

void KviIrcServerParser::encodeCtcpParameter(const char * param, KviCString & buffer, bool bSpaceBreaks)
{
	//
	// This one encodes a single ctcp parameter with the simplest
	// subset of rules and places it in the supplied buffer
	//
	if(!(*param))
	{
		// empty parameter: the only reason we REALLY need the double quotes
		if(bSpaceBreaks)
			buffer.append("\"\"");
		return;
	}

	const char * begin = param;

	while(*param)
	{
		switch(*param)
		{
			case ' ':
				if(bSpaceBreaks)
				{
					if(param != begin)
						buffer.append(begin, param - begin);
					buffer.append("\\040");
					param++;
					begin = param;
				}
				else
				{
					// space is non breaking (last parameter)
					param++;
				}
				break;
			case '\r':
				if(param != begin)
					buffer.append(begin, param - begin);
				buffer.append("\\015");
				param++;
				begin = param;
				break;
			case '\n':
				if(param != begin)
					buffer.append(begin, param - begin);
				buffer.append("\\012");
				param++;
				begin = param;
				break;
			case '"':
				if(param != begin)
					buffer.append(begin, param - begin);
				buffer.append("\\042");
				param++;
				begin = param;
				break;
			case '\\':
				if(param != begin)
					buffer.append(begin, param - begin);
				buffer.append("\\143");
				param++;
				begin = param;
				break;
			case 0x01:
				if(param != begin)
					buffer.append(begin, param - begin);
				buffer.append("\\001");
				param++;
				begin = param;
				break;
			default:
				param++;
				break;
		}
	}

	if(param != begin)
		buffer.append(begin, param - begin);
}

void KviIrcServerParser::encodeCtcpParameter(const char * parametr, QString & resultBuffer, bool bSpaceBreaks)
{
	// This one encodes a single ctcp parameter with the simplest
	// subset of rules and places it in the supplied buffer
	QByteArray buffer;
	const char * param = parametr;
	if(!param)
	{
		if(bSpaceBreaks)
			buffer.append("\"\"");
		return;
	}
	if(!(*param))
	{
		// empty parameter: the only reason we REALLY need the double quotes
		if(bSpaceBreaks)
			buffer.append("\"\"");
		return;
	}

	while(*param)
	{
		switch(*param)
		{
			case ' ':
				if(bSpaceBreaks)
				{
					buffer.append("\\040");
					param++;
				}
				else
				{
					buffer += *param;
					param++;
				}
				break;
			case '\r':
				buffer.append("\\015");
				param++;
				break;
			case '\n':
				buffer.append("\\012");
				param++;
				break;
			case '"':
				buffer.append("\\042");
				param++;
				break;
			case '\\':
				buffer.append("\\143");
				param++;
				break;
			case 0x01:
				buffer.append("\\001");
				param++;
				break;
			default:
				buffer += *param;
				param++;
				break;
		}
	}
	resultBuffer = buffer;
}

const char * KviIrcServerParser::decodeCtcpEscape(const char * msg_ptr, KviCString & buffer)
{
	// This one decodes an octal sequence
	// and returns the pointer "just after it".
	// It should be called when *msg_ptr points
	// just after a backslash.
	// The decoded escape is appended to the buffer
	//
	// We're also assuming that *msg_ptr is not null here
	if((*msg_ptr >= '0') && (*msg_ptr < '8'))
	{
		// a digit follows the backslash */
		char c = *msg_ptr - '0';
		msg_ptr++;
		if((*msg_ptr >= '0') && (*msg_ptr < '8'))
		{
			c = ((c << 3) + (*msg_ptr - '0'));
			msg_ptr++;
			if((*msg_ptr >= '0') && (*msg_ptr < '8'))
			{
				c = ((c << 3) + (*msg_ptr - '0'));
				msg_ptr++;
			} // else broken message, but let's be flexible
		}         // else it is broken, but let's be flexible
		buffer.append(c);
		return msg_ptr;
	}

	if(*msg_ptr == 'r')
	{
		buffer.append('\r');
		return ++msg_ptr;
	}

	if(*msg_ptr == 'n')
	{
		buffer.append('\n');
		return ++msg_ptr;
	}

	// null escape: just append the following
	// character (thus discarding its semantics)

	buffer.append(msg_ptr);
	return ++msg_ptr;
}

const char * KviIrcServerParser::decodeCtcpEscape(const char * msg_ptr, QByteArray & buffer)
{
	// This one decodes an octal sequence
	// and returns the pointer "just after it".
	// It should be called when *msg_ptr points
	// just after a backslash.
	// The decoded escape is appended to the buffer
	//
	// We're also assuming that *msg_ptr is not null here
	if((*msg_ptr >= '0') && (*msg_ptr < '8'))
	{
		// a digit follows the backslash */
		char c = *msg_ptr - '0';
		msg_ptr++;
		if((*msg_ptr >= '0') && (*msg_ptr < '8'))
		{
			c = ((c << 3) + (*msg_ptr - '0'));
			msg_ptr++;
			if((*msg_ptr >= '0') && (*msg_ptr < '8'))
			{
				c = ((c << 3) + (*msg_ptr - '0'));
				msg_ptr++;
			} // else broken message, but let's be flexible
		}     // else it is broken, but let's be flexible
		buffer += (c ? c : ' ');
		return msg_ptr;
	}

	if(*msg_ptr == 'r')
	{
		buffer += '\r';
		return ++msg_ptr;
	}
	if(*msg_ptr == 'n')
	{
		buffer += '\n';
		return ++msg_ptr;
	}

	// null escape: just append the following
	// character (thus discarding its semantics)

	buffer += *msg_ptr;
	return ++msg_ptr;
}

const char * KviIrcServerParser::extractCtcpParameter(const char * msg_ptr, KviCString & buffer, bool bSpaceBreaks, bool bSafeOnly)
{
	// This one extracts the "next" ctcp parameter in msg_ptr
	// and puts it in the supplied buffer.
	// It is assumed that the leading and trailing CTCP
	// tags have been already removed.
	// Skips the leading and trailing spaces.
	// bSpaceBreaks should be set to false if (and only if) the
	// extracted parameter is the last in a positional parameter
	// based CTCP message.

	if(!msg_ptr)
		return nullptr;
	while(*msg_ptr == ' ')
		msg_ptr++; // skip leading spaces

	int bInString = 0;
	if(*msg_ptr == '"')
	{
		// a quoted parameter
		bInString = 1;
		msg_ptr++;
	}

	const char * begin = msg_ptr;

	while(*msg_ptr)
	{
		switch(*msg_ptr)
		{
			case '\\':
				// backslash : escape sequence
				if(bSafeOnly)
					msg_ptr++;
				else
				{
					if(msg_ptr != begin)
						buffer.append(begin, msg_ptr - begin);
					msg_ptr++;
					if(*msg_ptr)
					{
						// decode the escape
						msg_ptr = decodeCtcpEscape(msg_ptr, buffer);
						begin = msg_ptr;
					}
					// else it is a senseless trailing backslash.
					// Just ignore and let the function
					// return spontaneously.
				}
				break;
			case ' ':
				// space : separate tokens if not in string
				if(bInString || (!bSpaceBreaks))
					msg_ptr++;
				else
				{
					// Not in string and space breaks: end of token
					// skip trailing white space (this could be avoided)
					// and return
					if(msg_ptr != begin)
						buffer.append(begin, msg_ptr - begin);
					while(*msg_ptr == ' ')
						msg_ptr++;
					return msg_ptr;
				}
				break;
			case '"':
				if(bInString && !bSafeOnly)
				{
					// A string terminator. We don't return
					// immediately since if !bSpaceBreaks
					// we must handle tokens until the end
					// and otherwise we just run up to the
					// next breaking space (but that's a bug anyway, heh).
					if(msg_ptr != begin)
						buffer.append(begin, msg_ptr - begin);
					bInString = 0;
					msg_ptr++;
					begin = msg_ptr;
				}
				else
				{
					// we don't begin a string here
					// since we're in the middle of the token
					// it is assumed to be simply a non encoded "
					msg_ptr++;
				}
				break;
			default:
				// any other char
				msg_ptr++;
				break;
		}
	}
	if(msg_ptr != begin)
		buffer.append(begin, msg_ptr - begin);
	return msg_ptr;
}

const char * KviIrcServerParser::extractCtcpParameter(const char * p_msg_ptr, QString & resultBuffer, bool bSpaceBreaks, bool bSafeOnly)
{
	// This one extracts the "next" ctcp parameter in p_msg_ptr
	// and puts it in the supplied buffer.
	// It is assumed that the leading and trailing CTCP
	// tags have been already removed.
	// Skips the leading and trailing spaces.
	// bSpaceBreaks should be set to false if (and only if) the
	// extracted parameter is the last in a positional parameter
	// based CTCP message.

	QByteArray buffer;
	const char * msg_ptr = p_msg_ptr;
	int bInString = 0;
	if(!msg_ptr)
		return nullptr;
	while(*msg_ptr == ' ')
		msg_ptr++; // skip leading spaces

	if(*msg_ptr == '"')
	{
		// a quoted parameter
		bInString = 1;
		msg_ptr++;
	}

	while(*msg_ptr)
	{
		switch(*msg_ptr)
		{
			case '\\':
				// backslash : escape sequence
				if(bSafeOnly)
					msg_ptr++;
				else
				{
					msg_ptr++;
					if(*msg_ptr)
					{
						// decode the escape
						msg_ptr = decodeCtcpEscape(msg_ptr, buffer);
					}
					// else it is a senseless trailing backslash.
					// Just ignore and let the function
					// return spontaneously.
				}
				break;
			case ' ':
				// space : separate tokens if not in string
				if(bInString || (!bSpaceBreaks))
				{
					buffer += *msg_ptr;
					msg_ptr++;
				}
				else
				{
					// Not in string and space breaks: end of token
					// skip trailing white space (this could be avoided)
					// and return
					while(*msg_ptr == ' ')
						msg_ptr++;
					resultBuffer = buffer;
					return msg_ptr;
				}
				break;
			case '"':
				if(bInString && !bSafeOnly)
				{
					// A string terminator. We don't return
					// immediately since if !bSpaceBreaks
					// we must handle tokens until the end
					// and otherwise we just run up to the
					// next breaking space (but that's a bug anyway, heh).
					buffer += *msg_ptr;
					bInString = 0;
					msg_ptr++;
				}
				else
				{
					// we don't begin a string here
					// since we're in the middle of the token
					// it is assumed to be simply a non encoded "
					buffer += *msg_ptr;
					msg_ptr++;
				}
				break;
			default:
				// any other char
				buffer += *msg_ptr;
				msg_ptr++;
				break;
		}
	}
	resultBuffer = buffer;
	return msg_ptr;
}

void KviIrcServerParser::parseCtcpRequest(KviCtcpMessage * msg)
{
	msg->pData = extractCtcpParameter(msg->pData, msg->szTag);

	bool bAction = KviQString::equalCI(msg->szTag, "ACTION");

	if (!bAction)
	{
		if (IS_ME(msg->msg, msg->pSource->nick()) && !IS_ME(msg->msg, msg->szTarget))
		{
			// "znc.in/self-message" capability: Handle a replayed message from ourselves to someone else.
			return;
		}
	}

	if(msg->szTag == "DCC" || msg->szTag == "XDCC" || msg->szTag == "TDCC") // is dcc request
	{
		if(KVI_OPTION_BOOL(KviOption_boolIgnoreCtcpDcc))
		{
			msg->msg->console()->output(KVI_OUT_IGNORE, __tr2qs("Ignoring DCC from \r!nc\r%s\r [%s@\r!h\r%s\r]"),
			    msg->pSource->nick().toUtf8().data(),
			    msg->pSource->user().toUtf8().data(),
			    msg->pSource->host().toUtf8().data());
			return;
		}
	}
	else
	{

		KviRegisteredUser * u = msg->msg->connection()->userDataBase()->registeredUser(msg->pSource->nick(), msg->pSource->user(), msg->pSource->host());

		//Ignore it?
		if(u)
		{
			if(
			    (!bAction && u->isIgnoreEnabledFor(KviRegisteredUser::Ctcp)) || (bAction && u->isIgnoreEnabledFor(IS_ME(msg->msg, msg->szTarget) ? KviRegisteredUser::Query : KviRegisteredUser::Channel)))
			{
				if(KVI_OPTION_BOOL(KviOption_boolVerboseIgnore))
				{
					msg->msg->console()->output(KVI_OUT_IGNORE, __tr2qs("Ignoring CTCP from \r!nc\r%s\r [%s@\r!h\r%s\r]"),
					    msg->pSource->nick().toUtf8().data(),
					    msg->pSource->user().toUtf8().data(),
					    msg->pSource->host().toUtf8().data());
				}
				return;
			}
		}
	}

	for(int i = 0; m_ctcpParseProcTable[i].msgName; i++)
	{
		if(KviQString::equalCS(KviQString::upperISO88591(msg->szTag), m_ctcpParseProcTable[i].msgName)
		    && m_ctcpParseProcTable[i].req)
		{
			if(!(m_ctcpParseProcTable[i].iFlags & KVI_CTCP_MESSAGE_PARSE_TRIGGERNOEVENT))
			{
				QString szData = msg->msg->connection()->decodeText(msg->pData);
				if(
				    KVS_TRIGGER_EVENT_6_HALTED(
				        KviEvent_OnCTCPRequest,
				        msg->msg->console(),
				        msg->pSource->nick(),
				        msg->pSource->user(),
				        msg->pSource->host(),
				        msg->szTarget,
				        msg->szTag,
				        szData))
					return;
			}
			(this->*(m_ctcpParseProcTable[i].req))(msg);
			return;
		}
	}

	QString szData = msg->msg->connection()->decodeText(msg->pData);
	// trigger the event on unrecognized requests too
	if(
	    KVS_TRIGGER_EVENT_6_HALTED(
	        KviEvent_OnCTCPRequest,
	        msg->msg->console(),
	        msg->pSource->nick(),
	        msg->pSource->user(),
	        msg->pSource->host(),
	        msg->szTarget,
	        msg->szTag,
	        szData))
		return;

	// unknown
	msg->bUnknown = true;
	echoCtcpRequest(msg);
}

void KviIrcServerParser::parseCtcpReply(KviCtcpMessage * msg)
{
	msg->pData = extractCtcpParameter(msg->pData, msg->szTag);

	for(int i = 0; m_ctcpParseProcTable[i].msgName; i++)
	{
		if(KviQString::equalCS(KviQString::upperISO88591(msg->szTag), m_ctcpParseProcTable[i].msgName)
		    && m_ctcpParseProcTable[i].rpl)
		{
			if(!(m_ctcpParseProcTable[i].iFlags & KVI_CTCP_MESSAGE_PARSE_TRIGGERNOEVENT))
			{
				QString szData = msg->msg->connection()->decodeText(msg->pData);
				if(KVS_TRIGGER_EVENT_6_HALTED(KviEvent_OnCTCPReply,
				       msg->msg->console(), msg->pSource->nick(), msg->pSource->user(),
				       msg->pSource->host(), msg->szTarget, msg->szTag, szData))
					return;
			}
			(this->*(m_ctcpParseProcTable[i].rpl))(msg);
			return;
		}
	}

	QString szData = msg->msg->connection()->decodeText(msg->pData);
	// trigger the event on unrecognized replies too
	if(KVS_TRIGGER_EVENT_6_HALTED(KviEvent_OnCTCPReply,
	       msg->msg->console(), msg->pSource->nick(), msg->pSource->user(),
	       msg->pSource->host(), msg->szTarget, msg->szTag, szData))
		return;

	// unknown
	msg->bUnknown = true;
	echoCtcpReply(msg);
}

// Ctcp message handlers

bool KviIrcServerParser::checkCtcpFlood(KviCtcpMessage * msg)
{
	if(!KVI_OPTION_BOOL(KviOption_boolUseCtcpFloodProtection))
		return false;

	kvi_time_t tNow = kvi_unixTime();

	KviIrcConnectionAntiCtcpFloodData * d = msg->msg->connection()->antiCtcpFloodData();

	unsigned int interval = (unsigned int)(((unsigned int)tNow) - ((unsigned int)d->lastCtcpTime()));

	if(interval < KVI_OPTION_UINT(KviOption_uintCtcpFloodCheckInterval))
	{
		d->increaseCtcpCount();
		if(d->ctcpCount() > KVI_OPTION_UINT(KviOption_uintMaxCtcpRequests))
		{
			// This is flood
			msg->bIsFlood = true;
			return true;
		}
	}
	else
	{
		d->setLastCtcpTime(tNow);
		d->setCtcpCount(1);
	}
	return false;
}

void KviIrcServerParser::replyCtcp(KviCtcpMessage * msg, const QString & data)
{
	QByteArray szNick = msg->msg->connection()->encodeText(msg->pSource->nick());
	msg->msg->connection()->sendFmtData(
	    "NOTICE %s :%c%s %s%c",
	    szNick.data(),
	    0x01,
	    msg->msg->connection()->encodeText(msg->szTag).data(),
	    msg->msg->connection()->encodeText(data).data(),
	    0x01);
}

void KviIrcServerParser::echoCtcpReply(KviCtcpMessage * msg)
{
	if(!msg->msg->haltOutput())
	{
		KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolCtcpRepliesToActiveWindow) ? msg->msg->console()->activeWindow() : msg->msg->console();

		bool bIsChannel = false;

		if(!IS_ME(msg->msg, msg->szTarget))
		{
			// Channel ctcp request!
			pOut = msg->msg->connection()->findChannel(msg->szTarget);
			if(!pOut)
			{
				pOut = msg->msg->console();
				pOut->output(KVI_OUT_SYSTEMWARNING,
				    __tr2qs("The following CTCP reply has unrecognized target %Q"), &(msg->szTarget));
			}
			else
				bIsChannel = true;
		}

		QString szData = msg->msg->connection()->decodeText(msg->pData);

		QString szWhat = bIsChannel ? __tr2qs("Channel CTCP") : QString("CTCP");

		pOut->output(
		    msg->bUnknown ? KVI_OUT_CTCPREPLYUNKNOWN : KVI_OUT_CTCPREPLY,
		    __tr2qs("%Q %Q reply from \r!n\r%Q\r [%Q@\r!h\r%Q\r]: %Q"),
		    &szWhat, &(msg->szTag), &(msg->pSource->nick()), &(msg->pSource->user()),
		    &(msg->pSource->host()), &szData);
	}
}

void KviIrcServerParser::echoCtcpRequest(KviCtcpMessage * msg)
{
	// FIXME: #warning "DEDICATED CTCP WINDOW...MINIMIZED ?"
	if(!msg->msg->haltOutput())
	{
		QString req = msg->szTag;
		if(*(msg->pData))
		{
			req.append(" ");
			req.append(msg->pData);
		}

		KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolCtcpRequestsToActiveWindow) ? msg->msg->console()->activeWindow() : msg->msg->console();

		bool bIsChannel = false;

		if(!IS_ME(msg->msg, msg->szTarget))
		{
			// Channel ctcp request!
			pOut = msg->msg->connection()->findChannel(msg->szTarget);
			if(!pOut)
			{
				pOut = msg->msg->console();
				pOut->output(KVI_OUT_SYSTEMWARNING,
				    __tr2qs("The following CTCP request has unrecognized target %Q"),
				    &(msg->szTarget));
			}
			else
				bIsChannel = true;
		}

		QString szRequest = req;
		QString szTag = msg->szTag;
		QString szWhat = bIsChannel ? __tr2qs("Channel CTCP") : QString("CTCP");

		if(msg->bIsFlood)
		{
			QString szData = msg->msg->connection()->decodeText(msg->pData);
			if(!KVS_TRIGGER_EVENT_6_HALTED(KviEvent_OnCTCPFlood, pOut, msg->pSource->nick(), msg->pSource->user(), msg->pSource->host(), msg->szTarget, msg->szTag, szData))
				pOut->output(KVI_OUT_CTCPREQUESTFLOOD,
				    __tr2qs("%Q %Q%c request from \r!n\r%Q\r [%Q@\r!h\r%Q\r] (%Q), ignored (flood limit exceeded)"),
				    &szWhat, &szTag, KviControlCodes::Reset, &(msg->pSource->nick()),
				    &(msg->pSource->user()), &(msg->pSource->host()), &szRequest);
		}
		else
		{
			QString szAction = msg->bUnknown ? __tr2qs("ignored (unrecognized)") : (msg->bIgnored ? __tr2qs("ignored") : __tr2qs("replied"));
			pOut->output(
			    msg->bUnknown ? KVI_OUT_CTCPREQUESTUNKNOWN : (msg->bIgnored ? KVI_OUT_CTCPREQUESTIGNORED : KVI_OUT_CTCPREQUESTREPLIED),
			    __tr2qs("%Q %Q%c request from \r!n\r%Q\r [%Q@\r!h\r%Q\r] (%Q), %Q"),
			    &szWhat, &szTag, KviControlCodes::Reset, &(msg->pSource->nick()),
			    &(msg->pSource->user()), &(msg->pSource->host()), &szRequest, &szAction);
		}
	}
}

void KviIrcServerParser::parseCtcpRequestPing(KviCtcpMessage * msg)
{
	if(!checkCtcpFlood(msg))
	{
		if(!KVI_OPTION_BOOL(KviOption_boolIgnoreCtcpPing))
		{
			replyCtcp(msg, msg->msg->connection()->encodeText(msg->pData));
		}
		else
			msg->bIgnored = true;
	}

	echoCtcpRequest(msg);
}

void KviIrcServerParser::parseCtcpReplyPing(KviCtcpMessage * msg)
{
	if(!msg->msg->haltOutput())
	{
		KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolCtcpRepliesToActiveWindow) ? msg->msg->console()->activeWindow() : msg->msg->console();

		bool bIsChannel = false;

		if(!IS_ME(msg->msg, msg->szTarget))
		{
			// Channel ctcp request!
			pOut = msg->msg->connection()->findChannel(msg->szTarget);
			if(!pOut)
			{
				pOut = msg->msg->console();
				pOut->output(KVI_OUT_SYSTEMWARNING,
				    __tr2qs("The following CTCP PING reply has unrecognized target \"%Q\""),
				    &(msg->szTarget));
			}
			else
				bIsChannel = true;
		}

		unsigned int uSecs;
		unsigned int uMSecs = 0;

		KviCString szTime;

		struct timeval tv;
		kvi_gettimeofday(&tv);

		msg->pData = extractCtcpParameter(msg->pData, szTime, true);

		bool bOk;

		if(szTime.contains('.'))
		{
			KviCString szUSecs = szTime;
			szUSecs.cutToFirst('.');
			szTime.cutFromFirst('.');

			uMSecs = szUSecs.toUInt(&bOk);
			if(!bOk)
			{
				uMSecs = 0;
				tv.tv_usec = 0;
			}
		}
		else
			tv.tv_usec = 0;

		uSecs = szTime.toUInt(&bOk);
		if(!bOk)
			pOut->output(KVI_OUT_SYSTEMWARNING,
			    __tr2qs("The following CTCP PING reply has a broken time identifier \"%S\", don't trust the displayed time"), &szTime);

		unsigned int uDiffSecs = tv.tv_sec - uSecs;

		while(uMSecs > 1000000)
			uMSecs /= 10; // precision too high?
		if(((unsigned int)tv.tv_usec) < uMSecs)
		{
			tv.tv_usec += 1000000;
			if(uDiffSecs > 0)
				uDiffSecs--;
		}
		unsigned int uDiffMSecs = (tv.tv_usec - uMSecs) / 1000;

		QString szWhat = bIsChannel ? __tr2qs("Channel CTCP") : QString("CTCP");

		pOut->output(
		    msg->bUnknown ? KVI_OUT_CTCPREPLYUNKNOWN : KVI_OUT_CTCPREPLY,
		    __tr2qs("%Q PING reply from \r!n\r%Q\r [%Q@\r!h\r%Q\r]: %u sec %u msec"),
		    &szWhat, &(msg->pSource->nick()),
		    &(msg->pSource->user()), &(msg->pSource->host()), uDiffSecs, uDiffMSecs);
	}
}

void KviIrcServerParser::parseCtcpRequestVersion(KviCtcpMessage * msg)
{
	if(!checkCtcpFlood(msg))
	{
		if(!KVI_OPTION_BOOL(KviOption_boolIgnoreCtcpVersion))
		{
			QString szVersion;

			szVersion = "KVIrc " KVI_VERSION;
			if(KviBuildInfo::buildRevision() != QString())
			{
				szVersion += " git:";
				szVersion += KviBuildInfo::buildRevision();
			}
			szVersion += " '" KVI_RELEASE_NAME "' " KVI_SOURCES_DATE " - build ";
			szVersion += KviBuildInfo::buildDate();
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
			szVersion.append(QString(" - %1").arg(KviRuntimeInfo::version()));
#else
			szVersion.append(QString(" - %1 (%2)").arg(KviRuntimeInfo::name(), KviRuntimeInfo::release()));
#endif
			if(!KVI_OPTION_STRING(KviOption_stringCtcpVersionPostfix).isEmpty())
			{
				QString sz = KVI_OPTION_STRING(KviOption_stringCtcpVersionPostfix);
				if(!sz.isEmpty())
				{
					szVersion.append(" :");
					szVersion.append(sz);
				}
			}
			replyCtcp(msg, szVersion);
		}
		else
			msg->bIgnored = true;
	}

	echoCtcpRequest(msg);
}

void KviIrcServerParser::parseCtcpRequestUserinfo(KviCtcpMessage * msg)
{
	if(!checkCtcpFlood(msg))
	{
		if(!KVI_OPTION_BOOL(KviOption_boolIgnoreCtcpUserinfo))
		{
			QString szReply;
			if(!KVI_OPTION_STRING(KviOption_stringCtcpUserInfoAge).isEmpty())
			{
				szReply = "Age=";
				szReply += KVI_OPTION_STRING(KviOption_stringCtcpUserInfoAge);
			}
			if(!KVI_OPTION_STRING(KviOption_stringCtcpUserInfoGender).isEmpty())
			{
				if(!szReply.isEmpty())
					szReply += "; ";
				szReply += "Gender=";
				szReply += KVI_OPTION_STRING(KviOption_stringCtcpUserInfoGender);
			}
			if(!KVI_OPTION_STRING(KviOption_stringCtcpUserInfoLocation).isEmpty())
			{
				if(!szReply.isEmpty())
					szReply += "; ";
				szReply += "Location=";
				szReply += KVI_OPTION_STRING(KviOption_stringCtcpUserInfoLocation);
			}
			if(!KVI_OPTION_STRING(KviOption_stringCtcpUserInfoLanguages).isEmpty())
			{
				if(!szReply.isEmpty())
					szReply += "; ";
				szReply += "Languages=";
				szReply += KVI_OPTION_STRING(KviOption_stringCtcpUserInfoLanguages);
			}
			if(!KVI_OPTION_STRING(KviOption_stringCtcpUserInfoOther).isEmpty())
			{
				if(!szReply.isEmpty())
					szReply += "; ";
				szReply += KVI_OPTION_STRING(KviOption_stringCtcpUserInfoOther);
			}
			if(szReply.isEmpty())
				szReply = KVI_DEFAULT_CTCP_USERINFO_REPLY;
			replyCtcp(msg, szReply);
		}
		else
			msg->bIgnored = true;
	}

	echoCtcpRequest(msg);
}

// FIXME: KEEP THIS TABLE UP TO DATE

static const char * ctcpTagTable[][2] = {
	{ "PING", "Returns given parameters without parsing them" },
	{ "VERSION", "Returns the version of this client" },
	{ "CLIENTINFO", "With no parameters, lists supported CTCP tags,"
	                " 'CLIENTINFO <tag>' describes <tag>" },
	{ "USERINFO", "Returns personal information about the current user" },
	{ "FINGER", "Returns information about the current user" },
	{ "SOURCE", "Returns the client homepage URL" },
	{ "TIME", "Returns the current local time" },
	{ "ACTION", "Used to describe actions, generates no reply" },
	{ "AVATAR", "Returns the current avatar (may trigger a DCC GET) or"
	            " sets your own on this side if sent through a NOTICE" },
	{ "DCC", "Initiates a DCC connection (XDCC,TDCC)" },
	{ "PAGE", "Leaves a message for this user" },
	{ nullptr, nullptr }
};

void KviIrcServerParser::parseCtcpRequestClientinfo(KviCtcpMessage * msg)
{
	// this is completely latin1
	if(!checkCtcpFlood(msg))
	{
		if(!KVI_OPTION_BOOL(KviOption_boolIgnoreCtcpClientinfo))
		{
			KviCString szTag;
			msg->pData = extractCtcpParameter(msg->pData, szTag, false);
			szTag.trim();
			szTag.toUpperISO88591();
			if(szTag.isEmpty())
			{
				QString reply("KVIrc " KVI_VERSION " '" KVI_RELEASE_NAME "' " KVI_SOURCES_DATE " - http://www.kvirc.net - Supported tags: ");
				for(int i = 0; ctcpTagTable[i][0]; i++)
				{
					reply += ctcpTagTable[i][0];
					if(ctcpTagTable[i + 1][0])
						reply += ",";
				}
				reply += " - Use 'CLIENTINFO <tag>' for a description of each tag";
				replyCtcp(msg, reply);
			}
			else
			{
				bool bFound = false;
				for(int i = 0; ctcpTagTable[i][0] && !bFound; i++)
				{
					if(kvi_strEqualCS(ctcpTagTable[i][0], szTag.ptr()))
					{
						KviCString reply(KviCString::Format, "%s: %s", ctcpTagTable[i][0], ctcpTagTable[i][1]);
						replyCtcp(msg, reply.ptr());
						bFound = true;
					}
				}
				if(!bFound)
				{
					msg->szTag = "ERRMSG";
					KviCString reply(KviCString::Format, "Unsupported tag %s", szTag.ptr());
					replyCtcp(msg, reply.ptr());
				}
			}
		}
		else
			msg->bIgnored = true;
	}

	echoCtcpRequest(msg);
}

void KviIrcServerParser::parseCtcpRequestFinger(KviCtcpMessage * msg)
{
	// completely latin1 atm
	if(!checkCtcpFlood(msg))
	{
		if(!KVI_OPTION_BOOL(KviOption_boolIgnoreCtcpFinger))
		{
			KviCString username = getenv("USER");
			if(username.isEmpty())
				username = getenv("LOGNAME");
			if(username.isEmpty())
				username = msg->msg->connection()->userInfo()->userName();
			// FIXME: #warning "UTSNAME ?...AND OTHER INFO ?...SYSTEM IDLE TIME ?...KVIRC IDLE TIME ?"
			KviCString reply(KviCString::Format, "%s", username.ptr());
			replyCtcp(msg, reply.ptr());
		}
		else
			msg->bIgnored = true;
	}

	echoCtcpRequest(msg);
}

void KviIrcServerParser::parseCtcpRequestSource(KviCtcpMessage * msg)
{
	if(!checkCtcpFlood(msg))
	{
		if(!KVI_OPTION_BOOL(KviOption_boolIgnoreCtcpSource))
		{
			QString version = "KVIrc " KVI_VERSION " '" KVI_RELEASE_NAME "' - http://www.kvirc.net/";
			if(!KVI_OPTION_STRING(KviOption_stringCtcpSourcePostfix).isEmpty())
			{
				version += " :";
				version += KVI_OPTION_STRING(KviOption_stringCtcpSourcePostfix);
			}
			replyCtcp(msg, version);
		}
		else
			msg->bIgnored = true;
	}

	echoCtcpRequest(msg);
}

void KviIrcServerParser::parseCtcpRequestTime(KviCtcpMessage * msg)
{
	if(!checkCtcpFlood(msg))
	{
		if(!KVI_OPTION_BOOL(KviOption_boolIgnoreCtcpTime))
		{
			QString szTmp;
			QDateTime date = QDateTime::currentDateTime();
			switch(KVI_OPTION_UINT(KviOption_uintOutputDatetimeFormat))
			{
				case 0:
					// this is the equivalent to an empty date.toString() call, but it's needed
					// to ensure qt4 will use the default() locale and not the system() one
					szTmp = QLocale().toString(date, "ddd MMM d hh:mm:ss yyyy");
					break;
				case 1:
					szTmp = date.toString(Qt::ISODate);
					break;
				case 2:
					szTmp = date.toString(Qt::SystemLocaleShortDate);
					break;
			}
			replyCtcp(msg, szTmp);
		}
		else
			msg->bIgnored = true;
	}

	echoCtcpRequest(msg);
}

void KviIrcServerParser::parseCtcpRequestPage(KviCtcpMessage * msg)
{
	if(!checkCtcpFlood(msg))
	{
		if(!KVI_OPTION_BOOL(KviOption_boolIgnoreCtcpPage))
		{
			KVI_OPTION_STRING(KviOption_stringCtcpPageReply) = KVI_OPTION_STRING(KviOption_stringCtcpPageReply).trimmed();
			if(KVI_OPTION_STRING(KviOption_stringCtcpPageReply).isEmpty())
				KVI_OPTION_STRING(KviOption_stringCtcpPageReply) = KVI_DEFAULT_CTCP_PAGE_REPLY;

			replyCtcp(msg, KVI_OPTION_STRING(KviOption_stringCtcpPageReply));

			bool bIsChannel = !IS_ME(msg->msg, msg->szTarget);

			if((KVI_OPTION_BOOL(KviOption_boolShowDialogOnCtcpPage) && !bIsChannel) || (KVI_OPTION_BOOL(KviOption_boolShowDialogOnChannelCtcpPage) && bIsChannel))
			{
				if(!g_pCtcpPageDialog)
					g_pCtcpPageDialog = new KviCtcpPageDialog();
				KviCString szData8;
				szData8 = msg->pData;
				QString szData = KviQString::toHtmlEscaped(msg->msg->connection()->decodeText(szData8.ptr()));
				g_pCtcpPageDialog->addPage(msg->pSource->nick(), msg->pSource->user(), msg->pSource->host(), szData);
				g_pCtcpPageDialog->popup();
			}
		}
		else
			msg->bIgnored = true;
	}

	echoCtcpRequest(msg);
}

#ifdef COMPILE_CRYPT_SUPPORT
#define DECRYPT_IF_NEEDED(target, txt, type, type2, buffer, retptr, retmsgtype)        \
	if(KviCryptSessionInfo * cinf = target->cryptSessionInfo())                    \
	{                                                                              \
		if(cinf->m_bDoDecrypt)                                                 \
		{                                                                      \
			switch(cinf->m_pEngine->decrypt(txt, buffer))                  \
			{                                                              \
				case KviCryptEngine::DecryptOkWasEncrypted:            \
					retptr = buffer.ptr();                         \
					retmsgtype = type2;                            \
					break;                                         \
				case KviCryptEngine::DecryptOkWasPlainText:            \
				case KviCryptEngine::DecryptOkWasEncoded:              \
					retptr = buffer.ptr();                         \
					retmsgtype = type;                             \
					break;                                         \
				default: /* also case KviCryptEngine::DecryptError: */ \
				{                                                      \
					QString szEngineError = cinf->m_pEngine->lastError(); \
					target->output(KVI_OUT_SYSTEMERROR,            \
					    __tr2qs("The following message appears to be encrypted but the encryption engine failed to decode it: %Q"), \
					    &szEngineError);                           \
					retptr = txt + 1;                              \
					retmsgtype = type;                             \
				}                                                      \
				break;                                                 \
			}                                                              \
		}                                                                      \
		else                                                                   \
			retptr = txt, retmsgtype = type;                               \
	}                                                                              \
	else                                                                           \
		retptr = txt, retmsgtype = type;
#else //COMPILE_CRYPT_SUPPORT
#define DECRYPT_IF_NEEDED(target, txt, type, type2, buffer, retptr, retmsgtype) \
	retptr = txt;                                                           \
	retmsgtype = type;
#endif //COMPILE_CRYPT_SUPPORT

void KviIrcServerParser::parseCtcpRequestAction(KviCtcpMessage * msg)
{
	KviCString szData8(msg->pData);
	// CTCP ACTION is a special exception... most clients do not encode/decode it.
	//msg->pData = extractCtcpParameter(msg->pData,szData8,false);

	KviWindow * pOut = nullptr;
	bool bIsChannel = msg->msg->connection()->serverInfo()->supportedChannelTypes().indexOf(msg->szTarget[0]) != -1;
	int msgtype = KVI_OUT_ACTION;

	// "znc.in/self-message" capability: Handle a replayed message from ourselves to someone else.
	bool bSelfMessage = IS_ME(msg->msg, msg->pSource->nick());
	QString szTargetNick, szTargetUser, szTargetHost;
	msg->msg->decodeAndSplitMask(msg->szTarget.toLatin1().data(), szTargetNick, szTargetUser, szTargetHost);
	QString szWindow = bIsChannel || bSelfMessage ? szTargetNick : msg->pSource->nick();
	const QString &szOtherNick = bSelfMessage ? szTargetNick : msg->pSource->nick();
	const QString &szOtherUser = bSelfMessage ? szTargetUser : msg->pSource->user();
	const QString &szOtherHost = bSelfMessage ? szTargetHost : msg->pSource->host();

	QString szData;

	if(bIsChannel)
	{
		KviChannelWindow * chan = msg->msg->connection()->findChannel(msg->szTarget);
		if(chan)
		{
			KviCString szBuffer;
			const char * txtptr;

			DECRYPT_IF_NEEDED(chan, szData8.ptr(), KVI_OUT_ACTION, KVI_OUT_ACTIONCRYPTED, szBuffer, txtptr, msgtype)

			szData = chan->decodeText(txtptr);

			pOut = static_cast<KviWindow *>(chan);
		}
		else
			szData = msg->msg->connection()->decodeText(szData8.ptr());
	}
	else
	{
		KviQueryWindow * query = msg->msg->connection()->findQuery(szWindow);
		if(!query)
		{
			// New query requested ?
			// FIXME: 			#warning "CHECK FOR SPAM!"
			if(KVI_OPTION_BOOL(KviOption_boolCreateQueryOnPrivmsg))
			{
				// We still want to create it
				// Give the scripter a chance to filter it out again
				if(KVS_TRIGGER_EVENT_4_HALTED(KviEvent_OnQueryWindowRequest, msg->msg->console(),
				       msg->pSource->nick(),
				       msg->pSource->user(),
				       msg->pSource->host(),
				       szData))
				{
					// check if the scripter hasn't created it
					query = msg->msg->connection()->findQuery(szWindow);
				}
				else
				{
					// no query yet, create it!
					// this will trigger OnQueryWindowCreated
					query = msg->msg->console()->connection()->createQuery(szWindow);
					query->setTarget(szOtherNick, szOtherUser, szOtherHost);
				}
				if(!KVI_OPTION_STRING(KviOption_stringOnNewQueryOpenedSound).isEmpty())
				{
					KviKvsVariantList soundParams{new KviKvsVariant{KVI_OPTION_STRING(KviOption_stringOnNewQueryOpenedSound)}};
					KviKvsScript::run("snd.play $0", nullptr, &soundParams);
				}
			}
		}
		else
		{
			if(!KVI_OPTION_STRING(KviOption_stringOnQueryMessageSound).isEmpty() && !query->hasAttention())
			{
				KviKvsVariantList soundParams(new KviKvsVariant(KVI_OPTION_STRING(KviOption_stringOnQueryMessageSound)));
				// coverity is wrong here.
				KviKvsScript::run("snd.play $0", query, &soundParams);
			}
		}

		if(query)
		{
			KviCString szBuffer;
			const char * txtptr;

			DECRYPT_IF_NEEDED(query, szData8.ptr(), KVI_OUT_ACTION, KVI_OUT_ACTIONCRYPTED, szBuffer, txtptr, msgtype)

			szData = query->decodeText(txtptr);
		}
		else
			szData = msg->msg->connection()->decodeText(szData8.ptr());

		pOut = static_cast<KviWindow *>(query);
	}

	bool bTargetFound = pOut;
	if(!pOut)
	{
		pOut = KVI_OPTION_BOOL(KviOption_boolExternalMessagesToActiveWindow) ? msg->msg->console()->activeWindow() : msg->msg->console();
	}

	//see bug ticket #220
	if(KVI_OPTION_BOOL(KviOption_boolStripMircColorsInUserMessages))
		szData = KviControlCodes::stripControlBytes(szData);

	if(KVS_TRIGGER_EVENT_7_HALTED(KviEvent_OnAction, pOut,
	       msg->pSource->nick(),
	       msg->pSource->user(),
	       msg->pSource->host(),
	       msg->szTarget,
	       szData,
	       msg->msg->messageTagsKvsHash(),
	       (kvs_int_t)(msgtype == KVI_OUT_ACTIONCRYPTED)))
	{
		msg->msg->setHaltOutput();
		return;
	}

	int type = msg->msg->console()->applyHighlighting(pOut, msgtype, msg->pSource->nick(), msg->pSource->user(), msg->pSource->host(), szData);

	if(type < 0)
		return; // event stopped the message!

	if(type == KVI_OUT_HIGHLIGHT || !bIsChannel)
	{
		if(!pOut->hasAttention(KviWindow::MainWindowIsVisible))
		{
			if(KVI_OPTION_BOOL(KviOption_boolFlashWindowOnHighlightedMessages))
				pOut->demandAttention();
			if(KVI_OPTION_BOOL(KviOption_boolPopupNotifierOnHighlightedMessages))
			{
				QString szMsg = "<b>";
				szMsg += msg->pSource->nick();
				szMsg += "</b> ";
				szMsg += KviQString::toHtmlEscaped(szData);
				//qDebug("KviIrcServerParser_ctcp.cpp:975 debug: %s",szMsg.data());
				g_pApp->notifierMessage(pOut, KVI_OPTION_MSGTYPE(msgtype).pixId(), szMsg, KVI_OPTION_UINT(KviOption_uintNotifierAutoHideTime));
			}
		}
	}

	if(bTargetFound)
	{
		QString szMsg = QString("\r!n\r%1\r ").arg(msg->pSource->nick());
		szMsg += szData;
		if(bIsChannel)
		{
			((KviChannelWindow *)pOut)->outputMessage(type, szMsg, msg->msg->serverTime());
		}
		else
		{
			pOut->outputNoFmt(type, szMsg, 0, msg->msg->serverTime());
		}
	}
	else
	{
		if(bIsChannel)
		{
			pOut->output(KVI_OUT_SYSTEMWARNING,
			    __tr2qs("The following CTCP ACTION has unrecognized target %Q"),
			    &(msg->szTarget));
		}
		KviCString buffer1, buffer2;
		pOut->output(type,
		    __tr2qs("CTCP ACTION from \r!n\r%Q\r [%Q@\r!h\r%Q\r]: %Q"),
		    &(msg->pSource->nick()), &(msg->pSource->user()),
		    &(msg->pSource->host()), &szData);
	}
}

// FIXME: #warning "UTSNAME ?...AND OTHER INFO ?...SYSTEM IDLE TIME ?...KVIRC IDLE TIME ?"
void KviIrcServerParser::parseCtcpRequestAvatar(KviCtcpMessage * msg)
{
	// AVATAR
	if(!KVI_OPTION_BOOL(KviOption_boolIgnoreCtcpAvatar))
	{
		QString szGenderTag = " ";
		if(KVI_OPTION_STRING(KviOption_stringCtcpUserInfoGender).startsWith("m", Qt::CaseInsensitive))
		{
			szGenderTag.append("M");
		}
		else if(KVI_OPTION_STRING(KviOption_stringCtcpUserInfoGender).startsWith("f", Qt::CaseInsensitive))
		{
			szGenderTag.append("F");
		}
		else
		{
			szGenderTag.append("?");
		}

		KviAvatar * a = msg->msg->console()->currentAvatar();
		if(a)
		{
			if(!checkCtcpFlood(msg))
			{
				// FIXME: #warning "OPTION FOR SETTING A FIXED BIND ADDRESS FOR OUTGOING DCC OFFERS"
				QString szUserMask;
				msg->pSource->mask(szUserMask);

				QString szReply, szFileName;
				szFileName = a->name();
				if(KVI_OPTION_BOOL(KviOption_boolDCCFileTransferReplaceOutgoingSpacesWithUnderscores))
					szFileName.replace(" ", "_");

				// escape the spaces with the right octal code
				encodeCtcpParameter(szFileName.toUtf8().data(), szReply);

				if(!a->isRemote())
				{
					if(!g_pSharedFilesManager->addSharedFile(szFileName, a->localPath(), szUserMask, KVI_OPTION_UINT(KviOption_uintAvatarOfferTimeoutInSecs)))
					{
						msg->msg->console()->output(KVI_OUT_SYSTEMWARNING, __tr2qs("Unable to add file offer for file %Q (File not readable?)"), &(a->localPath()));
					}
					else
					{
						if(_OUTPUT_VERBOSE)
						{
							msg->msg->console()->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs("Added %d sec file offer for file %Q (%Q) to recipient %Q"),
							    KVI_OPTION_UINT(KviOption_uintAvatarOfferTimeoutInSecs), &(a->name()), &(a->localPath()), &szUserMask);
						}
					}
				}

				szReply.append(szGenderTag);
				replyCtcp(msg, szReply);
			}
		}
		else
		{
			// no avatar set.. ignore channel requests if the user wishes
			if(!IS_ME(msg->msg, msg->szTarget))
			{
				// channel target
				if(KVI_OPTION_BOOL(KviOption_boolIgnoreChannelAvatarRequestsWhenNoAvatarSet))
					msg->bIgnored = true;
			}
			if(!msg->bIgnored)
				replyCtcp(msg, "");
		}
	}
	else
		msg->bIgnored = true;

	echoCtcpRequest(msg);
}

void KviIrcServerParser::parseCtcpReplyAvatar(KviCtcpMessage * msg)
{
	QString szRemoteFile;
	QString szGender;
	QString decoded = msg->msg->console()->decodeText(msg->pData);

	decoded = extractCtcpParameter(decoded.toUtf8().data(), szRemoteFile, true);
	decoded = extractCtcpParameter(decoded.toUtf8().data(), szGender, true);
	szRemoteFile = szRemoteFile.trimmed();

	bool bPrivate = IS_ME(msg->msg, msg->szTarget);

	QString textLine;
	KviAvatar * avatar = nullptr;

	bool bResetAvatar = true;

	QString nickLink = QString("\r!n\r%1\r").arg(msg->pSource->nick());

	KviIrcUserEntry * e = msg->msg->connection()->userDataBase()->find(msg->pSource->nick());
	if(e)
	{
		if((szGender == "m") || (szGender == "M"))
		{
			e->setGender(KviIrcUserEntry::Male);
		}
		else if((szGender == "f") || (szGender == "F"))
		{
			e->setGender(KviIrcUserEntry::Female);
		}
		else
		{
			e->setGender(KviIrcUserEntry::Unknown);
		}
	}

	QString szWhere = bPrivate ? __tr2qs("private") : __tr2qs("channel notification:");
	QString szWhat = bPrivate ? __tr2qs("notification") : msg->szTarget;

	if(szRemoteFile.isEmpty())
	{
		// avatar unset
		textLine = QString(__tr2qs("%1 unsets avatar")).arg(nickLink);
		if(_OUTPUT_VERBOSE)
			KviQString::appendFormatted(textLine, " (%Q %Q)", &szWhere, &szWhat);
	}
	else
	{

		// FIXME: #warning "The avatar should be the one with the requested size!!"
		textLine = QString(__tr2qs("%1 changes avatar to %2")).arg(nickLink, szRemoteFile);
		if(_OUTPUT_VERBOSE)
			KviQString::appendFormatted(textLine, " (%Q %Q)", &szWhere, &szWhat);

		bool bIsUrl = ((KviQString::equalCIN("http://", szRemoteFile, 7) && (szRemoteFile.length() > 7)) || (KviQString::equalCIN("https://", szRemoteFile, 8) && (szRemoteFile.length() > 8)));
		if(!bIsUrl)
		{
			// no hacks
			KviQString::cutToLast(szRemoteFile, '/');
			KviQString::cutToLast(szRemoteFile, '\\');
		}

		avatar = g_pIconManager->getAvatar(QString(), szRemoteFile);

		if((avatar == nullptr) && e)
		{
			// we have no such file on our HD....
			bResetAvatar = false;
			// request DCC GET ?
			if(KVI_OPTION_BOOL(KviOption_boolRequestMissingAvatars))
			{
				// FIXME: #warning "Request avatars only from registered users ?"
				// FIXME: #warning "Ask before making the request ?"
				if(bIsUrl)
				{
					QString szLocalFilePath;
					QString szLocalFile = szRemoteFile;
					g_pIconManager->urlToCachedFileName(szLocalFile);
					g_pApp->getLocalKvircDirectory(szLocalFilePath, KviApplication::Avatars, szLocalFile);
					KviQString::escapeKvs(&szLocalFilePath, KviQString::EscapeSpace);
					QString szCommand = "http.get -w=nm ";
					unsigned int uMaxSize = KVI_OPTION_UINT(KviOption_uintMaximumRequestedAvatarSize);
					if(uMaxSize > 0)
						KviQString::appendFormatted(szCommand, "-m=%u ", uMaxSize);
					szRemoteFile = szRemoteFile.replace(";", "%3B");
					szRemoteFile = szRemoteFile.replace("\"", "%22");
					szRemoteFile = szRemoteFile.replace("\\", "\\\\");
					szRemoteFile = szRemoteFile.replace("$", "\\$");
					szRemoteFile = szRemoteFile.replace("%", "\\%");
					szCommand += "\"" + szRemoteFile + "\"";
					szCommand += " ";
					szCommand += szLocalFilePath;

					if(KviKvsScript::run(szCommand, msg->msg->console()))
					{
						if(_OUTPUT_VERBOSE)
						{
							KviQString::appendFormatted(textLine,
							    __tr2qs(": No valid local copy of avatar available, requesting one (HTTP GET %s)"),
							    szRemoteFile.toUtf8().data());
						}
						g_pApp->setAvatarOnFileReceived(msg->msg->console(),
						    szRemoteFile, msg->pSource->nick(), msg->pSource->user(), msg->pSource->host());
					}
					else
					{
						if(_OUTPUT_VERBOSE)
							KviQString::appendFormatted(textLine, __tr2qs(": No valid local copy of avatar available; failed to start an HTTP transfer, ignoring"));
					}
				}
				else
				{
					if(!checkCtcpFlood(msg))
					{
						if(_OUTPUT_VERBOSE)
						{
							KviQString::appendFormatted(textLine,
							    __tr2qs(": No valid local copy of avatar available, requesting one (DCC GET %s)"),
							    szRemoteFile.toUtf8().data());
						}

						QString szFName;
						encodeCtcpParameter(szRemoteFile.toUtf8().data(), szFName);
						msg->msg->connection()->sendFmtData("PRIVMSG %s :%cDCC GET %s%c",
						    msg->msg->connection()->encodeText(msg->pSource->nick()).data(), 0x01, msg->msg->connection()->encodeText(szFName.toUtf8().data()).data(), 0x01);
						g_pApp->setAvatarOnFileReceived(msg->msg->console(),
						    szRemoteFile, msg->pSource->nick(), msg->pSource->user(), msg->pSource->host());
					}
					else
					{
						if(_OUTPUT_VERBOSE)
							KviQString::appendFormatted(textLine, __tr2qs(": No valid local copy of avatar available; flood limit exceeded, ignoring"));
					}
				}
			}
			else
			{
				if(_OUTPUT_VERBOSE)
					KviQString::appendFormatted(textLine, __tr2qs(": No valid local copy of avatar available, ignoring"));
			}
		}
	}

	if(!e)
	{
		if(_OUTPUT_VERBOSE)
			KviQString::appendFormatted(textLine, __tr2qs(": No such nickname in the user database, ignoring the change"));
		msg->msg->console()->outputNoFmt(KVI_OUT_AVATAR, textLine);
		return;
	}

	if(bResetAvatar)
		e->setAvatar(avatar);

	msg->msg->console()->avatarChanged(avatar, msg->pSource->nick(), msg->pSource->user(), msg->pSource->host(),
	    msg->msg->haltOutput() ? QString() : textLine);
}

using dccModuleCtcpDccParseRoutine = void (*)(KviDccRequest *);

void KviIrcServerParser::parseCtcpRequestDcc(KviCtcpMessage * msg)
{
	KviDccRequest p;
	KviCString aux = msg->pData;
	msg->pData = extractCtcpParameter(msg->pData, p.szType, true, true);
	msg->pData = extractCtcpParameter(msg->pData, p.szParam1);
	msg->pData = extractCtcpParameter(msg->pData, p.szParam2);
	msg->pData = extractCtcpParameter(msg->pData, p.szParam3);
	msg->pData = extractCtcpParameter(msg->pData, p.szParam4);
	msg->pData = extractCtcpParameter(msg->pData, p.szParam5);
	p.ctcpMsg = msg;
	p.bIPv6 = msg->msg->console()->isIPv6Connection();
	p.pConsole = msg->msg->console();

	KviRegisteredUser * u = msg->msg->connection()->userDataBase()->registeredUser(msg->pSource->nick(), msg->pSource->user(), msg->pSource->host());

	if(u)
	{
		if(u->isIgnoreEnabledFor(KviRegisteredUser::Dcc))
		{
			if(KVI_OPTION_BOOL(KviOption_boolVerboseIgnore))
			{
				msg->msg->console()->output(KVI_OUT_DCCREQUEST,
				    __tr2qs("Ignoring DCC %S request from \r!n\r%Q\r [%Q@\r!h\r%Q\r] (%Q %S)"),
				    &p.szType, &(msg->pSource->nick()),
				    &(msg->pSource->user()), &(msg->pSource->host()),
				    &msg->szTag, &aux);
			}
			return;
		}
	}

	bool bIsFlood = checkCtcpFlood(msg);

	if(bIsFlood && ((kvi_strEqualCI(p.szType.ptr(), "SEND")) || (kvi_strEqualCI(p.szType.ptr(), "RSEND")) || (kvi_strEqualCI(p.szType.ptr(), "TSEND")) || (kvi_strEqualCI(p.szType.ptr(), "TRSEND"))))
	{
		// don't consider as flood the avatars we have requested
		if(g_pApp->findPendingAvatarChange(msg->msg->console(), msg->pSource->nick(), p.szParam1.ptr()))
			bIsFlood = false;
	}

	if(!bIsFlood)
	{
		if(!msg->msg->haltOutput())
		{
			QString decoded = msg->msg->console()->decodeText(p.szType.ptr());
			msg->msg->console()->output(KVI_OUT_DCCREQUEST,
			    __tr2qs("Processing DCC %Q request from \r!n\r%Q\r [%Q@\r!h\r%Q\r] (%s %s)"),
			    &decoded, &(msg->pSource->nick()),
			    &(msg->pSource->user()), &(msg->pSource->host()),
			    msg->szTag.toUtf8().data(),
			    msg->msg->console()->decodeText(aux).toUtf8().data());
		}

		KviModule * m = g_pModuleManager->getModule("dcc");
		if(!m)
		{
			msg->msg->console()->output(KVI_OUT_DCCERROR,
			    __tr2qs("Unable to process the above request: can't load DCC module (%Q)"), &(g_pModuleManager->lastError()));
		}
		else
		{
			dccModuleCtcpDccParseRoutine proc = (dccModuleCtcpDccParseRoutine)m->getSymbol("dccModuleCtcpDccParseRoutine");
			if(!proc)
			{
				msg->msg->console()->outputNoFmt(KVI_OUT_DCCERROR,
				    __tr2qs("Unable to process the above request: DCC module may be broken"));
			}
			else
			{
				proc(&p);
			}
		}
	}
	else
	{
		// That's flood
		echoCtcpRequest(msg);
	}
}

void KviIrcServerParser::parseCtcpReplyUserinfo(KviCtcpMessage * msg)
{
	QString szRemoteFile;
	QString szGender;
	QString decoded = msg->msg->console()->decodeText(msg->pData);

	bool bNeedToUpdateUserlist = false;
	KviIrcUserEntry * e = msg->msg->connection()->userDataBase()->find(msg->pSource->nick());
	if(e)
	{
		int pos = decoded.indexOf("Gender=", 0, Qt::CaseInsensitive);

		if(pos >= 0)
		{
			QChar c = decoded[pos + 7];
			switch(c.unicode())
			{
				case 'F':
				case 'f':
					bNeedToUpdateUserlist = true;
					e->setGender(KviIrcUserEntry::Female);
					break;
				case 'M':
				case 'm':
					bNeedToUpdateUserlist = true;
					e->setGender(KviIrcUserEntry::Male);
					break;
			}
		}
	}

	if(bNeedToUpdateUserlist)
	{
		if(KviQString::equalCS(g_pActiveWindow->metaObject()->className(), QString("KviChannelWindow")))
		{
			((KviChannelWindow *)g_pActiveWindow)->userListView()->updateArea();
		}
	}

	echoCtcpReply(msg);
}

void KviIrcServerParser::parseCtcpReplyGeneric(KviCtcpMessage * msg)
{
	echoCtcpReply(msg);
}

void KviIrcServerParser::parseCtcpReplyLagcheck(KviCtcpMessage * msg)
{
	// this is an internal CTCP used for checking lag
	KviCString szTag;
	msg->pData = extractCtcpParameter(msg->pData, szTag, true);
	if(msg->msg->console()->connection()->lagMeter())
		msg->msg->console()->connection()->lagMeter()->lagCheckComplete(szTag.ptr());
}

//ERRORMSG,ECHO,ERRMSG
//SED,DCC,SOUND/MULTIMEDIA/MM,SCRIPT
