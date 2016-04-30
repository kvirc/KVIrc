#ifndef _KVI_KVS_VARIANT_H_
#define _KVI_KVS_VARIANT_H_
//=============================================================================
//
//   File : KviKvsVariant.h
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

/**
* \file KviKvsVariant.h
* \author Szymon Stefanek
* \brief Handling of variant data type in KVS
*/

#include "kvi_settings.h"
#include "KviQString.h"
#include "KviHeapObject.h"
#include "KviKvsTypes.h"

class KviKvsVariant;
class KviKvsArrayCast;

// pre-declare.. the real declarations are included below
class KviKvsHash;
class KviKvsArray;

/**
* \class KviKvsNumber
* \brief A class which maps every number
*/
class KVIRC_API KviKvsNumber
{
	friend class KviKvsVariant;

public:
	/**
	* \enum Type
	* \brief Describes the type of number
	*/
	enum Type
	{
		Real,   /**< The number is an integer */
		Integer /**< The number is a double floating point */
	};

protected:
	/**
	* \union DataType
	* \brief This union holds an integer or a double floating point number
	*/
	union DataType {
		kvs_int_t iInt;
		kvs_real_t dReal;
	};

protected:
	Type m_type;
	DataType m_u;

public:
	/**
	* \brief Returns the type of the number
	* \return Type
	*/
	Type type() const { return m_type; };

	/**
	* \brief Returns true if the number is a double floating point
	* \return bool
	*/
	bool isReal() const { return m_type == Real; };

	/**
	* \brief Returns true if the number is an integer
	* \return bool
	*/
	bool isInteger() const { return m_type == Integer; };

	/**
	* \brief Return the real number if the number itself is a double floating point
	* \return kvs_real_t
	*/
	kvs_real_t real() const { return m_u.dReal; };

	/**
	* \brief Return the integer number if the number itself is an integer
	* \return kvs_int_t
	*/
	kvs_int_t integer() const { return m_u.iInt; };
};

/**
* \class KviKvsVariantData
* \brief The class which holds the type of the variant data
*/
class KviKvsVariantData
{
public:
	/**
	* \enum Type
	* \brief Holds the type of the variant data
	*/
	enum Type
	{
		Nothing = 0,  /**< Data not initialized      */
		String = 1,   /**< QString          (scalar) */
		Integer = 2,  /**< long int         (scalar) */
		Real = 4,     /**< double           (scalar) */
		Array = 8,    /**< KviKvsArray               */
		Hash = 16,    /**< KviKvsHash                */
		Boolean = 32, /**< bool             (scalar) */
		HObject = 64  /**< object, shallow! (scalar) */
	};

public:
	/**
	* \union DataType
	* \brief Holds the value of the variant data
	*/
	union DataType {
		kvs_int_t iInt;
		kvs_real_t * pReal;
		QString * pString;
		KviKvsArray * pArray;
		KviKvsHash * pHash;
		bool bBoolean;
		kvs_hobject_t hObject;
	};

public:
	unsigned int m_uRefs;
	Type m_eType;
	DataType m_u;
};

/**
* \class KviKvsVariantComparison
* \brief A class for comparison between two variant data
*/
class KviKvsVariantComparison
{
public:
	/**
	* \enum Result
	* \brief Holds the result of the comparison
	*/
	enum Result
	{
		FirstGreater = -1, /**< the first variant is greater than the second */
		Equal = 0,         /**< the two variants are equal */
		SecondGreater = 1  /**< the second variant is greater than the first */
	};

public:
	/**
	* \brief Compares an integer and a string variant
	* \param pV1 The first variant to compare
	* \param pV2 The second variant to compare
	* \return int
	*/
	static inline int compareIntString(const KviKvsVariant * pV1, const KviKvsVariant * pV2);

	/**
	* \brief Compares an integer and a double floating point variant
	* \param pV1 The first variant to compare
	* \param pV2 The second variant to compare
	* \return int
	*/
	static inline int compareIntReal(const KviKvsVariant * pV1, const KviKvsVariant * pV2);

	/**
	* \brief Compares an integer and a boolean variant
	* \param pV1 The first variant to compare
	* \param pV2 The second variant to compare
	* \return int
	*/
	static inline int compareIntBool(const KviKvsVariant * pV1, const KviKvsVariant * pV2);

	/**
	* \brief Compares an integer and a hash variant
	* \param pV1 The first variant to compare
	* \param pV2 The second variant to compare
	* \return int
	*/
	static inline int compareIntHash(const KviKvsVariant * pV1, const KviKvsVariant * pV2);

	/**
	* \brief Compares an integer and an array variant
	* \param pV1 The first variant to compare
	* \param pV2 The second variant to compare
	* \return int
	*/
	static inline int compareIntArray(const KviKvsVariant * pV1, const KviKvsVariant * pV2);

	/**
	* \brief Compares an integer and an object handle variant
	* \param pV1 The first variant to compare
	* \param pV2 The second variant to compare
	* \return int
	*/
	static inline int compareIntHObject(const KviKvsVariant * pV1, const KviKvsVariant * pV2);

	/**
	* \brief Compares a double floating point and an object handle variant
	* \param pV1 The first variant to compare
	* \param pV2 The second variant to compare
	* \return int
	*/
	static inline int compareRealHObject(const KviKvsVariant * pV1, const KviKvsVariant * pV2);

	/**
	* \brief Compares a double floating point and a string variant
	* \param pV1 The first variant to compare
	* \param pV2 The second variant to compare
	* \return int
	*/
	static inline int compareRealString(const KviKvsVariant * pV1, const KviKvsVariant * pV2);

	/**
	* \brief Compares a double floating point and a boolean variant
	* \param pV1 The first variant to compare
	* \param pV2 The second variant to compare
	* \return int
	*/
	static inline int compareRealBool(const KviKvsVariant * pV1, const KviKvsVariant * pV2);

	/**
	* \brief Compares a double floating point and a hash variant
	* \param pV1 The first variant to compare
	* \param pV2 The second variant to compare
	* \return int
	*/
	static inline int compareRealHash(const KviKvsVariant * pV1, const KviKvsVariant * pV2);

	/**
	* \brief Compares a double floating point and an array variant
	* \param pV1 The first variant to compare
	* \param pV2 The second variant to compare
	* \return int
	*/
	static inline int compareRealArray(const KviKvsVariant * pV1, const KviKvsVariant * pV2);

	/**
	* \brief Compares a string and a hash variant
	* \param pV1 The first variant to compare
	* \param pV2 The second variant to compare
	* \return int
	*/
	static inline int compareStringHash(const KviKvsVariant * pV1, const KviKvsVariant * pV2);

	/**
	* \brief Compares a string and an array variant
	* \param pV1 The first variant to compare
	* \param pV2 The second variant to compare
	* \return int
	*/
	static inline int compareStringArray(const KviKvsVariant * pV1, const KviKvsVariant * pV2);

	/**
	* \brief Compares a string and an object handle variant
	* \param pV1 The first variant to compare
	* \param pV2 The second variant to compare
	* \return int
	*/
	static inline int compareStringHObject(const KviKvsVariant * pV1, const KviKvsVariant * pV2);

	/**
	* \brief Compares a boolean and a string variant
	* \param pV1 The first variant to compare
	* \param pV2 The second variant to compare
	* \return int
	*/
	static inline int compareBoolString(const KviKvsVariant * pV1, const KviKvsVariant * pV2);

	/**
	* \brief Compares a boolean and a hash variant
	* \param pV1 The first variant to compare
	* \param pV2 The second variant to compare
	* \return int
	*/
	static inline int compareBoolHash(const KviKvsVariant * pV1, const KviKvsVariant * pV2);

	/**
	* \brief Compares a boolean and an array variant
	* \param pV1 The first variant to compare
	* \param pV2 The second variant to compare
	* \return int
	*/
	static inline int compareBoolArray(const KviKvsVariant * pV1, const KviKvsVariant * pV2);

	/**
	* \brief Compares a boolean and an object handle variant
	* \param pV1 The first variant to compare
	* \param pV2 The second variant to compare
	* \return int
	*/
	static inline int compareBoolHObject(const KviKvsVariant * pV1, const KviKvsVariant * pV2);

	/**
	* \brief Compares an array and a hash variant
	* \param pV1 The first variant to compare
	* \param pV2 The second variant to compare
	* \return int
	*/
	static inline int compareArrayHash(const KviKvsVariant * pV1, const KviKvsVariant * pV2);

	/**
	* \brief Compares an object handle and a hash variant
	* \param pV1 The first variant to compare
	* \param pV2 The second variant to compare
	* \return int
	*/
	static inline int compareHObjectHash(const KviKvsVariant * pV1, const KviKvsVariant * pV2);

	/**
	* \brief Compares an object handle and an array variant
	* \param pV1 The first variant to compare
	* \param pV2 The second variant to compare
	* \return int
	*/
	static inline int compareHObjectArray(const KviKvsVariant * pV1, const KviKvsVariant * pV2);
};

/**
* \class KviKvsVariant
* \brief This class defines a new data type which contains variant data
*
* A variant data is a data which can assume different data types. This is very useful when you
* don't know in advance which data type you have to manage.
* \warning This class must NOT have virtual functions nor destructor otherwise it will happily
* crash on windows when it is allocated in modules and destroyed anywhere else around...
*/
class KVIRC_API KviKvsVariant : public KviHeapObject
{
	friend class KviKvsVariantComparison;

public:
	/**
	* \brief Constructs the variant data
	* \return KviKvsVariant
	*/
	KviKvsVariant();

	/**
	* \brief Constructs the variant data
	*
	* This is a carbon copy
	* \param variant The variant data to use as a copy
	* \return KviKvsVariant
	*/
	KviKvsVariant(const KviKvsVariant & variant);

	/**
	* \brief Constructs the variant data
	* \param szString The string data to use as variant data
	* \param bEscape Whether to escape the data in KVS
	* \return KviKvsVariant
	*/
	KviKvsVariant(const QString & szString, bool bEscape = false);

	/**
	* \brief Constructs the variant data
	* \param pcString The string data to use as variant data
	* \param bEscape Whether to escape the data in KVS
	* \warning Without this constructor gcc chooses the conversion (const char *)->(void *)
	* instead of (const char *)->(QString) and obviously calls the wrong constructor
	* \return KviKvsVariant
	*/
	KviKvsVariant(const char * pcString, bool bEscape = false);

	/**
	* \brief Constructs the variant data
	* \param pszString The string data to use as variant data
	* \param bEscape Whether to escape the data in KVS
	* \return KviKvsVariant
	*/
	KviKvsVariant(QString * pszString, bool bEscape = false);

	/**
	* \brief Constructs the variant data
	* \param iInteger The integer data to use as variant data
	* \param bEscape Whether to escape the data in KVS
	* \return KviKvsVariant
	*/
	KviKvsVariant(kvs_int_t iInteger, bool bEscape = false);

	/**
	* \brief Constructs the variant data
	* \param dReal The double floating point data to use as variant data
	* \return KviKvsVariant
	*/
	KviKvsVariant(kvs_real_t dReal);

	/**
	* \brief Constructs the variant data
	* \param pReal The double floating point data to use as variant data
	* \return KviKvsVariant
	*/
	KviKvsVariant(kvs_real_t * pReal);

	/**
	* \brief Constructs the variant data
	* \param bBoolean The boolean data to use as variant data
	* \return KviKvsVariant
	*/
	KviKvsVariant(bool bBoolean);

	/**
	* \brief Constructs the variant data
	* \param pArray The array data to use as variant data
	* \return KviKvsVariant
	*/
	KviKvsVariant(KviKvsArray * pArray);

	/**
	* \brief Constructs the variant data
	* \param pHash The hash data to use as variant data
	* \return KviKvsVariant
	*/
	KviKvsVariant(KviKvsHash * pHash);

	/**
	* \brief Constructs the variant data
	* \param hObject The hObject data to use as variant data
	* \return KviKvsVariant
	*/
	KviKvsVariant(kvs_hobject_t hObject);

	/**
	* \brief Destroys the variant data
	*/
	~KviKvsVariant();

protected:
	KviKvsVariantData * m_pData;

public:
	/**
	* \brief Returns the type of the variant data
	* \return KviKvsVariantData::Type
	*/
	KviKvsVariantData::Type type() { return m_pData ? m_pData->m_eType : KviKvsVariantData::Nothing; };

	/**
	* \brief Sets the variant data as double floating point
	* \param dReal The value to set
	* \return void
	*/
	void setReal(kvs_real_t dReal);

	/**
	* \brief Sets the variant data as double floating point
	* \param pReal The value to set
	* \return void
	*/
	void setReal(kvs_real_t * pReal);

	/**
	* \brief Sets the variant data as integer
	* \param iInteger The value to set
	* \return void
	*/
	void setInteger(kvs_int_t iInteger);

	/**
	* \brief Sets the variant data as string
	* \param szString The value to set
	* \return void
	*/
	void setString(const QString & szString);

	/**
	* \brief Sets the variant data as string
	* \param pszString The value to set
	* \return void
	*/
	void setString(QString * pszString);

	/**
	* \brief Sets the variant data as array
	* \param pArray The value to set
	* \return void
	*/
	void setArray(KviKvsArray * pArray);

	/**
	* \brief Sets the variant data as hash
	* \param pHash The value to set
	* \return void
	*/
	void setHash(KviKvsHash * pHash);

	/**
	* \brief Sets the variant data as boolean
	* \param bBoolean The value to set
	* \return void
	*/
	void setBoolean(bool bBoolean);

	/**
	* \brief Sets the variant data as hObject
	* \param hObject The value to set
	* \return void
	*/
	void setHObject(kvs_hobject_t hObject);

	/**
	* \brief Clears the variant data
	* \return void
	*/
	void setNothing();

	/**
	* \brief Gets the type from a variant and returns its name
	* \param szBuffer The buffer to save the type name
	* \return void
	*/
	void getTypeName(QString & szBuffer) const;

	/**
	* \brief Returns true if the variant is empty
	* \return bool
	*/
	bool isNothing() const { return !m_pData; };

	/**
	* \brief Returns true if the variant is an integer
	* \return bool
	*/
	bool isInteger() const { return m_pData ? (m_pData->m_eType == KviKvsVariantData::Integer) : false; };

	/**
	* \brief Returns true if the variant is a double floating point
	* \return bool
	*/
	bool isReal() const { return m_pData ? (m_pData->m_eType == KviKvsVariantData::Real) : false; };

	/**
	* \brief Returns true if the variant is numeric
	* \return bool
	*/
	bool isNumeric() const { return m_pData ? (m_pData->m_eType & (KviKvsVariantData::Integer | KviKvsVariantData::Real)) : false; };

	/**
	* \brief Returns true if the variant is a string
	* \return bool
	*/
	bool isString() const { return m_pData ? (m_pData->m_eType == KviKvsVariantData::String) : false; };

	/**
	* \brief Returns true if the variant is a scalar
	* \return bool
	*/
	bool isScalar() const { return m_pData ? (m_pData->m_eType & (KviKvsVariantData::String | KviKvsVariantData::Integer | KviKvsVariantData::Real)) : false; };

	/**
	* \brief Returns true if the variant is an array
	* \return bool
	*/
	bool isArray() const { return m_pData ? (m_pData->m_eType == KviKvsVariantData::Array) : false; };

	/**
	* \brief Returns true if the variant is an hash
	* \return bool
	*/
	bool isHash() const { return m_pData ? (m_pData->m_eType == KviKvsVariantData::Hash) : false; };

	/**
	* \brief Returns true if the variant is boolean
	* \return bool
	*/
	bool isBoolean() const { return m_pData ? (m_pData->m_eType == KviKvsVariantData::Boolean) : false; };

	/**
	* \brief Returns true if the variant is a hObject
	* \return bool
	*/
	bool isHObject() const { return m_pData ? (m_pData->m_eType == KviKvsVariantData::HObject) : false; };

	/**
	* \brief Returns true if the variant is empty
	* \return bool
	*/
	bool isEmpty() const;

	/**
	* \brief Returns true if the variant is nothing, false, null, 0 or an empty string
	* \return bool
	*/
	bool isEqualToNothing() const;

	/**
	* \brief Eevaluates Integer, Real and String representing an Integer
	* \param iVal The integer buffer to save
	* \return bool
	*/
	bool asInteger(kvs_int_t & iVal) const;

	/**
	* \brief Eevaluates Integer, Real and String representing a Real
	* \param dVal The double floating point buffer to save
	* \return bool
	*/
	bool asReal(kvs_real_t & dVal) const;

	/**
	* \brief Eevaluates Integer, Real and String representing a Real or Integer
	* \param n The number buffer to save
	* \return bool
	*/
	bool asNumber(KviKvsNumber & n) const;

	/**
	* \brief Always evaluates to a boolean value
	* \return bool
	*/
	bool asBoolean() const;

	/**
	* \brief Evaluates to an object handle
	* \param hObject The hObject buffer to save
	* \return bool
	*/
	bool asHObject(kvs_hobject_t & hObject) const;

	/**
	* \brief Evaluates anything to a string
	* \param szBuffer The string buffer to save
	* \return bool
	*/
	void asString(QString & szBuffer) const;

	/**
	* \brief Appends the variant data to a representative string
	* \param szBuffer The string buffer to save
	* \return void
	*/
	void appendAsString(QString & szBuffer) const;

	/**
	* \brief Casts the variant data to a number
	*
	* Always evaluates to some number
	* \param number The string buffer to save
	* \return void
	*/
	void castToNumber(KviKvsNumber & number) const;

	/**
	* \brief Casts the variant data to an integer
	*
	* Always evaluates to an integer
	* \param iVal The integer buffer to save
	* \return void
	*/
	void castToInteger(kvs_int_t & iVal) const;

	/**
	* \brief Casts the variant data to an array
	*
	* Always evaluates to an array
	* \param pCast The array buffer to save
	* \return void
	*/
	void castToArray(KviKvsArrayCast * pCast) const;

	/**
	* \brief Converts this variant to an array
	* \return void
	*/
	void convertToArray();

	/**
	* \brief Returns the integer contained in the variant data
	* \return kvs_int_t
	*/
	kvs_int_t integer() const { return m_pData ? m_pData->m_u.iInt : 0; };

	/**
	* \brief Returns the double floating point contained in the variant data
	* \return kvs_real_t
	*/
	kvs_real_t real() const { return m_pData ? *(m_pData->m_u.pReal) : 0.0; };

	/**
	* \brief Returns the string contained in the variant data
	* \return const QString &
	*/
	const QString & string() const { return m_pData ? *(m_pData->m_u.pString) : KviQString::Empty; };

	/**
	* \brief Returns the boolean contained in the variant data
	* \return bool
	*/
	bool boolean() const { return m_pData ? m_pData->m_u.bBoolean : false; };

	/**
	* \brief Returns the array contained in the variant data
	* \return KviKvsArray
	*/
	KviKvsArray * array() const { return m_pData ? m_pData->m_u.pArray : 0; };

	/**
	* \brief Returns the hash contained in the variant data
	* \return KviKvsHash
	*/
	KviKvsHash * hash() const { return m_pData ? m_pData->m_u.pHash : 0; };

	/**
	* \brief Returns the object handle contained in the variant data
	* \return kvs_hobject_t
	*/
	kvs_hobject_t hobject() const { return m_pData ? m_pData->m_u.hObject : (kvs_hobject_t)0; };

	/**
	* \brief Copies a variant from another
	* \param pVariant The variant to copy from
	* \return void
	*/
	void copyFrom(const KviKvsVariant * pVariant);

	/**
	* \brief Copies a variant from another
	* \param variant The variant to copy from
	* \return void
	*/
	void copyFrom(const KviKvsVariant & variant);

	/**
	* \brief Copies a variant from another and destroys the second one
	* \param pVariant The variant to take from
	* \return void
	*/
	void takeFrom(KviKvsVariant * pVariant);

	/**
	* \brief Copies a variant from another and destroys the second one
	* \param variant The variant to take from
	* \return void
	*/
	void takeFrom(KviKvsVariant & variant);

	/**
	* \brief Dumps the variant data
	* \param pcPrefix The prefix of the data
	* \return void
	*/
	void dump(const char * pcPrefix) const;

	/**
	* \brief Compares two variants
	*
	* Returns -1 if this variant is greater than the other, 0 if they are equal, 1 if the
	* other is greater.
	* If bPreferNumeric is true then a conversion to a numeric format is first attempted
	* \param pOther The second variant to compare
	* \param bPreferNumeric Whether to cast to a numeric format
	* \return int
	*/
	int compare(const KviKvsVariant * pOther, bool bPreferNumeric = false) const;

	// JSON serialization
	/**
	* \brief Serializes the variant data using the JSON format
	* \param szResult The buffer to store the serialization
	* \return void
	*/
	void serialize(QString & szResult);

	/**
	* \brief Replaces escapes chars and serialize the string in the given buffer
	* \param szBuffer The buffer to store the serialization
	* \return void
	*/
	static void serializeString(QString & szBuffer);

	/**
	* \brief Unserializes the variant data using the JSON format
	* \param szBuffer The buffer where is stored the serialization
	* \return KviKvsVariant *
	*/
	static KviKvsVariant * unserialize(const QString & szBuffer);

	/**
	* \brief Allows to create a variant using a carbon copy method
	* \param variant The variant to copy from
	* \return void
	*/
	void operator=(const KviKvsVariant & variant) { copyFrom(variant); };
private:
	/**
	* \brief Unserializes the variant data using the JSON format
	* \param ppAux The pointer where the serialization is located
	* \return KviKvsVariant *
	*/
	static KviKvsVariant * unserialize(const QChar ** ppAux);

	/**
	* \brief Unserializes a boolean into a variant data using the JSON format
	* \param ppAux The pointer where the serialization is located
	* \param bBool Whether the variant is true or false
	* \return KviKvsVariant *
	*/
	static KviKvsVariant * unserializeBool(const QChar ** ppAux, bool bBool);

	/**
	* \brief Unserializes null into a variant data using the JSON format
	* \param ppAux The pointer where the serialization is located
	* \return KviKvsVariant *
	*/
	static KviKvsVariant * unserializeNull(const QChar ** ppAux);

	/**
	* \brief Unserializes an array into a variant data using the JSON format
	* \param ppAux The pointer where the serialization is located
	* \return KviKvsVariant *
	*/
	static KviKvsVariant * unserializeArray(const QChar ** ppAux);

	/**
	* \brief Unserializes a hash into a variant data using the JSON format
	* \param ppAux The pointer where the serialization is located
	* \return KviKvsVariant *
	*/
	static KviKvsVariant * unserializeHash(const QChar ** ppAux);

	/**
	* \brief Unserializes a string into a variant data using the JSON format
	* \param ppAux The pointer where the serialization is located
	* \param szBuffer The buffer to store the unserialization
	* \return void
	*/
	static void unserializeString(const QChar ** ppAux, QString & szBuffer);

	/**
	* \brief Unserializes a string into a variant data using the JSON format
	* \param ppAux The pointer where the serialization is located
	* \return KviKvsVariant *
	*/
	static KviKvsVariant * unserializeString(const QChar ** ppAux);

	/**
	* \brief Unserializes a number into a variant data using the JSON format
	* \param ppAux The pointer where the serialization is located
	* \return KviKvsVariant *
	*/
	static KviKvsVariant * unserializeNumber(const QChar ** ppAux);

	/**
	* \brief Unserializes a double floating point into a variant data using the JSON format
	* \param ppAux The pointer where the serialization is located
	* \param szData The buffer where the serialization is stored
	* \return KviKvsVariant *
	*/
	static KviKvsVariant * unserializeReal(const QChar ** ppAux, QString & szData);

	/**
	* \brief Unserializes an integer into a variant data using the JSON format
	* \param ppAux The pointer where the serialization is located
	* \param szData The buffer where the serialization is stored
	* \return KviKvsVariant *
	*/
	static KviKvsVariant * unserializeInteger(const QChar ** ppAux, QString & szData);
};

#include "KviKvsArray.h"
#include "KviKvsHash.h"

#endif // _KVI_KVS_VARIANT_H_
