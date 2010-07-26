//=============================================================================
//
//   File : kvi_sp_ctcp.cpp
//   Creation date : Thu Aug 16 2000 13:34:42 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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


// FIXME: #warning "CTCP BEEP == WAKEUP == AWAKE"
// FIXME: #warning "CTCP AVATARREQ or QUERYAVATAR"

#include "kvi_mirccntrl.h"
#include "kvi_osinfo.h"
#include "kvi_app.h"
#include "kvi_sparser.h"
#include "kvi_window.h"
#include "kvi_out.h"
#include "kvi_locale.h"
#include "kvi_ircsocket.h"
#include "kvi_channel.h"
#include "kvi_defaults.h"
#include "kvi_channel.h"
#include "kvi_query.h"
#include "kvi_ircuserdb.h"
#include "kvi_iconmanager.h"
#include "kvi_modulemanager.h"
#include "kvi_sharedfiles.h"
#include "kvi_time.h"
#include "kvi_fileutils.h"
#include "kvi_ctcppagedialog.h"
#include "kvi_useraction.h"
#include "kvi_options.h"
#include "kvi_ircconnection.h"
#include "kvi_ircconnectionuserinfo.h"
#include "kvi_ircconnectionantictcpflooddata.h"
#include "kvi_lagmeter.h"
#include "kvi_kvs_eventtriggers.h"
#include "kvi_kvs_script.h"
#include "kvi_sourcesdate.h"
#include "kvi_regusersdb.h"
#include "kvi_buildinfo.h"

#include <stdlib.h>

#include <QDateTime>
#include <QTextDocument>

extern KVIRC_API KviSharedFilesManager * g_pSharedFilesManager;
extern KVIRC_API KviCtcpPageDialog * g_pCtcpPageDialog;

/*
	@doc: ctcp_handling
	@title:
		KVIrc and CTCP
	@short:
		For developers: Client-To-Client Protocol handling in KVIrc
	@body:
		[big]Introduction[/big][br]
		Personally, I think that the CTCP specification is to
		be symbolically printed & burned. It is really too complex
		(you can go mad with the quoting specifications)
		and NO IRC CLIENT supports it completely.
		Here is my personal point of view on the CTCP protocol.[br]
		[big]What is CTCP?[/big][br]
		CTCP stands for Client-to-Client Protocol. It is designed
		for exchanging almost arbitrary data between IRC clients;
		the data is embedded into text messages of the underlying
		IRC protocol.[br]
		[big]Basic concepts[/big][br]
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
		CTCP message; The origial protocol allowed more than one CTCP
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
		should be allowed and theoretically the trailing <0x01> delimiter can be optional.[br]
		[big]How to extract the CTCP message[/big][br]
		The IRC messages do not allow the following characters to be sent:[br]
		<NUL> (Ascii character 0), <CR> (Carriage return), <LF> (Line feed).[br]
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
		and thus from now on we will deal only with the <ctcp message> part.[br]

		[big]Parsing a CTCP message: The quoting dilemma[/big][br]
		Since there are characters that cannot appear in a <ctcp message>,
		theoretically we should have to use a quoting mechanism.
		Well, in fact, no actual CTCP message uses the quoting: there
		is no need to include a <NUL>, a <CR> or <LF> inside the actually
		defined messages (The only one could be CTCP SED, but I have never
		seen it in action... is there any client that implements it?).
		We could also leave the "quoting" to the "single message type semantic":
		a message that needs to include "any character" could have its own
		encoding method (Base64 for example). With the "one CTCP per IRC message"
		convention we could even allow <0x01> inside messages. Only the leading
		(and eventually trailing) <0x01> would be the delimiter, the other ones
		would be valid characters. Finally, is there any CTCP type that needs
		<0x01> inside a message? <0x01> is not printable (as well as <CR>,<LF> and <NUL>),
		so only encoded messages (and again we can stick to the single message semantic)
		messages or the ones including special parameters. Some machines might
		allow <0x01> in filenames....well, a file with <0x01> in its name has something
		broken inside, or the creator is a sort of "hacker" (so he also
		knows how to rename a file...) :).[br]
		Anyway, let's be pedantic, and define this quoting method.
		Let's use the most intuitive method, adopted all around the world:[br]
		The backslash character ('\') as escape.[br]
		An escape sequence is formed by the backslash character and a number
		of following ascii characters. We define the following two types of escape sequences:[br]
		[b]'\XXX'[/b] (where XXX is an [b]octal number[/b] formed by three digits)
		that indicates the ascii character with code that corresponds to the number.[br]
		[b]'\C'[/b] (where C is a [b]CTCP valid ascii non digit character[/b]) that corresponds
		literally to the character C discarding any other semantic that might be associated
		with it (This will become clear later).
		I've chosen the octal rappresentation just to follow a bit the old specification:
		the authors seemed to like it. This point could be discussed in
		some mailing list or sth. The '\C' sequence is useful to include the backslash
		character (escape sequence '\\').[br]

		[big]Let's mess a little more[/big][br]
		A CTCP message is made of [b]space separated parameters[/b].[br]
		The natural way of separating parameters is to use the space character.
		We define a "token" as a sequence of valid CTCP characters not including literal space.
		A <ctcp parameter> is usally a token, but not always;
		filenames can contain spaces inside names (and it happens very often!).
		So one of the parameters of CTCP DCC is not a space separated token.
		How do we handle it? Again a standard is missing. Some clients simply change
		the filename placing underscores instead of spaces, this is a reasonable solution if used with care.
		Other clients attempt to "isolate" the filename token by surrounding it with some kind
		of quotes, usually the '"' or ''' characters. This is also a good solution.
		Another one that naturally comes into my mind is to use the previously defined
		quoting to define a "non-breaking space" character, because a space after a backslash
		could lose its original semantic. Better yet, use the backslash followed by
		the octal rappresentation of the space character ('\040').
		Anyway, to maintain compatibility with other popular IRC clients (such as mIRC),
		let's include the '"' quotes in our standard: literal (unescaped) '"' quotes
		define a single token string. To include a literal '"' character, escape it.
		Additionally, the last parameter of a <ctcp message> may be made of multiple tokens.

		[big]A CTCP parameter extracting example[/big][br]
		A trivial example of a C "CTCP parameter extracting routine" follows.[br]
		An IRC message is made of up to 510 useable characters.
		When a CTCP is sent there is a PRIVMSG or NOTICE token that uses at least 6 characters,
		at least two spaces and a target token (that can not be empty, so it is at least one character)
		and finally one <0x01> escape character. This gives 500 characters as maximum size
		for a complete <ctcp message> and thus for a <ctcp token>.
		In fact, the <ctcp message> is always smaller than 500 characters; there are usually two
		<0x01> chars, there is a message source part at the beginning of the IRC message
		that is 10-15 characters long, and there is a ':' character before the trailing parameter.
		Anyway, to really be on the "safe side", we use a 512 character buffer for each
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
		[/example][br]

		[big]CTCP parameter semantics[/big][br]
		The first <ctcp parameter> of a <ctcp message> is the <ctcp tag>: it defines
		the semantic of the rest of the message.[br]
		Altough it is a convention to specify the <ctcp tag> as uppercase letters,
		and the original specification says that the whole <ctcp message> is
		case sensitive, I'd prefer to follow the IRC message semantic (just to
		have less "special cases") and treat the whole mssage as [b]case insensitive[/b].[br]
		The remaining tokens depend on the <ctcp tag>. A description of known <ctcp tags>
		and thus <ctcp messages> follows.[br]

		[big]PING[/big][br]
		[b]Syntax: <0x01>PING <data><0x01>[/b][br]
		The PING request is used to check the round trip time from one client to another.
		The receiving client should reply with exactly the same message but sent
		through a NOTICE instead of a PRIVMSG. The <data> usually contains an unsigned
		integer but not necessairly; it is not even mandatory for <data> to be a single token.
		The receiver should ignore the semantic of <data>.[br]
		The reply is intended to be processed by IRC clients.

		[big]VERSION[/big][br]
		[b]Syntax: <0x01>VERSION<0x01>[/b][br]
		The VERSION request asks for information about another user's IRC client program.
		The reply should be sent thru a NOTICE with the following syntax:[br]
		<0x01>VERSION <client_version_data><0x01>[br]
		The preferred form for <client_version_data> is
		"<client_name>:<client_version>:<client_enviroinement>", but historically
		clients (and users) send a generic reply describing the client name, version
		and eventually the used script name. This CTCP reply is intended to be human
		readable, so any form is accepted.

		[big]USERINFO[/big][br]
		[b]Syntax: <0x01>USERINFO<0x01>[/b][br]
		The USERINFO request asks for information about another user.
		The reply should be sent thru a NOTICE with the following syntax:[br]
		<0x01>USERINFO <user_info_data><0x01>[br]
		The <user_info_data> should be a human readable "user defined" string;

		[big]CLIENTINFO[/big][br]
		[b]Syntax: <0x01>CLIENTINFO<0x01>[/b][br]
		The CLIENTINFO request asks for information about another user's IRC client program.
		While VERSION requests the client program name and version, CLIENTINFO requests
		information about CTCP capabilities.[br]
		The reply should be sent thru a NOTICE with the following syntax:[br]
		<0x01>CLIENTINFO <client_info_data><0x01>[br]
		The <client_info_data> should contain a list of supported CTCP request tags.
		The CLIENTINFO reply is intended to be human readable.

		[big]FINGER[/big][br]
		[b]Syntax: <0x01>FINGER<0x01>[/b][br]
		The FINGER request asks for information about another IRC user.
		The reply should be sent thru a NOTICE with the following syntax:[br]
		<0x01>FINGER <user_info_data><0x01>[br]
		The <user_info_data> should be a human readable string containing
		the system username and possibly the system idle time;

		[big]SOURCE[/big][br]
		[b]Syntax: <0x01>SOURCE<0x01>[/b][br]
		The SOURCE request asks for the client homepage or ftp site information.
		The reply should be sent thru a NOTICE with the following syntax:[br]
		<0x01>VERSION <homepage_url_data><0x01>[br]
		This CTCP reply is intended to be human readable, so any form is accepted.

		[big]TIME[/big][br]
		[b]Syntax: <0x01>TIME<0x01>[/b][br]
		The TIME request asks for the user local time.
		The reply should be sent thru a NOTICE with the following syntax:[br]
		<0x01>TIME <time and date string><0x01>[br]
		This CTCP reply is intended to be human readable, so any form is accepted.

		[big]ACTION[/big][br]
		[b]Syntax: <0x01>ACTION<0x01>[/b][br]
		The ACTION tag is used to describe an action.[br]
		It should be sent through a NOTICE message and never generate a reply.[br]

		[big]AVATAR (equivalent to ICON or FACE)[/big][br]
		[b]Syntax: <0x01>AVATAR<0x01>[/b][br]
		The AVATAR tag is used to query an user's avatar.[br]

		[big]MULTIMEDIA (equivalent to MM or SOUND)[/big][br]
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
		non-standard private message syntax. This convention should be dropped.[br]

		[big]DCC[/big][br]
		[b]Syntax: <0x01>DCC <type> <type dependant parameters><0x01>[/b][br]
		The DCC tag is used to initiate a Direct Client Connection.
		The known dcc types are:[br]
		CHAT[br]
		SEND[br]
		TSEND[br]
		GET[br]
		TGET[br]
		ACCEPT[br]
		RESUME[br]

*/



void KviServerParser::encodeCtcpParameter(const char * param,KviStr &buffer,bool bSpaceBreaks)
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
					if(param != begin)buffer.append(begin,param - begin);
					buffer.append("\\040");
					param++;
					begin = param;
				} else {
					// space is non breaking (last parameter)
					param++;
				}
			break;
			case '\r':
				if(param != begin)buffer.append(begin,param - begin);
				buffer.append("\\015");
				param++;
				begin = param;
			break;
			case '\n':
				if(param != begin)buffer.append(begin,param - begin);
				buffer.append("\\012");
				param++;
				begin = param;
			break;
			case '"':
				if(param != begin)buffer.append(begin,param - begin);
				buffer.append("\\042");
				param++;
				begin = param;
			break;
			case '\\':
				if(param != begin)buffer.append(begin,param - begin);
				buffer.append("\\143");
				param++;
				begin = param;
			break;
			case 0x01:
				if(param != begin)buffer.append(begin,param - begin);
				buffer.append("\\001");
				param++;
				begin = param;
			break;
			default:
				param++;
			break;
		}
	}

	if(param != begin)buffer.append(begin,param - begin);
}

void KviServerParser::encodeCtcpParameter(const char * parametr,QString &resultBuffer,bool bSpaceBreaks)
{
	//
	// This one encodes a single ctcp parameter with the simplest
	// subset of rules and places it in the supplied buffer
	//
	QByteArray buffer;
	register const char* param=parametr;
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
				} else {
					buffer+=*param;
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
				buffer+=*param;
				param++;
			break;
		}
	}
	resultBuffer=buffer;
}



const char * KviServerParser::decodeCtcpEscape(const char * msg_ptr,KviStr &buffer)
{
	//
	// This one decodes an octal sequence
	// and returns the pointer "just after it".
	// It should be called when *msg_ptr points
	// just after a backslash.
	// The decoded escape is appended to the buffer
	//
	// We're also assuming that *msg_ptr is not null here
	//
	char c;
	if((*msg_ptr >= '0') && (*msg_ptr < '8'))
	{
		// a digit follows the backslash */
		c = *msg_ptr - '0';
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
		} // else it is broken, but let's be flexible
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

	//
	// null escape: just append the following
	// character (thus discarding its semantics)
	//

	buffer.append(msg_ptr);
	return ++msg_ptr;
}


const char * KviServerParser::decodeCtcpEscape(const char * msg_ptr,QByteArray &buffer)
{
	//
	// This one decodes an octal sequence
	// and returns the pointer "just after it".
	// It should be called when *msg_ptr points
	// just after a backslash.
	// The decoded escape is appended to the buffer
	//
	// We're also assuming that *msg_ptr is not null here
	//
	char c;
	if((*msg_ptr >= '0') && (*msg_ptr < '8'))
	{
		// a digit follows the backslash */
		c = *msg_ptr - '0';
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
		} // else it is broken, but let's be flexible
		buffer+= (c ? c : ' ') ;
		return msg_ptr;
	}

	if(*msg_ptr == 'r')
	{
		buffer+='\r';
		return ++msg_ptr;
	}
	if(*msg_ptr == 'n')
	{
		buffer+='\n';
		return ++msg_ptr;
	}
	//
	// null escape: just append the following
	// character (thus discarding its semantics)
	//

	buffer+=*msg_ptr;
	return ++msg_ptr;
}


const char * KviServerParser::extractCtcpParameter(const char * msg_ptr,KviStr &buffer,bool bSpaceBreaks, bool bSafeOnly)
{
	//
	// This one extracts the "next" ctcp parameter in msg_ptr
	// and puts it in the supplied buffer.
	// It is assumed that the leading and trailing CTCP
	// tags have been already removed.
	// Skips the leading and trailing spaces.
	// bSpaceBreaks should be set to false if (and only if) the
	// extracted parameter is the last in a positional parameter
	// based CTCP message.
	//

	int bInString = 0;
	if(!msg_ptr) return 0;
	while(*msg_ptr == ' ')msg_ptr++; // skip leading spaces

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
				if(bSafeOnly)msg_ptr++;
				else {
					if(msg_ptr != begin)buffer.append(begin,msg_ptr - begin);
					msg_ptr++;
					if(*msg_ptr)
					{
						// decode the escape
						msg_ptr = decodeCtcpEscape(msg_ptr,buffer);
						begin = msg_ptr;
					}
					// else it is a senseless trailing backslash.
					// Just ignore and let the function
					// return spontaneously.
				}
			break;
			case ' ':
				// space : separate tokens if not in string
				if(bInString || (!bSpaceBreaks))msg_ptr++;
				else {
					// Not in string and space breaks: end of token
					// skip trailing white space (this could be avoided)
					// and return
					if(msg_ptr != begin)buffer.append(begin,msg_ptr - begin);
					while(*msg_ptr == ' ')msg_ptr++;
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
					if(msg_ptr != begin)buffer.append(begin,msg_ptr - begin);
					bInString = 0;
					msg_ptr++;
					begin = msg_ptr;
				} else {
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
	if(msg_ptr != begin)buffer.append(begin,msg_ptr - begin);
	return msg_ptr;
}

const char * KviServerParser::extractCtcpParameter(const char * p_msg_ptr,QString &resultBuffer,bool bSpaceBreaks, bool bSafeOnly)
{
	//
	// This one extracts the "next" ctcp parameter in p_msg_ptr
	// and puts it in the supplied buffer.
	// It is assumed that the leading and trailing CTCP
	// tags have been already removed.
	// Skips the leading and trailing spaces.
	// bSpaceBreaks should be set to false if (and only if) the
	// extracted parameter is the last in a positional parameter
	// based CTCP message.
	//

	QByteArray buffer;
	register const char* msg_ptr=p_msg_ptr;
	int bInString = 0;
	if(!msg_ptr) return 0;
	while(*msg_ptr == ' ')msg_ptr++; // skip leading spaces

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
				if(bSafeOnly)msg_ptr++;
				else {
					msg_ptr++;
					if(*msg_ptr)
					{
						// decode the escape
						msg_ptr = decodeCtcpEscape(msg_ptr,buffer);
					}
					// else it is a senseless trailing backslash.
					// Just ignore and let the function
					// return spontaneously.
				}
			break;
			case ' ':
				// space : separate tokens if not in string
				if(bInString || (!bSpaceBreaks)) {
					buffer+=*msg_ptr;
					msg_ptr++;
				}
				else {
					// Not in string and space breaks: end of token
					// skip trailing white space (this could be avoided)
					// and return
					while(*msg_ptr == ' ')msg_ptr++;
					resultBuffer=buffer;
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
					buffer+=*msg_ptr;
					bInString = 0;
					msg_ptr++;
				} else {
					// we don't begin a string here
					// since we're in the middle of the token
					// it is assumed to be simply a non encoded "
					buffer+=*msg_ptr;
					msg_ptr++;
				}
			break;
			default:
				// any other char
				buffer+=*msg_ptr;
				msg_ptr++;
			break;
		}
	}
	resultBuffer=buffer;
	return msg_ptr;
}


void KviServerParser::parseCtcpRequest(KviCtcpMessage *msg)
{
	msg->pData = extractCtcpParameter(msg->pData,msg->szTag);

	bool bDCCRequest = msg->szTag == "DCC" || msg->szTag == "XDCC" || msg->szTag == "TDCC";

	bool bAction = (msg->szTag == "ACTION");

	if (bDCCRequest)
	{
		if (KVI_OPTION_BOOL(KviOption_boolIgnoreCtcpDcc))
		{
			msg->msg->console()->output(KVI_OUT_IGNORE,__tr2qs("Ignoring DCC from \r!nc\r%s\r [%s@\r!h\r%s\r]"),
				KviQString::toUtf8(msg->pSource->nick()).data(),
				KviQString::toUtf8(msg->pSource->user()).data(),
				KviQString::toUtf8(msg->pSource->host()).data()
				);
			return;
		}
	} else {
		KviRegisteredUser * u = msg->msg->connection()->userDataBase()->registeredUser(msg->pSource->nick(),msg->pSource->user(),msg->pSource->host());
		//Ignore it?
		if(u)
		{
			if( (!bAction && u->isIgnoreEnabledFor(KviRegisteredUser::Ctcp)) ||
				(bAction && u->isIgnoreEnabledFor(IS_ME(msg->msg,msg->szTarget) ? KviRegisteredUser::Query : KviRegisteredUser::Channel)) )
			{
				if (KVI_OPTION_BOOL(KviOption_boolVerboseIgnore))
				{
					msg->msg->console()->output(KVI_OUT_IGNORE,__tr2qs("Ignoring CTCP from \r!nc\r%s\r [%s@\r!h\r%s\r]"),
						KviQString::toUtf8(msg->pSource->nick()).data(),
						KviQString::toUtf8(msg->pSource->user()).data(),
						KviQString::toUtf8(msg->pSource->host()).data()
						);
				}
				return;
			}
		}
	}

	for(int i=0;m_ctcpRequestParseProcTable[i].msgName;i++)
	{
		if(KviQString::equalCI(msg->szTag,m_ctcpRequestParseProcTable[i].msgName))
		{
			if(!(m_ctcpReplyParseProcTable[i].iFlags & KVI_CTCP_MESSAGE_PARSE_TRIGGERNOEVENT))
			{
				QString szData = msg->msg->connection()->decodeText(msg->pData);
				if(KVS_TRIGGER_EVENT_6_HALTED(KviEvent_OnCTCPRequest, \
					msg->msg->console(), \
					msg->pSource->nick(), \
					msg->pSource->user(), \
					msg->pSource->host(), \
					msg->szTarget, \
					msg->szTag, \
					szData))return;
			}
			(this->*(m_ctcpRequestParseProcTable[i].proc))(msg);
			return;
		}
	}

	QString szData = msg->msg->connection()->decodeText(msg->pData);
	// trigger the event on unrecognized requests too
	if(KVS_TRIGGER_EVENT_6_HALTED(KviEvent_OnCTCPRequest, \
		msg->msg->console(), \
		msg->pSource->nick(), \
		msg->pSource->user(), \
		msg->pSource->host(), \
		msg->szTarget, \
		msg->szTag,
		szData))
			return;

	// unknown
	msg->bUnknown = true;
	echoCtcpRequest(msg);
}

void KviServerParser::parseCtcpReply(KviCtcpMessage *msg)
{
	msg->pData = extractCtcpParameter(msg->pData,msg->szTag);

	for(int i=0;m_ctcpReplyParseProcTable[i].msgName;i++)
	{
		if(KviQString::equalCI(msg->szTag,m_ctcpReplyParseProcTable[i].msgName))
		{
			if(!(m_ctcpReplyParseProcTable[i].iFlags & KVI_CTCP_MESSAGE_PARSE_TRIGGERNOEVENT))
			{
				QString szData = msg->msg->connection()->decodeText(msg->pData);
				if(KVS_TRIGGER_EVENT_6_HALTED(KviEvent_OnCTCPReply, \
					msg->msg->console(),msg->pSource->nick(),msg->pSource->user(), \
					msg->pSource->host(),msg->szTarget,msg->szTag,szData))return;
			}
			(this->*(m_ctcpReplyParseProcTable[i].proc))(msg);
			return;
		}
	}

	QString szData = msg->msg->connection()->decodeText(msg->pData);
	// trigger the event on unrecognized replies too
	if(KVS_TRIGGER_EVENT_6_HALTED(KviEvent_OnCTCPReply, \
		msg->msg->console(),msg->pSource->nick(),msg->pSource->user(), \
		msg->pSource->host(),msg->szTarget,msg->szTag,szData))return;

	// unknown
	msg->bUnknown = true;
	echoCtcpReply(msg);
}


// Ctcp message handlers

bool KviServerParser::checkCtcpFlood(KviCtcpMessage *msg)
{
	if(!KVI_OPTION_BOOL(KviOption_boolUseCtcpFloodProtection))return false;

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
	} else {
		d->setLastCtcpTime(tNow);
		d->setCtcpCount(1);
	}
	return false;
}


void KviServerParser::replyCtcp(KviCtcpMessage *msg,const QString &data)
{
	QByteArray szNick = msg->msg->connection()->encodeText(msg->pSource->nick());
	msg->msg->connection()->sendFmtData(
			"NOTICE %s :%c%s %s%c",
			szNick.data(),
			0x01,
			msg->msg->connection()->encodeText(msg->szTag).data(),
			msg->msg->connection()->encodeText(data).data(),
			0x01
		);
}

void KviServerParser::echoCtcpReply(KviCtcpMessage * msg)
{
	if(!msg->msg->haltOutput())
	{
		KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolCtcpRepliesToActiveWindow) ? msg->msg->console()->activeWindow() : msg->msg->console();

		bool bIsChannel = false;

		if(!IS_ME(msg->msg,msg->szTarget))
		{
			// Channel ctcp request!
			pOut = msg->msg->connection()->findChannel(msg->szTarget);
			if(!pOut)
			{
				pOut = msg->msg->console();
				pOut->output(KVI_OUT_SYSTEMWARNING,
					__tr2qs("The following CTCP reply has unrecognized target %Q"),&(msg->szTarget));
			} else bIsChannel = true;
		}

		QString szData = msg->msg->connection()->decodeText(msg->pData);

		QString szWhat = bIsChannel ? __tr2qs("Channel CTCP") : QString("CTCP");

		pOut->output(
			msg->bUnknown ? KVI_OUT_CTCPREPLYUNKNOWN : KVI_OUT_CTCPREPLY,
			__tr2qs("%Q %Q reply from \r!n\r%Q\r [%Q@\r!h\r%Q\r]: %Q"),
			&szWhat,&(msg->szTag),&(msg->pSource->nick()),&(msg->pSource->user()),
			&(msg->pSource->host()),&szData);
	}
}


void KviServerParser::echoCtcpRequest(KviCtcpMessage *msg)
{
	// FIXME: #warning "DEDICATED CTCP WINDOW...MINIMIZED ?"
	if(!msg->msg->haltOutput())
	{
		QString req = msg->szTag;
		if(*(msg->pData)) {
			req.append(" ");
			req.append(msg->pData);
		}

		KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolCtcpRequestsToActiveWindow) ? msg->msg->console()->activeWindow() : msg->msg->console();

		bool bIsChannel = false;

		if(!IS_ME(msg->msg,msg->szTarget))
		{
			// Channel ctcp request!
			pOut = msg->msg->connection()->findChannel(msg->szTarget);
			if(!pOut)
			{
				pOut = msg->msg->console();
				pOut->output(KVI_OUT_SYSTEMWARNING,
					__tr2qs("The following CTCP request has unrecognized target %Q"),
					&(msg->szTarget));
			} else bIsChannel = true;
		}

		QString szRequest = req;
		QString szTag = msg->szTag;
		QString szWhat = bIsChannel ? __tr2qs("Channel CTCP") : QString("CTCP");

		if(msg->bIsFlood)
		{
			QString szData = msg->msg->connection()->decodeText(msg->pData);
			if(!KVS_TRIGGER_EVENT_6_HALTED(KviEvent_OnCTCPFlood,pOut,msg->pSource->nick(),msg->pSource->user(),msg->pSource->host(),msg->szTarget,msg->szTag,szData))
				pOut->output(KVI_OUT_CTCPREQUESTFLOOD,
					__tr2qs("%Q %Q%c request from \r!n\r%Q\r [%Q@\r!h\r%Q\r] (%Q), ignored (flood limit exceeded)"),
					&szWhat,&szTag,KVI_TEXT_RESET,&(msg->pSource->nick()),
					&(msg->pSource->user()),&(msg->pSource->host()),&szRequest);
		} else {
			QString szAction = msg->bUnknown ? __tr2qs("ignored (unrecognized)") :
									(msg->bIgnored ? __tr2qs("ignored") : __tr2qs("replied"));
			pOut->output(
				msg->bUnknown ? KVI_OUT_CTCPREQUESTUNKNOWN :
					(msg->bIgnored ? KVI_OUT_CTCPREQUESTIGNORED : KVI_OUT_CTCPREQUESTREPLIED),
				__tr2qs("%Q %Q%c request from \r!n\r%Q\r [%Q@\r!h\r%Q\r] (%Q), %Q"),
				&szWhat,&szTag,KVI_TEXT_RESET,&(msg->pSource->nick()),
				&(msg->pSource->user()),&(msg->pSource->host()),&szRequest,&szAction);
		}
	}
}



void KviServerParser::parseCtcpRequestPing(KviCtcpMessage *msg)
{
	if(!checkCtcpFlood(msg))
	{
		if(!KVI_OPTION_BOOL(KviOption_boolIgnoreCtcpPing))
		{
			replyCtcp(msg,msg->msg->connection()->encodeText(msg->pData));
		} else msg->bIgnored = true;
	}

	echoCtcpRequest(msg);
}

void KviServerParser::parseCtcpReplyPing(KviCtcpMessage * msg)
{
	if(!msg->msg->haltOutput())
	{
		KviWindow * pOut = KVI_OPTION_BOOL(KviOption_boolCtcpRepliesToActiveWindow) ? msg->msg->console()->activeWindow() : msg->msg->console();

		bool bIsChannel = false;

		if(!IS_ME(msg->msg,msg->szTarget))
		{
			// Channel ctcp request!
			pOut = msg->msg->connection()->findChannel(msg->szTarget);
			if(!pOut)
			{
				pOut = msg->msg->console();
				pOut->output(KVI_OUT_SYSTEMWARNING,
					__tr2qs("The following CTCP PING reply has unrecognized target \"%Q\""),
					&(msg->szTarget));
			} else bIsChannel = true;
		}

		unsigned int uSecs;
		unsigned int uMSecs = 0;

		KviStr szTime;

		struct timeval tv;
		kvi_gettimeofday(&tv,0);

		msg->pData = extractCtcpParameter(msg->pData,szTime,true);

		bool bOk;

		if(szTime.contains('.'))
		{
			KviStr szUSecs = szTime;
			szUSecs.cutToFirst('.');
			szTime.cutFromFirst('.');

			uMSecs = szUSecs.toUInt(&bOk);
			if(!bOk)
			{
				uMSecs = 0;
				tv.tv_usec = 0;
			}
		} else tv.tv_usec = 0;


		uSecs = szTime.toUInt(&bOk);
		if(!bOk)pOut->output(KVI_OUT_SYSTEMWARNING,
				__tr2qs("The following CTCP PING reply has a broken time identifier \"%S\", don't trust the displayed time"),&szTime);

		unsigned int uDiffSecs = tv.tv_sec - uSecs;

		//unsigned int uDiffMSecs = ((((unsigned int)tv.tv_usec) - uMSecs) / 1000) % 1000;
		//if ((uMSecs  / 1000000) <= uDiffSecs) uDiffSecs -= (uMSecs  / 1000000)

		while(uMSecs > 1000000)uMSecs /= 10; // precision too high?
		if(((unsigned int)tv.tv_usec) < uMSecs)
		{
			tv.tv_usec += 1000000;
			if(uDiffSecs > 0)uDiffSecs --;
		}
		unsigned int uDiffMSecs = (tv.tv_usec - uMSecs) / 1000;

		QString szWhat = bIsChannel ? __tr2qs("Channel CTCP") : QString("CTCP");

		pOut->output(
			msg->bUnknown ? KVI_OUT_CTCPREPLYUNKNOWN : KVI_OUT_CTCPREPLY,
			__tr2qs("%Q PING reply from \r!n\r%Q\r [%Q@\r!h\r%Q\r]: %u sec %u msec"),
			&szWhat,&(msg->pSource->nick()),
			&(msg->pSource->user()),&(msg->pSource->host()),uDiffSecs,uDiffMSecs);
	}
}


void KviServerParser::parseCtcpRequestVersion(KviCtcpMessage *msg)
{
	if(!checkCtcpFlood(msg))
	{
		if(!KVI_OPTION_BOOL(KviOption_boolIgnoreCtcpVersion))
		{
			QString szVersion;


			szVersion = "KVIrc " KVI_VERSION;
			if(KviBuildInfo::buildRevision()!=QString())
			{
				szVersion+= " svn-";
				szVersion+=KviBuildInfo::buildRevision();
			}
			szVersion += " '" KVI_RELEASE_NAME "' " KVI_SOURCES_DATE " - build ";
			szVersion += KviBuildInfo::buildDate();
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
			szVersion.append(QString(" - %1").arg(KviOsInfo::version()));
#else
			szVersion.append(QString(" - %1 (%2)").arg(KviOsInfo::name(), KviOsInfo::release()));
#endif
			//szVersion.append(QString(" - QT Version: %1 - %2").arg(qVersion(), __tr2qs("http://www.kvirc.net/")));
			if(!KVI_OPTION_STRING(KviOption_stringCtcpVersionPostfix).isEmpty())
			{
				QString sz = KVI_OPTION_STRING(KviOption_stringCtcpVersionPostfix);
				if(!sz.isEmpty())
				{
					szVersion.append(" :");
					szVersion.append(sz);
				}
			}
			replyCtcp(msg,szVersion);
		} else msg->bIgnored = true;
	}

	echoCtcpRequest(msg);
}

void KviServerParser::parseCtcpRequestUserinfo(KviCtcpMessage *msg)
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
				if(!szReply.isEmpty())szReply += "; ";
				szReply += "Gender=";
				szReply += KVI_OPTION_STRING(KviOption_stringCtcpUserInfoGender);
			}
			if(!KVI_OPTION_STRING(KviOption_stringCtcpUserInfoLocation).isEmpty())
			{
				if(!szReply.isEmpty())szReply += "; ";
				szReply += "Location=";
				szReply += KVI_OPTION_STRING(KviOption_stringCtcpUserInfoLocation);
			}
			if(!KVI_OPTION_STRING(KviOption_stringCtcpUserInfoLanguages).isEmpty())
			{
				if(!szReply.isEmpty())szReply += "; ";
				szReply += "Languages=";
				szReply += KVI_OPTION_STRING(KviOption_stringCtcpUserInfoLanguages);
			}
			if(!KVI_OPTION_STRING(KviOption_stringCtcpUserInfoOther).isEmpty())
			{
				if(!szReply.isEmpty())szReply += "; ";
				szReply += KVI_OPTION_STRING(KviOption_stringCtcpUserInfoOther);
			}
			if(szReply.isEmpty())szReply = KVI_DEFAULT_CTCP_USERINFO_REPLY;
			replyCtcp(msg,szReply);
		} else msg->bIgnored = true;
	}

	echoCtcpRequest(msg);
}

// FIXME: CTCP SEX , AGE , LOCATION!!! <--- so we will be safe :)
// FIXME: KEEP THIS TABLE UP TO DATE

static const char * ctcpTagTable[][2]=
{
	{ "PING"        , "Returns given parameters without parsing them"               },
	{ "VERSION"     , "Returns the version of this client"                          },
	{ "CLIENTINFO"  , "With no parameters, lists supported CTCP tags," \
                          " 'CLIENTINFO <tag>' describes <tag>"                     },
	{ "USERINFO"    , "Returns personal information about the current user"         },
	{ "FINGER"      , "Returns information about the current user"                  },
	{ "SOURCE"      , "Returns the client homepage URL"                             },
	{ "TIME"        , "Returns the current local time"                              },
	{ "ACTION"      , "Used to describe actions, generates no reply"                },
	{ "AVATAR"      , "Returns the current avatar (may trigger a DCC GET) or" \
                          " sets your own on this side if sent through a NOTICE"    },
	{ "DCC"         , "Initiates a DCC connection (XDCC,TDCC)"                      },
	{ "PAGE"        , "Leaves a message for this user"                              },
	{ 0             , 0                                                             }
};

void KviServerParser::parseCtcpRequestClientinfo(KviCtcpMessage *msg)
{
	// this is completely latin1
	if(!checkCtcpFlood(msg))
	{
		if(!KVI_OPTION_BOOL(KviOption_boolIgnoreCtcpClientinfo))
		{
			KviStr szTag;
			msg->pData = extractCtcpParameter(msg->pData,szTag,false);
			szTag.trimmed();
			szTag.toUpper();
			if(szTag.isEmpty())
			{
				QString reply("KVIrc " KVI_VERSION " '" KVI_RELEASE_NAME "' " KVI_SOURCES_DATE " - http://www.kvirc.net - Supported tags: ");
				for(int i=0;ctcpTagTable[i][0];i++)
				{
					reply += ctcpTagTable[i][0];
					if(ctcpTagTable[i + 1][0])reply += ",";
				}
				reply += " - Use 'CLIENTINFO <tag>' for a description of each tag";
				replyCtcp(msg,reply);
			} else {
				bool bFound = false;
				for(int i=0;ctcpTagTable[i][0] && !bFound;i++)
				{
					if(kvi_strEqualCS(ctcpTagTable[i][0],szTag.ptr()))
					{
						KviStr reply(KviStr::Format,"%s: %s",ctcpTagTable[i][0],ctcpTagTable[i][1]);
						replyCtcp(msg,reply.ptr());
						bFound = true;
					}
				}
				if(!bFound)
				{
					msg->szTag= "ERRMSG";
					KviStr reply(KviStr::Format,"Unsupported tag %s",szTag.ptr());
					replyCtcp(msg,reply.ptr());
				}
			}
		} else msg->bIgnored = true;
	}

	echoCtcpRequest(msg);
}

void KviServerParser::parseCtcpRequestFinger(KviCtcpMessage *msg)
{
	// completely latin1 atm
	if(!checkCtcpFlood(msg))
	{
		if(!KVI_OPTION_BOOL(KviOption_boolIgnoreCtcpFinger))
		{
			KviStr username = getenv("USER");
			if(username.isEmpty())username = getenv("LOGNAME");
			if(username.isEmpty())username = msg->msg->connection()->userInfo()->userName();
			// FIXME: #warning "UTSNAME ?...AND OTHER INFO ?...SYSTEM IDLE TIME ?...KVIRC IDLE TIME ?"
			KviStr reply(KviStr::Format,"%s",username.ptr());
			replyCtcp(msg,reply.ptr());
		} else msg->bIgnored = true;
	}

	echoCtcpRequest(msg);
}

void KviServerParser::parseCtcpRequestSource(KviCtcpMessage *msg)
{
	if(!checkCtcpFlood(msg))
	{
		if(!KVI_OPTION_BOOL(KviOption_boolIgnoreCtcpSource))
		{
			QString version = "KVIrc " KVI_VERSION " '" KVI_RELEASE_NAME "' - http://www.kvirc.net/";
			if(!KVI_OPTION_STRING(KviOption_stringCtcpSourcePostfix).isEmpty())
			{
				version+=" :";
				version+= KVI_OPTION_STRING(KviOption_stringCtcpSourcePostfix);
			}
			replyCtcp(msg,version);
		} else msg->bIgnored = true;
	}

	echoCtcpRequest(msg);
}

void KviServerParser::parseCtcpRequestTime(KviCtcpMessage *msg)
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
					szTmp = date.toString();
					break;
				case 1:
					szTmp = date.toString(Qt::ISODate);
					break;
				case 2:
					szTmp = date.toString(Qt::SystemLocaleDate);
					break;
			}
			replyCtcp(msg,szTmp);
		} else msg->bIgnored = true;
	}

	echoCtcpRequest(msg);
}

void KviServerParser::parseCtcpRequestPage(KviCtcpMessage *msg)
{
	if(!checkCtcpFlood(msg))
	{
		if(!KVI_OPTION_BOOL(KviOption_boolIgnoreCtcpPage))
		{
			KVI_OPTION_STRING(KviOption_stringCtcpPageReply).trimmed();
			if(KVI_OPTION_STRING(KviOption_stringCtcpPageReply).isEmpty())
			{
				KVI_OPTION_STRING(KviOption_stringCtcpPageReply) = KVI_DEFAULT_CTCP_PAGE_REPLY;
			}
			replyCtcp(msg,KVI_OPTION_STRING(KviOption_stringCtcpPageReply));
			if(KVI_OPTION_BOOL(KviOption_boolShowDialogOnCtcpPage))
			{
				if(!g_pCtcpPageDialog)g_pCtcpPageDialog = new KviCtcpPageDialog();
				KviStr szData8;
				szData8 = msg->pData;
				QString szData = Qt::escape(msg->msg->connection()->decodeText(szData8.ptr()));
				g_pCtcpPageDialog->addPage(msg->pSource->nick(),msg->pSource->user(),msg->pSource->host(),szData);
				g_pCtcpPageDialog->popup();
			}
		} else msg->bIgnored = true;
	}

	echoCtcpRequest(msg);
}

void KviServerParser::parseCtcpRequestAction(KviCtcpMessage *msg)
{
	KviStr szData8;
	// CTCP ACTION is a special exception... most clients do not encode/decode it.
	//msg->pData = extractCtcpParameter(msg->pData,szData8,false);
	szData8 = msg->pData;

	KviWindow * pOut = 0;
	bool bIsChannel = !IS_ME(msg->msg,msg->szTarget);

	QString szData;

	if(bIsChannel)
	{
		pOut = (KviWindow *)msg->msg->connection()->findChannel(msg->szTarget);
		if(pOut)szData = pOut->decodeText(szData8.ptr());
		else szData = msg->msg->connection()->decodeText(szData8.ptr());
	} else {
		KviQuery * query = msg->msg->connection()->findQuery(msg->pSource->nick());
		if(!query)
		{
			szData = msg->msg->connection()->decodeText(szData8.ptr());
			// New query requested ?
			// FIXME: 			#warning "CHECK FOR SPAM!"
			if(KVI_OPTION_BOOL(KviOption_boolCreateQueryOnPrivmsg))
			{
				// We still want to create it
				// Give the scripter a chance to filter it out again
				if(KVS_TRIGGER_EVENT_4_HALTED(KviEvent_OnQueryWindowRequest,msg->msg->console(),
							msg->pSource->nick(),
							msg->pSource->user(),
							msg->pSource->host(),
							szData))
				{
					// check if the scripter hasn't created it
					query = msg->msg->connection()->findQuery(msg->pSource->nick());
				} else {
					// no query yet, create it!
					// this will trigger OnQueryWindowCreated
					query = msg->msg->console()->connection()->createQuery(msg->pSource->nick());
					query->setTarget(msg->pSource->nick(),msg->pSource->user(),msg->pSource->host());
				}
			}
		}
		pOut = (KviWindow *)query;
		if(pOut)szData = pOut->decodeText(szData8.ptr());
		else szData = msg->msg->connection()->decodeText(szData8.ptr());
	}

	bool bTargetFound = pOut;
	if(!pOut)
	{
		pOut = KVI_OPTION_BOOL(KviOption_boolExternalMessagesToActiveWindow) ? msg->msg->console()->activeWindow() : msg->msg->console();
	}

	//see bug ticket #220
	if(KVI_OPTION_BOOL(KviOption_boolStripMircColorsInUserMessages))
		szData = KviMircCntrl::stripControlBytes(szData);

	if(KVS_TRIGGER_EVENT_5_HALTED(KviEvent_OnAction,pOut,
				msg->pSource->nick(),
				msg->pSource->user(),
				msg->pSource->host(),
				msg->szTarget,
				szData
			))
	{
		msg->msg->setHaltOutput();
		return;
	}

	int type = msg->msg->console()->applyHighlighting(pOut,KVI_OUT_ACTION,msg->pSource->nick(),msg->pSource->user(),msg->pSource->host(),szData);

	if(type < 0)return; // event stopped the message!

	if(type == KVI_OUT_HIGHLIGHT || !bIsChannel)
	{
		if(!pOut->hasAttention())
		{
			if(KVI_OPTION_BOOL(KviOption_boolFlashWindowOnHighlightedMessages))
				pOut->demandAttention();
			if(KVI_OPTION_BOOL(KviOption_boolPopupNotifierOnHighlightedMessages))
			{
				QString szMsg = "<b>";
				szMsg += msg->pSource->nick();
				szMsg += "</b> ";
				szMsg += Qt::escape(szData);
				//debug("kvi_sp_ctcp.cpp:975 debug: %s",szMsg.data());
				g_pApp->notifierMessage(pOut,KVI_OPTION_MSGTYPE(KVI_OUT_ACTION).pixId(),szMsg,KVI_OPTION_UINT(KviOption_uintNotifierAutoHideTime));
			}
		}
	}

	if(bTargetFound)
	{
		QString szMsg = QString("\r!n\r%1\r ").arg(msg->pSource->nick());
		szMsg += szData;
		if(bIsChannel)
		{
			((KviChannel *)pOut)->outputMessage(type,szMsg);
		} else {
			pOut->outputNoFmt(type,szMsg);
		}
	} else {
		if(bIsChannel)
		{
			pOut->output(KVI_OUT_SYSTEMWARNING,
				__tr2qs("The following CTCP ACTION has unrecognized target %Q"),
				&(msg->szTarget));
		}
		KviStr buffer1,buffer2;
		pOut->output(type,
			__tr2qs("CTCP ACTION from \r!n\r%Q\r [%Q@\r!h\r%Q\r]: %Q"),
			&(msg->pSource->nick()),&(msg->pSource->user()),
			&(msg->pSource->host()),&szData);
	}
}

// FIXME: #warning "UTSNAME ?...AND OTHER INFO ?...SYSTEM IDLE TIME ?...KVIRC IDLE TIME ?"

void KviServerParser::parseCtcpRequestAvatar(KviCtcpMessage *msg)
{
	// AVATAR
	if(!KVI_OPTION_BOOL(KviOption_boolIgnoreCtcpAvatar))
	{
		QString szGenderTag=" ";
		if(KVI_OPTION_STRING(KviOption_stringCtcpUserInfoGender).startsWith("m",Qt::CaseInsensitive)){
			szGenderTag.append("M");
		} else if(KVI_OPTION_STRING(KviOption_stringCtcpUserInfoGender).startsWith("f",Qt::CaseInsensitive)){
			szGenderTag.append("F");
		} else {
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

				QString szReply,szFileName;
				szFileName=a->name();
				if(KVI_OPTION_BOOL(KviOption_boolDCCFileTransferReplaceOutgoingSpacesWithUnderscores))
					szFileName.replace(" ","_");

				// escape the spaces with the right octal code
				encodeCtcpParameter(szFileName.toUtf8().data(),szReply);


				if(!a->isRemote())
				{
					KviSharedFile * o;
					if(!(o = g_pSharedFilesManager->addSharedFile(szFileName,a->localPath(),szUserMask,KVI_OPTION_UINT(KviOption_uintAvatarOfferTimeoutInSecs))))
					{
						// Don't delete o...it has been already deleted by g_pSharedFilesManager
						msg->msg->console()->output(KVI_OUT_SYSTEMWARNING,__tr2qs("Unable to add file offer for file %Q (File not readable?)"),&(a->localPath()));
					} else {
						if(_OUTPUT_VERBOSE)
						{
							msg->msg->console()->output(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Added %d sec file offer for file %Q (%Q) to recipient %Q"),
								KVI_OPTION_UINT(KviOption_uintAvatarOfferTimeoutInSecs),&(a->name()),&(a->localPath()),&szUserMask);
						}
					}
					//if(o)szReply.append(QString(" %1").arg(o->fileSize()));
				}

				szReply.append(szGenderTag);
				replyCtcp(msg,szReply);
			}
		} else {
			// no avatar set.. ignore channel requests if the user wishes
			if(!IS_ME(msg->msg,msg->szTarget))
			{
				// channel target
				if(KVI_OPTION_BOOL(KviOption_boolIgnoreChannelAvatarRequestsWhenNoAvatarSet))msg->bIgnored = true;
			}
			if(!msg->bIgnored)replyCtcp(msg,"");
		}
	} else msg->bIgnored = true;

	echoCtcpRequest(msg);
}


void KviServerParser::parseCtcpReplyAvatar(KviCtcpMessage *msg)
{
	QString szRemoteFile;
	QString szGender;
	QString decoded=msg->msg->console()->decodeText(msg->pData);

	decoded = extractCtcpParameter(decoded.toUtf8().data(),szRemoteFile,true);
	decoded = extractCtcpParameter(decoded.toUtf8().data(),szGender,true);
	szRemoteFile.trimmed();

	bool bPrivate = IS_ME(msg->msg,msg->szTarget);

	QString textLine;
	KviAvatar * avatar = 0;

	bool bResetAvatar = true;

	QString nickLink;

	KviQString::sprintf(nickLink,"\r!n\r%Q\r",&(msg->pSource->nick()));

	KviIrcUserEntry * e = msg->msg->connection()->userDataBase()->find(msg->pSource->nick());
	if(e){
		if( (szGender=="m") || (szGender=="M") ) {
			e->setGender(KviIrcUserEntry::Male);
		} else if((szGender=="f") || (szGender=="F") ) {
			e->setGender(KviIrcUserEntry::Female);
		} else {
			e->setGender(KviIrcUserEntry::Unknown);
		}
	}

	QString szWhere = bPrivate ? __tr2qs("private") : __tr2qs("channel notification:");
	QString szWhat = bPrivate ? __tr2qs("notification") : msg->szTarget;

	if(szRemoteFile.isEmpty())
	{
		// avatar unset
		KviQString::sprintf(textLine,__tr2qs("%Q unsets avatar"),
			&nickLink);
		if(_OUTPUT_VERBOSE)
			KviQString::appendFormatted(textLine," (%Q %Q)",&szWhere,&szWhat);
	} else {

		// FIXME: #warning "The avatar should be the one with the requested size!!"
		KviQString::sprintf(textLine,__tr2qs("%Q changes avatar to %s"),
			&nickLink,szRemoteFile.toUtf8().data(),&szWhere,&szWhat);
		if(_OUTPUT_VERBOSE)
			KviQString::appendFormatted(textLine," (%Q %Q)",&szWhere,&szWhat);

		bool bIsUrl = KviQString::equalCIN("http://",szRemoteFile,7) && (szRemoteFile.length() > 7);
		if(!bIsUrl)
		{
			// no hacks
			KviQString::cutToLast(szRemoteFile,'/');
			KviQString::cutToLast(szRemoteFile,'\\');
		}

		avatar = g_pIconManager->getAvatar(QString(),szRemoteFile);

		if((avatar == 0) && e)
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
					g_pApp->getLocalKvircDirectory(szLocalFilePath,KviApp::Avatars,szLocalFile);
					szLocalFilePath.replace('\\',"\\\\"); // <-- this is especially for windows
					QString szCommand = "http.get -w=nm ";
					unsigned int uMaxSize = KVI_OPTION_UINT(KviOption_uintMaximumRequestedAvatarSize);
					if(uMaxSize > 0)KviQString::appendFormatted(szCommand,"-m=%u ",uMaxSize);
					szRemoteFile = szRemoteFile.replace(";","%3B");
					szRemoteFile = szRemoteFile.replace("\"","%22");
					szCommand += "\""+szRemoteFile+"\"";
					szCommand += " \"";
					szCommand += szLocalFilePath;
					szCommand += "\"";

					if(KviKvsScript::run(szCommand,msg->msg->console()))
					{
						if(_OUTPUT_VERBOSE)
						{
							KviQString::appendFormatted(textLine,
								__tr2qs(": No valid local copy of avatar available, requesting one (HTTP GET %s)"),
								szRemoteFile.toUtf8().data());
						}
						g_pApp->setAvatarOnFileReceived(msg->msg->console(),
							szRemoteFile,msg->pSource->nick(),msg->pSource->user(),msg->pSource->host());
					} else {
						if(_OUTPUT_VERBOSE)
							KviQString::appendFormatted(textLine,__tr2qs(": No valid local copy of avatar available; failed to start an HTTP transfer, ignoring"));
					}
				} else {
					if(!checkCtcpFlood(msg))
					{
						if(_OUTPUT_VERBOSE)
						{
							KviQString::appendFormatted(textLine,
								__tr2qs(": No valid local copy of avatar available, requesting one (DCC GET %s)"),
								szRemoteFile.toUtf8().data());
						}

						QString szFName;
						encodeCtcpParameter(szRemoteFile.toUtf8().data(),szFName);
						msg->msg->connection()->sendFmtData("PRIVMSG %s :%cDCC GET %s%c",
								msg->msg->connection()->encodeText(msg->pSource->nick()).data(),0x01,msg->msg->connection()->encodeText(szFName.toUtf8().data()).data(),0x01);
						g_pApp->setAvatarOnFileReceived(msg->msg->console(),
							szRemoteFile,msg->pSource->nick(),msg->pSource->user(),msg->pSource->host());
					} else {
						if(_OUTPUT_VERBOSE)
							KviQString::appendFormatted(textLine,__tr2qs(": No valid local copy of avatar available; flood limit exceeded, ignoring"));
					}
				}
			} else {
				if(_OUTPUT_VERBOSE)
					KviQString::appendFormatted(textLine,__tr2qs(": No valid local copy of avatar available, ignoring"));
			}
		}
	}

	if(!e)
	{
		if(_OUTPUT_VERBOSE)
			KviQString::appendFormatted(textLine,__tr2qs(": No such nickname in the user database, ignoring the change"));
		msg->msg->console()->outputNoFmt(KVI_OUT_AVATAR,textLine);
		return;
	}

	if(bResetAvatar)e->setAvatar(avatar);

	msg->msg->console()->avatarChanged(avatar,msg->pSource->nick(),msg->pSource->user(),msg->pSource->host(),
												msg->msg->haltOutput() ? QString() : textLine);
}



typedef void (*dccModuleCtcpDccParseRoutine)(KviDccRequest *par);



void KviServerParser::parseCtcpRequestDcc(KviCtcpMessage *msg)
{
	KviDccRequest p;
	KviStr aux    = msg->pData;
	msg->pData    = extractCtcpParameter(msg->pData,p.szType, true, true);
	msg->pData    = extractCtcpParameter(msg->pData,p.szParam1);
	msg->pData    = extractCtcpParameter(msg->pData,p.szParam2);
	msg->pData    = extractCtcpParameter(msg->pData,p.szParam3);
	msg->pData    = extractCtcpParameter(msg->pData,p.szParam4);
	msg->pData    = extractCtcpParameter(msg->pData,p.szParam5);
	p.ctcpMsg     = msg;
	p.bIPv6       = msg->msg->console()->isIPv6Connection();
	p.pConsole    = msg->msg->console();

	KviRegisteredUser * u = msg->msg->connection()->userDataBase()->registeredUser(msg->pSource->nick(),msg->pSource->user(),msg->pSource->host());

	if (u)
	{
		if (u->isIgnoreEnabledFor(KviRegisteredUser::Dcc))
		{
			if (KVI_OPTION_BOOL(KviOption_boolVerboseIgnore))
			{
				msg->msg->console()->output(KVI_OUT_DCCREQUEST,
					__tr2qs("Ignoring DCC %S request from \r!n\r%Q\r [%Q@\r!h\r%Q\r] (%Q %S)"),
					&p.szType,&(msg->pSource->nick()),
					&(msg->pSource->user()),&(msg->pSource->host()),
					&msg->szTag,&aux);
			}
			return;
		}
	}

	/*if(KVI_OPTION_BOOL(KviOption_boolIgnoreCtcpDcc))
	{
		if(!msg->msg->haltOutput())
		{
			msg->msg->console()->output(KVI_OUT_DCCREQUEST,
				__tr2qs("Ignoring DCC %S request from \r!n\r%Q\r [%Q@\r!h\r%Q\r] (%Q %S)"),
				&p.szType,&(msg->pSource->nick()),
				&(msg->pSource->user()),&(msg->pSource->host()),
				&msg->szTag,&aux);
		}
		return;
	}*/

	bool bIsFlood = checkCtcpFlood(msg);

	if(bIsFlood &&
			((kvi_strEqualCI(p.szType.ptr(),"SEND")) ||
			(kvi_strEqualCI(p.szType.ptr(),"RSEND")) ||
			(kvi_strEqualCI(p.szType.ptr(),"TSEND")) ||
			(kvi_strEqualCI(p.szType.ptr(),"TRSEND"))
		))
	{
		// don't consider as flood the avatars we have requested
		if(g_pApp->findPendingAvatarChange(msg->msg->console(),msg->pSource->nick(),p.szParam1.ptr()))
			bIsFlood = false;
	}

	if(!bIsFlood)
	{
		if(!msg->msg->haltOutput())
		{
			QString decoded = msg->msg->console()->decodeText(p.szType.ptr());
			msg->msg->console()->output(KVI_OUT_DCCREQUEST,
				__tr2qs("Processing DCC %Q request from \r!n\r%Q\r [%Q@\r!h\r%Q\r] (%s %s)"),
				&decoded,&(msg->pSource->nick()),
				&(msg->pSource->user()),&(msg->pSource->host()),
				msg->szTag.toUtf8().data(),
				msg->msg->console()->decodeText(aux).toUtf8().data());
		}

		KviModule * m = g_pModuleManager->getModule("dcc");
		if(!m)
		{
			msg->msg->console()->output(KVI_OUT_DCCERROR,
				__tr2qs("Unable to process the above request: Cannot load DCC module (%Q)"),&(g_pModuleManager->lastError()));
		} else {
			dccModuleCtcpDccParseRoutine proc = (dccModuleCtcpDccParseRoutine)m->getSymbol("dccModuleCtcpDccParseRoutine");
			if(!proc)
			{
				msg->msg->console()->outputNoFmt(KVI_OUT_DCCERROR,
					__tr2qs("Unable to process the above request: DCC module may be broken"));
			} else {
				proc(&p);
			}
		}
	} else {
		// That's flood
		echoCtcpRequest(msg);
	}
}

void KviServerParser::parseCtcpReplyUserinfo(KviCtcpMessage *msg)
{
	QString szRemoteFile;
	QString szGender;
	QString decoded=msg->msg->console()->decodeText(msg->pData);

	bool bNeedToUpdateUserlist = false;
	KviIrcUserEntry * e = msg->msg->connection()->userDataBase()->find(msg->pSource->nick());
	if(e)
	{
		int pos = decoded.indexOf("Gender=",0,Qt::CaseInsensitive);

		if(pos>=0)
		{
			QChar c = decoded[pos+7];
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
		if(KviQString::equalCS(g_pActiveWindow->metaObject()->className(),QString("KviChannel")))
		{
			((KviChannel*)g_pActiveWindow)->userListView()->updateArea();
		}

	}

	echoCtcpReply(msg);
}

void KviServerParser::parseCtcpReplyGeneric(KviCtcpMessage *msg)
{
	echoCtcpReply(msg);
}


void KviServerParser::parseCtcpReplyLagcheck(KviCtcpMessage * msg)
{
	// this is an internal CTCP used for checking lag
	KviStr szTag;
	msg->pData = extractCtcpParameter(msg->pData,szTag,true);
	if(msg->msg->console()->connection()->lagMeter())
		msg->msg->console()->connection()->lagMeter()->lagCheckComplete(szTag.ptr());
}

//ERRORMSG,ECHO,ERRMSG
//SED,DCC,SOUND/MULTIMEDIA/MM,SCRIPT
