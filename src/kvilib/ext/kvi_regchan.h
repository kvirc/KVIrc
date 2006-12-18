#ifndef _KVI_REGCHAN_H_
#define _KVI_REGCHAN_H_
//
//   File : kvi_regchan.h
//   Creation date : Sat Jun 29 01:01:15 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"
#include "kvi_heapobject.h"
#include "kvi_string.h"
#include "kvi_list.h"

#include <qasciidict.h>

class KVILIB_API KviRegisteredChannel : public KviHeapObject
{
	friend class KviRegisteredChannelDataBase;
public:
	KviRegisteredChannel(const KviStr &name,const KviStr &netmask);
	~KviRegisteredChannel();
protected:
	KviStr               m_szName;
	KviStr               m_szNetMask;
	QAsciiDict<KviStr> * m_pPropertyDict;
public:
	QAsciiDict<KviStr> * propertyDict(){ return m_pPropertyDict; };
	const KviStr & name(){ return m_szName; };
	const KviStr & netMask(){ return m_szNetMask; };
	KviStr * property(const char * name){ return m_pPropertyDict->find(name); };
	// val must be allocated with NEW!
	void setProperty(const char * name,KviStr * val){ m_pPropertyDict->replace(name,val); };
	void removeProperty(const char * name){ m_pPropertyDict->remove(name); };
};

typedef KVILIB_API KviPtrList<KviRegisteredChannel> KviRegisteredChannelList;

class KVILIB_API KviRegisteredChannelDataBase
{
public:
	KviRegisteredChannelDataBase();
	~KviRegisteredChannelDataBase();
protected:
	QAsciiDict<KviRegisteredChannelList> * m_pChannelDict;
public:
	QAsciiDict<KviRegisteredChannelList> * channelDict(){ return m_pChannelDict; };
	KviRegisteredChannel * find(const char * name,const char * net);
	KviRegisteredChannel * findExact(const char * name,const char * netmask);
	void remove(KviRegisteredChannel * c);
	void add(KviRegisteredChannel * c);
	void load(const char * filename);
	void save(const char * filename);
};


#endif //_KVI_REGCHAN_H_
