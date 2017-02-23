//=============================================================================
//
//   File : KviNickServRuleSet.cpp
//   Creation date : Thu Aug 09 2001 17:44:56 by Szymon Stefanek
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

#include "KviNickServRuleSet.h"
#include "KviConfigurationFile.h"
#include "KviIrcMask.h"
#include "KviQString.h"

#include <QRegExp>

/*
	@doc: nickserv_proto
	@title:
		Authentication with NickServ
	@keyterms:
		NickServ, automatic authentication with NickServ
	@type:
		generic
	@short:
		Automatic authentication with NickServ
	@body:
		KVIrc supports automatic authentication with the NickServ service.[br]
		This service is commonly implemented on major IRC networks: basically
		it is a program that allows users to register their nickname and protect
		it from being stolen by others.[br] The NickServ protocol is
		not standardized (at the time that I'm writing this doc) and automatic
		authentication is a pure experimental protocol.[br]
		Once you get on IRC with a registered nickname, the NickServ will
		ask you for identification by sending you a NOTICE.[br]
		The message will look in a way similar to the following:[br]
		<b>You're using a registered nickname: if this is your nick,
		please type /msg NickServ IDENTIFY password, otherwise please
		choose another nickname</b>.[br]
		The message is often broken in two or three lines of text.[br]
		Please note that many network policies suggest to avoid automatic authentication
		with NickServ.[br]I have implemented it because I know that it works on the networks
		that I'm usually on.[br]You have to check that this protocol works on your network and
		then eventually use it at your own risk.[br]
*/

// FIXME: The doc above is a bit outdated, fix it

KviNickServRuleSet::KviNickServRuleSet()
    : KviHeapObject()
{
	m_bEnabled = false;
	m_pRules = nullptr;
}

KviNickServRuleSet::KviNickServRuleSet(const KviNickServRuleSet & s)
{
	m_pRules = nullptr;
	copyFrom(s);
}

KviNickServRuleSet::~KviNickServRuleSet()
{
	if(m_pRules)
		delete m_pRules;
}

void KviNickServRuleSet::save(KviConfigurationFile * pCfg, const QString & szPrefix)
{
	if(!m_pRules)
		return; // nothing to save
	if(m_pRules->isEmpty())
		return; // should never happen anyway
	QString szTmp;
	if(m_bEnabled)
	{
		szTmp = QString("%1NSEnabled").arg(szPrefix);
		pCfg->writeEntry(szTmp, m_bEnabled);
	}
	szTmp = QString("%1NSRules").arg(szPrefix);
	pCfg->writeEntry(szTmp, m_pRules->count());
	int i = 0;
	for(KviNickServRule * pRule = m_pRules->first(); pRule; pRule = m_pRules->next())
	{
		szTmp = QString("%1NSRule%2_").arg(szPrefix).arg(i);
		pRule->save(pCfg, szTmp);
		i++;
	}
}

KviNickServRuleSet * KviNickServRuleSet::load(KviConfigurationFile * pCfg, const QString & szPrefix)
{
	QString szTmp;
	szTmp = QString("%1NSRules").arg(szPrefix);
	unsigned int uCount = pCfg->readUIntEntry(szTmp, 0);
	if(uCount == 0)
		return nullptr;
	KviNickServRuleSet * pSet = new KviNickServRuleSet();
	if(pSet->loadPrivate(pCfg, szPrefix, uCount))
		return pSet;
	delete pSet;
	return nullptr;
}

void KviNickServRuleSet::load(const QString & szConfigFile)
{
	clear();
	KviConfigurationFile cfg(szConfigFile, KviConfigurationFile::Read);

	QString szTmp = "NSRules";
	unsigned int uCount = cfg.readUIntEntry(szTmp, 0);
	if(uCount == 0)
		return;
	loadPrivate(&cfg, QString(""), uCount);
}

void KviNickServRuleSet::save(const QString & szConfigFile)
{
	KviConfigurationFile cfg(szConfigFile, KviConfigurationFile::Write);
	cfg.clear();
	save(&cfg, QString(""));
}

bool KviNickServRuleSet::loadPrivate(KviConfigurationFile * pCfg, const QString & szPrefix, unsigned int uEntries)
{
	if(m_pRules)
		m_pRules->clear();
	else
	{
		m_pRules = new KviPointerList<KviNickServRule>;
		m_pRules->setAutoDelete(true);
	}

	if(uEntries != 0)
	{
		QString szTmp;
		szTmp = QString("%1NSEnabled").arg(szPrefix);
		m_bEnabled = pCfg->readBoolEntry(szTmp, false);
		for(unsigned int u = 0; u < uEntries; u++)
		{
			szTmp = QString("%1NSRule%2_").arg(szPrefix).arg(u);
			KviNickServRule * pRule = new KviNickServRule();
			if(!pRule->load(pCfg, szTmp))
				delete pRule;
			else
				m_pRules->append(pRule);
		}
	}

	if(m_pRules->isEmpty())
	{
		m_bEnabled = false;
		delete m_pRules;
		m_pRules = nullptr;
		return false;
	}
	return true;
}

void KviNickServRuleSet::clear()
{
	if(m_pRules)
	{
		delete m_pRules;
		m_pRules = nullptr;
	}
	m_bEnabled = false;
}

void KviNickServRuleSet::addRule(KviNickServRule * r)
{
	if(!m_pRules)
	{
		m_pRules = new KviPointerList<KviNickServRule>;
		m_pRules->setAutoDelete(true);
	}
	m_pRules->append(r);
}

KviNickServRuleSet * KviNickServRuleSet::createInstance()
{
	return new KviNickServRuleSet();
}

KviNickServRule * KviNickServRuleSet::matchRule(const QString & szNick, const KviIrcMask * pNickServ, const QString & szMsg, const QString & szServer)
{
	if(!m_pRules)
		return nullptr;

	for(KviNickServRule * r = m_pRules->first(); r; r = m_pRules->next())
	{
		if(!KviQString::matchString(r->registeredNick(), szNick, false, true))
			continue;

		if(!szServer.isEmpty())
		{
			QRegExp res(r->serverMask(), Qt::CaseInsensitive, QRegExp::Wildcard);
			if(!res.exactMatch(szServer))
				continue;
		}
		if(!pNickServ->matchedBy(KviIrcMask(r->nickServMask())))
			continue;
		QRegExp re(r->messageRegexp(), Qt::CaseInsensitive, QRegExp::Wildcard);
		if(re.exactMatch(szMsg))
			return r;
	}
	return nullptr;
}

void KviNickServRuleSet::copyFrom(const KviNickServRuleSet & src)
{
	if(src.m_pRules)
	{
		if(m_pRules)
			m_pRules->clear();
		else
		{
			m_pRules = new KviPointerList<KviNickServRule>;
			m_pRules->setAutoDelete(true);
		}
		for(KviNickServRule * r = src.m_pRules->first(); r; r = src.m_pRules->next())
		{
			KviNickServRule * c = new KviNickServRule();
			c->copyFrom(*r);
			m_pRules->append(c);
		}
		if(m_pRules->isEmpty())
		{
			m_bEnabled = false;
			delete m_pRules;
			m_pRules = nullptr;
		}
		else
		{
			m_bEnabled = src.m_bEnabled;
		}
	}
	else
	{
		m_bEnabled = false;
		if(m_pRules)
		{
			delete m_pRules;
			m_pRules = nullptr;
		}
	}
}
