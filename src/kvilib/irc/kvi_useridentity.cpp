//=============================================================================
//
//   File : kvi_useridentity.cpp
//   Created on Sun 21 Jan 2007 04:31:47 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2007 Szymon Stefanek <pragma at kvirc dot net>
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

#include "kvi_useridentity.h"
#include "kvi_locale.h"
#include "kvi_defaults.h"

// FIXME: Put here also the default away message, default away nick, default ctcp replies etc ?

bool KviUserIdentity::load(KviConfig &cfg,const QString &szPrefix)
{
	QString szTmp;
	KviQString::sprintf(szTmp,"%QId",&szPrefix);
	m_szId = cfg.readQStringEntry(szTmp);
	KviQString::sprintf(szTmp,"%QNickName",&szPrefix);
	m_szNickName = cfg.readQStringEntry(szTmp);
	KviQString::sprintf(szTmp,"%QAltNickName1",&szPrefix);
	m_szAltNickName1 = cfg.readQStringEntry(szTmp);
	KviQString::sprintf(szTmp,"%QAltNickName2",&szPrefix);
	m_szAltNickName2 = cfg.readQStringEntry(szTmp);
	KviQString::sprintf(szTmp,"%QAltNickName3",&szPrefix);
	m_szAltNickName3 = cfg.readQStringEntry(szTmp);
	KviQString::sprintf(szTmp,"%QUserName",&szPrefix);
	m_szUserName = cfg.readQStringEntry(szTmp);
	// FIXME: At least scramble the pass ?
	KviQString::sprintf(szTmp,"%QPassword",&szPrefix);
	m_szPassword = cfg.readQStringEntry(szTmp);
	KviPixmap def;
	KviQString::sprintf(szTmp,"%QAvatar",&szPrefix);
	m_pixAvatar = cfg.readPixmapEntry(szTmp,def);
	KviQString::sprintf(szTmp,"%QPartMessage",&szPrefix);
	m_szPartMessage = cfg.readQStringEntry(szTmp);
	KviQString::sprintf(szTmp,"%QQuitMessage",&szPrefix);
	m_szQuitMessage= cfg.readQStringEntry(szTmp);
	KviQString::sprintf(szTmp,"%QAge",&szPrefix);
	m_szAge = cfg.readQStringEntry(szTmp);
	KviQString::sprintf(szTmp,"%QGender",&szPrefix);
	m_szGender = cfg.readQStringEntry(szTmp);
	KviQString::sprintf(szTmp,"%QLocation",&szPrefix);
	m_szLocation = cfg.readQStringEntry(szTmp);
	KviQString::sprintf(szTmp,"%QLanguages",&szPrefix);
	m_szLanguages = cfg.readQStringEntry(szTmp);
	KviQString::sprintf(szTmp,"%QOtherInfo",&szPrefix);
	m_szOtherInfo = cfg.readQStringEntry(szTmp);
	KviQString::sprintf(szTmp,"%QUserMode",&szPrefix);
	m_szUserMode = cfg.readQStringEntry(szTmp);
	return !(m_szId.isEmpty() || m_szNickName.isEmpty());
}

bool KviUserIdentity::save(KviConfig &cfg,const QString &szPrefix)
{
	QString szTmp;
	KviQString::sprintf(szTmp,"%QId",&szPrefix);
	cfg.writeEntry(szTmp,m_szId);
	KviQString::sprintf(szTmp,"%QNickName",&szPrefix);
	cfg.writeEntry(szTmp,m_szNickName);
	KviQString::sprintf(szTmp,"%QAltNickName1",&szPrefix);
	cfg.writeEntry(szTmp,m_szAltNickName1);
	KviQString::sprintf(szTmp,"%QAltNickName2",&szPrefix);
	cfg.writeEntry(szTmp,m_szAltNickName2);
	KviQString::sprintf(szTmp,"%QAltNickName3",&szPrefix);
	cfg.writeEntry(szTmp,m_szAltNickName3);
	KviQString::sprintf(szTmp,"%QUserName",&szPrefix);
	cfg.writeEntry(szTmp,m_szUserName);
	// FIXME: At least scramble the pass ?
	KviQString::sprintf(szTmp,"%QPassword",&szPrefix);
	cfg.writeEntry(szTmp,m_szPassword);
	KviQString::sprintf(szTmp,"%QAvatar",&szPrefix);
	cfg.writeEntry(szTmp,m_pixAvatar);
	KviQString::sprintf(szTmp,"%QPartMessage",&szPrefix);
	cfg.writeEntry(szTmp,m_szPartMessage);
	KviQString::sprintf(szTmp,"%QQuitMessage",&szPrefix);
	cfg.writeEntry(szTmp,m_szQuitMessage);
	KviQString::sprintf(szTmp,"%QAge",&szPrefix);
	cfg.writeEntry(szTmp,m_szAge);
	KviQString::sprintf(szTmp,"%QGender",&szPrefix);
	cfg.writeEntry(szTmp,m_szGender);
	KviQString::sprintf(szTmp,"%QLocation",&szPrefix);
	cfg.writeEntry(szTmp,m_szLocation);
	KviQString::sprintf(szTmp,"%QLanguages",&szPrefix);
	cfg.writeEntry(szTmp,m_szLanguages);
	KviQString::sprintf(szTmp,"%QOtherInfo",&szPrefix);
	cfg.writeEntry(szTmp,m_szOtherInfo);
	KviQString::sprintf(szTmp,"%QUserMode",&szPrefix);
	cfg.writeEntry(szTmp,m_szUserMode);
	return true;
}

void KviUserIdentity::copyFrom(const KviUserIdentity &src)
{
	m_szId = src.m_szId;
	m_szNickName = src.m_szNickName;
	
	m_szAltNickName1 = src.m_szAltNickName1;
	m_szAltNickName2 = src.m_szAltNickName2;
	m_szAltNickName3 = src.m_szAltNickName3;

	m_szUserName = src.m_szUserName;
	m_szRealName = src.m_szRealName;
	m_szPassword = src.m_szPassword;
	
	m_pixAvatar = src.m_pixAvatar;

	m_szUserName = src.m_szUserName;
	m_szRealName = src.m_szRealName;
	m_szPassword = src.m_szPassword;
	
	m_pixAvatar = src.m_pixAvatar;
	
	m_szPartMessage = src.m_szPartMessage;
	m_szQuitMessage = src.m_szQuitMessage;
	
	m_szAge = src.m_szAge;
	m_szGender = src.m_szGender;
	m_szLocation = src.m_szLocation;
	m_szLanguages = src.m_szLanguages;
	m_szOtherInfo = src.m_szOtherInfo;
	
	m_szUserMode = src.m_szUserMode;
}


KviUserIdentityManager * KviUserIdentityManager::m_pInstance = 0;

KviUserIdentityManager::KviUserIdentityManager()
: KviHeapObject()
{
	m_pIdentityDict = new KviDict<KviUserIdentity>();
	m_pIdentityDict->setAutoDelete(true);
}

KviUserIdentityManager::~KviUserIdentityManager()
{
	delete m_pIdentityDict;
}

void KviUserIdentityManager::init()
{
	if(m_pInstance)return;
	m_pInstance = new KviUserIdentityManager();
}

void KviUserIdentityManager::done()
{
	if(!m_pInstance)return;
	delete m_pInstance;
	m_pInstance = 0;
}

const KviUserIdentity * KviUserIdentityManager::defaultIdentity()
{
	KviUserIdentity * ret;
	if(!m_szDefaultIdentity.isEmpty())
	{
		ret = m_pIdentityDict->find(m_szDefaultIdentity);
		if(ret)return ret;
	}

	// the default identity is borken :/
	// grab the first one
	KviDictIterator<KviUserIdentity> it(*m_pIdentityDict);
	ret = it.current();
	if(ret)
	{
		m_szDefaultIdentity = ret->id();
		return ret;
	}
	// no identities available: create the default
	ret = new KviUserIdentity();

	ret->setId(__tr2qs("Default"));
	ret->setNickName(KVI_DEFAULT_NICKNAME1);
	ret->setAltNickName1(KVI_DEFAULT_NICKNAME2);
	ret->setAltNickName2(KVI_DEFAULT_NICKNAME3);
	ret->setAltNickName3(KVI_DEFAULT_NICKNAME4);
	ret->setUserName(KVI_DEFAULT_USERNAME);
	ret->setRealName(KVI_DEFAULT_REALNAME);
	ret->setPartMessage(KVI_DEFAULT_PART_MESSAGE);
	ret->setQuitMessage(KVI_DEFAULT_QUIT_MESSAGE);
	
	m_pIdentityDict->replace(ret->id(),ret);
	
	return ret;
}

void KviUserIdentityManager::save(const QString &szFileName)
{
}

void KviUserIdentityManager::load(const QString &szFileName)
{
}

KviUserIdentityManager * KviUserIdentityManager::getWorkingCopy()
{
	return 0;
}

void KviUserIdentityManager::setValuesFrom(KviUserIdentityManager * pWorkingCopy)
{
	
}

