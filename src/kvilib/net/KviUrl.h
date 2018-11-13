#ifndef _KVI_URL_H_
#define _KVI_URL_H_
//=============================================================================
//
//   File : KviUrl.h
//   Creation date : Sat Aug 17 14:09:16 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviHeapObject.h"
#include "kvi_inttypes.h"

#include <QString>

class KVILIB_API KviUrl : public KviHeapObject
{
public:
	KviUrl();
	KviUrl(const char * szUrl);
	KviUrl(const QString & szUrl);
	KviUrl(const KviUrl &);
	~KviUrl();

protected:
	QString m_szUrl;

	QString m_szProtocol;
	QString m_szHost;
	QString m_szPath;
	QString m_szUser;
	QString m_szPass;
	kvi_u32_t m_uPort;

protected:
	void parse();
	void build();

public:
	const QString & url() const { return m_szUrl; };
	const QString & protocol() const { return m_szProtocol; };
	const QString & host() const { return m_szHost; };
	const QString & path() const { return m_szPath; };
	const QString & user() const { return m_szUser; };
	const QString & pass() const { return m_szPass; };
	kvi_u32_t port() const { return m_uPort; };

	void setUrl(QString & szUrl);
	void setProtocol(QString & szProtocol);
	void setHost(QString & szHost);
	void setPath(QString & szPath);
	void setUser(QString & szUser);
	void setPass(QString & szPass);
	void setPort(kvi_u32_t uPort);

	KviUrl & operator=(const QString & szUrl);
	KviUrl & operator=(const KviUrl &);
};

#endif //_KVI_URL_H_
