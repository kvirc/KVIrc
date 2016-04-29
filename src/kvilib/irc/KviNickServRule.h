#ifndef _KVI_NICKSERV_RULE_H_
#define _KVI_NICKSERV_RULE_H_
//=============================================================================
//
//   File : KviNickServRule.h
//   Creation date : Thu Aug 09 2001 16:43:56 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"
#include "KviHeapObject.h"

#include <QString>

class KviConfigurationFile;
class KviIrcMask;

class KVILIB_API KviNickServRule : public KviHeapObject
{
protected:
	QString m_szRegisteredNick;  // my registered nickname
	QString m_szNickServMask;    // the NickServ service mask
	QString m_szMessageRegexp;   // the NickServ message regexp
	QString m_szIdentifyCommand; // the IDENTIFY command to send to server
	QString m_szServerMask;      // the mask that the server must match (not used in per-network rules)
public:
	KviNickServRule() : KviHeapObject(){};
	KviNickServRule(
	    const QString & szRegisteredNick,
	    const QString & szNickServMask,
	    const QString & szMessageRegexp,
	    const QString & szIdentifyCommand,
	    const QString & szServerMask = QString::null)
	    : KviHeapObject(),
	      m_szRegisteredNick(szRegisteredNick),
	      m_szNickServMask(szNickServMask),
	      m_szMessageRegexp(szMessageRegexp),
	      m_szIdentifyCommand(szIdentifyCommand),
	      m_szServerMask(szServerMask){};

public:
	const QString & registeredNick() const { return m_szRegisteredNick; };
	const QString & nickServMask() const { return m_szNickServMask; };
	const QString & messageRegexp() const { return m_szMessageRegexp; };
	const QString & identifyCommand() const { return m_szIdentifyCommand; };
	const QString & serverMask() const { return m_szServerMask; };
	void setRegisteredNick(const QString & szRegisteredNick) { m_szRegisteredNick = szRegisteredNick; };
	void setNickServMask(const QString & szNickServMask) { m_szNickServMask = szNickServMask; };
	void setMessageRegexp(const QString & szMessageRegexp) { m_szMessageRegexp = szMessageRegexp; };
	void setIdentifyCommand(const QString & szIdentifyCommand) { m_szIdentifyCommand = szIdentifyCommand; };
	void setServerMask(const QString & szServerMask) { m_szServerMask = szServerMask; };
public:
	// avoid crashes under windows
	static KviNickServRule * createInstance(const QString & szRegisteredNick,
	    const QString & szNickServMask,
	    const QString & szMessageRegexp,
	    const QString & szIdentifyCommand,
	    const QString & szServerMask = QString::null);

	void save(KviConfigurationFile * cfg, const QString & prefix);
	// returns false if the loaded data has no sense
	bool load(KviConfigurationFile * cfg, const QString & prefix);
	void copyFrom(const KviNickServRule & src);
};

#endif // _KVI_NICKSERV_RULE_H_
