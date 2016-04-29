#ifndef _KVI_KVS_VARIANTLIST_H_
#define _KVI_KVS_VARIANTLIST_H_
//=============================================================================
//
//   File : KviKvsVariantList.h
//   Creation date : Sat 11 Oct 2003 02:20:51 by Szymon Stefanek
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
* \file KviKvsVariantList.h
* \author Szymon Stefanek
* \brief Parser class to handle variant variables lists
*/

#include "kvi_settings.h"
#include "KviPointerList.h"
#include "KviKvsVariant.h"

/**
* \class KviKvsVariantList
* \brief Class to handle variant variables lists
*/
class KVIRC_API KviKvsVariantList
{
public:
	/**
	* \brief Constructs the KviKvsVariantList object
	* \return KviKvsVariantList
	*/
	KviKvsVariantList();

	/**
	* \brief Constructs the KviKvsVariantList object
	* \param pV1 The first element's list
	* \return KviKvsVariantList
	*/
	KviKvsVariantList(KviKvsVariant * pV1);

	/**
	* \brief Constructs the KviKvsVariantList object
	* \param pV1 The first element's list
	* \param pV2 The second element's list
	* \return KviKvsVariantList
	*/
	KviKvsVariantList(KviKvsVariant * pV1, KviKvsVariant * pV2);

	/**
	* \brief Constructs the KviKvsVariantList object
	* \param pV1 The first element's list
	* \param pV2 The second element's list
	* \param pV3 The third element's list
	* \return KviKvsVariantList
	*/
	KviKvsVariantList(KviKvsVariant * pV1, KviKvsVariant * pV2, KviKvsVariant * pV3);

	/**
	* \brief Constructs the KviKvsVariantList object
	* \param pV1 The first element's list
	* \param pV2 The second element's list
	* \param pV3 The third element's list
	* \param pV4 The fourth element's list
	* \return KviKvsVariantList
	*/
	KviKvsVariantList(KviKvsVariant * pV1, KviKvsVariant * pV2, KviKvsVariant * pV3, KviKvsVariant * pV4);

	/**
	* \brief Constructs the KviKvsVariantList object
	* \param pV1 The first element's list
	* \param pV2 The second element's list
	* \param pV3 The third element's list
	* \param pV4 The fourth element's list
	* \param pV5 The fifth element's list
	* \return KviKvsVariantList
	*/
	KviKvsVariantList(KviKvsVariant * pV1, KviKvsVariant * pV2, KviKvsVariant * pV3, KviKvsVariant * pV4, KviKvsVariant * pV5);

	/**
	* \brief Constructs the KviKvsVariantList object
	* \param pV1 The first element's list
	* \param pV2 The second element's list
	* \param pV3 The third element's list
	* \param pV4 The fourth element's list
	* \param pV5 The fifth element's list
	* \param pV6 The sixth element's list
	* \return KviKvsVariantList
	*/
	KviKvsVariantList(KviKvsVariant * pV1, KviKvsVariant * pV2, KviKvsVariant * pV3, KviKvsVariant * pV4, KviKvsVariant * pV5, KviKvsVariant * pV6);

	/**
	* \brief Constructs the KviKvsVariantList object
	* \param pV1 The first element's list
	* \param pV2 The second element's list
	* \param pV3 The third element's list
	* \param pV4 The fourth element's list
	* \param pV5 The fifth element's list
	* \param pV6 The sixth element's list
	* \param pV7 The seventh element's list
	* \return KviKvsVariantList
	*/
	KviKvsVariantList(KviKvsVariant * pV1, KviKvsVariant * pV2, KviKvsVariant * pV3, KviKvsVariant * pV4, KviKvsVariant * pV5, KviKvsVariant * pV6, KviKvsVariant * pV7);

	/**
	* \brief Constructs the KviKvsVariantList object
	* \param pS1 The first element's list
	* \return KviKvsVariantList
	*/
	KviKvsVariantList(QString * pS1);

	/**
	* \brief Constructs the KviKvsVariantList object
	* \param pS1 The first element's list
	* \param pS2 The second element's list
	* \return KviKvsVariantList
	*/
	KviKvsVariantList(QString * pS1, QString * pS2);

	/**
	* \brief Constructs the KviKvsVariantList object
	* \param pS1 The first element's list
	* \param pS2 The second element's list
	* \param pS3 The third element's list
	* \return KviKvsVariantList
	*/
	KviKvsVariantList(QString * pS1, QString * pS2, QString * pS3);

	/**
	* \brief Constructs the KviKvsVariantList object
	* \param pS1 The first element's list
	* \param pS2 The second element's list
	* \param pS3 The third element's list
	* \param pS4 The fourth element's list
	* \return KviKvsVariantList
	*/
	KviKvsVariantList(QString * pS1, QString * pS2, QString * pS3, QString * pS4);

	/**
	* \brief Constructs the KviKvsVariantList object
	* \param pS1 The first element's list
	* \param pS2 The second element's list
	* \param pS3 The third element's list
	* \param pS4 The fourth element's list
	* \param pS5 The fifth element's list
	* \return KviKvsVariantList
	*/
	KviKvsVariantList(QString * pS1, QString * pS2, QString * pS3, QString * pS4, QString * pS5);

	/**
	* \brief Constructs the KviKvsVariantList object
	* \param pS1 The first element's list
	* \param pS2 The second element's list
	* \param pS3 The third element's list
	* \param pS4 The fourth element's list
	* \param pS5 The fifth element's list
	* \param pS6 The sixth element's list
	* \return KviKvsVariantList
	*/
	KviKvsVariantList(QString * pS1, QString * pS2, QString * pS3, QString * pS4, QString * pS5, QString * pS6);

	/**
	* \brief Constructs the KviKvsVariantList object
	* \param pS1 The first element's list
	* \param pS2 The second element's list
	* \param pS3 The third element's list
	* \param pS4 The fourth element's list
	* \param pS5 The fifth element's list
	* \param pS6 The sixth element's list
	* \param pS7 The seventh element's list
	* \return KviKvsVariantList
	*/
	KviKvsVariantList(QString * pS1, QString * pS2, QString * pS3, QString * pS4, QString * pS5, QString * pS6, QString * pS7);

	/**
	* \brief Constructs the KviKvsVariantList object
	* \param pSL Pointer to a QStringList
	* \return KviKvsVariantList
	*/
	KviKvsVariantList(QStringList * pSL);

	/**
	* \brief Destroys the KviKvsVariantList object
	*/
	~KviKvsVariantList();

protected:
	KviPointerList<KviKvsVariant> * m_pList;

public:
	/**
	* \brief Returns the first element of the list
	* \return KviKvsVariant *
	*/
	KviKvsVariant * first() { return m_pList->first(); };

	/**
	* \brief Returns the next element of the list
	* \return KviKvsVariant *
	*/
	KviKvsVariant * next() { return m_pList->next(); };

	/**
	* \brief Returns the element of the list at the given index
	* \param iIdx The index of the list we want to extract
	* \return KviKvsVariant *
	*/
	KviKvsVariant * at(int iIdx) { return m_pList->at(iIdx); };

	/**
	* \brief Returns the size of the list
	* \return unsigned int
	*/
	unsigned int count() { return m_pList->count(); };

	/**
	* \brief Clears the list
	* \return void
	*/
	void clear() { m_pList->clear(); };

	/**
	* \brief Appends an element to the list
	* \param pItem The element to append
	* \return void
	*/
	void append(KviKvsVariant * pItem) { m_pList->append(pItem); };

	/**
	* \brief Prepends an element to the list
	* \param pItem The element to prepend
	* \return void
	*/
	void prepend(KviKvsVariant * pItem) { m_pList->prepend(pItem); };

	/**
	* \brief Appends an element to the list
	* \param szParam The string element to append
	* \param bEscape Whether the string has to be escaped for KVS
	* \return void
	*/
	void append(const QString & szParam, bool bEscape = false) { m_pList->append(new KviKvsVariant(szParam, bEscape)); };

	/**
	* \brief Appends an element to the list
	* \param iInt The integer element to append
	* \return void
	*/
	void append(kvs_int_t iInt) { m_pList->append(new KviKvsVariant(iInt)); };

	/**
	* \brief Appends an element to the list
	* \param dReal The real element to append
	* \return void
	*/
	void append(kvs_real_t dReal) { m_pList->append(new KviKvsVariant(dReal)); };

	/**
	* \brief Appends an element to the list
	* \param bBoolean The boolean element to append
	* \return void
	*/
	void append(bool bBoolean) { m_pList->append(new KviKvsVariant(bBoolean)); };

	/**
	* \brief Appends an element to the list
	* \param hObject The hObject element to append
	* \return void
	*/
	void append(kvs_hobject_t hObject) { m_pList->append(new KviKvsVariant(hObject)); };

	/**
	* \brief Appends an element to the list
	* \param pArray The array element to append
	* \return void
	*/
	void append(KviKvsArray * pArray) { m_pList->append(new KviKvsVariant(pArray)); };

	/**
	* \brief Appends an element to the list
	* \param pHash The hash element to append
	* \return void
	*/
	void append(KviKvsHash * pHash) { m_pList->append(new KviKvsVariant(pHash)); };

	/**
	* \brief Sets the auto delete flag on the list
	* \param bAutoDelete Whether the list has to auto delete itself
	* \return void
	*/
	void setAutoDelete(bool bAutoDelete); // this is true by default

	/**
	* \brief Appends all elements in a single string delimited by spaces
	* \param szBuffer The buffer where to store the string
	* \return void
	*/
	void allAsString(QString & szBuffer);

	/**
	* \brief Returns true if there was a first parameter at all
	* \param szBuffer The buffer where to store the string
	* \return bool
	*/
	bool firstAsString(QString & szBuffer);

	/**
	* \brief Returns true if there was a next parameter at all
	* \param szBuffer The buffer where to store the string
	* \return bool
	*/
	bool nextAsString(QString & szBuffer);
};

#endif // _KVI_KVS_VARIANTLIST_H_
