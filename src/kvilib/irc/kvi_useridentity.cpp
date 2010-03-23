//=============================================================================
//
//   File : kvi_useridentity.cpp
//   Creation date : Sun 21 Jan 2007 04:31:47 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2007-2008 Szymon Stefanek <pragma at kvirc dot net>
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


#include "kvi_useridentity.h"
#include "kvi_locale.h"
#include "kvi_defaults.h"

// FIXME: Put here also the default away message, default away nick, default ctcp replies etc ?

KviUserIdentity::KviUserIdentity()
: KviHeapObject()
{
}

KviUserIdentity::~KviUserIdentity()
{
}

bool KviUserIdentity::load(KviConfig &cfg)
{
	m_szId = cfg.group();
	m_szNickName = cfg.readQStringEntry("NickName");
	m_szAltNickName1 = cfg.readQStringEntry("AltNickName1");
	m_szAltNickName2 = cfg.readQStringEntry("AltNickName2");
	m_szAltNickName3 = cfg.readQStringEntry("AltNickName3");
	m_szUserName = cfg.readQStringEntry("UserName");
	// FIXME: At least scramble the pass ?
	m_szPassword = cfg.readQStringEntry("Password");
	KviPixmap def;
	m_pixAvatar = cfg.readPixmapEntry("Avatar",def);
	m_szPartMessage = cfg.readQStringEntry("PartMessage");
	m_szQuitMessage= cfg.readQStringEntry("QuitMessage");
	m_szAge = cfg.readQStringEntry("Age");
	m_szGender = cfg.readQStringEntry("Gender");
	m_szLocation = cfg.readQStringEntry("Location");
	m_szLanguages = cfg.readQStringEntry("Languages");
	m_szOtherInfo = cfg.readQStringEntry("OtherInfo");
	m_szUserMode = cfg.readQStringEntry("UserMode");
	m_szOnConnectCommand = cfg.readQStringEntry("OnConnectCommand");
	m_szOnLoginCommand = cfg.readQStringEntry("OnLoginCommand");
	return !(m_szId.isEmpty() || m_szNickName.isEmpty());
}

bool KviUserIdentity::save(KviConfig &cfg)
{
	cfg.setGroup(m_szId);
	cfg.writeEntry("NickName",m_szNickName);
	cfg.writeEntry("AltNickName1",m_szAltNickName1);
	cfg.writeEntry("AltNickName2",m_szAltNickName2);
	cfg.writeEntry("AltNickName3",m_szAltNickName3);
	cfg.writeEntry("UserName",m_szUserName);
	// FIXME: At least scramble the pass ?
	cfg.writeEntry("Password",m_szPassword);
	cfg.writeEntry("Avatar",m_pixAvatar);
	cfg.writeEntry("PartMessage",m_szPartMessage);
	cfg.writeEntry("QuitMessage",m_szQuitMessage);
	cfg.writeEntry("Age",m_szAge);
	cfg.writeEntry("Gender",m_szGender);
	cfg.writeEntry("Location",m_szLocation);
	cfg.writeEntry("Languages",m_szLanguages);
	cfg.writeEntry("OtherInfo",m_szOtherInfo);
	cfg.writeEntry("UserMode",m_szUserMode);
	cfg.writeEntry("OnConnectCommand",m_szOnConnectCommand);
	cfg.writeEntry("OnLoginCommand",m_szOnLoginCommand);
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

	m_szOnConnectCommand = src.m_szOnConnectCommand;
	m_szOnLoginCommand = src.m_szOnLoginCommand;
}


KviUserIdentityManager * KviUserIdentityManager::m_pInstance = 0;

KviUserIdentityManager::KviUserIdentityManager()
: KviHeapObject()
{
	m_pIdentityDict = new KviPointerHashTable<QString,KviUserIdentity>();
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
	KviPointerHashTableIterator<QString,KviUserIdentity> it(*m_pIdentityDict);
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

	ret->setAltNickName1(QString(KVI_DEFAULT_NICKNAME2).replace(KVI_DEFAULT_NICKNAME_TOKEN, KVI_DEFAULT_NICKNAME1));
	ret->setAltNickName2(QString(KVI_DEFAULT_NICKNAME3).replace(KVI_DEFAULT_NICKNAME_TOKEN, KVI_DEFAULT_NICKNAME1));
	ret->setAltNickName3(QString(KVI_DEFAULT_NICKNAME4).replace(KVI_DEFAULT_NICKNAME_TOKEN, KVI_DEFAULT_NICKNAME1));
	ret->setUserName(KVI_DEFAULT_USERNAME);
	ret->setRealName(KVI_DEFAULT_REALNAME);
	ret->setPartMessage(KVI_DEFAULT_PART_MESSAGE);
	ret->setQuitMessage(KVI_DEFAULT_QUIT_MESSAGE);

	m_pIdentityDict->replace(ret->id(),ret);

	return ret;
}

void KviUserIdentityManager::load(const QString &szFileName)
{
	m_pIdentityDict->clear();

	KviConfig cfg(szFileName,KviConfig::Read);

	cfg.setGroup("KVIrc");

	m_szDefaultIdentity = cfg.readQStringEntry("DefaultIdentity","");

	KviConfigIterator it(*(cfg.dict()));
	KviConfigGroup * grp;
	while( (grp = it.current()) )
	{
		if(!KviQString::equalCI(it.currentKey(),"KVIrc"))
		{
			cfg.setGroup(it.currentKey());

			KviUserIdentity * id = new KviUserIdentity();
			if(id->load(cfg))
				m_pIdentityDict->replace(id->id(),id);
			else
				delete id;
		}
		++it;
	}
}

void KviUserIdentityManager::save(const QString &szFileName)
{
	KviConfig cfg(szFileName,KviConfig::Write);
	cfg.clear();

	cfg.setGroup("KVIrc");

	cfg.writeEntry("DefaultIdentity",m_szDefaultIdentity);

	KviPointerHashTableIterator<QString,KviUserIdentity> it(*m_pIdentityDict);
	while(KviUserIdentity * id = it.current())
	{
		id->save(cfg);
		++it;
	}
}

void KviUserIdentityManager::copyFrom(KviUserIdentityManager * pWorkingCopy)
{
	m_pIdentityDict->clear();
	m_szDefaultIdentity = pWorkingCopy->m_szDefaultIdentity;
	KviPointerHashTableIterator<QString,KviUserIdentity> it(*(pWorkingCopy->m_pIdentityDict));
	while(KviUserIdentity * id = it.current())
	{
		KviUserIdentity * pNew = new KviUserIdentity();
		pNew->copyFrom(*id);
		m_pIdentityDict->replace(pNew->id(),pNew);
		++it;
	}
}

KviUserIdentityManager * KviUserIdentityManager::createWorkingCopy()
{
	KviUserIdentityManager * pCopy = new KviUserIdentityManager();
	pCopy->copyFrom(this);
	return pCopy;
}

void KviUserIdentityManager::releaseWorkingCopy(KviUserIdentityManager * pWorkingCopy)
{
	if(pWorkingCopy)delete pWorkingCopy;
}
