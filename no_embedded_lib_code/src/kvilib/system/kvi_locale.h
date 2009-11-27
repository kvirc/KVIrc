#ifndef _KVI_LOCALE_H_
#define _KVI_LOCALE_H_

//=============================================================================
//
//   File : kvi_locale.h
//   Creation date : Sat Jan 16 1999 18:15:01 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2009 Szymon Stefanek (pragma at kvirc dot net)
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
#include "kvi_qstring.h"
#include "kvi_string.h"
#include "kvi_pointerhashtable.h"

#include <QApplication>

class QTextCodec;
class KviMessageCatalogue;

namespace KviLocale
{
	typedef struct _EncodingDescription
	{
		const char * szName;
		char   bSmart; // is it a smart codec ?
		char   bSendUtf8; // does it send utf8 or the local charset ?
		const char * szDescription;
	} EncodingDescription;

	// you MUST start iterating from 0 and terminate when
	// you get an entry with a NULL szName
	KVILIB_API EncodingDescription * encodingDescription(int iIdx);
	KVILIB_API QTextCodec * codecForName(const char * szName);
	KVILIB_API const KviStr & localeName();
	KVILIB_API bool findCatalogue(QString &szBuffer,const QString& name,const QString& szLocaleDir);
	KVILIB_API bool loadCatalogue(const QString& name,const QString& szLocaleDir);
	KVILIB_API KviMessageCatalogue * getLoadedCatalogue(const QString& name);
	KVILIB_API bool unloadCatalogue(const QString& name);
	KVILIB_API void init(QApplication * app,const QString& localeDir);
	KVILIB_API void done(QApplication * app);
	KVILIB_API const char * translate(const char * text,const char * context);
	KVILIB_API const QString & translateToQString(const char * text,const char * context);
}

// not exported
class KviTranslationEntry
{
public:
	KviStr    m_szKey;
	KviStr    m_szEncodedTranslation;
	QString * m_pQTranslation;
public:
	KviTranslationEntry(char * keyptr,int keylen,char * trptr,int trlen)
	: m_szKey(keyptr,keylen) , m_szEncodedTranslation(trptr,trlen)
	{
		m_pQTranslation = 0;
	}

	KviTranslationEntry(const char * keyandtr)
	: m_szKey(keyandtr) , m_szEncodedTranslation(keyandtr)
	{
		m_pQTranslation = 0;
	}

	~KviTranslationEntry()
	{
		if(m_pQTranslation)delete m_pQTranslation;
	}
};


class KVILIB_API KviMessageCatalogue
{
public:
	KviMessageCatalogue();
	~KviMessageCatalogue();
protected:
	//KviPointerHashTable<const char *,KviTranslationEntry> * m_pMessages;
	KviPointerHashTable<const char *,KviTranslationEntry> * m_pMessages;
	QTextCodec                                            * m_pTextCodec;
public:
	bool load(const QString& name);
	const char * translate(const char * text);
	const QString & translateToQString(const char * text);
};

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

#include <QTranslator>
#include <QString>

class KVILIB_API KviTranslator : public QTranslator
{
	Q_OBJECT
	public:
		KviTranslator(QObject * parent,const char * name);
		~KviTranslator();
	public:
	virtual QString translate(const char * context,const char * message,const char * comment) const;
};


#endif //!_KVI_LOCALE_H_
