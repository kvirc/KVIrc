//=============================================================================
//
//   File : KviLocale.cpp
//   Creation date : Fri Mar 19 1999 19:08:41 by Szymon Stefanek
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

#include "kvi_debug.h"
#include "kvi_defaults.h"
#include "KviMemory.h"
#include "KviByteOrder.h"

#define _KVI_LOCALE_CPP_
#include "KviLocale.h"
#include "KviCString.h"
#include "KviQString.h"
#include "KviEnvironment.h"
#include "KviFileUtils.h"
#include "KviFile.h"
#include "KviPointerHashTable.h"
#include "KviTranslator.h"

#include <QApplication>
#include <QByteArray>
#include <QDir>
#include <QLocale>
#include <QString>
#include <QTextCodec>
#include <QtGlobal>

KVILIB_API KviMessageCatalogue * g_pMainCatalogue = nullptr;

static KviTranslator * g_pTranslator = nullptr;
static KviPointerHashTable<const char *, KviMessageCatalogue> * g_pCatalogueDict = nullptr;
static QTextCodec * g_pUtf8TextCodec = nullptr;
static QString g_szDefaultLocalePath; // FIXME: Convert this to a search path list

//
// The following code was extracted and adapted from gutf8.c
// from the GNU GLIB2 package.
//
// gutf8.c - Operations on UTF-8 strings.
//
// Copyright (C) 1999 Tom Tromey
// Copyright (C) 2000 Red Hat, Inc.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the
// Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.
//

#define UNICODE_VALID(Char) \
	((Char) < 0x110000 && (((Char)&0xFFFFF800) != 0xD800) && ((Char) < 0xFDD0 || (Char) > 0xFDEF) && ((Char)&0xFFFE) != 0xFFFE)

#define CONTINUATION_CHAR                                   \
	if((*(unsigned char *)p & 0xc0) != 0x80) /* 10xxxxxx */ \
		goto error;                                         \
	val <<= 6;                                              \
	val |= (*(unsigned char *)p) & 0x3f;

static const char * fast_validate(const char * str)
{
	unsigned int val = 0;
	unsigned int min = 0;
	const char * p;

	for(p = str; *p; p++)
	{
		if(*(unsigned char *)p < 128)
			/* done */;
		else
		{
			const char * last;

			last = p;
			if((*(unsigned char *)p & 0xe0) == 0xc0) /* 110xxxxx */
			{
				if((*(unsigned char *)p & 0x1e) == 0)
					goto error;
				p++;
				if((*(unsigned char *)p & 0xc0) != 0x80) /* 10xxxxxx */
					goto error;
			}
			else
			{
				if((*(unsigned char *)p & 0xf0) == 0xe0) /* 1110xxxx */
				{
					min = (1 << 11);
					val = *(unsigned char *)p & 0x0f;
					goto TWO_REMAINING;
				}
				else if((*(unsigned char *)p & 0xf8) == 0xf0) /* 11110xxx */
				{
					min = (1 << 16);
					val = *(unsigned char *)p & 0x07;
				}
				else
					goto error;

				p++;
				CONTINUATION_CHAR;
			TWO_REMAINING:
				p++;
				CONTINUATION_CHAR;
				p++;
				CONTINUATION_CHAR;

				if(val < min)
					goto error;

				if(!UNICODE_VALID(val))
					goto error;
			}

			continue;

		error:
			return last;
		}
	}

	return p;
}

static const char * fast_validate_len(const char * str, int max_len)
{
	unsigned int val = 0;
	unsigned int min = 0;
	const char * p;

	for(p = str; (max_len < 0 || (p - str) < max_len) && *p; p++)
	{
		if(*(unsigned char *)p < 128)
			/* done */;
		else
		{
			const char * last;

			last = p;
			if((*(unsigned char *)p & 0xe0) == 0xc0) /* 110xxxxx */
			{
				if(max_len >= 0 && max_len - (p - str) < 2)
					goto error;

				if((*(unsigned char *)p & 0x1e) == 0)
					goto error;
				p++;
				if((*(unsigned char *)p & 0xc0) != 0x80) /* 10xxxxxx */
					goto error;
			}
			else
			{
				if((*(unsigned char *)p & 0xf0) == 0xe0) /* 1110xxxx */
				{
					if(max_len >= 0 && max_len - (p - str) < 3)
						goto error;

					min = (1 << 11);
					val = *(unsigned char *)p & 0x0f;
					goto TWO_REMAINING;
				}
				else if((*(unsigned char *)p & 0xf8) == 0xf0) /* 11110xxx */
				{
					if(max_len >= 0 && max_len - (p - str) < 4)
						goto error;

					min = (1 << 16);
					val = *(unsigned char *)p & 0x07;
				}
				else
					goto error;

				p++;
				CONTINUATION_CHAR;
			TWO_REMAINING:
				p++;
				CONTINUATION_CHAR;
				p++;
				CONTINUATION_CHAR;

				if(val < min)
					goto error;
				if(!UNICODE_VALID(val))
					goto error;
			}

			continue;

		error:
			return last;
		}
	}

	return p;
}

static bool g_utf8_validate(const char * str, int max_len, const char ** end)
{
	const char * p;

	if(max_len < 0)
		p = fast_validate(str);
	else
		p = fast_validate_len(str, max_len);

	if(end)
		*end = p;

	if((max_len >= 0 && p != str + max_len) || (max_len < 0 && *p != '\0'))
		return false;
	else
		return true;
}

//
//   End of gutf8.c
//

class KviSmartTextCodec : public QTextCodec
{
private:
	QByteArray m_szName;
	QTextCodec * m_pRecvCodec;
	QTextCodec * m_pSendCodec;

public:
	KviSmartTextCodec(const char * szName, QTextCodec * pChildCodec, bool bSendInUtf8)
	    : QTextCodec()
	{
		Q_ASSERT(pChildCodec);

		m_pRecvCodec = pChildCodec;
		m_szName = szName;

		if(!g_pUtf8TextCodec)
		{
			g_pUtf8TextCodec = QTextCodec::codecForName("UTF-8");
			if(!g_pUtf8TextCodec)
			{
				qDebug("Can't find the global UTF-8 text codec!");
				g_pUtf8TextCodec = QTextCodec::codecForLocale(); // try anything else...
				if(!g_pUtf8TextCodec)
					qDebug("Argh! Have no UTF-8 text codec: we're in trouble.");
			}
		}

		m_pSendCodec = bSendInUtf8 ? g_pUtf8TextCodec : pChildCodec;
	}

public:
	bool ok() { return m_pRecvCodec && g_pUtf8TextCodec; };

	int mibEnum() const override { return 0; };

	QByteArray name() const override { return m_szName; };
protected:
	QByteArray convertFromUnicode(const QChar * input, int number, ConverterState * state) const override
	{
		return m_pSendCodec->fromUnicode(input, number, state);
	}
	QString convertToUnicode(const char * chars, int len, ConverterState * state) const override
	{
		if(g_utf8_validate(chars, len, nullptr))
			return g_pUtf8TextCodec->toUnicode(chars, len, state);

		return m_pRecvCodec->toUnicode(chars, len, state);
	}
};

static KviPointerHashTable<const char *, KviSmartTextCodec> * g_pSmartCodecDict = nullptr;

static const char * encoding_groups[] = {
	"Unicode",
	"West European",
	"East European",
	"Cyrillic",
	"Middle Eastern",
#ifndef QT_NO_BIG_CODECS
	"Chinese",
	"Japanese",
	"Other Asiatic",
#endif
	nullptr
};

static KviLocale::EncodingDescription supported_encodings[] = {
	// clang-format off
	// Unicode
	{ "UTF-8"                , 0 , 0 , 0, "8-bit Unicode" },
	// West European
	{ "ISO-8859-1"           , 0 , 0 , 1, "Western, Latin-1" },
	{ "ISO-8859-15"          , 0 , 0 , 1, "Western, Latin-1 + Euro" },
	{ "IBM-850"              , 0 , 0 , 1, "IBM-850" },
	{ "CP-1252"              , 0 , 0 , 1, "Western Codepage" },
	{ "ISO-8859-14"          , 0 , 0 , 1, "Celtic" },
	{ "ISO-8859-7"           , 0 , 0 , 1, "Greek" },
	{ "CP-1253"              , 0 , 0 , 1, "Greek Codepage" },
	{ "ISO-8859-10"          , 0 , 0 , 1, "Nordic" },
	{ "ISO-8859-3"           , 0 , 0 , 1, "South European" },
	// East European
	{ "ISO-8859-4"           , 0 , 0 , 2, "Baltic, Standard" },
	{ "ISO-8859-13"          , 0 , 0 , 2, "Baltic" },
	{ "CP-1257"              , 0 , 0 , 2, "Baltic Codepage" },
	{ "ISO-8859-2"           , 0 , 0 , 2, "Central European" },
	{ "CP-1250"              , 0 , 0 , 2, "Central European Codepage" },
	{ "ISO-8859-9"           , 0 , 0 , 2, "Turkish, Latin-5" },
	{ "CP-1254"              , 0 , 0 , 2, "Turkish Codepage" },
	// Cyrillic
	{ "ISO-8859-5"           , 0 , 0 , 3, "Cyrillic, ISO" },
	{ "CP-1251"              , 0 , 0 , 3, "Cyrillic Codepage" },
	{ "KOI8-R"               , 0 , 0 , 3, "Cyrillic, KOI" },
	{ "KOI8-U"               , 0 , 0 , 3, "Cyrillic/Ukrainian" },
	{ "IBM-866"              , 0 , 0 , 3, "IBM-866" },
	// Middle Eastern
	{ "ISO-8859-6"           , 0 , 0 , 4, "Arabic, Standard" },
	{ "CP-1256"              , 0 , 0 , 4, "Arabic Codepage" },
	{ "ISO-8859-8"           , 0 , 0 , 4, "Hebrew, visually ordered" },
	{ "ISO-8859-8-i"         , 0 , 0 , 4, "Hebrew, logically ordered" },
	{ "CP-1255"              , 0 , 0 , 4, "Hebrew Codepage" },
	// Other Asiatic
	{ "TIS-620"              , 0 , 0 , 7, "Thai" },
	{ "CP874"                , 0 , 0 , 7, "Thai Codepage" },
#ifndef QT_NO_BIG_CODECS
	// Chinese
	{ "Big5"                 , 0 , 0 , 5, "Chinese Traditional" },
	{ "Big5-HKSCS"           , 0 , 0 , 5, "Chinese Traditional, Hong Kong" },
	{ "GB18030"              , 0 , 0 , 5, "Chinese Simplified" },
	// Japanese
	{ "ISO-2022-JP"          , 0 , 0 , 6, "Japanese (ISO-2022-JP/JIS7)" },
	{ "Shift-JIS"            , 0 , 0 , 6, "Japanese (Shift-JIS)" },
	{ "EUC-JP"               , 0 , 0 , 6, "Japanese (EUC-JP)" },
	// Other Asiatic
	{ "EUC-KR"               , 0 , 0 , 7, "Korean" },
	{ "TSCII"                , 0 , 0 , 7, "Tamil" },
	{ "CP-949"               , 0 , 0 , 7, "Korean Codepage" },
#endif

	// smart codecs that send in the local charset
	// West European
	{ "ISO-8859-1 [UTF-8]"   , 1 , 0 , 1, "Western, Latin-1 - Unicode" },
	{ "ISO-8859-15 [UTF-8]"  , 1 , 0 , 1, "Western, Latin-1 + Euro - Unicode" },
	{ "IBM-850 [UTF-8]"      , 1 , 0 , 1, "IBM-850 - Unicode" },
	{ "CP-1252 [UTF-8]"      , 1 , 0 , 1, "Western Codepage - Unicode" },
	{ "ISO-8859-14 [UTF-8]"  , 1 , 0 , 1, "Celtic - Unicode" },
	{ "ISO-8859-7 [UTF-8]"   , 1 , 0 , 1, "Greek - Unicode" },
	{ "CP-1253 [UTF-8]"      , 1 , 0 , 1, "Greek Codepage - Unicode" },
	{ "ISO-8859-10 [UTF-8]"  , 1 , 0 , 1, "Nordic - Unicode" },
	{ "ISO-8859-3 [UTF-8]"   , 1 , 0 , 1, "South European - Unicode" },
	// East European
	{ "ISO-8859-4 [UTF-8]"   , 1 , 0 , 2, "Baltic, Standard - Unicode" },
	{ "ISO-8859-13 [UTF-8]"  , 1 , 0 , 2, "Baltic - Unicode" },
	{ "CP-1257 [UTF-8]"      , 1 , 0 , 2, "Baltic Codepage - Unicode" },
	{ "ISO-8859-2 [UTF-8]"   , 1 , 0 , 2, "Central European - Unicode" },
	{ "CP-1250 [UTF-8]"      , 1 , 0 , 2, "Central European Codepage - Unicode" },
	{ "ISO-8859-9 [UTF-8]"   , 1 , 0 , 2, "Turkish, Latin-5 - Unicode" },
	{ "CP-1254 [UTF-8]"      , 1 , 0 , 2, "Turkish Codepage - Unicode" },
	// Cyrillic
	{ "ISO-8859-5 [UTF-8]"   , 1 , 0 , 3, "Cyrillic, ISO - Unicode" },
	{ "CP-1251 [UTF-8]"      , 1 , 0 , 3, "Cyrillic Codepage - Unicode" },
	{ "KOI8-R [UTF-8]"       , 1 , 0 , 3, "Cyrillic, KOI - Unicode" },
	{ "KOI8-U [UTF-8]"       , 1 , 0 , 3, "Cyrillic/Ukrainian - Unicode" },
	{ "IBM-866 [UTF-8]"      , 1 , 0 , 3, "IBM-866 - Unicode" },
	// Middle Eastern
	{ "ISO-8859-6 [UTF-8]"   , 1 , 0 , 4, "Arabic, Standard - Unicode" },
	{ "CP-1256 [UTF-8]"      , 1 , 0 , 4, "Arabic Codepage - Unicode" },
	{ "ISO-8859-8 [UTF-8]"   , 1 , 0 , 4, "Hebrew, visually ordered - Unicode" },
	{ "ISO-8859-8-i [UTF-8]" , 1 , 0 , 4, "Hebrew, logically ordered - Unicode" },
	{ "CP-1255 [UTF-8]"      , 1 , 0 , 4, "Hebrew Codepage - Unicode" },
	// Other Asiatic
	{ "TIS-620 [UTF-8]"      , 1 , 0 , 7, "Thai - Unicode" },
	{ "CP874 [UTF-8]"        , 1 , 0 , 7, "Thai Codepage - Unicode" },
#ifndef QT_NO_BIG_CODECS
	// Chinese
	{ "Big5 [UTF-8]"         , 1 , 0 , 5, "Chinese Traditional - Unicode" },
	{ "Big5-HKSCS [UTF-8]"   , 1 , 0 , 5, "Chinese Traditional, Hong Kong - Unicode" },
	{ "GB18030 [UTF-8]"      , 1 , 0 , 5, "Chinese Simplified - Unicode" },
	// Japanese
	{ "ISO-2022-JP [UTF-8]"  , 1 , 0 , 6, "Japanese (ISO-2022-JP/JIS7) - Unicode" },
	{ "Shift-JIS [UTF-8]"    , 1 , 0 , 6, "Japanese (Shift-JIS) - Unicode" },
	{ "EUC-JP [UTF-8]"       , 1 , 0 , 6, "Japanese (EUC-JP) - Unicode" },
	// Other Asiatic
	{ "EUC-KR [UTF-8]"       , 1 , 0 , 7, "Korean - Unicode" },
	{ "TSCII [UTF-8]"        , 1 , 0 , 7, "Tamil - Unicode" },
	{ "CP-949 [UTF-8]"       , 1 , 0 , 7, "Korean Codepage - Unicode" },
#endif

	// smart codecs that send in utf8
	// West European
	{ "UTF-8 [ISO-8859-1]"   , 1 , 1 , 1, "Unicode - Western, Latin-1" },
	{ "UTF-8 [ISO-8859-15]"  , 1 , 1 , 1, "Unicode - Western, Latin-1 + Euro" },
	{ "UTF-8 [IBM-850]"      , 1 , 1 , 1, "Unicode - IBM-850" },
	{ "UTF-8 [CP-1252]"      , 1 , 1 , 1, "Unicode - Western Codepage" },
	{ "UTF-8 [ISO-8859-14]"  , 1 , 1 , 1, "Unicode - Celtic" },
	{ "UTF-8 [ISO-8859-7]"   , 1 , 1 , 1, "Unicode - Greek" },
	{ "UTF-8 [CP-1253]"      , 1 , 1 , 1, "Unicode - Greek Codepage" },
	{ "UTF-8 [ISO-8859-10]"  , 1 , 1 , 1, "Unicode - Nordic" },
	{ "UTF-8 [ISO-8859-3]"   , 1 , 1 , 1, "Unicode - South European" },
	// East European
	{ "UTF-8 [ISO-8859-4]"   , 1 , 1 , 2, "Unicode - Baltic, Standard" },
	{ "UTF-8 [ISO-8859-13]"  , 1 , 1 , 2, "Unicode - Baltic" },
	{ "UTF-8 [CP-1257]"      , 1 , 1 , 2, "Unicode - Baltic Codepage" },
	{ "UTF-8 [ISO-8859-2]"   , 1 , 1 , 2, "Unicode - Central European" },
	{ "UTF-8 [CP-1250]"      , 1 , 1 , 2, "Unicode - Central European Codepage" },
	{ "UTF-8 [ISO-8859-9]"   , 1 , 1 , 2, "Unicode - Turkish, Latin-5" },
	{ "UTF-8 [CP-1254]"      , 1 , 1 , 2, "Unicode - Turkish Codepage" },
	// Cyrillic
	{ "UTF-8 [ISO-8859-5]"   , 1 , 1 , 3, "Unicode - Cyrillic, ISO" },
	{ "UTF-8 [CP-1251]"      , 1 , 1 , 3, "Unicode - Cyrillic Codepage" },
	{ "UTF-8 [KOI8-R]"       , 1 , 1 , 3, "Unicode - Cyrillic, KOI" },
	{ "UTF-8 [KOI8-U]"       , 1 , 1 , 3, "Unicode - Cyrillic/Ukrainian" },
	{ "UTF-8 [IBM-866]"      , 1 , 1 , 3, "Unicode - IBM-866" },
	// Middle Eastern
	{ "UTF-8 [ISO-8859-6]"   , 1 , 1 , 4, "Unicode - Arabic, Standard" },
	{ "UTF-8 [CP-1256]"      , 1 , 1 , 4, "Unicode - Arabic Codepage" },
	{ "UTF-8 [ISO-8859-8]"   , 1 , 1 , 4, "Unicode - Hebrew, visually ordered" },
	{ "UTF-8 [ISO-8859-8-i]" , 1 , 1 , 4, "Unicode - Hebrew, logically ordered" },
	{ "UTF-8 [CP-1255]"      , 1 , 1 , 4, "Unicode - Hebrew Codepage" },
	// Other Asiatic
	{ "UTF-8 [TIS-620]"      , 1 , 1 , 7, "Unicode - Thai" },
	{ "UTF-8 [CP874]"        , 1 , 1 , 7, "Unicode - Thai Codepage" },
#ifndef QT_NO_BIG_CODECS
	// Chinese
	{ "UTF-8 [Big5]"         , 1 , 1 , 5, "Unicode - Chinese Traditional" },
	{ "UTF-8 [Big5-HKSCS]"   , 1 , 1 , 5, "Unicode - Chinese Traditional, Hong Kong" },
	{ "UTF-8 [GB18030]"      , 1 , 1 , 5, "Unicode - Chinese Simplified" },
	// Japanese
	{ "UTF-8 [ISO-2022-JP]"  , 1 , 1 , 6, "Unicode - Japanese (ISO-2022-JP/JIS7)" },
	{ "UTF-8 [Shift-JIS]"    , 1 , 1 , 6, "Unicode - Japanese (Shift-JIS)" },
	{ "UTF-8 [EUC-JP]"       , 1 , 1 , 6, "Unicode - Japanese (EUC-JP)" },
	// Other Asiatic
	{ "UTF-8 [EUC-KR]"       , 1 , 1 , 7, "Unicode - Korean" },
	{ "UTF-8 [TSCII]"        , 1 , 1 , 7, "Unicode - Tamil" },
	{ "UTF-8 [CP-949]"       , 1 , 1 , 7, "Unicode - Korean Codepage" },
#endif
	{ nullptr                      , 0 , 0 , 0 , nullptr }
	// clang-format on
};

KviLocale * KviLocale::m_pSelf = nullptr;
unsigned int KviLocale::m_uCount = 0;
QString KviLocale::g_szLang = "";

KviLocale::KviLocale(QApplication * pApp, const QString & szLocaleDir, const QString & szForceLocaleDir)
{
	m_pApp = pApp;

	// first of all try to find out the current locale
	QString szLangFile = QString("%1/%2").arg(szForceLocaleDir, KVI_FORCE_LOCALE_FILE_NAME);

	if(KviFileUtils::fileExists(szLangFile))
		KviFileUtils::readFile(szLangFile, g_szLang);
	if(g_szLang.isEmpty())
		g_szLang = KviEnvironment::getVariable("KVIRC_LANG");
#ifdef COMPILE_KDE_SUPPORT
	if(g_szLang.isEmpty())
		g_szLang = KviEnvironment::getVariable("KDE_LANG");
#endif //COMPILE_KDE_SUPPORT
	if(g_szLang.isEmpty())
		g_szLang = KviEnvironment::getVariable("LC_MESSAGES");
	if(g_szLang.isEmpty())
		g_szLang = KviEnvironment::getVariable("LANG");
	if(g_szLang.isEmpty())
		g_szLang = QLocale::system().name();
	if(g_szLang.isEmpty())
		g_szLang = "en";
	g_szLang = g_szLang.trimmed();

	g_szDefaultLocalePath = szLocaleDir;

	// the main catalogue is supposed to be kvirc_<language>.mo
	g_pMainCatalogue = new KviMessageCatalogue();
	// the catalogue dict
	g_pCatalogueDict = new KviPointerHashTable<const char *, KviMessageCatalogue>;
	g_pCatalogueDict->setAutoDelete(true);

	// the smart codec dict
	g_pSmartCodecDict = new KviPointerHashTable<const char *, KviSmartTextCodec>;
	// the Qt docs explicitly state that we shouldn't delete
	// the codecs by ourselves...
	g_pSmartCodecDict->setAutoDelete(false);

	if(!g_szLang.isEmpty())
	{
		// ensure Qt will use the right locale when formatting dates, numbers, ..
		// Note: Qt will use the system() locale anyway, we need to construct an empty QLocale()
		// to get it use our specified locale.
		QLocale::setDefault(QLocale(g_szLang));

		QString szBuffer;
		if(findCatalogue(szBuffer, "kvirc", szLocaleDir))
		{
			g_pMainCatalogue->load(szBuffer);
			g_pTranslator = new KviTranslator(m_pApp);
			m_pApp->installTranslator(g_pTranslator);
		}
		else
		{
			KviCString szTmp = g_szLang;
			szTmp.cutFromFirst('.');
			szTmp.cutFromFirst('_');
			szTmp.cutFromFirst('@');
			szTmp.toLower();
			if(!(kvi_strEqualCI(szTmp.ptr(), "en") || kvi_strEqualCI(szTmp.ptr(), "c") || kvi_strEqualCI(szTmp.ptr(), "us") || kvi_strEqualCI(szTmp.ptr(), "gb") || kvi_strEqualCI(szTmp.ptr(), "posix")))
			{
				// FIXME: THIS IS NO LONGER VALID!!!
				qDebug("Can't find the catalogue for locale \"%s\" (%s)", g_szLang.toUtf8().data(), szTmp.ptr());
				qDebug("There is no such translation or the $LANG variable was incorrectly set");
				qDebug("You can use $KVIRC_LANG to override the catalogue name");
				qDebug("For example you can set KVIRC_LANG to it_IT to force usage of the it.mo catalogue");
			}
		}
	}
}

KviLocale::~KviLocale()
{
	delete g_pMainCatalogue;
	delete g_pCatalogueDict;
	delete g_pSmartCodecDict;
	g_pMainCatalogue = nullptr;
	g_pCatalogueDict = nullptr;
	g_pSmartCodecDict = nullptr;
	if(g_pTranslator)
	{
		m_pApp->removeTranslator(g_pTranslator);
		delete g_pTranslator;
		g_pTranslator = nullptr;
	}
}

void KviLocale::init(QApplication * pApp, const QString & szLocaleDir, const QString & szForceLocaleDir)
{
	if((!m_pSelf) && (m_pSelf->count() == 0))
	{
		m_pSelf = new KviLocale(pApp, szLocaleDir, szForceLocaleDir);
		m_uCount++;
	}
}

void KviLocale::done()
{
	m_uCount--;
	if(m_pSelf->count() == 0)
		delete m_pSelf;
}

QTextCodec * KviLocale::codecForName(const char * pcName)
{
	KviCString szTmp = pcName;
	bool bSendUtf8;

	int iIdx = szTmp.findFirstIdx('[');
	if(iIdx != -1)
	{
		// Might be a composite codec: either UTF-8 [child codec] or child codec [UTF-8]
		KviSmartTextCodec * pCodec = g_pSmartCodecDict->find(pcName);
		if(pCodec)
			return pCodec; // got cached copy

		if(kvi_strEqualCIN("UTF-8 [", pcName, 7))
		{
			// Likely a smart codec that sends UTF-8
			szTmp.replaceAll("UTF-8 [", "");
			szTmp.replaceAll("]", "");
			bSendUtf8 = true;
		}
		else
		{
			// Likely a smart codec that sends child encoding ?
			szTmp.cutFromFirst(' ');
			bSendUtf8 = false;
		}

		QTextCodec * pChildCodec = QTextCodec::codecForName(szTmp.ptr());
		if(pChildCodec)
		{
			pCodec = new KviSmartTextCodec(pcName, pChildCodec, bSendUtf8);

			if(pCodec->ok())
			{
				g_pSmartCodecDict->replace(pcName, pCodec);
				return pCodec;
			}

			delete pCodec;
		}
		else
		{
			// The name of the child codec was invalid: can't create such a smart codec.
			// We probably screwed up the guess above related to the [ char.
			// This code path is also triggered by the yircfuzzer by specifying completely invalid codec names.
		}
	}

	return QTextCodec::codecForName(pcName);
}

bool KviLocale::findCatalogue(QString & szBuffer, const QString & szName, const QString & szLocaleDir)
{
	KviCString szLocale = g_szLang;

	QString szLocDir = szLocaleDir;
	KviQString::ensureLastCharIs(szLocDir, KVI_PATH_SEPARATOR_CHAR);

	szBuffer = QString("%1%2_%3.mo").arg(szLocDir, szName).arg(szLocale.ptr());

	if(KviFileUtils::fileExists(szBuffer))
		return true;

	if(szLocale.findFirstIdx('.') != -1)
	{
		// things like en_GB.utf8
		// kill them
		szLocale.cutFromFirst('.');

		szBuffer = QString("%1%2_%3.mo").arg(szLocDir, szName).arg(szLocale.ptr());
		if(KviFileUtils::fileExists(szBuffer))
			return true;
	}

	if(szLocale.findFirstIdx('@') != -1)
	{
		// things like @euro ?
		// kill them
		szLocale.cutFromFirst('@');
		szBuffer = QString("%1%2_%3.mo").arg(szLocDir, szName).arg(szLocale.ptr());
		if(KviFileUtils::fileExists(szBuffer))
			return true;
	}

	if(szLocale.findFirstIdx('_') != -1)
	{
		// things like en_GB
		// kill them
		szLocale.cutFromFirst('_');
		szBuffer = QString("%1%2_%3.mo").arg(szLocDir, szName).arg(szLocale.ptr());
		if(KviFileUtils::fileExists(szBuffer))
			return true;
	}

	// try the lower case version too
	szLocale.toLower();
	szBuffer = QString("%1%2_%3.mo").arg(szLocDir, szName).arg(szLocale.ptr());
	if(KviFileUtils::fileExists(szBuffer))
		return true;

	return false;
}

KviMessageCatalogue * KviLocale::loadCatalogue(const QString & szName, const QString & szLocaleDir)
{
	//qDebug("Looking up catalogue %s",szName.toUtf8().data());
	QString szBuffer;

	KviMessageCatalogue * pCatalogue = g_pCatalogueDict->find(szName.toUtf8().data());
	if(pCatalogue)
		return pCatalogue; // already loaded

	if(!findCatalogue(szBuffer, szName, szLocaleDir))
		return nullptr;

	pCatalogue = new KviMessageCatalogue();
	if(pCatalogue->load(szBuffer))
	{
		g_pCatalogueDict->insert(szName.toUtf8().data(), pCatalogue);
		return pCatalogue;
	}
	delete pCatalogue;
	return nullptr;
}

bool KviLocale::unloadCatalogue(const QString & szName)
{
	//qDebug("Unloading catalogue: %s",szName.toUtf8().data());
	return g_pCatalogueDict->remove(szName.toUtf8().data());
}

KviMessageCatalogue * KviLocale::getLoadedCatalogue(const QString & szName)
{
	return g_pCatalogueDict->find(szName.toUtf8().data());
}

const char * KviLocale::encodingGroup(int iIdx)
{
	if(iIdx > KVI_NUM_ENCODING_GROUPS)
		return encoding_groups[KVI_NUM_ENCODING_GROUPS];
	return encoding_groups[iIdx];
}

KviLocale::EncodingDescription * KviLocale::encodingDescription(int iIdx)
{
	if(iIdx > KVI_NUM_ENCODINGS)
		return &(supported_encodings[KVI_NUM_ENCODINGS]);
	return &(supported_encodings[iIdx]);
}

const char * KviLocale::translate(const char * pcText, const char * pcContext)
{
	if(!pcContext)
		return g_pMainCatalogue->translate(pcText);

	KviMessageCatalogue * pCatalogue = g_pCatalogueDict->find(pcContext);
	if(!pCatalogue)
	{
		pCatalogue = loadCatalogue(QString::fromUtf8(pcContext), g_szDefaultLocalePath);
		if(!pCatalogue)
		{
			// Fake it....
			pCatalogue = new KviMessageCatalogue();
			g_pCatalogueDict->insert(pcContext, pCatalogue);
		}
	}
	return pCatalogue->translate(pcText);
}

const QString & KviLocale::translateToQString(const char * pcText, const char * pcContext)
{
	if(!pcContext)
		return g_pMainCatalogue->translateToQString(pcText);

	KviMessageCatalogue * pCatalogue = g_pCatalogueDict->find(pcContext);
	if(!pCatalogue)
	{
		pCatalogue = loadCatalogue(QString::fromUtf8(pcContext), g_szDefaultLocalePath);
		if(!pCatalogue)
		{
			// Fake it....
			pCatalogue = new KviMessageCatalogue();
			g_pCatalogueDict->insert(pcContext, pCatalogue);
		}
	}
	return pCatalogue->translateToQString(pcText);
}
