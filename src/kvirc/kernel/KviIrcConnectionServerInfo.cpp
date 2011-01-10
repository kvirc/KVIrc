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



#include "KviIrcConnectionServerInfo.h"
#include "KviLocale.h"
#include "KviMemory.h"
#include "KviIrcUserDataBase.h"

KviIrcConnectionServerInfo::KviIrcConnectionServerInfo()
{
	// default assumptions
	m_szSupportedChannelTypes = "#&!+";
	m_szSupportedModePrefixes = "@+";
	m_szSupportedModeFlags = "ov";
	m_pModePrefixTable = 0;
	buildModePrefixTable();
	m_bSupportsWatchList = false;
	m_bSupportsCodePages = false;
	m_bSupportsCap = false;
	m_iMaxTopicLen = -1;
	m_iMaxModeChanges = 3;
	m_szListModes = "b";
	m_szParameterModes = "k";
	m_szParameterWhenSetModes = "l";
	m_szPlainModes = "pstnmi";
	m_szSupportedChannelModes = "pstnmiklb";
	m_pServInfo = new KviBasicIrcServerInfo(this);
}

KviIrcConnectionServerInfo::~KviIrcConnectionServerInfo()
{
	if(m_pServInfo)
		delete m_pServInfo;
	if(m_pModePrefixTable)
		KviMemory::free(m_pModePrefixTable);
}

void KviIrcConnectionServerInfo::addSupportedCaps(const QString &szCapList)
{
	m_bSupportsCap = true;

	QStringList lTmp = szCapList.split(' ',QString::SkipEmptyParts);
	foreach(QString szCap,lTmp)
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
				szCap.remove(0,1);
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

void KviIrcConnectionServerInfo::setSupportedChannelModes(const QString &szSupportedChannelModes)
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

void KviIrcConnectionServerInfo::setSupportedModePrefixes(const QString &szSupportedModePrefixes,const QString &szSupportedModeFlags)
{
	m_szSupportedModeFlags = szSupportedModeFlags;
	m_szSupportedModePrefixes = szSupportedModePrefixes;
	buildModePrefixTable();
}

void KviIrcConnectionServerInfo::buildModePrefixTable()
{
	if(m_pModePrefixTable) KviMemory::free(m_pModePrefixTable);
	const QChar * cPrefix = m_szSupportedModePrefixes.constData();
	const QChar * cFlag = m_szSupportedModeFlags.constData();
	if(!cPrefix || !cFlag)return; // eh ?

	m_uPrefixes=qMin(m_szSupportedModePrefixes.length(),m_szSupportedModePrefixes.length());
	m_pModePrefixTable=(kvi_u32_t*)KviMemory::allocate(sizeof(kvi_u32_t)*3*m_uPrefixes);

	unsigned short uPrefix,uFlag;

	// Pragma: FIXME: The whole mode handling needs rethinking!
	// Alexey: FIXED: rethinked:)
	// Pragma: LOL :DDDD
	unsigned int i=0;
	while( (uPrefix = cPrefix->unicode()) && (uFlag = cFlag->unicode()) && i<m_uPrefixes*3)
	{
		m_pModePrefixTable[i]=uPrefix;
		m_pModePrefixTable[i+1]=uFlag;
		switch(uFlag)
		{
			case 'o': m_pModePrefixTable[i+2] = KviIrcUserEntry::Op;
				break;
			case 'v': m_pModePrefixTable[i+2] = KviIrcUserEntry::Voice;
				break;
			case 'h': m_pModePrefixTable[i+2] = KviIrcUserEntry::HalfOp;
				break;
			case 'a': m_pModePrefixTable[i+2] = KviIrcUserEntry::ChanAdmin;
				break;
			case 'u': m_pModePrefixTable[i+2] = KviIrcUserEntry::UserOp;
				break;
			case 'q': m_pModePrefixTable[i+2] = KviIrcUserEntry::ChanOwner;
				break;
			default:
				m_pModePrefixTable[i+2]=0;
			break;
		}
		i+=3;
		cPrefix++;
		cFlag++;
	}
}

bool KviIrcConnectionServerInfo::isSupportedModePrefix(QChar c)
{
	if(!m_pModePrefixTable) return false;
	for(unsigned int i=0;i<m_uPrefixes;i++)
	{
		if(m_pModePrefixTable[i*3]==c.unicode()) return true;
	}
	return false;
}

bool KviIrcConnectionServerInfo::isSupportedModeFlag(QChar c)
{
	if(!m_pModePrefixTable) return false;
	for(unsigned int i=0;i<m_uPrefixes;i++)
	{
		if(m_pModePrefixTable[(i*3)+1]==c.unicode()) return true;
	}
	return false;
}

QChar KviIrcConnectionServerInfo::modePrefixChar(kvi_u32_t flag)
{
	if(!m_pModePrefixTable) return QChar(0);
	for(unsigned int i=0;i<m_uPrefixes;i++)
	{
		if(m_pModePrefixTable[i*3+2] & flag) return QChar(m_pModePrefixTable[i*3]);
	}
	return QChar(0);
}

QChar KviIrcConnectionServerInfo::modeFlagChar(kvi_u32_t flag)
{
	if(!m_pModePrefixTable) return QChar(0);
	for(unsigned int i=0;i<m_uPrefixes;i++)
	{
		if(m_pModePrefixTable[i*3+2] & flag ) return QChar(m_pModePrefixTable[i*3+1]);
	}
	return QChar(0);
}

kvi_u32_t KviIrcConnectionServerInfo::modeFlagFromPrefixChar(QChar c)
{
	if(!m_pModePrefixTable) return 0;
	for(unsigned int i=0;i<m_uPrefixes;i++)
	{
		if(m_pModePrefixTable[i*3]==c.unicode()) return m_pModePrefixTable[i*3+2];
	}
	return 0;
}

kvi_u32_t KviIrcConnectionServerInfo::modeFlagFromModeChar(QChar c)
{
	if(!m_pModePrefixTable) return 0;
	for(unsigned int i=0;i<m_uPrefixes;i++)
	{
		if(m_pModePrefixTable[i*3+1]==c.unicode()) return m_pModePrefixTable[i*3+2];
	}
	return 0;
}

void KviIrcConnectionServerInfo::setServerVersion(const QString & version)
{
	if(m_pServInfo) delete m_pServInfo;
	if(version.contains("unreal",Qt::CaseInsensitive))
		m_pServInfo = new KviUnrealIrcServerInfo(this, version);
	else if(version.contains("bahamut",Qt::CaseInsensitive))
		m_pServInfo = new KviBahamutIrcServerInfo(this, version);
	else if(version.contains("hyperion",Qt::CaseInsensitive))
		m_pServInfo = new KviHyperionIrcServerInfo(this, version);
	else if(version.contains("ircd-seven",Qt::CaseInsensitive))
		m_pServInfo = new KviIrcdSevenIrcServerInfo(this, version);
	else if(version.contains("ratbox",Qt::CaseInsensitive))
		m_pServInfo = new KviIrcdRatboxIrcServerInfo(this, version);
	else if(version.contains("inspircd",Qt::CaseInsensitive))
		m_pServInfo = new KviInspIRCdIrcServerInfo(this, version);
	else if(version.contains("snircd",Qt::CaseInsensitive))
		m_pServInfo = new KviSnircdIrcServerInfo(this, version);
	else if(version.contains("u2",Qt::CaseInsensitive))
		m_pServInfo = new KviIrcuIrcServerInfo(this, version);
	else if(version.contains("plexus",Qt::CaseInsensitive))
		m_pServInfo = new KviPlexusIrcServerInfo(this, version);
	else
		m_pServInfo = new KviBasicIrcServerInfo(this, version);
}


KviBasicIrcServerInfo::KviBasicIrcServerInfo(KviIrcConnectionServerInfo * pParent, const QString & version)
{
	m_szServerVersion = version;
	m_pParent = pParent;
}

KviBasicIrcServerInfo::~KviBasicIrcServerInfo()
{
}

const QString & KviBasicIrcServerInfo::getChannelModeDescription(char mode)
{
	switch(mode)
	{
// 		case '7': return __tr2qs("Only 7-bit letters in nicknames allowed"); break;
		case 'I': return __tr2qs("Invite Exceptions"); break;
		case 'R': return __tr2qs("Reop Masks"); break;
		case 'a': return __tr2qs("Anonymous messages"); break;
		case 'b': return __tr2qs("Ban Masks"); break;
		case 'e': return __tr2qs("Ban Exceptions"); break;
		case 'i': return __tr2qs("Invite only"); break;
		case 'k': return __tr2qs("Key"); break;
		case 'l': return __tr2qs("Limited number of users"); break;
		case 'm': return __tr2qs("Moderated"); break;
		case 'n': return __tr2qs("No external messages"); break;
		case 'o': return __tr2qs("Channel operators"); break;
		case 'p': return __tr2qs("Private"); break;
		case 'q': return __tr2qs("Quiet"); break;
		case 'r': return __tr2qs("Reop"); break;
		case 's': return __tr2qs("Secret"); break;
		case 't': return __tr2qs("Topic change restricted"); break;
// 		case 'u': return __tr2qs("User-operators"); break;
		case 'v': return __tr2qs("Voiced users"); break;
// 		case 'x': return __tr2qs("Impossible to use both 7-bit and 8-bit letters in nicknames"); break;
	}
	return KviQString::Empty;
}

const QString & KviBasicIrcServerInfo::getUserModeDescription(QChar mode)
{
	switch(mode.unicode())
	{
		case 'o': return __tr2qs("o: IRC operator (OPER)"); break;
		case 'O': return __tr2qs("O: Local IRC operator (LOCOP)"); break;
		case 'i': return __tr2qs("i: Invisible"); break;
		case 'w': return __tr2qs("w: Recipient for WALLOPS messages"); break;
		case 'r': return __tr2qs("r: User with restricted connection (or recipient for messages about rejected bots)"); break;
		case 's': return __tr2qs("s: Recipient for server notices"); break;
		case 'z': return __tr2qs("z: Recipient for oper wallop messages"); break;
		case 'c': return __tr2qs("c: Recipient for cconn messages"); break;
		case 'k': return __tr2qs("k: Recipient for server kill messages"); break;
		case 'f': return __tr2qs("f: Recipient for full server notices"); break;
		case 'y': return __tr2qs("y: Spy :)"); break;
		case 'd': return __tr2qs("d: Obscure 'DEBUG' flag"); break;
		case 'n': return __tr2qs("n: Recipient for nick changes"); break;
	}
	return KviQString::Empty;
}

const QString & KviUnrealIrcServerInfo::getChannelModeDescription(char mode)
{
	switch(mode)
	{
		case 'A': return __tr2qs("Server/Network/Tech Admin only channel"); break;
		case 'C': return __tr2qs("Forbid channel CTCPs"); break;
		case 'G': return __tr2qs("Censor bad words"); break;
		case 'K': return __tr2qs("Forbid /KNOCK"); break;
		case 'L': return __tr2qs("Redirect on channel full (mode +l)"); break;
		case 'M': return __tr2qs("Need auth to speak and change nick"); break;
		case 'N': return __tr2qs("Forbid nick change"); break;
		case 'O': return __tr2qs("IRC-Op only channel"); break;
		case 'Q': return __tr2qs("Disallow KICK (unless U-Line)"); break;
		case 'R': return __tr2qs("Only registered nicks can join"); break;
		case 'a': return __tr2qs("Protected/admin nicks"); break;
		case 'c': return __tr2qs("No control codes (colors, bold, ..)"); break;
		case 'f': return __tr2qs("Flood protection (<num><type>:<secs>)"); break;
		case 'h': return __tr2qs("Half-operators");break;
		case 'j': return __tr2qs("Join throttling (<num>:<secs>)"); break;
		case 'q': return __tr2qs("Channel owners"); break;
		case 'r': return __tr2qs("Registered"); break;
		case 'u': return __tr2qs("Auditorium: /NAMES and /WHO show only ops"); break;
		case 'z': return __tr2qs("Need SSL connection to join"); break;
	}
	return KviBasicIrcServerInfo::getChannelModeDescription(mode);
}


const QString & KviIrcdSevenIrcServerInfo::getChannelModeDescription(char mode)
{
	switch(mode)
	{
		case 'C': return __tr2qs("Forbid channel CTCPs"); break;
		case 'F': return __tr2qs("Enable forwarding"); break;
		case 'L': return __tr2qs("Large ban/exempt/invex lists (staff only)"); break;
		case 'M': return __tr2qs("Disallow kicking opers (staff only)"); break;
		case 'P': return __tr2qs("Persistent (staff only)"); break;
		case 'Q': return __tr2qs("Block forwarded users"); break;
		case 'R': return __tr2qs("Only registered nicks can join"); break;
		case 'S': return __tr2qs("Strip colors"); break;
		case 'T': return __tr2qs("Forbid channel NOTICEs"); break;
		case 'c': return __tr2qs("No control codes (colors, bold, ..)"); break;
		case 'f': return __tr2qs("Forward to another channel on uninvited"); break;
		case 'g': return __tr2qs("Allow anybody to invite");break;
		case 'j': return __tr2qs("Join throttling (<num>:<secs>)"); break;
		case 'p': return __tr2qs("Paranoid (disable KNOCK)"); break;
		case 'q': return __tr2qs("Quiet Ban Editor"); break;
		case 'r': return __tr2qs("Need auth to join channel"); break;
		case 'z': return __tr2qs("Reduced moderation for ops"); break;
	}
	return KviBasicIrcServerInfo::getChannelModeDescription(mode);
}

const QString & KviBahamutIrcServerInfo::getChannelModeDescription(char mode)
{
	switch(mode)
	{
		case 'L': return __tr2qs("Channel visible in LIST"); break;
		case 'M': return __tr2qs("Need auth to speak and change nick"); break;
		case 'O': return __tr2qs("IRC-Op only channel"); break;
		case 'R': return __tr2qs("Only registered nicks can join"); break;
		case 'c': return __tr2qs("No control codes (colors, bold, ..)"); break;
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
		case 'S': return __tr2qs("Need SSL connection to join"); break;
		case 'U': return __tr2qs("Permit foreign users without auth to JOIN"); break;
		case 'd': return __tr2qs("Need op/voice to change nick"); break;
		case 'h': return __tr2qs("Half-operators");break;
		case 'r': return __tr2qs("Registered"); break;
		case 'u': return __tr2qs("Hide QUIT and PART messages"); break;
	}
	return KviBasicIrcServerInfo::getChannelModeDescription(mode);
}

const QString & KviInspIRCdIrcServerInfo::getChannelModeDescription(char mode)
{
	switch(mode)
	{
		case 'A': return __tr2qs("Allow anybody to invite");break;
		case 'B': return __tr2qs("Block messages with too many CAPS");break;
		case 'C': return __tr2qs("Forbid channel CTCPs"); break;
		case 'D': return __tr2qs("Delay users join to first message"); break;
		case 'F': return __tr2qs("Limit nick changes (<num>:<secs>)"); break;
		case 'G': return __tr2qs("Censor bad words"); break;
		case 'J': return __tr2qs("Disable join after kick (<secs>)"); break;
		case 'K': return __tr2qs("Forbid /KNOCK"); break;
		case 'L': return __tr2qs("Redirect on channel full (mode +l)"); break;
		case 'M': return __tr2qs("Need auth to speak and change nick"); break;
		case 'N': return __tr2qs("Need auth to change nick"); break;
		case 'O': return __tr2qs("IRC-Op only channel"); break;
		case 'P': return __tr2qs("Persistent (staff only)"); break;
		case 'Q': return __tr2qs("Disallow KICK (unless U-Line)"); break;
		case 'R': return __tr2qs("Only registered nicks can join"); break;
		case 'S': return __tr2qs("Strip colors"); break;
		case 'T': return __tr2qs("Forbid channel NOTICEs"); break;
		case 'a': return __tr2qs("Protected/admin nicks"); break;
		case 'c': return __tr2qs("No control codes (colors, bold, ..)"); break;
		case 'f': return __tr2qs("Kick/[*]ban on message flood ([*]<num>:<secs>)"); break;
		case 'g': return __tr2qs("Block message matching"); break;
		case 'j': return __tr2qs("Join throttling (<num>:<secs>)"); break;
		case 'q': return __tr2qs("Channel owners"); break;
		case 'u': return __tr2qs("Auditorium: /NAMES and /WHO show only ops"); break;
		case 'y': return __tr2qs("Channel IRC op"); break;
		case 'z': return __tr2qs("Need SSL connection to join"); break;
	}
	return KviBasicIrcServerInfo::getChannelModeDescription(mode);
}

const QString & KviIrcdRatboxIrcServerInfo::getChannelModeDescription(char mode)
{
	switch(mode)
	{
		// not present of efnet, but supported by ratbox
		case 'r': return __tr2qs("Only registered nicks can join"); break;
		case 'S': return __tr2qs("Need SSL connection to join"); break;
	}
	return KviBasicIrcServerInfo::getChannelModeDescription(mode);
}

const QString & KviSnircdIrcServerInfo::getChannelModeDescription(char mode)
{
	switch(mode)
	{
		case 'C': return __tr2qs("Forbid channel CTCPs"); break;
		case 'D': return __tr2qs("Delay users join to first message"); break;
		case 'M': return __tr2qs("Moderate non auth users"); break;
		case 'N': return __tr2qs("Forbid channel NOTICEs"); break;
		case 'R': return __tr2qs("Registered"); break;
		case 'T': return __tr2qs("No multitarget messages"); break;
		case 'd': return __tr2qs("Contains hidden users (previously +D)"); break;
		case 'c': return __tr2qs("No control codes (colors, bold, ..)"); break;
		case 'r': return __tr2qs("Only registered nicks can join"); break;
		case 'u': return __tr2qs("Hide QUIT and PART messages"); break;
	}
	return KviBasicIrcServerInfo::getChannelModeDescription(mode);
}

const QString & KviIrcuIrcServerInfo::getChannelModeDescription(char mode)
{
	switch(mode)
	{
		case 'D': return __tr2qs("Delay users join to first message"); break;
		case 'R': return __tr2qs("Registered"); break;
		case 'd': return __tr2qs("Contains hidden users (previously +D)"); break;
		case 'r': return __tr2qs("Only registered nicks can join"); break;
	}
	return KviBasicIrcServerInfo::getChannelModeDescription(mode);
}

const QString & KviPlexusIrcServerInfo::getChannelModeDescription(char mode)
{
	switch(mode)
	{
		case 'B': return __tr2qs("Bandwidth Saver"); break;
		case 'M': return __tr2qs("Moderate non auth users"); break;
		case 'N': return __tr2qs("Forbid channel NOTICEs"); break;
		case 'O': return __tr2qs("IRC-Op only channel"); break;
		case 'R': return __tr2qs("Only registered nicks can join"); break;
		case 'S': return __tr2qs("Need SSL connection to join"); break;
		case 'a': return __tr2qs("Protected/admin nicks"); break;
		case 'c': return __tr2qs("No control codes (colors, bold, ..)"); break;
		case 'h': return __tr2qs("Half-operators");break;
		case 'p': return __tr2qs("Paranoia"); break;
		case 'q': return __tr2qs("Channel owners"); break;
		case 'z': return __tr2qs("Persistent (staff only)"); break;
	}
	return KviBasicIrcServerInfo::getChannelModeDescription(mode);
}
