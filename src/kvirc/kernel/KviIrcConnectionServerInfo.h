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
	KviBasicIrcServerInfo(KviIrcConnectionServerInfo * pParent = nullptr, QString version = KviQString::Empty);
	virtual ~KviBasicIrcServerInfo();

public:
	virtual const QString & getChannelModeDescription(char mode) const;
	virtual const QString & getUserModeDescription(QChar mode) const;
	virtual QChar getUserModeRequirement(QChar mode) const;
	virtual char getRegisterModeChar() const { return 0; }
	virtual const char * getSoftware() const { return "Ircd"; }
	virtual bool getNeedsOpToListModeseI() const { return false; }
	virtual bool getNeedsOperToSetS() const { return false; }
};

//
// Hybrid + forks
//

class KVIRC_API KviHybridServerInfo : public KviBasicIrcServerInfo
{
	// This is a major IRCd that most modern forks are based off of
public:
	KviHybridServerInfo(KviIrcConnectionServerInfo * pParent = nullptr, const QString & version = KviQString::Empty)
	    : KviBasicIrcServerInfo(pParent, version) {}
	const QString & getChannelModeDescription(char mode) const override;
	const QString & getUserModeDescription(QChar mode) const override;
	QChar getUserModeRequirement(QChar mode) const override;
	char getRegisterModeChar() const override { return 'r'; }
	const char * getSoftware() const override { return "Hybrid"; }
};

class KVIRC_API KviIrcdRatboxIrcServerInfo : public KviHybridServerInfo
{
	// efnet
public:
	KviIrcdRatboxIrcServerInfo(KviIrcConnectionServerInfo * pParent = nullptr, const QString & version = KviQString::Empty)
	    : KviHybridServerInfo(pParent, version) {}
	const QString & getChannelModeDescription(char mode) const override;
	const QString & getUserModeDescription(QChar mode) const override;
	QChar getUserModeRequirement(QChar mode) const override;
	const char * getSoftware() const override { return "Ircd-ratbox"; }
	bool getNeedsOpToListModeseI() const override { return true; }
};

class KVIRC_API KviCharybdisServerInfo : public KviIrcdRatboxIrcServerInfo
{
public:
	KviCharybdisServerInfo(KviIrcConnectionServerInfo * pParent = nullptr, const QString & version = KviQString::Empty)
	    : KviIrcdRatboxIrcServerInfo(pParent, version) {}
	const QString & getChannelModeDescription(char mode) const override;
	const QString & getUserModeDescription(QChar mode) const override;
	QChar getUserModeRequirement(QChar mode) const override;
	const char * getSoftware() const override { return "Charybdis"; }
	bool getNeedsOperToSetS() const override { return true; }
};

class KVIRC_API KviIrcdSevenIrcServerInfo : public KviCharybdisServerInfo
{
	// freenode
public:
	KviIrcdSevenIrcServerInfo(KviIrcConnectionServerInfo * pParent = nullptr, const QString & version = KviQString::Empty)
	    : KviCharybdisServerInfo(pParent, version) {}
	const QString & getUserModeDescription(QChar mode) const override;
	QChar getUserModeRequirement(QChar mode) const override;
	const char * getSoftware() const override { return "Ircd-seven"; }
};

class KVIRC_API KviPlexusIrcServerInfo : public KviHybridServerInfo
{
	// rizon; note: plexus is an extension to hybrid
public:
	KviPlexusIrcServerInfo(KviIrcConnectionServerInfo * pParent = nullptr, const QString & version = KviQString::Empty)
	    : KviHybridServerInfo(pParent, version) {}
	const QString & getChannelModeDescription(char mode) const override;
	const QString & getUserModeDescription(QChar mode) const override;
	QChar getUserModeRequirement(QChar mode) const override;
	const char * getSoftware() const override { return "Plexus"; }
	bool getNeedsOpToListModeseI() const override { return true; }
};

class KVIRC_API KviOftcIrcServerInfo : public KviHybridServerInfo
{
	// oftc; note: hybrid+oftc is an extension to hybrid
public:
	KviOftcIrcServerInfo(KviIrcConnectionServerInfo * pParent = nullptr, const QString & version = KviQString::Empty)
	    : KviHybridServerInfo(pParent, version) {}
	const QString & getChannelModeDescription(char mode) const override;
	const QString & getUserModeDescription(QChar mode) const override;
	QChar getUserModeRequirement(QChar mode) const override;
	char getRegisterModeChar() const override { return 'R'; }
	const char * getSoftware() const override { return "Hybrid+Oftc"; }
	bool getNeedsOpToListModeseI() const override { return true; }
};

//
// P10
//

class KVIRC_API KviIrcuIrcServerInfo : public KviBasicIrcServerInfo
{
	// undernet
public:
	KviIrcuIrcServerInfo(KviIrcConnectionServerInfo * pParent = nullptr, const QString & version = KviQString::Empty)
	    : KviBasicIrcServerInfo(pParent, version) {}
	const QString & getChannelModeDescription(char mode) const override;
	const QString & getUserModeDescription(QChar mode) const override;
	QChar getUserModeRequirement(QChar mode) const override;
	const char * getSoftware() const override { return "Ircu"; }
	bool getNeedsOpToListModeseI() const override { return true; }
	bool getNeedsOperToSetS() const override { return true; }
};

class KVIRC_API KviSnircdIrcServerInfo : public KviIrcuIrcServerInfo
{
	// quakenet; note: snird is an extension to ircu
public:
	KviSnircdIrcServerInfo(KviIrcConnectionServerInfo * pParent = nullptr, const QString & version = KviQString::Empty)
	    : KviIrcuIrcServerInfo(pParent, version) {}
	const QString & getChannelModeDescription(char mode) const override;
	const QString & getUserModeDescription(QChar mode) const override;
	QChar getUserModeRequirement(QChar mode) const override;
	const char * getSoftware() const override { return "Snircd"; }
	bool getNeedsOperToSetS() const override { return true; }
};

class KVIRC_API KviDarenetIrcServerInfo : public KviIrcuIrcServerInfo
{
	// darenet; note: u2+ircd-darenet is an extension to ircu
public:
	KviDarenetIrcServerInfo(KviIrcConnectionServerInfo * pParent = nullptr, const QString & version = KviQString::Empty)
	    : KviIrcuIrcServerInfo(pParent, version) {}
	const QString & getChannelModeDescription(char mode) const override;
	const QString & getUserModeDescription(QChar mode) const override;
	QChar getUserModeRequirement(QChar mode) const override;
	char getRegisterModeChar() const override { return 'r'; }
	const char * getSoftware() const override { return "Ircu+Darenet"; }
};

//
// Unreal -> Unreal 3.2 -> Unreal 4.0
//

class KVIRC_API KviUnrealIrcServerInfo : public KviBasicIrcServerInfo
{
public:
	KviUnrealIrcServerInfo(KviIrcConnectionServerInfo * pParent = nullptr, const QString & version = KviQString::Empty)
	    : KviBasicIrcServerInfo(pParent, version) {}
	const QString & getChannelModeDescription(char mode) const override;
	char getRegisterModeChar() const override { return 'r'; }
	const char * getSoftware() const override { return "Unreal"; }
	bool getNeedsOperToSetS() const override { return true; }
};

class KVIRC_API KviUnreal32IrcServerInfo : public KviUnrealIrcServerInfo
{
	// This is a continuation on to Unreal, so use its predecessor
	// as a base class.
public:
	KviUnreal32IrcServerInfo(KviIrcConnectionServerInfo * pParent = nullptr, const QString & version = KviQString::Empty)
	    : KviUnrealIrcServerInfo(pParent, version) {}
	const QString & getChannelModeDescription(char mode) const override;
	const QString & getUserModeDescription(QChar mode) const override;
	QChar getUserModeRequirement(QChar mode) const override;
	const char * getSoftware() const override { return "Unreal32"; }
};

class KVIRC_API KviUnreal40IrcServerInfo : public KviUnreal32IrcServerInfo
{
public:
	KviUnreal40IrcServerInfo(KviIrcConnectionServerInfo * pParent = nullptr, const QString & version = KviQString::Empty)
	    : KviUnreal32IrcServerInfo(pParent, version) {}
	const QString & getChannelModeDescription(char mode) const override;
	const QString & getUserModeDescription(QChar mode) const override;
	const char * getSoftware() const override { return "Unreal40"; }
};

class KVIRC_API KviCritenIrcServerInfo : public KviBasicIrcServerInfo
{
	// abjects
public:
	KviCritenIrcServerInfo(KviIrcConnectionServerInfo * pParent = nullptr, const QString & version = KviQString::Empty)
	    : KviBasicIrcServerInfo(pParent, version) {}
	const QString & getChannelModeDescription(char mode) const override;
	const QString & getUserModeDescription(QChar mode) const override;
	QChar getUserModeRequirement(QChar mode) const override;
	char getRegisterModeChar() const override { return 'r'; }
	const char * getSoftware() const override { return "Criten"; }
	bool getNeedsOpToListModeseI() const override { return true; }
};

class KVIRC_API KviNemesisIrcServerInfo : public KviCritenIrcServerInfo
{
	// criten
public:
	KviNemesisIrcServerInfo(KviIrcConnectionServerInfo * pParent = nullptr, const QString & version = KviQString::Empty)
	    : KviCritenIrcServerInfo(pParent, version) {}
	const char * getSoftware() const override { return "Nemesis"; }
};

class KVIRC_API KviNemesis20IrcServerInfo : public KviUnreal32IrcServerInfo
{
public:
	KviNemesis20IrcServerInfo(KviIrcConnectionServerInfo * pParent = nullptr, const QString & version = KviQString::Empty)
	    : KviUnreal32IrcServerInfo(pParent, version) {}
	const char * getSoftware() const override { return "Nemesis2.0"; }
};

class KVIRC_API KviBahamutIrcServerInfo : public KviBasicIrcServerInfo
{
	// dalnet, azzurranet
public:
	KviBahamutIrcServerInfo(KviIrcConnectionServerInfo * pParent = nullptr, const QString & version = KviQString::Empty)
	    : KviBasicIrcServerInfo(pParent, version) {}
	const QString & getChannelModeDescription(char mode) const override;
	const QString & getUserModeDescription(QChar mode) const override;
	QChar getUserModeRequirement(QChar mode) const override;
	char getRegisterModeChar() const override { return 'r'; }
	const char * getSoftware() const override { return "Bahamut"; }
};

class KVIRC_API KviHyperionIrcServerInfo : public KviBasicIrcServerInfo
{
	// legacy freenode : no longer maintained
public:
	KviHyperionIrcServerInfo(KviIrcConnectionServerInfo * pParent = nullptr, const QString & version = KviQString::Empty)
	    : KviBasicIrcServerInfo(pParent, version) {}
	const QString & getUserModeDescription(QChar mode) const override;
	QChar getUserModeRequirement(QChar mode) const override;
	char getRegisterModeChar() const override { return 'e'; }
	const char * getSoftware() const override { return "Hyperion"; }
	bool getNeedsOpToListModeseI() const override { return true; }
};

class KVIRC_API KviInspIRCdIrcServerInfo : public KviBasicIrcServerInfo
{
	// chatspike
public:
	KviInspIRCdIrcServerInfo(KviIrcConnectionServerInfo * pParent = nullptr, const QString & version = KviQString::Empty)
	    : KviBasicIrcServerInfo(pParent, version) {}
	const QString & getChannelModeDescription(char mode) const override;
	const QString & getUserModeDescription(QChar mode) const override;
	QChar getUserModeRequirement(QChar mode) const override;
	char getRegisterModeChar() const override { return 'r'; }
	const char * getSoftware() const override { return "InspIRCd"; }
	bool getNeedsOpToListModeseI() const override { return true; }
	bool getNeedsOperToSetS() const override { return true; }
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
	QString m_szSupportedChannelModes = "pstnmiklb"; // the supported channel modes (all of them)
	QString m_szSupportedModePrefixes = "@+";        // the actually used mode prefixes  @+
	QString m_szSupportedStatusMsgPrefixes;          // mode prefixes that can be used to target messages (from the STATUSMSG ISUPPORT token)
	kvi_u32_t * m_pModePrefixTable = nullptr;        // the mode prefixes above in a table
	unsigned int m_uPrefixes;
	QString m_szSupportedModeFlags = "ov";      // the actually used mode flags     ov
	QString m_szSupportedChannelTypes = "#&!+"; // the supported channel types
	bool m_bSupportsWatchList = false;          // supports the watch list ?
	bool m_bSupportsCodePages = false;          // supports the /CODEPAGE command ?
	int m_iMaxTopicLen = -1;
	int m_iMaxModeChanges = 3;
	// Mode that adds or removes a nick or address to a list. Always has a parameter (eg: "b" as ban)
	QString m_szListModes = "b";
	// Mode that changes a setting and always has a parameter (eg: "k" as channel key)
	QString m_szParameterModes = "k";
	// Mode that changes a setting and only has a parameter when set (eg: "l" as channel limit)
	QString m_szParameterWhenSetModes = "l";
	// Mode that changes a setting and never has a parameter (eg: "m" as channel moderated)
	QString m_szPlainModes = "pstnmi";
	bool m_bSupportsCap = false;
	QStringList m_lSupportedCaps;
	bool m_bSupportsWhox = false; // supports WHOX
public:
	char registerModeChar() const { return m_pServInfo ? m_pServInfo->getRegisterModeChar() : 0; }
	const char * software() const { return m_pServInfo ? m_pServInfo->getSoftware() : 0; }
	bool getNeedsOpToListModeseI() const { return m_pServInfo ? m_pServInfo->getNeedsOpToListModeseI() : false; }
	bool getNeedsOperToSetS() const { return m_pServInfo ? m_pServInfo->getNeedsOperToSetS() : false; }
	const QString & name() const { return m_szName; }
	const QString & networkName() const { return m_szNetworkName; }
	const QString & supportedUserModes() const { return m_szSupportedUserModes; }
	const QString & supportedChannelModes() const { return m_szSupportedChannelModes; }
	const QString & supportedChannelTypes() const { return m_szSupportedChannelTypes; }
	const QString & supportedModePrefixes() const { return m_szSupportedModePrefixes; }
	const QString & supportedStatusMsgPrefixes() const { return m_szSupportedStatusMsgPrefixes; }
	const QString & supportedModeFlags() const { return m_szSupportedModeFlags; }
	const QString & supportedListModes() const { return m_szListModes; }
	const QString & supportedParameterModes() const { return m_szParameterModes; }
	const QString & supportedParameterWhenSetModes() const { return m_szParameterWhenSetModes; }
	const QString & supportedPlainModes() const { return m_szPlainModes; }
	bool supportsCap() const { return m_bSupportsCap; }
	const QStringList & supportedCaps() const { return m_lSupportedCaps; }
	bool supportsWatchList() const { return m_bSupportsWatchList; }
	bool supportsCodePages() const { return m_bSupportsCodePages; }
	bool supportsWhox() const { return m_bSupportsWhox; }

	int maxTopicLen() const { return m_iMaxTopicLen; }
	int maxModeChanges() const { return m_iMaxModeChanges; }

	void setServerVersion(const QString & version);

	const QString & getChannelModeDescription(char mode) const { return m_pServInfo->getChannelModeDescription(mode); }
	const QString & getUserModeDescription(QChar mode) const { return m_pServInfo->getUserModeDescription(mode); }

	// Returning ! means the mode can never be set by the user. Returning QChar::Null means the mode is free to set.
	// Returning a QChar means the mode has another mode dependency (the QChar we're returning)
	QChar getUserModeRequirement(QChar mode) const { return m_pServInfo ? m_pServInfo->getUserModeRequirement(mode) : QChar::Null; }

	bool isSupportedChannelType(QChar c) const;
	bool isSupportedModePrefix(QChar c) const;
	bool isSupportedModeFlag(QChar c) const;
	QChar modePrefixChar(kvi_u32_t flag) const;
	QChar modeFlagChar(kvi_u32_t flag) const;
	kvi_u32_t modeFlagFromPrefixChar(QChar c) const;
	kvi_u32_t modeFlagFromModeChar(QChar c) const;

protected:
	void setNetworkName(const QString & szName) { m_szNetworkName = szName; }
	void setName(const QString & szName) { m_szName = szName; }
	void setSupportedUserModes(const QString & szSupportedUserModes) { m_szSupportedUserModes = szSupportedUserModes; }
	void setSupportedChannelModes(const QString & szSupportedChannelModes);
	void setSupportedModePrefixes(const QString & szSupportedModePrefixes, const QString & szSupportedModeFlags);
	void setSupportedStatusMsgPrefixes(const QString & szSupportedStatusMsgPrefixes) { m_szSupportedStatusMsgPrefixes = szSupportedStatusMsgPrefixes; }
	void setSupportedChannelTypes(const QString & szSupportedChannelTypes) { m_szSupportedChannelTypes = szSupportedChannelTypes; }
	void setSupportsWatchList(bool bSupportsWatchList) { m_bSupportsWatchList = bSupportsWatchList; }
	void setSupportsCodePages(bool bSupportsCodePages) { m_bSupportsCodePages = bSupportsCodePages; }
	void addSupportedCaps(const QString & szCapList);
	void setMaxTopicLen(int iTopLen) { m_iMaxTopicLen = iTopLen; }
	void setMaxModeChanges(int iModes) { m_iMaxModeChanges = iModes; }
	void setSupportsWhox(bool bSupportsWhox) { m_bSupportsWhox = bSupportsWhox; }
private:
	void buildModePrefixTable();
};

#endif //!_KVI_IRCCONNECTIONSERVERINFO_H_
