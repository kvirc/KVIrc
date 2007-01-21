#ifndef _KVI_USERIDENTITY_H_
#define _KVI_USERIDENTITY_H_
//=============================================================================
//
//   File : kvi_useridentity.h
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

#include "kvi_settings.h"
#include "kvi_heapobject.h"
#include "kvi_pixmap.h"
#include "kvi_config.h"
#include "kvi_dict.h"

class KVILIB_API KviUserIdentity : public KviHeapObject
{
	friend class KviUserIdentityManager;
public:
	KviUserIdentity()
	: KviHeapObject()
	{
	}
	~KviUserIdentity()
	{
	}
protected:
	QString m_szId; // the identity set name

	QString m_szNickName;
	
	QString m_szAltNickName1;
	QString m_szAltNickName2;
	QString m_szAltNickName3;
	
	QString m_szUserName;
	QString m_szRealName;
	QString m_szPassword;
	
	KviPixmap m_pixAvatar;
	
	QString m_szPartMessage;
	QString m_szQuitMessage;
	
	QString m_szAge;
	QString m_szGender;
	QString m_szLocation;
	QString m_szLanguages;
	QString m_szOtherInfo;
	
	QString m_szUserMode;
public:
	const QString & id() const { return m_szId; };
	const QString & nickName() const { return m_szNickName; };
	const QString & altNickName1() const { return m_szAltNickName1; };
	const QString & altNickName2() const { return m_szAltNickName2; };
	const QString & altNickName3() const { return m_szAltNickName3; };
	const QString & userName() const { return m_szUserName; };
	const QString & password() const { return m_szPassword; };
	const KviPixmap & avatar() const { return m_pixAvatar; };
	const QString & partMessage() const { return m_szPartMessage; };
	const QString & quitMessage() const { return m_szQuitMessage; };
	const QString & age() const { return m_szAge; };
	const QString & gender() const { return m_szGender; };
	const QString & location() const { return m_szLocation; };
	const QString & languages() const { return m_szLanguages; };
	const QString & otherInfo() const { return m_szOtherInfo; };
	const QString & userMode() const { return m_szUserMode; };
	void setId(const QString &szId){ m_szId = szId; };
	void setNickName(const QString &szNickName){ m_szNickName = szNickName; };
	void setAltNickName1(const QString &szNickName){ m_szAltNickName1 = szNickName; };
	void setAltNickName2(const QString &szNickName){ m_szAltNickName2 = szNickName; };
	void setAltNickName3(const QString &szNickName){ m_szAltNickName3 = szNickName; };
	void setUserName(const QString &szUserName){ m_szUserName = szUserName; };
	void setRealName(const QString &szRealName){ m_szRealName = szRealName; };
	void setPassword(const QString &szPassword){ m_szPassword = szPassword; };
	void setAvatar(const KviPixmap &pix){ m_pixAvatar = pix; };
	void setPartMessage(const QString &szMsg){ m_szPartMessage = szMsg; };
	void setQuitMessage(const QString &szMsg){ m_szQuitMessage = szMsg; };
	void setAge(const QString &szAge){ m_szAge = szAge; };
	void setGender(const QString &szGender){ m_szGender = szGender; };
	void setLocation(const QString &szLocation){ m_szLocation = szLocation; };
	void setLanguages(const QString &szLanguages){ m_szLanguages = szLanguages; };
	void setOtherInfo(const QString &szOtherInfo){ m_szOtherInfo = szOtherInfo; };
	void setUserMode(const QString &szUserMode){ m_szUserMode = szUserMode; };
protected:
	void copyFrom(const KviUserIdentity &src);
	bool save(KviConfig &cfg,const QString &szPrefix);
	bool load(KviConfig &cfg,const QString &szPrefix);
};

class KVILIB_API KviUserIdentityManager : public KviHeapObject
{
public:
	KviUserIdentityManager();
	~KviUserIdentityManager();
protected:
	static KviUserIdentityManager * m_pInstance;
	KviDict<KviUserIdentity> * m_pIdentityDict;
	QString m_szDefaultIdentity;
public:
	static void init();
	static void done();
	static KviUserIdentityManager * instance(){ return m_pInstance; };

	KviDict<KviUserIdentity> * identityDict(){ return m_pIdentityDict; };
	
	const KviUserIdentity * defaultIdentity();

	void setDefaultIdentity(const QString &szIdentityId){ m_szDefaultIdentity = szIdentityId; };
	
	KviUserIdentityManager * getWorkingCopy();
	void setValuesFrom(KviUserIdentityManager * pWorkingCopy);

	void save(const QString &szFileName);
	void load(const QString &szFileName);
};

#endif //!_KVI_USERIDENTITY_H_
