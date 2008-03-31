//=============================================================================
//
//   File : kvi_regchan.cpp
//   Creation date : Sat Jun 29 01:01:16 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2007 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#define __KVILIB__

#include "kvi_regchan.h"
#include "kvi_config.h"
#include "kvi_qstring.h"

KviRegisteredChannel::KviRegisteredChannel(const KviStr &name,const KviStr &netmask)
{
	m_szName = name;
	m_szNetMask = netmask;
	m_pPropertyDict = new KviPointerHashTable<const char *,KviStr>(7,false,true);
	m_pPropertyDict->setAutoDelete(true);
}

KviRegisteredChannel::~KviRegisteredChannel()
{
	delete m_pPropertyDict;
}



KviRegisteredChannelDataBase::KviRegisteredChannelDataBase()
{
	m_pChannelDict = new KviPointerHashTable<const char *,KviRegisteredChannelList>(17,false,true);
	m_pChannelDict->setAutoDelete(true);
}

KviRegisteredChannelDataBase::~KviRegisteredChannelDataBase()
{
	delete m_pChannelDict;
}

void KviRegisteredChannelDataBase::load(const char * filename)
{
	KviConfig cfg(filename,KviConfig::Read);
	m_pChannelDict->clear();
	KviConfigIterator it(*(cfg.dict()));
	while(KviConfigGroup * d = it.current())
	{
		KviStr szMask = it.currentKey();
		KviStr szChan = szMask.leftToLast('@');
		szMask.cutToLast('@');
		KviRegisteredChannel * c = new KviRegisteredChannel(szChan,szMask);
		add(c);
		KviConfigGroupIterator sit(*d);
		while(QString * s = sit.current())
		{
			c->setProperty(KviQString::toUtf8(sit.currentKey()).data(),new KviStr(*s));
			++sit;
		}
		++it;
	}
}

void KviRegisteredChannelDataBase::save(const char * filename)
{
	KviConfig cfg(filename,KviConfig::Write);
	cfg.clear();

	KviPointerHashTableIterator<const char *,KviRegisteredChannelList> it(*m_pChannelDict);
	while(KviRegisteredChannelList * l = it.current())
	{
		for(KviRegisteredChannel * c = l->first();c;c = l->next())
		{
			KviStr szGrp(KviStr::Format,"%s@%s",c->name().ptr(),c->netMask().ptr());
			cfg.setGroup(szGrp.ptr());
			KviPointerHashTableIterator<const char *,KviStr> pit(*(c->propertyDict()));
			while(KviStr * s = pit.current())
			{
				cfg.writeEntry(pit.currentKey(),s->ptr());
				++pit;
			}
		}
		++it;
	}
}

KviRegisteredChannel * KviRegisteredChannelDataBase::find(const char * name,const char * net)
{
	KviRegisteredChannelList * l = m_pChannelDict->find(name);
	if(!l)return 0;
	for(KviRegisteredChannel * c = l->first();c;c = l->next())
	{
		if(kvi_matchString(c->netMask().ptr(),net))return c;
	}
	
	return 0;
}

KviRegisteredChannel * KviRegisteredChannelDataBase::findExact(const char * name,const char * netmask)
{
	KviRegisteredChannelList * l = m_pChannelDict->find(name);
	if(!l)return 0;
	for(KviRegisteredChannel * c = l->first();c;c = l->next())
	{
		if(kvi_strEqualCI(c->netMask().ptr(),netmask))return c;
	}
	return 0;
}

void KviRegisteredChannelDataBase::remove(KviRegisteredChannel * c)
{
	KviRegisteredChannelList * l = m_pChannelDict->find(c->name().ptr());
	if(!l)return;
	for(KviRegisteredChannel * ch = l->first();ch;ch = l->next())
	{
		if(ch == c)
		{
			if(l->count() <= 1)
			{
				m_pChannelDict->remove(c->name().ptr());
			} else {
				l->removeRef(c);
			}
			return;
		}
	}
}


void KviRegisteredChannelDataBase::add(KviRegisteredChannel * c)
{
	KviRegisteredChannel * old = findExact(c->name().ptr(),c->netMask().ptr());
	if(old)
	{
		KviPointerHashTableIterator<const char *,KviStr> pit(*(old->propertyDict()));
		while(KviStr *s = pit.current())
		{
			if(!c->property(pit.currentKey()))
				c->setProperty(pit.currentKey(),new KviStr(*s));
			++pit;
		}
		remove(old);
	}
	KviRegisteredChannelList * l = m_pChannelDict->find(c->name().ptr());
	if(!l)
	{
		l = new KviRegisteredChannelList;
		l->setAutoDelete(true);
		m_pChannelDict->insert(c->name().ptr(),l);
	}
	// insert where there are less wildcards
	int o = c->netMask().occurences('*');
	int idx = 0;
	for(KviRegisteredChannel * rc = l->first();rc;rc = l->next())
	{
		if(rc->netMask().occurences('*') > o)
		{
			// the existing has more wildcards , insert here!
			l->insert(idx,c);
			return;
		}
		idx++;
	}
	l->append(c);
}
