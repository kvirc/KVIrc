#ifndef _KviRegisteredUser_h_
#define _KviRegisteredUser_h_
//=============================================================================
//
//   File : KviRegisteredUser.h
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

//
// this file was originally part of KviRegisteredUserDataBase.h
//

#include "kvi_settings.h"
#include "KviHeapObject.h"
#include "KviPointerHashTable.h"
#include "KviPointerList.h"

class KviIrcMask;
class QString;

class KVILIB_API KviRegisteredUser : public KviHeapObject
{
	friend class KviRegisteredUserDataBase;

public:
	enum IgnoreFlags
	{
		Channel = 1,
		Query = 2,
		Notice = 4,
		Ctcp = 8,
		Invite = 16,
		Dcc = 32,
		Highlight = 64
	};

	KviRegisteredUser(const QString & szName);
	~KviRegisteredUser();

private:
	int m_iIgnoreFlags;
	bool m_bIgnoreEnabled;
	QString m_szName;
	QString m_szGroup;
	KviPointerHashTable<QString, QString> * m_pPropertyDict; // owned properties
	KviPointerList<KviIrcMask> * m_pMaskList;                // owned masks
protected:
	// mask ownership is transferred! (always!) returns false if the mask was already there
	bool addMask(KviIrcMask * pMask);
	bool removeMask(KviIrcMask * pMask);
	KviIrcMask * findMask(const KviIrcMask & mask);

public:
	int ignoreFlags() { return m_iIgnoreFlags; };
	void setIgnoreFlags(int iFlags) { m_iIgnoreFlags = iFlags; };
	bool ignoreEnabled() { return m_bIgnoreEnabled; };
	void setIgnoreEnabled(bool bEnabled) { m_bIgnoreEnabled = bEnabled; };
	bool isIgnoreEnabledFor(IgnoreFlags flag);

	const QString & name() { return m_szName; };
	bool matches(const KviIrcMask & mask);
	bool matchesFixed(const KviIrcMask & mask);
	bool matchesFixed(const QString & szNick, const QString & szUser, const QString & szHost);

	void setProperty(const QString & szName, const QString & szValue);
	void setProperty(const QString & szName, bool szValue);

	void setGroup(const QString & szName) { m_szGroup = szName; };
	const QString & group() { return m_szGroup; };

	const QString & getProperty(const QString & szName);             // returns 0 if the property is not there
	bool getProperty(const QString & szName, QString & szValue);     // returns false if the property is not there
	bool getBoolProperty(const QString & szName, bool bDef = false); // returns true if the property is there and is true
	// the propertyDict may be 0!
	KviPointerHashTable<QString, QString> * propertyDict() { return m_pPropertyDict; };
	// this is never zero (but may contain no masks)
	KviPointerList<KviIrcMask> * maskList() { return m_pMaskList; };
};

#endif //!_KviRegisteredUser_h_
