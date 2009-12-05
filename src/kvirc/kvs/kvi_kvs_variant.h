#ifndef _KVI_KVS_VARIANT_H_
#define _KVI_KVS_VARIANT_H_
//=============================================================================
//
//   File : kvi_kvs_variant.h
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

#include "kvi_settings.h"
#include "kvi_qstring.h"
#include "kvi_heapobject.h"
#include "kvi_kvs_types.h"


class KviKvsVariant;
class KviKvsArrayCast;

class KVIRC_API KviKvsNumber
{
	friend class KviKvsVariant;
public:
	enum DataType { Real, Integer };
protected:
	union {
		kvs_int_t     iInteger;
		kvs_real_t    dReal;
	} m_u;
	DataType m_type;
public:
	DataType type() const { return m_type; };
	bool isReal() const { return m_type == Real; };
	bool isInteger() const { return m_type == Integer; };
	kvs_real_t real() const { return m_u.dReal; };
	kvs_int_t integer() const { return m_u.iInteger; };
};

// pre-declare.. the real declarations are included below
class KviKvsHash;
class KviKvsArray;

class KviKvsVariantData
{
public:
	enum Type {
		Nothing = 0,  // m_pData not initialized
		String  = 1,  // QString          (scalar)
		Integer = 2,  // long int         (scalar)
		Real    = 4,  // double           (scalar)
		Array   = 8,  // KviKvsArray
		Hash    = 16, // KviKvsHash
		Boolean = 32, // bool             (scalar)
		HObject = 64  // object, shallow! (scalar)
	};
public:
	unsigned int m_uRefs;
	Type m_eType;
	union {
		kvs_int_t       iInteger;
		kvs_real_t    * pReal;
		QString       * pString;
		KviKvsArray   * pArray;
		KviKvsHash    * pHash;
		bool            bBoolean;
		kvs_hobject_t   hObject;
	} m_u;
};

// This class must not have virtual funcitons nor destructor
// Otherwise it will happily crash on windows when it is
// allocated in modules and destroyed anywhere else around...
class KVIRC_API KviKvsVariant : public KviHeapObject
{
	friend class KviKvsVariantComparison;
public:
	KviKvsVariant();
	KviKvsVariant(kvs_int_t iInteger);
	KviKvsVariant(kvs_real_t dReal);
	KviKvsVariant(kvs_real_t * pReal);
	KviKvsVariant(bool bBoolean);
	KviKvsVariant(const QString &szString);
	KviKvsVariant(const char * szString); // without this gcc chooses the conversion (const char *)->(void *) instead of (const char *)->(QString) and obviously calls the wrong constructor
	KviKvsVariant(QString * pString);
	KviKvsVariant(KviKvsArray * pArray);
	KviKvsVariant(KviKvsHash * pHash);
	KviKvsVariant(kvs_hobject_t hObject);
	KviKvsVariant(const KviKvsVariant &v);
	~KviKvsVariant();
protected:
	KviKvsVariantData * m_pData;
public:
	KviKvsVariantData::Type type(){ return m_pData ? m_pData->m_eType : KviKvsVariantData::Nothing; };

	void setReal(kvs_real_t dReal);
	void setReal(kvs_real_t * pReal);
	void setInteger(kvs_int_t iInteger);
	void setString(const QString &szString);
	void setString(QString * pString);
	void setArray(KviKvsArray * pArray);
	void setHash(KviKvsHash * pHash);
	void setBoolean(bool bBoolean);
	void setHObject(kvs_hobject_t hObject);
	void setNothing();
	//void unset(){ setNothing(); };

	void getTypeName(QString &szBuffer) const;

	bool isNothing() const { return !m_pData; };
	bool isInteger() const { return m_pData ? (m_pData->m_eType == KviKvsVariantData::Integer) : false; };
	bool isReal() const { return m_pData ? (m_pData->m_eType == KviKvsVariantData::Real) : false; };
	bool isNumeric() const { return m_pData ? (m_pData->m_eType & (KviKvsVariantData::Integer | KviKvsVariantData::Real)) : false; };
	bool isString() const { return m_pData ? (m_pData->m_eType == KviKvsVariantData::String) : false; };
	bool isScalar() const { return m_pData ? (m_pData->m_eType & (KviKvsVariantData::String | KviKvsVariantData::Integer | KviKvsVariantData::Real)) : false; };
	bool isArray() const { return m_pData ? (m_pData->m_eType == KviKvsVariantData::Array) : false; };
	bool isHash() const { return m_pData ? (m_pData->m_eType == KviKvsVariantData::Hash) : false; };
	bool isBoolean() const { return m_pData ? (m_pData->m_eType == KviKvsVariantData::Boolean) : false; };
	bool isHObject() const { return m_pData ? (m_pData->m_eType == KviKvsVariantData::HObject) : false; };

	bool isEmpty() const;

	// returns true if this variant is nothing, false, null, 0 or an empty string
	bool isEqualToNothing() const;

	// evaluates Integer, Real and String rappresenting an Integer
	bool asInteger(kvs_int_t &iVal) const;
	// evaluates Real, Integer and String rappresenting a Real
	bool asReal(kvs_real_t &dVal) const;
	// evaluates Real, Integer and String rappresenting a Real or integer
	bool asNumber(KviKvsNumber &n) const;
	// always evaluates to a boolean value
	bool asBoolean() const;
	// evaluates to an object handle
	bool asHObject(kvs_hobject_t &hObject) const;
	// evaluates anything to a string
	void asString(QString &szBuffer) const;
	void appendAsString(QString &szBuffer) const;

	// always evaluates to some number...
	void castToNumber(KviKvsNumber &n) const;
	// always evaluates to an integer
	void castToInteger(kvs_int_t &iVal) const;
	// always evaluates to an array
	void castToArray(KviKvsArrayCast *c) const;
	// converts this variant to an array
	void convertToArray();

	kvs_int_t integer() const { return m_pData ? m_pData->m_u.iInteger : 0; };
	kvs_real_t real() const { return m_pData ? *(m_pData->m_u.pReal) : 0.0; };
	const QString & string() const { return m_pData ? *(m_pData->m_u.pString) : KviQString::Empty; };
	KviKvsArray * array() const { return m_pData ? m_pData->m_u.pArray : 0; };
	KviKvsHash * hash() const { return m_pData ? m_pData->m_u.pHash : 0; };
	bool boolean() const { return m_pData ? m_pData->m_u.bBoolean : false; };
	kvs_hobject_t hobject() const { return m_pData ? m_pData->m_u.hObject : (kvs_hobject_t)0; };

	void copyFrom(const KviKvsVariant * v);
	void copyFrom(const KviKvsVariant & v);
	void takeFrom(KviKvsVariant * v);
	void takeFrom(KviKvsVariant &v);

	void dump(const char * prefix) const;

	// returns -1 if this variant is greater than the other, 0 if are equal, 1 if the other is greater
	// if bPreferNumeric is true then when comparing strings a conversion to a numeric format
	// is first attempted.
	int compare(const KviKvsVariant * pOther,bool bPreferNumeric = false) const;

	void operator = (const KviKvsVariant &v){ copyFrom(v); };

	//JSON serialization
	void serialize(QString& result);
	static void serializeString(QString& buffer);
	static KviKvsVariant* unserialize(const QString& buffer);
private:
	static KviKvsVariant* unserialize(const QChar** aux);
	static KviKvsVariant* unserializeTrue(const QChar** aux);
	static KviKvsVariant* unserializeFalse(const QChar** aux);
	static KviKvsVariant* unserializeNull(const QChar** aux);
	static KviKvsVariant* unserializeArray(const QChar** aux);
	static KviKvsVariant* unserializeHash(const QChar** aux);
	static void unserializeString(const QChar** aux,QString& buffer);
	static KviKvsVariant* unserializeString(const QChar** aux);
	static KviKvsVariant* unserializeRealOrInteger(const QChar** aux);
	static KviKvsVariant* unserializeReal(const QChar** aux,QString& data);
	static KviKvsVariant* unserializeInteger(const QChar** aux,QString& data);
};

#include "kvi_kvs_array.h"
#include "kvi_kvs_hash.h"

#endif //!_KVI_KVS_VARIANT_H_
