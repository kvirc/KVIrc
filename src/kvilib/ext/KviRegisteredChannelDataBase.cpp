//=============================================================================
//
//   File : KviRegisteredChannelDataBase.cpp
//   Creation date : Wed Dec 29 2010 03:28:05 CEST by Elvio Basello
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2010 Elvio Basello (hellvis69 at gmail dot com)
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

#include "KviRegisteredChannelDataBase.h"
#include "KviConfigurationFile.h"
#include "KviQString.h"

#include <QString>

KviRegisteredChannelDataBase::KviRegisteredChannelDataBase()
{
	m_pChannelDict = new QHash<QString, KviRegisteredChannelList *>();
}

KviRegisteredChannelDataBase::~KviRegisteredChannelDataBase()
{
	qDeleteAll(*m_pChannelDict);
	delete m_pChannelDict;
}

void KviRegisteredChannelDataBase::load(const QString & filename)
{
	KviConfigurationFile cfg(filename, KviConfigurationFile::Read);

	qDeleteAll(*m_pChannelDict);
	m_pChannelDict->clear();

	KviConfigurationFileIterator it(*(cfg.dict()));

	while(KviConfigurationFileGroup * d = it.current())
	{
		QString szMask = it.currentKey();
		QString szChan = KviQString::leftToLast(szMask, QChar('@'), false);
		KviQString::cutToLast(szMask, QChar('@'), true);
		KviRegisteredChannel * c = new KviRegisteredChannel(szChan, szMask);
		add(c);
		KviConfigurationFileGroupIterator sit(*d);
		while(QString * s = sit.current())
		{
			c->setProperty(sit.currentKey(), *s);
			++sit;
		}
		++it;
	}
}

void KviRegisteredChannelDataBase::save(const QString & filename)
{
	KviConfigurationFile cfg(filename, KviConfigurationFile::Write);
	cfg.clear();
	cfg.preserveEmptyGroups(true);

	for(QHash<QString, KviRegisteredChannelList *>::Iterator it = m_pChannelDict->begin(); it != m_pChannelDict->end(); ++it)
	{
		KviRegisteredChannelList * l = it.value();
		KVI_ASSERT(l);

		for(KviRegisteredChannel * c = l->first(); c; c = l->next())
		{
			QString szGroup = QString::fromUtf8("%1@%2").arg(c->name(), c->netMask());
			cfg.setGroup(szGroup);

			QHash<QString, QString> * pPropertyDict = c->propertyDict();
			KVI_ASSERT(pPropertyDict);

			for(QHash<QString, QString>::Iterator it2 = pPropertyDict->begin(); it2 != pPropertyDict->end(); ++it2)
				cfg.writeEntry(it2.key(), it2.value());
		}
	}
}

KviRegisteredChannel * KviRegisteredChannelDataBase::find(const QString & name, const QString & net)
{
	KviRegisteredChannelList * l = m_pChannelDict->value(name);

	if(!l)
		return nullptr;

	for(KviRegisteredChannel * c = l->first(); c; c = l->next())
	{
		if(KviQString::matchWildExpressions(c->netMask(), net))
			return c;
	}

	return nullptr;
}

KviRegisteredChannel * KviRegisteredChannelDataBase::findExact(const QString & name, const QString & netmask)
{
	KviRegisteredChannelList * l = m_pChannelDict->value(name);

	if(!l)
		return nullptr;

	for(KviRegisteredChannel * c = l->first(); c; c = l->next())
	{
		if(KviQString::equalCI(c->netMask(), netmask))
			return c;
	}

	return nullptr;
}

void KviRegisteredChannelDataBase::remove(KviRegisteredChannel * c)
{
	KVI_TRACE_FUNCTION;
	KviRegisteredChannelList * l = m_pChannelDict->value(c->name());
	if(!l)
		return;
	for(KviRegisteredChannel * ch = l->first(); ch; ch = l->next())
	{
		if(ch == c)
		{
			if(l->count() <= 1)
			{
				m_pChannelDict->remove(c->name());
				delete l; // will kill the channel too
			}
			else
			{
				l->removeRef(c);
			}
			return;
		}
	}
}

void KviRegisteredChannelDataBase::add(KviRegisteredChannel * c)
{
	KviRegisteredChannel * old = findExact(c->name(), c->netMask());

	if(old)
	{
		// merge properties
		QHash<QString, QString> * pPropertyDict = old->propertyDict();
		KVI_ASSERT(pPropertyDict);

		for(QHash<QString, QString>::Iterator it = pPropertyDict->begin(); it != pPropertyDict->end(); ++it)
		{
			if(!c->property(it.key()).isEmpty())
				c->setProperty(it.key(), it.value());
		}

		remove(old);
	}

	KviRegisteredChannelList * l = m_pChannelDict->value(c->name());
	if(!l)
	{
		l = new KviRegisteredChannelList();
		l->setAutoDelete(true);
		m_pChannelDict->insert(c->name(), l);
	}

	// insert where there are less wildcards
	int o = c->netMask().count(QChar('*'));

	int idx = 0;
	for(KviRegisteredChannel * rc = l->first(); rc; rc = l->next())
	{
		if(rc->netMask().count(QChar('*')) > o)
		{
			// the existing has more wildcards, insert here!
			l->insert(idx, c);
			return;
		}
		idx++;
	}

	l->append(c);
}
