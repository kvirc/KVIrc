#ifndef _KVI_KVS_VARIANTLIST_H_
#define _KVI_KVS_VARIANTLIST_H_
//=============================================================================
//
//   File : kvi_kvs_variantlist.h
//   Creation date : Sat 11 Oct 2003 02:20:51 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2008 Szymon Stefanek <pragma at kvirc dot net>
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
#include "kvi_pointerlist.h"
#include "kvi_kvs_variant.h"



class KVIRC_API KviKvsVariantList
{
public:
	KviKvsVariantList();
	KviKvsVariantList(KviKvsVariant * v1);
	KviKvsVariantList(KviKvsVariant * v1,KviKvsVariant * v2);
	KviKvsVariantList(KviKvsVariant * v1,KviKvsVariant * v2,KviKvsVariant * v3);
	KviKvsVariantList(KviKvsVariant * v1,KviKvsVariant * v2,KviKvsVariant * v3,KviKvsVariant * v4);
	KviKvsVariantList(KviKvsVariant * v1,KviKvsVariant * v2,KviKvsVariant * v3,KviKvsVariant * v4,KviKvsVariant * v5);
	KviKvsVariantList(KviKvsVariant * v1,KviKvsVariant * v2,KviKvsVariant * v3,KviKvsVariant * v4,KviKvsVariant * v5,KviKvsVariant * v6);
	KviKvsVariantList(KviKvsVariant * v1,KviKvsVariant * v2,KviKvsVariant * v3,KviKvsVariant * v4,KviKvsVariant * v5,KviKvsVariant * v6,KviKvsVariant * v7);
	KviKvsVariantList(QString * s1);
	KviKvsVariantList(QString * s1,QString * s2);
	KviKvsVariantList(QString * s1,QString * s2,QString * s3);
	KviKvsVariantList(QString * s1,QString * s2,QString * s3,QString * s4);
	KviKvsVariantList(QString * s1,QString * s2,QString * s3,QString * s4,QString * s5);
	KviKvsVariantList(QString * s1,QString * s2,QString * s3,QString * s4,QString * s5,QString * s6);
	~KviKvsVariantList();
protected:
	KviPointerList<KviKvsVariant> * m_pList;
public:
	KviKvsVariant * first(){ return m_pList->first(); };
	KviKvsVariant * next(){ return m_pList->next(); };
	KviKvsVariant * at(int iIdx){ return m_pList->at(iIdx); };
	unsigned int count(){ return m_pList->count(); };

	void clear(){ m_pList->clear(); };

	void append(const QString &szParam){ m_pList->append(new KviKvsVariant(szParam)); };
	void append(kvs_int_t iInteger){ m_pList->append(new KviKvsVariant(iInteger)); };
	void append(kvs_real_t dReal){ m_pList->append(new KviKvsVariant(dReal)); };
	void append(bool bBoolean){ m_pList->append(new KviKvsVariant(bBoolean)); };
	void append(kvs_hobject_t hObject){ m_pList->append(new KviKvsVariant(hObject)); };
	void append(KviKvsArray * pArray){ m_pList->append(new KviKvsVariant(pArray)); };
	void append(KviKvsHash * pHash){ m_pList->append(new KviKvsVariant(pHash)); };

	void append(KviKvsVariant * v){ m_pList->append(v); };
	void prepend(KviKvsVariant * v){ m_pList->prepend(v); };

	void setAutoDelete(bool bAutoDelete); // this is TRUE by default

	void allAsString(QString &szBuffer);
	// returns true if there was a first parameter at all
	bool firstAsString(QString &szBuffer);
	// returns true if there was a next parameter at all
	bool nextAsString(QString &szBuffer);
};

#endif //!_KVI_KVS_VARIANTLIST_H_
