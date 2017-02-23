#ifndef _KVIREGUSER_H_
#define _KVIREGUSER_H_
//=============================================================================
//
//   File : KviRegisteredUser.cpp
//   Creation date : Wed Dec 29 2010 02:44:05 CEST by Elvio Basello
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

#include "KviRegisteredUser.h"
#include "KviIrcMask.h"

#include <QString>

KviRegisteredUser::KviRegisteredUser(const QString & name)
{
	m_iIgnoreFlags = 0;
	m_bIgnoreEnabled = false;
	m_szName = name;
	m_pPropertyDict = nullptr;
	m_pMaskList = new KviPointerList<KviIrcMask>;
	m_pMaskList->setAutoDelete(true);
}

KviRegisteredUser::~KviRegisteredUser()
{
	if(m_pPropertyDict)
		delete m_pPropertyDict;
	delete m_pMaskList;
}

bool KviRegisteredUser::isIgnoreEnabledFor(IgnoreFlags flag)
{
	if(!m_bIgnoreEnabled)
		return false;
	return m_iIgnoreFlags & flag;
}

KviIrcMask * KviRegisteredUser::findMask(const KviIrcMask & mask)
{
	for(KviIrcMask * m = m_pMaskList->first(); m; m = m_pMaskList->next())
	{
		if(*m == mask)
			return m;
	}
	return nullptr;
}

bool KviRegisteredUser::addMask(KviIrcMask * mask)
{
	if(findMask(*mask))
	{
		delete mask;
		mask = nullptr;
		return false;
	}
	m_pMaskList->append(mask);
	return true;
}

bool KviRegisteredUser::removeMask(KviIrcMask * mask)
{
	if(!mask)
		return false;
	return m_pMaskList->removeRef(mask);
}

bool KviRegisteredUser::matches(const KviIrcMask & mask)
{
	for(KviIrcMask * m = m_pMaskList->first(); m; m = m_pMaskList->next())
	{
		if(m->matches(mask))
			return true;
	}
	return false;
}

bool KviRegisteredUser::matchesFixed(const KviIrcMask & mask)
{
	for(KviIrcMask * m = m_pMaskList->first(); m; m = m_pMaskList->next())
	{
		if(m->matchesFixed(mask))
			return true;
	}
	return false;
}

bool KviRegisteredUser::matchesFixed(const QString & nick, const QString & user, const QString & host)
{
	for(KviIrcMask * m = m_pMaskList->first(); m; m = m_pMaskList->next())
	{
		if(m->matchesFixed(nick, user, host))
			return true;
	}
	return false;
}

void KviRegisteredUser::setProperty(const QString & name, bool value)
{
	setProperty(name, value ? QString("true") : QString("false"));
}

void KviRegisteredUser::setProperty(const QString & name, const QString & value)
{
	if(!value.isEmpty())
	{
		if(!m_pPropertyDict)
		{
			m_pPropertyDict = new KviPointerHashTable<QString, QString>(7, false);
			m_pPropertyDict->setAutoDelete(true);
		}

		QString * val = new QString(value.trimmed());
		if(!val->isEmpty())
		{
			m_pPropertyDict->replace(name, val);
		}
		else
		{
			delete val;
			val = nullptr;
		}
	}
	else
	{
		if(m_pPropertyDict)
			m_pPropertyDict->remove(name);
	}
}

bool KviRegisteredUser::getProperty(const QString & name, QString & value)
{
	if(!m_pPropertyDict)
		return false;
	if(name.isEmpty())
		return false;
	QString * pValue = m_pPropertyDict->find(name);
	if(pValue)
		value = *pValue;
	else
		return false;
	return true;
}

const QString & KviRegisteredUser::getProperty(const QString & name)
{
	if(!m_pPropertyDict)
		return KviQString::Empty;
	if(name.isEmpty())
		return KviQString::Empty;
	QString * pValue = m_pPropertyDict->find(name);
	if(pValue)
		return *pValue;
	return KviQString::Empty;
}

bool KviRegisteredUser::getBoolProperty(const QString & name, bool def)
{
	if(!m_pPropertyDict)
		return def;
	if(name.isEmpty())
		return def;
	QString * pValue = m_pPropertyDict->find(name);
	if(pValue)
	{
		// be flexible, allow more "true" values (pragma)
		if(KviQString::equalCS(*pValue, "1"))
			return true;
		if(KviQString::equalCI(*pValue, "true"))
			return true;
		if(KviQString::equalCI(*pValue, "yes"))
			return true;
	}
	return def;
}

#endif // _KVIREGUSER_H_
