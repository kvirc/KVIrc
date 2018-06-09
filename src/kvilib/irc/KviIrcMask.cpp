//=============================================================================
//
//   File : KviIrcMask.cpp
//   Creation date : Fri Jan 8 1999 20:56:07 by Szymon Stefanek
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

#include "kvi_debug.h"
#include "KviIrcMask.h"
#include "KviQString.h"

#include <QRegExp>

/*
	@doc: irc_masks
	@title:
		IRC masks
	@type:
		generic
	@short:
		Description of the standard IRC masks
	@keyterms:
		irc masks, nickname, username, hostname, wildcard
	@body:
		[big]Simple masks[/big]
		An IRC mask is a string in a special format that identifies a user on IRC.[br]
		The standard basic format is:[br]
		[b]<nick>!<username>@<host>[/b][br]
		The <nick> part contains the nickname with that the user is widely known across the network.[br]
		The nickname format is generally restricted by the IRC network rules: usually it has a maximum
		length (9 on actual IrcNet servers for example), and can contain only a defined set of characters.
		Just as example, the character '!' obviously can't be included in a nickname.[br]
		The <username> part is the machine username of the remote user: this is usually
		retrieved by the IRC server at connect time by contacting the Ident service on the user's machine.
		Some IRC servers allow specifying this username inside the login messages and do not connect
		to the Ident service at all.[br]
		The <username> often has a special prefix character added by the IRC server:[br]
		this is rather server specific protocol, but the prefixes are somewhat standardized and
		the common meanings of them are:[br]
		[pre]
			noprefix: I line with Ident
			^: I line with OTHER type Ident
			~: I line, no Ident
			+: i line with Ident
			=: i line with OTHER type Ident
			-: i line, no Ident
		[/pre]
		So finally you can find <username> strings like [i]~pragma[/i] or [i]^pragma[/i], where [i]pragma[/i]
		is the system username of the irc-user and ~ and ^ are prefixes.[br]
		The <host> part is the hostname of the remote user.[br]
		In most cases it is the human-readable format of the host name, but sometimes
		it happens to be an IP-address (when the host has no reverse DNS entry).[br]
		The IP address can be either in IPv4 format or in IPv6 format.[br]
		Some (weird from my point of view) servers hide certain parts of the IP address to
		prevent attacks to the user's machine.[br]
		Here are some examples of full irc-masks:[br]
		Pragma!^pragma@staff.kvirc.net[br]
		[jazz]!~jazz@jazz.myhome.com[br]
		luke!=skywalker@212.213.41.12[br]
		HAN!^solo@ff0f:a0a0:1011::ea80:1[br]
		Darth!vader@210.11.12.XXX[br]
		The irc-masks are [b]case insensitive[/b].
		[big]Wildcard masks[/big]
		In some contexts the irc-masks can contain [b]*[/b] and [b]?[/b] wildcards.[br]
		The wild masks are used to [i]match[/i] a user within a set of them.[br]
		[b]*[/b] matches any sequence (eventually empty) of characters and [b]?[/b] matches a single character.[br]
		Wildcards are allowed only in the <nick>, <user> and <host> part: so the
		[i]wildest[/i] mask possible is:[br]
		[b]*!*@*[/b][br]
		that designates [i]any nickname, any username on any host[/i].[br]
		Here are some examples of wild masks:[br]
		Pragma!*pragma@212.101.102.*: matches any user with nickname [i]Pragma[/i], username that ends with [i]pragma[/i] and
		coming from any machine on the 212.101.102 network.[br]
		*!solo@*.starwars.org: matches any nick with username solo (no prefix!) coming from any machine in
		the starwars.org domain.[br]
		Pragma!*@*: matches any user with nickname [i]Pragma[/i].[br]
*/

static unsigned char ucMaskTable[27][3] = {
	{ 0, 0, 0 }, //0 means normal block
	{ 0, 0, 2 }, //2 in the third field means type *.abc.host.top (or XXX.XXX.XXX.*) host mask
	{ 0, 0, 1 }, //2 in the second field means *user (strip prefixes)
	{ 0, 1, 0 }, //1 means *
	{ 0, 1, 2 }, //3 in the third field means type *.host.top (or XXX.XXX.*) host mask
	{ 0, 1, 1 }, //4 in the third field is like 3 but tries to detect masked ip addresses too
	{ 1, 0, 0 },
	{ 1, 0, 2 },
	{ 1, 0, 1 },
	{ 1, 1, 0 },
	{ 1, 1, 2 },
	{ 0, 2, 0 },
	{ 0, 2, 2 },
	{ 0, 2, 1 },
	{ 1, 2, 0 },
	{ 1, 2, 2 },
	{ 1, 2, 1 },
	{ 0, 0, 3 },
	{ 0, 1, 3 },
	{ 1, 0, 3 },
	{ 0, 2, 3 },
	{ 1, 2, 3 },
	{ 0, 0, 4 },
	{ 0, 1, 4 },
	{ 1, 0, 4 },
	{ 0, 2, 4 },
	{ 1, 2, 4 }
};

KviIrcMask::KviIrcMask()
{
	m_szHost = m_szWild;
	m_szUser = m_szWild;
	m_szNick = m_szWild;
}

KviIrcMask::KviIrcMask(const QString & szMask)
{
	static QString szWild("*");
	const QChar * pChar = szMask.constData();
	if(pChar)
	{
		const QChar * pChar2 = pChar;
		while(pChar2->unicode() && (pChar2->unicode() != '!'))
			pChar2++;
		if(pChar2->unicode())
		{
			if(pChar2 != pChar)
				m_szNick.setUnicode(pChar, pChar2 - pChar);
			else
				m_szNick = szWild; // ???
		}
		else
		{
			if(pChar2 != pChar)
				m_szNick.setUnicode(pChar, pChar2 - pChar);
			else
				m_szNick = szWild; // ???
			m_szUser = szWild;
			m_szHost = szWild;
			return;
		}
		pChar2++;
		pChar = pChar2;
		while(pChar2->unicode() && (pChar2->unicode() != '@'))
			pChar2++;
		if(pChar2->unicode())
		{
			if(pChar2 != pChar)
				m_szUser.setUnicode(pChar, pChar2 - pChar);
			else
				m_szUser = szWild; // ???
		}
		else
		{
			if(pChar2 != pChar)
				m_szUser.setUnicode(pChar, pChar2 - pChar);
			else
				m_szUser = szWild; // ???
			m_szHost = szWild;
			return;
		}
		pChar2++;
		pChar = pChar2;
		while(pChar2->unicode())
			pChar2++;
		if(pChar2 != pChar)
			m_szHost.setUnicode(pChar, pChar2 - pChar);
		else
			m_szHost = szWild; // ???
	}
	else
	{
		m_szUser = szWild;
		m_szHost = szWild;
		m_szNick = szWild;
	}
}

QString KviIrcMask::m_szWild("*");

bool KviIrcMask::hasNumericHost() const
{
	const QChar * pChar = m_szHost.constData();
	if(!pChar)
		return false;
	int iPoints = 0;
	int iDoublePoints = 0;
	unsigned short uc;

	if(m_szHost.endsWith("=")) // for CR servers, see ticket #358
		return true;

	while((uc = pChar->unicode()))
	{
		if(uc == '.')
			iPoints++; // ipv6 masks can contain dots too!
		else
		{
			if(uc == ':')
				iDoublePoints++;
			else
			{
				if((uc < '0') || (uc > '9'))
				{
					uc = pChar->toUpper().unicode();
					if((uc < 'A') || (uc > 'F'))
						return false;
				}
			}
		}
		pChar++;
	}
	return ((iPoints == 3) || (iDoublePoints > 1));
}

void KviIrcMask::mask(QString & szMask, MaskType eMaskType) const
{
	if((((int)eMaskType) > 26) || (((int)eMaskType) < 0))
		eMaskType = NickUserHost;
	szMask = ucMaskTable[((int)eMaskType)][0] ? m_szWild : m_szNick;
	szMask.append("!");
	switch(ucMaskTable[((int)eMaskType)][1])
	{
		case 0:
			szMask.append(m_szUser);
			break;
		case 1:
			szMask.append(m_szWild);
			break;
		default:
			if(m_szUser.length() > 0)
			{
				if(m_szUser[0].unicode() != '*')
					szMask.append(m_szWild);
				if((m_szUser[0].unicode() == '~') || (m_szUser[0].unicode() == '^') || (m_szUser[0].unicode() == '+') || (m_szUser[0].unicode() == '-') || (m_szUser[0].unicode() == '='))
					szMask.append(m_szUser.right(m_szUser.length() - 1));
				else
					szMask.append(m_szUser);
			}
			break;
	}
	szMask.append('@');
	switch(ucMaskTable[((int)eMaskType)][2])
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
				}
				else
				{
					szMask.append(m_szWild);
					szMask.append(getHostDomainMask());
				}
			}
			else
			{
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
				}
				else
				{
					szMask.append(m_szWild);
					szMask.append(getLargeHostDomainMask());
				}
			}
			else
			{
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
				}
				else
				{
					szMask.append(m_szWild);
					szMask.append(getLargeHostDomainMask());
				}
			}
			else
			{
				szMask.append(m_szWild);
			}
			break;
	}
}

bool KviIrcMask::matches(const KviIrcMask & mask) const
{
	if(KviQString::matchWildExpressions(m_szNick, mask.m_szNick))
	{
		if(KviQString::matchWildExpressions(m_szUser, mask.m_szUser))
		{
			if(KviQString::matchWildExpressions(m_szHost, mask.m_szHost))
				return true;
		}
	}
	return false;
}

bool KviIrcMask::matchesFixed(const KviIrcMask & mask) const
{
	if(matchWildString(m_szNick, mask.m_szNick))
	{
		if(matchWildString(m_szUser, mask.m_szUser))
		{
			if(matchWildString(m_szHost, mask.m_szHost))
				return true;
		}
	}
	return false;
}

bool KviIrcMask::matchesFixed(const QString & szNick, const QString & szUser, const QString & szHost) const
{
	if(!matchWildString(m_szNick, szNick))
		return false;
	if(!matchWildString(m_szUser, szUser))
		return false;
	if(!matchWildString(m_szHost, szHost))
		return false;
	return true;
}

bool KviIrcMask::matchWildString(const QString & szExp, const QString & szStr) const
{
	QString szWildcard;
	QChar * pPtr = (QChar *)szExp.constData();

	if(!pPtr)
		return false;

	while(pPtr->unicode())
	{
		if((pPtr->unicode() == '[') || (pPtr->unicode() == ']'))
		{
			szWildcard.append("[");
			szWildcard.append(*pPtr);
			szWildcard.append("]");
		}
		else
		{
			szWildcard.append(*pPtr);
		}
		pPtr++;
	}
	QRegExp re(szWildcard, Qt::CaseInsensitive, QRegExp::Wildcard);

	return re.exactMatch(szStr);
}

int KviIrcMask::getIpDomainMaskLen() const
{
	int iLen = m_szHost.length();
	const QChar * pChar = m_szHost.unicode();
	const QChar * pChar2 = pChar;
	pChar += iLen;
	if(pChar2 < pChar)
	{
		pChar--;
		while((pChar2 < pChar) && (pChar->unicode() != '.') && (pChar->unicode() != ':'))
			pChar--;
	}
	// 000.000.000.000
	//            pChar
	//
	return (pChar == pChar2) ? 0 : ((pChar - pChar2) + 1);
}

int KviIrcMask::getLargeIpDomainMaskLen() const
{
	int iLen = m_szHost.length();
	const QChar * pChar = m_szHost.unicode();
	const QChar * pChar2 = pChar;
	pChar += iLen;
	if(pChar2 < pChar)
	{
		pChar--;
		while((pChar2 < pChar) && (pChar->unicode() != '.') && (pChar->unicode() != ':'))
			pChar--;
		if(pChar2 < pChar)
		{
			pChar--;
			while((pChar2 < pChar) && (pChar->unicode() != '.') && (pChar->unicode() != ':'))
				pChar--;
		}
	}
	// 000.000.000.000
	//        pChar
	//
	return (pChar == pChar2) ? 0 : ((pChar - pChar2) + 1);
}

QString KviIrcMask::getHostDomainMask() const
{
	int iLen = m_szHost.length();
	const QChar * pChar = m_szHost.constData();
	if(!pChar)
		return QString();
	const QChar * pChar2 = pChar;
	while(pChar->unicode() && pChar->unicode() != '.')
		pChar++;
	QString szRet(pChar, iLen - (pChar - pChar2));
	return szRet;
}

QString KviIrcMask::getLargeHostDomainMask() const
{
	int iLen = m_szHost.length();
	const QChar * pChar = m_szHost.unicode();
	const QChar * pChar2 = pChar;
	pChar += iLen;

	if(pChar2 < pChar)
	{
		pChar--;
		while((pChar2 < pChar) && (pChar->unicode() != '.'))
			pChar--;
		if(pChar2 < pChar)
		{
			pChar--;
			while((pChar2 < pChar) && (pChar->unicode() != '.'))
				pChar--;
		}
	}

	// xyz.klm.abc.host.top
	//            pChar

	QString szRet(pChar, iLen - (pChar - pChar2));
	return szRet;
}

bool KviIrcMask::hasMaskedIp() const
{
	int iLen = m_szHost.length();
	const QChar * pChar = m_szHost.unicode();
	const QChar * pChar2 = pChar;
	if(iLen == 0)
		return false;
	//run to the end
	pChar += iLen;
	const QChar * pChar3 = pChar;
	pChar--;
	while((pChar2 < pChar) && (pChar->unicode() != '.'))
		pChar--;
	return ((pChar3 - pChar) > 4); // at the moment 4 should be enough : the largest top part is "name"
}

bool KviIrcMask::operator==(const KviIrcMask & user)
{
	if(KviQString::equalCI(m_szNick, user.m_szNick))
	{
		if(KviQString::equalCI(m_szUser, user.m_szUser))
		{
			if(KviQString::equalCI(m_szHost, user.m_szHost))
				return true;
		}
	}
	return false;
}

bool KviIrcMask::hasWildNick()
{
	const QChar * pAux = m_szNick.constData();
	if(!pAux)
		return false;
	unsigned short uc;
	while((uc = pAux->unicode()))
	{
		if((uc == '*') || (uc == '?'))
			return true;
		pAux++;
	}
	return false;
}

int KviIrcMask::nonWildChars()
{
	int iCnt = 0;
	const QChar * pAux = m_szNick.constData();
	if(!pAux)
		return 0;
	unsigned short uc;

	while((uc = pAux->unicode()))
	{
		if((uc != '*') && (uc != '?'))
			iCnt++;
		pAux++;
	}

	pAux = m_szUser.constData();
	while((uc = pAux->unicode()))
	{
		if((uc != '*') && (uc != '?'))
			iCnt++;
		pAux++;
	}

	pAux = m_szHost.constData();
	while((uc = pAux->unicode()))
	{
		if((uc != '*') && (uc != '?'))
			iCnt++;
		pAux++;
	}
	return iCnt;
}
