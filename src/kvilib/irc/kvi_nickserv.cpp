//=============================================================================
//
//   File : kvi_nickserv.cpp
//   Creation date : Thu Aug 09 2001 17:44:56 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2008 Szymon Stefanek (pragma at kvirc dot net)
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



#include "kvi_nickserv.h"
#include "kvi_config.h"
#include "kvi_ircmask.h"

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
		Once you get on IRC with a registered nickname , the NickServ will
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


// FIXME: The doc above is a bit outdated , fix it

KviNickServRuleSet::KviNickServRuleSet()
: KviHeapObject()
{
	m_bEnabled = false;
	m_pRules = 0;
}

KviNickServRuleSet::KviNickServRuleSet(const KviNickServRuleSet &s)
{
	m_pRules = 0;
	copyFrom(s);
}


KviNickServRuleSet::~KviNickServRuleSet()
{
	if(m_pRules)delete m_pRules;
}

void KviNickServRuleSet::save(KviConfig * cfg,const QString &prefix)
{
	if(!m_pRules)return; // nothing to save
	if(m_pRules->isEmpty())return; // should never happen anyway
	QString tmp;
	if(m_bEnabled)
	{
		KviQString::sprintf(tmp,"%QNSEnabled",&prefix);
		cfg->writeEntry(tmp,m_bEnabled);
	}
	KviQString::sprintf(tmp,"%QNSRules",&prefix);
	cfg->writeEntry(tmp,m_pRules->count());
	int idx = 0;
	for(KviNickServRule * r = m_pRules->first();r;r = m_pRules->next())
	{
		KviQString::sprintf(tmp,"%QNSRule%d_",&prefix,idx);
		r->save(cfg,tmp);
		idx++;
	}
}

KviNickServRuleSet * KviNickServRuleSet::load(KviConfig * cfg,const QString &prefix)
{
	QString tmp;
	KviQString::sprintf(tmp,"%QNSRules",&prefix);
	unsigned int cnt = cfg->readUIntEntry(tmp,0);
	if(cnt == 0)return 0;
	KviNickServRuleSet * s = new KviNickServRuleSet();
	if(s->loadPrivate(cfg,prefix,cnt))return s;
	delete s;
	return 0;
}

void KviNickServRuleSet::load(const QString &szConfigFile)
{
	clear();
	KviConfig cfg(szConfigFile,KviConfig::Read);

	QString tmp;
	KviQString::sprintf(tmp,"NSRules");
	unsigned int cnt = cfg.readUIntEntry(tmp,0);
	if(cnt == 0)return;
	loadPrivate(&cfg,QString(""),cnt);
}

void KviNickServRuleSet::save(const QString &szConfigFile)
{
	KviConfig cfg(szConfigFile,KviConfig::Write);
	cfg.clear();
	save(&cfg,QString(""));
}

bool KviNickServRuleSet::loadPrivate(KviConfig * cfg,const QString &prefix,unsigned int nEntries)
{
	if(m_pRules)m_pRules->clear();
	else {
		m_pRules = new KviPointerList<KviNickServRule>;
		m_pRules->setAutoDelete(true);
	}

	if(nEntries != 0)
	{
		QString tmp;
		KviQString::sprintf(tmp,"%QNSEnabled",&prefix);
		m_bEnabled = cfg->readBoolEntry(tmp,false);
		for(unsigned int u=0;u<nEntries;u++)
		{
			KviQString::sprintf(tmp,"%QNSRule%u_",&prefix,u);
			KviNickServRule * r = new KviNickServRule();
			if(!r->load(cfg,tmp))delete r;
			else m_pRules->append(r);
		}
	}

	if(m_pRules->isEmpty())
	{
		m_bEnabled = false;
		delete m_pRules;
		m_pRules = 0;
		return false;
	}
	return true;
}

void KviNickServRuleSet::clear()
{
	if(m_pRules)
	{
		delete m_pRules;
		m_pRules = 0;
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
	if(!m_pRules) return 0;
	
	for(KviNickServRule *r = m_pRules->first();r;r = m_pRules->next())
	{
		if(!KviQString::matchString(r->registeredNick(),szNick,false,true))
			continue;
		
		if(!szServer.isEmpty())
		{
			QRegExp res(r->serverMask(),Qt::CaseInsensitive,QRegExp::Wildcard);
			if(!res.exactMatch(szServer))
				continue;
		}
		if(!pNickServ->matchedBy(KviIrcMask(r->nickServMask())))
			continue;
		QRegExp re(r->messageRegexp(),Qt::CaseInsensitive,QRegExp::Wildcard);
		if(re.exactMatch(szMsg)) return r;
	}
	return 0;
}

void KviNickServRuleSet::copyFrom(const KviNickServRuleSet &src)
{
	if(src.m_pRules)
	{
		if(m_pRules)m_pRules->clear();
		else {
			m_pRules = new KviPointerList<KviNickServRule>;
			m_pRules->setAutoDelete(true);
		}
		for(KviNickServRule * r = src.m_pRules->first();r;r = src.m_pRules->next())
		{
			KviNickServRule * c = new KviNickServRule();
			c->copyFrom(*r);
			m_pRules->append(c);
		}
		if(m_pRules->isEmpty())
		{
			m_bEnabled = false;
			delete m_pRules;
			m_pRules = 0;
		} else {
			m_bEnabled = src.m_bEnabled;
		}
	} else {
		m_bEnabled = false;
		if(m_pRules)
		{
			delete m_pRules;
			m_pRules = 0;
		}
	}
}


void KviNickServRule::copyFrom(const KviNickServRule &src)
{
	m_szRegisteredNick = src.m_szRegisteredNick;
	m_szNickServMask = src.m_szNickServMask;
	m_szMessageRegexp = src.m_szMessageRegexp;
	m_szIdentifyCommand = src.m_szIdentifyCommand;
	m_szServerMask = src.m_szServerMask;
}

void KviNickServRule::save(KviConfig * cfg,const QString &prefix)
{
	QString tmp;
	KviQString::sprintf(tmp,"%QRegisteredNick",&prefix);
	cfg->writeEntry(tmp,m_szRegisteredNick);
	KviQString::sprintf(tmp,"%QNickServMask",&prefix);
	cfg->writeEntry(tmp,m_szNickServMask);
	KviQString::sprintf(tmp,"%QMessageRegexp",&prefix);
	cfg->writeEntry(tmp,m_szMessageRegexp);
	KviQString::sprintf(tmp,"%QIdentifyCommand",&prefix);
	cfg->writeEntry(tmp,m_szIdentifyCommand);
	KviQString::sprintf(tmp,"%QServerMask",&prefix);
	cfg->writeEntry(tmp,m_szServerMask);
}

bool KviNickServRule::load(KviConfig * cfg,const QString &prefix)
{
	QString tmp;
	KviQString::sprintf(tmp,"%QRegisteredNick",&prefix);
	m_szRegisteredNick = KviQString::trimmed(cfg->readQStringEntry(tmp));
	if(m_szRegisteredNick.isEmpty())return false;
	KviQString::sprintf(tmp,"%QNickServMask",&prefix);
	m_szNickServMask = cfg->readQStringEntry(tmp);
	if(m_szNickServMask.isEmpty())return false;
	KviQString::sprintf(tmp,"%QServerMask",&prefix);
	m_szServerMask = cfg->readQStringEntry(tmp,QString());
	KviQString::sprintf(tmp,"%QMessageRegexp",&prefix);
	m_szMessageRegexp = cfg->readQStringEntry(tmp);
	if(m_szMessageRegexp.isEmpty())return false;
	KviQString::sprintf(tmp,"%QIdentifyCommand",&prefix);
	m_szIdentifyCommand = cfg->readQStringEntry(tmp);
	return !m_szIdentifyCommand.isEmpty();
}

KviNickServRule * KviNickServRule::createInstance(const QString &szRegisteredNick,
		const QString &szNickServMask,
		const QString &szMessageRegexp,
		const QString &szIdentifyCommand,
		const QString &szServerMask)
{
	return new KviNickServRule(szRegisteredNick,szNickServMask,szMessageRegexp,szIdentifyCommand,szServerMask);
}
