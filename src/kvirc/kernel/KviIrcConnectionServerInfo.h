#ifndef _KVI_IRCCONNECTIONSERVERINFO_H_
#define _KVI_IRCCONNECTIONSERVERINFO_H_
//=============================================================================
//
//   File : KviIrcConnectionServerInfo.h
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

#include "kvi_settings.h"
#include "KviQString.h"
#include "kvi_inttypes.h"

#include <QStringList>

class KviIrcConnectionServerInfo;

class KVIRC_API KviBasicIrcServerInfo
{
	// ircnet
protected:
	QString m_szServerVersion;
	KviIrcConnectionServerInfo * m_pParent;

public:
	KviBasicIrcServerInfo(KviIrcConnectionServerInfo * pParent = 0, const QString & version = KviQString::Empty);
	virtual ~KviBasicIrcServerInfo();

public:
	virtual const QString & getChannelModeDescription(char mode);
	virtual const QString & getUserModeDescription(QChar mode);
	virtual QChar getUserModeRequirement(QChar mode);
	virtual char getRegisterModeChar() { return 0; };
	virtual const char * getSoftware() { return "Ircd"; };
	virtual bool getNeedsOpToListModeseI() { return false; };
	virtual bool getNeedsOperToSetS() { return false; };
};

//
// Hybrid + forks
//

class KVIRC_API KviHybridServerInfo : public KviBasicIrcServerInfo
{
	// This is a major IRCd that most modern forks are based off of
public:
	KviHybridServerInfo(KviIrcConnectionServerInfo * pParent = 0, const QString & version = KviQString::Empty)
		: KviBasicIrcServerInfo(pParent, version) {
		;
	};
	virtual const QString & getChannelModeDescription(char mode);
	virtual const QString & getUserModeDescription(QChar mode);
	virtual QChar getUserModeRequirement(QChar mode);
	virtual char getRegisterModeChar() { return 'r'; };
	virtual const char * getSoftware() { return "Hybrid"; };
};

class KVIRC_API KviIrcdRatboxIrcServerInfo : public KviHybridServerInfo
{
	// efnet
public:
	KviIrcdRatboxIrcServerInfo(KviIrcConnectionServerInfo * pParent = 0, const QString & version = KviQString::Empty)
		: KviHybridServerInfo(pParent, version) {
		;
	};
	virtual const QString & getChannelModeDescription(char mode);
	virtual const QString & getUserModeDescription(QChar mode);
	virtual QChar getUserModeRequirement(QChar mode);
	virtual char getRegisterModeChar() { return 0; };
	virtual const char * getSoftware() { return "Ircd-ratbox"; };
	virtual bool getNeedsOpToListModeseI() { return true; };
};

class KVIRC_API KviCharybdisServerInfo : public KviIrcdRatboxIrcServerInfo
{
public:
	KviCharybdisServerInfo(KviIrcConnectionServerInfo * pParent = 0, const QString & version = KviQString::Empty)
		: KviIrcdRatboxIrcServerInfo(pParent, version) {
		;
	};
	virtual const QString & getChannelModeDescription(char mode);
	virtual const QString & getUserModeDescription(QChar mode);
	virtual QChar getUserModeRequirement(QChar mode);
	virtual const char * getSoftware() { return "Charybdis"; };
	virtual bool getNeedsOperToSetS() { return true; };
};

class KVIRC_API KviIrcdSevenIrcServerInfo : public KviCharybdisServerInfo
{
	// freenode
public:
	KviIrcdSevenIrcServerInfo(KviIrcConnectionServerInfo * pParent = 0, const QString & version = KviQString::Empty)
		: KviCharybdisServerInfo(pParent, version) {
		;
	};
	virtual const QString & getUserModeDescription(QChar mode);
	virtual QChar getUserModeRequirement(QChar mode);
	virtual char getRegisterModeChar() { return 0; };
	virtual const char * getSoftware() { return "Ircd-seven"; };
};

class KVIRC_API KviPlexusIrcServerInfo : public KviHybridServerInfo
{
	// rizon; note: plexus is an extension to hybrid
public:
	KviPlexusIrcServerInfo(KviIrcConnectionServerInfo * pParent = 0, const QString & version = KviQString::Empty)
		: KviHybridServerInfo(pParent, version) {
		;
	};
	virtual const QString & getChannelModeDescription(char mode);
	virtual const QString & getUserModeDescription(QChar mode);
	virtual QChar getUserModeRequirement(QChar mode);
	virtual const char * getSoftware() { return "Plexus"; };
	virtual bool getNeedsOpToListModeseI() { return true; };
};

class KVIRC_API KviOftcIrcServerInfo : public KviHybridServerInfo
{
	// oftc; note: hybrid+oftc is an extension to hybrid
public:
	KviOftcIrcServerInfo(KviIrcConnectionServerInfo * pParent = 0, const QString & version = KviQString::Empty)
		: KviHybridServerInfo(pParent, version) {
		;
	};
	virtual const QString & getChannelModeDescription(char mode);
	virtual const QString & getUserModeDescription(QChar mode);
	virtual QChar getUserModeRequirement(QChar mode);
	virtual char getRegisterModeChar() { return 'R'; };
	virtual const char * getSoftware() { return "Hybrid+Oftc"; };
	virtual bool getNeedsOpToListModeseI() { return true; };
};

//
// P10
//

class KVIRC_API KviIrcuIrcServerInfo : public KviBasicIrcServerInfo
{
	// undernet
public:
	KviIrcuIrcServerInfo(KviIrcConnectionServerInfo * pParent = 0, const QString & version = KviQString::Empty)
		: KviBasicIrcServerInfo(pParent, version) {
		;
	};
	virtual const QString & getChannelModeDescription(char mode);
	virtual const QString & getUserModeDescription(QChar mode);
	virtual QChar getUserModeRequirement(QChar mode);
	virtual char getRegisterModeChar() { return 0; };
	virtual const char * getSoftware() { return "Ircu"; };
	virtual bool getNeedsOpToListModeseI() { return true; };
	virtual bool getNeedsOperToSetS() { return true; };
};

class KVIRC_API KviSnircdIrcServerInfo : public KviIrcuIrcServerInfo
{
	// quakenet; note: snird is an extension to ircu
public:
	KviSnircdIrcServerInfo(KviIrcConnectionServerInfo * pParent = 0, const QString & version = KviQString::Empty)
		: KviIrcuIrcServerInfo(pParent, version) {
		;
	};
	virtual const QString & getChannelModeDescription(char mode);
	virtual const QString & getUserModeDescription(QChar mode);
	virtual QChar getUserModeRequirement(QChar mode);
	virtual const char * getSoftware() { return "Snircd"; };
	virtual bool getNeedsOperToSetS() { return true; };
};

class KVIRC_API KviDarenetIrcServerInfo : public KviIrcuIrcServerInfo
{
	// darenet; note: u2+ircd-darenet is an extension to ircu
public:
	KviDarenetIrcServerInfo(KviIrcConnectionServerInfo * pParent = 0, const QString & version = KviQString::Empty)
		: KviIrcuIrcServerInfo(pParent, version) {
		;
	};
	virtual const QString & getChannelModeDescription(char mode);
	virtual const QString & getUserModeDescription(QChar mode);
	virtual QChar getUserModeRequirement(QChar mode);
	virtual char getRegisterModeChar() { return 'r'; };
	virtual const char * getSoftware() { return "Ircu+Darenet"; };
	virtual bool getNeedsOpToListModeseI() { return false; };
};

//
// Unreal -> Unreal 3.2 -> Unreal 4.0
//

class KVIRC_API KviUnrealIrcServerInfo : public KviBasicIrcServerInfo
{
public:
	KviUnrealIrcServerInfo(KviIrcConnectionServerInfo * pParent = 0, const QString & version = KviQString::Empty)
		: KviBasicIrcServerInfo(pParent, version) {
		;
	};
	virtual const QString & getChannelModeDescription(char mode);
	virtual char getRegisterModeChar() { return 'r'; };
	virtual const char * getSoftware() { return "Unreal"; };
	virtual bool getNeedsOpToListModeseI() { return false; };
	virtual bool getNeedsOperToSetS() { return true; };
};

class KVIRC_API KviUnreal32IrcServerInfo : public KviUnrealIrcServerInfo
{
	// This is a continuation on to Unreal, so use its predecessor
	// as a base class.
public:
	KviUnreal32IrcServerInfo(KviIrcConnectionServerInfo * pParent = 0, const QString & version = KviQString::Empty)
		: KviUnrealIrcServerInfo(pParent, version) {
		;
	};
	virtual const QString & getChannelModeDescription(char mode);
	virtual const QString & getUserModeDescription(QChar mode);
	virtual QChar getUserModeRequirement(QChar mode);
	virtual const char * getSoftware() { return "Unreal32"; };
};

class KVIRC_API KviUnreal40IrcServerInfo : public KviUnreal32IrcServerInfo
{
public:
	KviUnreal40IrcServerInfo(KviIrcConnectionServerInfo * pParent = 0, const QString & version = KviQString::Empty)
		: KviUnreal32IrcServerInfo(pParent, version) {
		;
	};
	virtual const QString & getChannelModeDescription(char mode);
	virtual const QString & getUserModeDescription(QChar mode);
	virtual const char * getSoftware() { return "Unreal40"; };
};

class KVIRC_API KviCritenIrcServerInfo : public KviBasicIrcServerInfo
{
	// abjects
public:
	KviCritenIrcServerInfo(KviIrcConnectionServerInfo * pParent = 0, const QString & version = KviQString::Empty)
		: KviBasicIrcServerInfo(pParent, version) {
		;
	};
	virtual const QString & getChannelModeDescription(char mode);
	virtual const QString & getUserModeDescription(QChar mode);
	virtual QChar getUserModeRequirement(QChar mode);
	virtual char getRegisterModeChar() { return 'r'; };
	virtual const char * getSoftware() { return "Criten"; };
	virtual bool getNeedsOpToListModeseI() { return true; };
};

class KVIRC_API KviNemesisIrcServerInfo : public KviCritenIrcServerInfo
{
	// criten
public:
	KviNemesisIrcServerInfo(KviIrcConnectionServerInfo * pParent = 0, const QString & version = KviQString::Empty)
		: KviCritenIrcServerInfo(pParent, version) {
		;
	};
	virtual const char * getSoftware() { return "Nemesis"; };
};

class KVIRC_API KviNemesis20IrcServerInfo : public KviUnreal32IrcServerInfo
{
public:
	KviNemesis20IrcServerInfo(KviIrcConnectionServerInfo * pParent = 0, const QString & version = KviQString::Empty)
		: KviUnreal32IrcServerInfo(pParent, version) {
		;
	};
	virtual const char * getSoftware() { return "Nemesis2.0"; };
	virtual bool getNeedsOperToSetS() { return false; };
};

class KVIRC_API KviBahamutIrcServerInfo : public KviBasicIrcServerInfo
{
	// dalnet, azzurranet
public:
	KviBahamutIrcServerInfo(KviIrcConnectionServerInfo * pParent = 0, const QString & version = KviQString::Empty)
		: KviBasicIrcServerInfo(pParent, version) {
		;
	};
	virtual const QString & getChannelModeDescription(char mode);
	virtual const QString & getUserModeDescription(QChar mode);
	virtual QChar getUserModeRequirement(QChar mode);
	virtual char getRegisterModeChar() { return 'r'; };
	virtual const char * getSoftware() { return "Bahamut"; };
	virtual bool getNeedsOpToListModeseI() { return false; };
};

class KVIRC_API KviHyperionIrcServerInfo : public KviBasicIrcServerInfo
{
	// legacy freenode : no longer maintained
public:
	KviHyperionIrcServerInfo(KviIrcConnectionServerInfo * pParent = 0, const QString & version = KviQString::Empty)
		: KviBasicIrcServerInfo(pParent, version) {
		;
	};
	virtual const QString & getUserModeDescription(QChar mode);
	virtual QChar getUserModeRequirement(QChar mode);
	virtual char getRegisterModeChar() { return 'e'; };
	virtual const char * getSoftware() { return "Hyperion"; };
	virtual bool getNeedsOpToListModeseI() { return true; };
};

class KVIRC_API KviInspIRCdIrcServerInfo : public KviBasicIrcServerInfo
{
	// chatspike
public:
	KviInspIRCdIrcServerInfo(KviIrcConnectionServerInfo * pParent = 0, const QString & version = KviQString::Empty)
		: KviBasicIrcServerInfo(pParent, version) {
		;
	};
	virtual const QString & getChannelModeDescription(char mode);
	virtual const QString & getUserModeDescription(QChar mode);
	virtual QChar getUserModeRequirement(QChar mode);
	virtual char getRegisterModeChar() { return 'r'; };
	virtual const char * getSoftware() { return "InspIRCd"; };
	virtual bool getNeedsOpToListModeseI() { return true; };
	virtual bool getNeedsOperToSetS() { return true; };
};

class KVIRC_API KviIrcConnectionServerInfo
{
	friend class KviConsoleWindow; // for now
	friend class KviIrcServerParser;
	friend class KviIrcConnection;

protected:
	KviIrcConnectionServerInfo();
	~KviIrcConnectionServerInfo();

private:
	KviBasicIrcServerInfo * m_pServInfo;
	QString m_szNetworkName;           // the most actual network name (may be the one we specify or the one that the server reports)
	QString m_szName;                  // the most actual server name (may be the one we specify or the one that the server wants to be known as)
	QString m_szSupportedUserModes;    // the supported user modes
	QString m_szSupportedChannelModes; // the supported channel modes (all of them)
	QString m_szSupportedModePrefixes; // the actually used mode prefixes  @+
	kvi_u32_t * m_pModePrefixTable;    // the mode prefixes above in a table
	unsigned int m_uPrefixes;
	QString m_szSupportedModeFlags;    // the actually used mode flags     ov
	QString m_szSupportedChannelTypes; // the supported channel types
	bool m_bSupportsWatchList;         // supports the watch list ?
	bool m_bSupportsCodePages;         // supports the /CODEPAGE command ?
	int m_iMaxTopicLen;
	int m_iMaxModeChanges;
	// Mode that adds or removes a nick or address to a list. Always has a parameter (eg: "b" as ban)
	QString m_szListModes;
	// Mode that changes a setting and always has a parameter (eg: "k" as channel key)
	QString m_szParameterModes;
	// Mode that changes a setting and only has a parameter when set (eg: "l" as channel limit)
	QString m_szParameterWhenSetModes;
	// Mode that changes a setting and never has a parameter (eg: "m" as channel moderated)
	QString m_szPlainModes;
	bool m_bSupportsCap;
	QStringList m_lSupportedCaps;
	bool m_bSupportsWhox; // supports WHOX
public:
	char registerModeChar() { return m_pServInfo ? m_pServInfo->getRegisterModeChar() : 0; };
	const char * software() { return m_pServInfo ? m_pServInfo->getSoftware() : 0; };
	bool getNeedsOpToListModeseI() { return m_pServInfo ? m_pServInfo->getNeedsOpToListModeseI() : false; };
	bool getNeedsOperToSetS() { return m_pServInfo ? m_pServInfo->getNeedsOperToSetS() : false; };
	const QString & name() { return m_szName; };
	const QString & networkName() { return m_szNetworkName; };
	const QString & supportedUserModes() { return m_szSupportedUserModes; };
	const QString & supportedChannelModes() { return m_szSupportedChannelModes; };
	const QString & supportedChannelTypes() { return m_szSupportedChannelTypes; };
	const QString & supportedModePrefixes() { return m_szSupportedModePrefixes; };
	const QString & supportedModeFlags() { return m_szSupportedModeFlags; };
	const QString & supportedListModes() { return m_szListModes; };
	const QString & supportedParameterModes() { return m_szParameterModes; };
	const QString & supportedParameterWhenSetModes() { return m_szParameterWhenSetModes; };
	const QString & supportedPlainModes() { return m_szPlainModes; };
	bool supportsCap() { return m_bSupportsCap; };
	const QStringList & supportedCaps() { return m_lSupportedCaps; };
	bool supportsWatchList() { return m_bSupportsWatchList; };
	bool supportsCodePages() { return m_bSupportsCodePages; };
	bool supportsWhox() { return m_bSupportsWhox; };

	int maxTopicLen() { return m_iMaxTopicLen; };
	int maxModeChanges() { return m_iMaxModeChanges; };

	void setServerVersion(const QString & version);

	const QString & getChannelModeDescription(char mode) { return m_pServInfo->getChannelModeDescription(mode); };
	const QString & getUserModeDescription(QChar mode) { return m_pServInfo->getUserModeDescription(mode); };

	// Returning ! means the mode can never be set by the user. Returning QChar::Null means the mode is free to set.
	// Returning a QChar means the mode has another mode dependency (the QChar we're returning)
	QChar getUserModeRequirement(QChar mode) { return m_pServInfo ? m_pServInfo->getUserModeRequirement(mode) : QChar::Null; };

	bool isSupportedChannelType(QChar c);
	bool isSupportedModePrefix(QChar c);
	bool isSupportedModeFlag(QChar c);
	QChar modePrefixChar(kvi_u32_t flag);
	QChar modeFlagChar(kvi_u32_t flag);
	kvi_u32_t modeFlagFromPrefixChar(QChar c);
	kvi_u32_t modeFlagFromModeChar(QChar c);

protected:
	void setNetworkName(const QString & szName) { m_szNetworkName = szName; };
	void setName(const QString & szName) { m_szName = szName; };
	void setSupportedUserModes(const QString & szSupportedUserModes) { m_szSupportedUserModes = szSupportedUserModes; };
	void setSupportedChannelModes(const QString & szSupportedChannelModes);
	void setSupportedModePrefixes(const QString & szSupportedModePrefixes, const QString & szSupportedModeFlags);
	void setSupportedChannelTypes(const QString & szSupportedChannelTypes) { m_szSupportedChannelTypes = szSupportedChannelTypes; };
	void setSupportsWatchList(bool bSupportsWatchList) { m_bSupportsWatchList = bSupportsWatchList; };
	void setSupportsCodePages(bool bSupportsCodePages) { m_bSupportsCodePages = bSupportsCodePages; };
	void addSupportedCaps(const QString & szCapList);
	void setMaxTopicLen(int iTopLen) { m_iMaxTopicLen = iTopLen; };
	void setMaxModeChanges(int iModes) { m_iMaxModeChanges = iModes; };
	void setSupportsWhox(bool bSupportsWhox) { m_bSupportsWhox = bSupportsWhox; };
private:
	void buildModePrefixTable();
};

#endif //!_KVI_IRCCONNECTIONSERVERINFO_H_
