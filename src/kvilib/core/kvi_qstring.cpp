//=============================================================================
//
//   File : kvi_qstring.cpp
//   Creation date : Mon Aug 04 2003 13:36:33 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2003-2008 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

//=============================================================================
//
// Helper functions for the QString class
//
//=============================================================================


#include "kvi_qstring.h"
#include "kvi_string.h"
#include "kvi_malloc.h"
#include "kvi_locale.h"

#include <ctype.h> // for tolower()
#include <stdio.h> // for sprintf()
#include <QRegExp>

// kvi_string.cpp
extern unsigned char iso88591_toLower_map[256];
extern unsigned char iso88591_toUpper_map[256];

#define MY_MIN(a,b) (((a) < (b)) ? (a) : (b))

namespace KviQString
{
	// The global empty (and null) string
	const QString empty;

	bool equalCSN(const QString &sz1,const QString &sz2,unsigned int len)
	{
		if(len == 0)return true; // assume equal
		const QChar * c1 = sz1.unicode();
		const QChar * c2 = sz2.unicode();
		unsigned int lmin = MY_MIN(sz1.length(),sz2.length());
		if(lmin < len)return false;
		const QChar * c1e = c1 + len;

		if(!c1 || !c2)return (c1 == c2);

		while(c1 < c1e)
		{
			if(c1->unicode() != c2->unicode())return false;
			c1++;
			c2++;
		}
		return (c1 == c1e);
	}

	bool equalCIN(const QString &sz1,const QString &sz2,unsigned int len)
	{
		if(len == 0)return true; // assume equal
		const QChar * c1 = sz1.unicode();
		const QChar * c2 = sz2.unicode();
		unsigned int lmin = MY_MIN(sz1.length(),sz2.length());
		if(lmin < len)return false;
		const QChar * c1e = c1 + len;

		if(!c1 || !c2)return (c1 == c2);

		while(c1 < c1e)
		{
			if(c1->toLower().unicode() != c2->toLower().unicode())return false;
			c1++;
			c2++;
		}
		return (c1 == c1e);
	}

	bool equalCSN(const QString &sz1,const char * sz2,unsigned int len)
	{
		if(len == 0)return true; // assume equal
		const QChar * c1 = sz1.unicode();
		if(sz1.length() < len)return false;
		const QChar * c1e = c1 + len;

		if(!sz2)return !c1;
		if(!c1)return !sz2;

		while((c1 < c1e) && (*sz2))
		{
			if(c1->unicode() != *sz2)return false;
			c1++;
			sz2++;
		}
		return (c1 == c1e);
	}

	bool equalCIN(const QString &sz1,const char * sz2,unsigned int len)
	{
		if(len == 0)return true; // assume equal
		const QChar * c1 = sz1.unicode();
		if(sz1.length() < len)return false;
		const QChar * c1e = c1 + len;

		if(!sz2)return !c1;
		if(!c1)return !(*sz2);

		while((c1 < c1e) && (*sz2))
		{
			if(c1->toLower().unicode() != tolower(*sz2))return false;
			c1++;
			sz2++;
		}
		return (c1 == c1e);
	}

	// sz2 is assumed to be null terminated, sz1 is not!
	bool equalCIN(const QString &sz1,const QChar *sz2,unsigned int len)
	{
		if(len == 0)return true; // assume equal
		const QChar * c1 = sz1.unicode();
		if(sz1.length() < len)return false;
		const QChar * c1e = c1 + len;

		if(!sz2)return !c1;
		if(!c1)return !(sz2->unicode());

		while((c1 < c1e) && (sz2->unicode()))
		{
			if(c1->toLower().unicode() != sz2->toLower().unicode())return false;
			c1++;
			sz2++;
		}
		return (c1 == c1e);
	}

	QString makeSizeReadable(size_t bytes)
	{
		double size = bytes;
		if(size<900)
			return QString(__tr2qs("%1 bytes")).arg(size,0,'f',3);

		size/=1024;
		if(size<900)
			return QString(__tr2qs("%1 KB")).arg(size,0,'f',3);

		size/=1024;
		if(size<900)
			return QString(__tr2qs("%1 MB")).arg(size,0,'f',3);

		//Pirated DVD?;)
		size/=1024;
		if(size<900)
			return QString(__tr2qs("%1 GB")).arg(size,0,'f',3);

		//Uhm.. We are downloading a whole internet:)))
		size/=1024;
		return QString(__tr2qs("%1 TB")).arg(size,0,'f',3);
	}

	bool equalCS(const QString &sz1,const QString &sz2)
	{
		if(sz1.length() != sz2.length())return false;

		const QChar * c1 = sz1.unicode();
		const QChar * c2 = sz2.unicode();
		const QChar * c1e = c1 + sz1.length();

		if(!c1 || !c2)return (c1 == c2);

		while(c1 < c1e)
		{
			if(c1->unicode() != c2->unicode())return false;
			c1++;
			c2++;
		}
		return (c1 == c1e);
	}

	bool equalCI(const QString &sz1,const QString &sz2)
	{
		if(sz1.length() != sz2.length())return false;

		const QChar * c1 = sz1.unicode();
		const QChar * c2 = sz2.unicode();
		const QChar * c1e = c1 + sz1.length();

		if(!c1 || !c2)return (c1 == c2);

		while(c1 < c1e)
		{
			if(c1->toLower().unicode() != c2->toLower().unicode())return false;
			c1++;
			c2++;
		}
		return (c1 == c1e);
	}

	// sz2 is assumed to be null terminated, sz1 is not!
	bool equalCI(const QString &sz1,const QChar *sz2)
	{
		const QChar * c1 = sz1.unicode();
		const QChar * c1e = c1 + sz1.length();

		if(!c1 || !sz2)return (c1 == sz2);

		while(c1 < c1e)
		{
			if(!sz2->unicode())return false; // sz1 has at least another character
			if(c1->toLower().unicode() != sz2->toLower().unicode())return false;
			c1++;
			sz2++;
		}
		return (c1 == c1e) && (!sz2->unicode());
	}

	bool equalCS(const QString &sz1,const char * sz2)
	{
		const QChar * c1 = sz1.unicode();
		const QChar * c1e = c1 + sz1.length();

		if(!c1)return !sz2;

		while((c1 < c1e) && (*sz2))
		{
			if(c1->unicode() != *sz2)return false;
			c1++;
			sz2++;
		}
		return ((c1 == c1e) && (*sz2 == '\0'));
	}

	bool equalCI(const QString &sz1,const char * sz2)
	{
		const QChar * c1 = sz1.unicode();
		const QChar * c1e = c1 + sz1.length();

		if(!c1)return !sz2;

		while((c1 < c1e) && (*sz2))
		{
			if(c1->toLower().unicode() != tolower(*sz2))return false;
			c1++;
			sz2++;
		}
		return ((c1 == c1e) && (*sz2 == '\0'));
	}

	int cmpCS(const QString &sz1,const QString &sz2)
	{
		const QChar * c1 = sz1.unicode();
		const QChar * c2 = sz2.unicode();
		const QChar * c1e = c1 + sz1.length();
		const QChar * c2e = c2 + sz2.length();

		if(!c1)
		{
			if(!c2)return 0;
			return -1;
		}
		if(!c2)return 1;


		for(;;)
		{
			if(c1 >= c1e)
			{
				if(c2 < c2e)return /* 0 */ - (c2->unicode());
				return 0;
			}
			if(c2 >= c2e)return c1->unicode() /* - 0 */;

			int diff = c1->unicode() - c2->unicode();
			if(diff)return diff;

			c1++;
			c2++;
		}

		return 0; // never here
	}

	int cmpCI(const QString &sz1,const QString &sz2,bool nonAlphaGreater)
	{
		const QChar * c1 = sz1.unicode();
		const QChar * c2 = sz2.unicode();
		const QChar * c1e = c1 + sz1.length();
		const QChar * c2e = c2 + sz2.length();

		if(!c1)
		{
			if(!c2)return 0;
			return -1;
		}
		if(!c2)return 1;

		for(;;)
		{
			if(nonAlphaGreater) {
				if(c1->isLetterOrNumber() && !c2->isLetterOrNumber()) {
					return -1;
				} else if(!c1->isLetterOrNumber() && c2->isLetterOrNumber()) {
					return 1;
				}
			}

			if(c1 >= c1e)
			{
				if(c2 < c2e)return /* 0 */ - (c2->toLower().unicode());
				return 0;
			}

			if(c2 >= c2e)return c1->toLower().unicode() /* - 0 */;

			int diff = c1->toLower().unicode() - c2->toLower().unicode();

			if(diff)return diff;

			c1++;
			c2++;
		}

		return 0; // never here
	}

	int cmpCIN(const QString &sz1,const QString &sz2,unsigned int len)
	{
		if(len == 0)return 0; // assume equal
		unsigned int l1 = MY_MIN(len,sz1.length());
		unsigned int l = MY_MIN(l1,sz2.length()); // FIXME: THIS IS NOT OK

		const QChar * c1 = sz1.unicode();
		const QChar * c2 = sz2.unicode();
		const QChar * c1e = c1 + l;

		if(!c1)
		{
			if(!c2)return 0;
			return -1;
		}
		if(!c2)return 1;

		int diff = 0;

		while((c1 < c1e) && !(diff = (c1->toLower().unicode() - c2->toLower().unicode())))
		{
			c1++;
			c2++;
		}

		return diff;
	}

	void ensureLastCharIs(QString &szString,const QChar &c)
	{
		if(!lastCharIs(szString,c))szString.append(c);
	}

	QString getToken(QString &szString,const QChar &sep)
	{
		int i=0;
		while(i < szString.length())
		{
			if(szString[i] == sep)break;
			i++;
		}
		QString ret;
		if(i == szString.length())
		{
			ret = szString;
			szString = "";
		} else {
			ret = szString.left(i);
			while(i < szString.length())
			{
				if(szString[i] != sep)break;
				i++;
			}
			if(i == szString.length())szString = "";
			else szString.remove(0,i);
		}
		return ret;
	}

	void stripRightWhiteSpace(QString &s)
	{
		int iRemove = 0;
		while(iRemove < s.length())
		{
			if(s.at(s.length() - (iRemove + 1)).isSpace())iRemove++;
			else break;
		}
		if(iRemove > 0)s.remove(s.length() - iRemove,iRemove);
	}

	void stripRight(QString &s,const QChar &c)
	{
		int iRemove = 0;
		while(iRemove < s.length())
		{
			if(s.at(s.length() - (iRemove + 1)) == c)iRemove++;
			else break;
		}
		if(iRemove > 0)s.remove(s.length() - iRemove,iRemove);
	}

	void stripLeft(QString &s,const QChar &c)
	{
		int iRemove = 0;
		while(iRemove < s.length())
		{
			if(s[iRemove] == c)
				iRemove++;
			else
				break;
		}
		if(iRemove > 0)s.remove(0,iRemove);
	}

	void detach(QString &sz)
	{
		sz.resize(sz.length());
	}

	const QChar * nullTerminatedArray(const QString &sz)
	{
		//sz.resize(sz.length()); // detach!
		return sz.constData();
	}

	void appendNumber(QString &s,double dReal)
	{
		char buffer[512];
		::sprintf(buffer,"%f",dReal);
		s.append(buffer);
	}

	void appendNumber(QString &s,int iInteger)
	{
		char buffer[64];
		::sprintf(buffer,"%d",iInteger);
		s.append(buffer);
	}

	void appendNumber(QString &s,kvi_i64_t iInteger)
	{
		char buffer[64];
		::sprintf(buffer,"%ld",iInteger);
		s.append(buffer);
	}

	void appendNumber(QString &s,kvi_u64_t uInteger)
	{
		char buffer[64];
		::sprintf(buffer,"%lu",uInteger);
		s.append(buffer);
	}

	void appendNumber(QString &s,unsigned int uInteger)
	{
		char buffer[64];
		::sprintf(buffer,"%u",uInteger);
		s.append(buffer);
	}

	void vsprintf(QString &s,const QString &szFmt,kvi_va_list list)
	{
#define MEMINCREMENT 32

		int reallen = 0;
		int allocsize = MEMINCREMENT;

		//s.resize(allocsize);

		const QChar * fmt = nullTerminatedArray(szFmt);
		if(!fmt)
		{
			s = QString::null;
			return;
		}

		QChar * buffer = (QChar *)kvi_malloc(sizeof(QChar) * allocsize);
		//QChar * p = (QChar *)s.unicode();

		char *argString;
		long argValue;
		unsigned long argUValue;

		//9999999999999999999999999999999\0
		char numberBuffer[32]; //enough ? 10 is enough for 32bit unsigned int...
		char *pNumBuf;
		unsigned int tmp;

		QChar * p = buffer;

#define INCREMENT_MEM \
		{ \
			allocsize += MEMINCREMENT; \
			buffer = (QChar *)kvi_realloc(buffer,sizeof(QChar) * allocsize); \
			p = buffer + reallen; \
		}

#define INCREMENT_MEM_BY(numchars) \
		{ \
			allocsize += numchars + MEMINCREMENT; \
			buffer = (QChar *)kvi_realloc(buffer,sizeof(QChar) * allocsize); \
			p = buffer + reallen; \
		}


		for(; fmt->unicode() ; ++fmt)
		{
			if(reallen == allocsize)INCREMENT_MEM

			//copy up to a '%'
			if(fmt->unicode() != '%')
			{
				*p++ = *fmt;
				reallen++;
				continue;
			}

			++fmt; //skip this '%'
			switch(fmt->unicode())
			{
				case 's': // char * string
				{
					argString = kvi_va_arg(list,char *);
					if(!argString)argString = (char*) "[!NULL!]";
					QString str(argString);
					if(str.isEmpty())continue;
					int len = str.length();
					const QChar * ch = str.unicode();
					if(!ch)continue;
					if((allocsize - reallen) < len)INCREMENT_MEM_BY(len)
					while(len--)*p++ = *ch++;
					reallen += str.length();
					continue;
				}
				case 'S': // KviStr * string
				{
					KviStr * str = kvi_va_arg(list,KviStr *);
					if(!str)continue;
					if((allocsize - reallen) < str->len())INCREMENT_MEM_BY(str->len())
					argString = str->ptr();
					while(*argString)*p++ = QChar(*argString++);
					reallen += str->len();
					continue;
				}
				case 'Q': // QString * string
				{
					QString * str = kvi_va_arg(list,QString *);
					if(!str)continue;
					if(str->isEmpty())continue;
					int len = str->length();
					const QChar * ch = str->unicode();
					if(!ch)continue;
					if((allocsize - reallen) < len)INCREMENT_MEM_BY(len)
					while(len--)*p++ = *ch++;
					reallen += str->length();
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
					*p++ = (char)kvi_va_arg(list,int);
					reallen++;
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
					*p++ = *((QChar *)kvi_va_arg(list,QChar *));
					reallen++;
					continue;
				}
				case 'd': //signed integer
				{
					argValue = kvi_va_arg(list,int);
					if(argValue < 0)
					{ //negative integer
						*p++ = '-';
						reallen++;
						argValue = -argValue; //need to have it positive
						// most negative integer exception (avoid completely senseless (non digit) responses)
						if(argValue < 0)argValue = 0;  //we get -0 here
					}
					//write the number in a temporary buffer
					pNumBuf = numberBuffer;
					do {
						tmp = argValue / 10;
						*pNumBuf++ = argValue - (tmp * 10) + '0';
					} while((argValue = tmp));
					//copy now....
					argUValue = pNumBuf - numberBuffer; //length of the number string
					if((allocsize - reallen) < (int)argUValue)INCREMENT_MEM_BY(argUValue)
					do { *p++ = QChar(*--pNumBuf); } while(pNumBuf != numberBuffer);
					reallen += argUValue;
					continue;
				}
				case 'u': //unsigned integer
				{
					argUValue = kvi_va_arg(list,unsigned int); //many implementations place int here
					//write the number in a temporary buffer
					pNumBuf = numberBuffer;
					do {
						tmp = argUValue / 10;
						*pNumBuf++ = argUValue - (tmp * 10) + '0';
					} while((argUValue = tmp));
					//copy now....
					argValue = pNumBuf - numberBuffer; //length of the number string
					if((allocsize - reallen) < argValue)INCREMENT_MEM_BY(argValue)
					do { *p++ = *--pNumBuf; } while(pNumBuf != numberBuffer);
					reallen += argValue;
					continue;
				}
				case 'h':
				case 'x': // hexadecimal unsigned integer
				{
					static char hexsmalldigits[]="0123456789abcdef";
					argUValue = kvi_va_arg(list,unsigned int); //many implementations place int here
					//write the number in a temporary buffer
					pNumBuf = numberBuffer;
					do {
						tmp = argUValue / 16;
						*pNumBuf++ = hexsmalldigits[argUValue - (tmp * 16)];
					} while((argUValue = tmp));
					//copy now....
					argValue = pNumBuf - numberBuffer; //length of the number string
					if((allocsize - reallen) < argValue)INCREMENT_MEM_BY(argValue)
					do { *p++ = *--pNumBuf; } while(pNumBuf != numberBuffer);
					reallen += argValue;
					continue;
				}
				case 'H':
				case 'X': // hexadecimal unsigned integer
				{
					static char hexbigdigits[]="0123456789ABCDEF";
					argUValue = kvi_va_arg(list,unsigned int); //many implementations place int here
					//write the number in a temporary buffer
					pNumBuf = numberBuffer;
					do {
						tmp = argUValue / 16;
						*pNumBuf++ = hexbigdigits[argUValue - (tmp * 16)];
					} while((argUValue = tmp));
					//copy now....
					argValue = pNumBuf - numberBuffer; //length of the number string
					if((allocsize - reallen) < argValue)INCREMENT_MEM_BY(argValue)
					do { *p++ = *--pNumBuf; } while(pNumBuf != numberBuffer);
					reallen += argValue;
					continue;
				}
				default: //a normal percent followed by some char
				{
					*p++ = '%';  //write it
					reallen++;
					if(fmt->unicode())
					{
						if(reallen == allocsize)INCREMENT_MEM
						*p++ = *fmt;
						reallen++;
					}
					continue;
				}
			}
		}

		s.setUnicode(buffer,reallen);
		kvi_free(buffer);
		//s.squeeze();
	}


	QString & sprintf(QString &s,const QString &szFmt,...)
	{
		kvi_va_list list;
		kvi_va_start_by_reference(list,szFmt);
		//print...with max 256 chars
		KviQString::vsprintf(s,szFmt,list);
		kvi_va_end(list);
		return s;
	}

	void appendFormatted(QString &s,const QString &szFmt,...)
	{
		QString tmp;
		kvi_va_list list;
		kvi_va_start_by_reference(list,szFmt);
		//print...with max 256 chars
		KviQString::vsprintf(tmp,szFmt,list);
		kvi_va_end(list);
		s.append(tmp);
	}

	bool matchWildExpressionsCI(const QString &szM1,const QString &szM2)
	{
		//Matches two regular expressions containging wildcards (* and ?)

		//          s1
		//          m1
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
		//      m1
		//      s1
		//

		const QChar * m1 = (const QChar *)szM1.constData();
		const QChar * m2 = (const QChar *)szM2.constData();

		if(!(m1 && m2 && (m1->unicode())))return false;
		const QChar * savePos1 = 0;
		const QChar * savePos2 = m2;
		while(m1->unicode())
		{
			//loop managed by m1 (initially first mask)
			if(m1->unicode()=='*')
			{
				//Found a wildcard in m1
				savePos1 = ++m1;            //move to the next char and save the position...this is our jolly
				if(!savePos1->unicode())return true;  //last was a wildcard , matches everything ahead...
				savePos2 = m2+1;            //next return state for the second string
				continue;                   //and return
			}
			if(!m2->unicode())return false;         //m2 finished and we had something to match here!
			if(m1->toLower()==m2->toLower())
			{
				//chars matched
				m1++;                       //Go ahead in the two strings
				m2++;                       //
				if((!(m1->unicode())) && m2->unicode() && savePos1)
				{
					//m1 finished , but m2 not yet and we have a savePosition for m1 (there was a wildcard)...
					//retry matching the string following the * from the savePos2 (one char ahead last time)
					m1 = savePos1;          //back to char after wildcard
					m2 = savePos2;          //back to last savePos2
					savePos2++;             //next savePos2 will be next char
				}
			} else {
				if(m2->unicode() == '*')
				{
					//A wlidcard in the second string
					//Invert the game : mask1 <-> mask2
					//mask2 now leads the game...
					savePos1 = m1;          //aux
					m1 = m2;                //...swap
					m2 = savePos1;          //...swap
					savePos1 = m1;          //sync save pos1
					savePos2 = m2 + 1;      //sync save pos2
					continue;               //...and again
				}
				// m1 != m2 , m1 != * , m2 != *
				if((m1->unicode() == '?') || (m2->unicode() == '?'))
				{
					m1++;
					m2++;
					if((!(m1->unicode())) && m2->unicode() && savePos1)
					{
						//m1 finished , but m2 not yet and we have a savePosition for m1 (there was a wildcard)...
						//retry matching the string following the * from the savePos2 (one char ahead last time)
						m1 = savePos1;          //back to char after wildcard
						m2 = savePos2;          //back to last savePos2
						savePos2++;             //next savePos2 will be next char
					}
				} else {
					if(savePos1)
					{
						//Have a jolly man...allow not matching...
						m1 = savePos1;          //go back to char after wildcard...need to rematch...
						m2 = savePos2;          //back to last savePos2
						savePos2++;             //and set next savePos2
					} else return false;        //No previous wildcards...not matched!
				}
			}
		}
		return (!(m2->unicode()));           //m1 surely finished , so for the match , m2 must be finished too
	}

	bool matchStringCI(const QString &szExp,const QString &szStr,bool bIsRegExp,bool bExact)
	{
		QString szWildcard;
		QChar* ptr=(QChar*)szExp.constData();

		if(!ptr) return 0;
		while(ptr->unicode())
		{
			if((ptr->unicode()=='[') || (ptr->unicode()==']'))
			{
				szWildcard.append("[");
				szWildcard.append(*ptr);
				szWildcard.append("]");
			} else {
				szWildcard.append(*ptr);
			}
			ptr++;
		}
		QRegExp re(szWildcard,Qt::CaseInsensitive,bIsRegExp ? QRegExp::RegExp : QRegExp::Wildcard);

		if(bExact) return re.exactMatch(szStr);
		return re.indexIn(szStr) != -1;
	}

	bool matchStringCS(const QString &szExp,const QString &szStr,bool bIsRegExp,bool bExact)
	{
		QString szWildcard;
		QChar* ptr=(QChar*)szExp.constData();

		if(!ptr) return 0;
		while(ptr->unicode())
		{
			if((ptr->unicode()=='[')) // <-- hum ?
			{
				szWildcard.append("[");
				szWildcard.append(*ptr);
				szWildcard.append("]");
			} else {
				szWildcard.append(*ptr);
			}
			ptr++;
		}
		QRegExp re(szWildcard,Qt::CaseSensitive,bIsRegExp ? QRegExp::RegExp : QRegExp::Wildcard);

		if(bExact) return re.exactMatch(szStr);
		return re.indexIn(szStr) != -1;
	}

	void cutFromFirst(QString &s,const QChar &c,bool bIncluded)
	{
		int idx = s.indexOf(c);

		if(idx == -1)return;
		s.truncate(bIncluded ? idx : idx + 1);
	}

	void cutFromLast(QString &s,const QChar &c,bool bIncluded)
	{
		int idx = s.lastIndexOf(c);

		if(idx == -1)return;
		s.truncate(bIncluded ? idx : idx + 1);
	}

	void cutToFirst(QString &s,const QChar &c,bool bIncluded,bool bClearIfNotFound)
	{
		int idx = s.indexOf(c);
		if(idx == -1)
		{
			if(bClearIfNotFound)s = "";
			return;
		}
		s.remove(0,bIncluded ? idx + 1 : idx);
	}

	void cutToLast(QString &s,const QChar &c,bool bIncluded,bool bClearIfNotFound)
	{
		int idx = s.lastIndexOf(c);
		if(idx == -1)
		{
			if(bClearIfNotFound)s = "";
			return;
		}
		s.remove(0,bIncluded ? idx + 1 : idx);
	}

	void cutFromFirst(QString &s,const QString &c,bool bIncluded)
	{
		int idx = s.indexOf(c);
		if(idx == -1)return;
		s.truncate(bIncluded ? idx : idx + c.length());
	}

	void cutFromLast(QString &s,const QString &c,bool bIncluded)
	{
		int idx = s.lastIndexOf(c);
		if(idx == -1)return;
		s.truncate(bIncluded ? idx : idx + c.length());
	}

	void cutToFirst(QString &s,const QString &c,bool bIncluded,bool bClearIfNotFound)
	{
		int idx = s.indexOf(c);
		if(idx == -1)
		{
			if(bClearIfNotFound)s = "";
			return;
		}
		s.remove(0,bIncluded ? idx + c.length() : idx);
	}

	void cutToLast(QString &s,const QString &c,bool bIncluded,bool bClearIfNotFound)
	{
		int idx = s.lastIndexOf(c);
		if(idx == -1)
		{
			if(bClearIfNotFound)s = "";
			return;
		}
		s.remove(0,bIncluded ? idx + c.length() : idx);
	}

	QString upperISO88591(const QString &szSrc)
	{
		const QChar * c = nullTerminatedArray(szSrc);
		if(!c)
		{
			QString ret;
			return ret;
		}
		QChar * buffer = (QChar *)kvi_malloc(sizeof(QChar) * szSrc.length());
		QChar * b = buffer;
		unsigned short us = c->unicode();
		while(us)
		{
			if(us < 256)
				*b=QChar((unsigned short)iso88591_toUpper_map[us]);
			else
				*b = *c;
			c++;
			b++;
			us = c->unicode();
		}
		QString ret(buffer,szSrc.length());
		kvi_free(buffer);
		return ret;
	}

	QString lowerISO88591(const QString &szSrc)
	{
		const QChar * c = nullTerminatedArray(szSrc);
		if(!c)
		{
			QString ret;
			return ret;
		}
		QChar * buffer = (QChar *)kvi_malloc(sizeof(QChar) * szSrc.length());
		QChar * b = buffer;
		unsigned short us = c->unicode();
		while(us)
		{
			if(us < 256)
			{
				*b=QChar((unsigned short)iso88591_toLower_map[us]);
			} else
				*b = *c;
			c++;
			b++;
			us = c->unicode();
		}
		QString ret(buffer,szSrc.length());
		kvi_free(buffer);
		return ret;
	}

	void transliterate(QString &s,const QString &szToFind,const QString &szReplacement)
	{
		int i=0;
		int il = MY_MIN(szToFind.length(),szReplacement.length());
		while(i < il)
		{
			int k=0;
			int kl = s.length();
			while(k < kl)
			{
				if(s[k] == szToFind[i])s[k] = szReplacement[i];
				k++;
			}
			i++;
		}
	}

	static char hexdigits[16] = { '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f' };

	void bufferToHex(QString &szRetBuffer,const unsigned char * buffer,unsigned int len)
	{
		szRetBuffer.resize(len * 2);
		unsigned int i=0;
		while(i < (len*2))
		{
			szRetBuffer[int(i)] = QChar( (unsigned int) hexdigits[(*buffer) / 16] );
			i++;
			szRetBuffer[int(i)] = QChar( (unsigned int)hexdigits[(*buffer) % 16] );
			i++;
			buffer++;
		}
	}
};
