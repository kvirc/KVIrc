#ifndef _KVI_NICKSERV_H_
#define _KVI_NICKSERV_H_
//=============================================================================
//
//   File : kvi_nickserv.h
//   Creation date : Thu Aug 09 2001 16:43:56 by Szymon Stefanek
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

#include "kvi_settings.h"
#include "kvi_qstring.h"
#include "kvi_heapobject.h"
#include "kvi_pointerlist.h"

class KviConfig;
class KviIrcMask;

class KVILIB_API KviNickServRule : public KviHeapObject
{
protected:
	QString m_szRegisteredNick;         // my registered nickname
	QString m_szNickServMask;           // the NickServ service mask
	QString m_szMessageRegexp;          // the NickServ message regexp
	QString m_szIdentifyCommand;        // the IDENTIFY command to send to server
	QString m_szServerMask;             // the mask that the server must match (not used in per-network rules)
public:
	KviNickServRule() : KviHeapObject() {};
	KviNickServRule(
		const QString &szRegisteredNick,
		const QString &szNickServMask,
		const QString &szMessageRegexp,
		const QString &szIdentifyCommand,
		const QString &szServerMask = QString::null)
		: KviHeapObject(),
			m_szRegisteredNick(szRegisteredNick),
			m_szNickServMask(szNickServMask),
			m_szMessageRegexp(szMessageRegexp),
			m_szIdentifyCommand(szIdentifyCommand),
			m_szServerMask(szServerMask)
		{};
public:
	const QString & registeredNick() const { return m_szRegisteredNick; };
	const QString & nickServMask() const { return m_szNickServMask; };
	const QString & messageRegexp() const { return m_szMessageRegexp; };
	const QString & identifyCommand() const { return m_szIdentifyCommand; };
	const QString & serverMask() const { return m_szServerMask; };
	void setRegisteredNick(const QString &szRegisteredNick){ m_szRegisteredNick = szRegisteredNick; };
	void setNickServMask(const QString &szNickServMask){ m_szNickServMask = szNickServMask; };
	void setMessageRegexp(const QString &szMessageRegexp){ m_szMessageRegexp = szMessageRegexp; };
	void setIdentifyCommand(const QString &szIdentifyCommand){ m_szIdentifyCommand = szIdentifyCommand; };
	void setServerMask(const QString &szServerMask){ m_szServerMask = szServerMask; };
public:
	// avoid crashes under windows
	static KviNickServRule * createInstance(const QString &szRegisteredNick,
		const QString &szNickServMask,
		const QString &szMessageRegexp,
		const QString &szIdentifyCommand,
		const QString &szServerMask = QString::null);

	void save(KviConfig * cfg,const QString &prefix);
	// returns false if the loaded data has no sense
	bool load(KviConfig * cfg,const QString &prefix);
	void copyFrom(const KviNickServRule &src);
};

class KVILIB_API KviNickServRuleSet : public KviHeapObject
{
public:
	KviNickServRuleSet();
	KviNickServRuleSet(const KviNickServRuleSet &s);
	~KviNickServRuleSet();
protected:
	KviPointerList<KviNickServRule> * m_pRules; // FIXME: Replace with KviPointerHashTable<QString,KviPointerList>
	bool                          m_bEnabled;
public:
	// avoid crashes under windows
	static KviNickServRuleSet * createInstance();
	void clear();
	bool isEnabled(){ return m_bEnabled; };
	void setEnabled(bool bEnabled){ m_bEnabled = bEnabled; };
	bool isEmpty(){ return m_pRules ? m_pRules->isEmpty() : true; };
	void addRule(KviNickServRule * r);
	KviNickServRule * matchRule(const QString &szNick,const KviIrcMask *nickServ,const QString &szMsg,const QString &szServer = QString::null);
	void copyFrom(const KviNickServRuleSet &src);
	void load(const QString &szConfigFile);
	void save(const QString &szConfigFile);
	void save(KviConfig * cfg,const QString &prefix);
	KviPointerList<KviNickServRule> * rules(){ return m_pRules; };
	static KviNickServRuleSet * load(KviConfig * cfg,const QString &prefix);
protected:
	bool loadPrivate(KviConfig * cfg,const QString &prefix,unsigned int nEntries);
};


#endif // _KVI_NICKSERV_H_
