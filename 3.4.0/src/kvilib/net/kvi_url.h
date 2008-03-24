#ifndef _KVI_URL_H_
#define _KVI_URL_H_
//
//   File : kvi_url.h
//   Creation date : Sat Aug 17 14:09:16 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_string.h"
#include "kvi_heapobject.h"
#include "kvi_inttypes.h"

class KVILIB_API KviUrl : public KviHeapObject
{
public:
	KviUrl();
	KviUrl(const char * szUrl);
	KviUrl(const QString &szUrl);
	KviUrl(const KviUrl &u);
	~KviUrl();
protected:
	KviStr             m_szUrl;

	KviStr             m_szProtocol;
	KviStr             m_szHost;
	KviStr             m_szPath;
	KviStr             m_szUser;
	KviStr             m_szPass;
	kvi_u32_t          m_uPort;
protected:
	void parse();
public:
	const KviStr & url() const { return m_szUrl; };
	const KviStr & protocol() const { return m_szProtocol; };
	const KviStr & host() const { return m_szHost; };
	const KviStr & path() const { return m_szPath; };
	const KviStr & user() const { return m_szUser; };
	const KviStr & pass() const { return m_szPass; };
	kvi_u32_t port() const { return m_uPort; };

	KviUrl & operator = (const char * szUrl);
	KviUrl & operator = (const KviUrl &u);

};


#endif //_KVI_URL_H_
