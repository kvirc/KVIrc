#ifndef _KVI_LOCALE_H_
#define _KVI_LOCALE_H_

//=============================================================================
//
//   File : KviLocale.h
//   Creation date : Sat Jan 16 1999 18:15:01 by Szymon Stefanek
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

/**
* \file KviLocale.h
* \author Szymon Stefanek
* \brief KVIrc localization stuff
*/

#include "kvi_settings.h"
#include "KviCString.h"
#include "KviMessageCatalogue.h"

class KviCString;
class QApplication;
class QString;
class QTextCodec;


#ifndef QT_NO_BIG_CODECS
	#define KVI_NUM_ENCODINGS 109
	#define KVI_NUM_ENCODING_GROUPS 8
#else
	#define KVI_NUM_ENCODINGS 85
	#define KVI_NUM_ENCODING_GROUPS 5
#endif

/**
* \namespace KviLocale
* \brief Holds the localization functions
*/
namespace KviLocale
{
	/**
	* \typedef EncodingDescription
	* \struct _EncodingDescription
	* \brief Holds the encoding data
	*/
	typedef struct _EncodingDescription
	{
		const char * pcName;          /**< name of the encoding */
		char         bSmart;          /**< is it a smart codec? */
		char         bSendUtf8;       /**< does it send utf8 or the local charset? */
		uint         uGroup;          /**< group */
		const char * pcDescription;   /**< description of the encoding */
	} EncodingDescription;

	// you MUST start iterating from 0 and terminate when
	// you get an entry with a NULL szName
	KVILIB_API EncodingDescription * encodingDescription(int iIdx);
	KVILIB_API const char * encodingGroup(int iIdx);
	KVILIB_API QTextCodec * codecForName(const char * szName);
	KVILIB_API const KviCString & localeName();
	
	/**
	* \brief Finds the catalogue
	* 
	* This function attempts to determine the current locale and then load the
	* corresponding translation file from the KVIrc locale directory.
	* Returns true if the locale was correctly set; i.e. the locale is C or POSIX
	* (no translation needed) or the locale is correctly defined and the translation
	* map was sucesfully loaded
	* \param szBuffer 
	* \param szName 
	* \param szLocaleDir 
	* \return bool
	*/
	KVILIB_API bool findCatalogue(QString & szBuffer, const QString & szName, const QString & szLocaleDir);
	KVILIB_API KviMessageCatalogue * loadCatalogue(const QString & szName, const QString & szLocaleDir);
	KVILIB_API KviMessageCatalogue * getLoadedCatalogue(const QString & szName);
	KVILIB_API bool unloadCatalogue(const QString & szName);
	KVILIB_API void init(QApplication * pApp, const QString & szLocaleDir, const QString & szForceLocaleDir);
	KVILIB_API void done(QApplication * pApp);
	KVILIB_API const char * translate(const char * pcText, const char * pcContext);
	KVILIB_API const QString & translateToQString(const char * pcText, const char * pcContext);
}


#ifndef _KVI_LOCALE_CPP_
	extern KVILIB_API KviMessageCatalogue * g_pMainCatalogue;
#endif // !_KVI_LOCALE_CPP_

#define __tr(__text__) g_pMainCatalogue->translate(__text__)
#define __tr_no_lookup(__text__) __text__
#define __tr_no_xgettext(__text__) g_pMainCatalogue->translate(__text__)

#define __tr2qs(__text__) g_pMainCatalogue->translateToQString(__text__)
#define __tr2qs_no_xgettext(__text__) g_pMainCatalogue->translateToQString(__text__)

#define __tr_ctx(__text__,__context__) KviLocale::translate(__text__,__context__)
#define __tr_no_lookup_ctx(__text__,__context__) __text__
#define __tr_no_xgettext_ctx(__text__,__context__) KviLocale::translate(__text__,__context__)
#define __tr2qs_ctx(__text__,__context__) KviLocale::translateToQString(__text__,__context__)
#define __tr2qs_ctx_no_xgettext(__text__,__context__) KviLocale::translateToQString(__text__,__context__)
#define __tr2qs_no_lookup(__text__) __text__


#endif //_KVI_LOCALE_H_
