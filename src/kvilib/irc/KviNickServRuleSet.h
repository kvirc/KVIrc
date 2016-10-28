#ifndef _KVI_NICKSERV_RULE_SET_H_
#define _KVI_NICKSERV_RULE_SET_H_
//=============================================================================
//
//   File : KviNickServRuleSet.h
//   Creation date : Thu Aug 09 2001 16:43:56 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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
#include "KviHeapObject.h"
#include "KviNickServRule.h"
#include "KviPointerList.h"

class KviConfigurationFile;
class KviIrcMask;

class QString;

class KVILIB_API KviNickServRuleSet : public KviHeapObject
{
public:
	KviNickServRuleSet();
	KviNickServRuleSet(const KviNickServRuleSet & s);
	~KviNickServRuleSet();

protected:
	KviPointerList<KviNickServRule> * m_pRules; // FIXME: Replace with KviPointerHashTable<QString,KviPointerList>
	bool m_bEnabled;

public:
	// avoid crashes under windows
	static KviNickServRuleSet * createInstance();
	void clear();
	bool isEnabled() { return m_bEnabled; };
	void setEnabled(bool bEnabled) { m_bEnabled = bEnabled; };
	bool isEmpty() { return m_pRules ? m_pRules->isEmpty() : true; };
	void addRule(KviNickServRule * r);
	KviNickServRule * matchRule(const QString & szNick, const KviIrcMask * nickServ, const QString & szMsg, const QString & szServer = QString::null);
	void copyFrom(const KviNickServRuleSet & src);
	void load(const QString & szConfigFile);
	void save(const QString & szConfigFile);
	void save(KviConfigurationFile * cfg, const QString & prefix);
	KviPointerList<KviNickServRule> * rules() { return m_pRules; };
	static KviNickServRuleSet * load(KviConfigurationFile * cfg, const QString & prefix);

protected:
	bool loadPrivate(KviConfigurationFile * cfg, const QString & prefix, unsigned int nEntries);
};

#endif // _KVI_NICKSERV_RULE_SET_H_
