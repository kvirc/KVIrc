//=============================================================================
//
//   File : kvi_ircconnectionserverinfo.cpp
//   Creation date : Tue 22 Jun 2004 03:57:32 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2008 Szymon Stefanek <pragma at kvirc dot net>
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



#include "kvi_ircconnectionserverinfo.h"
#include "kvi_locale.h"
#include "kvi_malloc.h"
#include "kvi_ircuserdb.h"

KviIrcConnectionServerInfo::KviIrcConnectionServerInfo()
{
	// default assumptions
	m_szSupportedChannelTypes = "#&!+";
	m_szSupportedModePrefixes = "@+";
	m_szSupportedModeFlags = "ov";
	m_pModePrefixTable = 0;
	buildModePrefixTable();
	m_bSupportsModesIe = true;
	m_bSupportsWatchList = false;
	m_bSupportsCodePages = false;
	m_bSupportsCap = false;
	m_iMaxTopicLen = -1;
	m_iMaxModeChanges = 3;
	m_pServInfo = new KviBasicIrcServerInfo();
}

KviIrcConnectionServerInfo::~KviIrcConnectionServerInfo()
{
	if(m_pServInfo)
		delete m_pServInfo;
	if(m_pModePrefixTable)
		kvi_free(m_pModePrefixTable);
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
	int pos=-1;

	pos=szSupportedChannelModes.indexOf(',');
	if(pos>=0) m_szListModes=szSupportedChannelModes.left(pos);

	pos=szSupportedChannelModes.lastIndexOf(',');
	if(pos>=0) m_szPlainModes=szSupportedChannelModes.right(szSupportedChannelModes.length()+pos-1);

	m_szSupportedChannelModes = szSupportedChannelModes;
	m_bSupportsModesIe = (szSupportedChannelModes.contains('e') && szSupportedChannelModes.contains('I'));
	m_bSupportsModeq = szSupportedChannelModes.contains('q');
	
	QChar* aux=(QChar*)szSupportedChannelModes.utf16();
	while(aux->unicode())
	{
		if(!m_szSupportedChannelModes.contains(*aux))
			m_szSupportedChannelModes.append(*aux);
		aux++;
	}
}

void KviIrcConnectionServerInfo::setSupportedModePrefixes(const QString &szSupportedModePrefixes,const QString &szSupportedModeFlags)
{
	m_szSupportedModeFlags = szSupportedModeFlags;
	m_szSupportedModePrefixes = szSupportedModePrefixes;
	buildModePrefixTable();
}

void KviIrcConnectionServerInfo::buildModePrefixTable()
{
	if(m_pModePrefixTable) kvi_free(m_pModePrefixTable);
	const QChar * cPrefix = KviQString::nullTerminatedArray(m_szSupportedModePrefixes);
	const QChar * cFlag = KviQString::nullTerminatedArray(m_szSupportedModeFlags);
	if(!cPrefix || !cFlag)return; // eh ?

	m_uPrefixes=qMin(m_szSupportedModePrefixes.length(),m_szSupportedModePrefixes.length());
	m_pModePrefixTable=(kvi_u32_t*)kvi_malloc(sizeof(kvi_u32_t)*3*m_uPrefixes);

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
			case 'o': m_pModePrefixTable[i+2] = KVI_USERFLAG_OP;
				break;
			case 'v': m_pModePrefixTable[i+2] = KVI_USERFLAG_VOICE;
				break;
			case 'h': m_pModePrefixTable[i+2] = KVI_USERFLAG_HALFOP;
				break;
			case 'a': m_pModePrefixTable[i+2] = KVI_USERFLAG_CHANADMIN;
				break;
			case 'u': m_pModePrefixTable[i+2] = KVI_USERFLAG_USEROP;
				break;
			case 'q': m_pModePrefixTable[i+2] = KVI_USERFLAG_CHANOWNER;
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
		m_pServInfo = new KviUnrealIrcServerInfo(version);
	else if(version.contains("bahamut",Qt::CaseInsensitive))
		m_pServInfo = new KviBahamutIrcServerInfo(version);
	else if(version.contains("hyperion",Qt::CaseInsensitive))
		m_pServInfo = new KviHyperionIrcServerInfo(version);
	else if(version.contains("ircd-seven",Qt::CaseInsensitive))
		m_pServInfo = new KviIrcdSevenIrcServerInfo(version);
	else if(version.contains("ratbox",Qt::CaseInsensitive))
		m_pServInfo = new KviIrcdRatboxIrcServerInfo(version);
	else if(version.contains("inspircd",Qt::CaseInsensitive))
		m_pServInfo = new KviInspIRCdIrcServerInfo(version);
	else
		m_pServInfo = new KviBasicIrcServerInfo(version);
}


KviBasicIrcServerInfo::KviBasicIrcServerInfo(const QString & version)
{
	m_szServerVersion = version;
}

KviBasicIrcServerInfo::~KviBasicIrcServerInfo()
{
}

const QString & KviBasicIrcServerInfo::getChannelModeDescription(QChar mode)
{
	QString tmp=getCustomChannelModeDescription(mode);
	if(tmp.isNull()) return getBasicChannelModeDescription(mode);
	return KviQString::Empty;
}

const QString & KviBasicIrcServerInfo::getUserModeDescription(QChar mode)
{
	QString tmp=getCustomUserModeDescription(mode);
	if(tmp.isNull()) return getBasicUserModeDescription(mode);
	return KviQString::Empty;
}

const QString & KviBasicIrcServerInfo::getBasicChannelModeDescription(QChar mode)
{
	switch(mode.unicode())
	{
		case 'o': return __tr2qs("Channel operators"); break;
		case 'v': return __tr2qs("Voiced users"); break;
		case 'h': return __tr2qs("Half-operators");break;
		case 'b': return __tr2qs("Ban masks"); break;
		case 'c': return __tr2qs("Color free (no ANSI colors)"); break;
		case 'e': return __tr2qs("Ban exception masks"); break;
		case 'I': return __tr2qs("Invite exception masks or forbid /INVITE"); break;
		case 's': return __tr2qs("Secret"); break;
		case 'p': return __tr2qs("Private"); break;
		case 't': return __tr2qs("Topic change restricted"); break;
		case 'i': return __tr2qs("Invite only"); break;
		case 'n': return __tr2qs("No external messages"); break;
		case 'a': return __tr2qs("Anonymous or protected user"); break;
		case 'q': return __tr2qs("Quiet or channel owner"); break;
		case 'l': return __tr2qs("Limited number of users"); break;
		case 'k': return __tr2qs("Key"); break;
		case 'm': return __tr2qs("Moderated"); break;
		case 'r': return __tr2qs("Registered"); break;
		case 'G': return __tr2qs("Censor swear words"); break;
		case 'M': return __tr2qs("Impossible to speak and change nick if not authenticated with NickServ"); break;
		case 'N': return __tr2qs("Impossible to change nick if not authenticated with NickServ"); break;
		case 'R': return __tr2qs("Only registered nicks can join"); break;
		case 'Q': return __tr2qs("No kicks able (unless U-Line)"); break;
		case 'O': return __tr2qs("IRC-Op only channel"); break;
		case 'A': return __tr2qs("Server/Network/Tech Admin only channel"); break;
		case 'K': return __tr2qs("Forbid /KNOCK"); break;
		case 'S': return __tr2qs("Strip colors"); break;
		case 'L': return __tr2qs("Redirect on channel full"); break;
		case 'C': return __tr2qs("Forbid channel CTCPs"); break;
		case 'u': return __tr2qs("User-operators OR Auditorium: /NAMES and /WHO show only ops"); break;
		case '7': return __tr2qs("Only 7-bit letters in nicknames allowed"); break;
		case 'x': return __tr2qs("Impossible to use both 7-bit and 8-bit letters in nicknames"); break;
	}
	return KviQString::Empty;
}

const QString & KviBasicIrcServerInfo::getBasicUserModeDescription(QChar mode)
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
