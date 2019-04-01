#ifndef _KVI_LOCALE_H_
#define _KVI_LOCALE_H_
//=============================================================================
//
//   File : KviLocale.h
//   Creation date : Sat Jan 16 1999 18:15:01 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 1999-2010 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2011 Elvio Basello (hellvis69 at gmail dot com)
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
* \file KviLocale.h
* \author Szymon Stefanek
* \brief KVIrc localization stuff
*/

#include "kvi_settings.h"
#include "KviHeapObject.h"
#include "KviMessageCatalogue.h"

class KviCString;
class QApplication;
class QString;
class QTextCodec;

/**
* \def KVI_NUM_ENCODINGS The number of charset encodings
* \def KVI_NUM_ENCODING_GROUPS The number of charset groups
*/
#ifndef QT_NO_BIG_CODECS
#define KVI_NUM_ENCODINGS 112
#define KVI_NUM_ENCODING_GROUPS 8
#else
#define KVI_NUM_ENCODINGS 85
#define KVI_NUM_ENCODING_GROUPS 5
#endif

/**
* \class KviLocale
* \brief Holds the localization functions
*/
class KVILIB_API KviLocale : public KviHeapObject
{
public:
	/**
	* \typedef EncodingDescription
	* \struct _EncodingDescription
	* \brief Holds the encoding data
	*/
	struct EncodingDescription
	{
		const char * pcName;        /**< name of the encoding */
		char bSmart;                /**< is it a smart codec? */
		char bSendUtf8;             /**< does it send utf8 or the local charset? */
		uint uGroup;                /**< group */
		const char * pcDescription; /**< description of the encoding */
	};

protected:
	/**
	* \brief Constructs the KviLocale object
	* \param pApp The main application
	* \param szLocaleDir The directory containing the localizations
	* \param szForceLocaleDir The directory forced by the user
	* \return KviLocale
	*/
	KviLocale(QApplication * pApp, const QString & szLocaleDir, const QString & szForceLocaleDir);

	/**
	* \brief Destroys the object
	* \return KviLocale
	*/
	~KviLocale();

public:
	static QString g_szLang;

protected:
	QApplication * m_pApp;

private:
	static KviLocale * m_pSelf;
	static unsigned int m_uCount;

public:
	/**
	* \brief Initializes the class instance
	* \param pApp The main application
	* \param szLocaleDir The directory containing the localizations
	* \param szForceLocaleDir The directory forced by the user
	* \return void
	*/
	static void init(QApplication * pApp, const QString & szLocaleDir, const QString & szForceLocaleDir);

	/**
	* \brief Destroys the class instance
	* \param pApp The main application
	* \return void
	*/
	static void done();

	/**
	* \brief Returns the instance of the class
	* \return KviLocale *
	*/
	static inline KviLocale * instance() { return m_pSelf; }

	/**
	* \brief Returns the number of instances of the class
	* \return unsigned int
	*/
	unsigned int count() { return m_uCount; }

	/**
	* \brief Returns the description of the encoding used
	* \param iIdx The index of the description
	* \warning You MUST start iterating from 0 and terminate when you get an entry with
	* a NULL pcName
	* \return EncodingDescription *
	*/
	EncodingDescription * encodingDescription(int iIdx);

	/**
	* \brief Returns the description of the encoding used
	* \param iIdx The index of the group
	* \warning You MUST start iterating from 0 and terminate when you get an entry with
	* a NULL value
	* \return const char *
	*/
	const char * encodingGroup(int iIdx);

	/**
	* \brief Returns the language code of the localization
	* \return const QString &
	*/
	const QString & localeName() const { return g_szLang; }

	/**
	* \brief Returns the codec associated to the given translation
	* \param pcName The name of the translation
	* \return QTextCodec *
	*/
	QTextCodec * codecForName(const char * pcName);

	/**
	* \brief Finds the catalogue
	*
	* This function attempts to determine the current locale and then load the
	* corresponding translation file from the KVIrc locale directory.
	* Returns true if the locale was correctly set; i.e. the locale is C or POSIX
	* (no translation needed) or the locale is correctly defined and the translation
	* map was successfully loaded
	* \param szBuffer The buffer where to store the translation full path
	* \param szName The name of the translation file
	* \param szLocaleDir The directory where the localizations are stored
	* \return bool
	*/
	bool findCatalogue(QString & szBuffer, const QString & szName, const QString & szLocaleDir);

	/**
	* \brief Loads the catalogue
	* \param szName The name of the catalogue
	* \param szLocaleDir The directory where to look for the catalogue
	* \return KviMessageCatalogue *
	*/
	KviMessageCatalogue * loadCatalogue(const QString & szName, const QString & szLocaleDir);

	/**
	* \brief Unloads a catalogue
	* \param szName The catalogue to unload
	* \return bool
	*/
	bool unloadCatalogue(const QString & szName);

	/**
	* \brief Returns the loaded catalogue
	* \param szName The name of the catalogue to get
	* \return KviMessageCatalogue *
	*/
	KviMessageCatalogue * getLoadedCatalogue(const QString & szName);

	/**
	* \brief Translates the given text from the given context
	* \param pcText The text to translate
	* \param pcContext The context where to look for the text
	* \return const char *
	*/
	const char * translate(const char * pcText, const char * pcContext);

	/**
	* \brief Translates the given text from the given context
	* \param pcText The text to translate
	* \param pcContext The context where to look for the text
	* \return const QString &
	*/
	const QString & translateToQString(const char * pcText, const char * pcContext);
};

#ifndef _KVI_LOCALE_CPP_
extern KVILIB_API KviMessageCatalogue * g_pMainCatalogue;
#endif // !_KVI_LOCALE_CPP_

#define __tr(text) g_pMainCatalogue->translate(text)
#define __tr_no_lookup(text) text
#define __tr_no_xgettext(text) g_pMainCatalogue->translate(text)
#define __tr2qs(text) g_pMainCatalogue->translateToQString(text)
#define __tr2qs_no_lookup(text) text
#define __tr2qs_no_xgettext(text) g_pMainCatalogue->translateToQString(text)

#define __tr_ctx(text, context) KviLocale::instance()->translate(text, context)
#define __tr_no_lookup_ctx(text, context) text
#define __tr_no_xgettext_ctx(text, context) KviLocale::instance()->translate(text, context)
#define __tr2qs_ctx(text, context) KviLocale::instance()->translateToQString(text, context)
#define __tr2qs_ctx_no_xgettext(text, context) KviLocale::instance()->translateToQString(text, context)

#endif //_KVI_LOCALE_H_
