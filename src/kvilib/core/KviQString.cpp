//=============================================================================
//
//   File : KviQString.cpp
//   Creation date : Mon Aug 04 2003 13:36:33 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2010 Szymon Stefanek (pragma at kvirc dot net)
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
// Helper functions for the QString class
//

#include "KviQString.h"
#include "KviCString.h"
#include "KviMemory.h"
#include "KviLocale.h"

#include <cctype> // for tolower()
#include <cstdio> // for sprintf()
#include <QRegExp>

// kvi_string.cpp
extern unsigned char iso88591_toLower_map[256];
extern unsigned char iso88591_toUpper_map[256];

#define MY_MIN(a, b) (((a) < (b)) ? (a) : (b))

namespace KviQString
{
	// The global empty (and null) string
	const QString Empty;

	static char cHexDigits[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

	bool equalCSN(const QString & sz1, const QString & sz2, unsigned int uLen)
	{
		if(uLen == 0)
		{
			// assume equal
			return true;
		}
		const QChar * pC1 = sz1.unicode();
		const QChar * pC2 = sz2.unicode();
		unsigned int uLMin = MY_MIN(sz1.length(), sz2.length());
		if(uLMin < uLen)
		{
			return false;
		}
		const QChar * pC1e = pC1 + uLen;

		if(!pC1 || !pC2)
		{
			return (pC1 == pC2);
		}

		while(pC1 < pC1e)
		{
			if(pC1->unicode() != pC2->unicode())
			{
				return false;
			}
			pC1++;
			pC2++;
		}
		return (pC1 == pC1e);
	}

	bool equalCIN(const QString & sz1, const QString & sz2, unsigned int uLen)
	{
		if(uLen == 0)
		{
			// assume equal
			return true;
		}
		const QChar * pC1 = sz1.unicode();
		const QChar * pC2 = sz2.unicode();
		unsigned int uLMin = MY_MIN(sz1.length(), sz2.length());
		if(uLMin < uLen)
		{
			return false;
		}
		const QChar * pC1e = pC1 + uLen;

		if(!pC1 || !pC2)
		{
			return (pC1 == pC2);
		}

		while(pC1 < pC1e)
		{
			if(pC1->toLower().unicode() != pC2->toLower().unicode())
			{
				return false;
			}
			pC1++;
			pC2++;
		}
		return (pC1 == pC1e);
	}

	bool equalCSN(const QString & sz1, const char * pc2, unsigned int uLen)
	{
		if(uLen == 0)
		{
			// assume equal
			return true;
		}
		const QChar * pC1 = sz1.unicode();
		if((unsigned)sz1.length() < uLen)
		{
			return false;
		}
		const QChar * pC1e = pC1 + uLen;

		if(!pc2)
		{
			return !pC1;
		}
		if(!pC1)
		{
			return !pc2;
		}

		while((pC1 < pC1e) && (*pc2))
		{
			if(pC1->unicode() != *pc2)
			{
				return false;
			}
			pC1++;
			pc2++;
		}
		return (pC1 == pC1e);
	}

	bool equalCIN(const QString & sz1, const char * pc2, unsigned int uLen)
	{
		if(uLen == 0)
		{
			// assume equal
			return true;
		}
		const QChar * pC1 = sz1.unicode();
		if((unsigned)sz1.length() < uLen)
		{
			return false;
		}
		const QChar * pC1e = pC1 + uLen;

		if(!pc2)
		{
			return !pC1;
		}
		if(!pC1)
		{
			return !(*pc2);
		}

		while((pC1 < pC1e) && (*pc2))
		{
			if(pC1->toLower().unicode() != tolower(*pc2))
			{
				return false;
			}
			pC1++;
			pc2++;
		}
		return (pC1 == pC1e);
	}

	// pC2 is assumed to be null terminated, sz1 is not!
	bool equalCIN(const QString & sz1, const QChar * pC2, unsigned int uLen)
	{
		if(uLen == 0)
		{
			// assume equal
			return true;
		}
		const QChar * pC1 = sz1.unicode();
		if((unsigned)sz1.length() < uLen)
		{
			return false;
		}
		const QChar * pC1e = pC1 + uLen;

		if(!pC2)
		{
			return !pC1;
		}
		if(!pC1)
		{
			return !(pC2->unicode());
		}

		while((pC1 < pC1e) && (pC2->unicode()))
		{
			if(pC1->toLower().unicode() != pC2->toLower().unicode())
			{
				return false;
			}
			pC1++;
			pC2++;
		}
		return (pC1 == pC1e);
	}

	QString makeSizeReadable(quint64 bytes)
	{
		double dSize = bytes;
		if(dSize < 1024)
		{
			return QString(__tr2qs("%1 bytes")).arg(dSize, 0, 'f', 0);
		}

		dSize /= 1024;
		if(dSize < 1024)
		{
			return QString(__tr2qs("%1 KiB")).arg(dSize, 0, 'f', 3);
		}

		dSize /= 1024;
		if(dSize < 1024)
		{
			return QString(__tr2qs("%1 MiB")).arg(dSize, 0, 'f', 3);
		}

		//Pirated DVD?;)
		dSize /= 1024;
		if(dSize < 1024)
		{
			return QString(__tr2qs("%1 GiB")).arg(dSize, 0, 'f', 3);
		}

		//Uhm.. We are downloading a whole internet:)))
		dSize /= 1024;
		return QString(__tr2qs("%1 TiB")).arg(dSize, 0, 'f', 3);
	}

	bool equalCS(const QString & sz1, const QString & sz2)
	{
		if(sz1.length() != sz2.length())
		{
			return false;
		}

		const QChar * pC1 = sz1.unicode();
		const QChar * pC2 = sz2.unicode();
		const QChar * pC1e = pC1 + sz1.length();

		if(!pC1 || !pC2)
		{
			return (pC1 == pC2);
		}

		while(pC1 < pC1e)
		{
			if(pC1->unicode() != pC2->unicode())
			{
				return false;
			}
			pC1++;
			pC2++;
		}
		return (pC1 == pC1e);
	}

	bool equalCI(const QString & sz1, const QString & sz2)
	{
		if(sz1.length() != sz2.length())
		{
			return false;
		}

		const QChar * pC1 = sz1.unicode();
		const QChar * pC2 = sz2.unicode();
		const QChar * pC1e = pC1 + sz1.length();

		if(!pC1 || !pC2)
		{
			return (pC1 == pC2);
		}

		while(pC1 < pC1e)
		{
			if(pC1->toLower().unicode() != pC2->toLower().unicode())
			{
				return false;
			}
			pC1++;
			pC2++;
		}
		return (pC1 == pC1e);
	}

	// sz2 is assumed to be null terminated, sz1 is not!
	bool equalCI(const QString & sz1, const QChar * pC2)
	{
		const QChar * pC1 = sz1.unicode();
		const QChar * pC1e = pC1 + sz1.length();

		if(!pC1 || !pC2)
		{
			return (pC1 == pC2);
		}

		while(pC1 < pC1e)
		{
			if(!pC2->unicode())
			{
				// sz1 has at least another character
				return false;
			}
			if(pC1->toLower().unicode() != pC2->toLower().unicode())
			{
				return false;
			}
			pC1++;
			pC2++;
		}
		return (pC1 == pC1e) && (!pC2->unicode());
	}

	bool equalCS(const QString & sz1, const char * pc2)
	{
		const QChar * pC1 = sz1.unicode();
		const QChar * pC1e = pC1 + sz1.length();

		if(!pC1)
		{
			return !pc2;
		}

		while((pC1 < pC1e) && (*pc2))
		{
			if(pC1->unicode() != *pc2)
			{
				return false;
			}
			pC1++;
			pc2++;
		}
		return ((pC1 == pC1e) && (*pc2 == '\0'));
	}

	bool equalCI(const QString & sz1, const char * pc2)
	{
		const QChar * pC1 = sz1.unicode();
		const QChar * pC1e = pC1 + sz1.length();

		if(!pC1)
		{
			return !pc2;
		}

		while((pC1 < pC1e) && (*pc2))
		{
			if(pC1->toLower().unicode() != tolower(*pc2))
			{
				return false;
			}
			pC1++;
			pc2++;
		}
		return ((pC1 == pC1e) && (*pc2 == '\0'));
	}

	int cmpCI(const QString & sz1, const QString & sz2, bool bNonAlphaGreater)
	{
		const QChar * pC1 = sz1.unicode();
		const QChar * pC2 = sz2.unicode();
		const QChar * pC1e = pC1 + sz1.length();
		const QChar * pC2e = pC2 + sz2.length();

		if(!pC1)
		{
			if(!pC2)
			{
				return 0;
			}
			return -1;
		}
		if(!pC2)
		{
			return 1;
		}

		for(;;)
		{
			if(bNonAlphaGreater)
			{
				if(pC1->isLetterOrNumber() && !pC2->isLetterOrNumber())
				{
					return -1;
				}
				else if(!pC1->isLetterOrNumber() && pC2->isLetterOrNumber())
				{
					return 1;
				}
			}

			if(pC1 >= pC1e)
			{
				if(pC2 < pC2e)
				{
					return /* 0 */ -(pC2->toLower().unicode());
				}
				return 0;
			}

			if(pC2 >= pC2e)
			{
				return pC1->toLower().unicode() /* - 0 */;
			}

			int iDiff = pC1->toLower().unicode() - pC2->toLower().unicode();

			if(iDiff)
			{
				return iDiff;
			}

			pC1++;
			pC2++;
		}

		return 0; // never here
	}

	int cmpCIN(const QString & sz1, const QString & sz2, unsigned int uLen)
	{
		if(uLen == 0)
		{
			// assume equal
			return 0;
		}
		unsigned int u1 = MY_MIN(uLen, (unsigned)sz1.length());
		unsigned int u2 = MY_MIN(u1, (unsigned)sz2.length()); // FIXME: THIS IS NOT OK

		const QChar * pC1 = sz1.unicode();
		const QChar * pC2 = sz2.unicode();
		const QChar * pC1e = pC1 + u2;

		if(!pC1)
		{
			if(!pC2)
			{
				return 0;
			}
			return -1;
		}
		if(!pC2)
		{
			return 1;
		}

		int iDiff = 0;

		while((pC1 < pC1e) && !(iDiff = (pC1->toLower().unicode() - pC2->toLower().unicode())))
		{
			pC1++;
			pC2++;
		}

		return iDiff;
	}

	void ensureLastCharIs(QString & szSrc, const QChar & c)
	{
		if(!szSrc.endsWith(c))
		{
			szSrc.append(c);
		}
	}

	QString getToken(QString & szSrc, const QChar & sep)
	{
		int i = 0;
		while(i < szSrc.length())
		{
			if(szSrc[i] == sep)
			{
				break;
			}
			i++;
		}
		QString szRet;
		if(i == szSrc.length())
		{
			szRet = szSrc;
			szSrc = "";
		}
		else
		{
			szRet = szSrc.left(i);
			while(i < szSrc.length())
			{
				if(szSrc[i] != sep)
				{
					break;
				}
				i++;
			}
			if(i == szSrc.length())
			{
				szSrc = "";
			}
			else
			{
				szSrc.remove(0, i);
			}
		}
		return szRet;
	}

	void stripRightWhiteSpace(QString & szSrc)
	{
		int iRemove = 0;
		while(iRemove < szSrc.length())
		{
			if(szSrc.at(szSrc.length() - (iRemove + 1)).isSpace())
			{
				iRemove++;
			}
			else
			{
				break;
			}
		}
		if(iRemove > 0)
		{
			szSrc.remove(szSrc.length() - iRemove, iRemove);
		}
	}

	void stripRight(QString & szSrc, const QChar & c)
	{
		int iRemove = 0;
		while(iRemove < szSrc.length())
		{
			if(szSrc.at(szSrc.length() - (iRemove + 1)) == c)
			{
				iRemove++;
			}
			else
			{
				break;
			}
		}
		if(iRemove > 0)
		{
			szSrc.remove(szSrc.length() - iRemove, iRemove);
		}
	}

	void stripLeft(QString & szSrc, const QChar & c)
	{
		int iRemove = 0;
		while(iRemove < szSrc.length())
		{
			if(szSrc[iRemove] == c)
			{
				iRemove++;
			}
			else
			{
				break;
			}
		}
		if(iRemove > 0)
		{
			szSrc.remove(0, iRemove);
		}
	}

	void appendNumber(QString & szSrc, double dReal)
	{
#ifdef COMPILE_ON_MINGW
		//workaround for #801
		QString cBuffer;
		cBuffer.setNum(dReal, 'f');
#else
		char cBuffer[512];
		::sprintf(cBuffer, "%f", dReal);
#endif
		szSrc.append(cBuffer);
	}

	void appendNumber(QString & szSrc, int iInteger)
	{
		char cBuffer[64];
		::sprintf(cBuffer, "%d", iInteger);
		szSrc.append(cBuffer);
	}

	void appendNumber(QString & szSrc, kvi_i64_t iInteger)
	{
		char cBuffer[64];
		::sprintf(cBuffer, "%lld", (long long int)iInteger);
		szSrc.append(cBuffer);
	}

	void appendNumber(QString & szSrc, kvi_u64_t uInteger)
	{
		char cBuffer[64];
		::sprintf(cBuffer, "%llu", (long long unsigned int)uInteger);
		szSrc.append(cBuffer);
	}

	void appendNumber(QString & szSrc, unsigned int uInteger)
	{
		char cBuffer[64];
		::sprintf(cBuffer, "%u", uInteger);
		szSrc.append(cBuffer);
	}

	void vsprintf(QString & szSrc, const QString & szFmt, kvi_va_list list)
	{
#define MEMINCREMENT 32

		int iRealLen = 0;
		int iAllocSize = MEMINCREMENT;

		//szSrc.resize(allocsize);

		const QChar * pFmt = szFmt.constData();
		if(!pFmt)
		{
			szSrc = QString();
			return;
		}

		QChar * pBuffer = (QChar *)KviMemory::allocate(sizeof(QChar) * iAllocSize);
		//QChar * p = (QChar *)s.unicode();

		char * pcArgString;
		long lArgValue;
		unsigned long ulArgValue;

		//9999999999999999999999999999999\0
		char cNumberBuffer[32]; //enough ? 10 is enough for 32bit unsigned int...
		char * pcNumBuf;
		unsigned int iTmp;

		QChar * p = pBuffer;

#define INCREMENT_MEM                                                                  \
	{                                                                                  \
		iAllocSize += MEMINCREMENT;                                                    \
		pBuffer = (QChar *)KviMemory::reallocate(pBuffer, sizeof(QChar) * iAllocSize); \
		p = pBuffer + iRealLen;                                                        \
	}

#define INCREMENT_MEM_BY(numchars)                                                     \
	{                                                                                  \
		iAllocSize += numchars + MEMINCREMENT;                                         \
		pBuffer = (QChar *)KviMemory::reallocate(pBuffer, sizeof(QChar) * iAllocSize); \
		p = pBuffer + iRealLen;                                                        \
	}

		for(; pFmt->unicode(); ++pFmt)
		{
			if(iRealLen == iAllocSize)
				INCREMENT_MEM

			//copy up to a '%'
			if(pFmt->unicode() != '%')
			{
				*p++ = *pFmt;
				iRealLen++;
				continue;
			}

			++pFmt; //skip this '%'
			switch(pFmt->unicode())
			{
				case 's': // char * string
				{
					pcArgString = kvi_va_arg(list, char *);
					if(!pcArgString)
					{
						pcArgString = (char *)"[!NULL!]";
					}
					QString szStr(pcArgString);
					if(szStr.isEmpty())
					{
						continue;
					}
					int iLen = szStr.length();
					const QChar * pCh = szStr.unicode();
					if(!pCh)
					{
						continue;
					}
					if((iAllocSize - iRealLen) < iLen)
					{
						INCREMENT_MEM_BY(iLen)
					}
					while(iLen--)
					{
						*p++ = *pCh++;
					}
					iRealLen += szStr.length();
					continue;
				}
				case 'S': // KviCString * string
				{
					KviCString * szStr = kvi_va_arg(list, KviCString *);
					if(!szStr)
					{
						continue;
					}
					if((iAllocSize - iRealLen) < szStr->len())
					{
						INCREMENT_MEM_BY(szStr->len())
					}
					pcArgString = szStr->ptr();
					while(*pcArgString)
					{
						*p++ = QChar(*pcArgString++);
					}
					iRealLen += szStr->len();
					continue;
				}
				case 'Q': // QString * string
				{
					QString * szStr = kvi_va_arg(list, QString *);
					if(!szStr)
					{
						continue;
					}
					if(szStr->isEmpty())
					{
						continue;
					}
					int iLen = szStr->length();
					const QChar * pCh = szStr->unicode();
					if(!pCh)
					{
						continue;
					}
					if((iAllocSize - iRealLen) < iLen)
					{
						INCREMENT_MEM_BY(iLen)
					}
					while(iLen--)
					{
						*p++ = *pCh++;
					}
					iRealLen += szStr->length();
					continue;
				}
				case 'c': //char
				{
					//
					// I'm not sure about this...
					// In the linux kernel source the
					// unsigned char is extracted from an integer type.
					// We assume that gcc stacks a char argument
					// as sizeof(int) bytes value.
					// Is this always true ?
					//
					*p++ = (char)kvi_va_arg(list, int);
					iRealLen++;
					continue;
				}
				case 'q': // QChar *
				{
					//
					// I'm not sure about this...
					// In the linux kernel source the
					// unsigned char is extracted from an integer type.
					// We assume that gcc stacks a char argument
					// as sizeof(int) bytes value.
					// Is this always true ?
					//
					*p++ = *((QChar *)kvi_va_arg(list, QChar *));
					iRealLen++;
					continue;
				}
				case 'd': //signed integer
				{
					lArgValue = kvi_va_arg(list, int);
					if(lArgValue < 0)
					{
						//negative integer
						*p++ = '-';
						iRealLen++;
						//need to have it positive. most negative integer exception (avoid completely senseless (non digit) responses)
						lArgValue = -lArgValue;
						if(lArgValue < 0)
						{
							//we get -0 here
							lArgValue = 0;
						}
					}
					//write the number in a temporary buffer
					pcNumBuf = cNumberBuffer;
					do
					{
						iTmp = lArgValue / 10;
						*pcNumBuf++ = lArgValue - (iTmp * 10) + '0';
					} while((lArgValue = iTmp));
					//copy now....
					ulArgValue = pcNumBuf - cNumberBuffer; //length of the number string
					if((iAllocSize - iRealLen) < (int)ulArgValue)
					{
						INCREMENT_MEM_BY(ulArgValue)
					}
					do
					{
						*p++ = QChar(*--pcNumBuf);
					} while(pcNumBuf != cNumberBuffer);
					iRealLen += ulArgValue;
					continue;
				}
				case 'u': //unsigned integer
				{
					ulArgValue = kvi_va_arg(list, unsigned int); //many implementations place int here
					//write the number in a temporary buffer
					pcNumBuf = cNumberBuffer;
					do
					{
						iTmp = ulArgValue / 10;
						*pcNumBuf++ = ulArgValue - (iTmp * 10) + '0';
					} while((ulArgValue = iTmp));
					//copy now....
					lArgValue = pcNumBuf - cNumberBuffer; //length of the number string
					if((iAllocSize - iRealLen) < lArgValue)
					{
						INCREMENT_MEM_BY(lArgValue)
					}
					do
					{
						*p++ = *--pcNumBuf;
					} while(pcNumBuf != cNumberBuffer);
					iRealLen += lArgValue;
					continue;
				}
				case 'h':
				case 'x': // hexadecimal unsigned integer
				{
					static char cHexSmallDigits[] = "0123456789abcdef";
					ulArgValue = kvi_va_arg(list, unsigned int); //many implementations place int here
					//write the number in a temporary buffer
					pcNumBuf = cNumberBuffer;
					do
					{
						iTmp = ulArgValue / 16;
						*pcNumBuf++ = cHexSmallDigits[ulArgValue - (iTmp * 16)];
					} while((ulArgValue = iTmp));
					//copy now....
					lArgValue = pcNumBuf - cNumberBuffer; //length of the number string
					if((iAllocSize - iRealLen) < lArgValue)
					{
						INCREMENT_MEM_BY(lArgValue)
					}
					do
					{
						*p++ = *--pcNumBuf;
					} while(pcNumBuf != cNumberBuffer);
					iRealLen += lArgValue;
					continue;
				}
				case 'H':
				case 'X': // hexadecimal unsigned integer
				{
					static char cHexBigDigits[] = "0123456789ABCDEF";
					ulArgValue = kvi_va_arg(list, unsigned int); //many implementations place int here
					//write the number in a temporary buffer
					pcNumBuf = cNumberBuffer;
					do
					{
						iTmp = ulArgValue / 16;
						*pcNumBuf++ = cHexBigDigits[ulArgValue - (iTmp * 16)];
					} while((ulArgValue = iTmp));
					//copy now....
					lArgValue = pcNumBuf - cNumberBuffer; //length of the number string
					if((iAllocSize - iRealLen) < lArgValue)
					{
						INCREMENT_MEM_BY(lArgValue)
					}
					do
					{
						*p++ = *--pcNumBuf;
					} while(pcNumBuf != cNumberBuffer);
					iRealLen += lArgValue;
					continue;
				}
				default:
				{
					// a normal percent followed by some char
					*p++ = '%'; //write it
					iRealLen++;
					if(pFmt->unicode())
					{
						if(iRealLen == iAllocSize)
						{
							INCREMENT_MEM
						}
						*p++ = *pFmt;
						iRealLen++;
					}
					continue;
				}
			}
		}

		szSrc.setUnicode(pBuffer, iRealLen);
		KviMemory::free(pBuffer);
		//szSrc.squeeze();
	}

	void appendFormatted(QString & szSrc, QString szFmt, ...)
	{
		QString szTmp;
		kvi_va_list list;
		kvi_va_start(list, szFmt);
		//print...with max 256 chars
		KviQString::vsprintf(szTmp, szFmt, list);
		kvi_va_end(list);
		szSrc.append(szTmp);
	}

	bool matchWildExpressions(const QString & szM1, const QString & szM2)
	{
		//          s1
		//          pM1
		// mask1 : *xor
		// mask2 : xorand*xor
		//         m2
		//          s2

		//                        s2
		//                       m2
		//                       |
		// XorT!xor@111.111.111.11
		//
		// *!*@*.net
		//      |
		//      pM1
		//      s1
		//

		const QChar * pM1 = (const QChar *)szM1.constData();
		const QChar * pM2 = (const QChar *)szM2.constData();

		if(!(pM1 && pM2 && (pM1->unicode())))
		{
			return false;
		}

		const QChar * pSavePos1 = nullptr;
		const QChar * pSavePos2 = pM2;
		while(pM1->unicode())
		{
			//loop managed by pM1 (initially first mask)
			if(pM1->unicode() == '*')
			{
				//Found a wildcard in pM1
				//move to the next char and save the position...this is our jolly
				pSavePos1 = ++pM1;
				if(!pSavePos1->unicode())
				{
					//last was a wildcard, matches everything ahead...
					return true;
				}
				//next return state for the second string
				pSavePos2 = pM2 + 1;
				//and return
				continue;
			}
			if(!pM2->unicode())
			{
				//pM2 finished and we had something to match here!
				return false;
			}
			if(pM1->toLower() == pM2->toLower())
			{
				//chars matched. go ahead in the two strings
				pM1++;
				pM2++;
				if((!(pM1->unicode())) && pM2->unicode() && pSavePos1)
				{
					//pM1 finished, but pM2 not yet and we have a savePosition for pM1 (there was a wildcard)...
					//retry matching the string following the * from the savePos2 (one char ahead last time)
					//back to char after wildcard
					pM1 = pSavePos1;
					//back to last savePos2
					pM2 = pSavePos2;
					//next savePos2 will be next char
					pSavePos2++;
				}
			}
			else
			{
				if(pM2->unicode() == '*')
				{
					//A wlidcard in the second string
					//Invert the game : mask1 <-> mask2
					//mask2 now leads the game...
					//aux
					pSavePos1 = pM1;
					//...swap
					pM1 = pM2;
					//...swap
					pM2 = pSavePos1;
					//sync save pos1
					pSavePos1 = pM1;
					//sync save pos2
					pSavePos2 = pM2 + 1;
					//...and again
					continue;
				}
				// pM1 != pM2, pM1 != *, pM2 != *
				if((pM1->unicode() == '?') || (pM2->unicode() == '?'))
				{
					pM1++;
					pM2++;
					if((!(pM1->unicode())) && pM2->unicode() && pSavePos1)
					{
						//pM1 finished, but pM2 not yet and we have a savePosition for pM1 (there was a wildcard)...
						//retry matching the string following the * from the pSavePos2 (one char ahead last time)
						//back to char after wildcard
						pM1 = pSavePos1;
						//back to last pSavePos2
						pM2 = pSavePos2;
						//next pSavePos2 will be next char
						pSavePos2++;
					}
				}
				else
				{
					if(pSavePos1)
					{
						//Have a jolly man...allow not matching...
						//go back to char after wildcard...need to rematch...
						pM1 = pSavePos1;
						//back to last pSavePos2
						pM2 = pSavePos2;
						//and set next pSavePos2
						pSavePos2++;
					}
					else
					{
						//No previous wildcards...not matched!
						return false;
					}
				}
			}
		}
		//pM1 surely finished, so for the match, pM2 must be finished too
		return (!(pM2->unicode()));
	}

	bool matchString(const QString & szExp, const QString & szStr, bool bIsRegExp, bool bExact, bool bCs)
	{
		QString szWildcard;

		if(!bIsRegExp)
		{
			// In wildcard matching mode, QRegExp not only interprets * and ?, but also [ ]
			// This workaround embeds square brackes in square brackets (ticket #1264)

			QChar * pPtr = (QChar *)szExp.constData();

			if(!pPtr)
				return false;

			while(pPtr->unicode())
			{
				if((pPtr->unicode() == '[') || (pPtr->unicode() == ']'))
				{
					szWildcard.append("[");
					szWildcard.append(*pPtr);
					szWildcard.append("]");
				}
				else
				{
					szWildcard.append(*pPtr);
				}
				pPtr++;
			}
		}
		else
		{
			szWildcard = szExp;
		}

		QRegExp re(szWildcard, bCs ? Qt::CaseSensitive : Qt::CaseInsensitive, bIsRegExp ? QRegExp::RegExp2 : QRegExp::Wildcard);

		if(bExact)
			return re.exactMatch(szStr);
		return re.indexIn(szStr) != -1;
	}

	void cutFromFirst(QString & szSrc, const QChar & c, bool bIncluded)
	{
		int iIdx = szSrc.indexOf(c);

		if(iIdx == -1)
			return;
		szSrc.truncate(bIncluded ? iIdx : iIdx + 1);
	}

	void cutFromFirst(QString & szSrc, const QString & szFind, bool bIncluded)
	{
		int iIdx = szSrc.indexOf(szFind);
		if(iIdx == -1)
			return;
		szSrc.truncate(bIncluded ? iIdx : iIdx + szFind.length());
	}

	void cutFromLast(QString & szSrc, const QChar & c, bool bIncluded)
	{
		int iIdx = szSrc.lastIndexOf(c);

		if(iIdx == -1)
			return;
		szSrc.truncate(bIncluded ? iIdx : iIdx + 1);
	}

	void cutFromLast(QString & szSrc, const QString & szFind, bool bIncluded)
	{
		int iIdx = szSrc.lastIndexOf(szFind);
		if(iIdx == -1)
			return;
		szSrc.truncate(bIncluded ? iIdx : iIdx + szFind.length());
	}

	void cutToFirst(QString & szSrc, const QChar & c, bool bIncluded, bool bClearIfNotFound)
	{
		int iIdx = szSrc.indexOf(c);
		if(iIdx == -1)
		{
			if(bClearIfNotFound)
				szSrc = "";
			return;
		}
		szSrc.remove(0, bIncluded ? iIdx + 1 : iIdx);
	}

	void cutToFirst(QString & szSrc, const QString & szFind, bool bIncluded, bool bClearIfNotFound)
	{
		int iIdx = szSrc.indexOf(szFind);
		if(iIdx == -1)
		{
			if(bClearIfNotFound)
				szSrc = "";
			return;
		}
		szSrc.remove(0, bIncluded ? iIdx + szFind.length() : iIdx);
	}

	void cutToLast(QString & szSrc, const QChar & c, bool bIncluded, bool bClearIfNotFound)
	{
		int iIdx = szSrc.lastIndexOf(c);
		if(iIdx == -1)
		{
			if(bClearIfNotFound)
				szSrc = "";
			return;
		}
		szSrc.remove(0, bIncluded ? iIdx + 1 : iIdx);
	}

	void cutToLast(QString & szSrc, const QString & szFind, bool bIncluded, bool bClearIfNotFound)
	{
		int iIdx = szSrc.lastIndexOf(szFind);
		if(iIdx == -1)
		{
			if(bClearIfNotFound)
				szSrc = "";
			return;
		}
		szSrc.remove(0, bIncluded ? iIdx + szFind.length() : iIdx);
	}

	QString leftToFirst(QString & szSrc, const QChar & c, bool bIncluded, bool bReturnFullStringIfNotFound)
	{
		int iIdx = szSrc.indexOf(c);
		if(iIdx == -1)
			return bReturnFullStringIfNotFound ? szSrc : QString();
		return szSrc.left(bIncluded ? iIdx + 1 : iIdx);
	}

	QString leftToFirst(QString & szSrc, const QString & szFind, bool bIncluded, bool bReturnFullStringIfNotFound)
	{
		int iIdx = szSrc.indexOf(szFind);
		if(iIdx == -1)
			return bReturnFullStringIfNotFound ? szSrc : QString();
		return szSrc.left(bIncluded ? iIdx + 1 : iIdx);
	}

	QString leftToLast(QString & szSrc, const QChar & c, bool bIncluded, bool bReturnFullStringIfNotFound)
	{
		int iIdx = szSrc.lastIndexOf(c);
		if(iIdx == -1)
			return bReturnFullStringIfNotFound ? szSrc : QString();
		return szSrc.left(bIncluded ? iIdx + 1 : iIdx);
	}

	QString leftToLast(QString & szSrc, const QString & szFind, bool bIncluded, bool bReturnFullStringIfNotFound)
	{
		int iIdx = szSrc.lastIndexOf(szFind);
		if(iIdx == -1)
			return bReturnFullStringIfNotFound ? szSrc : QString();
		return szSrc.left(bIncluded ? iIdx + 1 : iIdx);
	}

	QString upperISO88591(const QString & szSrc)
	{
		const QChar * pC = szSrc.constData();
		if(!pC)
		{
			QString szRet;
			return szRet;
		}
		QChar * pBuffer = (QChar *)KviMemory::allocate(sizeof(QChar) * szSrc.length());
		QChar * pB = pBuffer;
		unsigned short usX = pC->unicode();
		while(usX)
		{
			if(usX < 256)
				*pB = QChar((unsigned short)iso88591_toUpper_map[usX]);
			else
				*pB = *pC;
			pC++;
			pB++;
			usX = pC->unicode();
		}
		QString szRet(pBuffer, szSrc.length());
		KviMemory::free(pBuffer);
		return szRet;
	}

	QString lowerISO88591(const QString & szSrc)
	{
		const QChar * pC = szSrc.constData();
		if(!pC)
		{
			QString szRet;
			return szRet;
		}
		QChar * pBuffer = (QChar *)KviMemory::allocate(sizeof(QChar) * szSrc.length());
		QChar * pB = pBuffer;
		unsigned short usX = pC->unicode();
		while(usX)
		{
			if(usX < 256)
			{
				*pB = QChar((unsigned short)iso88591_toLower_map[usX]);
			}
			else
				*pB = *pC;
			pC++;
			pB++;
			usX = pC->unicode();
		}
		QString szRet(pBuffer, szSrc.length());
		KviMemory::free(pBuffer);
		return szRet;
	}

	void transliterate(QString & szSrc, const QString & szToFind, const QString & szReplacement)
	{
		int i = 0;
		int il = MY_MIN(szToFind.length(), szReplacement.length());
		while(i < il)
		{
			int k = 0;
			int kl = szSrc.length();
			while(k < kl)
			{
				if(szSrc[k] == szToFind[i])
				{
					szSrc[k] = szReplacement[i];
				}
				k++;
			}
			i++;
		}
	}

	void bufferToHex(QString & szRetBuffer, const unsigned char * pcBuffer, unsigned int uLen)
	{
		szRetBuffer.resize(uLen * 2);
		unsigned int u = 0;
		while(u < (uLen * 2))
		{
			szRetBuffer[int(u)] = QChar((unsigned int)cHexDigits[(*pcBuffer) / 16]);
			u++;
			szRetBuffer[int(u)] = QChar((unsigned int)cHexDigits[(*pcBuffer) % 16]);
			u++;
			pcBuffer++;
		}
	}

	void escapeKvs(QString * szData, uint uFlags)
	{
		// escape any $;\%
		szData->replace("\\", "\\\\");
		szData->replace("\"", "\\\"");
		szData->replace(";", "\\;");
		if(!(uFlags & PermitFunctions))
			szData->replace("$", "\\$");
		if(!(uFlags & PermitVariables))
			szData->replace("%", "\\%");
		if(!(uFlags & PermitMultiLine))
		{
			szData->replace("\r", " ");
			szData->replace("\n", " ");
		}
		if(uFlags & EscapeSpace)
			szData->replace(" ", "\\ ");

		if(uFlags & EscapeParenthesis)
		{
			szData->replace("(", "\\(");
			szData->replace(")", "\\)");
		}
	}

	QString toHtmlEscaped(QString szData)
	{
		return szData.toHtmlEscaped();
	}
}
