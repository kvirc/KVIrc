#ifndef _KVITRANSLATIONENTRY_H_
#define _KVITRANSLATIONENTRY_H_
//=============================================================================
//
//   File : KviTranslationEntry.h
//   Creation date : Wed 23 Mar 2011 03:32:12
//   Originally in KviLocale.h
//      Created on: Sat Jan 16 1999 18:15:01 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 1999-2010 Szymon Stefanek (pragma at kvirc dot net)
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
* \file KviTranslationEntry.h
* \author Szymon Stefanek
* \brief Holds the translation strings
*
* This file was originally part of KviLocale.h
*/

#include "kvi_settings.h"
#include "KviCString.h"
#include "KviHeapObject.h"

class QString;

/**
* \class KviTranslationEntry
* \brief The KviTranslationEntry class
*
* This class holds the translation
*/
class KVILIB_API KviTranslationEntry : public KviHeapObject
{
	friend class KviMessageCatalogue;

private:
	KviCString m_szKey;
	KviCString m_szEncodedTranslation;
	QString * m_pTranslation;

public:
	/**
	* \brief Creates an instance of KviTranslationEntry
	* \param pcKey The string to translate from
	* \param iKeyLen The length of the string
	* \param pcTr The translated string
	* \param iTrLen The length of the translation
	* \return KviTranslationEntry
	*/
	KviTranslationEntry(char * pcKey, int iKeyLen, char * pcTr, int iTrLen);

	/**
	* \brief Creates an instance of KviTranslationEntry
	* \param pcKeyAndTr The string to translate from
	* \return KviTranslationEntry
	*/
	KviTranslationEntry(const char * pcKeyAndTr);

	/**
	* \brief Destroys the instance of KviTranslationEntry and frees all the relevant resources
	*/
	~KviTranslationEntry();
};

#endif //_KVITRANSLATIONENTRY_H_
