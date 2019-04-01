#ifndef _KVI_POINTERHASHTABLE_H_
#define _KVI_POINTERHASHTABLE_H_
//============================================================================
//
//   File : KviPointerHashTable.h
//   Creation date : Sat Jan 12 2008 04:53 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2008 Szymon Stefanek (pragma at kvirc dot net)
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
//============================================================================

/**
* \file KviPointerHashTable.h
* \author Szymon Stefanek
* \brief Pointer Hash Table
*/

#include "kvi_settings.h"
#include "KviPointerList.h"
#include "KviCString.h"
#include "KviQString.h"
#include "KviMemory.h"
#include "kvi_debug.h"

#include <ctype.h>

///
/// Hash functions for various data types
///

/**
* \brief Hash function for the char * data type
*/
inline unsigned int kvi_hash_hash(const char * szKey, bool bCaseSensitive)
{
	unsigned int uResult = 0;
	if(bCaseSensitive)
	{
		while(*szKey)
		{
			uResult += (unsigned char)(*(szKey));
			szKey++;
		}
	}
	else
	{
		while(*szKey)
		{
			uResult += (unsigned char)tolower(*(szKey));
			szKey++;
		}
	}
	return uResult;
}

/**
* \brief Hash key compare function for the char * data type
*/
inline bool kvi_hash_key_equal(const char * szKey1, const char * szKey2, bool bCaseSensitive)
{
	if(bCaseSensitive)
	{
		while(*szKey1 && *szKey2)
		{
			if(*szKey1 != *szKey2)
				return false;
			szKey1++;
			szKey2++;
		}
	}
	else
	{
		while(*szKey1 && *szKey2)
		{
			if(tolower(*szKey1) != tolower(*szKey2))
				return false;
			szKey1++;
			szKey2++;
		}
	}
	//check if one of the strings is a substring of the other (like "perl" and "perlcore")
	if(*szKey1 || *szKey2)
		return false;
	return true;
}

/**
* \brief Hash key copy function for the char * data type
*/
inline void kvi_hash_key_copy(const char * const & szFrom, const char *& szTo, bool bDeepCopy)
{
	if(bDeepCopy)
	{
#if defined(COMPILE_ON_WINDOWS)
		// strlen() returns size_t under windows, which is 32 bits long on a 32 bits
		// system and 64 bits long on a 64 bits one. cast it to avoid a warning
		int len = (int)kvi_strLen(szFrom);
#else
		int len = kvi_strLen(szFrom);
#endif
		char * dst = (char *)KviMemory::allocate(len + 1);
		KviMemory::copy(dst, szFrom, len + 1);
		szTo = dst;
	}
	else
	{
		szTo = szFrom; // we never modify it anyway
	}
}

/**
* \brief Hash key destruction function for the char * data type
*/
inline void kvi_hash_key_destroy(const char *& szKey, bool bDeepCopy)
{
	if(bDeepCopy)
		KviMemory::free((char *)szKey);
}

/**
* \brief Default (empty) hash key for the char * data type
*/
inline const char *& kvi_hash_key_default(const char **)
{
	static const char * static_null = nullptr;
	return static_null;
}

/**
* \brief Hash function for the KviCString data type
*/
inline unsigned int kvi_hash_hash(const KviCString & szKey, bool bCaseSensitive)
{
	unsigned int uResult = 0;
	const char * p = szKey.ptr();
	if(bCaseSensitive)
	{
		while(*p)
		{
			uResult += *((const unsigned char *)p);
			p++;
		}
	}
	else
	{
		while(*p)
		{
			uResult += tolower(*((const unsigned char *)p));
			p++;
		}
	}
	return uResult;
}

/**
* \brief Hash key compare function for the KviCString data type
*/
inline bool kvi_hash_key_equal(const KviCString & szKey1, const KviCString & szKey2)
{
	return kvi_hash_key_equal(szKey1.ptr(), szKey2.ptr());
}

/**
* \brief Hash key copy function for the KviCString data type
*/
inline void kvi_hash_key_copy(const KviCString & szFrom, KviCString & szTo, bool)
{
	szTo = szFrom;
}

/**
* \brief Hash key destruction function for the KviCString data type
*/
inline void kvi_hash_key_destroy(KviCString &, bool)
{
}

/**
* \brief Default (empty) hash key for the KviCString data type
*/
inline const KviCString & kvi_hash_key_default(KviCString *)
{
	return KviCString::emptyString();
}

/**
* \brief Hash function for the int data type
*/
inline unsigned int kvi_hash_hash(const int & iKey, bool)
{
	return (unsigned int)iKey;
}

/**
* \brief Hash key compare function for the int data type
*/
inline bool kvi_hash_key_equal(const int & iKey1, const int & iKey2, bool)
{
	return iKey1 == iKey2;
}

/**
* \brief Hash key copy function for the int data type
*/
inline void kvi_hash_key_copy(const int & iKeyFrom, int & iKeyTo, bool)
{
	iKeyTo = iKeyFrom;
}

/**
* \brief Hash key destruction function for the int data type
*/
inline void kvi_hash_key_destroy(int &, bool)
{
}

/**
* \brief Default (empty) hash key for the int data type
*/
inline const int & kvi_hash_key_default(int *)
{
	static int static_default = 0;
	return static_default;
}

/**
* \brief Hash function for the unsigned short data type
*/
inline unsigned int kvi_hash_hash(const unsigned short & iKey, bool)
{
	return (unsigned int)iKey;
}

/**
* \brief Hash key compare function for the unsigned short data type
*/
inline bool kvi_hash_key_equal(const unsigned short & iKey1, const unsigned short & iKey2, bool)
{
	return iKey1 == iKey2;
}

/**
* \brief Hash key copy function for the unsigned short data type
*/
inline void kvi_hash_key_copy(const unsigned short & iKeyFrom, unsigned short & iKeyTo, bool)
{
	iKeyTo = iKeyFrom;
}

/**
* \brief Hash key destruction function for the unsigned short data type
*/
inline void kvi_hash_key_destroy(unsigned short &, bool)
{
}

/**
* \brief Default (empty) hash key for the unsigned short data type
*/
inline const unsigned short & kvi_hash_key_default(unsigned short *)
{
	static unsigned short static_default = 0;
	return static_default;
}

/**
* \brief Hash function for the void * data type
*/
inline unsigned int kvi_hash_hash(void * pKey, bool)
{
	unsigned char * pBytes = (unsigned char *)&(pKey);
	unsigned char * pEnd = pBytes + sizeof(void *);
	unsigned int uSum = 0;
	while(pBytes < pEnd)
	{
		uSum += *pBytes;
		pBytes++;
	}
	return uSum;
}

/**
* \brief Hash key compare function for the void * data type
*/
inline bool kvi_hash_key_equal(void * pKey1, void * pKey2, bool)
{
	return pKey1 == pKey2;
}

/**
* \brief Hash key copy function for the void * data type
*/
inline void kvi_hash_key_copy(void * const & pKeyFrom, void *& pKeyTo, bool)
{
	pKeyTo = pKeyFrom;
}

/**
* \brief Hash key destruction function for the void * data type
*/
inline void kvi_hash_key_destroy(void *, bool)
{
}

/**
* \brief Default (empty) hash key for the void * data type
*/
inline void *& kvi_hash_key_default(void *)
{
	static void * static_default = nullptr;
	return static_default;
}

/**
* \brief Hash function for the QString data type
*/
inline unsigned int kvi_hash_hash(const QString & szKey, bool bCaseSensitive)
{
	unsigned int uResult = 0;
	const QChar * p = szKey.constData();
	if(!p)
		return 0;
	if(bCaseSensitive)
	{
		while(p->unicode())
		{
			uResult += p->unicode();
			p++;
		}
	}
	else
	{
		while(p->unicode())
		{
			uResult += p->toLower().unicode();
			p++;
		}
	}
	return uResult;
}

/**
* \brief Hash key compare function for the QString data type
*/
inline bool kvi_hash_key_equal(const QString & szKey1, const QString & szKey2, bool bCaseSensitive)
{
	if(bCaseSensitive)
		return KviQString::equalCS(szKey1, szKey2);
	return KviQString::equalCI(szKey1, szKey2);
}

/**
* \brief Hash key copy function for the QString data type
*/
inline void kvi_hash_key_copy(const QString & szFrom, QString & szTo, bool)
{
	szTo = szFrom;
}

/**
* \brief Hash key destruction function for the QString data type
*/
inline void kvi_hash_key_destroy(QString &, bool)
{
}

/**
* \brief Default (empty) hash key for the QString data type
*/
inline const QString & kvi_hash_key_default(QString *)
{
	return KviQString::Empty;
}

template <typename Key, typename T>
class KviPointerHashTable;
template <typename Key, typename T>
class KviPointerHashTableIterator;

template <typename Key, typename T>
class KviPointerHashTableEntry
{
	friend class KviPointerHashTable<Key, T>;

protected:
	T * pData;
	Key hKey;

public:
	Key & key() { return hKey; };
	T * data() { return pData; };
};

/**
* \class KviPointerHashTable
* \brief A fast pointer hash table implementation
*
* A very cool, very fast hash table implementation :P
*
* To use this hash table you need to provide implementations for the
* following functions:
*
* \verbatim
* unsigned int kvi_hash_hash(const Key & hKey, bool bCaseSensitive);
* bool kvi_hash_key_equal(const Key & hKey1, const Key & hKey2, bool
* bCaseSensitive);
* void kvi_hash_key_copy(const Key & hKeyFrom, Key & hKeyTo, bool bDeepCopy);
* void kvi_hash_key_destroy(Key & hKey, bool bIsDeepCopy);
* const Key & kvi_hash_key_default(Key *);
* \endverbatim
*
* Implementations for the most likey Key data types are provided below.
* KviPointerHashTable will automagically work with const char, QString,
* KviCString and integer types as keys.
*
* For string Key types, the hash table may or may not be case sensitive.
* For other Key types the case sensitive flag has no meaning and will
* (hopefully) be optimized out by the compiler.
*
* For pointer based keys the hash table may or may not maintain deep copies of
* Key data. For example, with char * keys, if deep copying is enabled then a
* private copy of the string data will be maintained. With deep copying
* disabled only char * pointers will be kept. For types that do not have
* meaning of deep copy the deep copying code will (hopefully) be optimized
* out by the compiler.
*
* The hashtable maintains an array of KviPointerList based buckets.
* The number of buckets may be specified by the application user and does NOT
* need to be a prime number. Yet better to have it a power of two so the
* memory allocation routines will feel better and are less likely to waste
* space.
*/
template <class Key, class T>
class KviPointerHashTable
{
	friend class KviPointerHashTableIterator<Key, T>;

protected:
	KviPointerList<KviPointerHashTableEntry<Key, T>> ** m_pDataArray;
	bool m_bAutoDelete;
	unsigned int m_uSize;
	unsigned int m_uCount;
	bool m_bCaseSensitive;
	bool m_bDeepCopyKeys;
	unsigned int m_uIteratorIdx = 0;

public:
	/**
	* \brief Returns the item associated to the key
	*
	* Returns nullptr if no such item exists in the hash table.
	* Places the hash table iterator at the position of the item found.
	* \param hKey The key to find
	* \return T *
	*/
	T * find(const Key & hKey)
	{
		m_uIteratorIdx = kvi_hash_hash(hKey, m_bCaseSensitive) % m_uSize;
		if(!m_pDataArray[m_uIteratorIdx])
			return nullptr;
		for(KviPointerHashTableEntry<Key, T> * e = m_pDataArray[m_uIteratorIdx]->first(); e; e = m_pDataArray[m_uIteratorIdx]->next())
		{
			if(kvi_hash_key_equal(e->hKey, hKey, m_bCaseSensitive))
				return (T *)e->pData;
		}
		return nullptr;
	}

	/**
	* \brief Returns the item associated to the key hKey
	*
	* Returns nullptr if no such item exists in the hash table.
	* Places the hash table iterator at the position of the item found.
	* This is an alias to find().
	* \param hKey The key to find
	* \return T *
	*/
	T * operator[](const Key & hKey)
	{
		return find(hKey);
	}

	/**
	* \brief Returns the number of items in this hash table
	* \return unsigned int
	*/
	unsigned int count() const
	{
		return m_uCount;
	}

	/**
	* \brief Returns true if the hash table is empty
	* \return bool
	*/
	bool isEmpty() const
	{
		return m_uCount == 0;
	}

	/**
	* \brief Inserts the item pData at the position specified by the key hKey.
	*
	* Replaces any previous item with the same key
	* The replaced item is deleted if autodelete is enabled.
	* The hash table iterator is placed at the newly inserted item.
	* \param hKey The key where to insert data
	* \param pData The data to insert
	* \return void
	*/
	void insert(const Key & hKey, T * pData)
	{
		if(!pData)
			return;
		unsigned int uEntry = kvi_hash_hash(hKey, m_bCaseSensitive) % m_uSize;
		if(!m_pDataArray[uEntry])
			m_pDataArray[uEntry] = new KviPointerList<KviPointerHashTableEntry<Key, T>>(true);
		for(KviPointerHashTableEntry<Key, T> * e = m_pDataArray[uEntry]->first(); e; e = m_pDataArray[uEntry]->next())
		{
			if(kvi_hash_key_equal(e->hKey, hKey, m_bCaseSensitive))
			{
				if(!m_bCaseSensitive)
				{
					// must change the key too
					kvi_hash_key_destroy(e->hKey, m_bDeepCopyKeys);
					kvi_hash_key_copy(hKey, e->hKey, m_bDeepCopyKeys);
				}
				if(m_bAutoDelete)
					delete e->pData;
				e->pData = pData;
				return;
			}
		}
		KviPointerHashTableEntry<Key, T> * n = new KviPointerHashTableEntry<Key, T>;
		kvi_hash_key_copy(hKey, n->hKey, m_bDeepCopyKeys);
		n->pData = pData;
		m_pDataArray[uEntry]->append(n);
		m_uCount++;
	}

	/**
	* \brief Inserts the item pData at the position specified by the key hKey.
	*
	* Replaces any previous item with the same key.
	* The replaced item is deleted if autodelete is enabled.
	* The hash table iterator is placed at the newly inserted item.
	* This is just an alias to insert() with a different name.
	* \param hKey The key where to insert data
	* \param pData The new data to insert
	* \return void
	*/
	void replace(const Key & hKey, T * pData)
	{
		insert(hKey, pData);
	}

	/**
	* \brief Removes the item pointer associated to the key hKey, if such an item exists in the hash table.
	*
	* The item is deleted if autodeletion is enabled. Returns true if the
	* item was found and removed and false if it wasn't found.
	* Invalidates the hash table iterator.
	* \param hKey The key where to remove the pointer
	* \return bool
	*/
	bool remove(const Key & hKey)
	{
		unsigned int uEntry = kvi_hash_hash(hKey, m_bCaseSensitive) % m_uSize;
		if(!m_pDataArray[uEntry])
			return false;
		for(KviPointerHashTableEntry<Key, T> * e = m_pDataArray[uEntry]->first(); e; e = m_pDataArray[uEntry]->next())
		{
			if(kvi_hash_key_equal(e->hKey, hKey, m_bCaseSensitive))
			{
				kvi_hash_key_destroy(e->hKey, m_bDeepCopyKeys);
				if(m_bAutoDelete)
					delete((T *)(e->pData));
				m_pDataArray[uEntry]->removeRef(e);
				if(m_pDataArray[uEntry]->isEmpty())
				{
					delete m_pDataArray[uEntry];
					m_pDataArray[uEntry] = nullptr;
				}
				m_uCount--;
				return true;
			}
		}
		return false;
	}

	/**
	* \brief Removes the first occurrence of the item pointer pRef.
	*
	* The item is deleted if autodeletion is enabled. Returns true if the
	* pointer was found and false otherwise.
	* Invalidates the hash table iterator.
	* \param pRef The pointer to remove the first occurrence
	* \return bool
	*/
	bool removeRef(const T * pRef)
	{
		for(unsigned int i = 0; i < m_uSize; i++)
		{
			if(m_pDataArray[i])
			{
				for(KviPointerHashTableEntry<Key, T> * e = m_pDataArray[i]->first(); e; e = m_pDataArray[i]->next())
				{
					if(e->pData == pRef)
					{
						kvi_hash_key_destroy(e->hKey, m_bDeepCopyKeys);
						if(m_bAutoDelete)
							delete((T *)(e->pData));
						m_pDataArray[i]->removeRef(e);
						if(m_pDataArray[i]->isEmpty())
						{
							delete m_pDataArray[i];
							m_pDataArray[i] = nullptr;
						}
						m_uCount--;
						return true;
					}
				}
			}
		}
		return false;
	}

	/**
	* \brief Removes all the items from the hash table.
	*
	* The items are deleted if autodeletion is enabled.
	* Invalidates the hash table iterator.
	* \return void
	*/
	void clear()
	{
		for(unsigned int i = 0; i < m_uSize; i++)
		{
			if(!m_pDataArray[i])
				continue;

			while(KviPointerHashTableEntry<Key, T> * e = m_pDataArray[i]->takeFirst())
			{
				kvi_hash_key_destroy(e->hKey, m_bDeepCopyKeys);

				if(m_bAutoDelete)
					delete((T *)(e->pData));

				delete e;

				if(!m_pDataArray[i])
					break; // emptied in the meantime
			}

			if(m_pDataArray[i])
			{
				delete m_pDataArray[i];
				m_pDataArray[i] = nullptr;
			}
		}
		m_uCount = 0;
	}

	/**
	* \brief Searches for the item pointer pRef
	*
	* Returns its hash table entry, if found, and nullptr otherwise.
	* The hash table iterator is placed at the item found.
	* \param pRef The pointer to search
	* \return KviPointerHashTableEntry<Key,T> *
	*/
	KviPointerHashTableEntry<Key, T> * findRef(const T * pRef)
	{
		for(m_uIteratorIdx = 0; m_uIteratorIdx < m_uSize; m_uIteratorIdx++)
		{
			if(m_pDataArray[m_uIteratorIdx])
			{
				for(KviPointerHashTableEntry<Key, T> * e = m_pDataArray[m_uIteratorIdx]->first(); e; e = m_pDataArray[m_uIteratorIdx]->next())
				{
					if(e->pData == pRef)
						return e;
				}
			}
		}
		return nullptr;
	}

	/**
	* \brief Returns the entry pointed by the hash table iterator.
	*
	* This function must be preceded by a call to firstEntry(), first()
	* or findRef().
	* \return KviPointerHashTableEntry<Key,T> *
	*/
	KviPointerHashTableEntry<Key, T> * currentEntry()
	{
		if(m_uIteratorIdx >= m_uSize)
			return nullptr;
		if(m_pDataArray[m_uIteratorIdx])
			return m_pDataArray[m_uIteratorIdx]->current();
		return nullptr;
	}

	/**
	* \brief Places the hash table iterator at the first entry and returns it.
	* \return KviPointerHashTableEntry<Key,T> *
	*/
	KviPointerHashTableEntry<Key, T> * firstEntry()
	{
		m_uIteratorIdx = 0;
		while(m_uIteratorIdx < m_uSize && (!m_pDataArray[m_uIteratorIdx]))
		{
			m_uIteratorIdx++;
		}
		if(m_uIteratorIdx == m_uSize)
			return nullptr;
		return m_pDataArray[m_uIteratorIdx]->first();
	}

	/**
	* \brief Places the hash table iterator at the next entry and returns it.
	*
	* This function must be preceded by a call to firstEntry(), first()
	* or findRef().
	* \return KviPointerHashTableEntry<Key,T> *
	*/
	KviPointerHashTableEntry<Key, T> * nextEntry()
	{
		if(m_uIteratorIdx >= m_uSize)
			return nullptr;

		if(m_uIteratorIdx < m_uSize)
		{
			KviPointerHashTableEntry<Key, T> * t = m_pDataArray[m_uIteratorIdx]->next();
			if(t)
				return t;
		}

		m_uIteratorIdx++;

		while(m_uIteratorIdx < m_uSize && (!m_pDataArray[m_uIteratorIdx]))
		{
			m_uIteratorIdx++;
		}

		if(m_uIteratorIdx == m_uSize)
			return nullptr;

		return m_pDataArray[m_uIteratorIdx]->first();
	}

	/**
	* \brief Returns the data value pointer pointed by the hash table iterator.
	*
	* This function must be preceded by a call to firstEntry(), first()
	* or findRef().
	* \return T *
	*/
	T * current()
	{
		if(m_uIteratorIdx >= m_uSize)
			return nullptr;
		if(m_pDataArray[m_uIteratorIdx])
		{
			KviPointerHashTableEntry<Key, T> * e = m_pDataArray[m_uIteratorIdx]->current();
			if(!e)
				return nullptr;
			return e->data();
		}
		return nullptr;
	}

	/**
	* \brief Returns the key pointed by the hash table iterator.
	*
	* This function must be preceded by a call to firstEntry(), first()
	* or findRef().
	* \return const Key &
	*/
	const Key & currentKey()
	{
		if(m_uIteratorIdx >= m_uSize)
			return kvi_hash_key_default(((Key *)nullptr));
		if(m_pDataArray[m_uIteratorIdx])
		{
			KviPointerHashTableEntry<Key, T> * e = m_pDataArray[m_uIteratorIdx]->current();
			if(!e)
				return kvi_hash_key_default(((Key *)nullptr));
			return e->key();
		}
		return kvi_hash_key_default(((Key *)nullptr));
	}

	/** \brief Places the hash table iterator at the first entry
	*
	* It returns the associated data value pointer.
	* \return T *
	*/
	T * first()
	{
		m_uIteratorIdx = 0;
		while(m_uIteratorIdx < m_uSize && (!m_pDataArray[m_uIteratorIdx]))
		{
			m_uIteratorIdx++;
		}
		if(m_uIteratorIdx == m_uSize)
			return nullptr;
		KviPointerHashTableEntry<Key, T> * e = m_pDataArray[m_uIteratorIdx]->first();
		if(!e)
			return nullptr;
		return e->data();
	}

	/**
	* \brief Places the hash table iterator at the next entry and returns the associated data value pointer.
	*
	* This function must be preceded by a call to firstEntry(), first()
	* or findRef().
	* \return T *
	*/
	T * next()
	{
		if(m_uIteratorIdx >= m_uSize)
			return nullptr;

		if(m_uIteratorIdx < m_uSize)
		{
			KviPointerHashTableEntry<Key, T> * t = m_pDataArray[m_uIteratorIdx]->next();
			if(t)
			{
				return t->data();
			}
		}

		m_uIteratorIdx++;

		while(m_uIteratorIdx < m_uSize && (!m_pDataArray[m_uIteratorIdx]))
		{
			m_uIteratorIdx++;
		}

		if(m_uIteratorIdx == m_uSize)
			return nullptr;

		KviPointerHashTableEntry<Key, T> * e = m_pDataArray[m_uIteratorIdx]->first();
		if(!e)
			return nullptr;
		return e->data();
	}

	/**
	* \brief Removes all items in the hash table and then makes a complete shallow copy of the data contained in t.
	*
	* The removed items are deleted if autodeletion is enabled.
	* The hash table iterator is invalidated.
	* Does not change autodelete flag: make sure you don't delete the
	* items twice :)
	* \param t The data to copy
	* \return void
	*/
	void copyFrom(KviPointerHashTable<Key, T> & t)
	{
		clear();
		for(KviPointerHashTableEntry<Key, T> * e = t.firstEntry(); e; e = t.nextEntry())
			insert(e->key(), e->data());
	}

	/**
	* \brief Inserts a complete shallow copy of the data contained in t.
	*
	* The hash table iterator is invalidated.
	* \param t The data to insert
	* \return void
	*/
	void insert(KviPointerHashTable<Key, T> & t)
	{
		for(KviPointerHashTableEntry<Key, T> * e = t.firstEntry(); e; e = t.nextEntry())
			insert(e->key(), e->data());
	}

	/**
	* \brief Enables or disabled the autodeletion feature.
	*
	* Items are deleted upon removal when the feature is enabled.
	* \param bAutoDelete Set the autodelete state
	* \return void
	*/
	void setAutoDelete(bool bAutoDelete)
	{
		m_bAutoDelete = bAutoDelete;
	}

	/**
	* \brief Creates an empty hash table.
	*
	* Automatic deletion is enabled.
	* \param uSize The number of hash buckets: does NOT necesairly need to be prime
	* \param bCaseSensitive Are the key comparisons case sensitive ?
	* \param bDeepCopyKeys Do we need to maintain deep copies of keys ?
	* \return KviPointerHashTable
	*/
	KviPointerHashTable(unsigned int uSize = 32, bool bCaseSensitive = true, bool bDeepCopyKeys = true)
	{
		m_uCount = 0;
		m_bCaseSensitive = bCaseSensitive;
		m_bAutoDelete = true;
		m_bDeepCopyKeys = bDeepCopyKeys;
		m_uSize = uSize > 0 ? uSize : 32;
		m_pDataArray = new KviPointerList<KviPointerHashTableEntry<Key, T>> *[m_uSize];
		for(unsigned int i = 0; i < m_uSize; i++)
			m_pDataArray[i] = nullptr;
	}

	/**
	* \brief First creates an empty hash table and then inserts a copy of all the item pointers present in t.
	*
	* The autodelete feature is automatically disabled (take care!).
	* \param t The data to copy
	* \return KviPointerHashTable
	*/
	KviPointerHashTable(KviPointerHashTable<Key, T> & t)
	{
		m_uCount = 0;
		m_bAutoDelete = false;
		m_bCaseSensitive = t.m_bCaseSensitive;
		m_bDeepCopyKeys = t.m_bDeepCopyKeys;
		m_uSize = t.m_uSize;
		m_pDataArray = new KviPointerList<KviPointerHashTableEntry<Key, T>> *[m_uSize];
		for(unsigned int i = 0; i < m_uSize; i++)
			m_pDataArray[i] = nullptr;
		copyFrom(t);
	}

	/**
	* \brief Destroys the hash table and all the items contained within.
	*
	* Items are deleted if autodeletion is enabled.
	*/
	~KviPointerHashTable()
	{
		clear();
		delete[] m_pDataArray;
	}
};

/**
* \class KviPointerHashTableIterator
* \brief A fast pointer hash table iterator implementation
*/
template <typename Key, typename T>
class KviPointerHashTableIterator
{
protected:
	const KviPointerHashTable<Key, T> * m_pHashTable;
	unsigned int m_uEntryIndex;
	KviPointerListIterator<KviPointerHashTableEntry<Key, T>> * m_pIterator;

public:
	/**
	* \brief Creates an iterator copy.
	*
	* The new iterator points exactly to the item pointed by src.
	* \param src The source pointer to copy from
	* \return void
	*/
	void operator=(const KviPointerHashTableIterator<Key, T> & src)
	{
		m_pHashTable = src.m_pHashTable;
		m_uEntryIndex = src.m_uEntryIndex;
		if(src.m_pIterator)
			m_pIterator = new KviPointerListIterator<KviPointerHashTableEntry<Key, T>>(*(src.m_pIterator));
		else
			m_pIterator = nullptr;
	}

	/**
	* \brief Moves the iterator to the first element of the hash table.
	*
	* Returns true in case of success or false if the hash table is empty.
	* \return bool
	*/
	bool moveFirst()
	{
		if(m_pIterator)
		{
			delete m_pIterator;
			m_pIterator = nullptr;
		}

		m_uEntryIndex = 0;
		while((m_uEntryIndex < m_pHashTable->m_uSize) && (!(m_pHashTable->m_pDataArray[m_uEntryIndex])))
		{
			m_uEntryIndex++;
		}

		if(m_uEntryIndex == m_pHashTable->m_uSize)
			return false;

		m_pIterator = new KviPointerListIterator<KviPointerHashTableEntry<Key, T>>(*(m_pHashTable->m_pDataArray[m_uEntryIndex]));
		bool bRet = m_pIterator->moveFirst();
		if(!bRet)
		{
			delete m_pIterator;
			m_pIterator = nullptr;
		}
		return bRet;
	}

	/**
	* \brief Moves the iterator to the last element of the hash table.
	*
	* Returns true in case of success or false if the hash table is empty.
	* \return bool
	*/
	bool moveLast()
	{
		if(m_pIterator)
		{
			delete m_pIterator;
			m_pIterator = nullptr;
		}

		m_uEntryIndex = m_pHashTable->m_uSize;
		while(m_uEntryIndex > 0)
		{
			m_uEntryIndex--;
			if(m_pHashTable->m_pDataArray[m_uEntryIndex])
			{
				m_pIterator = new KviPointerListIterator<KviPointerHashTableEntry<Key, T>>(*(m_pHashTable->m_pDataArray[m_uEntryIndex]));
				bool bRet = m_pIterator->moveLast();
				if(!bRet)
				{
					delete m_pIterator;
					m_pIterator = nullptr;
				}
				return bRet;
			}
		}
		return false;
	}

	/**
	* \brief Moves the iterator to the next element of the hash table.
	*
	* The iterator must be actually valid for this function to work.
	* Returns true in case of success or false if there is no next item.
	* \return bool
	*/
	bool moveNext()
	{
		if(!m_pIterator)
			return false;
		if(m_pIterator->moveNext())
			return true;
		if(m_pIterator)
		{
			delete m_pIterator;
			m_pIterator = nullptr;
		}
		m_uEntryIndex++;
		while((m_uEntryIndex < m_pHashTable->m_uSize) && (!(m_pHashTable->m_pDataArray[m_uEntryIndex])))
		{
			m_uEntryIndex++;
		}
		if(m_uEntryIndex == m_pHashTable->m_uSize)
			return false;
		m_pIterator = new KviPointerListIterator<KviPointerHashTableEntry<Key, T>>(*(m_pHashTable->m_pDataArray[m_uEntryIndex]));
		bool bRet = m_pIterator->moveFirst();
		if(!bRet)
		{
			delete m_pIterator;
			m_pIterator = nullptr;
		}
		return bRet;
	}

	/**
	* \brief Moves the iterator to the next element of the hash table.
	*
	* The iterator must be actually valid for this function to work.
	* Returns true in case of success or false if there is no next item.
	* This is just an alias to moveNext().
	* \return bool
	*/
	bool operator++()
	{
		return moveNext();
	}

	/**
	* \brief Moves the iterator to the previous element of the hash table.
	*
	* The iterator must be actually valid for this function to work.
	* Returns true in case of success or false if there is no previous
	* item.
	* \return bool
	*/
	bool movePrev()
	{
		if(!m_pIterator)
			return false;
		if(m_pIterator->movePrev())
			return true;
		if(m_pIterator)
		{
			delete m_pIterator;
			m_pIterator = nullptr;
		}
		if(m_uEntryIndex >= m_pHashTable->m_uSize)
			return false;
		while(m_uEntryIndex > 0)
		{
			m_uEntryIndex--;
			if(m_pHashTable->m_pDataArray[m_uEntryIndex])
			{
				m_pIterator = new KviPointerListIterator<KviPointerHashTableEntry<Key, T>>(*(m_pHashTable->m_pDataArray[m_uEntryIndex]));
				bool bRet = m_pIterator->moveLast();
				if(!bRet)
				{
					delete m_pIterator;
					m_pIterator = nullptr;
				}
				return bRet;
			}
		}
		return false;
	}

	/**
	* \brief Moves the iterator to the previous element of the hash table.
	*
	* The iterator must be actually valid for this function to work.
	* Returns true in case of success or false if there is no previous
	* item.
	* This is just an alias to movePrev() with a different name.
	* \return bool
	*/
	bool operator--()
	{
		return movePrev();
	}

	/**
	* \brief Returns the value pointed by the iterator
	*
	* If the iterator is not valid it returns a default constructed value
	* This is an alias to operator *() with just a different name.
	* \return T *
	*/
	T * current() const
	{
		return m_pIterator ? m_pIterator->current()->data() : nullptr;
	}

	/**
	* \brief Returns the value pointed by the iterator
	*
	* If the iterator is not valid it returns a default constructed value
	* This is an alias to current() with just a different name.
	* \return T *
	*/
	T * operator*() const
	{
		return m_pIterator ? m_pIterator->current()->data() : nullptr;
	}

	/**
	* \brief Returns the key pointed by the iterator
	*
	* If the iterator is not valid it returns a default constructed value
	* \return const Key &
	*/
	const Key & currentKey() const
	{
		if(m_pIterator)
			return m_pIterator->current()->key();
		return kvi_hash_key_default(((Key *)nullptr));
	}

	/**
	* \brief Moves the iterator to the first element of the hash table.
	*
	* Returns the first item found or nullptr if the hash table is empty.
	* \return T *
	*/
	T * toFirst()
	{
		if(!moveFirst())
			return nullptr;
		return current();
	}

public:
	/**
	* \brief Creates an iterator pointing to the first item in the hash table, if any.
	* \param hTable The hash table
	* \return KviPointerHashTableIterator
	*/
	KviPointerHashTableIterator(const KviPointerHashTable<Key, T> & hTable)
	{
		m_pHashTable = &hTable;
		m_uEntryIndex = 0;
		m_pIterator = nullptr;
		moveFirst();
	}

	/**
	* \brief Destroys the iterator
	*/
	~KviPointerHashTableIterator()
	{
		if(m_pIterator)
			delete m_pIterator;
	}
};

#endif //_KVI_POINTERHASHTABLE_H_
