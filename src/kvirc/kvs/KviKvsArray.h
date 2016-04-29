#ifndef _KVI_KVS_ARRAY_H_
#define _KVI_KVS_ARRAY_H_
//=============================================================================
//
//   File : KviKvsArray.h
//   Creation date : Tue 07 Oct 2003 01:07:31 by Szymon Stefanek
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
* \file KviKvsArray.h
* \author Szymon Stefanek
* \brief Handling of array data type in KVS
*/

#include "kvi_settings.h"
#include "KviKvsVariant.h"
#include "KviHeapObject.h"

/**
* \class KviKvsArray
* \brief This class defines a new data type which contains array data
* \warning This class must not have virtual functions nor destructor. Otherwise it will happily
* crash on windows when it is allocated in modules and destroyed anywhere else around
*/
class KVIRC_API KviKvsArray : public KviHeapObject
{
public:
	/**
	* \brief Constructs the array data
	* \return KviKvsArray
	*/
	KviKvsArray();

	/**
	* \brief Constructs the array data
	*
	* This is a carbon copy
	* \param array The array to copy from
	* \return KviKvsArray
	*/
	KviKvsArray(const KviKvsArray & array);

	/**
	* \brief Destroys the array data
	*/
	~KviKvsArray();

protected:
	KviKvsVariant ** m_pData;
	kvs_uint_t m_uSize;
	kvs_uint_t m_uAllocSize;

public:
	/**
	* \brief Unsets an element from the array
	* \param uIdx The index of the element to unset
	* \return void
	*/
	void unset(kvs_uint_t uIdx);

	/**
	* \brief Sets an element into the array at the given index
	* \param uIdx The index of the element to set
	* \param pVal The value to set
	* \return void
	*/
	void set(kvs_uint_t uIdx, KviKvsVariant * pVal);

	/**
	* Appends a variant to this array.
	*/
	void append(KviKvsVariant * pVal);

	/**
	* \brief Returns the element at the given index
	* \param uIdx The index of the element to retrieve
	* \return KviKvsVariant *
	*/
	KviKvsVariant * at(kvs_uint_t uIdx) const { return (uIdx < m_uSize) ? m_pData[uIdx] : 0; };

	/**
	* \brief Returns the element at the given index
	*
	* If the element doesn't exists, it returns an empty element. If the index is out of
	* array bounds, it increases the array size, fillin the array in with zeros.
	* \param uIdx The index of the element to retrieve
	* \return KviKvsVariant *
	*/
	KviKvsVariant * getAt(kvs_uint_t uIdx);

	/**
	* \brief Returns true if the array is empty
	* \return bool
	*/
	bool isEmpty() { return m_uSize == 0; };

	/**
	* \brief Returns the size of the array
	* \return kvs_uint_t
	*/
	kvs_uint_t size() { return m_uSize; };

	/**
	* \brief Appends data to the array converting it into a string
	* \param szBuffer The string to append
	* \return void
	*/
	void appendAsString(QString & szBuffer);

	/**
	* \brief Serializes the array to a given buffer
	* \param szResult The buffer to store
	* \return void
	*/
	void serialize(QString & szResult);

	/**
	* \brief Sorts the array
	* \return void
	*/
	void sort();

	/**
	* \brief Sorts the array in reverse order
	* \return void
	*/
	void rsort();

protected:
	/**
	* \brief Finds the new size of the array
	* \return void
	*/
	void findNewSize();

private:
	/**
	* \brief Compares two elements of the array
	* \param pV1 The first element to compare
	* \param pV2 The second element to compare
	* \return int
	*/
	static int compare(const void * pV1, const void * pV2);

	/**
	* \brief Compares two elements of the array in reverse order
	* \param pV1 The first element to compare
	* \param pV2 The second element to compare
	* \return int
	*/
	static int compareReverse(const void * pV1, const void * pV2);
};

#endif // _KVI_KVS_ARRAY_H_
