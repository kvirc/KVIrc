//=============================================================================
//
//   File : KviIrcConnectionServerInfo.cpp
//   Creation date : Tue 22 Jun 2004 03:57:32 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "KviIrcConnectionServerInfo.h"

#include <utility>
#include "KviLocale.h"
#include "KviMemory.h"
#include "KviIrcUserDataBase.h"

KviIrcConnectionServerInfo::KviIrcConnectionServerInfo()
{
	// default assumptions
	buildModePrefixTable();
	m_pServInfo = new KviBasicIrcServerInfo(this);
}

KviIrcConnectionServerInfo::~KviIrcConnectionServerInfo()
{
	if(m_pServInfo)
		delete m_pServInfo;
	if(m_pModePrefixTable)
		KviMemory::free(m_pModePrefixTable);
}

bool KviIrcConnectionServerInfo::isSupportedChannelType(QChar c) const
{
	return m_szSupportedChannelTypes.contains(c);
}

void KviIrcConnectionServerInfo::addSupportedCaps(const QString & szCapList)
{
	m_bSupportsCap = true;

	QStringList lTmp = szCapList.split(' ', QString::SkipEmptyParts);
	foreach(QString szCap, lTmp)
	{
		// cap modifiers are:
		//  '-' : disable a capability (should not be present in a LS message...)
		//  '=' : sticky (can't be disabled once enabled)
		//  '~' : needs ack for modification

		if(szCap.length() < 1)
			continue; // shouldn't happen

		switch(szCap[0].unicode())
		{
			case '-':
			case '~':
			case '=':
				szCap.remove(0, 1);
				break;
			default:
				// ok
				break;
		}

		szCap = szCap.toLower();

		if(!m_lSupportedCaps.contains(szCap))
			m_lSupportedCaps.append(szCap);
	}
}

void KviIrcConnectionServerInfo::setSupportedChannelModes(const QString & szSupportedChannelModes)
{
	QStringList szAllModes = szSupportedChannelModes.split(',', QString::KeepEmptyParts);

	if(szAllModes.count() != 4)
	{
		// in this situation, better not to make assumptions about which modes needs a parameter
		return;
	}

	m_szListModes = szAllModes.at(0);
	m_szParameterModes = szAllModes.at(1);
	m_szParameterWhenSetModes = szAllModes.at(2);
	m_szPlainModes = szAllModes.at(3);

	m_szSupportedChannelModes = szSupportedChannelModes;
}

void KviIrcConnectionServerInfo::setSupportedModePrefixes(const QString & szSupportedModePrefixes, const QString & szSupportedModeFlags)
{
	m_szSupportedModeFlags = szSupportedModeFlags;
	m_szSupportedModePrefixes = szSupportedModePrefixes;
	buildModePrefixTable();
}

void KviIrcConnectionServerInfo::buildModePrefixTable()
{
	if(m_pModePrefixTable)
		KviMemory::free(m_pModePrefixTable);
	const QChar * cPrefix = m_szSupportedModePrefixes.constData();
	const QChar * cFlag = m_szSupportedModeFlags.constData();
	if(!cPrefix || !cFlag)
		return; // eh ?

	m_uPrefixes = qMin(m_szSupportedModePrefixes.length(), m_szSupportedModePrefixes.length());
	m_pModePrefixTable = (kvi_u32_t *)KviMemory::allocate(sizeof(kvi_u32_t) * 3 * m_uPrefixes);

	unsigned short uPrefix, uFlag;

	// Pragma: FIXME: The whole mode handling needs rethinking!
	// Alexey: FIXED: rethinked:)
	// Pragma: LOL :DDDD
	unsigned int i = 0;
	while((uPrefix = cPrefix->unicode()) && (uFlag = cFlag->unicode()) && i < m_uPrefixes * 3)
	{
		m_pModePrefixTable[i] = uPrefix;
		m_pModePrefixTable[i + 1] = uFlag;
		switch(uFlag)
		{
			case 'o':
				m_pModePrefixTable[i + 2] = KviIrcUserEntry::Op;
				break;
			case 'v':
				m_pModePrefixTable[i + 2] = KviIrcUserEntry::Voice;
				break;
			case 'h':
				m_pModePrefixTable[i + 2] = KviIrcUserEntry::HalfOp;
				break;
			case 'a':
				m_pModePrefixTable[i + 2] = KviIrcUserEntry::ChanAdmin;
				break;
			case 'u':
				m_pModePrefixTable[i + 2] = KviIrcUserEntry::UserOp;
				break;
			case 'q':
				m_pModePrefixTable[i + 2] = KviIrcUserEntry::ChanOwner;
				break;
			default:
				m_pModePrefixTable[i + 2] = 0;
				break;
		}
		i += 3;
		cPrefix++;
		cFlag++;
	}
}

bool KviIrcConnectionServerInfo::isSupportedModePrefix(QChar c) const
{
	if(!m_pModePrefixTable)
		return false;
	for(unsigned int i = 0; i < m_uPrefixes; i++)
	{
		if(m_pModePrefixTable[i * 3] == c.unicode())
			return true;
	}
	return false;
}

bool KviIrcConnectionServerInfo::isSupportedModeFlag(QChar c) const
{
	if(!m_pModePrefixTable)
		return false;
	for(unsigned int i = 0; i < m_uPrefixes; i++)
	{
		if(m_pModePrefixTable[(i * 3) + 1] == c.unicode())
			return true;
	}
	return false;
}

QChar KviIrcConnectionServerInfo::modePrefixChar(kvi_u32_t flag) const
{
	if(!m_pModePrefixTable)
		return { 0 };
	for(unsigned int i = 0; i < m_uPrefixes; i++)
	{
		if(m_pModePrefixTable[i * 3 + 2] & flag)
			return QChar(m_pModePrefixTable[i * 3]);
	}
	return QChar(0);
}

QChar KviIrcConnectionServerInfo::modeFlagChar(kvi_u32_t flag) const
{
	if(!m_pModePrefixTable)
		return { 0 };
	for(unsigned int i = 0; i < m_uPrefixes; i++)
	{
		if(m_pModePrefixTable[i * 3 + 2] & flag)
			return QChar(m_pModePrefixTable[i * 3 + 1]);
	}
	return QChar(0);
}

kvi_u32_t KviIrcConnectionServerInfo::modeFlagFromPrefixChar(QChar c) const
{
	if(!m_pModePrefixTable)
		return 0;
	for(unsigned int i = 0; i < m_uPrefixes; i++)
	{
		if(m_pModePrefixTable[i * 3] == c.unicode())
			return m_pModePrefixTable[i * 3 + 2];
	}
	return 0;
}

kvi_u32_t KviIrcConnectionServerInfo::modeFlagFromModeChar(QChar c) const
{
	if(!m_pModePrefixTable)
		return 0;
	for(unsigned int i = 0; i < m_uPrefixes; i++)
	{
		if(m_pModePrefixTable[i * 3 + 1] == c.unicode())
			return m_pModePrefixTable[i * 3 + 2];
	}
	return 0;
}

void KviIrcConnectionServerInfo::setServerVersion(const QString & version)
{
	if(m_pServInfo)
		delete m_pServInfo;
	if(version.contains("unrealircd-4", Qt::CaseInsensitive))
		m_pServInfo = new KviUnreal40IrcServerInfo(this, version);
	else if(version.contains("unreal3.2", Qt::CaseInsensitive))
		m_pServInfo = new KviUnreal32IrcServerInfo(this, version);
	else if(version.contains("unreal", Qt::CaseInsensitive))
		m_pServInfo = new KviUnrealIrcServerInfo(this, version);
	else if(version.contains("charybdis", Qt::CaseInsensitive))
		m_pServInfo = new KviCharybdisServerInfo(this, version);
	else if(version.contains("bahamut", Qt::CaseInsensitive))
		m_pServInfo = new KviBahamutIrcServerInfo(this, version);
	else if(version.contains("hyperion", Qt::CaseInsensitive))
		m_pServInfo = new KviHyperionIrcServerInfo(this, version);
	else if(version.contains("ircd-seven", Qt::CaseInsensitive))
		m_pServInfo = new KviIrcdSevenIrcServerInfo(this, version);
	else if(version.contains("ratbox", Qt::CaseInsensitive))
		m_pServInfo = new KviIrcdRatboxIrcServerInfo(this, version);
	else if(version.contains("inspircd", Qt::CaseInsensitive))
		m_pServInfo = new KviInspIRCdIrcServerInfo(this, version);
	else if(version.contains("snircd", Qt::CaseInsensitive))
		m_pServInfo = new KviSnircdIrcServerInfo(this, version);
	else if(version.contains("ircd-darenet", Qt::CaseInsensitive))
		m_pServInfo = new KviDarenetIrcServerInfo(this, version);
	else if(version.contains("u2", Qt::CaseInsensitive))
		m_pServInfo = new KviIrcuIrcServerInfo(this, version);
	else if(version.contains("plexus", Qt::CaseInsensitive))
		m_pServInfo = new KviPlexusIrcServerInfo(this, version);
	else if(version.contains("criten", Qt::CaseInsensitive))
		m_pServInfo = new KviCritenIrcServerInfo(this, version);
	else if(version.contains("nemesis2.0", Qt::CaseInsensitive))
		m_pServInfo = new KviNemesis20IrcServerInfo(this, version);
	else if(version.contains("nemesis", Qt::CaseInsensitive))
		m_pServInfo = new KviNemesisIrcServerInfo(this, version);
	else if(version.contains("oftc", Qt::CaseInsensitive))
		m_pServInfo = new KviOftcIrcServerInfo(this, version);
	else if(version.contains("hybrid", Qt::CaseInsensitive))
		m_pServInfo = new KviHybridServerInfo(this, version);
	else
		m_pServInfo = new KviBasicIrcServerInfo(this, version);
}

KviBasicIrcServerInfo::KviBasicIrcServerInfo(KviIrcConnectionServerInfo * pParent, QString version)
    : m_szServerVersion(std::move(version)), m_pParent(pParent)
{
}

KviBasicIrcServerInfo::~KviBasicIrcServerInfo()
    = default;

//
// User modes
//

const QString & KviBasicIrcServerInfo::getUserModeDescription(QChar mode) const
{
	switch(mode.unicode())
	{
		case 'O':
			return __tr2qs("O: Local IRC Operator (locop)");
			break;
		case 'a':
			return __tr2qs("a: Server administrator");
			break;
		case 'c':
			return __tr2qs("c: Recipient for cconn messages");
			break;
		case 'd':
			return __tr2qs("d: Recipient for server debug notices");
			break;
		case 'f':
			return __tr2qs("f: Recipient for full server notices");
			break;
		case 'i':
			return __tr2qs("i: Invisible");
			break;
		case 'k':
			return __tr2qs("k: Recipient for server KILL messages");
			break;
		case 'n':
			return __tr2qs("n: Recipient for nick changes");
			break;
		case 'o':
			return __tr2qs("o: IRC Operator (oper)");
			break;
		case 'r':
			return __tr2qs("r: User with restricted connection (or recipient for messages about rejected bots)");
			break;
		case 's':
			return __tr2qs("s: Recipient for server notices");
			break;
		case 'w':
			return __tr2qs("w: Recipient for WALLOPS messages");
			break;
		case 'y':
			return __tr2qs("y: Recipient for spy notices");
			break;
		case 'z':
			return __tr2qs("z: Recipient for oper WALLOP messages");
			break;
	}
	return KviQString::Empty;
}

const QString & KviHybridServerInfo::getUserModeDescription(QChar mode) const
{
	switch(mode.unicode())
	{
		case 'D':
			return __tr2qs("D: Deaf");
			break;
		case 'F':
			return __tr2qs("F: Recipient for far connect/quit notices");
			break;
		case 'G':
			return __tr2qs("G: Only accept private messages from users in common channels");
			break;
		case 'H':
			return __tr2qs("H: Hide oper status");
			break;
		case 'R':
			return __tr2qs("R: Only receive private messages from registered nicks");
			break;
		case 'S':
			return __tr2qs("S: Connected over SSL");
			break;
		case 'W':
			return __tr2qs("W: Connected over WEBIRC");
			break;
		case 'a':
			return __tr2qs("a: Server administrator");
			break;
		case 'b':
			return __tr2qs("b: Recipient for bot/join flood warnings");
			break;
		case 'e':
			return __tr2qs("e: Recipient for server introduction and split notices");
			break;
		case 'f':
			return __tr2qs("f: Recipient for full I-Line notices");
			break;
		case 'g':
			return __tr2qs("g: Only allow accepted clients to message you");
			break;
		case 'j':
			return __tr2qs("j: Recipient for rejected client notices");
			break;
		case 'l':
			return __tr2qs("l: Recipient for LOCOPS");
			break;
		case 'p':
			return __tr2qs("p: Channels hidden from WHOIS");
			break;
		case 'q':
			return __tr2qs("q: Idle time hidden from WHOIS");
			break;
		case 'r':
			return __tr2qs("r: Registered");
			break;
		case 'u':
			return __tr2qs("u: Recipient for unauthorized client notices");
			break;
		case 'x':
			return __tr2qs("x: Host hidden");
			break;
		case 'y':
			return __tr2qs("y: Can see stats/links/admin requests");
			break;
	}
	return KviBasicIrcServerInfo::getUserModeDescription(mode);
}

const QString & KviIrcdRatboxIrcServerInfo::getUserModeDescription(QChar mode) const
{
	switch(mode.unicode())
	{
		case 'C':
			return __tr2qs("C: Prevent CTCPs");
			break;
		case 'D':
			return __tr2qs("D: Deaf");
			break;
		case 'Z':
			return __tr2qs("Z: Recipient for oper spy notices");
			break;
		case 'a':
			return __tr2qs("a: Server administrator");
			break;
		case 'b':
			return __tr2qs("b: Recipient for bot/join flood warnings");
			break;
		case 'f':
			return __tr2qs("f: Recipient for full I-Line notices");
			break;
		case 'g':
			return __tr2qs("g: Only allow accepted clients to message you");
			break;
		case 'l':
			return __tr2qs("l: Recipient for LOCOPS");
			break;
		case 'r':
			return __tr2qs("r: Recipient for rejected client notices");
			break;
		case 'u':
			return __tr2qs("u: Recipient for unauthorised client notices");
			break;
		case 'x':
			return __tr2qs("x: Recipient for remote server connection and split notices");
			break;
		case 'z':
			return __tr2qs("z: Recipient for OPERWALL messages");
			break;
	}
	return KviHybridServerInfo::getUserModeDescription(mode);
}

const QString & KviCharybdisServerInfo::getUserModeDescription(QChar mode) const
{
	switch(mode.unicode())
	{
		case 'Q':
			return __tr2qs("Q: Prevents you from being affected by channel forwarding");
			break;
		case 'R':
			return __tr2qs("R: Only receive private messages from registered nicks");
			break;
		case 'S':
			return __tr2qs("S: Network service");
			break;
		case 'Z':
			return __tr2qs("Z: Connected over SSL");
			break;
		case 'h':
			return __tr2qs("h: Host hidden");
			break;
		case 'p':
			return __tr2qs("p: Enable oper overrides");
			break;
		case 'x':
			return __tr2qs("x: Host hidden");
			break;
	}
	return KviIrcdRatboxIrcServerInfo::getUserModeDescription(mode);
}

const QString & KviIrcdSevenIrcServerInfo::getUserModeDescription(QChar mode) const
{
	switch(mode.unicode())
	{
		case 'O':
			return __tr2qs("O: IRC Help Operator");
			break;
		case 'h':
			return __tr2qs("h: Marks you as a helper in /stats p");
			break;
	}
	return KviCharybdisServerInfo::getUserModeDescription(mode);
}

const QString & KviPlexusIrcServerInfo::getUserModeDescription(QChar mode) const
{
	switch(mode.unicode())
	{
		case 'C':
			return __tr2qs("C: Prevent CTCPs");
			break;
		case 'N':
			return __tr2qs("N: Network administrator");
			break;
		case 'U':
			return __tr2qs("U: Network service");
			break;
		case 'X':
			return __tr2qs("X: Recipient for new server introduction and split messages");
			break;
		case 'w':
			return __tr2qs("w: Recipient for server WALLOPS");
			break;
		case 'z':
			return __tr2qs("z: Recipient for oper WALLOPS");
			break;
		case 'y':
			return __tr2qs("y: Recipient for notices on WHOIS");
			break;
		case 'a':
			return __tr2qs("a: Server administrator");
			break;
		case 'q':
			return __tr2qs("q: Services administrator, can use SQUIT");
			break;
	}
	return KviHybridServerInfo::getUserModeDescription(mode);
}

const QString & KviOftcIrcServerInfo::getUserModeDescription(QChar mode) const
{
	switch(mode.unicode())
	{
		case 'C':
			return __tr2qs("C: Recipient for far connect/quit notices");
			break;
		case 'P':
			return __tr2qs("P: Network service");
			break;
		case 'w':
			return __tr2qs("w: Recipient for server WALLOPS");
			break;
		case 'z':
			return __tr2qs("z: Recipient for oper WALLOPS");
			break;
	}
	return KviHybridServerInfo::getUserModeDescription(mode);
}

const QString & KviIrcuIrcServerInfo::getUserModeDescription(QChar mode) const
{
	switch(mode.unicode())
	{
		case 'I':
			return __tr2qs("I: Idle time hidden");
			break;
		case 'd':
			return __tr2qs("d: Deaf");
			break;
		case 'g':
			return __tr2qs("g: Recipient for server debug notices");
			break;
		case 'k':
			return __tr2qs("k: Network service");
			break;
		case 'n':
			return __tr2qs("n: Channels hidden from WHOIS");
			break;
		case 'r':
			return __tr2qs("r: Registered");
			break;
		case 'x':
			return __tr2qs("x: Host hidden");
			break;
	}
	return KviBasicIrcServerInfo::getUserModeDescription(mode);
}

const QString & KviSnircdIrcServerInfo::getUserModeDescription(QChar mode) const
{
	switch(mode.unicode())
	{
		case 'P':
			return __tr2qs("P: Recipient for notices on WHOIS");
			break;
		case 'R':
			return __tr2qs("R: Only receive private messages from registered nicks");
			break;
		case 'X':
			return __tr2qs("X: Special powers");
			break;
		case 'h':
			return __tr2qs("h: Host changed");
			break;
	}
	return KviIrcuIrcServerInfo::getUserModeDescription(mode);
}

const QString & KviDarenetIrcServerInfo::getUserModeDescription(QChar mode) const
{
	switch(mode.unicode())
	{
		case 'F':
			return __tr2qs("F: Disable fake lag");
			break;
		case 'H':
			return __tr2qs("H: Hide oper status");
			break;
		case 'N':
			return __tr2qs("N: Network administrator");
			break;
		case 'P':
			return __tr2qs("P: Recipient for notices on WHOIS");
			break;
		case 'R':
			return __tr2qs("R: Only receive private messages from registered nicks");
			break;
		case 'X':
			return __tr2qs("X: Special powers");
			break;
		case 'a':
			return __tr2qs("a: Server administrator");
			break;
	}
	return KviIrcuIrcServerInfo::getUserModeDescription(mode);
}

const QString & KviUnreal32IrcServerInfo::getUserModeDescription(QChar mode) const
{
	switch(mode.unicode())
	{
		case 'A':
			return __tr2qs("A: Server administrator");
			break;
		case 'B':
			return __tr2qs("B: Marks as being a bot");
			break;
		case 'C':
			return __tr2qs("C: Co-Admin");
			break;
		case 'F':
			return __tr2qs("F: Override filter settings");
			break;
		case 'G':
			return __tr2qs("G: Filter out bad words");
			break;
		case 'H':
			return __tr2qs("H: Hide oper status");
			break;
		case 'I':
			return __tr2qs("I: Idle time hidden from WHOIS");
			break;
		case 'N':
			return __tr2qs("N: Network administrator");
			break;
		case 'R':
			return __tr2qs("R: Only receive private messages from registered nicks");
			break;
		case 'S':
			return __tr2qs("S: Network service");
			break;
		case 'T':
			return __tr2qs("T: Prevent CTCPs");
			break;
		case 'V':
			return __tr2qs("V: Connected over WebTV");
			break;
		case 'W':
			return __tr2qs("W: Recipient for notices on WHOIS");
			break;
		case 'a':
			return __tr2qs("a: Services administrator");
			break;
		case 'd':
			return __tr2qs("d: Deaf");
			break;
		case 'g':
			return __tr2qs("g: Can send & read LOCOPS and GLOBOPS");
			break;
		case 'h':
			return __tr2qs("h: Available for help (helpop)");
			break;
		case 'p':
			return __tr2qs("p: Channels hidden from WHOIS");
			break;
		case 'q':
			return __tr2qs("q: Immune to kicks (except U-Line)");
			break;
		case 'r':
			return __tr2qs("r: Registered");
			break;
		case 't':
			return __tr2qs("t: Using a vhost");
			break;
		case 'v':
			return __tr2qs("v: Recipient for infected DCC SEND rejection notices");
			break;
		case 'x':
			return __tr2qs("x: Host hidden");
			break;
		case 'z':
			return __tr2qs("z: Connected over SSL");
			break;
	}
	return KviBasicIrcServerInfo::getUserModeDescription(mode);
}

const QString & KviUnreal40IrcServerInfo::getUserModeDescription(QChar mode) const
{
	switch(mode.unicode())
	{
		case 'D':
			return __tr2qs("D: Only receive private messages from opers, servers, or services");
			break;
		case 'Z':
			return __tr2qs("Z: Only receive private messages from users with SSL");
			break;
	}
	return KviUnreal32IrcServerInfo::getUserModeDescription(mode);
}

const QString & KviCritenIrcServerInfo::getUserModeDescription(QChar mode) const
{
	switch(mode.unicode())
	{
		case 'C':
			return __tr2qs("C: Can see CHATOPS notices");
			break;
		case 'D':
			return __tr2qs("D: Has seen DCCALLOW warning message");
			break;
		case 'F':
			return __tr2qs("F: Recipient for far connect/quit notices");
			break;
		case 'P':
			return __tr2qs("P: Services administrator");
			break;
		case 'S':
			return __tr2qs("S: Network service");
			break;
		case 'W':
			return __tr2qs("W: Recipient for notices on WHOIS");
			break;
		case 'Z':
			return __tr2qs("Z: Services root administrator");
			break;
		case 'a':
			return __tr2qs("a: Services operator");
			break;
		case 'e':
			return __tr2qs("e: Recipient for blocked DCC notices");
			break;
		case 'f':
			return __tr2qs("f: Recipient for excess flood notices");
			break;
		case 'g':
			return __tr2qs("g: Recipient for GLOBOPS notices");
			break;
		case 'h':
			return __tr2qs("h: Available for help (helpop)");
			break;
		case 'm':
			return __tr2qs("m: Recipient for spam warning notices");
			break;
		case 'n':
			return __tr2qs("n: Recipient for NETINFO and routing notices");
			break;
		case 'p':
			return __tr2qs("p: Protected IRC operator");
			break;
		case 'r':
			return __tr2qs("r: Registered");
			break;
		case 'x':
			return __tr2qs("x: Host hidden");
			break;
	}
	return KviBasicIrcServerInfo::getUserModeDescription(mode);
}

const QString & KviBahamutIrcServerInfo::getUserModeDescription(QChar mode) const
{
	switch(mode.unicode())
	{
		case 'A':
			return __tr2qs("A: Server administrator");
			break;
		case 'C':
			return __tr2qs("C: Only accept private messages from users in common channels");
			break;
		case 'F':
			return __tr2qs("F: Can bypass the IRCd's recvq throttling");
			break;
		case 'I':
			return __tr2qs("I: Hide oper status");
			break;
		case 'K':
			return __tr2qs("K: Can see U:lined kill messages");
			break;
		case 'R':
			return __tr2qs("R: Only receive private messages from registered nicks");
			break;
		case 'S':
			return __tr2qs("S: Connected over SSL");
			break;
		case 'X':
			return __tr2qs("X: Squelch without notice");
			break;
		case 'a':
			return __tr2qs("a: Services administrator");
			break;
		case 'b':
			return __tr2qs("b: Can see CHATOPS notices");
			break;
		case 'e':
			return __tr2qs("e: Recipient for blocked DCC notices");
			break;
		case 'f':
			return __tr2qs("f: Recipient for flood warnings");
			break;
		case 'g':
			return __tr2qs("g: Recipient for GLOBOPS notices");
			break;
		case 'h':
			return __tr2qs("h: Available for help (helpop)");
			break;
		case 'j':
			return __tr2qs("j: Recipient for rejected client notices");
			break;
		case 'm':
			return __tr2qs("m: Recipient for spambot notices");
			break;
		case 'n':
			return __tr2qs("n: Recipient for routing notices");
			break;
		case 'r':
			return __tr2qs("r: Registered");
			break;
		case 's':
			return __tr2qs("s: Recipient for server KILL messages");
			break;
		case 'x':
			return __tr2qs("x: Squelch with notice");
			break;
		case 'y':
			return __tr2qs("y: Can see certain information requests (e.g. /stats)");
			break;
	}
	return KviBasicIrcServerInfo::getUserModeDescription(mode);
}

const QString & KviHyperionIrcServerInfo::getUserModeDescription(QChar mode) const
{
	switch(mode.unicode())
	{
		case 'A':
			return __tr2qs("A: Can see all servers and IP addresses");
			break;
		case 'B':
			return __tr2qs("B: Allows the use of SETHOST, SETNAME, and SETIDENT commands on other people");
			break;
		case 'C':
			return __tr2qs("C: Prevent CTCPs");
			break;
		case 'D':
			return __tr2qs("D: Allows the use of DIE and RESTART");
			break;
		case 'E':
			return __tr2qs("E: Only receive private messages from registered nicks");
			break;
		case 'F':
			return __tr2qs("F: Immune to flood protection");
			break;
		case 'G':
			return __tr2qs("G: Allows the use of KILL for users on remote servers");
			break;
		case 'H':
			return __tr2qs("H: Allows the use of REHASH");
			break;
		case 'I':
			return __tr2qs("I: Prevents you from receiving INVITEs");
			break;
		case 'K':
			return __tr2qs("K: Allows the use of KLINE");
			break;
		case 'L':
			return __tr2qs("L: Allows the use of 4-argument LUSERS to force a recount");
			break;
		case 'M':
			return __tr2qs("M: Allows the use of NOTICE based on mask");
			break;
		case 'N':
			return __tr2qs("N: Can override Q-Lines");
			break;
		case 'P':
			return __tr2qs("P: Allows the use of SETHOST, SETIDENT and SETNAME");
			break;
		case 'Q':
			return __tr2qs("Q: Prevents you from being affected by channel forwarding");
			break;
		case 'R':
			return __tr2qs("R: Allows the use of CONNECT, SQUIT, and HTM");
			break;
		case 'S':
			return __tr2qs("S: Allows the use of remotely running server commands");
			break;
		case 'T':
			return __tr2qs("T: Allows you to appear in /stats p");
			break;
		case 'U':
			return __tr2qs("U: Allows the use of UNKLINE");
			break;
		case 'V':
			return __tr2qs("V: Allows the use of MAP, LINKS, as well as receiving connection information");
		case 'W':
			return __tr2qs("W: Allows the ability to send WALLOPS");
			break;
		case 'X':
			return __tr2qs("X: Grants access to experimental features");
			break;
		case 'Y':
			return __tr2qs("Y: Displays extra information during netjoins");
			break;
		case 'Z':
			return __tr2qs("Z: Allows the ability to send OPERWALL messages");
			break;
		case 'a':
			return __tr2qs("a: Can see all users and channels");
			break;
		case 'b':
			return __tr2qs("b: Recipient for spam warning notices");
			break;
		case 'c':
			return __tr2qs("c: Recipient for connect and netjoin notices");
			break;
		case 'e':
			return __tr2qs("e: Registered");
			break;
		case 'f':
			return __tr2qs("f: Recipient for full I-Line notices");
			break;
		case 'h':
			return __tr2qs("h: Sets user in high priority mode");
			break;
		case 'j':
			return __tr2qs("j: Allows the use of auto D-Lines");
			break;
		case 'l':
			return __tr2qs("l: Recipient for new channel creation notices");
			break;
		case 'm':
			return __tr2qs("m: Can't be KICKed, DEOPed, or D-Lined");
			break;
		case 'p':
			return __tr2qs("p: Allows implicit access in all channels, regardless of status");
			break;
		case 'r':
			return __tr2qs("r: Recipient for notices on users attempting to use invalid nicks, Q-Lined or X-Lined nicks");
			break;
		case 't':
			return __tr2qs("t: Allows the use of channel logging");
			break;
		case 'u':
			return __tr2qs("u: Allows the ability to join more channels than the default IRCd limit");
			break;
		case 'v':
			return __tr2qs("v: Allows the ability to view oper privileges via WHOIS");
			break;
		case 'x':
			return __tr2qs("x: Allows the ability to see netjoins");
			break;
		case 'z':
			return __tr2qs("z: Recipient for OPERWALL messages");
			break;
		case '0':
			return __tr2qs("0: Allows the ability to view oper information");
			break;
		case '1':
			return __tr2qs("1: Allows the ability to view I-Lines and Y-Lines");
			break;
		case '2':
			return __tr2qs("2: Allows the ability to view D-Lines and K-Lines");
			break;
		case '3':
			return __tr2qs("3: Allows the ability to view Q-Lines and X-Lines");
			break;
		case '4':
			return __tr2qs("4: Allows the ability to use STATS T");
			break;
		case '5':
			return __tr2qs("5: Allows the ability to use STATS ?");
			break;
		case '9':
			return __tr2qs("9: Allows the use of TESTLINE");
			break;
		case '*':
			return __tr2qs("*: Allows the user to grant umodes they have access to to other people");
			break;
		case '@':
			return __tr2qs("@: allows the user to change their host to anything with the SETHOST command");
			break;
	}
	return KviBasicIrcServerInfo::getUserModeDescription(mode);
}

const QString & KviInspIRCdIrcServerInfo::getUserModeDescription(QChar mode) const
{
	switch(mode.unicode())
	{
		case 'c':
			return __tr2qs("c: Prevent CTCPs");
			break;
		case 'd':
			return __tr2qs("d: Deaf");
			break;
		case 'g':
			return __tr2qs("g: Server side ignore");
			break;
		case 'h':
			return __tr2qs("h: Available for help (helpop)");
			break;
		case 'k':
			return __tr2qs("k: Network service");
			break;
		case 'r':
			return __tr2qs("r: Registered");
			break;
		case 'x':
			return __tr2qs("x: Host hidden");
			break;
		case 'B':
			return __tr2qs("B: Marks as being a bot");
			break;
		case 'G':
			return __tr2qs("G: Censor bad words");
			break;
		case 'H':
			return __tr2qs("H: Hide oper status");
			break;
		case 'I':
			return __tr2qs("I: Channels hidden from WHOIS");
			break;
		case 'Q':
			return __tr2qs("Q: Marks an IRC operator as invisible from user lists");
			break;
		case 'R':
			return __tr2qs("R: Only receive private messages from registered nicks");
			break;
		case 'S':
			return __tr2qs("S: Strip colors out of private messages");
			break;
		case 'W':
			return __tr2qs("W: Recipient for notices on WHOIS");
			break;
	}
	return KviBasicIrcServerInfo::getUserModeDescription(mode);
}

//
// UMODE Requirements
//

// Cases returning themselves (case 'o': return 'o';) are modes
// that can be obtained by IRCd commands (/OPER) yet can be removed
// by the user. e.g. you can obtain usermode +o by running /OPER
// yet can remove it (de-oper) if you have it.
//
// Cases returning a mode different from their case are modes that
// are dependent on the modes being returned. For example, the function
// receives 'y' which could be, for example, routing requests. We would
// return 'o', as 'y' requires operator privileges and the operator mode
// is 'o'. We are then basically saying that 'y' requires 'o', so return
// 'o' for further parsing. If they do not, then we know they are not an
// IRC Operator and are not allowed to set mode 'y'.
//
// Cases returning a ! mean that the mode cannot be set by the user in
// any way, shape, or form. It is strictly IRCd set and can not be set
// or unset by the user. An example of this is 'S' for 'Connected over
// SSL'. This is determined at connection time and set by the IRCd if
// you are using SSL. There is no way to magically switch to SSL after
// you've made the connection. Thus, the mode can never be set or unset
// by the user.
//
// Cases returning QChar::Null are free to set by the user without restrictions.
QChar KviBasicIrcServerInfo::getUserModeRequirement(QChar mode) const
{
	switch(mode.unicode())
	{
		// These cases are for modes that can be obtained by IRCd
		// commands (/OPER) and in no other way. However, they can
		// be unset if the user has these modes. (i.e. de-opering)
		case 'O':
			return 'O';
		case 'a':
			return 'a';

		// These modes require operator privileges to set. We return
		// 'o' to see if the user is an oper.
		case 'c':
		case 'd':
		case 'f':
		case 'k':
		case 'n':
		case 'o':
		case 's':
		case 'y':
		case 'z':
			return 'o';
	}
	// No restriction, mode is free to set.
	return QChar::Null;
}

QChar KviHybridServerInfo::getUserModeRequirement(QChar mode) const
{
	switch(mode.unicode())
	{
		case 'a':
			return 'a';

		case 'S':
		case 'W':
		case 'r':
		case 'x':
			return '!';

		case 'F':
		case 'H':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'j':
		case 'k':
		case 'l':
		case 'n':
		case 'o':
		case 's':
		case 'u':
		case 'y':
			return 'o';
	}
	return QChar::Null;
}

QChar KviIrcdRatboxIrcServerInfo::getUserModeRequirement(QChar mode) const
{
	switch(mode.unicode())
	{
		case 'a':
			return 'a';

		case 'Z':
		case 'b':
		case 'c':
		case 'd':
		case 'f':
		case 'k':
		case 'l':
		case 'n':
		case 'o':
		case 'r':
		case 'u':
		case 'x':
		case 'y':
		case 'z':
			return 'o';
	}
	return QChar::Null;
}

QChar KviCharybdisServerInfo::getUserModeRequirement(QChar mode) const
{
	switch(mode.unicode())
	{
		case 'a':
			return 'a';

		case 'S':
		case 'Z':
			return '!';

		case 'l':
		case 'o':
		case 'p':
		case 's':
		case 'z':
			return 'o';
	}
	return QChar::Null;
}

QChar KviIrcdSevenIrcServerInfo::getUserModeRequirement(QChar mode) const
{
	switch(mode.unicode())
	{
		case 'O':
			return 'O';
		case 'a':
			return 'a';

		case 'S':
		case 'Z':
			return '!';

		case 'h':
		case 'l':
		case 'o':
		case 'p':
		case 'z':
			return 'o';
	}
	return QChar::Null;
}

QChar KviPlexusIrcServerInfo::getUserModeRequirement(QChar mode) const
{
	switch(mode.unicode())
	{
		case 'N':
			return 'N';
		case 'a':
			return 'a';
		case 'q':
			return 'q';

		case 'S':
		case 'U':
		case 'W':
		case 'r':
			return '!';

		case 'F':
		case 'X':
		case 'b':
		case 'c':
		case 'd':
		case 'f':
		case 'j':
		case 'k':
		case 'l':
		case 'n':
		case 'o':
		case 'u':
		case 'y':
		case 'z':
			return 'o';
	}
	return QChar::Null;
}

QChar KviOftcIrcServerInfo::getUserModeRequirement(QChar mode) const
{
	switch(mode.unicode())
	{
		case 'a':
			return 'a';

		case 'P':
		case 'S':
			return '!';

		case 'C':
		case 'b':
		case 'c':
		case 'd':
		case 'f':
		case 'k':
		case 'l':
		case 'n':
		case 'o':
		case 'r':
		case 'u':
		case 'y':
		case 'x':
		case 'z':
			return 'o';
	}
	return QChar::Null;
}

QChar KviIrcuIrcServerInfo::getUserModeRequirement(QChar mode) const
{
	switch(mode.unicode())
	{
		case 'O':
			return 'O';

		case 'k':
		case 'r':
		case 'x':
			return '!';

		case 'I':
		case 'g':
		case 'n':
		case 'o':
			return 'o';
	}
	return QChar::Null;
}

QChar KviSnircdIrcServerInfo::getUserModeRequirement(QChar mode) const
{
	switch(mode.unicode())
	{
		case 'O':
			return 'O';

		case 'k':
		case 'r':
		case 'x':
			return '!';

		case 'I':
		case 'P':
		case 'X':
		case 'g':
		case 'n':
		case 'o':
			return 'o';
	}
	return QChar::Null;
}

QChar KviDarenetIrcServerInfo::getUserModeRequirement(QChar mode) const
{
	switch(mode.unicode())
	{
		case 'N':
			return 'N';
		case 'O':
			return 'O';
		case 'a':
			return 'a';

		case 'k':
		case 'x':
			return '!';

		case 'F':
		case 'H':
		case 'I':
		case 'P':
		case 'X':
		case 'g':
		case 'n':
		case 'o':
		case 'z':
			return 'o';
	}
	return QChar::Null;
}

QChar KviUnreal32IrcServerInfo::getUserModeRequirement(QChar mode) const
{
	switch(mode.unicode())
	{
		case 'A':
			return 'A';
		case 'C':
			return 'C';
		case 'N':
			return 'N';
		case 'O':
			return 'O';
		case 'a':
			return 'a';
		case 'r':
			return 'r';
		case 't':
			return 't';

		// Modes that cannot be set by the user
		case 'S':
		case 'V':
		case 'x':
		case 'z':
			return '!';

		// Modes requiring oper (o)
		case 'F':
		case 'H':
		case 'I':
		case 'W':
		case 'f':
		case 'g':
		case 'h':
		case 'o':
		case 's':
		case 'v':
		case 'w':
			return 'o';

		// 'q': Only U:Lines can kick you (Services Admins Only)
		// Requires 'a': Services Admin
		case 'q':
			return 'a';
	}
	return QChar::Null;
}

QChar KviCritenIrcServerInfo::getUserModeRequirement(QChar mode) const
{
	switch(mode.unicode())
	{
		case 'O':
			return 'O';

		case 'P':
		case 'S':
		case 'Z':
		case 'a':
		case 'r':
		case 'x':
			return '!';

		case 'c':
		case 'C':
		case 'd':
		case 'e':
		case 'f':
		case 'F':
		case 'g':
		case 'm':
		case 'n':
		case 'p':
		case 'W':
		case 'y':
		case 'h':
		case 'o':
			return 'o';
	}
	return QChar::Null;
}

QChar KviBahamutIrcServerInfo::getUserModeRequirement(QChar mode) const
{
	switch(mode.unicode())
	{
		case 'A':
			return 'A';
		case 'O':
			return 'O';

		case 'S':
		case 'W':
		case 'X':
		case 'a':
		case 'r':
		case 'w':
		case 'x':
			return '!';

		case 'F':
		case 'I':
		case 'K':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case 'h':
		case 'j':
		case 'k':
		case 'm':
		case 'n':
		case 'o':
		case 'y':
			return 'o';
	}
	return QChar::Null;
}

QChar KviHyperionIrcServerInfo::getUserModeRequirement(QChar mode) const
{
	switch(mode.unicode())
	{
		case 'e':
			return '!';
		case 'A':
		case 'B':
		case 'D':
		case 'F':
		case 'G':
		case 'K':
		case 'L':
		case 'M':
		case 'N':
		case 'P':
		case 'R':
		case 'S':
		case 'T':
		case 'U':
		case 'V':
		case 'W':
		case 'X':
		case 'Y':
		case 'Z':
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'f':
		case 'h':
		case 'H':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'n':
		case 'o':
		case 'p':
		case 'r':
		case 'u':
		case 'v':
		case 'x':
		case 'y':
		case 'z':
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '9':
		case '*':
		case '@':
			return 'o';
	}
	return QChar::Null;
}

QChar KviInspIRCdIrcServerInfo::getUserModeRequirement(QChar mode) const
{
	switch(mode.unicode())
	{
		case 'k':
		case 'r':
		case 'x':
			return '!';

		case 'H':
		case 'Q':
		case 'W':
		case 'h':
		case 'o':
			return 'o';
	}
	return QChar::Null;
}

//
// Channel modes
//

const QString & KviBasicIrcServerInfo::getChannelModeDescription(char mode) const
{
	switch(mode)
	{
		case 'I':
			return __tr2qs("Invite exceptions");
			break;
		case 'b':
			return __tr2qs("Ban masks");
			break;
		case 'e':
			return __tr2qs("Ban exceptions");
			break;
		case 'i':
			return __tr2qs("Invite only");
			break;
		case 'k':
			return __tr2qs("Key");
			break;
		case 'l':
			return __tr2qs("Limited number of users");
			break;
		case 'm':
			return __tr2qs("Moderated");
			break;
		case 'n':
			return __tr2qs("No external messages");
			break;
		case 'o':
			return __tr2qs("Channel operators");
			break;
		case 'p':
			return __tr2qs("Private");
			break;
		case 's':
			return __tr2qs("Secret");
			break;
		case 't':
			return __tr2qs("Topic change restricted");
			break;
		case 'v':
			return __tr2qs("Voiced users");
			break;
	}
	return KviQString::Empty;
}

const QString & KviHybridServerInfo::getChannelModeDescription(char mode) const
{
	switch(mode)
	{
		case 'C':
			return __tr2qs("Forbid channel CTCPs");
			break;
		case 'M':
			return __tr2qs("Moderate non auth users");
			break;
		case 'O':
			return __tr2qs("IRC-Op only channel");
			break;
		case 'R':
			return __tr2qs("Only registered nicks can join");
			break;
		case 'S':
			return __tr2qs("Need SSL connection to join");
			break;
		case 'T':
			return __tr2qs("Forbid channel NOTICEs");
			break;
		case 'c':
			return __tr2qs("No control codes (colors, bold, ..)");
			break;
		case 'h':
			return __tr2qs("Half-operators");
			break;
		case 'r':
			return __tr2qs("Registered");
			break;
	}
	return KviBasicIrcServerInfo::getChannelModeDescription(mode);
}

const QString & KviIrcdRatboxIrcServerInfo::getChannelModeDescription(char mode) const
{
	switch(mode)
	{
		// not present of efnet, but supported by ratbox
		case 'r':
			return __tr2qs("Only registered nicks can join");
			break;
	}
	return KviHybridServerInfo::getChannelModeDescription(mode);
}

const QString & KviCharybdisServerInfo::getChannelModeDescription(char mode) const
{
	switch(mode)
	{
		case 'A':
			return __tr2qs("Server administrator only channel");
			break;
		case 'F':
			return __tr2qs("Allow anyone to forward users to this channel");
			break;
		case 'L':
			return __tr2qs("Large ban/exempt/invex lists (staff only)");
			break;
		case 'M':
			return __tr2qs("Disallow kicking opers (staff only)");
			break;
		case 'P':
			return __tr2qs("Persistent (staff only)");
			break;
		case 'Q':
			return __tr2qs("Block forwarded users");
			break;
		case 'R':
			return __tr2qs("Only registered nicks can join");
			break;
		case 'S':
			return __tr2qs("SSL Only");
			break;
		case 'T':
			return __tr2qs("Forbid channel NOTICEs");
			break;
		case 'c':
			return __tr2qs("Strip color codes");
			break;
		case 'f':
			return __tr2qs("Forward to another channel on uninvited");
			break;
		case 'g':
			return __tr2qs("Allow anybody to invite");
			break;
		case 'j':
			return __tr2qs("Join throttling (<num>:<secs>)");
			break;
		case 'p':
			return __tr2qs("Paranoid (disable KNOCK)");
			break;
		case 'q':
			return __tr2qs("Quiet");
			break;
		case 'r':
			return __tr2qs("Need auth to join channel");
			break;
		case 'z':
			return __tr2qs("Reduced moderation for ops");
			break;
	}
	return KviIrcdRatboxIrcServerInfo::getChannelModeDescription(mode);
}

const QString & KviPlexusIrcServerInfo::getChannelModeDescription(char mode) const
{
	switch(mode)
	{
		case 'B':
			return __tr2qs("Bandwidth Saver");
			break;
		case 'N':
			return __tr2qs("Forbid channel NOTICEs");
			break;
		case 'a':
			return __tr2qs("Protected/administrator nicks");
			break;
		case 'p':
			return __tr2qs("Paranoia");
			break;
		case 'q':
			return __tr2qs("Channel owners");
			break;
		case 'z':
			return __tr2qs("Persistent (staff only)");
			break;
	}
	return KviHybridServerInfo::getChannelModeDescription(mode);
}

const QString & KviOftcIrcServerInfo::getChannelModeDescription(char mode) const
{
	switch(mode)
	{
		case 'q':
			return __tr2qs("Quiet");
			break;
		case 'z':
			return __tr2qs("Reduced moderation for ops");
			break;
	}
	return KviHybridServerInfo::getChannelModeDescription(mode);
}

const QString & KviIrcuIrcServerInfo::getChannelModeDescription(char mode) const
{
	switch(mode)
	{
		case 'A':
			return __tr2qs("Admin password");
			break;
		case 'C':
			return __tr2qs("Forbid channel CTCPs");
			break;
		case 'D':
			return __tr2qs("Delay users join to first message");
			break;
		case 'R':
			return __tr2qs("Registered (staff only)");
			break;
		case 'U':
			return __tr2qs("User password");
			break;
		case 'c':
			return __tr2qs("No control codes (colors, bold, ..)");
			break;
		case 'd':
			return __tr2qs("Contains hidden users (previously +D)");
			break;
		case 'r':
			return __tr2qs("Only registered nicks can join");
			break;
		case 'z':
			return __tr2qs("Persistent (staff only)");
			break;
	}
	return KviBasicIrcServerInfo::getChannelModeDescription(mode);
}

const QString & KviSnircdIrcServerInfo::getChannelModeDescription(char mode) const
{
	switch(mode)
	{
		case 'M':
			return __tr2qs("Moderate non auth users");
			break;
		case 'N':
			return __tr2qs("Forbid channel NOTICEs");
			break;
		case 'T':
			return __tr2qs("No multi-target messages");
			break;
		case 'u':
			return __tr2qs("Hide QUIT and PART messages");
			break;
	}
	return KviIrcuIrcServerInfo::getChannelModeDescription(mode);
}

const QString & KviDarenetIrcServerInfo::getChannelModeDescription(char mode) const
{
	switch(mode)
	{
		case 'M':
			return __tr2qs("Moderate non auth users");
			break;
		case 'N':
			return __tr2qs("Block channel notices");
			break;
		case 'S':
			return __tr2qs("Strip color codes");
			break;
		case 'T':
			return __tr2qs("No multi-targets");
			break;
		case 'Z':
			return __tr2qs("Need SSL connection to join");
			break;
		case 'h':
			return __tr2qs("Half-operators");
			break;
		case 'q':
			return __tr2qs("Quiet");
			break;
		case 'u':
			return __tr2qs("Squelch parts/quits");
			break;
	}
	return KviIrcuIrcServerInfo::getChannelModeDescription(mode);
}

const QString & KviUnrealIrcServerInfo::getChannelModeDescription(char mode) const
{
	switch(mode)
	{
		case 'A':
			return __tr2qs("Server/Network/Tech admin only channel");
			break;
		case 'C':
			return __tr2qs("Forbid channel CTCPs");
			break;
		case 'G':
			return __tr2qs("Censor bad words");
			break;
		case 'K':
			return __tr2qs("Forbid /KNOCK");
			break;
		case 'L':
			return __tr2qs("Redirect on channel full (mode +l)");
			break;
		case 'M':
			return __tr2qs("Need auth to speak and change nick");
			break;
		case 'N':
			return __tr2qs("Forbid nick change");
			break;
		case 'O':
			return __tr2qs("IRC-Op only channel");
			break;
		case 'Q':
			return __tr2qs("Disallow KICK (unless U-Line)");
			break;
		case 'R':
			return __tr2qs("Only registered nicks can join");
			break;
		case 'a':
			return __tr2qs("Protected/administrator nicks");
			break;
		case 'c':
			return __tr2qs("No control codes (colors, bold, ..)");
			break;
		case 'f':
			return __tr2qs("Flood protection (<num><type>:<secs>)");
			break;
		case 'h':
			return __tr2qs("Half-operators");
			break;
		case 'j':
			return __tr2qs("Join throttling (<num>:<secs>)");
			break;
		case 'q':
			return __tr2qs("Channel owners");
			break;
		case 'r':
			return __tr2qs("Registered");
			break;
		case 'u':
			return __tr2qs("Auditorium: /NAMES and /WHO show only ops");
			break;
		case 'z':
			return __tr2qs("Need SSL connection to join");
			break;
	}
	return KviBasicIrcServerInfo::getChannelModeDescription(mode);
}

const QString & KviUnreal32IrcServerInfo::getChannelModeDescription(char mode) const
{
	switch(mode)
	{
		case 'B':
			return __tr2qs("No multi-target messages");
			break;
		case 'E':
			return __tr2qs("Forbid ACTIONs from non-ops");
			break;
		case 'F':
			return __tr2qs("Trusted filter (staff only)");
			break;
		case 'H':
			return __tr2qs("Help operator and above only channel (staff only)");
			break;
		case 'M':
			return __tr2qs("Moderate non auth users");
			break;
		case 'S':
			return __tr2qs("Strip colors");
			break;
		case 'T':
			return __tr2qs("Forbid channel NOTICEs");
			break;
		case 'V':
			return __tr2qs("No invites");
			break;
		case 'Z':
			return __tr2qs("All clients are using SSL (server only)");
			break;
	}
	return KviUnrealIrcServerInfo::getChannelModeDescription(mode);
}

const QString & KviUnreal40IrcServerInfo::getChannelModeDescription(char mode) const
{
	switch(mode)
	{
		case 'D':
			return __tr2qs("Delay users join to first message");
			break;
		case 'P':
			return __tr2qs("Persistent (staff only)");
			break;
		case 'd':
			return __tr2qs("Contains hidden users (previously +D)");
			break;
	}
	return KviUnreal32IrcServerInfo::getChannelModeDescription(mode);
}

const QString & KviCritenIrcServerInfo::getChannelModeDescription(char mode) const
{
	switch(mode)
	{
		case 'A':
			return __tr2qs("Server/Network/Tech admin only channel");
			break;
		case 'K':
			return __tr2qs("Forbid /KNOCK");
			break;
		case 'M':
			return __tr2qs("Need auth to speak and change nick");
			break;
		case 'N':
			return __tr2qs("No invites");
			break;
		case 'O':
			return __tr2qs("IRC-Op only channel");
			break;
		case 'R':
			return __tr2qs("Only registered nicks can join");
			break;
		case 'S':
			return __tr2qs("Need SSL connection to join");
			break;
		case 'c':
			return __tr2qs("No control codes (colors, bold, ..)");
			break;
		case 'f':
			return __tr2qs("Flood limit");
			break;
		case 'r':
			return __tr2qs("Registered channel");
			break;
	}
	return KviBasicIrcServerInfo::getChannelModeDescription(mode);
}

const QString & KviBahamutIrcServerInfo::getChannelModeDescription(char mode) const
{
	switch(mode)
	{
		case 'L':
			return __tr2qs("Channel visible in LIST");
			break;
		case 'M':
			return __tr2qs("Need auth to speak and change nick");
			break;
		case 'O':
			return __tr2qs("IRC-Op only channel");
			break;
		case 'R':
			return __tr2qs("Only registered nicks can join");
			break;
		case 'c':
			return __tr2qs("No control codes (colors, bold, ..)");
			break;
		case 'j':
			if(m_pParent)
			{
				if(m_pParent->supportedPlainModes().contains('j'))
				{
					// Azzurra (bahamut 1.4) similar to 'R'
					return __tr2qs("Only registered nicks can join");
				}
			}

			// Dalnet (bahamut 1.8)
			return __tr2qs("Join throttling (<num>:<secs>)");
			break;
		// azzurra (bahamuth 1.4) specific addition:
		case 'S':
			return __tr2qs("Need SSL connection to join");
			break;
		case 'U':
			return __tr2qs("Permit foreign users without auth to JOIN");
			break;
		case 'd':
			return __tr2qs("Need op/voice to change nick");
			break;
		case 'h':
			return __tr2qs("Half-operators");
			break;
		case 'r':
			return __tr2qs("Registered");
			break;
		case 'u':
			return __tr2qs("Hide QUIT and PART messages");
			break;
	}
	return KviBasicIrcServerInfo::getChannelModeDescription(mode);
}

const QString & KviInspIRCdIrcServerInfo::getChannelModeDescription(char mode) const
{
	switch(mode)
	{
		case 'A':
			return __tr2qs("Allow anybody to invite");
			break;
		case 'B':
			return __tr2qs("Block messages with too many CAPS");
			break;
		case 'C':
			return __tr2qs("Forbid channel CTCPs");
			break;
		case 'D':
			return __tr2qs("Delay users join to first message");
			break;
		case 'F':
			return __tr2qs("Limit nick changes (<num>:<secs>)");
			break;
		case 'G':
			return __tr2qs("Censor bad words");
			break;
		case 'J':
			return __tr2qs("Disable join after kick (<secs>)");
			break;
		case 'K':
			return __tr2qs("Forbid /KNOCK");
			break;
		case 'L':
			return __tr2qs("Redirect on channel full (mode +l)");
			break;
		case 'M':
			return __tr2qs("Need auth to speak and change nick");
			break;
		case 'N':
			return __tr2qs("Need auth to change nick");
			break;
		case 'O':
			return __tr2qs("IRC-Op only channel");
			break;
		case 'P':
			return __tr2qs("Persistent (staff only)");
			break;
		case 'Q':
			return __tr2qs("Disallow KICK (unless U-Line)");
			break;
		case 'R':
			return __tr2qs("Only registered nicks can join");
			break;
		case 'S':
			return __tr2qs("Strip colors");
			break;
		case 'T':
			return __tr2qs("Forbid channel NOTICEs");
			break;
		case 'X':
			return __tr2qs("Channel exemptions");
			break;
		case 'Y':
			return __tr2qs("Channel IRC op");
			break;
		case 'a':
			return __tr2qs("Protected/administrator nicks");
			break;
		case 'c':
			return __tr2qs("No control codes (colors, bold, ..)");
			break;
		case 'd':
			return __tr2qs("Delay first message after join by n seconds");
			break;
		case 'f':
			return __tr2qs("Kick/[*]ban on message flood ([*]<num>:<secs>)");
			break;
		case 'g':
			return __tr2qs("Spam filter");
			break;
		case 'h':
			return __tr2qs("Half-operators");
			break;
		case 'j':
			return __tr2qs("Join throttling (<num>:<secs>)");
			break;
		case 'q':
			return __tr2qs("Channel owners");
			break;
		case 'r':
			return __tr2qs("Registered");
			break;
		case 'u':
			return __tr2qs("Auditorium: /NAMES and /WHO show only ops");
			break;
		case 'w':
			return __tr2qs("Channel access");
			break;
		case 'y':
			return __tr2qs("Channel IRC op");
			break;
		case 'z':
			return __tr2qs("Need SSL connection to join");
			break;
	}
	return KviBasicIrcServerInfo::getChannelModeDescription(mode);
}
