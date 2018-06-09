//=============================================================================
//
//   File : KviMessageCatalogue.cpp
//   Creation date : Wed 23 Mar 2011 03:32:12
//   Originally in KviLocale.cpp
//      Created on: Sat Jan 16 1999 19:08:41 by Szymon Stefanek
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

//
//
//   Pieces of the following code were extracted and adapted from from gettext.h and
//   gettextP.h from the GNU gettext package.
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
//

#include "KviMessageCatalogue.h"
#include "KviFile.h"
#include "KviByteOrder.h"
#include "KviCString.h"
#include "KviMemory.h"
#include "KviLocale.h"
#include "KviPointerHashTable.h"
#include "KviTranslationEntry.h"

#include <QString>
#include <QTextCodec>

#include <cstdio>

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

#define KVI_SWAP_IF_NEEDED(flag, value) (flag ? KviByteOrder::swap32(value) : (value))

//
//   End of gettext.h & gettextP.h
//

// HELPERS

static int somePrimeNumbers[90] = {
	257, 521, 769, 1031, 1087, 1091, 1103, 1117, 1123, 1151,    // Incomplete *.mo files
	1163, 1171, 1181, 1193, 1201, 1213, 1217, 1223, 1229, 1231, // Complete *.mo files
	1237, 1249, 1259, 1277, 1283, 1289, 1291, 1297, 1307, 1319,
	1321, 1327, 1361, 1367, 1373, 1381, 1399, 1409, 1423, 1433,
	1447, 1459, 1471, 1481, 1493, 1511, 1523, 1531, 1543, 1553,
	1567, 1571, 1583, 1597, 1609, 1619, 1627, 1637, 1657, 1667, // Too big for KVIrc *.mo files
	1693, 1709, 1721, 1733, 1741, 1753, 1777, 1789, 1811, 1831,
	1907, 2069, 2111, 2221, 2309, 2441, 2531, 2617, 2731, 2837,
	2903, 3121, 3329, 3331, 3767, 4127, 5051, 6089, 7039, 9973
};

int kvi_getFirstBiggerPrime(int iNumber)
{
	for(int somePrimeNumber : somePrimeNumbers)
	{
		if(somePrimeNumber >= iNumber)
			return somePrimeNumber;
	}
	return 9973; //error!
}

KviMessageCatalogue::KviMessageCatalogue()
{
	//m_uEncoding = 0;
	m_pTextCodec = QTextCodec::codecForLocale();

	//m_pMessages = new KviPointerHashTable<const char *,KviTranslationEntry>(1123,true,false); // dictSize, case sensitive, don't copy keys
	m_pMessages = new KviPointerHashTable<const char *, KviTranslationEntry>(32, true, false); // dictSize, case sensitive, don't copy keys
	m_pMessages->setAutoDelete(true);
}

KviMessageCatalogue::~KviMessageCatalogue()
{
	if(m_pMessages)
		delete m_pMessages;
}

bool KviMessageCatalogue::load(const QString & szName)
{
	QString szCatalogueFile(szName);

	// Try to load the header
	KviFile f(szCatalogueFile);
	if(!f.open(QFile::ReadOnly))
	{
		qDebug("[KviLocale]: Failed to open the messages file %s: probably doesn't exist", szCatalogueFile.toUtf8().data());
		return false;
	}

	GnuMoFileHeader hdr;

	if(f.read((char *)&hdr, sizeof(GnuMoFileHeader)) < (int)sizeof(GnuMoFileHeader))
	{
		qDebug("KviLocale: Failed to read header of %s", szCatalogueFile.toUtf8().data());
		f.close();
		return false;
	}

	bool bMustSwap = false;

	if(hdr.magic != KVI_LOCALE_MAGIC)
	{
		if(hdr.magic == KVI_LOCALE_MAGIC_SWAPPED)
		{
			qDebug("KviLocale: Swapped magic for file %s: swapping data too", szCatalogueFile.toUtf8().data());
			bMustSwap = true;
		}
		else
		{
			qDebug("KviLocale: Bad locale magic for file %s: not a *.mo file?", szCatalogueFile.toUtf8().data());
			f.close();
			return false;
		}
	}

	if(KVI_SWAP_IF_NEEDED(bMustSwap, hdr.revision) != MO_REVISION_NUMBER)
	{
		qDebug("KviLocale: Invalid *.mo file revision number for file %s", szCatalogueFile.toUtf8().data());
		f.close();
		return false;
	}

	int iStringsNum = KVI_SWAP_IF_NEEDED(bMustSwap, hdr.nstrings);

	if(iStringsNum <= 0)
	{
		qDebug("KviLocale: No translated messages found in file %s", szCatalogueFile.toUtf8().data());
		f.close();
		return false;
	}

	if(iStringsNum >= 9972)
	{
		qDebug("Number of strings too big... sure that it is a KVIrc catalog file?");
		iStringsNum = 9972;
	}

	// return back
	f.seek(0);

	unsigned int uSize = f.size();
	char * pcBuffer = (char *)KviMemory::allocate(uSize);

	// FIXME: maybe read it in blocks eh ?
	if(f.read(pcBuffer, uSize) < (int)uSize)
	{
		qDebug("KviLocale: Error while reading the translation file %s", szCatalogueFile.toUtf8().data());
		KviMemory::free(pcBuffer);
		f.close();
		return false;
	}

	// Check for broken *.mo files
	if(uSize < (24 + (sizeof(GnuMoStringDescriptor) * iStringsNum)))
	{
		qDebug("KviLocale: Broken translation file %s (too small for all descriptors)", szCatalogueFile.toUtf8().data());
		KviMemory::free(pcBuffer);
		f.close();
		return false;
	}

	GnuMoStringDescriptor * pOrigDescriptor = (GnuMoStringDescriptor *)(pcBuffer + KVI_SWAP_IF_NEEDED(bMustSwap, hdr.orig_tab_offset));
	GnuMoStringDescriptor * pTransDescriptor = (GnuMoStringDescriptor *)(pcBuffer + KVI_SWAP_IF_NEEDED(bMustSwap, hdr.trans_tab_offset));

	// Check again for broken *.mo files
	int iExpectedFileSize = KVI_SWAP_IF_NEEDED(bMustSwap, pTransDescriptor[iStringsNum - 1].offset) + KVI_SWAP_IF_NEEDED(bMustSwap, pTransDescriptor[iStringsNum - 1].length);

	if(uSize < (unsigned int)iExpectedFileSize)
	{
		qDebug("KviLocale: Broken translation file %s (too small for all the message strings)", szCatalogueFile.toUtf8().data());
		KviMemory::free(pcBuffer);
		f.close();
		return false;
	}

	// Ok...we can run now

	int iDictSize = kvi_getFirstBiggerPrime(iStringsNum);
	if(m_pMessages)
		delete m_pMessages;
	m_pMessages = new KviPointerHashTable<const char *, KviTranslationEntry>(iDictSize, true, false); // dictSize, case sensitive, don't copy keys
	m_pMessages->setAutoDelete(true);

	KviCString szHeader;

	for(int i = 0; i < iStringsNum; i++)
	{
		// FIXME: "Check for NULL inside strings here ?"
		//qDebug("original seems to be at %u and %u bytes long",KVI_SWAP_IF_NEEDED(bMustSwap,pOrigDescriptor[i].offset),
		//	KVI_SWAP_IF_NEEDED(bMustSwap,pOrigDescriptor[i].length));
		//qDebug("translated seems to be at %u and %u bytes long",KVI_SWAP_IF_NEEDED(bMustSwap,pTransDescriptor[i].offset),
		//	KVI_SWAP_IF_NEEDED(bMustSwap,pTransDescriptor[i].length));

		KviTranslationEntry * e = new KviTranslationEntry(
		    (char *)(pcBuffer + KVI_SWAP_IF_NEEDED(bMustSwap, pOrigDescriptor[i].offset)),
		    KVI_SWAP_IF_NEEDED(bMustSwap, pOrigDescriptor[i].length),
		    (char *)(pcBuffer + KVI_SWAP_IF_NEEDED(bMustSwap, pTransDescriptor[i].offset)),
		    KVI_SWAP_IF_NEEDED(bMustSwap, pTransDescriptor[i].length));

		// In some (or all?) *.mo files the first string
		// is zero bytes long and the translated one contains
		// information about the translation
		if(e->m_szKey.len() == 0)
		{
			szHeader = e->m_szEncodedTranslation;
			delete e;
			continue;
		}

		m_pMessages->insert(e->m_szKey.ptr(), e);
	}

	KviMemory::free(pcBuffer);
	f.close();

	m_pTextCodec = nullptr;

	// find out the text encoding, if possible
	if(szHeader.hasData())
	{
		// find "charset=*\n"
		int iIdx = szHeader.findFirstIdx("charset=");
		if(iIdx != -1)
		{
			szHeader.cutLeft(iIdx + 8);
			szHeader.cutFromFirst('\n');
			szHeader.trim();
			m_pTextCodec = KviLocale::instance()->codecForName(szHeader.ptr());
			if(!m_pTextCodec)
			{
				qDebug("Can't find the codec for charset=%s", szHeader.ptr());
				qDebug("Falling back to codecForLocale()");
				m_pTextCodec = QTextCodec::codecForLocale();
			}
		}
	}

	if(!m_pTextCodec)
	{
		qDebug("The message catalogue does not have a \"charset\" header");
		qDebug("Assuming UTF-8"); // FIXME: or codecForLocale() ?
		m_pTextCodec = QTextCodec::codecForName("UTF-8");
	}

	return true;
}

const char * KviMessageCatalogue::translate(const char * pcText)
{
	KviTranslationEntry * pAux = m_pMessages->find(pcText);
	if(pAux)
		return pAux->m_szEncodedTranslation.ptr();
	return pcText;
}

const QString & KviMessageCatalogue::translateToQString(const char * pcText)
{
	KviTranslationEntry * pAux = m_pMessages->find(pcText);
	if(pAux)
	{
		if(pAux->m_pTranslation)
			return *(pAux->m_pTranslation);
		pAux->m_pTranslation = new QString(m_pTextCodec->toUnicode(pAux->m_szEncodedTranslation.ptr()));
		return *(pAux->m_pTranslation);
	}
	// no translation is available: let's avoid continuous string decoding
	pAux = new KviTranslationEntry(pcText);
	m_pMessages->insert(pAux->m_szKey.ptr(), pAux);
	pAux->m_pTranslation = new QString(m_pTextCodec->toUnicode(pAux->m_szEncodedTranslation.ptr()));
	return *(pAux->m_pTranslation);
}
