#ifndef _KVI_KVS_HASH_H_
#define _KVI_KVS_HASH_H_
//=============================================================================
//
//   File : KviKvsHash.h
//   Creation date : Tue 07 Oct 2003 01:22:37 by Szymon Stefanek
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
* \file KviKvsHash.h
* \author Szymon Stefanek
* \brief Handling of hash data type in KVS
*/

#include "kvi_settings.h"
#include "KviPointerHashTable.h"
#include "KviQString.h"
#include "KviKvsVariant.h"
#include "KviHeapObject.h"

typedef KVIRC_API_TYPEDEF KviPointerHashTableIterator<QString, KviKvsVariant> KviKvsHashIterator;

/**
* \class KviKvsHash
* \brief This class defines a new data type which contains hash data
* \warning This class must not have virtual functions nor destructor. Otherwise it will happily
* crash on windows when it is allocated in modules and destroyed anywhere else around
*/
class KVIRC_API KviKvsHash : public KviHeapObject
{
public:
	/**
	* \brief Constructs the hash data
	* \return KviKvsHash
	*/
	KviKvsHash();

	/**
	* \brief Constructs the hash data
	*
	* This is a carbon copy
	* \param hash The hash to copy from
	* \return KviKvsHash
	*/
	KviKvsHash(const KviKvsHash & hash);

	/**
	* \brief Destroys the array data
	*/
	~KviKvsHash();

public:
	/**
	* \brief Unsets an element from the hash
	* \param szKey The key of the element to unset
	* \return void
	*/
	void unset(const QString & szKey);

	/**
	* \brief Sets an element into the hash
	* \param szKey The key of the element to set
	* \param pVal The value to set
	* \return void
	*/
	void set(const QString & szKey, KviKvsVariant * pVal);

	/**
	* \brief Returns the element associated to the given key
	* \param szKey The key of the element to retrieve
	* \return KviKvsVariant *
	*/
	KviKvsVariant * find(const QString & szKey) const;

	/**
	* \brief Returns the element associated to the given key
	*
	* If the element doesn't exists, it returns an empty element. If the index is out of
	* hash bounds, it increases the hash size, fillin the hash in with empty strings.
	* \param szKey The key of the element to retrieve
	* \return KviKvsVariant *
	*/
	KviKvsVariant * get(const QString & szKey);

	/**
	* \brief Returns true if the hash is empty
	* \return bool
	*/
	bool isEmpty() const;

	/**
	* \brief clear the hash
	*/
	void clear();

	/**
	* \brief Returns the size of the hash
	* \return kvs_uint_t
	*/
	kvs_uint_t size() const;

	/**
	* \brief Appends data to the hash converting it into a string
	* \param szBuffer The string to append
	* \return void
	*/
	void appendAsString(QString & szBuffer) const;

	/**
	* \brief Returns the internal dictionary of the hash
	* \return const KviPointerHashTable<QString,KviKvsVariant> *
	*/
	const KviPointerHashTable<QString, KviKvsVariant> * dict();

	/**
	* \brief Serializes the hash to a given buffer
	* \param szResult The buffer to store
	* \return void
	*/
	void serialize(QString & szResult);

private:
	KviPointerHashTable<QString, KviKvsVariant> * m_pDict;
};

#endif // _KVI_KVS_HASH_H_
