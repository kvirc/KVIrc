//=============================================================================
//
//   File : KviCString.cpp
//   Creation date : Fri Mar 19 1999 03:20:45 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 1999-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#define _KVI_STRING_CPP_

#include "KviCString.h"
#include "KviMemory.h"

#include <QString>

static char hexdigits[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

kvi_wslen_t kvi_wstrlen(const kvi_wchar_t * str)
{
	const kvi_wchar_t * ptr = str;
	while(*ptr)
		ptr++;
	return (ptr - str);
}

bool kvi_qstringEqualCI(const QString & s1, const QString & s2)
{
	const QChar * p1 = s1.unicode();
	const QChar * p2 = s2.unicode();
	int l = s1.length() < s2.length() ? s1.length() : s2.length();

	while(l-- && (p1->toLower() == p2->toLower()))
		p1++, p2++;

	if(l == -1)
		return true;
	return false;
}

bool kvi_matchStringCI(const char * exp, const char * str)
{
	//               a
	//               .
	// exp = a*x?mem*a
	// str = arexoxmexamemizazv
	//                         .
	//                         n
	const char * afterWild = nullptr;
	const char * nextStrToCheck = nullptr;

	while(*exp)
	{
		if(*exp == '*')
		{
			// exp is a wildcard...
			afterWild = ++exp;
			nextStrToCheck = str + 1;
			if(!(*exp))
				return true; // and it's the last char in the string: matches everything ahead
			continue;
		}

		if(!(*str))
			return false; // str finished but we had something to match :(

		if(tolower(*exp) == tolower(*str))
		{
			// chars matched
			++exp;
			++str;
			if((!(*exp)) && *str)
				goto check_recovery;
			continue;
		}

		if(*exp == '?')
		{
			// any-char wildcard
			++exp;
			++str;
			continue;
		}

	check_recovery:
		// chars unmatched!!!
		if(afterWild)
		{
			// we had a wildcard in exp...
			// let's use this jolly then
			exp = afterWild;
			str = nextStrToCheck;
			nextStrToCheck++;
			// and try to compare now
			continue;
		}

		return false; // no match :(
	}
	return (!(*str));
}

bool kvi_matchStringCS(const char * exp, const char * str)
{
	//               a
	//               .
	// exp = a*x?mem*a
	// str = arexoxmexamemizazv
	//                         .
	//                         n
	const char * afterWild = nullptr;
	const char * nextStrToCheck = nullptr;

	while(*exp)
	{
		if(*exp == '*')
		{
			// exp is a wildcard...
			afterWild = ++exp;
			nextStrToCheck = str + 1;
			if(!(*exp))
				return true; // and it's the last char in the string: matches everything ahead
			continue;
		}

		if(!(*str))
			return false; // str finished but we had something to match :(

		if(*exp == *str)
		{
			// chars matched
			++exp;
			++str;
			if((!(*exp)) && *str)
				goto check_recovery;
			continue;
		}

		if(*exp == '?')
		{
			// any-char wildcard
			++exp;
			++str;
			continue;
		}

	check_recovery:
		// chars unmatched!!!
		if(afterWild)
		{
			// we had a wildcard in exp...
			// let's use this jolly then
			exp = afterWild;
			str = nextStrToCheck;
			nextStrToCheck++;
			// and try to compare now
			continue;
		}

		return false; // no match :(
	}
	return (!(*str));
}

bool kvi_matchStringWithTerminator(const char * exp, const char * str, char terminator, const char ** r1, const char ** r2)
{
#define NOT_AT_END(__str) (*__str && (*__str != terminator))

	//               a
	//               .
	// exp = a*x?mem*a
	// str = arexoxmexamemizazv
	//                         .
	//                         n
	const char * afterWild = nullptr;
	const char * nextStrToCheck = nullptr;

	while(NOT_AT_END(exp))
	{
		if(*exp == '*')
		{
			// exp is a wildcard...
			afterWild = ++exp;
			nextStrToCheck = str + 1;
			if(!(NOT_AT_END(exp)))
			{
				while(NOT_AT_END(str))
					str++;
				*r1 = exp;
				*r2 = str;
				return true; // and it's the last char in the string: matches everything ahead
			}
			continue;
		}

		if(!(*str))
			return false; // str finished but we had something to match :(

		if(tolower(*exp) == tolower(*str))
		{
			// chars matched
			++exp;
			++str;
			if((!(NOT_AT_END(exp))) && NOT_AT_END(str))
				goto check_recovery;
			continue;
		}

		if(*exp == '?')
		{
			// any-char wildcard
			++exp;
			++str;
			continue;
		}

	check_recovery:
		// chars unmatched!!!
		if(afterWild)
		{
			// we had a wildcard in exp...
			// let's use this jolly then
			exp = afterWild;
			str = nextStrToCheck;
			nextStrToCheck++;
			// and try to compare now
			continue;
		}

		return false; // no match :(
	}
	*r1 = exp;
	*r2 = str;
	return (!(NOT_AT_END(str)));

#undef NOT_AT_END
}

bool kvi_matchWildExpr(const char * m1, const char * m2)
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

	if(!(m1 && m2 && (*m1)))
		return false;
	const char * savePos1 = nullptr;
	const char * savePos2 = m2;
	while(*m1)
	{
		//loop managed by m1 (initially first mask)
		if(*m1 == '*')
		{
			//Found a wildcard in m1
			savePos1 = ++m1; //move to the next char and save the position...this is our jolly
			if(!*savePos1)
				return true;   //last was a wildcard, matches everything ahead...
			savePos2 = m2 + 1; //next return state for the second string
			continue;          //and return
		}
		if(!(*m2))
			return false; //m2 finished and we had something to match here!
		if(tolower(*m1) == tolower(*m2))
		{
			//chars matched
			m1++; //Go ahead in the two strings
			m2++; //
			if((!(*m1)) && *m2 && savePos1)
			{
				//m1 finished, but m2 not yet and we have a savePosition for m1 (there was a wildcard)...
				//retry matching the string following the * from the savePos2 (one char ahead last time)
				m1 = savePos1; //back to char after wildcard
				m2 = savePos2; //back to last savePos2
				savePos2++;    //next savePos2 will be next char
			}
		}
		else
		{
			if(*m2 == '*')
			{
				//A wlidcard in the second string
				//Invert the game : mask1 <-> mask2
				//mask2 now leads the game...
				savePos1 = m1;     //aux
				m1 = m2;           //...swap
				m2 = savePos1;     //...swap
				savePos1 = m1;     //sync save pos1
				savePos2 = m2 + 1; //sync save pos2
				continue;          //...and again
			}
			// m1 != m2, m1 != *, m2 != *
			if((*m1 == '?') || (*m2 == '?'))
			{
				m1++;
				m2++;
				if((!(*m1)) && *m2 && savePos1)
				{
					//m1 finished, but m2 not yet and we have a savePosition for m1 (there was a wildcard)...
					//retry matching the string following the * from the savePos2 (one char ahead last time)
					m1 = savePos1; //back to char after wildcard
					m2 = savePos2; //back to last savePos2
					savePos2++;    //next savePos2 will be next char
				}
			}
			else
			{
				if(savePos1)
				{
					//Have a jolly man...allow not matching...
					m1 = savePos1; //go back to char after wildcard...need to rematch...
					m2 = savePos2; //back to last savePos2
					savePos2++;    //and set next savePos2
				}
				else
					return false; //No previous wildcards...not matched!
			}
		}
	}
	return (!(*m2)); //m1 surely finished, so for the match, m2 must be finished too
}

/*

	WARNING: Don't remove: working code but actually unused in KVIrc
			Later it might become useful

bool kvi_matchWildExprCS(const char *m1,const char *m2)
{
	if(!(m1 && m2 && (*m1)))return false;
	const char * savePos1 = nullptr;
	const char * savePos2 = m2;
	while(*m1){ //loop managed by m1 (initially first mask)
		if(*m1=='*'){
			//Found a wildcard in m1
			savePos1 = ++m1;            //move to the next char and save the position...this is our jolly
			if(!*savePos1)return true;  //last was a wildcard, matches everything ahead...
			savePos2 = m2+1;            //next return state for the second string
			continue;                   //and return
		}
		if(!(*m2))return false;         //m2 finished and we had something to match here!
		if((*m1)==(*m2)){
			//chars matched
			m1++;                       //Go ahead in the two strings
			m2++;                       //
			if((!(*m1)) && *m2 && savePos1){
				//m1 finished, but m2 not yet and we have a savePosition for m1 (there was a wildcard)...
				//retry matching the string following the * from the savePos2 (one char ahead last time)
				m1 = savePos1;          //back to char after wildcard
				m2 = savePos2;          //back to last savePos2
				savePos2++;             //next savePos2 will be next char
			}
		} else {
			if(*m2 == '*'){
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
			if(savePos1){               //Have a jolly man...allow not matching...
				m1 = savePos1;          //go back to char after wildcard...need to rematch...
				m2 = savePos2;          //back to last savePos2
				savePos2++;             //and set next savePos2
			} else return false;        //No previous wildcards...not matched!
		}
	}
	return (!(*m2));                     //m1 surely finished, so for the match, m2 must be finished too

}
*/

bool kvi_matchWildExprWithTerminator(const char * m1, const char * m2, char terminator,
    const char ** r1, const char ** r2)
{
//Matches two regular expressions containging wildcards

#define NOT_AT_END(__str) (*__str && (*__str != terminator))

	bool bSwapped = false;
	if(!(m1 && m2 && (NOT_AT_END(m1))))
		return false;
	const char * savePos1 = nullptr;
	const char * savePos2 = m2;
	while(NOT_AT_END(m1))
	{
		//loop managed by m1 (initially first mask)
		if(*m1 == '*')
		{
			//Found a wildcard in m1
			savePos1 = ++m1; //move to the next char and save the position...this is our jolly
			if(!NOT_AT_END(savePos1))
			{
				//last was a wildcard, matches everything ahead...
				while(NOT_AT_END(m2))
					m2++;
				*r1 = bSwapped ? m2 : m1;
				*r2 = bSwapped ? m1 : m2;
				return true;
			}
			savePos2 = m2 + 1; //next return state for the second string
			continue;          //and return
		}
		if(!NOT_AT_END(m2))
			return false; //m2 finished and we had something to match here!
		if(tolower(*m1) == tolower(*m2))
		{
			//chars matched
			m1++; //Go ahead in the two strings
			m2++; //
			if((!NOT_AT_END(m1)) && NOT_AT_END(m2) && savePos1)
			{
				//m1 finished, but m2 not yet and we have a savePosition for m1 (there was a wildcard)...
				//retry matching the string following the * from the savePos2 (one char ahead last time)
				m1 = savePos1; //back to char after wildcard
				m2 = savePos2; //back to last savePos2
				savePos2++;    //next savePos2 will be next char
			}
		}
		else
		{
			if(*m2 == '*')
			{
				//A wlidcard in the second string
				//Invert the game : mask1 <-> mask2
				//mask2 now leads the game...
				bSwapped = !bSwapped;
				savePos1 = m1;     //aux
				m1 = m2;           //...swap
				m2 = savePos1;     //...swap
				savePos1 = m1;     //sync save pos1
				savePos2 = m2 + 1; //sync save pos2
				continue;          //...and again
			}
			// m1 != m2, m1 != *, m2 != *
			if((*m1 == '?') || (*m2 == '?'))
			{
				m1++;
				m2++;
				if((!NOT_AT_END(m1)) && NOT_AT_END(m2) && savePos1)
				{
					//m1 finished, but m2 not yet and we have a savePosition for m1 (there was a wildcard)...
					//retry matching the string following the * from the savePos2 (one char ahead last time)
					m1 = savePos1; //back to char after wildcard
					m2 = savePos2; //back to last savePos2
					savePos2++;    //next savePos2 will be next char
				}
			}
			else
			{
				if(savePos1)
				{
					//Have a jolly man...allow not matching...
					m1 = savePos1; //go back to char after wildcard...need to rematch...
					m2 = savePos2; //back to last savePos2
					savePos2++;    //and set next savePos2
				}
				else
					return false; //No previous wildcards...not matched!
			}
		}
	}
	*r1 = bSwapped ? m2 : m1;
	*r2 = bSwapped ? m1 : m2;

	return (!NOT_AT_END(m2)); //m1 surely finished, so for the match, m2 must be finished too

#undef NOT_AT_END
}

const char * kvi_extractToken(KviCString & str, const char * aux_ptr, char sep)
{
	KVI_ASSERT(aux_ptr);
	while(*aux_ptr && (*aux_ptr == sep))
		aux_ptr++;
	const char * p = aux_ptr;
	while(*p && (*p != sep))
		p++;
	str.m_len = p - aux_ptr;
	str.m_ptr = (char *)KviMemory::reallocate(str.m_ptr, str.m_len + 1);
	KviMemory::copy(str.m_ptr, aux_ptr, str.m_len);
	*(str.m_ptr + str.m_len) = '\0';
	while(*p && (*p == sep))
		p++;
	return p;
}

const char * kvi_extractUpTo(KviCString & str, const char * aux_ptr, char sep)
{
	KVI_ASSERT(aux_ptr);
	const char * p = aux_ptr;
	while(*p && (*p != sep))
		p++;
	str.m_len = p - aux_ptr;
	str.m_ptr = (char *)KviMemory::reallocate(str.m_ptr, str.m_len + 1);
	KviMemory::copy(str.m_ptr, aux_ptr, str.m_len);
	*(str.m_ptr + str.m_len) = '\0';
	return p;
}

int kvi_vsnprintf(char * buffer, int len, const char * fmt, kvi_va_list list)
{
	KVI_ASSERT(fmt);
	KVI_ASSERT(buffer);
	KVI_ASSERT(len > 0); //printing 0 characters is senseless

	char * p;
	char * argString;
	long argValue;
	unsigned long argUValue;

	//9999999999999999999999999999999\0
	char numberBuffer[32]; //enough ? 10 is enough for 32bit unsigned int...
	char * pNumBuf;
	unsigned int tmp;

	for(p = buffer; *fmt; ++fmt)
	{
		if(len < 1)
			return (-1); //not enough space ... (in fact this could be len < 2 for the terminator)
		//copy up to a '%'
		if(*fmt != '%')
		{
			*p++ = *fmt;
			--len;
			continue;
		}

		++fmt; //skip this '%'
		switch(*fmt)
		{
			case 's': //string
				argString = kvi_va_arg(list, char *);
				if(!argString)
					continue;
				argValue = (long)strlen(argString);
				//check for space...
				if(len <= argValue)
					return (-1); //not enough space for buffer and terminator
				while(*argString)
					*p++ = *argString++;
				len -= argValue;
				continue;
			case 'd': //signed integer
				argValue = kvi_va_arg(list, int);
				if(argValue < 0)
				{ //negative integer
					*p++ = '-';
					if(--len == 0)
						return (-1);
					argValue = -argValue; //need to have it positive
					// most negative integer exception (avoid completely senseless (non digit) responses)
					if(argValue < 0)
						argValue = 0; //we get -0 here
				}
				//write the number in a temporary buffer
				pNumBuf = numberBuffer;
				do
				{
					tmp = argValue / 10;
					*pNumBuf++ = argValue - (tmp * 10) + '0';
				} while((argValue = tmp));
				//copy now....
				argUValue = pNumBuf - numberBuffer; //length of the number string
				if(((uint)len) <= argUValue)
					return (-1); //not enough space for number and terminator
				do
				{
					*p++ = *--pNumBuf;
				} while(pNumBuf != numberBuffer);
				len -= argUValue;
				continue;
			case 'u':                                       //unsigned integer
				argUValue = kvi_va_arg(list, unsigned int); //many implementations place int here
				//write the number in a temporary buffer
				pNumBuf = numberBuffer;
				do
				{
					tmp = argUValue / 10;
					*pNumBuf++ = argUValue - (tmp * 10) + '0';
				} while((argUValue = tmp));
				//copy now....
				argValue = pNumBuf - numberBuffer; //length of the number string
				if(len <= argValue)
					return (-1); //not enough space for number and terminator
				do
				{
					*p++ = *--pNumBuf;
				} while(pNumBuf != numberBuffer);
				len -= argValue;
				continue;
			case 'x':                                       // hexadecimal unsigned integer
				argUValue = kvi_va_arg(list, unsigned int); //many implementations place int here
				//write the number in a temporary buffer
				pNumBuf = numberBuffer;
				do
				{
					tmp = argUValue / 16;
					*pNumBuf++ = hexdigits[argUValue % 16];
				} while((argUValue = tmp));
				//copy now....
				argValue = pNumBuf - numberBuffer; //length of the number string
				if(len <= argValue)
					return (-1); //not enough space for number and terminator
				do
				{
					*p++ = *--pNumBuf;
				} while(pNumBuf != numberBuffer);
				len -= argValue;
				continue;
			case 'c': //char
				//
				// I'm not sure about this...
				// In the linux kernel source the
				// unsigned char is extracted from an integer type.
				// We assume that gcc stacks a char argument
				// as sizeof(int) bytes value.
				// Is this always true ?
				//
				*p++ = (char)kvi_va_arg(list, int);
				--len;
				continue;
			case 'Q': // QString! (this should almost never happen)
			{
				QString * s = kvi_va_arg(list, QString *);
				QByteArray cs = (*s).toUtf8();
				const char * t = cs.data();
				if(!t)
					continue; // nothing to do
				//check for space...
				if(len <= (int)cs.length())
					return (-1); //not enough space for buffer and terminator
				while(*t)
					*p++ = *t++;
				len -= cs.length();
				continue;
			}
			default:        //a normal percent
				*p++ = '%'; //write it
				if(--len == 0)
					return (-1); //not enough space for next char or terminator
				if(*fmt)
				{                //this if is just in case that we have a % at the end of the string.
					*p++ = *fmt; //and write this char
					--len;
				}
				continue;
		}
	}
	if(len < 1)
		return (-1); //missing space for terminator
	*p = '\0';
	return p - buffer;
}

//
// Nearly the same as the above function...
//

int kvi_irc_vsnprintf(char * buffer, const char * fmt, kvi_va_list list, bool * bTruncated)
{
	KVI_ASSERT(fmt);
	KVI_ASSERT(buffer);
	if(!(buffer && fmt))
		return false;
	char * p;
	char * argString;
	long argValue;
	unsigned long argUValue;
	char numberBuffer[64]; //enough ? 10 is enough for 32bit unsigned int...
	char * pNumBuf;
	unsigned int tmp;
	*bTruncated = false;
	int len = 512;

	for(p = buffer; *fmt; ++fmt)
	{
		if(len < 3)
			goto truncate;
		//copy up to a '%'
		if(*fmt != '%')
		{
			*p++ = *fmt;
			--len;
			continue;
		}
		++fmt; //skip this '%'
		switch(*fmt)
		{
			case 's': //string
				argString = kvi_va_arg(list, char *);
				if(!argString)
					continue;
				//check for space...
				while(*argString)
				{
					*p++ = *argString++;
					if(--len < 3)
						goto truncate;
				}
				continue;
			case 'Q': // QString! (this should almost never happen)
			{
				QString * s = kvi_va_arg(list, QString *);
				QByteArray cs = (*s).toUtf8();
				const char * t = cs.data();
				if(!t)
					continue; // nothing to do
				while(*t)
				{
					*p++ = *t++;
					if(--len < 3)
						goto truncate;
				}
				continue;
			}
			case 'd': //signed integer
				argValue = kvi_va_arg(list, int);
				if(argValue < 0)
				{ //negative integer
					*p++ = '-';
					if(--len < 3)
						goto truncate;    //place just for CRLF
					argValue = -argValue; //need to have it positive
					if(argValue < 0)
						argValue = 0; // -0 (hack the exception)
				}
				//write the number in a temporary buffer
				pNumBuf = numberBuffer;
				do
				{
					tmp = argValue / 10;
					*pNumBuf++ = argValue - (tmp * 10) + '0';
				} while((argValue = tmp));
				//copy now....
				do
				{
					*p++ = *--pNumBuf;
					if(--len < 3)
						goto truncate;
				} while(pNumBuf != numberBuffer);
				continue;
			case 'u':                                       //unsigned integer
				argUValue = kvi_va_arg(list, unsigned int); //many implementations place int here
				//write the number in a temporary buffer
				pNumBuf = numberBuffer;
				do
				{
					tmp = argUValue / 10;
					*pNumBuf++ = argUValue - (tmp * 10) + '0';
				} while((argUValue = tmp));
				//copy now....
				if(--len < 3)
					goto truncate; //no place for digits
				do
				{
					*p++ = *--pNumBuf;
					if(--len < 3)
						goto truncate;
				} while(pNumBuf != numberBuffer);
				continue;
			case 'c': //char
				*p++ = (char)kvi_va_arg(list, int);
				--len;
				continue;
			default:        //a normal percent
				*p++ = '%'; //write it
				if(--len < 3)
					goto truncate; //not enough space for next char
				if(*fmt)
				{                //this if is just in case that we have a % at the end of the string.
					*p++ = *fmt; //and write this char
					--len;
				}
				continue;
		}
	}
	//successful finish
	KVI_ASSERT(len >= 2);
	*p++ = '\r';
	*p = '\n';
	return ((p - buffer) + 1);
truncate:
	KVI_ASSERT(len >= 2);
	*bTruncated = true;
	*p++ = '\r';
	*p = '\n';
	return ((p - buffer) + 1);
}

#ifndef COMPILE_ix86_ASM

bool kvi_strEqualCS(const char * str1, const char * str2)
{
	KVI_ASSERT(str1);
	KVI_ASSERT(str2);
	if(!(str1 && str2))
		return false;
	unsigned char * s1 = (unsigned char *)str1;
	unsigned char * s2 = (unsigned char *)str2;
	while(*s1)
		if(*s1++ != *s2++)
			return false;
	return (*s1 == *s2);
}

bool kvi_strEqualCSN(const char * str1, const char * str2, int len)
{
	KVI_ASSERT(str1);
	KVI_ASSERT(str2);
	KVI_ASSERT(len >= 0);
	if(!(str1 && str2 && (len >= 0)))
		return false;
	unsigned char * s1 = (unsigned char *)str1;
	unsigned char * s2 = (unsigned char *)str2;
	while(len-- && *s1)
		if(*s1++ != *s2++)
			return false;
	return (len < 0);
}

#endif

bool kvi_strEqualCIN(const char * str1, const char * str2, int len)
{
	KVI_ASSERT(str1);
	KVI_ASSERT(str2);
	KVI_ASSERT(len >= 0);
	if(!(str1 && str2 && (len >= 0)))
		return false;
	unsigned char * s1 = (unsigned char *)str1;
	unsigned char * s2 = (unsigned char *)str2;
	while(len-- && *s1)
		if(tolower(*s1++) != tolower(*s2++))
			return false;
	return (len < 0);
}

bool kvi_strEqualCI(const char * str1, const char * str2)
{
	KVI_ASSERT(str1);
	KVI_ASSERT(str2);
	if(!(str1 && str2))
		return false;
	unsigned char * s1 = (unsigned char *)str1;
	unsigned char * s2 = (unsigned char *)str2;
	while(*s1)
		if(tolower(*s1++) != tolower(*s2++))
			return false;
	return (*s1 == *s2);
}

//
//note that greater here means that come AFTER in the alphabetic order
// return < 0 ---> str1 < str2
// return = 0 ---> str1 = str2
// return > 0 ---> str1 > str2
//

int kvi_strcmpCI(const char * str1, const char * str2)
{
	//abcd abce
	KVI_ASSERT(str1);
	KVI_ASSERT(str2);
	if(!(str1 && str2))
		return false;
	unsigned char * s1 = (unsigned char *)str1;
	unsigned char * s2 = (unsigned char *)str2;
	int diff;
	unsigned char rightchar;
	while(!(diff = (rightchar = tolower(*s1++)) - tolower(*s2++)))
		if(!rightchar)
			break;
	return diff; //diff is nonzero or end of both was reached (it is positive if *s2 > *s1
}

//
//note that greater here means that come AFTER in the alphabetic order
// return < 0 ---> str1 < str2
// return = 0 ---> str1 = str2
// return > 0 ---> str1 > str2
//int kvi_strcmpCIN(const char *str1,const char *str2,int len)
//

int kvi_strcmpCS(const char * str1, const char * str2)
{
	//abcd abce
	KVI_ASSERT(str1);
	KVI_ASSERT(str2);
	if(!(str1 && str2))
		return false;
	unsigned char * s1 = (unsigned char *)str1;
	unsigned char * s2 = (unsigned char *)str2;
	int diff;
	while(!(diff = (*s1) - (*s2++)))
		if(!*s1++)
			break;
	return diff; //diff is nonzero or end of both was reached (it is positive if *s2 > *s1
}

int kvi_strMatchRevCS(const char * str1, const char * str2, int index)
{
	KVI_ASSERT(str1);
	KVI_ASSERT(str2);
	if(!(str1 && str2))
		return false;
	char * s1 = (char *)str1;
	char * s2 = (char *)str2;

	int curlen = (int)strlen(str1);
	int diff;

	if(index < 0 || index >= curlen)
		index = curlen - 1;

	s1 += index;
	while(*s2)
		s2++;
	s2--;

	// now start comparing
	while(true)
	{
		/* in this case, we have str1 = "lo" and str2 = "hello" */
		if(s1 < str1 && !(s2 < str2))
			return 256;
		if(s2 < str2)
			return 0;
		if((diff = (*s1) - (*s2)))
			return diff;
		s1--;
		s2--;
	}
}

KviCString::KviCString()
{
	m_ptr = (char *)KviMemory::allocate(1);
	*m_ptr = '\0';
	m_len = 0;
}

KviCString::KviCString(const char * str)
{
	//Deep copy constructor
	if(str)
	{
		//Deep copy
		m_len = (int)strlen(str);
		m_ptr = (char *)KviMemory::allocate(m_len + 1);
		KviMemory::copy(m_ptr, str, m_len + 1);
	}
	else
	{
		m_ptr = (char *)KviMemory::allocate(1);
		*m_ptr = '\0';
		m_len = 0;
	}
}

KviCString::KviCString(const QByteArray & str)
{
	//Deep copy constructor
	if(str.data())
	{
		//Deep copy
		m_len = str.length();
		m_ptr = (char *)KviMemory::allocate(m_len + 1);
		KviMemory::copy(m_ptr, str, m_len + 1);
	}
	else
	{
		m_ptr = (char *)KviMemory::allocate(1);
		*m_ptr = '\0';
		m_len = 0;
	}
}

KviCString::KviCString(const char * str, int len)
{
	KVI_ASSERT(str);
	//KVI_ASSERT(len <= ((int)strlen(str))); <-- we trust the user here (and a strlen() call may run AFTER len if data is not null terminated)
	KVI_ASSERT(len >= 0);
	m_len = len;
	m_ptr = (char *)KviMemory::allocate(m_len + 1);
	KviMemory::copy(m_ptr, str, m_len);
	*(m_ptr + m_len) = '\0';
}

KviCString::KviCString(const char * bg, const char * end)
{
	KVI_ASSERT(bg);
	KVI_ASSERT(end);
	KVI_ASSERT(bg <= end);
	m_len = end - bg;
	m_ptr = (char *)KviMemory::allocate(m_len + 1);
	KviMemory::copy(m_ptr, bg, m_len);
	*(m_ptr + m_len) = '\0';
}

KviCString::KviCString(KviFormatConstructorTag, const char * fmt, ...)
{
	m_ptr = (char *)KviMemory::allocate(256);
	//First try
	kvi_va_list list;
	kvi_va_start(list, fmt);
	//print...with max 256 chars
	m_len = kvi_vsnprintf(m_ptr, 256, fmt, list);
	kvi_va_end(list);

	//check if we failed
	if(m_len < 0)
	{
		//yes, failed....
		int dummy = 256;
		do
		{ //we failed, so retry with 256 more chars
			dummy += 256;
			//realloc
			m_ptr = (char *)KviMemory::reallocate(m_ptr, dummy);
			//print...
			kvi_va_start(list, fmt);
			m_len = kvi_vsnprintf(m_ptr, dummy, fmt, list);
			kvi_va_end(list);
		} while(m_len < 0);
	}
	//done...
	//now m_len is the length of the written string not including the terminator...
	//perfect! :)
	m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + 1);
}

KviCString::KviCString(const KviCString & str)
{
	KVI_ASSERT(str.m_ptr);
	m_len = str.m_len;
	m_ptr = (char *)KviMemory::allocate(m_len + 1);
	KviMemory::copy(m_ptr, str.m_ptr, m_len + 1);
}

KviCString::KviCString(const QString & str)
{
	QByteArray sz = str.toUtf8();
	if(sz.length() > 0)
	{
		m_len = sz.length();
		m_ptr = (char *)KviMemory::allocate(m_len + 1);
		KviMemory::copy(m_ptr, sz.data(), m_len + 1);
	}
	else
	{
		m_ptr = (char *)KviMemory::allocate(1);
		*m_ptr = '\0';
		m_len = 0;
	}
}

KviCString::KviCString(char c, int fillLen)
{
	KVI_ASSERT(fillLen >= 0);
	m_len = fillLen;
	m_ptr = (char *)KviMemory::allocate(m_len + 1);
	char * p = m_ptr;
	while(fillLen--)
		*p++ = c;
	*p = '\0';
}

KviCString::KviCString(const kvi_wchar_t * unicode)
{
	if(!unicode)
	{
		m_len = 0;
		m_ptr = (char *)KviMemory::allocate(1);
		*m_ptr = 0;
	}
	else
	{
		m_len = kvi_wstrlen(unicode);
		m_ptr = (char *)KviMemory::allocate(m_len + 1);
		char * p = m_ptr;
		while(*unicode)
			*p++ = *unicode++;
		*p = 0;
	}
}

KviCString::KviCString(const kvi_wchar_t * unicode, int len)
{
	m_len = len;
	m_ptr = (char *)KviMemory::allocate(m_len + 1);
	char * p = m_ptr;
	char * end = p + len;
	while(p != end)
	{
		*p++ = *unicode++;
	}
	*p = 0;
}

KviCString::~KviCString()
{
	KviMemory::free(m_ptr);
}

KviCString & KviCString::operator=(const KviCString & str)
{
	KVI_ASSERT(str.m_ptr);
	KVI_ASSERT(str.m_ptr != m_ptr);
	m_len = str.m_len;
	m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + 1);
	KviMemory::copy(m_ptr, str.m_ptr, m_len + 1);
	return (*this);
}

KviCString & KviCString::operator=(const QByteArray & str)
{
	m_len = str.length();
	m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + 1);
	if(str.data())
		KviMemory::copy(m_ptr, str.data(), m_len + 1);
	else
		*m_ptr = 0;
	return (*this);
}

KviCString & KviCString::operator=(const char * str)
{
	//KVI_ASSERT(str);
	if(str)
	{
		m_len = (int)strlen(str);
		m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + 1);
		KviMemory::move(m_ptr, str, m_len + 1);
	}
	else
	{
		m_ptr = (char *)KviMemory::reallocate(m_ptr, 1);
		*m_ptr = '\0';
		m_len = 0;
	}
	return (*this);
}

void KviCString::clear()
{
	m_ptr = (char *)KviMemory::reallocate(m_ptr, 1);
	*m_ptr = '\0';
	m_len = 0;
}

bool KviCString::hasNonWhiteSpaceData() const
{
	const char * aux = m_ptr;
	while(*aux)
	{
		if(((*aux) != ' ') && ((*aux) != '\t'))
			return true;
		aux++;
	}
	return false;
}

void KviCString::bufferToHex(const char * buffer, int len)
{
	KVI_ASSERT(buffer);
	m_len = (len * 2);
	m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + 1);
	char * aux = m_ptr;
	while(len)
	{
		*aux = hexdigits[(unsigned int)(((unsigned char)(*buffer)) / 16)];
		aux++;
		*aux = hexdigits[(unsigned int)(((unsigned char)(*buffer)) % 16)];
		aux++;
		len--;
		buffer++;
	}
	*(m_ptr + m_len) = '\0';
}

static char get_decimal_from_hex_digit_char(char dgt)
{
	if((dgt >= '0') && (dgt <= '9'))
		return (dgt - '0');
	if((dgt >= 'A') && (dgt <= 'F'))
		return (10 + (dgt - 'A'));
	if((dgt >= 'a') && (dgt <= 'f'))
		return (10 + (dgt - 'a'));
	return -1;
}

int KviCString::hexToBuffer(char ** buffer, bool bNullToNewlines)
{
	*buffer = nullptr;
	if((m_len == 0) || (m_len & 1))
		return -1; // this is an error
	int len = (m_len / 2);
	if(len < 1)
		return -1;
	*buffer = (char *)KviMemory::allocate(len);

	char * ptr = *buffer;
	char * aux = m_ptr;

	while(*aux)
	{
		char temp = get_decimal_from_hex_digit_char(*aux);
		if(temp == -1)
		{
			KviMemory::free(*buffer);
			*buffer = nullptr;
			return -1;
		}
		*ptr = temp * 16;
		aux++;
		temp = get_decimal_from_hex_digit_char(*aux);
		if(temp == -1)
		{
			KviMemory::free(*buffer);
			*buffer = nullptr;
			return -1;
		}
		*ptr += temp;
		aux++;
		if(bNullToNewlines)
			if(!(*ptr))
				*ptr = '\n';
		ptr++;
	}
	return len;
}

static const char * base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void KviCString::bufferToBase64(const char * buffer, int len)
{
	m_len = (len / 3) << 2;
	if(len % 3)
		m_len += 4;

	m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + 1);

	unsigned char aux1, aux2, aux3;
	char * aux_ptr = m_ptr;
	while(len > 2)
	{
		aux1 = (unsigned char)*buffer++;
		aux2 = (unsigned char)*buffer++;
		aux3 = (unsigned char)*buffer++;
		*aux_ptr++ = base64_chars[(aux1 & 0xFC) >> 2];
		*aux_ptr++ = base64_chars[((aux1 & 0x03) << 4) | ((aux2 & 0xF0) >> 4)];
		*aux_ptr++ = base64_chars[((aux2 & 0x0F) << 2) | ((aux3 & 0xC0) >> 6)];
		*aux_ptr++ = base64_chars[(aux3 & 0x3F)];
		len -= 3;
	}
	switch(len)
	{
		case 2:
			aux1 = (unsigned char)*buffer++;
			aux2 = (unsigned char)*buffer++;
			*aux_ptr++ = base64_chars[(aux1 & 0xFC) >> 2];
			*aux_ptr++ = base64_chars[((aux1 & 0x03) << 4) | ((aux2 & 0xF0) >> 4)];
			*aux_ptr++ = base64_chars[((aux2 & 0x0F) << 2)];
			*aux_ptr++ = '=';
			break;
		case 1:
			aux1 = (unsigned char)*buffer++;
			aux2 = (unsigned char)*buffer++;
			*aux_ptr++ = base64_chars[(aux1 & 0xFC) >> 2];
			*aux_ptr++ = base64_chars[((aux1 & 0x03) << 4)];
			*aux_ptr++ = '=';
			*aux_ptr++ = '=';
			break;
	}
	*aux_ptr = 0;
}

static unsigned char get_base64_idx(char base64)
{
	if((base64 >= 'A') && (base64 <= 'Z'))
		return (base64 - 'A');
	if((base64 >= 'a') && (base64 <= 'z'))
		return ((base64 - 'a') + 26);
	if((base64 >= '0') && (base64 <= '9'))
		return ((base64 - '0') + 52);
	if(base64 == '+')
		return 62;
	if(base64 == '/')
		return 63;
	if(base64 == '=')
		return 64;
	return 65;
}

int KviCString::base64ToBuffer(char ** buffer, bool)
{
	*buffer = nullptr;
	if((m_len == 0) || (m_len & 3))
		return -1; // this is an error
	int len = (m_len >> 2) * 3;
	*buffer = (char *)KviMemory::allocate(len);

	char * auxBuf = *buffer;

	unsigned char aux1, aux2, aux3, aux4;
	char * aux_ptr = m_ptr;

	int newLen = len;

	while(*aux_ptr)
	{
		if(newLen != len)
		{
			// ops... there was a padding and we still have chars after it
			// this is an error
			KviMemory::free(*buffer);
			*buffer = nullptr;
			return -1;
		}
		aux1 = get_base64_idx(*aux_ptr++);
		aux2 = get_base64_idx(*aux_ptr++);
		aux3 = get_base64_idx(*aux_ptr++);
		aux4 = get_base64_idx(*aux_ptr++);
		if((aux3 > 64) || (aux4 > 64))
		{
			// error
			KviMemory::free(*buffer);
			*buffer = nullptr;
			return -1;
		}
		if((aux1 | aux2) > 63)
		{
			// again error...impossible padding
			KviMemory::free(*buffer);
			*buffer = nullptr;
			return -1;
		}
		if(aux4 == 64)
		{
			if(aux3 == 64)
			{
				// Double padding, only one digit here
				*auxBuf++ = (char)((aux1 << 2) | (aux2 >> 4));
				newLen -= 2;
			}
			else
			{
				// Single padding, two digits here
				*auxBuf++ = (char)((aux1 << 2) | (aux2 >> 4)); // >> 4 is a shr, not a ror! :)
				*auxBuf++ = (char)((aux2 << 4) | (aux3 >> 2));
				newLen -= 1;
			}
		}
		else
		{
			if(aux3 == 64)
			{
				// error... impossible padding
				KviMemory::free(*buffer);
				*buffer = nullptr;
				return -1;
			}
			else
			{
				// Ok, no padding, three digits here
				*auxBuf++ = (char)((aux1 << 2) | (aux2 >> 4));
				*auxBuf++ = (char)((aux2 << 4) | (aux3 >> 2));
				*auxBuf++ = (char)((aux3 << 6) | aux4);
			}
		}
	}

	if(newLen != len)
		*buffer = (char *)KviMemory::reallocate(*buffer, newLen);
	return newLen;
}

KviCString & KviCString::setStr(const char * str, int len)
{
	if(!str)
	{
		clear();
		return *this;
	}
	int alen = (int)strlen(str);
	if((len < 0) || (len > alen))
		m_len = alen;
	else
		m_len = len;
	m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + 1);
	KviMemory::move(m_ptr, str, m_len);
	*(m_ptr + m_len) = '\0';
	return (*this);
}

KviCString & KviCString::operator=(const QString & str)
{
	QByteArray sz = str.toUtf8();
	if(sz.length() > 0)
	{
		m_len = sz.length();
		m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + 1);
		KviMemory::copy(m_ptr, sz.data(), m_len + 1);
	}
	else
	{
		m_ptr = (char *)KviMemory::reallocate(m_ptr, 1);
		*m_ptr = '\0';
		m_len = 0;
	}
	return (*this);
}

KviCString & KviCString::operator=(char c)
{
	m_len = 1;
	m_ptr = (char *)KviMemory::reallocate(m_ptr, 2);
	*m_ptr = c;
	*(m_ptr + 1) = '\0';
	return (*this);
}

void KviCString::append(char c)
{
	m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + 2);
	*(m_ptr + m_len) = c;
	m_len++;
	*(m_ptr + m_len) = '\0';
}

void KviCString::append(const KviCString & str)
{
	KVI_ASSERT(str.m_ptr);
	m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + str.m_len + 1);
	KviMemory::copy((m_ptr + m_len), str.m_ptr, str.m_len + 1);
	m_len += str.m_len;
}

void KviCString::append(const char * str)
{
	if(!str)
		return;
	int len = (int)strlen(str);
	m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + len + 1);
	KviMemory::copy((m_ptr + m_len), str, len + 1);
	m_len += len;
}

void KviCString::append(const QString & str)
{
	QByteArray sz = str.toUtf8();
	if(sz.length() < 1)
		return;
	m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + sz.length() + 1);
	KviMemory::copy((m_ptr + m_len), sz.data(), sz.length() + 1);
	m_len += sz.length();
}

void KviCString::append(const char * str, int len)
{
	KVI_ASSERT(str);
	//	KVI_ASSERT(len <= ((int)strlen(str)));
	KVI_ASSERT(len >= 0);
	m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + len + 1);
	KviMemory::copy((m_ptr + m_len), str, len);
	m_len += len;
	*(m_ptr + m_len) = '\0';
}

void KviCString::append(KviFormatConstructorTag, const char * fmt, ...)
{
	int auxLen;
	m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + 256);
	//First try
	kvi_va_list list;
	kvi_va_start(list, fmt);
	//print...with max 256 chars
	auxLen = kvi_vsnprintf(m_ptr + m_len, 256, fmt, list);
	kvi_va_end(list);

	//check if we failed
	if(auxLen < 0)
	{
		//yes, failed....
		int dummy = 256;
		do
		{ //we failed, so retry with 256 more chars
			dummy += 256;
			//realloc
			m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + dummy);
			//print...
			kvi_va_start(list, fmt);
			auxLen = kvi_vsnprintf(m_ptr + m_len, dummy, fmt, list);
			kvi_va_end(list);
		} while(auxLen < 0);
	}
	m_len += auxLen;
	//done...
	//now m_len is the length of the written string not including the terminator...
	//perfect! :)
	m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + 1);
}

void KviCString::extractFromString(const char * begin, const char * end)
{
	KVI_ASSERT(begin);
	KVI_ASSERT(end);
	KVI_ASSERT(end >= begin);
	m_len = end - begin;
	m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + 1);
	KviMemory::copy(m_ptr, begin, m_len);
	*(m_ptr + m_len) = '\0';
}

void KviCString::prepend(const KviCString & str)
{
	KVI_ASSERT(str.m_ptr);
	KVI_ASSERT(str.m_ptr != m_ptr);
	m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + str.m_len + 1);
	KviMemory::move((m_ptr + str.m_len), m_ptr, m_len + 1); //move self
	KviMemory::copy(m_ptr, str.m_ptr, str.m_len);
	m_len += str.m_len;
}

void KviCString::prepend(const char * str)
{
	if(!str)
		return;
	int len = (int)strlen(str);
	m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + len + 1);
	KviMemory::move((m_ptr + len), m_ptr, m_len + 1); //move self
	KviMemory::copy(m_ptr, str, len);
	m_len += len;
}

void KviCString::prepend(const char * str, int len)
{
	KVI_ASSERT(str);
	KVI_ASSERT(len <= ((int)strlen(str)));
	KVI_ASSERT(len >= 0);
	m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + len + 1);
	KviMemory::move((m_ptr + len), m_ptr, m_len + 1); //move self
	KviMemory::copy(m_ptr, str, len);
	m_len += len;
}

unsigned char iso88591_toUpper_map[256] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
	0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
	0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
	0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
	0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
	0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
	0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
	0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
	0x58, 0x59, 0x5a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
	0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
	0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
	0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
	0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
	0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
	0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
	0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
	0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
	0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
	0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
	0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
	0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
	0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
	0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};

void KviCString::toUpperISO88591()
{
	char * p = m_ptr;
	while(*p)
	{
		*p = (char)iso88591_toUpper_map[(unsigned char)*p];
		p++;
	}
}

void KviCString::toUpper()
{
	char * p = m_ptr;
	while(*p)
	{
		*p = toupper(*p);
		p++;
	}
}

unsigned char iso88591_toLower_map[256] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
	0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
	0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
	0x40, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
	0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
	0x78, 0x79, 0x7a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
	0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
	0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
	0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
	0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
	0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
	0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
	0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
	0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
	0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
	0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
	0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
	0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
	0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
	0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
	0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
	0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
	0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};

void KviCString::toLowerISO88591()
{
	char * p = m_ptr;
	while(*p)
	{
		*p = (char)iso88591_toLower_map[(unsigned char)*p];
		p++;
	}
}

void KviCString::toLower()
{
	char * p = m_ptr;
	while(*p)
	{
		*p = tolower(*p);
		p++;
	}
}

KviCString KviCString::upper() const
{
	KviCString tmp(*this);
	tmp.toUpper();
	return tmp;
}

KviCString KviCString::upperISO88591() const
{
	KviCString tmp(*this);
	tmp.toUpperISO88591();
	return tmp;
}

KviCString KviCString::lower() const
{
	KviCString tmp(*this);
	tmp.toLower();
	return tmp;
}

KviCString KviCString::lowerISO88591() const
{
	KviCString tmp(*this);
	tmp.toLowerISO88591();
	return tmp;
}

KviCString KviCString::left(int maxLen) const
{
	if(maxLen <= 0)
	{
		KviCString empty;
		return empty;
	}
	if(maxLen > m_len)
		maxLen = m_len;
	KviCString str(m_ptr, maxLen);
	return str;
}

KviCString KviCString::right(int maxLen) const
{
	if(maxLen <= 0)
	{
		KviCString empty;
		return empty;
	}
	if(maxLen > m_len)
		maxLen = m_len;
	KviCString str((m_ptr + (m_len - maxLen)), maxLen);
	return str;
}

KviCString KviCString::middle(int idx, int maxLen) const
{
	KVI_ASSERT(maxLen >= 0);
	KVI_ASSERT(idx >= 0);
	if((maxLen <= 0) || (idx < 0))
	{ //max len negative...invalid params
		KviCString ret;
		return ret;
	}
	if((maxLen + idx) <= m_len)
	{ //valid params
		KviCString str(m_ptr + idx, maxLen);
		return str;
	}
	if(idx < m_len)
	{ //string shorter than requested
		KviCString str(m_ptr + idx);
		return str;
	}
	// idx out of bounds
	KviCString ret;
	return ret;
}

KviCString ** KviCString::splitToArray(char sep, int max, int * realCount) const
{
	KviCString ** strings = (KviCString **)KviMemory::allocate(sizeof(KviCString *));
	int number = 0;
	char * ptr = m_ptr;
	char * last = ptr;
	while((max > 0) && *ptr)
	{
		strings = (KviCString **)KviMemory::reallocate((void *)strings, sizeof(KviCString *) * (number + 2));
		if(max > 1)
		{
			while(*ptr && (*ptr != sep))
				ptr++;
			strings[number] = new KviCString(last, ptr - last);
		}
		else
		{
			strings[number] = new KviCString(ptr);
		}
		number++;
		max--;
		if(*ptr)
		{
			ptr++;
			last = ptr;
		}
	}
	if(realCount)
		*realCount = number;
	strings[number] = nullptr;
	return strings;
}

void KviCString::freeArray(KviCString ** strings)
{
	if(!strings)
		return;
	KviCString ** aux = strings;
	while(*aux)
	{
		delete(*aux); // delete (KviCString *)
		aux++;
	}
	KviMemory::free(strings);
}

void KviCString::freeBuffer(char * buffer)
{
	if(!buffer)
		return;
	KviMemory::free(buffer);
}

void KviCString::joinFromArray(KviCString ** strings, const char * sep, bool bLastSep)
{
	setLen(0);
	if(!strings)
		return;

	while(*strings)
	{
		append(*(*strings));
		strings++;
		if(*strings)
		{
			if(sep)
				append(sep);
		}
		else
		{
			if(sep && bLastSep)
				append(sep);
		}
	}
}

KviCString & KviCString::insert(int idx, const char * data)
{
	KVI_ASSERT(data);
	if(idx <= m_len)
	{
		int len = (int)strlen(data);
		m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + len + 1);
		KviMemory::move(m_ptr + idx + len, m_ptr + idx, (m_len - idx) + 1);
		KviMemory::copy(m_ptr + idx, data, len);
		m_len += len;
	}
	return (*this);
}

KviCString & KviCString::insert(int idx, char c)
{
	if(idx <= m_len)
	{
		m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + 2);
		KviMemory::move(m_ptr + idx + 1, m_ptr + idx, (m_len - idx) + 1);
		m_len++;
		*(m_ptr + idx) = c;
	}
	return (*this);
}

// FIXME: #warning "Double check the following two functions !!!"

KviCString & KviCString::hexEncodeWithTable(const unsigned char table[256])
{
	char * aux = m_ptr;
	char * begin = m_ptr;

	char * n = nullptr;
	int curSize = 0;

	while(*aux)
	{
		if(table[*((unsigned char *)aux)] || (*aux == '%'))
		{
			int len = aux - begin;
			n = (char *)KviMemory::reallocate(n, curSize + len + 3);
			KviMemory::move(n + curSize, begin, len);
			curSize += len;

			n[curSize] = '%';
			curSize++;
			n[curSize] = hexdigits[(unsigned int)(((unsigned char)(*aux)) / 16)];
			curSize++;
			n[curSize] = hexdigits[(unsigned int)(((unsigned char)(*aux)) % 16)];
			curSize++;

			aux++;
			begin = aux;
		}
		else
			aux++;
	}

	int len = aux - begin;
	n = (char *)KviMemory::reallocate(n, curSize + len + 1);
	KviMemory::move(n + curSize, begin, len);
	curSize += len;

	n[curSize] = '\0';

	KviMemory::free((void *)m_ptr);
	m_ptr = n;
	m_len = curSize;

	return (*this);
}

KviCString & KviCString::hexEncodeWhiteSpace()
{
	static unsigned char ascii_jump_table[256] = {
		// clang-format off
		//	000 001 002 003 004 005 006 007   008 009 010 011 012 013 014 015
		//	NUL SOH STX ETX EOT ENQ ACK BEL   BS  HT  LF  VT  FF  CR  SO  SI
			1  ,1  ,1  ,1  ,1  ,1  ,1  ,1    ,1  ,1  ,1  ,1  ,1  ,1  ,1  ,1  ,
		//	016 017 018 019 020 021 022 023   024 025 026 027 028 029 030 031
		//	DLE DC1 DC2 DC3 DC4 NAK SYN ETB   CAN EM  SUB ESC FS  GS  RS  US
			1  ,1  ,1  ,1  ,1  ,1  ,1  ,1    ,1  ,1  ,1  ,1  ,1  ,1  ,1  ,1  ,
		//	032 033 034 035 036 037 038 039   040 041 042 043 044 045 046 047
		//	    !   "   #   $   %   &   '     (   )   *   +   ,   -   .   /
			1  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
		//	048 049 050 051 052 053 054 055   056 057 058 059 060 061 062 063
		//	0   1   2   3   4   5   6   7     8   9   :   ;   <   =   >   ?
			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
		//	064 065 066 067 068 069 070 071   072 073 074 075 076 077 078 079
		//	@   A   B   C   D   E   F   G     H   I   J   K   L   M   N   O
			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
		//	080 081 082 083 084 085 086 087   088 089 090 091 092 093 094 095
		//	P   Q   R   S   T   U   V   W     X   Y   Z   [   \   ]   ^   _
			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
		//	096 097 098 099 100 101 102 103   104 105 106 107 108 109 110 111
		//	`   a   b   c   d   e   f   g     h   i   j   k   l   m   n   o
			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
		//	112 113 114 115 116 117 118 119   120 121 122 123 124 125 126 127
		//	p   q   r   s   t   u   v   w     x   y   z   {   |   }   ~   
			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
		//	128 129 130 131 132 133 134 135   136 137 138 139 140 141 142 143
		//
			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
		//	144 145 146 147 148 149 150 151   152 153 154 155 156 157 158 159
		//
			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
		//	160 161 162 163 164 165 166 167   168 169 170 171 172 173 174 175
		//
			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
		//	176 177 178 179 180 181 182 183   184 185 186 187 188 189 190 191
		//
			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
		//	192 193 194 195 196 197 198 199   200 201 202 203 204 205 206 207
		//	�  �  �  �  �  �  �  �    �  �  �  �  �  �  �  �
			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
		//	208 209 210 211 212 213 214 215   216 217 218 219 220 221 222 223
		//	�  �  �  �  �  �  �  �    �  �  �  �  �  �  �  �
			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
		//	224 225 226 227 228 229 230 231   232 233 234 235 236 237 238 239
		//	�  �  �  �  �  �  �  �    �  �  �  �  �  �  �  �
			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
		//	240 241 242 243 244 245 246 247   248 249 250 251 252 253 254 255
		//	�  �  �  �  �  �  �  �
			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0
		// clang-format on
	};

	return hexEncodeWithTable(ascii_jump_table);
}

KviCString & KviCString::hexDecode(const char * pFrom)
{
	// WARNING: pFrom can be also m_ptr here!
	const char * aux = pFrom;
	const char * begin = pFrom;

	char * n = nullptr;
	int curSize = 0;

	while(*aux)
	{
		if(*aux == '%')
		{
			// move last block
			int len = aux - begin;
			n = (char *)KviMemory::reallocate(n, curSize + len + 1);
			KviMemory::move(n + curSize, begin, len);
			curSize += len;

			// get the hex code
			aux++;

			char theChar = get_decimal_from_hex_digit_char(*aux);
			if(theChar < 0)
			{
				n[curSize] = '%'; // wrong code...just a '%'
				curSize++;
			}
			else
			{
				aux++;
				char theChar2 = get_decimal_from_hex_digit_char(*aux);
				if(theChar2 < 0)
				{
					// wrong code...just a '%' and step back
					n[curSize] = '%';
					curSize++;
					aux--;
				}
				else
				{
					n[curSize] = (theChar * 16) + theChar2;
					curSize++;
					aux++;
				}
			}

			begin = aux;
		}
		else
			aux++;
	}

	int len = aux - begin;
	n = (char *)KviMemory::reallocate(n, curSize + len + 2);
	KviMemory::move(n + curSize, begin, len);
	curSize += len;
	n[curSize] = '\0';

	KviMemory::free((void *)m_ptr);
	m_ptr = n;
	m_len = curSize;

	return (*this);
}

KviCString & KviCString::replaceAll(const char c, const char * str)
{
	int idx = findFirstIdx(c);
	KviCString tmp;
	while(idx >= 0)
	{
		if(idx > 0)
			tmp += left(idx);
		cutLeft(idx + 1);
		tmp.append(str);
		idx = findFirstIdx(c);
	}
	tmp.append(*this);
	// Now copy
	m_len = tmp.m_len;
	m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + 1);
	KviMemory::copy(m_ptr, tmp.m_ptr, m_len + 1);
	return (*this);
}

KviCString & KviCString::replaceAll(const char * toFind, const char * str, bool bCaseS)
{
	int len = (int)strlen(toFind);
	int idx = findFirstIdx(toFind, bCaseS);
	KviCString tmp;
	while(idx >= 0)
	{
		if(idx > 0)
			tmp += left(idx);
		cutLeft(idx + len);
		tmp.append(str);
		idx = findFirstIdx(toFind, bCaseS);
	}
	tmp.append(*this);
	// Now copy
	m_len = tmp.m_len;
	m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + 1);
	KviMemory::copy(m_ptr, tmp.m_ptr, m_len + 1);
	return (*this);
}

KviCString & KviCString::transliterate(const char * szToFind, const char * szReplacement)
{
	while(*szToFind && *szReplacement)
	{
		char * p = m_ptr;
		while(*p)
		{
			if(*p == *szToFind)
				*p = *szReplacement;
			++p;
		}
		++szToFind;
		++szReplacement;
	}
	return (*this);
}

int KviCString::occurrences(char c, bool caseS) const
{
	char * p = m_ptr;
	int cnt = 0;
	if(caseS)
	{
		while(*p)
		{
			if(*p == c)
				cnt++;
			p++;
		}
	}
	else
	{
		char b = tolower(c);
		while(*p)
		{
			if(tolower(*p) == b)
				cnt++;
			p++;
		}
	}
	return cnt;
}

int KviCString::occurrences(const char * str, bool caseS) const
{
	KVI_ASSERT(str);
	char * p = m_ptr;
	int cnt = 0;
	int len = (int)strlen(str);
	if(caseS)
	{
		while(*p)
		{
			if(*p == *str)
			{
				if(kvi_strEqualCSN(p, str, len))
					cnt++;
			}
			p++;
		}
	}
	else
	{
		while(*p)
		{
			char c = tolower(*str);
			if(tolower(*p) == c)
			{
				if(kvi_strEqualCIN(p, str, len))
					cnt++;
			}
			p++;
		}
	}
	return cnt;
}

bool KviCString::contains(char c, bool caseS) const
{
	char * p = m_ptr;
	if(caseS)
	{
		while(*p)
		{
			if(*p == c)
				return true;
			p++;
		}
	}
	else
	{
		char b = tolower(c);
		while(*p)
		{
			if(tolower(*p) == b)
				return true;
			p++;
		}
	}
	return false;
}

bool KviCString::contains(const char * str, bool caseS) const
{
	KVI_ASSERT(str);
	char * p = m_ptr;
	int len = (int)strlen(str);
	if(caseS)
	{
		while(*p)
		{
			if(*p == *str)
			{
				if(kvi_strEqualCSN(p, str, len))
					return true;
			}
			p++;
		}
	}
	else
	{
		while(*p)
		{
			char c = tolower(*str);
			if(tolower(*p) == c)
			{
				if(kvi_strEqualCIN(p, str, len))
					return true;
			}
			p++;
		}
	}
	return false;
}

KviCString & KviCString::setNum(long num)
{
	char numberBuffer[30];
	bool bNegative = false;
	long tmp;
	char * p;
	char * pNumBuf = numberBuffer;

	// somebody can explain to me why 	-(-2147483648) = -2147483648 ? (2^31)
	// it is like signed char x = 128 ---> 10000000 that is signed -0 (!?)
	// mmmmh...or it is assumed to be -128 (a number representation exception)
	// at least on my machine it happens...

	// found the solution by myself today...
	//
	// ABS(3)              Linux Programmer's Manual              ABS(3)
	// NAME
	//        abs - computes the absolute value of an integer.
	// ...
	// DESCRIPTION
	//        The abs() function computes the absolute value of the integer argument j.
	// RETURN VALUE
	//        Returns the absolute value of the integer argument.
	// CONFORMING TO
	//        SVID 3, POSIX, BSD 4.3, ISO 9899
	// NOTE ##################################################################################
	//        Trying to take the absolute value of the most negative integer is not defined.
	// #######################################################################################

	// so should i use temporaneous doubles to make calculations ?

	if(num < 0)
	{ //negative integer
		bNegative = true;
		num = -num; //need to have it positive
		if(num < 0)
		{	// 2^31 exception
			// We need to avoid absurd responses like ".(./),." :)
			num = 0; // we get a negative zero here...it is still an exception
		}
	}

	//write the number in a temporary buffer (at least '0')
	do
	{
		tmp = num / 10;
		*pNumBuf++ = num - (tmp * 10) + '0';
	} while((num = tmp));

	//copy now....
	m_len = pNumBuf - numberBuffer; //length of the number string
	if(bNegative)
	{
		m_len++;
		m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + 1);
		p = m_ptr;
		*p++ = '-';
	}
	else
	{
		m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + 1);
		p = m_ptr;
	}
	do
	{
		*p++ = *--pNumBuf;
	} while(pNumBuf != numberBuffer);
	*(m_ptr + m_len) = '\0';
	return (*this);
}

KviCString & KviCString::setNum(unsigned long num)
{
	char numberBuffer[30];
	unsigned long tmp;
	char * p;
	char * pNumBuf = numberBuffer;

	//write the number in a temporary buffer (at least '0')
	do
	{
		tmp = num / 10;
		*pNumBuf++ = num - (tmp * 10) + '0';
	} while((num = tmp));

	//copy now....
	m_len = pNumBuf - numberBuffer; //length of the number string
	m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + 1);
	p = m_ptr;
	do
	{
		*p++ = *--pNumBuf;
	} while(pNumBuf != numberBuffer);
	*(m_ptr + m_len) = '\0';
	return (*this);
}

long long KviCString::toLongLong(bool * bOk) const
{
	long long result = 0;
	if(bOk)
		*bOk = false;
	char * p = m_ptr;
	bool bNeg = false;
	while(isspace(*p))
		p++; //skip spaces
	if(*p == '-')
	{
		bNeg = true;
		p++;
	}
	else
	{
		if(*p == '+')
			p++;
	}
	if(isdigit(*p))
	{ //point to something interesting ?
		do
		{
			result = (result * 10) + (*p - '0');
			p++;
		} while(isdigit(*p));
		if(bNeg)
			result = -result;
		while(isspace(*p))
			p++; //skip trailing spaces
		if(*p)
			return 0; //if this is not the end...die.
		if(bOk)
			*bOk = true;
		return result;
	}
	return 0;
}

unsigned long long KviCString::toULongLong(bool * bOk) const
{
	unsigned long long result = 0;
	if(bOk)
		*bOk = false;
	char * p = m_ptr;
	while(isspace(*p))
		p++; //skip spaces
	if(isdigit(*p))
	{ //point to something interesting ?
		do
		{
			result = (result * 10) + (*p - '0');
			p++;
		} while(isdigit(*p));
		while(isspace(*p))
			p++; //skip trailing spaces
		if(*p)
			return 0; //if this is not the end...die.
		if(bOk)
			*bOk = true;
		return result;
	}
	return 0;
}

long KviCString::toLong(bool * bOk) const
{
	long result = 0;
	if(bOk)
		*bOk = false;
	char * p = m_ptr;
	bool bNeg = false;
	while(isspace(*p))
		p++; //skip spaces
	if(*p == '-')
	{
		bNeg = true;
		p++;
	}
	else
	{
		if(*p == '+')
			p++;
	}
	if(isdigit(*p))
	{ //point to something interesting ?
		do
		{
			result = (result * 10) + (*p - '0');
			p++;
		} while(isdigit(*p));
		if(bNeg)
			result = -result;
		while(isspace(*p))
			p++; //skip trailing spaces
		if(*p)
			return 0; //if this is not the end...die.
		if(bOk)
			*bOk = true;
		return result;
	}
	return 0;
}

unsigned long KviCString::toULong(bool * bOk) const
{
	unsigned long result = 0;
	if(bOk)
		*bOk = false;
	char * p = m_ptr;
	while(isspace(*p))
		p++; //skip spaces
	if(isdigit(*p))
	{ //point to something interesting ?
		do
		{
			result = (result * 10) + (*p - '0');
			p++;
		} while(isdigit(*p));
		while(isspace(*p))
			p++; //skip trailing spaces
		if(*p)
			return 0; //if this is not the end...die.
		if(bOk)
			*bOk = true;
		return result;
	}
	return 0;
}

long KviCString::toLongExt(bool * bOk, int base)
{
	if(m_len == 0)
	{
		if(bOk)
			*bOk = false;
		return 0;
	}
	char * endptr;
	long result = strtol(m_ptr, &endptr, base);
	if(*endptr)
	{
		// must be whitespaces, otherwise there is trailing garbage inside
		while(isspace(*endptr) && (*endptr))
			endptr++;
		if(*endptr)
		{
			// still not at the end
			// trailing garbage not allowed
			if(bOk)
				*bOk = false;
			return result;
		}
	}
	if(bOk)
		*bOk = true;
	return result;
}

KviCString & KviCString::cutLeft(int len)
{
	KVI_ASSERT(len >= 0);
	if(len <= m_len)
	{
		m_len -= len;
		KviMemory::move(m_ptr, m_ptr + len, m_len + 1);
		m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + 1);
	}
	else
	{
		m_ptr = (char *)KviMemory::reallocate(m_ptr, 1);
		*m_ptr = '\0';
		m_len = 0;
	}
	return (*this);
}

KviCString & KviCString::cutRight(int len)
{
	KVI_ASSERT(len >= 0);
	if(len <= m_len)
	{
		m_len -= len;
		m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + 1);
		*(m_ptr + m_len) = '\0';
	}
	else
	{
		m_ptr = (char *)KviMemory::reallocate(m_ptr, 1);
		*m_ptr = '\0';
		m_len = 0;
	}
	return (*this);
}

KviCString & KviCString::cut(int idx, int len)
{
	KVI_ASSERT(idx >= 0);
	KVI_ASSERT(len >= 0);
	if(idx < m_len)
	{
		// idx = 3 len = 3 m_len = 10
		// 0123456789
		// abcdefghij
		//    ^  ^
		//   p1  p2
		char * p1 = m_ptr + idx;
		if(len + idx > m_len)
			len = m_len - idx;
		char * p2 = p1 + len;
		KviMemory::move(p1, p2, (m_len - (len + idx)) + 1);
		m_len -= len;
		m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + 1);
	}
	return (*this);
}

KviCString & KviCString::cutToFirst(char c, bool bIncluded)
{
	int idx = findFirstIdx(c);
	if(idx != -1)
		cutLeft(bIncluded ? idx + 1 : idx);
	return (*this);
}

KviCString KviCString::leftToFirst(char c, bool bIncluded) const
{
	int idx = findFirstIdx(c);
	if(idx == -1)
		return KviCString(*this);
	return KviCString(m_ptr, bIncluded ? idx + 1 : idx);
}

KviCString KviCString::leftToLast(char c, bool bIncluded) const
{
	int idx = findLastIdx(c);
	return KviCString(m_ptr, bIncluded ? idx + 1 : idx);
}

KviCString & KviCString::cutFromFirst(char c, bool bIncluded)
{
	int idx = findFirstIdx(c);
	if(idx != -1)
		cutRight(bIncluded ? (m_len - idx) : (m_len - (idx + 1)));
	return (*this);
}

KviCString & KviCString::cutToLast(char c, bool bIncluded)
{
	int idx = findLastIdx(c);
	if(idx != -1)
		cutLeft(bIncluded ? idx + 1 : idx);
	return (*this);
}

KviCString & KviCString::cutFromLast(char c, bool bIncluded)
{
	int idx = findLastIdx(c);
	if(idx != -1)
		cutRight(bIncluded ? (m_len - idx) : (m_len - (idx + 1)));
	return (*this);
}

KviCString & KviCString::cutToFirst(const char * c, bool bIncluded)
{
	int len = (int)strlen(c);
	int idx = findFirstIdx(c);
	if(idx != -1)
		cutLeft(bIncluded ? idx + len : idx);
	return (*this);
}

KviCString & KviCString::cutFromFirst(const char * c, bool bIncluded)
{
	int len = (int)strlen(c);
	int idx = findFirstIdx(c);
	if(idx != -1)
		cutRight(bIncluded ? (m_len - idx) : (m_len - (idx + len)));
	return (*this);
}

KviCString & KviCString::cutToLast(const char * c, bool bIncluded)
{
	int len = (int)strlen(c);
	int idx = findLastIdx(c);
	if(idx != -1)
		cutLeft(bIncluded ? idx + len : idx);
	return (*this);
}

KviCString & KviCString::cutFromLast(const char * c, bool bIncluded)
{
	int len = (int)strlen(c);
	int idx = findLastIdx(c);
	if(idx != -1)
		cutRight(bIncluded ? (m_len - idx) : (m_len - (idx + len)));
	return (*this);
}

KviCString & KviCString::setLen(int iLen)
{
	KVI_ASSERT(iLen >= 0);
	m_len = iLen;
	m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + 1);
	*(m_ptr + m_len) = '\0';
	return (*this);
}

KviCString & KviCString::padRight(int iLen, const char c)
{
	KVI_ASSERT(iLen >= 0);
	m_ptr = (char *)KviMemory::reallocate(m_ptr, iLen + 1);
	*(m_ptr + iLen) = '\0';
	if(iLen > m_len)
		KviMemory::set(m_ptr + m_len, c, iLen - m_len);
	m_len = iLen;
	return (*this);
}

KviCString & KviCString::stripLeftWhiteSpace()
{
	char * p = m_ptr;
	while(isspace(*p))
		p++;
	m_len -= (p - m_ptr);
	KviMemory::move(m_ptr, p, m_len + 1);
	m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + 1);
	return (*this);
}

KviCString & KviCString::stripLeft(char c)
{
	KVI_ASSERT(c != '\0');
	char * p = m_ptr;
	while(*p == c)
		p++;
	m_len -= (p - m_ptr);
	KviMemory::move(m_ptr, p, m_len + 1);
	m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + 1);
	return (*this);
}

bool KviCString::getToken(KviCString & str, char sep)
{
	KVI_ASSERT(str.m_ptr);
	KVI_ASSERT(str.m_ptr != m_ptr);
	char * p = m_ptr;
	//skip to the end
	while(*p && (*p != sep))
		p++;
	//0123456789
	//abcd xyz
	//^   ^
	str.m_len = p - m_ptr;
	str.m_ptr = (char *)KviMemory::reallocate(str.m_ptr, str.m_len + 1);
	KviMemory::copy(str.m_ptr, m_ptr, str.m_len);
	*(str.m_ptr + str.m_len) = '\0';
	while(*p && (*p == sep))
		p++;
	cutLeft(p - m_ptr);
	return (m_len != 0);
}

bool KviCString::getLine(KviCString & str)
{
	KVI_ASSERT(str.m_ptr);
	KVI_ASSERT(str.m_ptr != m_ptr);
	if(m_len == 0)
		return false;
	char * p = m_ptr;
	//skip to the end
	while(*p && (*p != '\n'))
		p++;
	//0123456789
	//abcd xyz
	//^   ^
	str.m_len = p - m_ptr;
	str.m_ptr = (char *)KviMemory::reallocate(str.m_ptr, str.m_len + 1);
	KviMemory::copy(str.m_ptr, m_ptr, str.m_len);
	*(str.m_ptr + str.m_len) = '\0';
	p++;
	cutLeft(p - m_ptr);
	return true;
}

KviCString KviCString::getToken(char sep)
{
	char * p = m_ptr;
	while(*p && (*p != sep))
		p++;
	KviCString ret(m_ptr, p);
	while(*p && (*p == sep))
		p++;
	cutLeft(p - m_ptr);
	return ret;
}

KviCString & KviCString::vsprintf(const char * fmt, kvi_va_list list)
{
	kvi_va_list save;
	kvi_va_copy(save, list);

	m_ptr = (char *)KviMemory::reallocate(m_ptr, 256);
	//First try
	//print...with max 256 chars
	m_len = kvi_vsnprintf(m_ptr, 256, fmt, list);

	//check if we failed
	if(m_len < 0)
	{
		//yes, failed....
		int dummy = 256;
		do
		{ //we failed, so retry with 256 more chars
			dummy += 256;
			//realloc
			m_ptr = (char *)KviMemory::reallocate(m_ptr, dummy);
			//print...
			kvi_va_copy(list, save);
			m_len = kvi_vsnprintf(m_ptr, dummy, fmt, list);
		} while(m_len < 0);
	}
	//done...
	//now m_len is the length of the written string not including the terminator...
	//perfect! :)
	m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + 1);
	kvi_va_end(save);
	return (*this);
}

KviCString & KviCString::sprintf(const char * fmt, ...)
{
	m_ptr = (char *)KviMemory::reallocate(m_ptr, 256);
	//First try
	kvi_va_list list;
	kvi_va_start(list, fmt);
	//print...with max 256 chars
	m_len = kvi_vsnprintf(m_ptr, 256, fmt, list);
	kvi_va_end(list);

	//check if we failed
	if(m_len < 0)
	{
		//yes, failed....
		int dummy = 256;
		do
		{ //we failed, so retry with 256 more chars
			dummy += 256;
			//realloc
			m_ptr = (char *)KviMemory::reallocate(m_ptr, dummy);
			//print...
			kvi_va_start(list, fmt);
			m_len = kvi_vsnprintf(m_ptr, dummy, fmt, list);
			kvi_va_end(list);
		} while(m_len < 0);
	}
	//done...
	//now m_len is the length of the written string not including the terminator...
	//perfect! :)
	m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + 1);
	return (*this);
}

int KviCString::find(const char * str, int idx, bool caseS) const
{
	if(idx >= m_len)
		return -1;
	char * p = m_ptr + idx;
	int len = (int)strlen(str);
	if(caseS)
	{
		for(;;)
		{
			while(*p && (*p != *str))
				p++;
			if(*p)
			{
				if(kvi_strEqualCSN(str, p, len))
					return (p - m_ptr);
				else
					p++;
			}
			else
				return -1;
		}
	}
	else
	{
		for(;;)
		{
			char tmp = toupper(*str);
			while(*p && (toupper(*p) != tmp))
				p++;
			if(*p)
			{
				if(kvi_strEqualCIN(str, p, len))
					return (p - m_ptr);
				else
					p++;
			}
			else
				return -1;
		}
	}
}

int KviCString::find(char c, int idx) const
{
	if(idx >= m_len)
		return -1;
	char * p = m_ptr + idx;
	while(*p && (*p != c))
		p++;
	return (*p ? p - m_ptr : -1);
}

int KviCString::findRev(const char * str, int idx, bool caseS) const
{
	if((m_len + idx) < 0)
		return -1;
	char * p = m_ptr + m_len + idx;
	int len = (int)strlen(str);
	if(caseS)
	{
		for(;;)
		{
			while((p >= m_ptr) && (*p != *str))
				p--;
			if(p >= m_ptr)
			{
				if(kvi_strEqualCSN(str, p, len))
					return (p - m_ptr);
				else
					p--;
			}
			else
				return -1;
		}
	}
	else
	{
		for(;;)
		{
			char tmp = toupper(*str);
			while((p >= m_ptr) && (toupper(*p) != tmp))
				p--;
			if(p >= m_ptr)
			{
				if(kvi_strEqualCIN(str, p, len))
					return (p - m_ptr);
				else
					p--;
			}
			else
				return -1;
		}
	}
}

int KviCString::findFirstIdx(char c) const
{
	char * p = m_ptr;
	while(*p && (*p != c))
		p++;
	return (*p ? p - m_ptr : -1);
}

int KviCString::findFirstIdx(const char * str, bool caseS) const
{
	// This function can't be used to search inside
	// multibyte encoded strings... convert your
	// code to QString and use QString::findRev().
	// We must throw away KviCString at all in this case...

	// return QString(m_ptr).find(QString(str),0,caseS);

	// Both this KviCString and the const char * str are assumed
	// to be in the proper (and same) encoding.
	// If KviCString is in encoding A then QString(m_ptr) might
	// or not be decoded correctly.
	// Also if KviCString is in UTF-8 (for example), then
	// a position in QString() does not map to the position in the char array
	// since a single UNICODE char may use one or more bytes...

	KVI_ASSERT(str);
	char * p = m_ptr;
	int len = (int)strlen(str);
	if(caseS)
	{
		for(;;)
		{
			while(*p && (*p != *str))
				p++;
			if(*p)
			{
				if(kvi_strEqualCSN(str, p, len))
					return (p - m_ptr);
				else
					p++;
			}
			else
				return -1;
		}
	}
	else
	{
		// this will NOT work for strings that aren't in the current system encoding :(
		for(;;)
		{
			char tmp = toupper(*str);
			while(*p && (toupper(*p) != tmp))
				p++;
			if(*p)
			{
				if(kvi_strEqualCIN(str, p, len))
					return (p - m_ptr);
				else
					p++;
			}
			else
				return -1;
		}
	}
}

int KviCString::findLastIdx(char c) const
{
	//Empty string ?
	if(m_len < 1)
		return -1;
	//p points to the last character in the string
	char * p = ((m_ptr + m_len) - 1);
	//go back until we find a match or we run to the first char in the string.
	while((*p != c) && (p > m_ptr))
		p--;
	//if *p == c --> matched, else we are at the beginning of the string.
	return ((*p == c) ? p - m_ptr : -1);
}

int KviCString::findLastIdx(const char * str, bool caseS) const
{
	// This function can't be used to search inside
	// multibyte encoded strings... convert your
	// code to QString and use QString::findRev().
	// We must throw away KviCString at all in this case...

	// return QString(m_ptr).findRev(QString(str),-1,caseS);

	KVI_ASSERT(str);
	//Calc the len of the searched string
	int len = (int)strlen(str);
	//Too long ?
	if(m_len < len)
		return -1;
	//p points to the last character in the string
	char * p = ((m_ptr + m_len) - 1);
	if(caseS)
	{
		for(;;)
		{
			//go back until we find a character that mathes or we run to the first char.
			while((*p != *str) && (p > m_ptr))
				p--;
			if(*p == *str)
			{
				//maybe occurrence....
				if(kvi_strEqualCSN(str, p, len))
					return (p - m_ptr);
				else
				{
					//Nope...continue if there is more data to check...
					if(p == m_ptr)
						return -1;
					p--;
				}
			}
			else
				return -1; //Beginning of the string
		}
	}
	else
	{
		// case insensitive
		for(;;)
		{
			//go back until we find a character that mathes or we run to the first char.
			char tmp = toupper(*str);
			while((toupper(*p) != tmp) && (p > m_ptr))
				p--;
			if(toupper(*p) == tmp)
			{
				//maybe occurrence....
				if(kvi_strEqualCIN(str, p, len))
					return (p - m_ptr);
				else
				{
					//Nope...continue if there is more data to check...
					if(p == m_ptr)
						return -1;
					p--;
				}
			}
			else
				return -1; //Beginning of the string
		}
	}
}

KviCString & KviCString::trim()
{
	// 0123456789
	//    abcd   0
	// ^        ^
	// left   right
	char * left = m_ptr;
	char * right = m_ptr + m_len - 1;
	// skip initial spaces
	while(isspace(*left))
		left++;
	if(*left)
	{
		// valid string, left points to first non-space
		while((right >= left) && isspace(*right))
			right--;
		// 0123456789
		//    abcd   0
		//    ^  ^
		// left   right
		m_len = (right - left) + 1;
		KviMemory::move(m_ptr, left, m_len);
		m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + 1);
		*(m_ptr + m_len) = '\0';
	}
	else
	{
		m_ptr = (char *)KviMemory::reallocate(m_ptr, 1);
		*m_ptr = '\0';
		m_len = 0;
	}
	return (*this);
}

KviCString & KviCString::stripRightWhiteSpace()
{
	if(*m_ptr)
	{
		char * right = m_ptr + m_len - 1;
		const char * start = right;
		//isspace accepts 0..255 values in MSVC
		while((right >= m_ptr) && ((unsigned)(*right + 1) <= 256) && isspace(*right))
			right--;
		if(right != start)
		{
			m_len = (right - m_ptr) + 1;
			m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + 1);
			*(m_ptr + m_len) = '\0';
		}
	}
	return (*this);
}

KviCString & KviCString::stripRight(char c)
{
	if(*m_ptr)
	{
		char * right = m_ptr + m_len - 1;
		const char * start = right;
		while((right >= m_ptr) && (*right == c))
			right--;
		if(right != start)
		{
			m_len = (right - m_ptr) + 1;
			m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + 1);
			*(m_ptr + m_len) = '\0';
		}
	}
	return (*this);
}

KviCString & KviCString::stripSpace()
{
	// 0123456789
	//    abcd   0
	// ^        ^
	// left   right
	char * left = m_ptr;
	char * right = m_ptr + m_len - 1;
	// skip initial spaces
	while((*left == ' ') || (*left == '\t'))
		left++;
	if(*left)
	{
		// valid string, left points to first non-space
		while((right >= left) && ((*right == ' ') || (*right == '\t')))
			right--;
		// 0123456789
		//    abcd   0
		//    ^  ^
		// left   right
		m_len = (right - left) + 1;
		KviMemory::move(m_ptr, left, m_len);
		m_ptr = (char *)KviMemory::reallocate(m_ptr, m_len + 1);
		*(m_ptr + m_len) = '\0';
	}
	else
	{
		m_ptr = (char *)KviMemory::reallocate(m_ptr, 1);
		*m_ptr = '\0';
		m_len = 0;
	}
	return (*this);
}

bool KviCString::isNum() const
{
	char * p = m_ptr;
	while(isspace(*p))
		p++;
	if(*p == '-')
		p++;
	if(!isdigit(*p))
		return false;
	while(isdigit(*p))
		p++;
	while(isspace(*p))
		p++;
	return (*p == '\0');
}

bool KviCString::isUnsignedNum() const
{
	char * p = m_ptr;
	while(isspace(*p))
		p++;
	if(!isdigit(*p))
		return false;
	while(isdigit(*p))
		p++;
	while(isspace(*p))
		p++;
	return (*p == '\0');
}

static KviCString g_szApplicationWideEmptyString;

KviCString & KviCString::emptyString()
{
	return g_szApplicationWideEmptyString;
}

bool KviCString::ext_contains(const char * data, const char * item, bool caseS)
{
	if(item && data)
	{
		int len = (int)strlen(item);
		char c = tolower(*item);
		if(caseS)
		{
			while(*data)
			{
				while(*data && (tolower(*data) != c))
					data++;
				if(*data)
				{
					if(kvi_strEqualCSN(item, data, len))
						return true;
					else
						data++;
				}
			}
		}
		else
		{
			while(*data)
			{
				while(*data && (tolower(*data) != c))
					data++;
				if(*data)
				{
					if(kvi_strEqualCIN(item, data, len))
						return true;
					else
						data++;
				}
			}
		}
	}
	return false;
}
