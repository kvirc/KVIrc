//==============================================================================
//
//   File : KviRegisteredUserDataBase.cpp
//   Creation date : Sat Sep 09 2000 15:46:12 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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
//==============================================================================

#define _KVI_REGUSERDB_CPP_

#include "kvi_debug.h"
#include "KviRegisteredUserDataBase.h"
#include "KviConfigurationFile.h"
#include "KviIrcMask.h"
#include "KviLocale.h"

#include <QString>

/*
	@doc: registered_users
	@title:
		Registered users
	@type:
		generic
	@short:
		Registration of users in KVIrc
	@keyterms:
		registered users, registration mask, registered user properties,
		user properties, notify property, avatar property
	@body:
		[big]Introduction[/big]
		The registered user database is basically a set of users with associated
		[doc:irc_masks]irc-masks[/doc] and properties.[br]
		It is used to recognize users on IRC and associate properties to them.[br]
		This works more or less like the IRC ban list, K-Line list, or invite list.
		[big]User entry[/big]
		A registered user database entry is identified by a [b]unique[/b] name.[br]
		It may be the nickname of the user that you want to match, or the real name (if you know it)
		or any other string (even with spaces). The name is an [i]internal identifier[/i] for the user entry:
		each name maps to a single entry and each entry has a single name.[br]
		Each entry has a set of registration [doc:irc_masks]irc-masks[/doc]: these masks
		are used to recognize the user on IRC.
		[big]Registration masks[/big]
		The masks have the common IRC mask format: [b]<nick>!<user>@<host>[/b][br]
		The masks may contain [b]*[/b] and [b]?[/b] wildcards that match any portion of text.[br]
		[b]*!*@*[/b][br]
		[b]Pragma!*@*[/b][br]
		[b]*!~daemon@*[/b][br]
		[b]Pragma!*daemon@*.it[/b][br]
		[b]Pragma!?daemon@some*.it[/b][br]
		[b]Pragma!~daemon@some.host.it[/b][br]
		Are examples of valid registration masks.[br][br]
		The masks with wildcards can actually match more than a single user.[br]
		For example the mask *!root@*.host.com will match all the users
		having root as username and coming from the host.com domain.[br]
		For this reason putting wildcards in nicknames could become a problem
		if not used carefully (but may also be used to achieve interesting tricks).[br]
		If you don't use wildcards in nicknames you are sure that
		in a single IRC connection, a mask will always refer to a single user.[br]
		You will commonly use the following format:[br]
		[b]<nick>!*<username>@*.<host>.<top>[/b][br]
		or[br]
		[b]<nick>!*<username>@<number>.<number>.<number>.*[/b][br]
		In this way you can be 95% sure that the mask will really match the correct user.
		[big]Example of registration and lookups[/big]
		Assume that you want to register a friend of yours: Derek Riggs.[br]
		Derek often uses [i]Eddie[/i] as his nickname
		[i]stranger[/i] as username and has a dial-up connection that makes his IP address appear as
		<variable-number>.somewhere.in.time.org.[br]
		You will add an entry with name [i]Derek Riggs[/i] and a registration mask like the following:[br]
		[b]Eddie!stranger@*.somewhere.in.time.org.[/b][br]
		If the IRC servers keep adding strange characters ([doc:irc_masks]prefixes[/doc]) at the beginning of his username you may use;[br]
		[b]Eddie!*stranger@*.somewhere.in.time.org.[/b][br]
		If Eddie also often connects from the wasted.years.org domain and gets [i]eddie[/i] as username there,
		you might add a second registration mask as follows:[br]
		[b]Eddie!*eddie@*.wasted.years.org.[/b][br]
		An alternative could be use only one mask with *.org as domain and allow any username (Eddie!*@*.org) but this
		could become dangerous since it could match the users that you don't want to.[br]
		On the other hand, if you dislike the users with the nickname Eddie that come from .org
		and you're implementing an auto-kick system, the correct mask to register is [i]Eddie!*@*.org[/i].
		[br][br]
		KVIrc tries to be smart, and always find the most correct match for a user:
		If you have two masks registered: [b]Pragma!*xor@*.myisp.it[/b] and [b]*!*@*.myisp.it[/b],
		KVIrc will match [b]Pragma!~xor@233-dyn.myisp.it[/b] with the first one even if the second
		one also matches; the first one however, is a best match.
		[big]Properties[/big]
		A registered user has an (eventually empty) set of properties
		defined by name/value pairs. (In versions prior to 3.0.0 flags were used instead,
		but revealed to be insufficient).[br]
		KVIrc recognizes some of these properties and associates semantic actions to it; other properties
		are left for scripting extension. Property names are case insensitive.[br]
		One of the recognized properties is the [i][doc:notify_list]notify[/doc][/i] property.[br][br]
		When a user is found to have this property set to a special value
		KVIrc will attempt to track the user presence on IRC.
		Another one is the [doc:avatar]avatar[/doc] property. Its value should be the
		name of the [i]default[/i] [doc:avatar]avatar image file[/doc] for the specified user.[br]
		The [i]ignore[/i] property should be set to [i]1[/i] (or [i]true[/i]) for users that have to be ignored (:D).
		[big]The interface to the database[/big]
		The [module:reguser]reguser module[/module] is the interface to the [i]registered users database[/i].[br]
		It provides a set of commands for adding and removing masks and manipulating properties.[br]
*/

KVILIB_API KviRegisteredUserDataBase * g_pRegisteredUserDataBase = nullptr;

//
// KviRegisteredUserDb
//

KviRegisteredUserDataBase::KviRegisteredUserDataBase()
{
	m_pUserDict = new KviPointerHashTable<QString, KviRegisteredUser>(31, false); // do not copy keys
	m_pUserDict->setAutoDelete(true);

	m_pWildMaskList = new KviRegisteredUserMaskList;
	m_pWildMaskList->setAutoDelete(true);

	m_pMaskDict = new KviPointerHashTable<QString, KviRegisteredUserMaskList>(49, false); // copy keys here!
	m_pMaskDict->setAutoDelete(true);

	m_pGroupDict = new KviPointerHashTable<QString, KviRegisteredUserGroup>(5, false); // copy keys here!
	m_pGroupDict->setAutoDelete(true);
}

KviRegisteredUserDataBase::~KviRegisteredUserDataBase()
{
	emit(databaseCleared());
	delete m_pUserDict;
	delete m_pWildMaskList;
	delete m_pMaskDict;
	delete m_pGroupDict;
}

KviRegisteredUser * KviRegisteredUserDataBase::addUser(const QString & name)
{
	if(name.isEmpty())
		return nullptr;
	if(m_pUserDict->find(name))
		return nullptr;
	KviRegisteredUser * u = new KviRegisteredUser(name);
	m_pUserDict->replace(u->name(), u); //u->name() because we're NOT copying keys!
	emit(userAdded(name));
	return u;
}

KviRegisteredUserGroup * KviRegisteredUserDataBase::addGroup(const QString & name)
{
	if(name.isEmpty())
		return nullptr;
	if(m_pGroupDict->find(name))
		return nullptr;
	KviRegisteredUserGroup * pGroup = new KviRegisteredUserGroup(name);
	m_pGroupDict->replace(pGroup->name(), pGroup); //u->name() because we're NOT copying keys!
	return pGroup;
}

KviRegisteredUser * KviRegisteredUserDataBase::getUser(const QString & name)
{
	if(name.isEmpty())
		return nullptr;
	KviRegisteredUser * u = m_pUserDict->find(name);
	if(!u)
	{
		u = new KviRegisteredUser(name);
		m_pUserDict->replace(u->name(), u); //u->name() because we're NOT copying keys!
	}
	return u;
}

static void append_mask_to_list(KviRegisteredUserMaskList * l, KviRegisteredUser * u, KviIrcMask * mask)
{
	KviRegisteredUserMask * newMask = new KviRegisteredUserMask(u, mask);
	int idx = 0;
	for(KviRegisteredUserMask * m = l->first(); m; m = l->next())
	{
		if(m->nonWildChars() < newMask->nonWildChars())
		{
			l->insert(idx, newMask);
			return;
		}
		idx++;
	}
	l->append(newMask);
}

KviRegisteredUser * KviRegisteredUserDataBase::addMask(KviRegisteredUser * u, KviIrcMask * mask)
{
	if(!u || !mask)
		return nullptr;
	KVI_ASSERT(u == m_pUserDict->find(u->name()));

	KviRegisteredUserMaskList * l;
	if(mask->hasWildNick())
	{
		for(KviRegisteredUserMask * m = m_pWildMaskList->first(); m; m = m_pWildMaskList->next())
		{
			if(*(m->mask()) == *mask)
			{
				delete mask;
				mask = nullptr;
				return m->user();
			}
		}
		// not found ...ok... add it
		// masks with more info go first in the list
		l = m_pWildMaskList;
	}
	else
	{
		l = m_pMaskDict->find(mask->nick());
		if(l)
		{
			// FIXME: #warning "Here we could compare the host and username only: nick matches for sure"
			for(KviRegisteredUserMask * m = l->first(); m; m = l->next())
			{
				if(*(m->mask()) == *mask)
				{
					delete mask;
					mask = nullptr;
					return m->user();
				}
			}
			// not found ...ok... add it
		}
		else
		{
			// not found ...ok... add it
			// this is the first mask in the list
			l = new KviRegisteredUserMaskList;
			l->setAutoDelete(true);
			if(!u->addMask(mask))
			{
				qDebug("Oops! Received an incoherent regusers action, recovered?");
				delete l;
				l = nullptr;
			}
			else
			{
				append_mask_to_list(l, u, mask);
				m_pMaskDict->insert(mask->nick(), l);
			}
			return nullptr;
		}
	}
	// Ok...add it
	if(!u->addMask(mask))
	{
		qDebug("Oops! Received an incoherent regusers action, recovered?");
		return nullptr; // ops...already there ?
	}
	append_mask_to_list(l, u, mask);
	return nullptr;
}

void KviRegisteredUserDataBase::copyFrom(KviRegisteredUserDataBase * db)
{
	m_pUserDict->clear();
	m_pWildMaskList->clear();
	m_pMaskDict->clear();
	m_pGroupDict->clear();
	emit(databaseCleared());

	KviPointerHashTableIterator<QString, KviRegisteredUser> it(*(db->m_pUserDict));

	while(KviRegisteredUser * theCur = it.current())
	{
		KviRegisteredUser * u = getUser(theCur->name());
		// copy masks
		KviPointerList<KviIrcMask> * l = theCur->maskList();
		for(KviIrcMask * m = l->first(); m; m = l->next())
		{
			KviIrcMask * m2 = new KviIrcMask(*m);
			addMask(u, m2);
		}
		// copy properties
		KviPointerHashTable<QString, QString> * pd = theCur->propertyDict();
		if(pd)
		{
			KviPointerHashTableIterator<QString, QString> pdi(*pd);
			while(pdi.current())
			{
				u->setProperty(pdi.currentKey(), *(pdi.current()));
				++pdi;
			}
		}
		u->m_iIgnoreFlags = theCur->m_iIgnoreFlags;
		u->m_bIgnoreEnabled = theCur->m_bIgnoreEnabled;
		u->setGroup(theCur->group());
		++it;
	}

	KviPointerHashTableIterator<QString, KviRegisteredUserGroup> git(*db->m_pGroupDict);
	while(git.current())
	{
		addGroup(git.currentKey());
		++git;
	}
}

bool KviRegisteredUserDataBase::removeUser(const QString & name)
{
	if(name.isEmpty())
		return false;
	KviRegisteredUser * u = m_pUserDict->find(name);
	if(!u)
		return false;
	while(KviIrcMask * mask = u->maskList()->first())
	{
		if(!removeMaskByPointer(mask))
			qDebug("Oops! removeMaskByPointer(%s) has failed", name.toUtf8().data());
	}
	emit(userRemoved(name));
	m_pUserDict->remove(name);
	return true;
}

bool KviRegisteredUserDataBase::removeGroup(const QString & name)
{
	if(name.isEmpty())
		return false;
	m_pGroupDict->remove(name);
	return true;
}

bool KviRegisteredUserDataBase::removeMask(const KviIrcMask & mask)
{
	// find the mask pointer
	KviRegisteredUserMask * m = findExactMask(mask);
	// and remove it
	if(m)
	{
		if(removeMaskByPointer(m->mask()))
		{
			return true;
		}
	}
	return false;
}

bool KviRegisteredUserDataBase::removeMaskByPointer(KviIrcMask * mask)
{
	if(!mask)
		return false;
	if(mask->hasWildNick())
	{
		// remove from the wild list
		for(KviRegisteredUserMask * m = m_pWildMaskList->first(); m; m = m_pWildMaskList->next())
		{
			if(m->mask() == mask)
			{
				// ok..got it, remove from the list and from the user struct (user struct deletes it!)
				emit(userChanged(mask->nick()));
				m->user()->removeMask(mask);   // this one deletes m->mask()
				m_pWildMaskList->removeRef(m); // this one deletes m
				return true;
			}
		}
		// not found ...opz :)
	}
	else
	{
		KviRegisteredUserMaskList * l = m_pMaskDict->find(mask->nick());
		if(l)
		{
			// FIXME: #warning "Here we could compare the host and username only: nick matches for sure"
			for(KviRegisteredUserMask * m = l->first(); m; m = l->next())
			{
				if(m->mask() == mask)
				{
					QString nick = mask->nick();
					emit(userChanged(nick));
					m->user()->removeMask(mask); // this one deletes m->mask() (or mask)
					l->removeRef(m);             // this one deletes m
					if(l->count() == 0)
						m_pMaskDict->remove(nick);
					return true;
				}
			}
			// not found ...opz
		}
	}
	// not found...
	return false;
}

KviRegisteredUser * KviRegisteredUserDataBase::findMatchingUser(const QString & nick, const QString & user, const QString & host)
{
	KviRegisteredUserMask * m = findMatchingMask(nick, user, host);
	if(m)
		return m->user();
	return nullptr; // no match at all
}

KviRegisteredUserMask * KviRegisteredUserDataBase::findMatchingMask(const QString & nick, const QString & user, const QString & host)
{
	// first lookup the nickname in the maskDict
	if(nick.isEmpty())
		return nullptr;
	KviRegisteredUserMaskList * l = m_pMaskDict->find(nick);
	if(l)
	{
		for(KviRegisteredUserMask * m = l->first(); m; m = l->next())
		{
			if(m->mask()->matchesFixed(nick, user, host))
				return m;
		}
	}
	// not found....lookup the wild ones
	for(KviRegisteredUserMask * m = m_pWildMaskList->first(); m; m = m_pWildMaskList->next())
	{
		if(m->mask()->matchesFixed(nick, user, host))
			return m;
	}
	return nullptr; // no match at all
}

KviRegisteredUser * KviRegisteredUserDataBase::findUserWithMask(const KviIrcMask & mask)
{
	KviRegisteredUserMask * m = findExactMask(mask);
	if(m)
		return m->user();
	return nullptr;
}

KviRegisteredUserMask * KviRegisteredUserDataBase::findExactMask(const KviIrcMask & mask)
{
	// first lookup the nickname in the maskDict
	if(mask.nick() == "")
		return nullptr;
	KviRegisteredUserMaskList * l = m_pMaskDict->find(mask.nick());
	if(l)
	{
		for(KviRegisteredUserMask * m = l->first(); m; m = l->next())
		{
			if(*(m->mask()) == mask)
				return m;
		}
	}
	// not found....lookup the wild ones
	for(KviRegisteredUserMask * m = m_pWildMaskList->first(); m; m = m_pWildMaskList->next())
	{
		if(*(m->mask()) == mask)
			return m;
	}
	return nullptr; // no match at all
}

void KviRegisteredUserDataBase::load(const QString & filename)
{
	QString szCurrent;
	KviConfigurationFile cfg(filename, KviConfigurationFile::Read);

	KviConfigurationFileIterator it(*cfg.dict());
	while(it.current())
	{
		cfg.setGroup(it.currentKey());
		szCurrent = it.currentKey();
		if(KviQString::equalCSN("#Group ", szCurrent, 7))
		{
			szCurrent.remove(0, 7);
			addGroup(szCurrent);
		}
		else
		{
			KviRegisteredUser * u = addUser(szCurrent);

			if(u)
			{
				u->setIgnoreEnabled(cfg.readBoolEntry("IgnoreEnabled", false));
				u->setIgnoreFlags(cfg.readIntEntry("IgnoreFlags", 0));
				KviConfigurationFileGroupIterator sdi(*(it.current()));
				while(sdi.current())
				{
					QString tmp = sdi.currentKey();
					if(KviQString::equalCSN("prop_", tmp, 5))
					{
						tmp.remove(0, 5);
						u->setProperty(tmp, *(sdi.current()));
					}
					else if(KviQString::equalCSN("mask_", tmp, 5))
					{
						KviIrcMask * mask = new KviIrcMask(*(sdi.current()));
						addMask(u, mask);
					}
					else if(KviQString::equalCI(tmp, "Group"))
					{
						u->setGroup(*(sdi.current()));
					}
					++sdi;
				}
			}
		}
		++it;
	}
	if(!m_pGroupDict->find(__tr("Default")))
		addGroup(__tr("Default"));
}

void KviRegisteredUserDataBase::save(const QString & szFilename)
{
	KviConfigurationFile cfg(szFilename, KviConfigurationFile::Write);
	cfg.clear();
	cfg.preserveEmptyGroups(true);

	KviPointerHashTableIterator<QString, KviRegisteredUser> it(*m_pUserDict);

	while(it.current())
	{
		cfg.setGroup(it.current()->name());
		// Write properties
		cfg.writeEntry("IgnoreEnabled", it.current()->ignoreEnabled());
		cfg.writeEntry("IgnoreFlags", it.current()->ignoreFlags());
		if(it.current()->propertyDict())
		{
			KviPointerHashTableIterator<QString, QString> pit(*(it.current()->propertyDict()));
			while(pit.current())
			{
				QString tmp = "prop_";
				tmp.append(pit.currentKey());
				cfg.writeEntry(tmp, *(pit.current()));
				++pit;
			}
		}
		// Write masks
		int i = 0;
		for(KviIrcMask * pMask = it.current()->maskList()->first(); pMask; pMask = it.current()->maskList()->next())
		{
			QString szTmp = QString("mask_%1").arg(i);
			QString szMask;
			pMask->mask(szMask, KviIrcMask::NickUserHost);
			cfg.writeEntry(szTmp, szMask);
			++i;
		}
		cfg.writeEntry("Group", it.current()->group());
		++it;
	}

	KviPointerHashTableIterator<QString, KviRegisteredUserGroup> git(*m_pGroupDict);
	QString szTmp;
	while(git.current())
	{
		szTmp = QString("#Group %1").arg(git.current()->name());
		cfg.setGroup(szTmp);
		++git;
	}
}
