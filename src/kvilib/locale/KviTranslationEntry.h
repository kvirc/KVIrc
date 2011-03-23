#ifndef _KviTranslationEntry_h_
#define _KviTranslationEntry_h_
//=============================================================================
//
//   File : KviTranslationEntry.h
//   Creation date : Wed 23 Mar 2011 03:32:12
//   Originally in KviLocale.h
//      Created on: Sat Jan 16 1999 18:15:01 by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 1999-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviHeapObject.h"
#include "KviCString.h"

#include <QString>

///
/// \class KviTranslationEntry
/// \brief The KviTranslationEntry class
///
/// This class...
///
class KVILIB_API KviTranslationEntry : public KviHeapObject
{
	friend class KviMessageCatalogue;
private:
	KviCString m_szKey;
	KviCString m_szEncodedTranslation;
	QString * m_pQTranslation;
public:

	///
	/// Creates an instance of KviTranslationEntry
	///
	KviTranslationEntry(char * keyptr,int keylen,char * trptr,int trlen)
		: m_szKey(keyptr,keylen),
		m_szEncodedTranslation(trptr,trlen)
	{
		m_pQTranslation = NULL;
	}

	KviTranslationEntry(const char * keyandtr)
		: m_szKey(keyandtr),
		m_szEncodedTranslation(keyandtr)
	{
		m_pQTranslation = NULL;
	}

	///
	/// Destroys the instance of KviTranslationEntry
	/// and frees all the relevant resources
	///
	~KviTranslationEntry()
	{
		if(m_pQTranslation)
			delete m_pQTranslation;
	}

}; // class KviTranslationEntry



#endif //!_KviTranslationEntry_h_
