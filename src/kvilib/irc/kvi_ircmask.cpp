//=============================================================================
//
//   File : kvi_ircuser.cpp
//   Creation date : Fri Jan 8 1999 20:56:07 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2008 Szymon Stefanek (pragma at kvirc dot net)
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



#include "kvi_debug.h"
#include "kvi_ircmask.h"

/*
	@doc: irc_masks
	@title:
		Irc masks
	@type:
		generic
	@short:
		Decription of the standard IRC masks
	@keyterms:
		irc masks , nickname , username , hostname , wildcard
	@body:
		[big]Simple masks[/big][br]
		An irc mask is a string in a special format that identifies an user on irc.[br]
		The standard basic format is:[br]
		[b]<nick>!<username>@<host>[/b][br]
		The <nick> part contains the nickname with that the user is widely known across the network.[br]
		The nickname format is generally restricted by the irc network rules: usually it has a maximum
		length (9 on actual IrcNet servers for example), and can contain only a defined set of characters.
		Just as example, the character '!' obviously can't be included in a nickname.[br]
		The <username> part is the machine username of the remote user: this is usually
		retrieved by the irc server at connect time by contacting the ident service on the user's machine.
		Some IRC servers allow specifying this username inside the login messages and do not connect
		to the ident service at all.[br]
		The <username> often has a special prefix character added by the irc server:[br]
		this is rather server specific protocol , but the prefixes are somewhat standardized and
		the common meanings of them are:[br]
		noprefix: I line with ident[br]
		^: I line with OTHER type ident[br]
		~: I line, no ident[br]
		+: i line with ident[br]
		=: i line with OTHER type ident[br]
		-: i line, no ident[br]
		So finally you can find <username> strings like "~pragma" or "^pragma", where "pragma"
		is the system username of the irc-user and ~ and ^ are prefixes.[br]
		The <host> part is the hostname of the remote user.[br]
		In most cases it is the human-readable format of the host name, but sometimes
		it happens to be an IP-address (when the host has no reverse dns entry).[br]
		The IP address can be either in IPV4 format or in IPV6 format.[br]
		Some (weird from my point of view) servers hide certain parts of the IP address to
		prevent attacks to the user's machine.[br]
		Here are some examples of full irc-masks:[br]
		Pragma!^pragma@staff.kvirc.net[br]
		[jazz]!~jazz@jazz.myhome.com[br]
		luke!=skywalker@212.213.41.12[br]
		HAN!^solo@ff0f:a0a0:1011::ea80:1[br]
		Darth!vader@210.11.12.XXX[br]
		The irc-masks are [b]case insensitive[/b].[br]
		[br]
		[big]Wildcard masks[/big][br]
		In some contexts the irc-masks can contain '*' and '?' wildcards.[br]
		The wild masks are used to "match" an user within a set of them.[br]
		'*' matches any sequence (eventually empty) of characters and '?' matches a single character.[br]
		Wildcards are allowed only in the <nick> , <user> and <host> part: so the
		"wildest" mask possible is:[br]
		[b]*!*@*[/b][br]
		that designates "any nickname, any username on any host".[br]
		Here are some examples of wild masks:[br]
		Pragma!*pragma@212.101.102.*: matches any user with nickname "Pragma" , username that ends with "pragma" and
		coming from any machine on the 212.101.102 network.[br]
		*!solo@*.starwars.org: matches any nick with username solo (no prefix!) coming from any machine in
		the starwars.org domain.[br]
		Pragma!*@*: matches any user with nickname "Pragma".[br]
*/

/*
const char * KviIrcMask::setMask(const char *szMask,char c)
{
	__range_valid(szMask);
	//nick!username@host.top
	//0123456789
	register const char *p=szMask;
	//Run over nick....
	while(*p && (*p != '!'))p++;
	int len = p - szMask;
	if(len > 0){
		m_nick_ptr = (char *)kvi_realloc(m_nick_ptr,len+1);
		kvi_memmove((void *)m_nick_ptr,(void *)szMask,len);
	} else { //Empty nick...set it to "*"
		len = 1;
		m_nick_ptr = (char *)kvi_realloc(m_nick_ptr,len+1);
		kvi_memmove((void *)m_nick_ptr,(void *)"*",len);
	}
	*(m_nick_ptr+len) = '\0'; //With zero length nick it will be just an empty-string
	if(!(*p)){
		setHost("*");
		setUsername("*");
		return p;
	}
	szMask = ++p;
	//The username
	while(*p && (*p != '@'))p++;
	len = p - szMask;
	if(len > 0){
		m_user_ptr = (char *)kvi_realloc(m_user_ptr,len+1);
		kvi_memmove((void *)m_user_ptr,(void *)szMask,len);
	} else {
		len = 1;
		m_user_ptr = (char *)kvi_realloc(m_user_ptr,len+1);
		kvi_memmove((void *)m_user_ptr,(void *)"*",len);
	}
	*(m_user_ptr+len) = '\0';
	if(!(*p)){
		setHost("*");
		return p;
	}
	szMask = ++p;
	//And finally the host
	while(*p && (*p != c))p++;
	len = p - szMask;
	if(len > 0){
		m_host_ptr = (char *)kvi_realloc(m_host_ptr,len+1);
		kvi_memmove((void *)m_host_ptr,(void *)szMask,len);
	} else {
		len = 1;
		m_host_ptr = (char *)kvi_realloc(m_host_ptr,len+1);
		kvi_memmove((void *)m_host_ptr,(void *)"*",len);
	}
	*(m_host_ptr+len) = '\0';
	return p;
}

const char * KviIrcMask::setUserhostMask(const char *szMask)
{
	__range_valid(szMask);
	//nick[*]=<+!->username@host.top
	//0123456789
	register const char *p=szMask;
	// Run over nick....
	while(*p && (*p != '*') && (*p != '=') && (!isspace(*p)))p++;
	// extract it
	int len = p - szMask;
	if(len > 0){
		m_nick_ptr = (char *)kvi_realloc(m_nick_ptr,len+1);
		kvi_memmove((void *)m_nick_ptr,(void *)szMask,len);
	} else { //Empty nick...set it to "*"
		len = 1;
		m_nick_ptr = (char *)kvi_realloc(m_nick_ptr,len+1);
		kvi_memmove((void *)m_nick_ptr,(void *)"*",len);
	}
	*(m_nick_ptr+len) = '\0'; //With zero length nick it will be just an empty-string
	// now skip all the flags
	while(*p && ((*p=='*')||(*p=='=')||(*p=='+')||(*p=='-')) && (!isspace(*p)))p++;
	// check...
	if((!(*p)) || isspace(*p)){
		// ooops  , finished or isspace
		setHost("*");
		setUsername("*");
		while(*p && isspace(*p))p++;
		return p;
	}

	szMask = p;
	//The username
	while(*p && (*p != '@') && (!isspace(*p)))p++;
	len = p - szMask;
	if(len > 0){
		m_user_ptr = (char *)kvi_realloc(m_user_ptr,len+1);
		kvi_memmove((void *)m_user_ptr,(void *)szMask,len);
	} else {
		len = 1;
		m_user_ptr = (char *)kvi_realloc(m_user_ptr,len+1);
		kvi_memmove((void *)m_user_ptr,(void *)"*",len);
	}
	*(m_user_ptr+len) = '\0';

	if((!(*p))||isspace(*p)){
		// oops finished or isspace
		setHost("*");
		while(*p && isspace(*p))p++;
		return p;
	}
	szMask = ++p;
	//And finally the host
	while(*p && (!isspace(*p)))p++;
	len = p - szMask;
	if(len > 0){
		m_host_ptr = (char *)kvi_realloc(m_host_ptr,len+1);
		kvi_memmove((void *)m_host_ptr,(void *)szMask,len);
	} else {
		len = 1;
		m_host_ptr = (char *)kvi_realloc(m_host_ptr,len+1);
		kvi_memmove((void *)m_host_ptr,(void *)"*",len);
	}
	*(m_host_ptr+len) = '\0';
	while(*p && isspace(*p))p++;
	return p;
}

*/

KviIrcMask::KviIrcMask()
{
	m_szHost = m_szWild;
	m_szUser = m_szWild;
	m_szNick = m_szWild;
}

KviIrcMask::KviIrcMask(const QString &szMask)
{
	static QString szWild("*");
	const QChar * b = KviQString::nullTerminatedArray(szMask);
	if(b)
	{
		const QChar * p = b;
		while(p->unicode() && (p->unicode() != '!'))p++;
		if(p->unicode())
		{
			if(p != b)
			{
				m_szNick.setUnicode(b,p-b);
			} else {
				m_szNick = szWild; // ???
			}
		} else {
			if(p != b)m_szNick.setUnicode(b,p-b);
			else m_szNick = szWild; // ???
			m_szUser = szWild;
			m_szHost = szWild;
			return;
		}
		p++;
		b = p;
		while(p->unicode() && (p->unicode() != '@'))p++;
		if(p->unicode())
		{
			if(p != b)
			{
				m_szUser.setUnicode(b,p-b);
			} else {
				m_szUser = szWild; // ???
			}
		} else {
			if(p != b)m_szUser.setUnicode(b,p-b);
			else m_szUser = szWild; // ???
			m_szHost = szWild;
			return;
		}
		p++;
		b=p;
		while(p->unicode())p++;
		if(p != b)
		{
			m_szHost.setUnicode(b,p-b);
		} else {
			m_szHost = szWild; // ???
		}

	} else {
		m_szUser = szWild;
		m_szHost = szWild;
		m_szNick = szWild;
	}
}

QString KviIrcMask::m_szWild("*");

bool KviIrcMask::hasNumericHost() const
{
	const QChar * p = KviQString::nullTerminatedArray(m_szHost);
	if(!p)return false;
	int nPoints = 0;
	int nDoublePoints = 0;
	unsigned short uc;

	if(m_szHost.endsWith("=")) // for CR servers, see ticket #358
		return true;

	while((uc = p->unicode()))
	{
		if(uc == '.')nPoints++; // ipv6 masks can contain dots too!
		else {
			if(uc == ':')nDoublePoints++;
			else {
				if((uc < '0') || (uc > '9'))
				{
					uc = p->toUpper().unicode();
					if((uc < 'A') || (uc > 'F'))return false;
				}
			}
		}
		p++;
	}
	return ((nPoints == 3) || (nDoublePoints > 1));
}


/**
* Returns in szMask the specified (if possible) mask of this user.<br>
* If the host or username are not known , the mask may contain less information
* than requested.<br>
* Mask types:<br>
* 0 : nick!user@machine.host.top  (nick!user@XXX.XXX.XXX.XXX) (default)<br>
* 1 : nick!user@*.host.top        (nick!user@XXX.XXX.XXX.*)<br>
* 2 : nick!user@*<br>
* 3 : nick!*@machine.host.top     (nick!user@XXX.XXX.XXX.XXX)<br>
* 4 : nick!*@*.host.top           (nick!user@XXX.XXX.XXX.*)<br>
* 5 : nick!*@*<br>
* 6 : *!user@machine.host.top     (*!user@XXX.XXX.XXX.XX)<br>
* 7 : *!user@*.host.top           (*!user@XXX.XXX.XXX.*)<br>
* 8 : *!user@*<br>
* 9 : *!*@machine.host.top        (*!*@XXX.XXX.XXX.XXX)<br>
* 10: *!*@*.host.top              (*!*@XXX.XXX.XXX.*)<br>
* 11: nick!*user@machine.host.top (nick!*user@machine.host.top)<br>
* 12: nick!*user@*.host.top       (nick!*user@*.host.top)<br>
* 13: nick!*user@*<br>
* 14: *!*user@machine.host.top    (*!*user@machine.host.top)<br>
* 15: *!*user@*.host.top          (*!*user@*.host.top)<br>
* 16: *!*user@*<br>
* 17: nick!~user@*.host.top       (nick!~user@XXX.XXX.*)
* 18: nick!*@*.host.top          (nick!*@XXX.XXX.*)
* 19: *!~user@*.host.top          (*!~user@XXX.XXX.*)
* 20: nick!*user@*.host.top          (nick!*user@XXX.XXX.*)
* 21: *!*user@*.host.top          (*!user@*XXX.XXX.*)
* smart versions of the masks 17-21 that try take care of masked ip addresses
* in the form xxx.xxx.INVALID-TOP-MASK
* 22: nick!~user@*.host.top       (nick!~user@XXX.XXX.*)
* 23: nick!*@*.host.top          (nick!*@XXX.XXX.*)
* 24: *!~user@*.host.top          (*!~user@XXX.XXX.*)
* 25: nick!*user@*.host.top          (nick!*user@XXX.XXX.*)
* 26: *!*user@*.host.top          (*!user@*XXX.XXX.*)
* If some data is missing , these types may change:<br>
* For example , if hostname is missing , the mask type 3 or 4 may be reduced to type 5
*/

/*
** ident is fun.. ahem
** prefixes used:
** 	none	I line with ident
**	^	I line with OTHER type ident
**	~	I line, no ident
** 	+	i line with ident
**	=	i line with OTHER type ident
**	-	i line, no ident
*/

static unsigned char maskTable[27][3] = {
	{ 0 , 0 , 0 }, //0 means normal block
	{ 0 , 0 , 2 }, //2 in the third field means type *.abc.host.top (or XXX.XXX.XXX.*) host mask
	{ 0 , 0 , 1 }, //2 in the second field means *user (strip prefixes)
	{ 0 , 1 , 0 }, //1 means *
	{ 0 , 1 , 2 }, //3 in the third field means type *.host.top (or XXX.XXX.*) host mask
	{ 0 , 1 , 1 }, //4 in the third field is like 3 but tries to detect masked ip addresses too
	{ 1 , 0 , 0 },
	{ 1 , 0 , 2 },
	{ 1 , 0 , 1 },
	{ 1 , 1 , 0 },
	{ 1 , 1 , 2 },
	{ 0 , 2 , 0 },
	{ 0 , 2 , 2 },
	{ 0 , 2 , 1 },
	{ 1 , 2 , 0 },
	{ 1 , 2 , 2 },
	{ 1 , 2 , 1 },
	{ 0 , 0 , 3 },
	{ 0 , 1 , 3 },
	{ 1 , 0 , 3 },
	{ 0 , 2 , 3 },
	{ 1 , 2 , 3 },
	{ 0 , 0 , 4 },
	{ 0 , 1 , 4 },
	{ 1 , 0 , 4 },
	{ 0 , 2 , 4 },
	{ 1 , 2 , 4 }
};

void KviIrcMask::mask(QString &szMask,MaskType eMaskType) const
{
	if((((int)eMaskType) > 26)||(((int)eMaskType) < 0))eMaskType = NickUserHost;
	szMask = maskTable[((int)eMaskType)][0] ? m_szWild : m_szNick;
	szMask.append("!");
	switch(maskTable[((int)eMaskType)][1])
	{
		case 0:
			szMask.append(m_szUser);
			break;
		case 1:
			szMask.append(m_szWild);
			break;
		default:
			if (m_szUser.length() > 0) {
				if(m_szUser[0].unicode() != '*')
					szMask.append(m_szWild);
				if ((m_szUser[0].unicode() == '~') ||
					(m_szUser[0].unicode() == '^') ||
					(m_szUser[0].unicode() == '+') ||
					(m_szUser[0].unicode() == '-') ||
					(m_szUser[0].unicode() == '='))szMask.append(m_szUser.right(m_szUser.length() - 1));
				else
					szMask.append(m_szUser);
			}
		break;
	}
	szMask.append('@');
	switch(maskTable[((int)eMaskType)][2])
	{
		case 0:
			szMask.append(m_szHost);
			break;
		case 1:
			szMask.append(m_szWild);
			break;
		case 2:
			if(m_szHost != m_szWild)
			{
				if(hasNumericHost())
				{
					QString szHost(m_szHost.left(getIpDomainMaskLen()));
					szMask.append(szHost);
					szMask.append(m_szWild);
				} else {
					szMask.append(m_szWild);
					szMask.append(getHostDomainMask());
				}
			} else {
				szMask.append(m_szWild);
			}
			break;
		case 3:
			if(m_szHost != m_szWild)
			{
				if(hasNumericHost())
				{
					QString szHost(m_szHost.left(getLargeIpDomainMaskLen()));
					szMask.append(szHost);
					szMask.append(m_szWild);
				} else {
					szMask.append(m_szWild);
					szMask.append(getLargeHostDomainMask());
				}
			} else {
				szMask.append(m_szWild);
			}
			break;
		default: // case 4 and others
			if(m_szHost != m_szWild)
			{
				if(hasNumericHost() || hasMaskedIp())
				{
					QString szHost(m_szHost.left(getLargeIpDomainMaskLen()));
					szMask.append(szHost);
					szMask.append(m_szWild);
				} else {
					szMask.append(m_szWild);
					szMask.append(getLargeHostDomainMask());
				}
			} else {
				szMask.append(m_szWild);
			}
			break;
	}
}


/*
bool KviIrcMask::matches(const char *szMask)
{
	const char * ret1;
	const char * ret2;

	if(kvi_matchWildExprWithTerminator(szMask,m_nick_ptr,'!',&ret1,&ret2))
	{
		if(*ret1 == '!')
		{
			ret1++;
			if(kvi_matchWildExprWithTerminator(ret1,m_user_ptr,'@',&ret1,&ret2))
			{
				if(*ret1 == '@')
				{
					ret1++;
					return kvi_matchWildExpr(ret1,m_host_ptr);
				}
			}
		}
	}
	return false;
}
*/

/*
bool KviIrcMask::matchesFixed(const char *szMask) const
{
	const char * ret1;
	const char * ret2;

	if(kvi_matchStringWithTerminator(m_nick_ptr,szMask,'!',&ret1,&ret2))
	{
		if(*ret2 == '!')
		{
			ret2++;
			if(kvi_matchStringWithTerminator(m_user_ptr,ret2,'@',&ret1,&ret2))
			{
				if(*ret2 == '@')
				{
					ret2++;
					return kvi_matchString(m_host_ptr,ret2);
				}
			}
		}
	}
	return false;
}
*/

/*
bool KviIrcMask::matchedBy(const QString &szMask) const
{
	const char * ret1;
	const char * ret2;

	if(kvi_matchStringWithTerminator(szMask,m_nick_ptr,'!',&ret1,&ret2))
	{
		if(*ret1 == '!')
		{
			ret1++;
			if(kvi_matchStringWithTerminator(ret1,m_user_ptr,'@',&ret1,&ret2))
			{
				if(*ret1 == '@')
				{
					ret1++;
					return kvi_matchString(ret1,m_host_ptr);
				}
			}
		}
	}
	return false;
}
*/

bool KviIrcMask::matches(const KviIrcMask &mask) const
{
	if(KviQString::matchWildExpressions(m_szNick,mask.m_szNick))
	{
		if(KviQString::matchWildExpressions(m_szUser,mask.m_szUser))
		{
			if(KviQString::matchWildExpressions(m_szHost,mask.m_szHost))return true;
		}
	}
	return false;
}

bool KviIrcMask::matchesFixed(const KviIrcMask &mask) const
{
	if(KviQString::matchString(m_szNick,mask.m_szNick,false,true))
	{
		if(KviQString::matchString(m_szUser,mask.m_szUser,false,true))
		{
			if(KviQString::matchString(m_szHost,mask.m_szHost,false,true))
			{
				return true;
			}
		}
	}
	return false;
}

/*
bool KviIrcMask::matches(const char * nick,const char * user,const char * host)
{
	if(nick)
	{
		if(!kvi_matchWildExpr(m_nick_ptr,nick))return false;
	}

	if(user)
	{
		if(!kvi_matchWildExpr(m_user_ptr,user))return false;
	}

	if(host)
	{
		if(!kvi_matchWildExpr(m_host_ptr,host))return false;
	}
	return true;
}
*/

bool KviIrcMask::matchesFixed(const QString &nick,const QString &user,const QString &host) const
{
	if(!KviQString::matchString(m_szNick,nick,false,true)) return false;
	if(!KviQString::matchString(m_szUser,user,false,true)) return false;
	if(!KviQString::matchString(m_szHost,host,false,true)) return false;
	return true;
}

//Internals for mask()

int KviIrcMask::getIpDomainMaskLen() const
{
	int len = m_szHost.length();
	const QChar *p = m_szHost.unicode();
	const QChar *b = p;
	p += len;
	if(b < p)
	{
		p--;
		while((b < p) && (p->unicode() != '.') && (p->unicode() != ':'))p--;
	}
	// 000.000.000.000
	//            p
	//
	return (p == b) ? 0 : ((p-b) + 1);
}


int KviIrcMask::getLargeIpDomainMaskLen() const
{
	int len = m_szHost.length();
	const QChar *p = m_szHost.unicode();
	const QChar *b = p;
	p += len;
	if(b < p)
	{
		p--;
		while((b < p) && (p->unicode() != '.') && (p->unicode() != ':'))p--;
		if(b < p)
		{
			p--;
			while((b < p) && (p->unicode() != '.') && (p->unicode() != ':'))p--;
		}
	}
	// 000.000.000.000
	//        p
	//
	return (p == b) ? 0 : ((p-b) + 1);
}

QString KviIrcMask::getHostDomainMask() const
{
	int len = m_szHost.length();
	const QChar *p=KviQString::nullTerminatedArray(m_szHost);
	if(!p)return QString();
	const QChar *b = p;
	while(p->unicode() && p->unicode() != '.')p++;
	QString ret(p,len - (p - b));
	return ret;
}

QString KviIrcMask::getLargeHostDomainMask() const
{
	int len = m_szHost.length();
	const QChar *p = m_szHost.unicode();
	const QChar *b = p;
	p += len;

	if(b < p)
	{
		p--;
		while((b < p) && (p->unicode() != '.'))p--;
		if(b < p)
		{
			p--;
			while((b < p) && (p->unicode() != '.'))p--;
		}
	}

	// xyz.klm.abc.host.top
	//            p

	QString ret(p,len - (p - b));
	return ret;
}

bool KviIrcMask::hasMaskedIp() const
{
	int len = m_szHost.length();
	const QChar *p = m_szHost.unicode();
	const QChar *b = p;
	if(len == 0)return false;
	//run to the end
	p += len;
	const QChar *e = p;
	p--;
	while((b < p) && (p->unicode() != '.'))p--;
	return ((e - p) > 4); // at the moment 4 should be enough : the largest top part is "name"
}

bool KviIrcMask::operator==(const KviIrcMask &user)
{
	if(KviQString::equalCI(m_szNick,user.m_szNick))
	{
		if(KviQString::equalCI(m_szUser,user.m_szUser))
		{
			if(KviQString::equalCI(m_szHost,user.m_szHost))return true;
		}
	}
	return false;
}

bool KviIrcMask::hasWildNick()
{
	const QChar * aux = KviQString::nullTerminatedArray(m_szNick);
	if(!aux)return false;
	unsigned short uc;
	while((uc = aux->unicode()))
	{
		if((uc == '*') || (uc == '?'))return true;
		aux++;
	}
	return false;
}

int KviIrcMask::nonWildChars()
{
	int iCnt = 0;
	const QChar * aux = KviQString::nullTerminatedArray(m_szNick);
	if(!aux)return 0;
	unsigned short uc;

	while((uc = aux->unicode()))
	{
		if((uc != '*') && (uc != '?'))iCnt++;
		aux++;
	}

	aux = KviQString::nullTerminatedArray(m_szUser);
	while((uc = aux->unicode()))
	{
		if((uc != '*') && (uc != '?'))iCnt++;
		aux++;
	}

	aux = KviQString::nullTerminatedArray(m_szHost);
	while((uc = aux->unicode()))
	{
		if((uc != '*') && (uc != '?'))iCnt++;
		aux++;
	}
	return iCnt;
}
