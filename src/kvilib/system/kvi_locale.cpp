//=============================================================================
//
//   File : kvi_locale.cpp
//   Creation date : Fri Mar 19 1999 19:08:41 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2008 Szymon Stefanek (pragma at kvirc dot net)
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



//#define _KVI_DEBUG_CHECK_RANGE_
#include "kvi_debug.h"
#include "kvi_malloc.h"
#include "kvi_bswap.h"

#define _KVI_LOCALE_CPP_
#include "kvi_locale.h"

#include <QtGlobal>
#include <QTextCodec>
#include <QDir>
#include <QLocale>
#include <QByteArray>

#include "kvi_string.h"
#include "kvi_env.h"
#include "kvi_fileutils.h"
#include "kvi_file.h"


KVILIB_API KviMessageCatalogue           * g_pMainCatalogue       = 0;

static KviStr                              g_szLang;
static KviTranslator                     * g_pTranslator          = 0;
static KviPointerHashTable<const char *,KviMessageCatalogue> * g_pCatalogueDict       = 0;
static QTextCodec                        * g_pUtf8TextCodec       = 0;


/////////////////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////////////

typedef char gchar;
typedef unsigned char guchar;
typedef signed int gssize;
typedef unsigned int gunichar;

#define UNICODE_VALID(Char)                  \
	((Char) < 0x110000 &&                    \
	(((Char) & 0xFFFFF800) != 0xD800) &&     \
	((Char) < 0xFDD0 || (Char) > 0xFDEF) &&  \
	((Char) & 0xFFFE) != 0xFFFE)

#define CONTINUATION_CHAR                            \
	if ((*(guchar *)p & 0xc0) != 0x80) /* 10xxxxxx */ \
		goto error;                                     \
		val <<= 6;                                      \
		val |= (*(guchar *)p) & 0x3f;


static const char *
fast_validate (const char *str)
{
	gunichar val = 0;
	gunichar min = 0;
	const gchar *p;

	for (p = str; *p; p++)
	{
		if (*(guchar *)p < 128)
			/* done */;
		else
		{
			const gchar *last;

			last = p;
			if ((*(guchar *)p & 0xe0) == 0xc0) /* 110xxxxx */
			{
				if ((*(guchar *)p & 0x1e) == 0)
					goto error;
				p++;
				if ((*(guchar *)p & 0xc0) != 0x80) /* 10xxxxxx */
					goto error;
			}
			else
			{
				if ((*(guchar *)p & 0xf0) == 0xe0) /* 1110xxxx */
				{
					min = (1 << 11);
					val = *(guchar *)p & 0x0f;
					goto TWO_REMAINING;
				}
				else if ((*(guchar *)p & 0xf8) == 0xf0) /* 11110xxx */
				{
					min = (1 << 16);
					val = *(guchar *)p & 0x07;
				}
				else goto error;

				p++;
				CONTINUATION_CHAR;
				TWO_REMAINING:
				p++;
				CONTINUATION_CHAR;
				p++;
				CONTINUATION_CHAR;

				if (val < min) goto error;

				if (!UNICODE_VALID(val)) goto error;
			}

			continue;

			error:
			return last;
		}
	}

	return p;
}

static const gchar *
fast_validate_len (const char *str, gssize max_len)
{
	gunichar val = 0;
	gunichar min = 0;
	const gchar *p;

	for (p = str; (max_len < 0 || (p - str) < max_len) && *p; p++)
	{
		if (*(guchar *)p < 128)
			/* done */;
		else
		{
			const gchar *last;

			last = p;
			if ((*(guchar *)p & 0xe0) == 0xc0) /* 110xxxxx */
			{
			if (max_len >= 0 && max_len - (p - str) < 2)
				goto error;

			if ((*(guchar *)p & 0x1e) == 0)
				goto error;
			p++;
			if ((*(guchar *)p & 0xc0) != 0x80) /* 10xxxxxx */
				goto error;
			}
			else
			{
				if ((*(guchar *)p & 0xf0) == 0xe0) /* 1110xxxx */
				{
					if (max_len >= 0 && max_len - (p - str) < 3)
					goto error;

					min = (1 << 11);
					val = *(guchar *)p & 0x0f;
					goto TWO_REMAINING;
				}
				else if ((*(guchar *)p & 0xf8) == 0xf0) /* 11110xxx */
				{
					if (max_len >= 0 && max_len - (p - str) < 4)
					goto error;

					min = (1 << 16);
					val = *(guchar *)p & 0x07;
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

				if (val < min) goto error;
				if (!UNICODE_VALID(val)) goto error;
			}

			continue;

			error:
			return last;
		}
	}

	return p;
}

static bool g_utf8_validate (const char   *str,
                                gssize        max_len,
                                const gchar **end)
{
	const gchar *p;

	if (max_len < 0)
		p = fast_validate (str);
	else
		p = fast_validate_len (str, max_len);

	if (end) *end = p;

	if ((max_len >= 0 && p != str + max_len) ||
	(max_len < 0 && *p != '\0'))
		return false;
	else
		return true;
}

/////////////////////////////////////////////////////////////////////////////////////
//   End of gutf8.c
/////////////////////////////////////////////////////////////////////////////////////


class KviSmartTextCodec : public QTextCodec
{
protected:
	QByteArray  m_szName;
	QTextCodec * m_pRecvCodec;
	QTextCodec * m_pSendCodec;
public:
	KviSmartTextCodec(const char * szName,const char * szChildCodecName,bool bSendInUtf8)
	: QTextCodec()
	{
		m_szName = szName;
		if(!g_pUtf8TextCodec)
		{
			g_pUtf8TextCodec = QTextCodec::codecForName("UTF-8");
			if(!g_pUtf8TextCodec)
			{
				debug("Can't find the global utf8 text codec!");
				g_pUtf8TextCodec = QTextCodec::codecForLocale(); // try anything else...
			}
		}
		m_pRecvCodec = QTextCodec::codecForName(szChildCodecName);
		if(!m_pRecvCodec)
		{
			debug("Can't find the codec for name %s (composite codec creation)",szName);
			m_pRecvCodec = g_pUtf8TextCodec;
		}
		if(bSendInUtf8)
			m_pSendCodec = g_pUtf8TextCodec;
		else
			m_pSendCodec = m_pRecvCodec;
	}
public:
	bool ok(){ return m_pRecvCodec && g_pUtf8TextCodec; };

	virtual int mibEnum () const { return 0; };

	virtual QByteArray name() const { return m_szName; };
protected:
	virtual QByteArray convertFromUnicode(const QChar * input,int number,ConverterState * state) const
	{
		return m_pSendCodec->fromUnicode(input,number,state);
	}
	virtual QString convertToUnicode(const char * chars,int len,ConverterState * state) const
	{
		if(g_utf8_validate(chars,len,NULL))return g_pUtf8TextCodec->toUnicode(chars,len,state);
		return m_pRecvCodec->toUnicode(chars,len,state);
	}
};

static KviPointerHashTable<const char *,KviSmartTextCodec>   * g_pSmartCodecDict      = 0;



/////////////////////////////////////////////////////////////////////////////////////
//
//   The following code was extracted and adapted from gettext.h and gettextP.h
//   from the GNU gettext package.
//
//   Internal header for GNU gettext internationalization functions.
//   Copyright (C) 1995, 1997 Free Software Foundation, Inc.
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2, or (at your option)
//   any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU Library General Public
//   License along with the GNU C Library; see the file COPYING.LIB.  If not,
//   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
//   Boston, MA 02111-1307, USA.
//
/////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>

/*
#if HAVE_LIMITS_H || _LIBC
	#include <limits.h>
#endif
*/

// The magic number of the GNU message catalog format.
#define KVI_LOCALE_MAGIC 0x950412de
#define KVI_LOCALE_MAGIC_SWAPPED 0xde120495

// Revision number of the currently used .mo (binary) file format.
#define MO_REVISION_NUMBER 0


// Header for binary .mo file format.
struct GnuMoFileHeader
{
	// The magic number.
	kvi_u32_t magic;
	// The revision number of the file format.
	kvi_u32_t revision;
	// The number of strings pairs.
	kvi_u32_t nstrings;
	// Offset of table with start offsets of original strings.
	kvi_u32_t orig_tab_offset;
	// Offset of table with start offsets of translation strings.
	kvi_u32_t trans_tab_offset;
	// Size of hashing table.
	kvi_u32_t hash_tab_size;
	// Offset of first hashing entry.
	kvi_u32_t hash_tab_offset;
};

struct GnuMoStringDescriptor
{
	// Length of addressed string.
	kvi_u32_t length;
	// Offset of string in file.
	kvi_u32_t offset;
};

#define KVI_SWAP_IF_NEEDED(flag,value) (flag ? kvi_swap32(value) : (value))

////////////////////////////////////////////////////////////////////////////////////////   End of gettext.h & gettextP.h
//////////////////////////////////////////////////////////////////////////////////////

// HELPERS

static int somePrimeNumbers[90]=
{
	257 , 521 , 769 , 1031, 1087, 1091, 1103, 1117, 1123, 1151, // Incomplete *.mo files
	1163, 1171, 1181, 1193, 1201, 1213, 1217, 1223, 1229, 1231, // Complete *.mo files
	1237, 1249, 1259, 1277, 1283, 1289, 1291, 1297, 1307, 1319,
	1321, 1327, 1361, 1367, 1373, 1381, 1399, 1409, 1423, 1433,
	1447, 1459, 1471, 1481, 1493, 1511, 1523, 1531, 1543, 1553,
	1567, 1571, 1583, 1597, 1609, 1619, 1627, 1637, 1657, 1667, // Too big for KVIrc *.mo files
	1693, 1709, 1721, 1733, 1741, 1753, 1777, 1789, 1811, 1831,
	1907, 2069, 2111, 2221, 2309, 2441, 2531, 2617, 2731, 2837,
	2903, 3121, 3329, 3331, 3767, 4127, 5051, 6089, 7039, 9973
};

int kvi_getFirstBiggerPrime(int number)
{
	for(int i=0;i<90;i++){
		if(somePrimeNumbers[i] >= number)return somePrimeNumbers[i];
	}
	return 9973; //error!
}


KviMessageCatalogue::KviMessageCatalogue()
{
	//m_uEncoding = 0;
	m_pTextCodec = QTextCodec::codecForLocale();

	//m_pMessages = new KviPointerHashTable<const char *,KviTranslationEntry>(1123,true,false); // dictSize, case sensitive , don't copy keys
	m_pMessages = new KviPointerHashTable<const char *,KviTranslationEntry>(32,true,false); // dictSize, case sensitive , don't copy keys
	m_pMessages->setAutoDelete(true);
}

KviMessageCatalogue::~KviMessageCatalogue()
{
	if(m_pMessages)
		delete m_pMessages;
}

bool KviMessageCatalogue::load(const QString& name)
{
	QString szCatalogueFile(name);

	// Try to load the header
	KviFile f(szCatalogueFile);
	if(!f.open(QFile::ReadOnly))
	{
		debug("[KviLocale]: Failed to open the messages file %s: probably doesn't exist",KviQString::toUtf8(szCatalogueFile).data());
		return false;
	}

	GnuMoFileHeader hdr;

	if(f.read((char *)&hdr,sizeof(GnuMoFileHeader)) < (int)sizeof(GnuMoFileHeader))
	{
		debug("KviLocale: Failed to read header of %s",KviQString::toUtf8(szCatalogueFile).data());
		f.close();
		return false;
	}

	bool bMustSwap = false;

	if(hdr.magic != KVI_LOCALE_MAGIC)
	{
		if(hdr.magic == KVI_LOCALE_MAGIC_SWAPPED)
		{
			debug("KviLocale: Swapped magic for file %s: swapping data too",KviQString::toUtf8(szCatalogueFile).data());
			bMustSwap = true;
		} else {
			debug("KviLocale: Bad locale magic for file %s: not a *.mo file ?",KviQString::toUtf8(szCatalogueFile).data());
			f.close();
			return false;
		}
	}

	if(KVI_SWAP_IF_NEEDED(bMustSwap,hdr.revision) != MO_REVISION_NUMBER)
	{
		debug("KviLocale: Invalid *.mo file revision number for file %s",KviQString::toUtf8(szCatalogueFile).data());
		f.close();
		return false;
	}

	int numberOfStrings = KVI_SWAP_IF_NEEDED(bMustSwap,hdr.nstrings);

	if(numberOfStrings <= 0)
	{
		debug("KviLocale: No translated messages found in file %s",KviQString::toUtf8(szCatalogueFile).data());
		f.close();
		return false;
	}

	if(numberOfStrings >= 9972)
	{
		debug("Number of strings too big...sure that it is a KVIrc catalog file ?");
		numberOfStrings = 9972;
	}

	// return back
	f.seek(0);

	unsigned int fSize = f.size();
	char * buffer = (char *)kvi_malloc(fSize);

	// FIXME: maybe read it in blocks eh ?
	if(f.read(buffer,fSize) < (int)fSize)
	{
		debug("KviLocale: Error while reading the translation file %s",KviQString::toUtf8(szCatalogueFile).data());
		kvi_free(buffer);
		f.close();
		return false;
	}

	// Check for broken *.mo files
	if(fSize < (24 + (sizeof(GnuMoStringDescriptor) * numberOfStrings)))
	{
		debug("KviLocale: Broken translation file %s (too small for all descriptors)",KviQString::toUtf8(szCatalogueFile).data());
		kvi_free(buffer);
		f.close();
		return false;
	}

	GnuMoStringDescriptor * origDescriptor  = (GnuMoStringDescriptor *)(buffer + KVI_SWAP_IF_NEEDED(bMustSwap,hdr.orig_tab_offset));
	GnuMoStringDescriptor * transDescriptor = (GnuMoStringDescriptor *)(buffer + KVI_SWAP_IF_NEEDED(bMustSwap,hdr.trans_tab_offset));

	// Check again for broken *.mo files
	int expectedFileSize = KVI_SWAP_IF_NEEDED(bMustSwap,transDescriptor[numberOfStrings - 1].offset) +
			KVI_SWAP_IF_NEEDED(bMustSwap,transDescriptor[numberOfStrings - 1].length);

	if(fSize < (unsigned int)expectedFileSize)
	{
		debug("KviLocale: Broken translation file %s (too small for all the message strings)",KviQString::toUtf8(szCatalogueFile).data());
		kvi_free(buffer);
		f.close();
		return false;
	}

	// Ok...we can run now

	int dictSize = kvi_getFirstBiggerPrime(numberOfStrings);
	if(m_pMessages)
		delete m_pMessages;
	m_pMessages = new KviPointerHashTable<const char *,KviTranslationEntry>(dictSize,true,false); // dictSize, case sensitive , don't copy keys
	m_pMessages->setAutoDelete(true);

	KviStr szHeader;

	for(int i=0;i < numberOfStrings;i++)
	{
		// FIXME: "Check for NULL inside strings here ?"
		//debug("original seems to be at %u and %u byttes long",KVI_SWAP_IF_NEEDED(bMustSwap,origDescriptor[i].offset),
		//	KVI_SWAP_IF_NEEDED(bMustSwap,origDescriptor[i].length));
		//debug("translated seems to be at %u and %u byttes long",KVI_SWAP_IF_NEEDED(bMustSwap,transDescriptor[i].offset),
		//	KVI_SWAP_IF_NEEDED(bMustSwap,transDescriptor[i].length));

		KviTranslationEntry * e = new KviTranslationEntry(
			(char *)(buffer + KVI_SWAP_IF_NEEDED(bMustSwap,origDescriptor[i].offset)),
			KVI_SWAP_IF_NEEDED(bMustSwap,origDescriptor[i].length),
			(char *)(buffer + KVI_SWAP_IF_NEEDED(bMustSwap,transDescriptor[i].offset)),
			KVI_SWAP_IF_NEEDED(bMustSwap,transDescriptor[i].length));

		// In some (or all?) *.mo files the first string
		// is zero bytes long and the translated one contains
		// information about the translation
		if(e->m_szKey.len() == 0)
		{
			szHeader = e->m_szEncodedTranslation;
			delete e;
			continue;
		}

		m_pMessages->insert(e->m_szKey.ptr(),e);
	}

	kvi_free(buffer);
	f.close();

	m_pTextCodec = 0;

	// find out the text encoding , if possible
	if(szHeader.hasData())
	{
		// find "charset=*\n"
		int idx = szHeader.findFirstIdx("charset=");
		if(idx != -1)
		{
			szHeader.cutLeft(idx + 8);
			szHeader.cutFromFirst('\n');
			szHeader.trimmed();
			m_pTextCodec = KviLocale::codecForName(szHeader.ptr());
			if(!m_pTextCodec)
			{
				debug("Can't find the codec for charset=%s",szHeader.ptr());
				debug("Falling back to codecForLocale()");
				m_pTextCodec = QTextCodec::codecForLocale();
			}
		}
	}

	if(!m_pTextCodec)
	{
		debug("The message catalogue does not have a \"charset\" header");
		debug("Assuming utf8"); // FIXME: or codecForLocale() ?
		m_pTextCodec = QTextCodec::codecForName("UTF-8");
	}

	return true;
}

const char * KviMessageCatalogue::translate(const char *text)
{
	KviTranslationEntry * aux = m_pMessages->find(text);
	if(aux)return aux->m_szEncodedTranslation.ptr();
	return text;
}

const QString & KviMessageCatalogue::translateToQString(const char *text)
{
	KviTranslationEntry * aux = m_pMessages->find(text);
	if(aux)
	{
		if(aux->m_pQTranslation)return *(aux->m_pQTranslation);
		aux->m_pQTranslation = new QString(m_pTextCodec->toUnicode(aux->m_szEncodedTranslation.ptr()));
		return *(aux->m_pQTranslation);
	}
	// no translation is available: let's avoid continous string decoding
	aux = new KviTranslationEntry(text);
	m_pMessages->insert(aux->m_szKey.ptr(),aux);
	aux->m_pQTranslation = new QString(m_pTextCodec->toUnicode(aux->m_szEncodedTranslation.ptr()));
	return *(aux->m_pQTranslation);
}

namespace KviLocale
{
#ifndef QT_NO_BIG_CODECS
	#define NUM_ENCODINGS 109
#else
	#define NUM_ENCODINGS 85
#endif

	static EncodingDescription supported_encodings[]=
	{
		{ "UTF-8"                , 0 , 0 , "8-bit Unicode" },
		{ "ISO-8859-1"           , 0 , 0 , "Western, Latin-1" },
		{ "ISO-8859-2"           , 0 , 0 , "Central European 1" },
		{ "ISO-8859-3"           , 0 , 0 , "Central European 2" },
		{ "ISO-8859-4"           , 0 , 0 , "Baltic, Standard" },
		{ "ISO-8859-5"           , 0 , 0 , "Cyrillic, ISO" },
		{ "ISO-8859-6"           , 0 , 0 , "Arabic, Standard" },
		{ "ISO-8859-7"           , 0 , 0 , "Greek" },
		{ "ISO-8859-8"           , 0 , 0 , "Hebrew, visually ordered" },
		{ "ISO-8859-8-i"         , 0 , 0 , "Hebrew, logically ordered" },
		{ "ISO-8859-9"           , 0 , 0 , "Turkish, Latin-5" },
		{ "ISO-8859-15"          , 0 , 0 , "Western, Latin-1 + Euro" },
		{ "KOI8-R"               , 0 , 0 , "Cyrillic, KOI" },
		{ "KOI8-U"               , 0 , 0 , "Ukrainian" },
		{ "CP-1250"              , 0 , 0 , "Central European 3" },
		{ "CP-1251"              , 0 , 0 , "Cyrillic, Windows" },
		{ "CP-1252"              , 0 , 0 , "Western, CP" },
		{ "CP-1253"              , 0 , 0 , "Greek, CP" },
		{ "CP-1256"              , 0 , 0 , "Arabic, CP" },
		{ "CP-1257"              , 0 , 0 , "Baltic, CP" },
		{ "CP-1255"              , 0 , 0 , "Hebrew, CP" },
		{ "CP-1254"              , 0 , 0 , "Turkish, CP" },
		{ "TIS-620"              , 0 , 0 , "Thai" },
#ifndef QT_NO_BIG_CODECS
		{ "Big5"                 , 0 , 0 , "Chinese Traditional" },
		{ "Big5-HKSCS"           , 0 , 0 , "Chinese Traditional, Hong Kong" },
		{ "GB18030"              , 0 , 0 , "Chinese Simplified" },
		{ "JIS7"                 , 0 , 0 , "Japanese (JIS7)" },
		{ "Shift-JIS"            , 0 , 0 , "Japanese (Shift-JIS)" },
		{ "EUC-JP"               , 0 , 0 , "Japanese (EUC-JP)" },
		{ "EUC-KR"               , 0 , 0 , "Korean" },
		{ "TSCII"                , 0 , 0 , "Tamil" },
#endif
		{ "ISO-8859-10"          , 0 , 0 , "ISO-8859-10" },
		{ "ISO-8859-13"          , 0 , 0 , "ISO-8859-13" },
		{ "ISO-8859-14"          , 0 , 0 , "ISO-8859-14" },
		{ "IBM-850"              , 0 , 0 , "IBM-850" },
		{ "IBM-866"              , 0 , 0 , "IBM-866" },
		{ "CP874"                , 0 , 0 , "CP874" },

		// smart codecs that send in the local charset
		{ "ISO-8859-1 [UTF-8]"     , 1 , 0 , "I: 8-bit Unicode / Western Latin-1, O: Western Latin-1" },
		{ "ISO-8859-2 [UTF-8]"     , 1 , 0 , "I: 8-bit Unicode / Central European 1, O: Central European 1" },
		{ "ISO-8859-3 [UTF-8]"     , 1 , 0 , "I: 8-bit Unicode / Central European 2, O: Central European 2" },
		{ "ISO-8859-4 [UTF-8]"     , 1 , 0 , "I: 8-bit Unicode / Baltic, Standard, O: Baltic, Standard" },
		{ "ISO-8859-5 [UTF-8]"     , 1 , 0 , "I: 8-bit Unicode / Cyrillic, ISO, O: Cyrillic, ISO" },
		{ "ISO-8859-6 [UTF-8]"     , 1 , 0 , "I: 8-bit Unicode / Arabic, Standard, O: Arabic, Standard" },
		{ "ISO-8859-7 [UTF-8]"     , 1 , 0 , "I: 8-bit Unicode / Greek, O: Greek" },
		{ "ISO-8859-8 [UTF-8]"     , 1 , 0 , "I: 8-bit Unicode / Hebrew, visually ordered, O: Hebrew, visually ordered" },
		{ "ISO-8859-8-i [UTF-8]"   , 1 , 0 , "I: 8-bit Unicode / Hebrew, logically ordered, O: Hebrew, logically ordered" },
		{ "ISO-8859-9 [UTF-8]"     , 1 , 0 , "I: 8-bit Unicode / Turkish, Latin-5, O: Turkish, Latin-5" },
		{ "ISO-8859-15 [UTF-8]"    , 1 , 0 , "I: 8-bit Unicode / Western, Latin-1 + Euro, O: Western, Latin-1 + Euro" },
		{ "KOI8-R [UTF-8]"         , 1 , 0 , "I: 8-bit Unicode / Cyrillic, KOI, O: Cyrillic, KOI" },
		{ "KOI8-U [UTF-8]"         , 1 , 0 , "I: 8-bit Unicode / Ukrainian, O: Ukrainian" },
		{ "CP-1250 [UTF-8]"        , 1 , 0 , "I: 8-bit Unicode / Central European 3, O: Central European 3" },
		{ "CP-1251 [UTF-8]"        , 1 , 0 , "I: 8-bit Unicode / Cyrillic, Windows, O: Cyrillic, Windows" },
		{ "CP-1252 [UTF-8]"        , 1 , 0 , "I: 8-bit Unicode / Western, CP, O: Western, CP" },
		{ "CP-1253 [UTF-8]"        , 1 , 0 , "I: 8-bit Unicode / Greek, CP, O: Greek, CP" },
		{ "CP-1256 [UTF-8]"        , 1 , 0 , "I: 8-bit Unicode / Arabic, CP, O: Arabic, CP" },
		{ "CP-1257 [UTF-8]"        , 1 , 0 , "I: 8-bit Unicode / Baltic, CP, O: Baltic, CP" },
		{ "CP-1255 [UTF-8]"        , 1 , 0 , "I: 8-bit Unicode / Hebrew, CP, O: Hebrew, CP" },
		{ "CP-1254 [UTF-8]"        , 1 , 0 , "I: 8-bit Unicode / Turkish, CP, O: Turkish, CP" },
		{ "TIS-620 [UTF-8]"        , 1 , 0 , "I: 8-bit Unicode / Thai, O: Thai" },
#ifndef QT_NO_BIG_CODECS
		{ "Big5 [UTF-8]"           , 1 , 0 , "I: 8-bit Unicode / Chinese Traditional, O: Chinese Traditional" },
		{ "Big5-HKSCS [UTF-8]"     , 1 , 0 , "I: 8-bit Unicode / Chinese Traditional, Hong Kong, O: Chinese Traditional, Hong Kong" },
		{ "GB18030 [UTF-8]"        , 1 , 0 , "I: 8-bit Unicode / Chinese Simplified, O: Chinese Simplified" },
		{ "JIS7 [UTF-8]"           , 1 , 0 , "I: 8-bit Unicode / Japanese (JIS7), O: Japanese " },
		{ "Shift-JIS [UTF-8]"      , 1 , 0 , "I: 8-bit Unicode / Japanese (Shift-JIS), O: Japanese (Shift-JIS)" },
		{ "EUC-JP [UTF-8]"         , 1 , 0 , "I: 8-bit Unicode / Japanese (EUC-JP), O: Japanese (EUC-JP)" },
		{ "EUC-KR [UTF-8]"         , 1 , 0 , "I: 8-bit Unicode / Korean, O: Korean" },
		{ "TSCII [UTF-8]"          , 1 , 0 , "I: 8-bit Unicode / Tamil, O: Tamil" },
#endif
		{ "ISO-8859-10 [UTF-8]"    , 1 , 0 , "I: 8-bit Unicode / ISO-8859-10, O: ISO-8859-10" },
		{ "ISO-8859-13 [UTF-8]"    , 1 , 0 , "I: 8-bit Unicode / ISO-8859-13, O: ISO-8859-13" },
		{ "ISO-8859-14 [UTF-8]"    , 1 , 0 , "I: 8-bit Unicode / ISO-8859-14, O: ISO-8859-14" },
		{ "IBM-850 [UTF-8]"        , 1 , 0 , "I: 8-bit Unicode / IBM-850, O: IBM-850" },
		{ "IBM-866 [UTF-8]"        , 1 , 0 , "I: 8-bit Unicode / IBM-866, O: IBM-866" },
		{ "CP874 [UTF-8]"          , 1 , 0 , "I: 8-bit Unicode / CP874, O: CP874" },

		// smart codecs that send in utf8
		{ "UTF-8 [ISO-8859-1]"     , 1 , 1 , "I: 8-bit Unicode / Western Latin-1, O: 8-bit Unicode" },
		{ "UTF-8 [ISO-8859-2]"     , 1 , 1 , "I: 8-bit Unicode / Central European 1, O: 8-bit Unicode" },
		{ "UTF-8 [ISO-8859-3]"     , 1 , 1 , "I: 8-bit Unicode / Central European 2, O: 8-bit Unicode" },
		{ "UTF-8 [ISO-8859-4]"     , 1 , 1 , "I: 8-bit Unicode / Baltic, Standard, O: 8-bit Unicode" },

		{ "UTF-8 [ISO-8859-5]"     , 1 , 1 , "I: 8-bit Unicode / Cyrillic, ISO, O: 8-bit Unicode" },
		{ "UTF-8 [ISO-8859-6]"     , 1 , 1 , "I: 8-bit Unicode / Arabic, Standard, O: 8-bit Unicode" },
		{ "UTF-8 [ISO-8859-7]"     , 1 , 1 , "I: 8-bit Unicode / Greek, O: 8-bit Unicode" },
		{ "UTF-8 [ISO-8859-8]"     , 1 , 1 , "I: 8-bit Unicode / Hebrew, visually ordered, O: 8-bit Unicode" },

		{ "UTF-8 [ISO-8859-8-i]"   , 1 , 1 , "I: 8-bit Unicode / Hebrew, logically ordered, O: 8-bit Unicode" },
		{ "UTF-8 [ISO-8859-9]"     , 1 , 1 , "I: 8-bit Unicode / Turkish, Latin-5, O: 8-bit Unicode" },
		{ "UTF-8 [ISO-8859-15]"    , 1 , 1 , "I: 8-bit Unicode / Western, Latin-1 + Euro, O: 8-bit Unicode" },
		{ "UTF-8 [KOI8-R]"         , 1 , 1 , "I: 8-bit Unicode / Cyrillic, KOI, O: 8-bit Unicode" },

		{ "UTF-8 [KOI8-U]"         , 1 , 1 , "I: 8-bit Unicode / Ukrainian, O: 8-bit Unicode" },
		{ "UTF-8 [CP-1250]"        , 1 , 1 , "I: 8-bit Unicode / Central European 3, O: 8-bit Unicode" },
		{ "UTF-8 [CP-1251]"        , 1 , 1 , "I: 8-bit Unicode / Cyrillic, Windows, O: 8-bit Unicode" },
		{ "UTF-8 [CP-1252]"        , 1 , 1 , "I: 8-bit Unicode / Western, CP, O: 8-bit Unicode" },

		{ "UTF-8 [CP-1253]"        , 1 , 1 , "I: 8-bit Unicode / Greek, CP, O: 8-bit Unicode" },
		{ "UTF-8 [CP-1256]"        , 1 , 1 , "I: 8-bit Unicode / Arabic, CP, O: 8-bit Unicode" },
		{ "UTF-8 [CP-1257]"        , 1 , 1 , "I: 8-bit Unicode / Baltic, CP, O: 8-bit Unicode" },
		{ "UTF-8 [CP-1255]"        , 1 , 1 , "I: 8-bit Unicode / Hebrew, CP, O: 8-bit Unicode" },

		{ "UTF-8 [CP-1254]"        , 1 , 1 , "I: 8-bit Unicode / Turkish, CP, O: 8-bit Unicode" },
		{ "UTF-8 [TIS-620]"        , 1 , 1 , "I: 8-bit Unicode / Thai, O: 8-bit Unicode" },
#ifndef QT_NO_BIG_CODECS
		{ "UTF-8 [Big5]"           , 1 , 1 , "I: 8-bit Unicode / Chinese Traditional, O: 8-bit Unicode" },
		{ "UTF-8 [Big5-HKSCS]"     , 1 , 1 , "I: 8-bit Unicode / Chinese Traditional, Hong Kong, O: 8-bit Unicode" },

		{ "UTF-8 [GB18030]"        , 1 , 1 , "I: 8-bit Unicode / Chinese Simplified, O: 8-bit Unicode" },
		{ "UTF-8 [JIS7]"           , 1 , 1 , "I: 8-bit Unicode / Japanese (JIS7), O: 8-bit Unicode" },
		{ "UTF-8 [Shift-JIS]"      , 1 , 1 , "I: 8-bit Unicode / Japanese (Shift-JIS), O: Japanese (Shift-JIS)" },
		{ "UTF-8 [EUC-JP]"         , 1 , 1 , "I: 8-bit Unicode / Japanese (EUC-JP), O: Japanese (EUC-JP)" },

		{ "UTF-8 [EUC-KR]"         , 1 , 1 , "I: 8-bit Unicode / Korean, O: 8-bit Unicode" },
		{ "UTF-8 [TSCII]"          , 1 , 1 , "I: 8-bit Unicode / Tamil, O: 8-bit Unicode" },
#endif
		{ "UTF-8 [ISO-8859-10]"    , 1 , 1 , "I: 8-bit Unicode / ISO-8859-10, O: 8-bit Unicode" },
		{ "UTF-8 [ISO-8859-13]"    , 1 , 1 , "I: 8-bit Unicode / ISO-8859-13, O: 8-bit Unicode" },

		{ "UTF-8 [ISO-8859-14]"    , 1 , 1 , "I: 8-bit Unicode / ISO-8859-14, O: 8-bit Unicode" },
		{ "UTF-8 [IBM-850]"        , 1 , 1 , "I: 8-bit Unicode / IBM-850, O: 8-bit Unicode" },
		{ "UTF-8 [IBM-866]"        , 1 , 1 , "I: 8-bit Unicode / IBM-866, O: 8-bit Unicode" },
		{ "UTF-8 [CP874]"          , 1 , 1 , "I: 8-bit Unicode / CP874, O: 8-bit Unicode" },

		{ 0                        , 0 , 0 , 0 }
	};

	EncodingDescription * encodingDescription(int iIdx)
	{
		if(iIdx > NUM_ENCODINGS)return &(supported_encodings[NUM_ENCODINGS]);
		return &(supported_encodings[iIdx]);
	}

	QTextCodec * codecForName(const char * szName)
	{
		KviStr szTmp = szName;
		int idx = szTmp.findFirstIdx('[');
		if(idx != -1)
		{
			// composite codec: either UTF-8 [child codec] or child codec [UTF-8]
			KviSmartTextCodec * c = g_pSmartCodecDict->find(szName);
			if(c)return c;


			if(kvi_strEqualCIN("UTF-8 [",szName,7))
			{
				szTmp.replaceAll("UTF-8 [","");
				szTmp.replaceAll("]","");
				// smart codec that sends UTF-8
				c = new KviSmartTextCodec(szName,szTmp.ptr(),true);
			} else {
				szTmp.cutFromFirst(' ');
				// smart codec that sends child encoding
				c = new KviSmartTextCodec(szName,szTmp.ptr(),false);
			}
			if(c->ok())
			{
				g_pSmartCodecDict->replace(szName,c);
				return c;
			} else {
				delete c;
			}
		}
		return QTextCodec::codecForName(szName);
	}

	const KviStr & localeName()
	{
		return g_szLang;
	}

	bool loadCatalogue(const QString &name,const QString &szLocaleDir)
	{
		//debug("Looking up catalogue %s",name);
		if(g_pCatalogueDict->find(KviQString::toUtf8(name).data()))
			return true; // already loaded

		QString szBuffer;

		if(findCatalogue(szBuffer,name,szLocaleDir))
		{
			KviMessageCatalogue * c = new KviMessageCatalogue();
			if(c->load(szBuffer))
			{
				g_pCatalogueDict->insert(KviQString::toUtf8(name).data(),c);
				return true;
			}
			delete c;
			c = 0;
		}
		return false;
	}

	bool unloadCatalogue(const QString &name)
	{
		//debug("Unloading catalogue : %s",name);
		return g_pCatalogueDict->remove(KviQString::toUtf8(name).data());
	}

	bool findCatalogue(QString &szBuffer,const QString& name,const QString& szLocaleDir)
	{
		KviStr szLocale = g_szLang;

		QString szLocDir = szLocaleDir;
		KviQString::ensureLastCharIs(szLocDir,KVI_PATH_SEPARATOR_CHAR);

		KviQString::sprintf(szBuffer,"%Q%Q_%s.mo",&szLocDir,&name,szLocale.ptr());

		if(KviFileUtils::fileExists(szBuffer))return true;

		if(szLocale.findFirstIdx('.') != -1)
		{
			// things like en_GB.utf8
			// kill them
			szLocale.cutFromFirst('.');

			KviQString::sprintf(szBuffer,"%Q%Q_%s.mo",&szLocDir,&name,szLocale.ptr());
			if(KviFileUtils::fileExists(szBuffer))return true;
		}

		if(szLocale.findFirstIdx('@') != -1)
		{
			// things like @euro ?
			// kill them
			szLocale.cutFromFirst('@');
			KviQString::sprintf(szBuffer,"%Q%Q_%s.mo",&szLocDir,&name,szLocale.ptr());
			if(KviFileUtils::fileExists(szBuffer))return true;
		}

		if(szLocale.findFirstIdx('_') != -1)
		{
			// things like en_GB
			// kill them
			szLocale.cutFromFirst('_');
			KviQString::sprintf(szBuffer,"%Q%Q_%s.mo",&szLocDir,&name,szLocale.ptr());
			if(KviFileUtils::fileExists(szBuffer))return true;
		}

		// try the lower case version too
		szLocale.toLower();
		KviQString::sprintf(szBuffer,"%Q%Q_%s.mo",&szLocDir,&name,szLocale.ptr());
		if(KviFileUtils::fileExists(szBuffer))return true;

		return false;
	}

	//
	// This function attempts to determine the current locale
	// and then load the corresponding translation file
	// from the KVIrc locale directory
	// Returns true if the locale was correctly set
	// i.e. the locale is C or POSIX (no translation needed)
	//     or the locale is correctly defined and the
	//     translation map was sucesfully loaded
	//

	void init(QApplication * app,const QString &localeDir)
	{
		// first of all try to find out the current locale
		g_szLang="";
		QString szLangFile=QString("%1/.kvirc_force_locale").arg(QDir::homePath());

		if(KviFileUtils::fileExists(szLangFile))
		{
			QString szTmp;
			KviFileUtils::readFile(szLangFile,szTmp);
			g_szLang=szTmp;
		}
		if(g_szLang.isEmpty())g_szLang = kvi_getenv("KVIRC_LANG");
		if(g_szLang.isEmpty())g_szLang = kvi_getenv("LC_MESSAGES");
		if(g_szLang.isEmpty())g_szLang = kvi_getenv("LANG");
		if(g_szLang.isEmpty())g_szLang = QLocale::system().name();
		if(g_szLang.isEmpty())g_szLang = "en";
		g_szLang.trimmed();

		// the main catalogue is supposed to be kvirc_<language>.mo
		g_pMainCatalogue = new KviMessageCatalogue();
		// the catalogue dict
		g_pCatalogueDict = new KviPointerHashTable<const char *,KviMessageCatalogue>;
		g_pCatalogueDict->setAutoDelete(true);

		// the smart codec dict
		g_pSmartCodecDict = new KviPointerHashTable<const char *,KviSmartTextCodec>;
		// the Qt docs explicitly state that we shouldn't delete
		// the codecs by ourselves...
		g_pSmartCodecDict->setAutoDelete(false);

		if(g_szLang.hasData())
		{
			QString szBuffer;
			if(findCatalogue(szBuffer,"kvirc",localeDir))
			{
				g_pMainCatalogue->load(szBuffer);
				g_pTranslator = new KviTranslator(app,"kvirc_translator");
				app->installTranslator(g_pTranslator);
			} else {
				KviStr szTmp = g_szLang;
				szTmp.cutFromFirst('.');
				szTmp.cutFromFirst('_');
				szTmp.cutFromFirst('@');
				szTmp.toLower();
				if(!(kvi_strEqualCI(szTmp.ptr(),"en") ||
					kvi_strEqualCI(szTmp.ptr(),"c") ||
					kvi_strEqualCI(szTmp.ptr(),"us") ||
					kvi_strEqualCI(szTmp.ptr(),"gb") ||
					kvi_strEqualCI(szTmp.ptr(),"posix")))
				{
					// FIXME: THIS IS NO LONGER VALID!!!
					debug("Can't find the catalogue for locale \"%s\" (%s)",g_szLang.ptr(),szTmp.ptr());
					debug("There is no such translation or the $LANG variable was incorrectly set");
					debug("You can use $KVIRC_LANG to override the catalogue name");
					debug("For example you can set KVIRC_LANG to it_IT to force usage of the it.mo catalogue");
				}
			}
		}

		//g_pTextCodec = QTextCodec::codecForLocale();
		//if(!g_pTextCodec)g_pTextCodec = QTextCodec::codecForLocale();
	}

	void done(QApplication * app)
	{
		delete g_pMainCatalogue;
		delete g_pCatalogueDict;
		delete g_pSmartCodecDict;
		g_pMainCatalogue = 0;
		g_pCatalogueDict = 0;
		g_pSmartCodecDict = 0;
		if(g_pTranslator)
		{
			app->removeTranslator(g_pTranslator);
			delete g_pTranslator;
            g_pTranslator = 0;
		}
	}

	KviMessageCatalogue * getLoadedCatalogue(const QString& name)
	{
		return g_pCatalogueDict->find(KviQString::toUtf8(name).data());
	}


	const char * translate(const char * text,const char * context)
	{
		if(context)
		{
			KviMessageCatalogue * c = g_pCatalogueDict->find(context);
			if(!c)
			{
				// FIXME: Should really try to load the catalogue here!
				c = new KviMessageCatalogue();
				g_pCatalogueDict->insert(context,c);
			}
			return c->translate(text);
		}
		return g_pMainCatalogue->translate(text);
	}

	const QString & translateToQString(const char * text,const char * context)
	{
		if(context)
		{
			KviMessageCatalogue * c = g_pCatalogueDict->find(context);
			if(!c)
			{
				// FIXME: Should really try to load the catalogue here!
				c = new KviMessageCatalogue();
				g_pCatalogueDict->insert(context,c);
			}
			return c->translateToQString(text);
		}
		return g_pMainCatalogue->translateToQString(text);
	}
}

KviTranslator::KviTranslator(QObject * par,const char *)
: QTranslator(par)
{
}

KviTranslator::~KviTranslator()
{
}

QString KviTranslator::translate(const char *,const char * message,const char *) const
{
	// we ignore contexts and comments for qt translations
	return g_pMainCatalogue->translateToQString(message);
}

#if 0

// a fake table that will force these translations
// to be included in the *.pot file

static QString fake_translations_table[]=
{
	// global
	__tr2qs("OK"),
	__tr2qs("Cancel"),
	// color dialog
	__tr2qs("Select color"),
	__tr2qs("&Basic colors"),
	__tr2qs("&Custom colors"),
	__tr2qs("&Red"),
	__tr2qs("&Green"),
	__tr2qs("Bl&ue"),
	__tr2qs("&Define Custom Colors >>"),
	__tr2qs("&Add to Custom Colors"),
	// font dialog
	__tr2qs("Select Font"),
	__tr2qs("&Font"),
	__tr2qs("Font st&yle"),
	__tr2qs("&Size"),
	__tr2qs("Sample"),
	__tr2qs("Effects"),
	__tr2qs("Stri&keout"),
	__tr2qs("&Underline"),
	__tr2qs("Scr&ipt"),
	//File selector
	__tr2qs("Parent Directory"),
	__tr2qs("Back"),
	__tr2qs("Forward"),
	__tr2qs("Reload"),
	__tr2qs("New Directory"),
	__tr2qs("Bookmarks"),
	__tr2qs("Add Bookmark"),
	__tr2qs("&Edit Bookmarks"),
	__tr2qs("New Bookmark Folder..."),
	__tr2qs("Configure"),
	__tr2qs("Sorting"),
	__tr2qs("By Name"),
	__tr2qs("By Date"),
	__tr2qs("By Size"),
	__tr2qs("Reverse"),
	__tr2qs("Directories First"),
	__tr2qs("Case Insensitive"),
	__tr2qs("Short View"),
	__tr2qs("Detailed View"),
	__tr2qs("Show Hidden Files"),
	__tr2qs("Show Quick Access Navigation Panel"),
	__tr2qs("Show Preview"),
	__tr2qs("Separate Directories"),
	__tr2qs("Often used directories"),
	__tr2qs("Desktop"),
	__tr2qs("Home Directory"),
	__tr2qs("Floppy"),
	__tr2qs("Temporary Files"),
	__tr2qs("Network"),
	__tr2qs("New Directory..."),
	__tr2qs("Delete"),
	__tr2qs("Thumbnail Previews"),
	__tr2qs("Large Icons"),
	__tr2qs("Small Icons"),
	__tr2qs("Properties..."),
	__tr2qs("&Automatic Preview"),
	__tr2qs("&Preview"),
	__tr2qs("&Location:"),
	__tr2qs("&Filter:"),
	__tr2qs("All Files"),
	__tr2qs("&OK"),
	__tr2qs("&Cancel")
}
#endif
