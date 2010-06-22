//=============================================================================
//
//   File : kvi_kvs_variant.cpp
//   Creation date : Tue 07 Oct 2003 04:01:19 by Szymon Stefanek
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



#include "kvi_kvs_variant.h"
#include "kvi_kvs_arraycast.h"
#include "kvi_kvs_hash.h"
#include "kvi_kvs_array.h"

#include <math.h>

KviKvsVariant::KviKvsVariant()
{
	m_pData = 0;
}

KviKvsVariant::KviKvsVariant(QString * pString)
{
	m_pData = new KviKvsVariantData;
	m_pData->m_eType = KviKvsVariantData::String;
	m_pData->m_uRefs = 1;
	m_pData->m_u.pString = pString;
}

KviKvsVariant::KviKvsVariant(const QString &szString)
{
	m_pData = new KviKvsVariantData;
	m_pData->m_eType = KviKvsVariantData::String;
	m_pData->m_uRefs = 1;
	m_pData->m_u.pString = new QString(szString);
}

KviKvsVariant::KviKvsVariant(const char * szString)
{
	m_pData = new KviKvsVariantData;
	m_pData->m_eType = KviKvsVariantData::String;
	m_pData->m_uRefs = 1;
	m_pData->m_u.pString = new QString(QString::fromUtf8(szString));
}

KviKvsVariant::KviKvsVariant(KviKvsArray * pArray)
{
	m_pData = new KviKvsVariantData;
	m_pData->m_eType = KviKvsVariantData::Array;
	m_pData->m_uRefs = 1;
	m_pData->m_u.pArray = pArray;
}

KviKvsVariant::KviKvsVariant(KviKvsHash * pHash)
{
	m_pData = new KviKvsVariantData;
	m_pData->m_eType = KviKvsVariantData::Hash;
	m_pData->m_uRefs = 1;
	m_pData->m_u.pHash = pHash;
}

KviKvsVariant::KviKvsVariant(kvs_real_t * pReal)
{
	m_pData = new KviKvsVariantData;
	m_pData->m_eType = KviKvsVariantData::Real;
	m_pData->m_uRefs = 1;
	m_pData->m_u.pReal = pReal;
}

KviKvsVariant::KviKvsVariant(kvs_real_t dReal)
{
	m_pData = new KviKvsVariantData;
	m_pData->m_eType = KviKvsVariantData::Real;
	m_pData->m_uRefs = 1;
	m_pData->m_u.pReal = new kvs_real_t;
	*(m_pData->m_u.pReal) = dReal;
}

KviKvsVariant::KviKvsVariant(bool bBoolean)
{
	m_pData = new KviKvsVariantData;
	m_pData->m_eType = KviKvsVariantData::Boolean;
	m_pData->m_uRefs = 1;
	m_pData->m_u.bBoolean = bBoolean;
}

KviKvsVariant::KviKvsVariant(kvs_int_t iInteger)
{
	m_pData = new KviKvsVariantData;
	m_pData->m_eType = KviKvsVariantData::Integer;
	m_pData->m_uRefs = 1;
	m_pData->m_u.iInteger = iInteger;
}

KviKvsVariant::KviKvsVariant(kvs_hobject_t hObject)
{
	m_pData = new KviKvsVariantData;
	m_pData->m_eType = KviKvsVariantData::HObject;
	m_pData->m_uRefs = 1;
	m_pData->m_u.hObject = hObject;
}

KviKvsVariant::KviKvsVariant(const KviKvsVariant &v)
{
	m_pData = v.m_pData;
	if(m_pData)m_pData->m_uRefs++;
}

#define DELETE_VARIANT_CONTENTS \
	switch(m_pData->m_eType) \
	{ \
		case KviKvsVariantData::Array: delete m_pData->m_u.pArray; break; \
		case KviKvsVariantData::Hash: delete m_pData->m_u.pHash; break; \
		case KviKvsVariantData::String: delete m_pData->m_u.pString; break; \
		case KviKvsVariantData::Real: delete m_pData->m_u.pReal; break; \
		default: /* make gcc happy */ break; \
	}

#define DETACH_CONTENTS \
	if(m_pData) \
	{ \
		if(m_pData->m_uRefs <= 1) \
		{ \
			DELETE_VARIANT_CONTENTS \
			delete m_pData; \
		} else { \
			m_pData->m_uRefs--; \
		} \
	}

#define RENEW_VARIANT_DATA \
	if(m_pData) \
	{ \
		if(m_pData->m_uRefs > 1) \
		{ \
			m_pData->m_uRefs--; \
			m_pData = new KviKvsVariantData; \
			m_pData->m_uRefs = 1; \
		} else { \
			DELETE_VARIANT_CONTENTS \
		} \
	} else { \
		m_pData = new KviKvsVariantData; \
		m_pData->m_uRefs = 1; \
	}

KviKvsVariant::~KviKvsVariant()
{
	DETACH_CONTENTS
}

void KviKvsVariant::setString(QString * pString)
{
	RENEW_VARIANT_DATA
	m_pData->m_eType = KviKvsVariantData::String;
	m_pData->m_u.pString = pString;
}

void KviKvsVariant::setString(const QString &szString)
{
	RENEW_VARIANT_DATA
	m_pData->m_eType = KviKvsVariantData::String;
	m_pData->m_u.pString = new QString(szString);
}

void KviKvsVariant::setReal(kvs_real_t dReal)
{
	RENEW_VARIANT_DATA
	m_pData->m_eType = KviKvsVariantData::Real;
	m_pData->m_u.pReal = new kvs_real_t;
	*(m_pData->m_u.pReal) = dReal;
}

void KviKvsVariant::setHObject(kvs_hobject_t hObject)
{
	RENEW_VARIANT_DATA
	m_pData->m_eType = KviKvsVariantData::HObject;
	m_pData->m_u.hObject = hObject;
}

void KviKvsVariant::setBoolean(bool bBoolean)
{
	RENEW_VARIANT_DATA
	m_pData->m_eType = KviKvsVariantData::Boolean;
	m_pData->m_u.bBoolean = bBoolean;
}

void KviKvsVariant::setReal(kvs_real_t * pReal)
{
	RENEW_VARIANT_DATA
	m_pData->m_eType = KviKvsVariantData::Real;
	m_pData->m_u.pReal = pReal;
}

void KviKvsVariant::setInteger(kvs_int_t iInteger)
{
	RENEW_VARIANT_DATA
	m_pData->m_eType = KviKvsVariantData::Integer;
	m_pData->m_u.iInteger = iInteger;
}

void KviKvsVariant::setArray(KviKvsArray * pArray)
{
	RENEW_VARIANT_DATA
	m_pData->m_eType = KviKvsVariantData::Array;
	m_pData->m_u.pArray = pArray;
}

void KviKvsVariant::setHash(KviKvsHash * pHash)
{
	RENEW_VARIANT_DATA
	m_pData->m_eType = KviKvsVariantData::Hash;
	m_pData->m_u.pHash = pHash;
}

void KviKvsVariant::setNothing()
{
	if(m_pData)
	{
		if(m_pData->m_uRefs <= 1)
		{
			DELETE_VARIANT_CONTENTS
			delete m_pData;
		} else {
			// just detach
			m_pData->m_uRefs--;
		}
		m_pData = 0;
	}
}

bool KviKvsVariant::isEmpty() const
{
	if(!m_pData)return true;
	switch(m_pData->m_eType)
	{
		case KviKvsVariantData::String: return m_pData->m_u.pString->isEmpty(); break;
		case KviKvsVariantData::Array: return m_pData->m_u.pArray->isEmpty(); break;
		case KviKvsVariantData::Hash: return m_pData->m_u.pHash->isEmpty(); break;
		case KviKvsVariantData::HObject: return m_pData->m_u.hObject == 0; break;
		default: /* make gcc happy */ break;
	}
	return false;
}

bool KviKvsVariant::asBoolean() const
{
	if(!m_pData)return false;
	switch(m_pData->m_eType)
	{
		case KviKvsVariantData::Boolean: return m_pData->m_u.bBoolean; break;
		case KviKvsVariantData::String:
		{
			if(m_pData->m_u.pString->isEmpty())return false;
			// check integer or real values
			bool bOk;
			kvs_int_t iVal = (kvs_int_t)KviQString::toI64(*(m_pData->m_u.pString),&bOk);
			if(bOk)return iVal;
			kvs_real_t dVal = m_pData->m_u.pString->toDouble(&bOk);
			if(bOk)return (dVal != 0.0);
			// non number, non empty
			return true;
		}
		break;
		case KviKvsVariantData::Integer: return m_pData->m_u.iInteger; break;
		case KviKvsVariantData::Real: return *(m_pData->m_u.pReal) != 0.0; break;
		case KviKvsVariantData::Array: return !(m_pData->m_u.pArray->isEmpty()); break;
		case KviKvsVariantData::Hash: return !(m_pData->m_u.pHash->isEmpty()); break;
		case KviKvsVariantData::HObject: return m_pData->m_u.hObject; break;
		default: /* make gcc happy */ break;
	}
	qDebug("WARNING: invalid variant type %d in KviKvsVariant::asBoolean()",m_pData->m_eType);
	return false;
}

bool KviKvsVariant::asHObject(kvs_hobject_t &hObject) const
{
	if(!m_pData)
	{
		// nothing evaluates to a null object
		hObject = 0;
		return true;
	}
	switch(m_pData->m_eType)
	{
		case KviKvsVariantData::HObject:
			hObject = m_pData->m_u.hObject;
			return true;
		break;
		case KviKvsVariantData::Integer:
			if(m_pData->m_u.iInteger == 0)
			{
				hObject = 0;
				return true;
			}
			return false;
		break;
		case KviKvsVariantData::String:
			if(*(m_pData->m_u.pString) == "0")
			{
				hObject = 0;
				return true;
			}
			return false;
		break;
		case KviKvsVariantData::Boolean:
			if(!(m_pData->m_u.bBoolean))
			{
				hObject = 0;
				return true;
			}
		default: /* make gcc happy */ break;
	}
	return false;
}

bool KviKvsVariant::asNumber(KviKvsNumber &n) const
{
	if(!m_pData)return false;

	if(isInteger())
	{
		n.m_u.iInteger = m_pData->m_u.iInteger;
		n.m_type = KviKvsNumber::Integer;
		return true;
	}
	if(isReal())
	{
		n.m_u.dReal = *(m_pData->m_u.pReal);
		n.m_type = KviKvsNumber::Real;
		return true;
	}
	if(asInteger(n.m_u.iInteger))
	{
		n.m_type = KviKvsNumber::Integer;
		return true;
	}
	if(asReal(n.m_u.dReal))
	{
		n.m_type = KviKvsNumber::Real;
		return true;
	}
	return false;
}

void KviKvsVariant::castToNumber(KviKvsNumber &n) const
{
	if(!m_pData)
	{
		n.m_u.iInteger = 0;
		n.m_type = KviKvsNumber::Integer;
		return;
	}

	if(isInteger())
	{
		n.m_u.iInteger = m_pData->m_u.iInteger;
		n.m_type = KviKvsNumber::Integer;
		return;
	}
	if(isReal())
	{
		n.m_u.dReal = *(m_pData->m_u.pReal);
		n.m_type = KviKvsNumber::Real;
		return;
	}
	if(asInteger(n.m_u.iInteger))
	{
		n.m_type = KviKvsNumber::Integer;
		return;
	}
	if(asReal(n.m_u.dReal))
	{
		n.m_type = KviKvsNumber::Real;
		return;
	}
	castToInteger(n.m_u.iInteger);
	n.m_type = KviKvsNumber::Integer;
}

void KviKvsVariant::castToArray(KviKvsArrayCast *c) const
{
	if(!m_pData)
	{
		c->set(new KviKvsArray(),true);
		return;
	}

	switch(m_pData->m_eType)
	{
		case KviKvsVariantData::Array:
			c->set(m_pData->m_u.pArray,false);
		break;
		case KviKvsVariantData::Hash:
		{
			KviPointerHashTableIterator<QString,KviKvsVariant> it(*(m_pData->m_u.pHash->dict()));
			KviKvsArray * a = new KviKvsArray();
			kvs_int_t idx = 0;
			while(KviKvsVariant * v = it.current())
			{
				a->set(idx,new KviKvsVariant(*v));
				++it;
				idx++;
			}
			c->set(a,true);
		}
		break;
		default:
		{
			// other scalars
			KviKvsArray * a = new KviKvsArray();
			a->set(0,new KviKvsVariant(*this));
			c->set(a,true);
		}
		break;
	}
}

void KviKvsVariant::convertToArray()
{
	if(!m_pData)
	{
		setArray(new KviKvsArray());
		return;
	}

	switch(m_pData->m_eType)
	{
		case KviKvsVariantData::Array:
			return;
		break;
		case KviKvsVariantData::Hash:
		{
			KviPointerHashTableIterator<QString,KviKvsVariant> it(*(m_pData->m_u.pHash->dict()));
			KviKvsArray * a = new KviKvsArray();
			kvs_int_t idx = 0;
			while(KviKvsVariant * v = it.current())
			{
				a->set(idx,new KviKvsVariant(*v));
				++it;
				idx++;
			}
			setArray(a);
		}
		break;
		default:
		{
			// other scalars
			KviKvsArray * a = new KviKvsArray();
			a->set(0,new KviKvsVariant(*this));
			setArray(a);
		}
		break;
	}
}

bool KviKvsVariant::asInteger(kvs_int_t &iVal) const
{
	if(!m_pData)return false;
	switch(m_pData->m_eType)
	{
		case KviKvsVariantData::Integer:
			iVal = m_pData->m_u.iInteger;
			return true;
		break;
		case KviKvsVariantData::String:
		{
			bool bOk;
			iVal = (kvs_int_t)KviQString::toI64(*(m_pData->m_u.pString),&bOk);
			return bOk;
		}
		break;
		case KviKvsVariantData::Real:
			// FIXME: this truncates the value!
			iVal = (kvs_int_t)*(m_pData->m_u.pReal);
			return true;
		break;
		case KviKvsVariantData::Boolean:
			iVal = m_pData->m_u.bBoolean ? 1 : 0;
			return true;
		break;
		//case KviKvsVariantData::HObject: <-- light casts from objects to integer are not valid
		//	iVal = m_pData->m_u.hObject ? 1 : 0;
		//	return true;
		//break;
		default: /* make gcc happy */
		break;
	}
	return false;
}

void KviKvsVariant::castToInteger(kvs_int_t &iVal) const
{
	if(!m_pData)
	{
		iVal = 0;
		return;
	}
	switch(m_pData->m_eType)
	{
		case KviKvsVariantData::Integer:
			iVal = m_pData->m_u.iInteger;
		break;
		case KviKvsVariantData::Boolean:
			iVal = m_pData->m_u.bBoolean ? 1 : 0;
		break;
		case KviKvsVariantData::HObject:
			iVal = m_pData->m_u.hObject ? 1 : 0;
		break;
		case KviKvsVariantData::String:
		{
			bool bOk;
			iVal = (kvs_int_t)KviQString::toI64(*(m_pData->m_u.pString),&bOk);
			if(bOk)return;
			iVal = m_pData->m_u.pString->length();
		}
		break;
		case KviKvsVariantData::Real:
			// FIXME: this truncates the value!
			iVal = (kvs_int_t)*(m_pData->m_u.pReal);
		break;
		case KviKvsVariantData::Array:
			iVal = m_pData->m_u.pArray->size();
		break;
		case KviKvsVariantData::Hash:
			iVal = m_pData->m_u.pHash->size();
		break;
		default: /* make gcc happy */
			iVal = 0;
		break;
	}
}

bool KviKvsVariant::asReal(kvs_real_t & dVal) const
{
	if(!m_pData)return false;
	switch(m_pData->m_eType)
	{
		case KviKvsVariantData::Integer:
			dVal = m_pData->m_u.iInteger;
			return true;
		break;
		case KviKvsVariantData::String:
		{
			bool bOk;
			dVal = m_pData->m_u.pString->toDouble(&bOk);
			return bOk;
		}
		break;
		case KviKvsVariantData::Real:
			dVal = *(m_pData->m_u.pReal);
			return true;
		break;
		case KviKvsVariantData::Boolean:
			dVal = m_pData->m_u.bBoolean ? 1.0 : 0.0;
			return true;
		break;
		default: /* by default we make gcc happy */ break;
	}
	return false;
}

void KviKvsVariant::asString(QString &szBuffer) const
{
	if(!m_pData)
	{
		szBuffer = QString();
		return;
	}
	switch(m_pData->m_eType)
	{
		case KviKvsVariantData::String: szBuffer = *(m_pData->m_u.pString); break;
		case KviKvsVariantData::Array: szBuffer = QString(); m_pData->m_u.pArray->appendAsString(szBuffer); break;
		case KviKvsVariantData::Hash: szBuffer = QString(); m_pData->m_u.pHash->appendAsString(szBuffer); break;
		case KviKvsVariantData::Integer:  szBuffer.setNum(m_pData->m_u.iInteger); break;
		case KviKvsVariantData::Real: szBuffer.setNum(*(m_pData->m_u.pReal)); break;
		case KviKvsVariantData::Boolean: szBuffer.setNum(m_pData->m_u.bBoolean ? 1 : 0); break;
		case KviKvsVariantData::HObject:
			if(m_pData->m_u.hObject)
				KviQString::sprintf(szBuffer,"object[%lx]",m_pData->m_u.hObject);
			else
				szBuffer = "null-object";
		break;
		default: /* make gcc happy */ break;
	}
}

void KviKvsVariant::appendAsString(QString &szBuffer) const
{
	if(!m_pData)return;
	switch(m_pData->m_eType)
	{
		case KviKvsVariantData::String: szBuffer.append(*(m_pData->m_u.pString)); break;
		case KviKvsVariantData::Array: m_pData->m_u.pArray->appendAsString(szBuffer); break;
		case KviKvsVariantData::Hash: m_pData->m_u.pHash->appendAsString(szBuffer); break;
		case KviKvsVariantData::Integer:  KviQString::appendNumber(szBuffer,m_pData->m_u.iInteger); break;
		case KviKvsVariantData::Real: KviQString::appendNumber(szBuffer,*(m_pData->m_u.pReal)); break;
		case KviKvsVariantData::Boolean: KviQString::appendNumber(szBuffer,m_pData->m_u.bBoolean ? 1 : 0); break;
		case KviKvsVariantData::HObject: szBuffer.append(m_pData->m_u.hObject ? "object" : "null-object"); break;
		default: /* make gcc happy */ break;
	}
}

void KviKvsVariant::dump(const char * prefix) const
{
	if(!m_pData)
	{
		debug("%s Nothing [this=0x%lx]",prefix,(long unsigned int) this);
		return;
	}
	switch(m_pData->m_eType)
	{
		case KviKvsVariantData::String: debug("%s String(%s) [this=0x%lx]",prefix,m_pData->m_u.pString->toUtf8().data(),(long unsigned int)this); break;
		case KviKvsVariantData::Array: debug("%s Array(ptr=0x%lx) [this=0x%lx]",prefix,(long unsigned int)m_pData->m_u.pArray,(long unsigned int)this); break;
		case KviKvsVariantData::Hash: debug("%s Hash(ptr=0x%lx,dict=0x%lx) [this=0x%lx]",prefix,(long unsigned int)m_pData->m_u.pHash,(long unsigned int)m_pData->m_u.pHash->dict(),(long unsigned int)this); break;
		case KviKvsVariantData::Integer:  debug("%s Integer(%d) [this=0x%lx]",prefix,(int) m_pData->m_u.iInteger,(long unsigned int)this); break;
		case KviKvsVariantData::Real: debug("%s Real(%f) [this=0x%lx]",prefix,*(m_pData->m_u.pReal),(long unsigned int)this); break;
		case KviKvsVariantData::Boolean: debug("%s Boolean(%s) [this=0x%lx]",prefix,m_pData->m_u.bBoolean ? "true" : "false",(long unsigned int)this); break;
		case KviKvsVariantData::HObject: debug("%s HObject(%lx) [this=0x%lx]",prefix,(long unsigned int)m_pData->m_u.hObject,(long unsigned int)this); break;
		default: /* make gcc happy */ break;
	}
}

void KviKvsVariant::copyFrom(const KviKvsVariant * v)
{
	DETACH_CONTENTS
	m_pData = v->m_pData;
	if(m_pData)m_pData->m_uRefs++;
}

void KviKvsVariant::copyFrom(const KviKvsVariant &v)
{
	DETACH_CONTENTS
	m_pData = v.m_pData;
	if(m_pData)m_pData->m_uRefs++;

}

void KviKvsVariant::takeFrom(KviKvsVariant * v)
{
	DETACH_CONTENTS
	m_pData = v->m_pData;
	v->m_pData = 0;
}

void KviKvsVariant::takeFrom(KviKvsVariant &v)
{
	DETACH_CONTENTS
	m_pData = v.m_pData;
	v.m_pData = 0;
}

void KviKvsVariant::getTypeName(QString &szBuffer) const
{
	if(!m_pData)
	{
		szBuffer = "nothing";
		return;
	}
	switch(m_pData->m_eType)
	{
		case KviKvsVariantData::String: szBuffer = "string"; break;
		case KviKvsVariantData::Hash: szBuffer = "hash"; break;
		case KviKvsVariantData::Array: szBuffer = "array"; break;
		case KviKvsVariantData::Real: szBuffer = "real"; break;
		case KviKvsVariantData::Integer: szBuffer = "integer"; break;
		case KviKvsVariantData::Boolean: szBuffer = "boolean"; break;
		case KviKvsVariantData::HObject: szBuffer = "hobject"; break;
		default: szBuffer = "internal_error"; break;
	}
}

bool KviKvsVariant::isEqualToNothing() const
{
	if(!m_pData)return true;
	switch(m_pData->m_eType)
	{
		case KviKvsVariantData::HObject:
			return (m_pData->m_u.hObject == (kvs_hobject_t)0);
		break;
		case KviKvsVariantData::Integer:
			return (m_pData->m_u.iInteger == 0);
		break;
		case KviKvsVariantData::Real:
			return (*(m_pData->m_u.pReal) == 0.0);
		break;
		case KviKvsVariantData::String:
			{
				if(m_pData->m_u.pString->isEmpty())return true;
				kvs_real_t dReal;
				if(asReal(dReal))
					return dReal == 0.0;
				return false;
			}
		break;
		case KviKvsVariantData::Boolean:
			return !m_pData->m_u.bBoolean;
		break;
		case KviKvsVariantData::Hash:
			return m_pData->m_u.pHash->isEmpty();
		break;
		case KviKvsVariantData::Array:
			return m_pData->m_u.pArray->isEmpty();
		break;
		default:
		break;
	}

	return false;
}


#define CMP_THISGREATER -1
#define CMP_EQUAL 0
#define CMP_OTHERGREATER 1

class KviKvsVariantComparison
{
public:
	static inline int compare_integer_string(const KviKvsVariant * v1,const KviKvsVariant * v2)
	{
		if(v1->m_pData->m_u.iInteger == 0)
		{
			if(v2->m_pData->m_u.pString->isEmpty())return CMP_EQUAL;
		}
		kvs_real_t dReal;
		if(v2->asReal(dReal))
		{
			if(((kvs_real_t)v1->m_pData->m_u.iInteger) == dReal)return CMP_EQUAL;
			if(((kvs_real_t)v1->m_pData->m_u.iInteger) > dReal)return CMP_THISGREATER;
			return CMP_OTHERGREATER;
		}
		// compare as strings instead
		QString szString;
		v1->asString(szString);
		return -1 * KviQString::cmpCI(szString,*(v2->m_pData->m_u.pString));
	}

	static inline int compare_integer_real(const KviKvsVariant * v1,const KviKvsVariant * v2)
	{
		if(((kvs_real_t)v1->m_pData->m_u.iInteger) == *(v2->m_pData->m_u.pReal))return CMP_EQUAL;
		if(((kvs_real_t)v1->m_pData->m_u.iInteger) > *(v2->m_pData->m_u.pReal))return CMP_THISGREATER;
		return CMP_OTHERGREATER;
	}

	static inline int compare_integer_boolean(const KviKvsVariant * v1,const KviKvsVariant * v2)
	{
		if(v1->m_pData->m_u.iInteger == 0)
			return v2->m_pData->m_u.bBoolean ? CMP_OTHERGREATER : CMP_EQUAL;
		return v2->m_pData->m_u.bBoolean ? CMP_EQUAL : CMP_THISGREATER;
	}

	static inline int compare_integer_hash(const KviKvsVariant * v1,const KviKvsVariant * v2)
	{
		if(v1->m_pData->m_u.iInteger == 0)
			return v2->m_pData->m_u.pHash->isEmpty() ? CMP_EQUAL : CMP_OTHERGREATER;
		return CMP_THISGREATER;
	}

	static inline int compare_integer_array(const KviKvsVariant * v1,const KviKvsVariant * v2)
	{
		if(v1->m_pData->m_u.iInteger == 0)
			return v2->m_pData->m_u.pArray->isEmpty() ? CMP_EQUAL : CMP_OTHERGREATER;
		return CMP_THISGREATER;
	}

	static inline int compare_integer_hobject(const KviKvsVariant * v1,const KviKvsVariant * v2)
	{
		if(v1->m_pData->m_u.iInteger == 0.0)
			return (v2->m_pData->m_u.hObject == (kvs_hobject_t)0) ? CMP_EQUAL : CMP_THISGREATER;
		return CMP_OTHERGREATER;
	}

	static inline int compare_real_hobject(const KviKvsVariant * v1,const KviKvsVariant * v2)
	{
		if(*(v1->m_pData->m_u.pReal) == 0.0)
			return (v2->m_pData->m_u.hObject == (kvs_hobject_t)0) ? CMP_EQUAL : CMP_THISGREATER;
		return CMP_OTHERGREATER;
	}

	static inline int compare_real_string(const KviKvsVariant * v1,const KviKvsVariant * v2)
	{
		if(*(v1->m_pData->m_u.pReal) == 0.0)
		{
			if(v2->m_pData->m_u.pString->isEmpty())return CMP_EQUAL;
		}
		kvs_real_t dReal;
		if(v2->asReal(dReal))
		{
			if(*(v1->m_pData->m_u.pReal) == dReal)return CMP_EQUAL;
			if(*(v1->m_pData->m_u.pReal) > dReal)return CMP_THISGREATER;
			return CMP_OTHERGREATER;
		}
		// compare as strings instead
		QString szString;
		v1->asString(szString);
		return -1 * KviQString::cmpCI(szString,*(v2->m_pData->m_u.pString));
	}

	static inline int compare_real_boolean(const KviKvsVariant * v1,const KviKvsVariant * v2)
	{
		if(*(v1->m_pData->m_u.pReal) == 0.0)
			return v2->m_pData->m_u.bBoolean ? CMP_OTHERGREATER : CMP_EQUAL;
		return v2->m_pData->m_u.bBoolean ? CMP_EQUAL : CMP_THISGREATER;
	}

	static inline int compare_real_hash(const KviKvsVariant * v1,const KviKvsVariant * v2)
	{
		if(*(v1->m_pData->m_u.pReal) == 0)
			return v2->m_pData->m_u.pHash->isEmpty() ? CMP_EQUAL : CMP_OTHERGREATER;
		return CMP_THISGREATER;
	}

	static inline int compare_real_array(const KviKvsVariant * v1,const KviKvsVariant * v2)
	{
		if(*(v1->m_pData->m_u.pReal) == 0)
			return v2->m_pData->m_u.pArray->isEmpty() ? CMP_EQUAL : CMP_OTHERGREATER;
		return CMP_THISGREATER;
	}

	static inline int compare_string_hash(const KviKvsVariant * v1,const KviKvsVariant * v2)
	{
		if(v1->m_pData->m_u.pString->isEmpty())
		{
			return v2->m_pData->m_u.pHash->isEmpty() ? CMP_EQUAL : CMP_OTHERGREATER;
		}
		return CMP_THISGREATER;
	}

	static inline int compare_string_array(const KviKvsVariant * v1,const KviKvsVariant * v2)
	{
		if(v1->m_pData->m_u.pString->isEmpty())
		{
			return v2->m_pData->m_u.pArray->isEmpty() ? CMP_EQUAL : CMP_OTHERGREATER;
		}
		return CMP_THISGREATER;
	}

	static inline int compare_string_hobject(const KviKvsVariant * v1,const KviKvsVariant * v2)
	{
		if(v2->m_pData->m_u.hObject == (kvs_hobject_t)0)
		{
			if(v1->m_pData->m_u.pString->isEmpty())
				return CMP_EQUAL;
			kvs_real_t dReal;
			if(v1->asReal(dReal))
			{
				if(dReal == 0)return CMP_EQUAL;
			}
		}

		return CMP_THISGREATER;
	}

	static inline int compare_boolean_string(const KviKvsVariant * v1,const KviKvsVariant * v2)
	{
		if(v2->isEqualToNothing())
		{
			return v1->m_pData->m_u.bBoolean ? CMP_THISGREATER : CMP_EQUAL;
		} else {
			return v1->m_pData->m_u.bBoolean ? CMP_EQUAL : CMP_THISGREATER;
		}
	}

	static inline int compare_boolean_hash(const KviKvsVariant * v1,const KviKvsVariant * v2)
	{
		if(v1->m_pData->m_u.bBoolean)
			return v2->m_pData->m_u.pHash->isEmpty() ? CMP_THISGREATER : CMP_EQUAL;
		else
			return v2->m_pData->m_u.pHash->isEmpty() ? CMP_EQUAL : CMP_OTHERGREATER;
	}

	static inline int compare_boolean_array(const KviKvsVariant * v1,const KviKvsVariant * v2)
	{
		if(v1->m_pData->m_u.bBoolean)
			return v2->m_pData->m_u.pArray->isEmpty() ? CMP_THISGREATER : CMP_EQUAL;
		else
			return v2->m_pData->m_u.pArray->isEmpty() ? CMP_EQUAL : CMP_OTHERGREATER;
	}

	static inline int compare_boolean_hobject(const KviKvsVariant * v1,const KviKvsVariant * v2)
	{
		if(v1->m_pData->m_u.bBoolean)
			return v2->m_pData->m_u.hObject == ((kvs_hobject_t)0) ? CMP_THISGREATER : CMP_EQUAL;
		else
			return v2->m_pData->m_u.hObject == ((kvs_hobject_t)0) ? CMP_EQUAL : CMP_OTHERGREATER;
	}

	static inline int compare_array_hash(const KviKvsVariant * v1,const KviKvsVariant * v2)
	{
		if(v1->m_pData->m_u.pArray->size() > v2->m_pData->m_u.pHash->size())return CMP_THISGREATER;
		if(v1->m_pData->m_u.pArray->size() == v2->m_pData->m_u.pHash->size())return CMP_EQUAL;
		return CMP_OTHERGREATER;
	}

	static inline int compare_hobject_hash(const KviKvsVariant * v1,const KviKvsVariant * v2)
	{
		if(v2->m_pData->m_u.pHash->isEmpty())
			return v1->m_pData->m_u.hObject == ((kvs_hobject_t)0) ? CMP_EQUAL : CMP_OTHERGREATER;
		return v1->m_pData->m_u.hObject == ((kvs_hobject_t)0) ? CMP_THISGREATER : CMP_EQUAL;
	}

	static inline int compare_hobject_array(const KviKvsVariant * v1,const KviKvsVariant * v2)
	{
		if(v2->m_pData->m_u.pArray->isEmpty())
			return v1->m_pData->m_u.hObject == ((kvs_hobject_t)0) ? CMP_EQUAL : CMP_OTHERGREATER;
		return v1->m_pData->m_u.hObject == ((kvs_hobject_t)0) ? CMP_THISGREATER : CMP_EQUAL;
	}

};

void KviKvsVariant::serializeString(QString& buffer)
{
	buffer.replace('\\',"\\\\");
	buffer.replace('\n',"\\n");
	buffer.replace('\r',"\\r");
	buffer.replace('\b',"\\b");
	buffer.replace('\t',"\\t");
	buffer.replace('\f',"\\f");
	buffer.replace('/',"\\/");
	buffer.replace('"',"\\\"");
	buffer.prepend('"');
	buffer.append('"');
}

void KviKvsVariant::serialize(QString& result)
{
	if(!m_pData)
	{
		result = "null";
		return;
	}

	switch(m_pData->m_eType)
	{
		case KviKvsVariantData::HObject:
			//can't serialize objects yet
		break;
		case KviKvsVariantData::Integer:
			result.setNum(m_pData->m_u.iInteger);
		break;
		case KviKvsVariantData::Real:
			result.setNum(*(m_pData->m_u.pReal));
		break;
		case KviKvsVariantData::String:
			result = *(m_pData->m_u.pString);
			serializeString(result);
		break;
		case KviKvsVariantData::Boolean:
			result = m_pData->m_u.bBoolean ? "true" : "false";
		break;
		case KviKvsVariantData::Hash:
			m_pData->m_u.pHash->serialize(result);
		break;
		case KviKvsVariantData::Array:
			m_pData->m_u.pArray->serialize(result);
		break;
		case KviKvsVariantData::Nothing:
			result = "null";
		break;
		default: // just make gcc happy
		break;
	}
}

KviKvsVariant* KviKvsVariant::unserializeTrue(const QChar** aux)
{
	if(KviQString::equalCIN(QString("true"),*aux,4))
	{
		(*aux)+=4;
		return new KviKvsVariant(true);
	}
	return 0;
}

KviKvsVariant* KviKvsVariant::unserializeFalse(const QChar** aux)
{
	if(KviQString::equalCIN(QString("false"),*aux,5))
	{
		(*aux)+=5;
		return new KviKvsVariant(false);
	}
	return 0;
}

KviKvsVariant* KviKvsVariant::unserializeNull(const QChar** aux)
{
	if(KviQString::equalCIN(QString("null"),*aux,4))
	{
		(*aux)+=4;
		return new KviKvsVariant();
	}
	return 0;
}

KviKvsVariant* KviKvsVariant::unserializeRealOrInteger(const QChar** aux)
{
	QString data;
	if((*aux)->unicode() == '-')
	{
		data.append('-');
		(*aux)++;
	}
	if(!(*aux)->isDigit())
	{
		return 0;
	}
	while((*aux)->isDigit())
	{
		data.append(**aux);
		(*aux)++;
	}
	if((*aux)->unicode()=='.')
	{
		return unserializeReal(aux,data);
	}
	return unserializeInteger(aux,data);
}

KviKvsVariant* KviKvsVariant::unserializeReal(const QChar** aux,QString& data)
{
	QString exponent;
	(*aux)++; //skip .
	data.append('.');
	while((*aux)->isDigit())
	{
		data.append(**aux);
		(*aux)++;
	}

	if((*aux)->unicode() == 'e' || (*aux)->unicode() == 'E')
	{
		(*aux)++;
		if((*aux)->unicode() == '-')
		{
			exponent.append('-');
			(*aux)++;
		} else {
			if((*aux)->unicode() == '+')
			{
				exponent.append('+');
				(*aux)++;
			}
		}

		while((*aux)->isDigit())
		{
			exponent.append(**aux);
			(*aux)++;
		}
	}

	float value = data.toFloat();
	if(!exponent.isNull())
	{
		value*=pow(10.0,exponent.toInt());
	}
	return new KviKvsVariant(value);
}

KviKvsVariant* KviKvsVariant::unserializeInteger(const QChar** aux,QString& data)
{

	QString exponent;

	if((*aux)->unicode() == 'e' || (*aux)->unicode() == 'E')
	{
		(*aux)++;
		if((*aux)->unicode() == '-')
		{
			exponent.append('-');
			(*aux)++;
		} else {
			if((*aux)->unicode() == '+')
			{
				exponent.append('+');
				(*aux)++;
			}
		}

		while((*aux)->isDigit())
		{
			exponent.append(**aux);
			(*aux)++;
		}
	}

	kvs_int_t value = data.toInt();
	if(!exponent.isNull())
	{
		value*= (kvs_int_t) pow(10.0,exponent.toInt());
	}
	return new KviKvsVariant(value);
}

KviKvsVariant* KviKvsVariant::unserializeString(const QChar** aux)
{
	QString buffer;
	unserializeString(aux,buffer);
	return new KviKvsVariant(buffer);
}

void KviKvsVariant::unserializeString(const QChar** aux,QString& data)
{
	data="";
	QString hex; //temp var
	//skip leading "
	(*aux)++;
	while((*aux)->unicode())
	{
		switch((*aux)->unicode())
		{
		case '"':
			//EOF
			(*aux)++;
			return;
			break;
		case '\\':
			//Special
			(*aux)++;
			switch((*aux)->unicode())
			{
			case 't':
				data.append('\t');
				break;
			case '\"':
				data.append('\"');
				break;
			case '/':
				data.append('/');
				break;
			case 'b':
				data.append('\b');
				break;
			case 'f':
				data.append('\f');
				break;
			case 'n':
				data.append('\n');
				break;
			case 'r':
				data.append('\r');
				break;
			case 'u':
				//4 hexadecmical digits pending...
				hex="";
				(*aux)++;
				for(int k=0;k<4 && (*aux)->unicode(); k++)
				{
					if((*aux)->isDigit() ||
						((*aux)->unicode() >='A' && (*aux)->unicode() <='F')|| //ABCDEF
						((*aux)->unicode() >='a' && (*aux)->unicode() <='f')) //abcdef
					{
						hex.append(**aux);
						(*aux)++;
					} else {
						break;
					}
				}
				(*aux)--;
				data.append(QChar(hex.toUInt(0,16)));
				break;
			default:
				//Fallback; incorrect escape
				(*aux)--;
				data.append('\\');
			}
			(*aux)++;
			break;
		default:
			data.append(**aux);
			(*aux)++;
			break;
		}
	}
}

KviKvsVariant* KviKvsVariant::unserializeHash(const QChar** aux)
{
	KviKvsHash* pHash = new KviKvsHash();
	QString szKey;
	KviKvsVariant* pElement = 0;
	//skip leading '{'
	(*aux)++;
	int i=0;
	while(1)
	{
		//skip leading space
		while((*aux)->isSpace())
				(*aux)++;
		//waiting for starting of string
		if((*aux)->unicode()!='\"')
		{
			//strange characters
			delete pHash;
			return 0;
		}
		unserializeString(aux,szKey);
		if(szKey.isEmpty())
		{
			//Strange element name
			delete pHash;
			return 0;
		}

		//skip leading space before ':'
		while((*aux)->isSpace())
				(*aux)++;
		//waiting for name-value delimeter
		if((*aux)->unicode()!=':')
		{
			//strange characters
			delete pHash;
			return 0;
		}
		(*aux)++;


		//getting element
		pElement = unserialize(aux);
		if(pElement)
		{
			pHash->set(szKey,pElement);
			i++;
			while((*aux)->isSpace())
				(*aux)++;
			switch((*aux)->unicode())
			{
			case ',':
				//goto next
				(*aux)++;
				break;
			case '}':
				//EOF array
				(*aux)++;
				return new KviKvsVariant(pHash);
				break;
			default:
				delete pHash;
				return 0;
				break;
			}
		} else {
			//error
			delete pHash;
			return 0;
		}
	}
	return 0;
}

KviKvsVariant* KviKvsVariant::unserializeArray(const QChar** aux)
{
	KviKvsArray* pArray = new KviKvsArray();
	KviKvsVariant* pElement = 0;
	(*aux)++;
	int i=0;
	while(1)
	{
		pElement = unserialize(aux);
		if(pElement)
		{
			pArray->set(i,pElement);
			i++;
			while((*aux)->isSpace())
				(*aux)++;
			switch((*aux)->unicode())
			{
			case ',':
				//goto next
				(*aux)++;
				break;
			case ']':
				//EOF array
				(*aux)++;
				return new KviKvsVariant(pArray);
				break;
			default:
				delete pArray;
				return 0;
				break;
			}
		} else {
			//error
			delete pArray;
			return 0;
		}
	}
	return 0;
}

KviKvsVariant* KviKvsVariant::unserialize(const QChar** aux)
{
	KviKvsVariant* pResult = 0;

	while((*aux)->isSpace())
		(*aux)++;

	switch((*aux)->unicode())
	{
	case 't':
		//true
		pResult = unserializeTrue(aux);
		break;
	case 'f':
		//false
		pResult = unserializeFalse(aux);
		break;
	case 'n':
		//null
		pResult = unserializeNull(aux);
		break;
	case '[':
		//array
		pResult = unserializeArray(aux);
		break;
	case '{':
		//hash
		pResult = unserializeHash(aux);
		break;
	case '"':
		//string
		pResult = unserializeString(aux);
		break;
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '0':
	case '-':
		//real or integer
		pResult = unserializeRealOrInteger(aux);
		break;
	default:
		//incorrect value
		return 0;
	}

	while((*aux)->isSpace())
		(*aux)++;

	return pResult;
}

KviKvsVariant* KviKvsVariant::unserialize(const QString& data)
{
	KviKvsVariant* pResult = 0;

	const QChar * aux = (const QChar *)data.constData();

	pResult = unserialize(&aux);

	if(aux->unicode())
	{
		//strange extra characters?
		if(pResult) delete pResult;
		pResult = 0;
	}

	return pResult;
}

int KviKvsVariant::compare(const KviKvsVariant * pOther,bool bPreferNumeric) const
{
	// returns -1 if this variant is greater than pOther
	// 0 if they are considered to be equal
	// 1 if the other variant is greater

	if(!pOther)
		return isEqualToNothing() ? CMP_EQUAL : CMP_THISGREATER;
	if(!pOther->m_pData)
		return isEqualToNothing() ? CMP_EQUAL : CMP_THISGREATER;
	if(!m_pData)
		return pOther->isEqualToNothing() ? CMP_EQUAL : CMP_OTHERGREATER;

	switch(m_pData->m_eType)
	{
		case KviKvsVariantData::HObject:
			switch(pOther->m_pData->m_eType)
			{
				case KviKvsVariantData::HObject:
					if(m_pData->m_u.hObject == pOther->m_pData->m_u.hObject)return CMP_EQUAL;
					if(m_pData->m_u.hObject == ((kvs_hobject_t)0))return CMP_OTHERGREATER;
					return CMP_THISGREATER;
				break;
				case KviKvsVariantData::Integer:
					return -1 * KviKvsVariantComparison::compare_integer_hobject(pOther,this);
				break;
				case KviKvsVariantData::Real:
					return -1 * KviKvsVariantComparison::compare_real_hobject(pOther,this);
				break;
				case KviKvsVariantData::String:
					return -1 * KviKvsVariantComparison::compare_string_hobject(pOther,this);
				break;
				case KviKvsVariantData::Boolean:
					return -1 * KviKvsVariantComparison::compare_boolean_hobject(pOther,this);
				break;
				case KviKvsVariantData::Hash:
					return KviKvsVariantComparison::compare_hobject_hash(this,pOther);
				break;
				case KviKvsVariantData::Array:
					return KviKvsVariantComparison::compare_hobject_array(this,pOther);
				break;
				default: // just make gcc happy
				break;
			}
		break;
		case KviKvsVariantData::Integer:
			switch(pOther->m_pData->m_eType)
			{
				case KviKvsVariantData::HObject:
					return KviKvsVariantComparison::compare_integer_hobject(this,pOther);
				break;
				case KviKvsVariantData::Integer:
					if(m_pData->m_u.iInteger == pOther->m_pData->m_u.iInteger)return CMP_EQUAL;
					if(m_pData->m_u.iInteger > pOther->m_pData->m_u.iInteger)return CMP_THISGREATER;
					return CMP_OTHERGREATER;
				break;
				case KviKvsVariantData::Real:
					return KviKvsVariantComparison::compare_integer_real(this,pOther);
				break;
				case KviKvsVariantData::String:
					return KviKvsVariantComparison::compare_integer_string(this,pOther);
				break;
				case KviKvsVariantData::Boolean:
					return KviKvsVariantComparison::compare_integer_boolean(this,pOther);
				break;
				case KviKvsVariantData::Hash:
					return KviKvsVariantComparison::compare_integer_hash(this,pOther);
				break;
				case KviKvsVariantData::Array:
					return KviKvsVariantComparison::compare_integer_array(this,pOther);
				break;
				default: // just make gcc happy
				break;
			}
		break;
		case KviKvsVariantData::Real:
			switch(pOther->m_pData->m_eType)
			{
				case KviKvsVariantData::HObject:
					return KviKvsVariantComparison::compare_real_hobject(this,pOther);
				break;
				case KviKvsVariantData::Integer:
					return -1 * KviKvsVariantComparison::compare_integer_real(pOther,this);
				break;
				case KviKvsVariantData::Real:
					if(*(m_pData->m_u.pReal) == *(pOther->m_pData->m_u.pReal))return CMP_EQUAL;
					if(*(m_pData->m_u.pReal) > *(pOther->m_pData->m_u.pReal))return CMP_THISGREATER;
					return CMP_OTHERGREATER;
				break;
				case KviKvsVariantData::String:
					return KviKvsVariantComparison::compare_real_string(this,pOther);
				break;
				case KviKvsVariantData::Boolean:
					return KviKvsVariantComparison::compare_real_boolean(this,pOther);
				break;
				case KviKvsVariantData::Hash:
					return KviKvsVariantComparison::compare_real_hash(this,pOther);
				break;
				case KviKvsVariantData::Array:
					return KviKvsVariantComparison::compare_real_array(this,pOther);
				break;
				default: // just make gcc happy
				break;
			}
		break;
		case KviKvsVariantData::String:
			switch(pOther->m_pData->m_eType)
			{
				case KviKvsVariantData::String:
					if(bPreferNumeric)
					{
						// prefer numeric comparison
						double dReal1;
						double dReal2;
						if(asReal(dReal1))
						{
							if(pOther->asReal(dReal2))
							{
								if(dReal1 == dReal2)return CMP_EQUAL;
								if(dReal1 > dReal2)return CMP_THISGREATER;
								return CMP_OTHERGREATER;
							}
						}
					}
					return -1 * KviQString::cmpCI(*(m_pData->m_u.pString),*(pOther->m_pData->m_u.pString));
				case KviKvsVariantData::Real:
					return -1 * KviKvsVariantComparison::compare_real_string(pOther,this);
				case KviKvsVariantData::Integer:
					return -1 * KviKvsVariantComparison::compare_integer_string(pOther,this);
				case KviKvsVariantData::Boolean:
					return -1 * KviKvsVariantComparison::compare_boolean_string(pOther,this);
				break;
				case KviKvsVariantData::Hash:
					return KviKvsVariantComparison::compare_string_hash(this,pOther);
				break;
				case KviKvsVariantData::Array:
					return KviKvsVariantComparison::compare_string_array(this,pOther);
				break;
				case KviKvsVariantData::HObject:
					return KviKvsVariantComparison::compare_string_hobject(this,pOther);
				break;
				default:
				break;
			}
		break;
		case KviKvsVariantData::Hash:
			switch(pOther->m_pData->m_eType)
			{
				case KviKvsVariantData::String:
					return -1 * KviKvsVariantComparison::compare_string_hash(pOther,this);
				case KviKvsVariantData::Real:
					return -1 * KviKvsVariantComparison::compare_real_hash(pOther,this);
				break;
				case KviKvsVariantData::Integer:
					return -1 * KviKvsVariantComparison::compare_integer_hash(pOther,this);
				break;
				case KviKvsVariantData::Boolean:
					return -1 * KviKvsVariantComparison::compare_boolean_hash(pOther,this);
				break;
				case KviKvsVariantData::Hash:
					if(m_pData->m_u.pHash->size() > pOther->m_pData->m_u.pHash->size())return CMP_THISGREATER;
					if(m_pData->m_u.pHash->size() == pOther->m_pData->m_u.pHash->size())return CMP_EQUAL;
					return CMP_OTHERGREATER;
				break;
				case KviKvsVariantData::Array:
					return -1 * KviKvsVariantComparison::compare_array_hash(pOther,this);
				break;
				case KviKvsVariantData::HObject:
					return -1 * KviKvsVariantComparison::compare_hobject_hash(pOther,this);
				break;
				default:
				break;
			}
		break;
		case KviKvsVariantData::Array:
			switch(pOther->m_pData->m_eType)
			{
				case KviKvsVariantData::String:
					return -1 * KviKvsVariantComparison::compare_string_array(pOther,this);
				case KviKvsVariantData::Real:
					return -1 * KviKvsVariantComparison::compare_real_array(pOther,this);
				case KviKvsVariantData::Integer:
					return -1 * KviKvsVariantComparison::compare_integer_array(pOther,this);
				case KviKvsVariantData::Boolean:
					return -1 * KviKvsVariantComparison::compare_boolean_array(pOther,this);
				break;
				case KviKvsVariantData::Hash:
					return KviKvsVariantComparison::compare_array_hash(this,pOther);
				break;
				case KviKvsVariantData::Array:
					if(m_pData->m_u.pArray->size() > pOther->m_pData->m_u.pArray->size())return CMP_THISGREATER;
					if(m_pData->m_u.pArray->size() == pOther->m_pData->m_u.pArray->size())return CMP_EQUAL;
					return CMP_OTHERGREATER;
				break;
				case KviKvsVariantData::HObject:
					return -1 * KviKvsVariantComparison::compare_hobject_array(pOther,this);
				break;
				default:
				break;
			}
		break;
		case KviKvsVariantData::Boolean:
			switch(pOther->m_pData->m_eType)
			{
				case KviKvsVariantData::String:
					return KviKvsVariantComparison::compare_boolean_string(this,pOther);
				break;
				case KviKvsVariantData::Real:
					return -1 * KviKvsVariantComparison::compare_real_boolean(pOther,this);
				break;
				case KviKvsVariantData::Integer:
					return -1 * KviKvsVariantComparison::compare_integer_boolean(pOther,this);
				break;
				case KviKvsVariantData::Boolean:
					if(m_pData->m_u.bBoolean == pOther->m_pData->m_u.bBoolean)return CMP_EQUAL;
					if(m_pData->m_u.bBoolean)return CMP_THISGREATER;
					return CMP_OTHERGREATER;
				break;
				case KviKvsVariantData::Hash:
					return KviKvsVariantComparison::compare_boolean_hash(this,pOther);
				break;
				case KviKvsVariantData::Array:
					return KviKvsVariantComparison::compare_boolean_array(this,pOther);
				break;
				case KviKvsVariantData::HObject:
					return KviKvsVariantComparison::compare_boolean_hobject(this,pOther);
				break;
				default:
				break;
			}
		break;
		default: // should never happen anyway
		break;
	}

	return CMP_THISGREATER; // should never happen
}
