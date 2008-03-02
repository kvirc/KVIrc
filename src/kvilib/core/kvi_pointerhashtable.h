#ifndef _KVI_POINTERHASHTABLE_H_
#define _KVI_POINTERHASHTABLE_H_
//=================================================================================================
//
//   File : kvi_pointerhashtable.h
//   Creation date : Sat Jan 12 2008 04:53 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2008 Szymon Stefanek (pragma at kvirc dot net)
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
//=================================================================================================

#include "kvi_settings.h"
#include "kvi_pointerlist.h"
#include "kvi_string.h"
#include "kvi_qstring.h"
#include "kvi_malloc.h"
#include "kvi_memmove.h"

#include <ctype.h>

///
/// Hash functions for various data types
///

inline unsigned int kvi_hash_hash(const char * szKey,bool bCaseSensitive)
{
	unsigned int uResult = 0;
	if(bCaseSensitive)
	{
		while(*szKey)
		{
			uResult += (unsigned char)(*(szKey));
			szKey++;
		}
	} else {
		while(*szKey)
		{
			uResult += (unsigned char)tolower(*(szKey));
			szKey++;
		}
	}
	return uResult;
}

inline bool kvi_hash_key_equal(const char * szKey1,const char * szKey2,bool bCaseSensitive)
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
	} else {
		while(*szKey1 && *szKey2)
		{
			if(tolower(*szKey1) != tolower(*szKey2))
				return false;
			szKey1++;
			szKey2++;
		}
	}
	return true;
}

inline void kvi_hash_key_copy(const char * const &szFrom,const char * &szTo,bool bDeepCopy)
{
	if(bDeepCopy)
	{
		int len = kvi_strLen(szFrom);
		char * dst = (char *)kvi_malloc(len+1);
		kvi_fastmove(dst,szFrom,len+1);
		szTo = dst;
	} else {
		szTo = szFrom; // we never modify it anyway
	}
}

inline void kvi_hash_key_destroy(const char * &szKey,bool bDeepCopy)
{
	if(bDeepCopy)
		kvi_free(szKey);
}

inline const char * & kvi_hash_key_default(const char **)
{
	static const char * static_null = NULL;
	return static_null;
}

inline unsigned int kvi_hash_hash(const KviStr &szKey,bool bCaseSensitive)
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
	} else {
		while(*p)
		{
			uResult += tolower(*((const unsigned char *)p));
			p++;
		}
	}
	return uResult;
}

inline bool kvi_hash_key_equal(const KviStr &szKey1,const KviStr &szKey2)
{
	return kvi_hash_key_equal(szKey1.ptr(),szKey2.ptr());
}

inline void kvi_hash_key_copy(const KviStr &szFrom,KviStr &szTo,bool)
{
	szTo = szFrom;
}

inline void kvi_hash_key_destroy(KviStr &szKey,bool)
{
}

inline const KviStr & kvi_hash_key_default(KviStr *)
{
	return KviStr::emptyString();
}

inline unsigned int kvi_hash_hash(const int &iKey,bool)
{
	return (unsigned int)iKey;
}

inline bool kvi_hash_key_equal(const int &iKey1,const int &iKey2,bool)
{
	return iKey1 == iKey2;
}

inline void kvi_hash_key_copy(const int &iKeyFrom,int &iKeyTo,bool)
{
	iKeyTo = iKeyFrom;
}

inline void kvi_hash_key_destroy(int &iKey,bool)
{
}

inline const int & kvi_hash_key_default(int *)
{
	static int static_default = 0;
	return static_default;
}

inline unsigned int kvi_hash_hash(const unsigned short &iKey,bool)
{
	return (unsigned int)iKey;
}

inline bool kvi_hash_key_equal(const unsigned short &iKey1,const unsigned short &iKey2,bool)
{
	return iKey1 == iKey2;
}

inline void kvi_hash_key_copy(const unsigned short &iKeyFrom,unsigned short &iKeyTo,bool)
{
	iKeyTo = iKeyFrom;
}

inline void kvi_hash_key_destroy(unsigned short &iKey,bool)
{
}

inline const unsigned short & kvi_hash_key_default(unsigned short *)
{
	static unsigned short static_default = 0;
	return static_default;
}


inline unsigned int kvi_hash_hash(void * pKey,bool)
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

inline bool kvi_hash_key_equal(void *pKey1,void *pKey2,bool)
{
	return pKey1 == pKey2;
}

inline void kvi_hash_key_copy(void * const &pKeyFrom,void *&pKeyTo,bool)
{
	pKeyTo = pKeyFrom;
}

inline void kvi_hash_key_destroy(void *iKey,bool)
{
}

inline void * & kvi_hash_key_default(void *)
{
	static void * static_default = NULL;
	return static_default;
}

inline unsigned int kvi_hash_hash(const QString &szKey,bool bCaseSensitive)
{
	unsigned int uResult = 0;
	const QChar * p = KviQString::nullTerminatedArray(szKey);
	if(!p)return 0;
	if(bCaseSensitive)
	{
		while(p->unicode())
		{
			uResult += p->unicode();
			p++;
		}
	} else {
		while(p->unicode())
		{
#ifdef COMPILE_USE_QT4
			uResult += p->toLower().unicode();
#else
			uResult += p->lower().unicode();
#endif
			p++;
		}
	}
	return uResult;
}

inline bool kvi_hash_key_equal(const QString &szKey1,const QString &szKey2,bool bCaseSensitive)
{
	if(bCaseSensitive)
		return KviQString::equalCS(szKey1,szKey2);
	return KviQString::equalCI(szKey1,szKey2);
}

inline void kvi_hash_key_copy(const QString &szFrom,QString &szTo,bool)
{
	szTo = szFrom;
}

inline void kvi_hash_key_destroy(QString &szKey,bool)
{
}

inline const QString & kvi_hash_key_default(QString *)
{
	return KviQString::empty;
}

template<typename Key,typename T> class KviPointerHashTable;
template<typename Key,typename T> class KviPointerHashTableIterator;

template<typename Key,typename T> class KviPointerHashTableEntry
{
	friend class KviPointerHashTable<Key,T>;
protected:
	T  * pData;
	Key  hKey;
public:
	Key & key(){ return hKey; };
	T * data(){ return pData; };
};

///
///
/// \class KviPointerHashTable
/// \brief A fast pointer hash table implementation
///
/// A very cool, very fast hash table implementation :P
///
/// To use this hash table you need to provide implementations
/// for the following functions:
///
/// \verbatim
///
/// unsigned int kvi_hash_hash(const Key &hKey,bool bCaseSensitive);
/// bool kvi_hash_key_equal(const Key &hKey1,const Key &hKey2,bool bCaseSensitive);
/// void kvi_hash_key_copy(const Key &hKeyFrom,Key &hKeyTo,bool bDeepCopy);
/// void kvi_hash_key_destroy(Key &hKey,bool bIsDeepCopy);
/// const Key & kvi_hash_key_default(Key *);
///
/// \endverbatim
///
/// Implementations for the most likey Key data types are provided below.
/// KviPointerHashTable will automagically work with const char *,QString,KviStr
/// and integer types as keys.
///
/// For string Key types, the hash table may or may not be case sensitive.
/// For other Key types the case sensitive flag has no meaning and will
/// (hopefully) be optimized out by the compiler.
///
/// For pointer based keys the hash table may or may not mantain deep copies
/// of Key data. For example, with char * keys, if deep copying is enabled
/// then a private copy of the string data will be mantained. With deep
/// copying disabled only char * pointers will be kept. For types
/// that do not have meaning of deep copy the deep copying code will
/// (hopefully) be optimized out by the compiler.
///
/// The hashtable mantains an array of KviPointerList based buckets.
/// The number of buckets may be specified by the application user
/// and does NOT need to be a prime number. Yet better to have it a power
/// of two so the memory allocation routines will feel better and are
/// less likely to waste space.
///
template<class Key,class T> class KviPointerHashTable
{
	friend class KviPointerHashTableIterator<Key,T>;
protected:
	KviPointerList<KviPointerHashTableEntry<Key,T> >      ** m_pDataArray;
	bool                                                     m_bAutoDelete;
	unsigned int                                             m_uSize;
	unsigned int                                             m_uCount;
	bool                                                     m_bCaseSensitive;
	bool                                                     m_bDeepCopyKeys;
	unsigned int                                             m_uIteratorIdx;
public:
	///
	/// Returns the item associated to the key hKey
	/// or NULL if no such item exists in the hash table.
	/// Places the hash table iterator at the position
	/// of the item found.
	///
	T * find(const Key & hKey)
	{
		m_uIteratorIdx = kvi_hash_hash(hKey,m_bCaseSensitive) % m_uSize;
		if(!m_pDataArray[m_uIteratorIdx])return 0;
		for(KviPointerHashTableEntry<Key,T> * e = m_pDataArray[m_uIteratorIdx]->first();e;e = m_pDataArray[m_uIteratorIdx]->next())
		{
			if(kvi_hash_key_equal(e->hKey,hKey,m_bCaseSensitive))return (T *)e->pData;
		}
		return 0;
	}
	
	///
	/// Returns the item associated to the key hKey
	/// or NULL if no such item exists in the hash table.
	/// Places the hash table iterator at the position
	/// of the item found. This is an alias to find().
	///
	T * operator[](const Key & hKey)
	{
		return find(hKey);
	}

	///
	/// Returns the number of items in this hash table
	///
	unsigned int count() const
	{
		return m_uCount;
	}

	///
	/// Returns true if the hash table is empty
	///
	bool isEmpty() const
	{
		return m_uCount == 0;
	}

	///
	/// Inserts the item pData at the position specified by the key hKey.
	/// Replaces any previous item with the same key
	/// The replaced item is deleted if autodelete is enabled.
	/// The hash table iterator is placed at the newly inserted item.
	///
	void insert(const Key & hKey,T * pData)
	{
		if(!pData)return;
		unsigned int uEntry = kvi_hash_hash(hKey,m_bCaseSensitive) % m_uSize;
		if(!m_pDataArray[uEntry])m_pDataArray[uEntry] = new KviPointerList<KviPointerHashTableEntry<Key,T> >(true);
		for(KviPointerHashTableEntry<Key,T> * e = m_pDataArray[uEntry]->first();e;e = m_pDataArray[uEntry]->next())
		{
			if(kvi_hash_key_equal(e->hKey,hKey,m_bCaseSensitive))
			{
				if(!m_bCaseSensitive)
				{
					// must change the key too
					kvi_hash_key_destroy(e->hKey,m_bDeepCopyKeys);
					kvi_hash_key_copy(hKey,e->hKey,m_bDeepCopyKeys);
				}
				if(m_bAutoDelete)delete e->pData;
				e->pData = pData;
				return;
			}
		}
		KviPointerHashTableEntry<Key,T> * n = new KviPointerHashTableEntry<Key,T>;
		kvi_hash_key_copy(hKey,n->hKey,m_bDeepCopyKeys);
		n->pData = pData;
		m_pDataArray[uEntry]->append(n);
		m_uCount++;
	}

	///
	/// Inserts the item pData at the position specified by the key hKey.
	/// Replaces any previous item with the same key
	/// The replaced item is deleted if autodelete is enabled.
	/// The hash table iterator is placed at the newly inserted item.
	/// This is just an alias to insert() with a different name.
	///
	void replace(const Key & hKey,T * pData)
	{
		insert(hKey,pData);
	}

	///
	/// Removes the item pointer associated to the key hKey, if such an item
	/// exists in the hash table. The item is deleted if autodeletion
	/// is enabled. Returns true if the item was found and removed and false if it wasn't found.
	/// Invalidates the hash table iterator.
	///
	bool remove(const Key & hKey)
	{
		unsigned int uEntry = kvi_hash_hash(hKey,m_bCaseSensitive) % m_uSize;
		if(!m_pDataArray[uEntry])return false;
		for(KviPointerHashTableEntry<Key,T> * e = m_pDataArray[uEntry]->first();e;e = m_pDataArray[uEntry]->next())
		{
			if(kvi_hash_key_equal(e->hKey,hKey,m_bCaseSensitive))
			{
				kvi_hash_key_destroy(e->hKey,m_bDeepCopyKeys);
				if(m_bAutoDelete)delete ((T *)(e->pData));
				m_pDataArray[uEntry]->removeRef(e);
				if(m_pDataArray[uEntry]->isEmpty())
				{
					delete m_pDataArray[uEntry];
					m_pDataArray[uEntry] = 0;
				}
				m_uCount--;
				return true;
			}
		}
		return false;
	}

	///
	/// Removes the first occurence of the item pointer pRef. The item is deleted if autodeletion
	/// is enabled. Returns true if the pointer was found and false otherwise
	/// Invalidates the hash table iterator.
	///
	bool removeRef(const T * pRef)
	{
		for(unsigned int i=0;i<m_uSize;i++)
		{
			if(m_pDataArray[i])
			{
				for(KviPointerHashTableEntry<Key,T> * e = m_pDataArray[i]->first();e;e = m_pDataArray[i]->next())
				{
					if(e->pData == pRef)
					{
						kvi_hash_key_destroy(e->hKey,m_bDeepCopyKeys);
						if(m_bAutoDelete)delete ((T *)(e->pData));
						m_pDataArray[i]->removeRef(e);
						if(m_pDataArray[i]->isEmpty())
						{
							delete m_pDataArray[i];
							m_pDataArray[i] = 0;
						}
						m_uCount--;
						return true;
					}
				}
			}			
		}
		return false;
	}

	///
	/// Removes all the items from the hash table.
	/// The items are deleted if autodeletion is enabled.
	/// Invalidates the hash table iterator.
	///
	void clear()
	{
		for(unsigned int i=0;i<m_uSize;i++)
		{
			if(m_pDataArray[i])
			{
				for(KviPointerHashTableEntry<Key,T> * e = m_pDataArray[i]->first();e;e = m_pDataArray[i]->next())
				{
					kvi_hash_key_destroy(e->hKey,m_bDeepCopyKeys);
					if(m_bAutoDelete)
						delete ((T *)(e->pData));
				}
				delete m_pDataArray[i];
				m_pDataArray[i] = 0;
			}
		}
		m_uCount = 0;
	}

	///
	/// Searches for the item pointer pRef and returns
	/// it's hash table entry, if found, and NULL otherwise.
	/// The hash table iterator is placed at the item found.
	///
	KviPointerHashTableEntry<Key,T> * findRef(const T * pRef)
	{
		for(m_uIteratorIdx = 0;m_uIteratorIdx<m_uSize;m_uIteratorIdx++)
		{
			if(m_pDataArray[m_uIteratorIdx])
			{
				for(KviPointerHashTableEntry<Key,T> * e = m_pDataArray[m_uIteratorIdx]->first();e;e = m_pDataArray[m_uIteratorIdx]->next())
				{
					if(e->pData == pRef)return e;
				}
			}
		}
		return 0;
	}

	///
	/// Returns the entry pointed by the hash table iterator.
	/// This function must be preceeded by a call to firstEntry(), first()
	/// or findRef().
	///
	KviPointerHashTableEntry<Key,T> * currentEntry()
	{
		if(m_uIteratorIdx >= m_uSize)return 0;
		if(m_pDataArray[m_uIteratorIdx])return m_pDataArray[m_uIteratorIdx]->current();
		return 0;
	}

	///
	/// Places the hash table iterator at the first entry
	/// and returns it.
	///
	KviPointerHashTableEntry<Key,T> * firstEntry()
	{
		m_uIteratorIdx = 0;
		while(m_uIteratorIdx < m_uSize && (!m_pDataArray[m_uIteratorIdx]))
		{
			m_uIteratorIdx++;
		}
		if(m_uIteratorIdx == m_uSize)return 0;
		return m_pDataArray[m_uIteratorIdx]->first();
	}

	///
	/// Places the hash table iterator at the next entry
	/// and returns it.
	/// This function must be preceeded by a call to firstEntry(), first()
	/// or findRef().
	///
	KviPointerHashTableEntry<Key,T> * nextEntry()
	{
		if(m_uIteratorIdx >= m_uSize)return 0;

		if(m_uIteratorIdx < m_uSize)
		{
			KviPointerHashTableEntry<Key,T> * t = m_pDataArray[m_uIteratorIdx]->next();
			if(t)return t;
		}

		m_uIteratorIdx++;

		while(m_uIteratorIdx < m_uSize && (!m_pDataArray[m_uIteratorIdx]))
		{
			m_uIteratorIdx++;
		}

		if(m_uIteratorIdx == m_uSize)return 0;

		return m_pDataArray[m_uIteratorIdx]->first();

	}

	///
	/// Returns the data value pointer pointed by the hash table iterator.
	/// This function must be preceeded by a call to firstEntry(), first()
	/// or findRef().
	///
	T * current()
	{
		if(m_uIteratorIdx >= m_uSize)return 0;
		if(m_pDataArray[m_uIteratorIdx])
		{
			KviPointerHashTableEntry<Key,T> * e = m_pDataArray[m_uIteratorIdx]->current();
			if(!e)return 0;
			return e->data();
		}
		return 0;
	}
	
	///
	/// Returns the key pointed by the hash table iterator.
	/// This function must be preceeded by a call to firstEntry(), first()
	/// or findRef().
	///
	const Key & currentKey()
	{
		if(m_uIteratorIdx >= m_uSize)return kvi_hash_key_default(((Key *)NULL));
		if(m_pDataArray[m_uIteratorIdx])
		{
			KviPointerHashTableEntry<Key,T> * e = m_pDataArray[m_uIteratorIdx]->current();
			if(!e)return kvi_hash_key_default(((Key *)NULL));
			return e->key();
		}
		return kvi_hash_key_default(((Key *)NULL));
	}

	///
	/// Places the hash table iterator at the first entry
	/// and returns the associated data value pointer.
	///
	T * first()
	{
		m_uIteratorIdx = 0;
		while(m_uIteratorIdx < m_uSize && (!m_pDataArray[m_uIteratorIdx]))
		{
			m_uIteratorIdx++;
		}
		if(m_uIteratorIdx == m_uSize)return 0;
		KviPointerHashTableEntry<Key,T> * e = m_pDataArray[m_uIteratorIdx]->first();
		if(!e)return 0;
		return e->data();
	}

	///
	/// Places the hash table iterator at the next entry
	/// and returns the associated data value pointer.
	/// This function must be preceeded by a call to firstEntry(), first()
	/// or findRef().
	///
	T * next()
	{
		if(m_uIteratorIdx >= m_uSize)return 0;

		if(m_uIteratorIdx < m_uSize)
		{
			KviPointerHashTableEntry<Key,T> * t = m_pDataArray[m_uIteratorIdx]->next();
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

		if(m_uIteratorIdx == m_uSize)return 0;

		KviPointerHashTableEntry<Key,T> * e = m_pDataArray[m_uIteratorIdx]->first();
		if(!e)return 0;
		return e->data();
	}

	///
	/// Removes all items in the hash table and then
	/// makes a complete shallow copy of the data contained in t.
	/// The removed items are deleted if autodeletion is enabled.
	/// The hash table iterator is invalidated.
	/// Does not change autodelete flag: make sure you not delete the items twice :)
	///
	void copyFrom(KviPointerHashTable<Key,T> &t)
	{
		clear();
		for(KviPointerHashTableEntry<Key,T> * e = t.firstEntry();e;e = t.nextEntry())
			insert(e->key(),e->data());
	}

	///
	/// Inserts a complete shallow copy of the data contained in t.
	/// The hash table iterator is invalidated.
	///
	void insert(KviPointerHashTable<Key,T> &t)
	{
		for(KviPointerHashTableEntry<Key,T> * e = t.firstEntry();e;e = t.nextEntry())
			insert(e->key(),e->data());
	}

	///
	/// Enables or disabled the autodeletion feature.
	/// Items are deleted upon removal when the feature is enabled.
	///
	void setAutoDelete(bool bAutoDelete)
	{
		m_bAutoDelete = bAutoDelete;
	}

	///
	/// Creates an empty hash table.
	/// Automatic deletion is enabled.
	///
	/// \param uSize The number of hash buckets: does NOT necesairly need to be prime
	/// \param bCaseSensitive Are the key comparisons case sensitive ?
	/// \param Do we need to mantain deep copies of keys ?
	///
	KviPointerHashTable(unsigned int uSize = 32,bool bCaseSensitive = true,bool bDeepCopyKeys = true)
	{
		m_uCount = 0;
		m_bCaseSensitive = bCaseSensitive;
		m_bAutoDelete = true;
		m_bDeepCopyKeys = bDeepCopyKeys;
		m_uSize = uSize > 0 ? uSize : 32;
		m_pDataArray = new KviPointerList<KviPointerHashTableEntry<Key,T> > *[uSize];
		for(unsigned int i=0;i<m_uSize;i++)m_pDataArray[i] = 0;
	}

	///
	/// First creates an empty hash table
	/// and then inserts a copy of all the item pointers present in t.
	/// The autodelete feature is automatically disabled (take care!).
	///
	KviPointerHashTable(KviPointerHashTable<Key,T> &t)
	{
		m_uCount = 0;
		m_bAutoDelete = false;
		m_bCaseSensitive = t.m_bCaseSensitive;
		m_bDeepCopyKeys = t.m_bDeepCopyKeys;
		m_uSize = t.m_uSize;
		m_pDataArray = new KviPointerList<KviPointerHashTableEntry<Key,T> > *[m_uSize];
		for(unsigned int i=0;i<m_uSize;i++)m_pDataArray[i] = 0;
		copyFrom(t);
	}

	///
	/// Destroys the hash table and all the items contained within.
	/// Items are deleted if autodeletion is enabled.
	///
	~KviPointerHashTable()
	{
		clear();
		delete [] m_pDataArray;
	}
};

template<typename Key,typename T> class KviPointerHashTableIterator
{
protected:
	const KviPointerHashTable<Key,T>                         * m_pHashTable;
	unsigned int                                               m_uEntryIndex;
	KviPointerListIterator<KviPointerHashTableEntry<Key,T> > * m_pIterator;
public:
	///
	/// Creates an iterator copy.
	/// The new iterator points exactly to the item pointed by src.
	///
	void operator = (const KviPointerHashTableIterator<Key,T> &src)
	{
		m_pHashTable = src.m_pHashTable;
		m_uEntryIndex = src.m_uEntryIndex;
		if(src.m_pIterator)
			m_pIterator = new KviPointerListIterator<KviPointerHashTableEntry<Key,T> >(*(src.m_pIterator));
		else
			m_pIterator = NULL;
	}

	///
	/// Moves the iterator to the first element of the hash table.
	/// Returns true in case of success or false if the hash table is empty.
	///
	bool moveFirst()
	{
		if(m_pIterator)
		{
			delete m_pIterator;
			m_pIterator = NULL;
		}

		m_uEntryIndex = 0;
		while((m_uEntryIndex < m_pHashTable->m_uSize) && (!(m_pHashTable->m_pDataArray[m_uEntryIndex])))
		{
			m_uEntryIndex++;
		}

		if(m_uEntryIndex == m_pHashTable->m_uSize)
			return false;

		m_pIterator = new KviPointerListIterator<KviPointerHashTableEntry<Key,T> >(*(m_pHashTable->m_pDataArray[m_uEntryIndex]));
		bool bRet = m_pIterator->moveFirst();
		if(!bRet)
		{
			delete m_pIterator;
			m_pIterator = NULL;
		}
		return bRet;
	}
	
	///
	/// Moves the iterator to the last element of the hash table.
	/// Returns true in case of success or false if the hash table is empty.
	///
	bool moveLast()
	{
		if(m_pIterator)
		{
			delete m_pIterator;
			m_pIterator = NULL;
		}

		m_uEntryIndex = m_pHashTable->m_uSize;
		while(m_uEntryIndex > 0)
		{
			m_uEntryIndex--;
			if(m_pHashTable->m_pDataArray[m_uEntryIndex])
			{
				m_pIterator = new KviPointerListIterator<KviPointerHashTableEntry<Key,T> >(*(m_pHashTable->m_pDataArray[m_uEntryIndex]));
				bool bRet = m_pIterator->moveLast();
				if(!bRet)
				{
					delete m_pIterator;
					m_pIterator = NULL;
				}
				return bRet;
			}
		}
		return false;
	}

	///
	/// Moves the iterator to the next element of the hash table.
	/// The iterator must be actually valid for this function to work.
	/// Returns true in case of success or false if there is no next item.
	///
	bool moveNext()
	{
		if(!m_pIterator)
			return false;
		if(m_pIterator->moveNext())
			return true;
		if(m_pIterator)
		{
			delete m_pIterator;
			m_pIterator = NULL;
		}
		m_uEntryIndex++;
		while((m_uEntryIndex < m_pHashTable->m_uSize) && (!(m_pHashTable->m_pDataArray[m_uEntryIndex])))
		{
			m_uEntryIndex++;
		}
		if(m_uEntryIndex == m_pHashTable->m_uSize)
			return false;
		m_pIterator = new KviPointerListIterator<KviPointerHashTableEntry<Key,T> >(*(m_pHashTable->m_pDataArray[m_uEntryIndex]));
		bool bRet = m_pIterator->moveFirst();
		if(!bRet)
		{
			delete m_pIterator;
			m_pIterator = NULL;
		}
		return bRet;
	}
	
	///
	/// Moves the iterator to the next element of the hash table.
	/// The iterator must be actually valid for this function to work.
	/// Returns true in case of success or false if there is no next item.
	/// This is just an alias to moveNext().
	///
	bool operator ++()
	{
		return moveNext();
	}
	
	///
	/// Moves the iterator to the previous element of the hash table.
	/// The iterator must be actually valid for this function to work.
	/// Returns true in case of success or false if there is no previous item.
	///
	bool movePrev()
	{
		if(!m_pIterator)
			return false;
		if(m_pIterator->movePrev())
			return true;
		if(m_pIterator)
		{
			delete m_pIterator;
			m_pIterator = NULL;
		}
		if(m_uEntryIndex >= m_pHashTable->m_uSize)
			return false;
		while(m_uEntryIndex > 0)
		{
			m_uEntryIndex--;
			if(m_pHashTable->m_pDataArray[m_uEntryIndex])
			{
				m_pIterator = new KviPointerListIterator<KviPointerHashTableEntry<Key,T> >(*(m_pHashTable->m_pDataArray[m_uEntryIndex]));
				bool bRet = m_pIterator->moveLast();
				if(!bRet)
				{
					delete m_pIterator;
					m_pIterator = NULL;
				}
				return bRet;
			}
		}
		return false;
	}

	
	///
	/// Moves the iterator to the previous element of the hash table.
	/// The iterator must be actually valid for this function to work.
	/// Returns true in case of success or false if there is no previous item.
	/// This is just an alias to movePrev() with a different name.
	///
	bool operator --()
	{
		return movePrev();
	}

	///
	/// Returs the value pointed by the iterator
	/// or a default constructed value if the iterator is not valid.
	/// This is an alias to operator *() with just a different name.
	///
	T * current() const
	{
		return m_pIterator ? m_pIterator->current()->data() : NULL;
	}

	///
	/// Returs the value pointed by the iterator
	/// or a default constructed value if the iterator is not valid.
	/// This is an alias to current() with just a different name.
	///
	T * operator *() const
	{
		return m_pIterator ? m_pIterator->current()->data() : NULL;
	}
	
	///
	/// Returs the key pointed by the iterator
	/// or a default constructed key if the iterator is not valid.
	///
	const Key & currentKey() const
	{
		return m_pIterator ? m_pIterator->current()->key() : kvi_hash_key_default(((Key *)NULL));
	}

	///
	/// Moves the iterator to the first element of the hash table.
	/// Returns the first item found or NULL if the hash table is empty.
	///
	T * toFirst()
	{
		if(!moveFirst())
			return NULL;
		return current();
	}
public:
	///
	/// Creates an iterator pointing to the first item in the hash table, if any.
	///
	KviPointerHashTableIterator(const KviPointerHashTable<Key,T> &hTable)
	{
		m_pHashTable = &hTable;
		m_uEntryIndex = 0;
		m_pIterator = NULL;
		moveFirst();
	}
	
	///
	/// Destroys the iterator
	///
	~KviPointerHashTableIterator()
	{
		if(m_pIterator)
			delete m_pIterator;
	}
};




#endif //_KVI_POINTERHASHTABLE_H_
