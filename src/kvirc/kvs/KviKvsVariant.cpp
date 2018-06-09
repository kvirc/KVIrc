//=============================================================================
//
//   File : KviKvsVariant.cpp
//   Creation date : Tue 07 Oct 2003 04:01:19 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "KviKvsVariant.h"
#include "KviKvsArrayCast.h"
#include "KviKvsHash.h"
#include "KviKvsArray.h"

#include <cmath>
#include <cinttypes>

int KviKvsVariantComparison::compareIntString(const KviKvsVariant * pV1, const KviKvsVariant * pV2)
{
	kvs_real_t dReal;

	if(pV1->m_pData->m_u.iInt == 0)
	{
		if(pV2->m_pData->m_u.pString->isEmpty())
			return KviKvsVariantComparison::Equal;
	}

	if(pV2->asReal(dReal))
	{
		if(((kvs_real_t)pV1->m_pData->m_u.iInt) == dReal)
			return KviKvsVariantComparison::Equal;
		if(((kvs_real_t)pV1->m_pData->m_u.iInt) > dReal)
			return KviKvsVariantComparison::FirstGreater;
		return KviKvsVariantComparison::SecondGreater;
	}

	// compare as strings instead
	QString szString;
	pV1->asString(szString);
	return -1 * szString.compare(*(pV2->m_pData->m_u.pString), Qt::CaseInsensitive);
}

int KviKvsVariantComparison::compareIntReal(const KviKvsVariant * pV1, const KviKvsVariant * pV2)
{
	if(((kvs_real_t)pV1->m_pData->m_u.iInt) == *(pV2->m_pData->m_u.pReal))
		return KviKvsVariantComparison::Equal;
	if(((kvs_real_t)pV1->m_pData->m_u.iInt) > *(pV2->m_pData->m_u.pReal))
		return KviKvsVariantComparison::FirstGreater;
	return KviKvsVariantComparison::SecondGreater;
}

int KviKvsVariantComparison::compareIntBool(const KviKvsVariant * pV1, const KviKvsVariant * pV2)
{
	if(pV1->m_pData->m_u.iInt == 0)
		return pV2->m_pData->m_u.bBoolean ? KviKvsVariantComparison::SecondGreater : KviKvsVariantComparison::Equal;
	return pV2->m_pData->m_u.bBoolean ? KviKvsVariantComparison::Equal : KviKvsVariantComparison::FirstGreater;
}

int KviKvsVariantComparison::compareIntHash(const KviKvsVariant * pV1, const KviKvsVariant * pV2)
{
	if(pV1->m_pData->m_u.iInt == 0)
		return pV2->m_pData->m_u.pHash->isEmpty() ? KviKvsVariantComparison::Equal : KviKvsVariantComparison::SecondGreater;
	return KviKvsVariantComparison::FirstGreater;
}

int KviKvsVariantComparison::compareIntArray(const KviKvsVariant * pV1, const KviKvsVariant * pV2)
{
	if(pV1->m_pData->m_u.iInt == 0)
		return pV2->m_pData->m_u.pArray->isEmpty() ? KviKvsVariantComparison::Equal : KviKvsVariantComparison::SecondGreater;
	return KviKvsVariantComparison::FirstGreater;
}

int KviKvsVariantComparison::compareIntHObject(const KviKvsVariant * pV1, const KviKvsVariant * pV2)
{
	if(pV1->m_pData->m_u.iInt == 0.0)
		return (pV2->m_pData->m_u.hObject == (kvs_hobject_t) nullptr) ? KviKvsVariantComparison::Equal : KviKvsVariantComparison::FirstGreater;
	return KviKvsVariantComparison::SecondGreater;
}

int KviKvsVariantComparison::compareRealHObject(const KviKvsVariant * pV1, const KviKvsVariant * pV2)
{
	if(*(pV1->m_pData->m_u.pReal) == 0.0)
		return (pV2->m_pData->m_u.hObject == (kvs_hobject_t) nullptr) ? KviKvsVariantComparison::Equal : KviKvsVariantComparison::FirstGreater;
	return KviKvsVariantComparison::SecondGreater;
}

int KviKvsVariantComparison::compareRealString(const KviKvsVariant * pV1, const KviKvsVariant * pV2)
{
	kvs_real_t dReal;

	if(*(pV1->m_pData->m_u.pReal) == 0.0)
	{
		if(pV2->m_pData->m_u.pString->isEmpty())
			return KviKvsVariantComparison::Equal;
	}

	if(pV2->asReal(dReal))
	{
		if(*(pV1->m_pData->m_u.pReal) == dReal)
			return KviKvsVariantComparison::Equal;
		if(*(pV1->m_pData->m_u.pReal) > dReal)
			return KviKvsVariantComparison::FirstGreater;
		return KviKvsVariantComparison::SecondGreater;
	}

	// compare as strings instead
	QString szString;
	pV1->asString(szString);
	return -1 * szString.compare(*(pV2->m_pData->m_u.pString), Qt::CaseInsensitive);
}

int KviKvsVariantComparison::compareRealBool(const KviKvsVariant * pV1, const KviKvsVariant * pV2)
{
	if(*(pV1->m_pData->m_u.pReal) == 0.0)
		return pV2->m_pData->m_u.bBoolean ? KviKvsVariantComparison::SecondGreater : KviKvsVariantComparison::Equal;
	return pV2->m_pData->m_u.bBoolean ? KviKvsVariantComparison::Equal : KviKvsVariantComparison::FirstGreater;
}

int KviKvsVariantComparison::compareRealHash(const KviKvsVariant * pV1, const KviKvsVariant * pV2)
{
	if(*(pV1->m_pData->m_u.pReal) == 0)
		return pV2->m_pData->m_u.pHash->isEmpty() ? KviKvsVariantComparison::Equal : KviKvsVariantComparison::SecondGreater;
	return KviKvsVariantComparison::FirstGreater;
}

int KviKvsVariantComparison::compareRealArray(const KviKvsVariant * pV1, const KviKvsVariant * pV2)
{
	if(*(pV1->m_pData->m_u.pReal) == 0)
		return pV2->m_pData->m_u.pArray->isEmpty() ? KviKvsVariantComparison::Equal : KviKvsVariantComparison::SecondGreater;
	return KviKvsVariantComparison::FirstGreater;
}

int KviKvsVariantComparison::compareStringHash(const KviKvsVariant * pV1, const KviKvsVariant * pV2)
{
	if(pV1->m_pData->m_u.pString->isEmpty())
	{
		return pV2->m_pData->m_u.pHash->isEmpty() ? KviKvsVariantComparison::Equal : KviKvsVariantComparison::SecondGreater;
	}
	return KviKvsVariantComparison::FirstGreater;
}

int KviKvsVariantComparison::compareStringArray(const KviKvsVariant * pV1, const KviKvsVariant * pV2)
{
	if(pV1->m_pData->m_u.pString->isEmpty())
	{
		return pV2->m_pData->m_u.pArray->isEmpty() ? KviKvsVariantComparison::Equal : KviKvsVariantComparison::SecondGreater;
	}
	return KviKvsVariantComparison::FirstGreater;
}

int KviKvsVariantComparison::compareStringHObject(const KviKvsVariant * pV1, const KviKvsVariant * pV2)
{
	kvs_real_t dReal;

	if(pV2->m_pData->m_u.hObject == (kvs_hobject_t) nullptr)
	{
		if(pV1->m_pData->m_u.pString->isEmpty())
			return KviKvsVariantComparison::Equal;

		if(pV1->asReal(dReal))
		{
			if(dReal == 0)
				return KviKvsVariantComparison::Equal;
		}
	}

	return KviKvsVariantComparison::FirstGreater;
}

int KviKvsVariantComparison::compareBoolString(const KviKvsVariant * pV1, const KviKvsVariant * pV2)
{
	if(pV2->isEqualToNothing())
		return pV1->m_pData->m_u.bBoolean ? KviKvsVariantComparison::FirstGreater : KviKvsVariantComparison::Equal;
	else
		return pV1->m_pData->m_u.bBoolean ? KviKvsVariantComparison::Equal : KviKvsVariantComparison::FirstGreater;
}

int KviKvsVariantComparison::compareBoolHash(const KviKvsVariant * pV1, const KviKvsVariant * pV2)
{
	if(pV1->m_pData->m_u.bBoolean)
		return pV2->m_pData->m_u.pHash->isEmpty() ? KviKvsVariantComparison::FirstGreater : KviKvsVariantComparison::Equal;
	else
		return pV2->m_pData->m_u.pHash->isEmpty() ? KviKvsVariantComparison::Equal : KviKvsVariantComparison::SecondGreater;
}

int KviKvsVariantComparison::compareBoolArray(const KviKvsVariant * pV1, const KviKvsVariant * pV2)
{
	if(pV1->m_pData->m_u.bBoolean)
		return pV2->m_pData->m_u.pArray->isEmpty() ? KviKvsVariantComparison::FirstGreater : KviKvsVariantComparison::Equal;
	else
		return pV2->m_pData->m_u.pArray->isEmpty() ? KviKvsVariantComparison::Equal : KviKvsVariantComparison::SecondGreater;
}

int KviKvsVariantComparison::compareBoolHObject(const KviKvsVariant * pV1, const KviKvsVariant * pV2)
{
	if(pV1->m_pData->m_u.bBoolean)
		return pV2->m_pData->m_u.hObject == ((kvs_hobject_t) nullptr) ? KviKvsVariantComparison::FirstGreater : KviKvsVariantComparison::Equal;
	else
		return pV2->m_pData->m_u.hObject == ((kvs_hobject_t) nullptr) ? KviKvsVariantComparison::Equal : KviKvsVariantComparison::SecondGreater;
}

int KviKvsVariantComparison::compareArrayHash(const KviKvsVariant * pV1, const KviKvsVariant * pV2)
{
	if(pV1->m_pData->m_u.pArray->size() > pV2->m_pData->m_u.pHash->size())
		return KviKvsVariantComparison::FirstGreater;
	if(pV1->m_pData->m_u.pArray->size() == pV2->m_pData->m_u.pHash->size())
		return KviKvsVariantComparison::Equal;
	return KviKvsVariantComparison::SecondGreater;
}

int KviKvsVariantComparison::compareHObjectHash(const KviKvsVariant * pV1, const KviKvsVariant * pV2)
{
	if(pV2->m_pData->m_u.pHash->isEmpty())
		return pV1->m_pData->m_u.hObject == ((kvs_hobject_t) nullptr) ? KviKvsVariantComparison::Equal : KviKvsVariantComparison::SecondGreater;
	return pV1->m_pData->m_u.hObject == ((kvs_hobject_t) nullptr) ? KviKvsVariantComparison::FirstGreater : KviKvsVariantComparison::Equal;
}

int KviKvsVariantComparison::compareHObjectArray(const KviKvsVariant * pV1, const KviKvsVariant * pV2)
{
	if(pV2->m_pData->m_u.pArray->isEmpty())
		return pV1->m_pData->m_u.hObject == ((kvs_hobject_t) nullptr) ? KviKvsVariantComparison::Equal : KviKvsVariantComparison::SecondGreater;
	return pV1->m_pData->m_u.hObject == ((kvs_hobject_t) nullptr) ? KviKvsVariantComparison::FirstGreater : KviKvsVariantComparison::Equal;
}

KviKvsVariant::KviKvsVariant()
{
	m_pData = nullptr;
}

KviKvsVariant::KviKvsVariant(QString * pString, bool bEscape)
{
	m_pData = new KviKvsVariantData;
	m_pData->m_eType = KviKvsVariantData::String;
	m_pData->m_uRefs = 1;
	m_pData->m_u.pString = pString;
	if(bEscape)
		KviQString::escapeKvs(m_pData->m_u.pString);
}

KviKvsVariant::KviKvsVariant(const QString & szString, bool bEscape)
{
	m_pData = new KviKvsVariantData;
	m_pData->m_eType = KviKvsVariantData::String;
	m_pData->m_uRefs = 1;
	m_pData->m_u.pString = new QString(szString);
	if(bEscape)
		KviQString::escapeKvs(m_pData->m_u.pString);
}

KviKvsVariant::KviKvsVariant(const char * pcString, bool bEscape)
{
	m_pData = new KviKvsVariantData;
	m_pData->m_eType = KviKvsVariantData::String;
	m_pData->m_uRefs = 1;
	m_pData->m_u.pString = new QString(QString::fromUtf8(pcString));
	if(bEscape)
		KviQString::escapeKvs(m_pData->m_u.pString);
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

KviKvsVariant::KviKvsVariant(kvs_int_t iInt, bool)
{
	m_pData = new KviKvsVariantData;
	m_pData->m_eType = KviKvsVariantData::Integer;
	m_pData->m_uRefs = 1;
	m_pData->m_u.iInt = iInt;
}

KviKvsVariant::KviKvsVariant(kvs_hobject_t hObject)
{
	m_pData = new KviKvsVariantData;
	m_pData->m_eType = KviKvsVariantData::HObject;
	m_pData->m_uRefs = 1;
	m_pData->m_u.hObject = hObject;
}

KviKvsVariant::KviKvsVariant(const KviKvsVariant & variant)
{
	m_pData = variant.m_pData;
	if(m_pData)
		m_pData->m_uRefs++;
}

#define DELETE_VARIANT_CONTENTS          \
	switch(m_pData->m_eType)             \
	{                                    \
		case KviKvsVariantData::Array:   \
			delete m_pData->m_u.pArray;  \
			break;                       \
		case KviKvsVariantData::Hash:    \
			delete m_pData->m_u.pHash;   \
			break;                       \
		case KviKvsVariantData::String:  \
			delete m_pData->m_u.pString; \
			break;                       \
		case KviKvsVariantData::Real:    \
			delete m_pData->m_u.pReal;   \
			break;                       \
		default: /* make gcc happy */    \
			break;                       \
	}

#define DETACH_CONTENTS             \
	if(m_pData)                     \
	{                               \
		if(m_pData->m_uRefs <= 1)   \
		{                           \
			DELETE_VARIANT_CONTENTS \
			delete m_pData;         \
		}                           \
		else                        \
		{                           \
			m_pData->m_uRefs--;     \
		}                           \
	}

#define RENEW_VARIANT_DATA                   \
	if(m_pData)                              \
	{                                        \
		if(m_pData->m_uRefs > 1)             \
		{                                    \
			m_pData->m_uRefs--;              \
			m_pData = new KviKvsVariantData; \
			m_pData->m_uRefs = 1;            \
		}                                    \
		else                                 \
		{                                    \
			DELETE_VARIANT_CONTENTS          \
		}                                    \
	}                                        \
	else                                     \
	{                                        \
		m_pData = new KviKvsVariantData;     \
		m_pData->m_uRefs = 1;                \
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

void KviKvsVariant::setString(const QString & szString)
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

void KviKvsVariant::setInteger(kvs_int_t iInt)
{
	RENEW_VARIANT_DATA
	m_pData->m_eType = KviKvsVariantData::Integer;
	m_pData->m_u.iInt = iInt;
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
		}
		else
		{
			// just detach
			m_pData->m_uRefs--;
		}
		m_pData = nullptr;
	}
}

bool KviKvsVariant::isEmpty() const
{
	if(!m_pData)
		return true;
	switch(m_pData->m_eType)
	{
		case KviKvsVariantData::String:
			return m_pData->m_u.pString->isEmpty();
			break;
		case KviKvsVariantData::Array:
			return m_pData->m_u.pArray->isEmpty();
			break;
		case KviKvsVariantData::Hash:
			return m_pData->m_u.pHash->isEmpty();
			break;
		case KviKvsVariantData::HObject:
			return m_pData->m_u.hObject == nullptr;
			break;
		default: /* make gcc happy */
			break;
	}
	return false;
}

bool KviKvsVariant::asBoolean() const
{
	if(!m_pData)
		return false;
	switch(m_pData->m_eType)
	{
		case KviKvsVariantData::Boolean:
			return m_pData->m_u.bBoolean;
			break;
		case KviKvsVariantData::String:
		{
			if(m_pData->m_u.pString->isEmpty())
				return false;

			// check integer or real values
			bool bOk;
			kvs_int_t iVal = (kvs_int_t)KviQString::toI64(*(m_pData->m_u.pString), &bOk);
			if(bOk)
				return iVal;
			kvs_real_t dVal = m_pData->m_u.pString->toDouble(&bOk);
			if(bOk)
				return (dVal != 0.0);
			// non number, non empty
			return true;
		}
		break;
		case KviKvsVariantData::Integer:
			return m_pData->m_u.iInt;
			break;
		case KviKvsVariantData::Real:
			return *(m_pData->m_u.pReal) != 0.0;
			break;
		case KviKvsVariantData::Array:
			return !(m_pData->m_u.pArray->isEmpty());
			break;
		case KviKvsVariantData::Hash:
			return !(m_pData->m_u.pHash->isEmpty());
			break;
		case KviKvsVariantData::HObject:
			return m_pData->m_u.hObject;
			break;
		default: /* make gcc happy */
			break;
	}
	qDebug("WARNING: invalid variant type %d in KviKvsVariant::asBoolean()", m_pData->m_eType);
	return false;
}

bool KviKvsVariant::asHObject(kvs_hobject_t & hObject) const
{
	if(!m_pData)
	{
		// nothing evaluates to a null object
		hObject = nullptr;
		return true;
	}
	switch(m_pData->m_eType)
	{
		case KviKvsVariantData::HObject:
			hObject = m_pData->m_u.hObject;
			return true;
			break;
		case KviKvsVariantData::Integer:
			if(m_pData->m_u.iInt == 0)
			{
				hObject = nullptr;
				return true;
			}
			return false;
			break;
		case KviKvsVariantData::String:
			if(*(m_pData->m_u.pString) == "0")
			{
				hObject = nullptr;
				return true;
			}
			return false;
			break;
		case KviKvsVariantData::Boolean:
			if(!(m_pData->m_u.bBoolean))
			{
				hObject = nullptr;
				return true;
			}
		default: /* make gcc happy */
			break;
	}
	return false;
}

bool KviKvsVariant::asNumber(KviKvsNumber & number) const
{
	if(!m_pData)
		return false;

	if(isInteger())
	{
		number.m_u.iInt = m_pData->m_u.iInt;
		number.m_type = KviKvsNumber::Integer;
		return true;
	}

	if(isReal())
	{
		number.m_u.dReal = *(m_pData->m_u.pReal);
		number.m_type = KviKvsNumber::Real;
		return true;
	}

	if(asInteger(number.m_u.iInt))
	{
		number.m_type = KviKvsNumber::Integer;
		return true;
	}

	if(asReal(number.m_u.dReal))
	{
		number.m_type = KviKvsNumber::Real;
		return true;
	}
	return false;
}

void KviKvsVariant::castToNumber(KviKvsNumber & number) const
{
	if(!m_pData)
	{
		number.m_u.iInt = 0;
		number.m_type = KviKvsNumber::Integer;
		return;
	}

	if(isInteger())
	{
		number.m_u.iInt = m_pData->m_u.iInt;
		number.m_type = KviKvsNumber::Integer;
		return;
	}

	if(isReal())
	{
		number.m_u.dReal = *(m_pData->m_u.pReal);
		number.m_type = KviKvsNumber::Real;
		return;
	}

	if(asInteger(number.m_u.iInt))
	{
		number.m_type = KviKvsNumber::Integer;
		return;
	}

	if(asReal(number.m_u.dReal))
	{
		number.m_type = KviKvsNumber::Real;
		return;
	}
	castToInteger(number.m_u.iInt);
	number.m_type = KviKvsNumber::Integer;
}

void KviKvsVariant::castToArray(KviKvsArrayCast * pCast) const
{
	if(!m_pData)
	{
		pCast->set(new KviKvsArray(), true);
		return;
	}

	switch(m_pData->m_eType)
	{
		case KviKvsVariantData::Array:
			pCast->set(m_pData->m_u.pArray, false);
			break;
		case KviKvsVariantData::Hash:
		{
			KviPointerHashTableIterator<QString, KviKvsVariant> it(*(m_pData->m_u.pHash->dict()));
			KviKvsArray * pArray = new KviKvsArray();
			kvs_int_t idx = 0;
			while(KviKvsVariant * pVariant = it.current())
			{
				pArray->set(idx, new KviKvsVariant(*pVariant));
				++it;
				idx++;
			}
			pCast->set(pArray, true);
		}
		break;
		default:
		{
			// other scalars
			KviKvsArray * pArray = new KviKvsArray();
			pArray->set(0, new KviKvsVariant(*this));
			pCast->set(pArray, true);
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
			KviPointerHashTableIterator<QString, KviKvsVariant> it(*(m_pData->m_u.pHash->dict()));
			KviKvsArray * pArray = new KviKvsArray();
			kvs_int_t idx = 0;
			while(KviKvsVariant * pVariant = it.current())
			{
				pArray->set(idx, new KviKvsVariant(*pVariant));
				++it;
				idx++;
			}
			setArray(pArray);
		}
		break;
		default:
		{
			// other scalars
			KviKvsArray * pArray = new KviKvsArray();
			pArray->set(0, new KviKvsVariant(*this));
			setArray(pArray);
		}
		break;
	}
}

bool KviKvsVariant::asInteger(kvs_int_t & iVal) const
{
	if(!m_pData)
		return false;
	switch(m_pData->m_eType)
	{
		case KviKvsVariantData::Integer:
			iVal = m_pData->m_u.iInt;
			return true;
			break;
		case KviKvsVariantData::String:
		{
			bool bOk;
			iVal = (kvs_int_t)KviQString::toI64(*(m_pData->m_u.pString), &bOk);
			return bOk;
		}
		break;
		case KviKvsVariantData::Real:
			// FIXME: this truncates the value!
			iVal = (kvs_int_t) * (m_pData->m_u.pReal);
			return true;
			break;
		case KviKvsVariantData::Boolean:
			iVal = m_pData->m_u.bBoolean ? 1 : 0;
			return true;
			break;
		default: /* make gcc happy */
			break;
	}
	return false;
}

void KviKvsVariant::castToInteger(kvs_int_t & iVal) const
{
	if(!m_pData)
	{
		iVal = 0;
		return;
	}
	switch(m_pData->m_eType)
	{
		case KviKvsVariantData::Integer:
			iVal = m_pData->m_u.iInt;
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
			iVal = (kvs_int_t)KviQString::toI64(*(m_pData->m_u.pString), &bOk);
			if(bOk)
				return;
			iVal = m_pData->m_u.pString->length();
		}
		break;
		case KviKvsVariantData::Real:
			// FIXME: this truncates the value!
			iVal = (kvs_int_t) * (m_pData->m_u.pReal);
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
	if(!m_pData)
		return false;
	switch(m_pData->m_eType)
	{
		case KviKvsVariantData::Integer:
			dVal = m_pData->m_u.iInt;
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
		default: /* by default we make gcc happy */
			break;
	}
	return false;
}

void KviKvsVariant::asString(QString & szBuffer) const
{
	if(!m_pData)
	{
		szBuffer = QString();
		return;
	}
	switch(m_pData->m_eType)
	{
		case KviKvsVariantData::String:
			szBuffer = *(m_pData->m_u.pString);
			break;
		case KviKvsVariantData::Array:
			szBuffer = QString();
			m_pData->m_u.pArray->appendAsString(szBuffer);
			break;
		case KviKvsVariantData::Hash:
			szBuffer = QString();
			m_pData->m_u.pHash->appendAsString(szBuffer);
			break;
		case KviKvsVariantData::Integer:
			szBuffer.setNum(m_pData->m_u.iInt);
			break;
		case KviKvsVariantData::Real:
			szBuffer.setNum(*(m_pData->m_u.pReal));
			break;
		case KviKvsVariantData::Boolean:
			szBuffer.setNum(m_pData->m_u.bBoolean ? 1 : 0);
			break;
		case KviKvsVariantData::HObject:
			if(m_pData->m_u.hObject)
				szBuffer = QString("object[%1]").arg((uintptr_t)m_pData->m_u.hObject, 0, 16);
			else
				szBuffer = "null-object";
			break;
		default: /* make gcc happy */
			break;
	}
}

void KviKvsVariant::appendAsString(QString & szBuffer) const
{
	if(!m_pData)
		return;
	switch(m_pData->m_eType)
	{
		case KviKvsVariantData::String:
			szBuffer.append(*(m_pData->m_u.pString));
			break;
		case KviKvsVariantData::Array:
			m_pData->m_u.pArray->appendAsString(szBuffer);
			break;
		case KviKvsVariantData::Hash:
			m_pData->m_u.pHash->appendAsString(szBuffer);
			break;
		case KviKvsVariantData::Integer:
			KviQString::appendNumber(szBuffer, m_pData->m_u.iInt);
			break;
		case KviKvsVariantData::Real:
			KviQString::appendNumber(szBuffer, *(m_pData->m_u.pReal));
			break;
		case KviKvsVariantData::Boolean:
			KviQString::appendNumber(szBuffer, m_pData->m_u.bBoolean ? 1 : 0);
			break;
		case KviKvsVariantData::HObject:
			szBuffer.append(m_pData->m_u.hObject ? "object" : "null-object");
			break;
		default: /* make gcc happy */
			break;
	}
}

void KviKvsVariant::dump(const char * pcPrefix) const
{
	if(!m_pData)
	{
		qDebug("%s Nothing [this=0x%" PRIxPTR "]", pcPrefix, (uintptr_t) this);
		return;
	}
	switch(m_pData->m_eType)
	{
		case KviKvsVariantData::String:
			qDebug("%s String(%s) [this=0x%" PRIxPTR "]", pcPrefix, m_pData->m_u.pString->toUtf8().data(), (uintptr_t) this);
			break;
		case KviKvsVariantData::Array:
			qDebug("%s Array(ptr=0x%" PRIxPTR ") [this=0x%" PRIxPTR "]", pcPrefix, (uintptr_t)m_pData->m_u.pArray, (uintptr_t) this);
			break;
		case KviKvsVariantData::Hash:
			qDebug("%s Hash(ptr=0x%" PRIxPTR ",dict=0x%" PRIxPTR ") [this=0x%" PRIxPTR "]", pcPrefix, (uintptr_t)m_pData->m_u.pHash, (uintptr_t)m_pData->m_u.pHash->dict(), (uintptr_t) this);
			break;
		case KviKvsVariantData::Integer:
			qDebug("%s Integer(%d) [this=0x%" PRIxPTR "]", pcPrefix, (int)m_pData->m_u.iInt, (uintptr_t) this);
			break;
		case KviKvsVariantData::Real:
			qDebug("%s Real(%f) [this=0x%" PRIxPTR "]", pcPrefix, *(m_pData->m_u.pReal), (uintptr_t) this);
			break;
		case KviKvsVariantData::Boolean:
			qDebug("%s Boolean(%s) [this=0x%" PRIxPTR "]", pcPrefix, m_pData->m_u.bBoolean ? "true" : "false", (uintptr_t) this);
			break;
		case KviKvsVariantData::HObject:
			qDebug("%s HObject(%" PRIxPTR ") [this=0x%" PRIxPTR "]", pcPrefix, (uintptr_t)m_pData->m_u.hObject, (uintptr_t) this);
			break;
		default: /* make gcc happy */
			break;
	}
}

void KviKvsVariant::copyFrom(const KviKvsVariant * pVariant)
{
	DETACH_CONTENTS
	m_pData = pVariant->m_pData;
	if(m_pData)
		m_pData->m_uRefs++;
}

void KviKvsVariant::copyFrom(const KviKvsVariant & variant)
{
	DETACH_CONTENTS
	m_pData = variant.m_pData;
	if(m_pData)
		m_pData->m_uRefs++;
}

void KviKvsVariant::takeFrom(KviKvsVariant * pVariant)
{
	DETACH_CONTENTS
	m_pData = pVariant->m_pData;
	pVariant->m_pData = nullptr;
}

void KviKvsVariant::takeFrom(KviKvsVariant & variant)
{
	DETACH_CONTENTS
	m_pData = variant.m_pData;
	variant.m_pData = nullptr;
}

void KviKvsVariant::getTypeName(QString & szBuffer) const
{
	if(!m_pData)
	{
		szBuffer = "nothing";
		return;
	}
	switch(m_pData->m_eType)
	{
		case KviKvsVariantData::String:
			szBuffer = "string";
			break;
		case KviKvsVariantData::Hash:
			szBuffer = "hash";
			break;
		case KviKvsVariantData::Array:
			szBuffer = "array";
			break;
		case KviKvsVariantData::Real:
			szBuffer = "real";
			break;
		case KviKvsVariantData::Integer:
			szBuffer = "integer";
			break;
		case KviKvsVariantData::Boolean:
			szBuffer = "boolean";
			break;
		case KviKvsVariantData::HObject:
			szBuffer = "hobject";
			break;
		default:
			szBuffer = "internal_error";
			break;
	}
}

bool KviKvsVariant::isEqualToNothing() const
{
	if(!m_pData)
		return true;
	switch(m_pData->m_eType)
	{
		case KviKvsVariantData::HObject:
			return (m_pData->m_u.hObject == (kvs_hobject_t) nullptr);
			break;
		case KviKvsVariantData::Integer:
			return (m_pData->m_u.iInt == 0);
			break;
		case KviKvsVariantData::Real:
			return (*(m_pData->m_u.pReal) == 0.0);
			break;
		case KviKvsVariantData::String:
		{
			if(m_pData->m_u.pString->isEmpty())
				return true;
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

int KviKvsVariant::compare(const KviKvsVariant * pOther, bool bPreferNumeric) const
{
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
					if(m_pData->m_u.hObject == pOther->m_pData->m_u.hObject)
						return CMP_EQUAL;
					if(m_pData->m_u.hObject == ((kvs_hobject_t) nullptr))
						return CMP_OTHERGREATER;
					return CMP_THISGREATER;
					break;
				case KviKvsVariantData::Integer:
					return -1 * KviKvsVariantComparison::compareIntHObject(pOther, this);
					break;
				case KviKvsVariantData::Real:
					return -1 * KviKvsVariantComparison::compareRealHObject(pOther, this);
					break;
				case KviKvsVariantData::String:
					return -1 * KviKvsVariantComparison::compareStringHObject(pOther, this);
					break;
				case KviKvsVariantData::Boolean:
					return -1 * KviKvsVariantComparison::compareBoolHObject(pOther, this);
					break;
				case KviKvsVariantData::Hash:
					return KviKvsVariantComparison::compareHObjectHash(this, pOther);
					break;
				case KviKvsVariantData::Array:
					return KviKvsVariantComparison::compareHObjectArray(this, pOther);
					break;
				default: // just make gcc happy
					break;
			}
			break;
		case KviKvsVariantData::Integer:
			switch(pOther->m_pData->m_eType)
			{
				case KviKvsVariantData::HObject:
					return KviKvsVariantComparison::compareIntHObject(this, pOther);
					break;
				case KviKvsVariantData::Integer:
					if(m_pData->m_u.iInt == pOther->m_pData->m_u.iInt)
						return CMP_EQUAL;
					if(m_pData->m_u.iInt > pOther->m_pData->m_u.iInt)
						return CMP_THISGREATER;
					return CMP_OTHERGREATER;
					break;
				case KviKvsVariantData::Real:
					return KviKvsVariantComparison::compareIntReal(this, pOther);
					break;
				case KviKvsVariantData::String:
					return KviKvsVariantComparison::compareIntString(this, pOther);
					break;
				case KviKvsVariantData::Boolean:
					return KviKvsVariantComparison::compareIntBool(this, pOther);
					break;
				case KviKvsVariantData::Hash:
					return KviKvsVariantComparison::compareIntHash(this, pOther);
					break;
				case KviKvsVariantData::Array:
					return KviKvsVariantComparison::compareIntArray(this, pOther);
					break;
				default: // just make gcc happy
					break;
			}
			break;
		case KviKvsVariantData::Real:
			switch(pOther->m_pData->m_eType)
			{
				case KviKvsVariantData::HObject:
					return KviKvsVariantComparison::compareRealHObject(this, pOther);
					break;
				case KviKvsVariantData::Integer:
					return -1 * KviKvsVariantComparison::compareIntReal(pOther, this);
					break;
				case KviKvsVariantData::Real:
					if(*(m_pData->m_u.pReal) == *(pOther->m_pData->m_u.pReal))
						return CMP_EQUAL;
					if(*(m_pData->m_u.pReal) > *(pOther->m_pData->m_u.pReal))
						return CMP_THISGREATER;
					return CMP_OTHERGREATER;
					break;
				case KviKvsVariantData::String:
					return KviKvsVariantComparison::compareRealString(this, pOther);
					break;
				case KviKvsVariantData::Boolean:
					return KviKvsVariantComparison::compareRealBool(this, pOther);
					break;
				case KviKvsVariantData::Hash:
					return KviKvsVariantComparison::compareRealHash(this, pOther);
					break;
				case KviKvsVariantData::Array:
					return KviKvsVariantComparison::compareRealArray(this, pOther);
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
								if(dReal1 == dReal2)
									return CMP_EQUAL;
								if(dReal1 > dReal2)
									return CMP_THISGREATER;
								return CMP_OTHERGREATER;
							}
						}
					}
					return -1 * m_pData->m_u.pString->compare(*(pOther->m_pData->m_u.pString), Qt::CaseInsensitive);
				case KviKvsVariantData::Real:
					return -1 * KviKvsVariantComparison::compareRealString(pOther, this);
				case KviKvsVariantData::Integer:
					return -1 * KviKvsVariantComparison::compareIntString(pOther, this);
				case KviKvsVariantData::Boolean:
					return -1 * KviKvsVariantComparison::compareBoolString(pOther, this);
					break;
				case KviKvsVariantData::Hash:
					return KviKvsVariantComparison::compareStringHash(this, pOther);
					break;
				case KviKvsVariantData::Array:
					return KviKvsVariantComparison::compareStringArray(this, pOther);
					break;
				case KviKvsVariantData::HObject:
					return KviKvsVariantComparison::compareStringHObject(this, pOther);
					break;
				default:
					break;
			}
			break;
		case KviKvsVariantData::Hash:
			switch(pOther->m_pData->m_eType)
			{
				case KviKvsVariantData::String:
					return -1 * KviKvsVariantComparison::compareStringHash(pOther, this);
				case KviKvsVariantData::Real:
					return -1 * KviKvsVariantComparison::compareRealHash(pOther, this);
					break;
				case KviKvsVariantData::Integer:
					return -1 * KviKvsVariantComparison::compareIntHash(pOther, this);
					break;
				case KviKvsVariantData::Boolean:
					return -1 * KviKvsVariantComparison::compareBoolHash(pOther, this);
					break;
				case KviKvsVariantData::Hash:
					if(m_pData->m_u.pHash->size() > pOther->m_pData->m_u.pHash->size())
						return CMP_THISGREATER;
					if(m_pData->m_u.pHash->size() == pOther->m_pData->m_u.pHash->size())
						return CMP_EQUAL;
					return CMP_OTHERGREATER;
					break;
				case KviKvsVariantData::Array:
					return -1 * KviKvsVariantComparison::compareArrayHash(pOther, this);
					break;
				case KviKvsVariantData::HObject:
					return -1 * KviKvsVariantComparison::compareHObjectHash(pOther, this);
					break;
				default:
					break;
			}
			break;
		case KviKvsVariantData::Array:
			switch(pOther->m_pData->m_eType)
			{
				case KviKvsVariantData::String:
					return -1 * KviKvsVariantComparison::compareStringArray(pOther, this);
				case KviKvsVariantData::Real:
					return -1 * KviKvsVariantComparison::compareRealArray(pOther, this);
				case KviKvsVariantData::Integer:
					return -1 * KviKvsVariantComparison::compareIntArray(pOther, this);
				case KviKvsVariantData::Boolean:
					return -1 * KviKvsVariantComparison::compareBoolArray(pOther, this);
					break;
				case KviKvsVariantData::Hash:
					return KviKvsVariantComparison::compareArrayHash(this, pOther);
					break;
				case KviKvsVariantData::Array:
					if(m_pData->m_u.pArray->size() > pOther->m_pData->m_u.pArray->size())
						return CMP_THISGREATER;
					if(m_pData->m_u.pArray->size() == pOther->m_pData->m_u.pArray->size())
						return CMP_EQUAL;
					return CMP_OTHERGREATER;
					break;
				case KviKvsVariantData::HObject:
					return -1 * KviKvsVariantComparison::compareHObjectArray(pOther, this);
					break;
				default:
					break;
			}
			break;
		case KviKvsVariantData::Boolean:
			switch(pOther->m_pData->m_eType)
			{
				case KviKvsVariantData::String:
					return KviKvsVariantComparison::compareBoolString(this, pOther);
					break;
				case KviKvsVariantData::Real:
					return -1 * KviKvsVariantComparison::compareRealBool(pOther, this);
					break;
				case KviKvsVariantData::Integer:
					return -1 * KviKvsVariantComparison::compareIntBool(pOther, this);
					break;
				case KviKvsVariantData::Boolean:
					if(m_pData->m_u.bBoolean == pOther->m_pData->m_u.bBoolean)
						return CMP_EQUAL;
					if(m_pData->m_u.bBoolean)
						return CMP_THISGREATER;
					return CMP_OTHERGREATER;
					break;
				case KviKvsVariantData::Hash:
					return KviKvsVariantComparison::compareBoolHash(this, pOther);
					break;
				case KviKvsVariantData::Array:
					return KviKvsVariantComparison::compareBoolArray(this, pOther);
					break;
				case KviKvsVariantData::HObject:
					return KviKvsVariantComparison::compareBoolHObject(this, pOther);
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

void KviKvsVariant::serializeString(QString & szBuffer)
{
	szBuffer.replace('\\', "\\\\");
	szBuffer.replace('\n', "\\n");
	szBuffer.replace('\r', "\\r");
	szBuffer.replace('\b', "\\b");
	szBuffer.replace('\t', "\\t");
	szBuffer.replace('\f', "\\f");
	szBuffer.replace('/', "\\/");
	szBuffer.replace('"', "\\\"");
	szBuffer.prepend('"');
	szBuffer.append('"');
}

void KviKvsVariant::serialize(QString & szResult)
{
	if(!m_pData)
	{
		szResult = "null";
		return;
	}

	switch(m_pData->m_eType)
	{
		case KviKvsVariantData::HObject:
			//can't serialize objects yet
			break;
		case KviKvsVariantData::Integer:
			szResult.setNum(m_pData->m_u.iInt);
			break;
		case KviKvsVariantData::Real:
			szResult.setNum(*(m_pData->m_u.pReal));
			break;
		case KviKvsVariantData::String:
			szResult = *(m_pData->m_u.pString);
			serializeString(szResult);
			break;
		case KviKvsVariantData::Boolean:
			szResult = m_pData->m_u.bBoolean ? "true" : "false";
			break;
		case KviKvsVariantData::Hash:
			m_pData->m_u.pHash->serialize(szResult);
			break;
		case KviKvsVariantData::Array:
			m_pData->m_u.pArray->serialize(szResult);
			break;
		case KviKvsVariantData::Nothing:
			szResult = "null";
			break;
		default: // just make gcc happy
			break;
	}
}

KviKvsVariant * KviKvsVariant::unserialize(const QString & szBuffer)
{
	KviKvsVariant * pResult = nullptr;

	const QChar * pAux = (const QChar *)szBuffer.constData();

	pResult = unserialize(&pAux);

	if(pAux->unicode())
	{
		//strange extra characters?
		if(pResult)
			delete pResult;
		pResult = nullptr;
	}

	return pResult;
}

KviKvsVariant * KviKvsVariant::unserialize(const QChar ** ppAux)
{
	KviKvsVariant * pResult = nullptr;

	while((*ppAux)->isSpace())
		(*ppAux)++;

	switch((*ppAux)->unicode())
	{
		case 't':
			//true
			pResult = unserializeBool(ppAux, true);
			break;
		case 'f':
			//false
			pResult = unserializeBool(ppAux, false);
			break;
		case 'n':
			//null
			pResult = unserializeNull(ppAux);
			break;
		case '[':
			//array
			pResult = unserializeArray(ppAux);
			break;
		case '{':
			//hash
			pResult = unserializeHash(ppAux);
			break;
		case '"':
			//string
			pResult = unserializeString(ppAux);
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '-':
			//real or integer
			pResult = unserializeNumber(ppAux);
			break;
		default:
			//incorrect value
			return nullptr;
	}

	while((*ppAux)->isSpace())
		(*ppAux)++;

	return pResult;
}

KviKvsVariant * KviKvsVariant::unserializeBool(const QChar ** ppAux, bool bBool)
{
	bool bOk = false;

	if(bBool)
	{
		if(KviQString::equalCIN(QString("true"), *ppAux, 4))
		{
			(*ppAux) += 4;
			bOk = true;
		}
	}
	else
	{
		if(KviQString::equalCIN(QString("false"), *ppAux, 5))
		{
			(*ppAux) += 5;
			bOk = true;
		}
	}

	if(bOk)
		return new KviKvsVariant(bBool);
	return nullptr;
}

KviKvsVariant * KviKvsVariant::unserializeNull(const QChar ** ppAux)
{
	if(KviQString::equalCIN(QString("null"), *ppAux, 4))
	{
		(*ppAux) += 4;
		return new KviKvsVariant();
	}
	return nullptr;
}

KviKvsVariant * KviKvsVariant::unserializeNumber(const QChar ** ppAux)
{
	QString szData;

	if((*ppAux)->unicode() == '-')
	{
		szData.append('-');
		(*ppAux)++;
	}

	if(!(*ppAux)->isDigit())
		return nullptr;

	while((*ppAux)->isDigit())
	{
		szData.append(**ppAux);
		(*ppAux)++;
	}

	if((*ppAux)->unicode() == '.')
		return unserializeReal(ppAux, szData);

	return unserializeInteger(ppAux, szData);
}

KviKvsVariant * KviKvsVariant::unserializeReal(const QChar ** ppAux, QString & szData)
{
	QString szExponent;
	(*ppAux)++; //skip .
	szData.append('.');
	while((*ppAux)->isDigit())
	{
		szData.append(**ppAux);
		(*ppAux)++;
	}

	if((*ppAux)->unicode() == 'e' || (*ppAux)->unicode() == 'E')
	{
		(*ppAux)++;
		if((*ppAux)->unicode() == '-')
		{
			szExponent.append('-');
			(*ppAux)++;
		}
		else
		{
			if((*ppAux)->unicode() == '+')
			{
				szExponent.append('+');
				(*ppAux)++;
			}
		}

		while((*ppAux)->isDigit())
		{
			szExponent.append(**ppAux);
			(*ppAux)++;
		}
	}

	float fValue = szData.toFloat();
	if(!szExponent.isNull())
	{
		fValue *= pow(10.0, szExponent.toInt());
	}
	return new KviKvsVariant(fValue);
}

KviKvsVariant * KviKvsVariant::unserializeInteger(const QChar ** ppAux, QString & szData)
{
	QString szExponent;

	if((*ppAux)->unicode() == 'e' || (*ppAux)->unicode() == 'E')
	{
		(*ppAux)++;
		if((*ppAux)->unicode() == '-')
		{
			szExponent.append('-');
			(*ppAux)++;
		}
		else
		{
			if((*ppAux)->unicode() == '+')
			{
				szExponent.append('+');
				(*ppAux)++;
			}
		}

		while((*ppAux)->isDigit())
		{
			szExponent.append(**ppAux);
			(*ppAux)++;
		}
	}

	kvs_int_t iValue = szData.toInt();
	if(!szExponent.isNull())
	{
		iValue *= (kvs_int_t)pow(10.0, szExponent.toInt());
	}
	return new KviKvsVariant(iValue);
}

KviKvsVariant * KviKvsVariant::unserializeString(const QChar ** ppAux)
{
	QString szBuffer;
	unserializeString(ppAux, szBuffer);
	return new KviKvsVariant(szBuffer);
}

void KviKvsVariant::unserializeString(const QChar ** ppAux, QString & szData)
{
	szData = "";
	QString szHex; //temp var
	//skip leading "
	(*ppAux)++;
	while((*ppAux)->unicode())
	{
		switch((*ppAux)->unicode())
		{
			case '"':
				//EOF
				(*ppAux)++;
				return;
				break;
			case '\\':
				//Special
				(*ppAux)++;
				switch((*ppAux)->unicode())
				{
					case 't':
						szData.append('\t');
						break;
					case '\"':
						szData.append('\"');
						break;
					case '/':
						szData.append('/');
						break;
					case 'b':
						szData.append('\b');
						break;
					case 'f':
						szData.append('\f');
						break;
					case 'n':
						szData.append('\n');
						break;
					case 'r':
						szData.append('\r');
						break;
					case 'u':
						//4 hexadecmical digits pending...
						szHex = "";
						(*ppAux)++;
						for(int i = 0; i < 4 && (*ppAux)->unicode(); i++)
						{
							if((*ppAux)->isDigit() || ((*ppAux)->unicode() >= 'A' && (*ppAux)->unicode() <= 'F') || //ABCDEF
							    ((*ppAux)->unicode() >= 'a' && (*ppAux)->unicode() <= 'f'))                         //abcdef
							{
								szHex.append(**ppAux);
								(*ppAux)++;
							}
							else
							{
								break;
							}
						}
						(*ppAux)--;
						szData.append(QChar(szHex.toUInt(nullptr, 16)));
						break;
					default:
						//Fallback; incorrect escape
						(*ppAux)--;
						szData.append('\\');
				}
				(*ppAux)++;
				break;
			default:
				szData.append(**ppAux);
				(*ppAux)++;
				break;
		}
	}
}

KviKvsVariant * KviKvsVariant::unserializeHash(const QChar ** ppAux)
{
	KviKvsHash * pHash = new KviKvsHash();
	QString szKey;
	KviKvsVariant * pElement = nullptr;
	//skip leading '{'
	(*ppAux)++;
	int i = 0;
	while(true)
	{
		//skip leading space
		while((*ppAux)->isSpace())
			(*ppAux)++;
		//waiting for starting of string
		if((*ppAux)->unicode() != '\"')
		{
			//strange characters
			delete pHash;
			return nullptr;
		}
		unserializeString(ppAux, szKey);
		if(szKey.isEmpty())
		{
			//Strange element name
			delete pHash;
			return nullptr;
		}

		//skip leading space before ':'
		while((*ppAux)->isSpace())
			(*ppAux)++;
		//waiting for name-value delimiter
		if((*ppAux)->unicode() != ':')
		{
			//strange characters
			delete pHash;
			return nullptr;
		}
		(*ppAux)++;

		//getting element
		pElement = unserialize(ppAux);
		if(pElement)
		{
			pHash->set(szKey, pElement);
			i++;
			while((*ppAux)->isSpace())
				(*ppAux)++;
			switch((*ppAux)->unicode())
			{
				case ',':
					//goto next
					(*ppAux)++;
					break;
				case '}':
					//EOF array
					(*ppAux)++;
					return new KviKvsVariant(pHash);
					break;
				default:
					delete pHash;
					return nullptr;
					break;
			}
		}
		else
		{
			//error
			delete pHash;
			return nullptr;
		}
	}
	return nullptr;
}

KviKvsVariant * KviKvsVariant::unserializeArray(const QChar ** ppAux)
{
	KviKvsArray * pArray = new KviKvsArray();
	KviKvsVariant * pElement = nullptr;
	(*ppAux)++;
	int i = 0;
	while(true)
	{
		pElement = unserialize(ppAux);
		if(pElement)
		{
			pArray->set(i, pElement);
			i++;
			while((*ppAux)->isSpace())
				(*ppAux)++;
			switch((*ppAux)->unicode())
			{
				case ',':
					//goto next
					(*ppAux)++;
					break;
				case ']':
					//EOF array
					(*ppAux)++;
					return new KviKvsVariant(pArray);
					break;
				default:
					delete pArray;
					return nullptr;
					break;
			}
		}
		else
		{
			//error
			delete pArray;
			return nullptr;
		}
	}
	return nullptr;
}
