//=============================================================================
//
//   File : kvi_regchan.cpp
//   Creation date : Sat Jun 29 01:01:16 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002-2008 Szymon Stefanek (pragma at kvirc dot net)
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


#include "kvi_regchan.h"
#include "kvi_config.h"
#include "kvi_qstring.h"
#include "kvi_debug.h"

KviRegisteredChannel::KviRegisteredChannel(const QString &name,const QString &netmask)
{
	m_szName = name;
	m_szNetMask = netmask;
	m_pPropertyDict = new QHash<QString,QString>();
}

KviRegisteredChannel::~KviRegisteredChannel()
{
	delete m_pPropertyDict;
}



KviRegisteredChannelDataBase::KviRegisteredChannelDataBase()
{
	m_pChannelDict = new QHash<QString,KviRegisteredChannelList *>();
}

KviRegisteredChannelDataBase::~KviRegisteredChannelDataBase()
{
	qDeleteAll(*m_pChannelDict);
	delete m_pChannelDict;
}

void KviRegisteredChannelDataBase::load(const QString &filename)
{
	KviConfig cfg(filename,KviConfig::Read);

	qDeleteAll(*m_pChannelDict);
	m_pChannelDict->clear();

	KviConfigIterator it(*(cfg.dict()));

	while(KviConfigGroup * d = it.current())
	{
		QString szMask = it.currentKey();
		QString szChan = KviQString::leftToLast(szMask,QChar('@'),false);
		KviQString::cutToLast(szMask,QChar('@'),true);
		KviRegisteredChannel * c = new KviRegisteredChannel(szChan,szMask);
		add(c);
		KviConfigGroupIterator sit(*d);
		while(QString * s = sit.current())
		{
			c->setProperty(sit.currentKey(),*s);
			++sit;
		}
		++it;
	}
}

void KviRegisteredChannelDataBase::save(const QString &filename)
{
	KviConfig cfg(filename,KviConfig::Write);
	cfg.clear();
	cfg.preserveEmptyGroups(true);

	for(QHash<QString,KviRegisteredChannelList *>::Iterator it = m_pChannelDict->begin();it != m_pChannelDict->end();++it)
	{
		KviRegisteredChannelList * l = it.value();

		for(KviRegisteredChannel * c = l->first();c;c = l->next())
		{
			QString szGroup = QString::fromUtf8("%1@%2").arg(c->name(),c->netMask());
			cfg.setGroup(szGroup);

			QHash<QString,QString> * pPropertyDict = c->propertyDict();
			
			for(QHash<QString,QString>::Iterator it2 = pPropertyDict->begin();it2 != pPropertyDict->end();++it2)
				cfg.writeEntry(it2.key(),it2.value());
		}
	}
}

KviRegisteredChannel * KviRegisteredChannelDataBase::find(const QString &name,const QString &net)
{
	KviRegisteredChannelList * l = m_pChannelDict->value(name);

	if(!l)
		return 0;

	for(KviRegisteredChannel * c = l->first();c;c = l->next())
	{
		if(KviQString::matchWildExpressions(c->netMask(),net))
			return c;
	}

	return 0;
}

KviRegisteredChannel * KviRegisteredChannelDataBase::findExact(const QString &name,const QString &netmask)
{
	KviRegisteredChannelList * l = m_pChannelDict->value(name);

	if(!l)
		return 0;

	for(KviRegisteredChannel * c = l->first();c;c = l->next())
	{
		if(KviQString::equalCI(c->netMask(),netmask))
			return c;
	}

	return 0;
}

void KviRegisteredChannelDataBase::remove(KviRegisteredChannel * c)
{
	KviRegisteredChannelList * l = m_pChannelDict->value(c->name());
	if(!l)return;
	for(KviRegisteredChannel * ch = l->first();ch;ch = l->next())
	{
		if(ch == c)
		{
			if(l->count() <= 1)
			{
				m_pChannelDict->remove(c->name());
				delete l; // will kill the channel too
			} else {
				l->removeRef(c);
			}
			return;
		}
	}
}


void KviRegisteredChannelDataBase::add(KviRegisteredChannel * c)
{
	KviRegisteredChannel * old = findExact(c->name(),c->netMask());

	if(old)
	{
		// merge properties
		QHash<QString,QString> * pPropertyDict = old->propertyDict();
		
		for(QHash<QString,QString>::Iterator it = pPropertyDict->begin();it != pPropertyDict->end();++it)
		{
			if(!c->property(it.key()).isEmpty())
				c->setProperty(it.key(),it.value());
		}

		remove(old);
	}

	KviRegisteredChannelList * l = m_pChannelDict->value(c->name());
	if(!l)
	{
		l = new KviRegisteredChannelList();
		l->setAutoDelete(true);
		m_pChannelDict->insert(c->name(),l);
	}

	// insert where there are less wildcards
	int o = c->netMask().count(QChar('*'));

	int idx = 0;
	for(KviRegisteredChannel * rc = l->first();rc;rc = l->next())
	{
		if(rc->netMask().count(QChar('*')) > o)
		{
			// the existing has more wildcards , insert here!
			l->insert(idx,c);
			return;
		}
		idx++;
	}
	
	l->append(c);
}

