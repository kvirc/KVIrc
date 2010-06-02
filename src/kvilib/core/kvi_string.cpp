//=============================================================================
//
//   File : kvi_string.cpp
//   Creation date : Fri Mar 19 1999 03:20:45 by Szymon Stefanek
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



#define _KVI_DEBUG_CHECK_RANGE_
#include "kvi_debug.h"

#define _KVI_STRING_CPP_
#include "kvi_string.h"

#include "kvi_memmove.h"
#include "kvi_malloc.h"

#include "kvi_qstring.h"

kvi_wslen_t kvi_wstrlen(const kvi_wchar_t * str)
{
	const kvi_wchar_t * ptr = str;
	while(*ptr)ptr++;
	return (ptr - str);
}

/*
WORKING CODE, COMMENTED OUT BECAUSE NOT USED AND GENERATES WARNINGS
// %s = Latin1 char string (can't be null)
// %d = signed int (short,char)
// %u = unsigned int (short,char)
// %c = char value (kvi_wchar_t value)

// %f = double value

// %w = kvi_wchar_t string (can't be null)

// %S = Latin1 KviStr pointer (#ifdef WSTRINGCONFIG_USE_KVISTR) : can't be NULL!
// %W = KviWStr pointer : can't be NULL!
// %Q = QString pointer : can't be NULL!

#define _WSTRING_WMEMCPY(_dst,_src,_len) kvi_fastmoveodd((void *)(_dst),(const void *)(_src),sizeof(kvi_wchar_t) * (_len))
#define _WSTRING_STRLEN(_str) kvi_strLen(_str)

#define WVSNPRINTF_BODY \
\
	register kvi_wchar_t *p; \
	long int argValue; \
	unsigned long argUValue; \
\
	kvi_wchar_t numberBuffer[32]; \
	kvi_wchar_t *pNumBuf; \
	unsigned int tmp; \
\
	for(p=buffer ; *fmt ; ++fmt) \
	{ \
		if(len < 1)return (-1); \
\
		if(*fmt != '%') \
		{ \
			*p++ = *fmt; \
			--len; \
			continue; \
		} \
\
		++fmt; \
\
		switch(*fmt) \
		{ \
			case 's': \
			{ \
				char * argString = kvi_va_arg(list,char *); \
				argValue = (int)_WSTRING_STRLEN(argString); \
				if(len <= argValue)return (-1); \
				while(*argString)*p++ = *argString++; \
				len -= argValue; \
			} \
			break; \
			case 'S': \
			{ \
				KviStr * pString = kvi_va_arg(list,KviStr *); \
				char * argString = pString->ptr(); \
				if(len <= ((int)(pString->len())))return (-1); \
				while(*argString)*p++ = *argString++; \
				len -= pString->len(); \
			} \
			break; \
			case 'Q': \
			{ \
				QString * pString = kvi_va_arg(list,QString *); \
				if(pString->length() > 0) \
				{ \
					if(len <= ((int)(pString->length())))return (-1); \
					_WSTRING_WMEMCPY(p,pString->unicode(),pString->length()); \
					p += pString->length(); \
					len -= pString->length(); \
				} \
			} \
			break; \
			case 'd': \
				argValue = kvi_va_arg(list,int); \
				if(argValue < 0) \
				{ \
					*p++ = '-'; \
					if(--len == 0)return (-1); \
					argValue = -argValue; \
					if(argValue < 0)argValue = 0; \
				} \
				pNumBuf = numberBuffer; \
				do { \
					tmp = argValue / 10; \
					*pNumBuf++ = argValue - (tmp * 10) + '0'; \
				} while((argValue = tmp)); \
				argUValue = pNumBuf - numberBuffer; \
				if(((unsigned int)len) <= argUValue)return (-1); \
				do { \
					*p++ = *--pNumBuf; \
				} while(pNumBuf != numberBuffer); \
				len -= argUValue; \
			break; \
			case 'u': \
				argUValue = kvi_va_arg(list,unsigned int); \
				pNumBuf = numberBuffer; \
				do { \
					tmp = argUValue / 10; \
					*pNumBuf++ = argUValue - (tmp * 10) + '0'; \
				} while((argUValue = tmp)); \
				argValue = pNumBuf - numberBuffer; \
				if(len <= argValue)return (-1); \
				do { \
					*p++ = *--pNumBuf; \
				} while(pNumBuf != numberBuffer); \
				len -= argValue; \
			break; \
			case 'f': \
			{ \
				double dVal = (double)kvi_va_arg(list,double); \
				char sprintfBuffer[32]; \
				argValue = sprintf(sprintfBuffer,"%f",dVal); \
				if(len <= argValue)return (-1); \
				char * pSprintfBuffer = sprintfBuffer; \
				while(*pSprintfBuffer)*p++ = *pSprintfBuffer++; \
				len -= argValue; \
			} \
			break; \
			case 'c': \
				*p++ = (kvi_wchar_t)kvi_va_arg(list,int); \
				--len; \
			break; \
			default: \
				*p++ = '%';  \
				if(--len == 0)return (-1); \
				if(*fmt){ \
					*p++ = *fmt; \
					--len; \
				} \
			break; \
		} \
		continue; \
	} \
	if(len < 1)return (-1); \
	*p = 0; \
	return p-buffer;

int kvi_wvsnprintcf(kvi_wchar_t *buffer,kvi_wslen_t len,const char *fmt,kvi_va_list list)
{
	WVSNPRINTF_BODY
}

int kvi_wvsnprintf(kvi_wchar_t *buffer,kvi_wslen_t len,const kvi_wchar_t *fmt,kvi_va_list list)
{
	WVSNPRINTF_BODY
}
*/

bool kvi_qstringEqualCI(const QString &s1,const QString &s2)
{
	const QChar * p1 = s1.unicode();
	const QChar * p2 = s2.unicode();
	int l = s1.length() < s2.length() ? s1.length() : s2.length();

	while(l-- && (p1->toLower() == p2->toLower()))p1++,p2++;

	if(l==-1)return true;
	return false;
}

bool kvi_matchStringCI(register const char * exp,register const char * str)
{
	//               a
	//               .
	// exp = a*x?mem*a
	// str = arexoxmexamemizazv
	//                         .
	//                         n
	const char * afterWild = 0;
	const char * nextStrToCheck = 0;

	while(*exp)
	{
		if(*exp == '*')
		{
			// exp is a wildcard...
			afterWild = ++exp;
			nextStrToCheck = str + 1;
			if(!(*exp))return true; // and it's the last char in the string: matches everything ahead
			continue;
		}

		if(!(*str))return false; // str finished but we had something to match :(

		if(tolower(*exp) == tolower(*str))
		{
			// chars matched
			++exp;
			++str;
			if((!(*exp)) && *str)goto check_recovery;
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


bool kvi_matchStringCS(register const char * exp,register const char * str)
{
	//               a
	//               .
	// exp = a*x?mem*a
	// str = arexoxmexamemizazv
	//                         .
	//                         n
	const char * afterWild = 0;
	const char * nextStrToCheck = 0;

	while(*exp)
	{
		if(*exp == '*')
		{
			// exp is a wildcard...
			afterWild = ++exp;
			nextStrToCheck = str + 1;
			if(!(*exp))return true; // and it's the last char in the string: matches everything ahead
			continue;
		}

		if(!(*str))return false; // str finished but we had something to match :(

		if(*exp == *str)
		{
			// chars matched
			++exp;
			++str;
			if((!(*exp)) && *str)goto check_recovery;
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



bool kvi_matchStringWithTerminator(register const char * exp,register const char * str,char terminator,const char ** r1,const char ** r2)
{
#define NOT_AT_END(__str) (*__str && (*__str != terminator))

	//               a
	//               .
	// exp = a*x?mem*a
	// str = arexoxmexamemizazv
	//                         .
	//                         n
	const char * afterWild = 0;
	const char * nextStrToCheck = 0;

	while(NOT_AT_END(exp))
	{
		if(*exp == '*')
		{
			// exp is a wildcard...
			afterWild = ++exp;
			nextStrToCheck = str + 1;
			if(!(NOT_AT_END(exp)))
			{
				while(NOT_AT_END(str))str++;
				*r1 = exp;
				*r2 = str;
				return true; // and it's the last char in the string: matches everything ahead
			}
			continue;
		}

		if(!(*str))return false; // str finished but we had something to match :(

		if(tolower(*exp) == tolower(*str))
		{
			// chars matched
			++exp;
			++str;
			if((!(NOT_AT_END(exp))) && NOT_AT_END(str))goto check_recovery;
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

bool kvi_matchWildExpr(register const char *m1,register const char *m2)
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

	if(!(m1 && m2 && (*m1)))return false;
	const char * savePos1 = 0;
	const char * savePos2 = m2;
	while(*m1)
	{
		//loop managed by m1 (initially first mask)
		if(*m1=='*')
		{
			//Found a wildcard in m1
			savePos1 = ++m1;            //move to the next char and save the position...this is our jolly
			if(!*savePos1)return true;  //last was a wildcard , matches everything ahead...
			savePos2 = m2+1;            //next return state for the second string
			continue;                   //and return
		}
		if(!(*m2))return false;         //m2 finished and we had something to match here!
		if(tolower(*m1)==tolower(*m2))
		{
			//chars matched
			m1++;                       //Go ahead in the two strings
			m2++;                       //
			if((!(*m1)) && *m2 && savePos1)
			{
				//m1 finished , but m2 not yet and we have a savePosition for m1 (there was a wildcard)...
				//retry matching the string following the * from the savePos2 (one char ahead last time)
				m1 = savePos1;          //back to char after wildcard
				m2 = savePos2;          //back to last savePos2
				savePos2++;             //next savePos2 will be next char
			}
		} else {
			if(*m2 == '*')
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
			if((*m1 == '?') || (*m2 == '?'))
			{
				m1++;
				m2++;
				if((!(*m1)) && *m2 && savePos1)
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
	return (!(*m2));                     //m1 surely finished , so for the match , m2 must be finished too

}

/*

	WARNING: Don't remove: working code but actually unused in KVIrc
			Later it might become useful

bool kvi_matchWildExprCS(register const char *m1,register const char *m2)
{
	if(!(m1 && m2 && (*m1)))return false;
	const char * savePos1 = 0;
	const char * savePos2 = m2;
	while(*m1){ //loop managed by m1 (initially first mask)
		if(*m1=='*'){
			//Found a wildcard in m1
			savePos1 = ++m1;            //move to the next char and save the position...this is our jolly
			if(!*savePos1)return true;  //last was a wildcard , matches everything ahead...
			savePos2 = m2+1;            //next return state for the second string
			continue;                   //and return
		}
		if(!(*m2))return false;         //m2 finished and we had something to match here!
		if((*m1)==(*m2)){
			//chars matched
			m1++;                       //Go ahead in the two strings
			m2++;                       //
			if((!(*m1)) && *m2 && savePos1){
				//m1 finished , but m2 not yet and we have a savePosition for m1 (there was a wildcard)...
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
	return (!(*m2));                     //m1 surely finished , so for the match , m2 must be finished too

}
*/

bool kvi_matchWildExprWithTerminator(register const char *m1,register const char *m2,char terminator,
			const char ** r1,const char ** r2)
{
	//Matches two regular expressions containging wildcards

#define NOT_AT_END(__str) (*__str && (*__str != terminator))

	bool bSwapped = false;
	if(!(m1 && m2 && (NOT_AT_END(m1))))return false;
	const char * savePos1 = 0;
	const char * savePos2 = m2;
	while(NOT_AT_END(m1))
	{
		//loop managed by m1 (initially first mask)
		if(*m1=='*')
		{
			//Found a wildcard in m1
			savePos1 = ++m1;            //move to the next char and save the position...this is our jolly
			if(!NOT_AT_END(savePos1))
			{
				//last was a wildcard , matches everything ahead...
				while(NOT_AT_END(m2))m2++;
				*r1 = bSwapped ? m2 : m1;
				*r2 = bSwapped ? m1 : m2;
				return true;
			}
			savePos2 = m2+1;            //next return state for the second string
			continue;                   //and return
		}
		if(!NOT_AT_END(m2))return false;         //m2 finished and we had something to match here!
		if(tolower(*m1)==tolower(*m2))
		{
			//chars matched
			m1++;                       //Go ahead in the two strings
			m2++;                       //
			if((!NOT_AT_END(m1)) && NOT_AT_END(m2) && savePos1)
			{
				//m1 finished , but m2 not yet and we have a savePosition for m1 (there was a wildcard)...
				//retry matching the string following the * from the savePos2 (one char ahead last time)
				m1 = savePos1;          //back to char after wildcard
				m2 = savePos2;          //back to last savePos2
				savePos2++;             //next savePos2 will be next char
			}
		} else {
			if(*m2 == '*')
			{
				//A wlidcard in the second string
				//Invert the game : mask1 <-> mask2
				//mask2 now leads the game...
				bSwapped = !bSwapped;
				savePos1 = m1;          //aux
				m1 = m2;                //...swap
				m2 = savePos1;          //...swap
				savePos1 = m1;          //sync save pos1
				savePos2 = m2 + 1;      //sync save pos2
				continue;               //...and again
			}
			// m1 != m2 , m1 != * , m2 != *
			if((*m1 == '?') || (*m2 == '?'))
			{
				m1++;
				m2++;
				if((!NOT_AT_END(m1)) && NOT_AT_END(m2) && savePos1)
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
	*r1 = bSwapped ? m2 : m1;
	*r2 = bSwapped ? m1 : m2;

	return (!NOT_AT_END(m2));           //m1 surely finished , so for the match , m2 must be finished too

#undef NOT_AT_END
}



const char * kvi_extractToken(KviStr &str,const char *aux_ptr,char sep)
{
	__range_valid(aux_ptr);
	while(*aux_ptr && (*aux_ptr == sep))aux_ptr++;
	const char *p=aux_ptr;
	while(*p && (*p != sep))p++;
	str.m_len=p-aux_ptr;
	str.m_ptr = (char *)kvi_realloc(str.m_ptr,str.m_len+1);
	kvi_fastmove(str.m_ptr,aux_ptr,str.m_len);
	*(str.m_ptr+str.m_len)='\0';
	while(*p && (*p == sep))p++;
	return p;
}

const char * kvi_extractUpTo(KviStr &str,const char *aux_ptr,char sep)
{
	__range_valid(aux_ptr);
	const char *p=aux_ptr;
	while(*p && (*p != sep))p++;
	str.m_len=p-aux_ptr;
	str.m_ptr = (char *)kvi_realloc(str.m_ptr,str.m_len+1);
	kvi_fastmove(str.m_ptr,aux_ptr,str.m_len);
	*(str.m_ptr+str.m_len)='\0';
	return p;
}

int kvi_vsnprintf(char *buffer,int len,const char *fmt,kvi_va_list list)
{
	__range_valid(fmt);
	__range_valid(buffer);
	__range_valid(len > 0); //printing 0 characters is senseless

	register char *p;
	char *argString;
	long argValue;
	unsigned long argUValue;

	//9999999999999999999999999999999\0
	char numberBuffer[32]; //enough ? 10 is enough for 32bit unsigned int...
	char *pNumBuf;
	unsigned int tmp;


	for(p=buffer ; *fmt ; ++fmt)
	{
		if(len < 1)return (-1); //not enough space ... (in fact this could be len < 2 for the terminator)
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
				argString = kvi_va_arg(list,char *);
				if(!argString)continue;
				argValue = (long)strlen(argString);
				//check for space...
				if(len <= argValue)return (-1); //not enough space for buffer and terminator
				while(*argString)*p++ = *argString++;
				len -= argValue;
				continue;
			case 'd': //signed integer
				argValue = kvi_va_arg(list,int);
				if(argValue < 0){ //negative integer
					*p++ = '-';
					if(--len == 0)return (-1);
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
				if(((uint)len) <= argUValue)return (-1); //not enough space for number and terminator
				do { *p++ = *--pNumBuf; } while(pNumBuf != numberBuffer);
				len -= argUValue;
				continue;
			case 'u': //unsigned integer
				argUValue = kvi_va_arg(list,unsigned int); //many implementations place int here
				//write the number in a temporary buffer
				pNumBuf = numberBuffer;
				do {
					tmp = argUValue / 10;
					*pNumBuf++ = argUValue - (tmp * 10) + '0';
				} while((argUValue = tmp));
				//copy now....
				argValue = pNumBuf - numberBuffer; //length of the number string
				if(len <= argValue)return (-1); //not enough space for number and terminator
				do { *p++ = *--pNumBuf; } while(pNumBuf != numberBuffer);
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
				*p++ = (char)kvi_va_arg(list,int);
				--len;
				continue;
			case 'Q': // QString! (this should almost never happen)
			{
				QString * s = kvi_va_arg(list,QString *);
				QByteArray cs = KviQString::toUtf8(*s);
				const char * t = cs.data();
				if(!t)continue; // nothing to do
				//check for space...
				if(len <= (int)cs.length())return (-1); //not enough space for buffer and terminator
				while(*t)*p++ = *t++;
				len -= cs.length();
				continue;
			}
			default: //a normal percent
				*p++ = '%';  //write it
				if(--len == 0)return (-1); //not enough space for next char or terminator
				if(*fmt){        //this if is just in case that we have a % at the end of the string.
					*p++ = *fmt; //and write this char
					--len;
				}
				continue;
		}
	}
	if(len < 1)return (-1); //missing space for terminator
	*p = '\0';
	return p-buffer;
}

//
// Nearly the same as the above function...
//

int kvi_irc_vsnprintf(char *buffer,const char *fmt,kvi_va_list list,bool *bTruncated)
{
	__range_valid(fmt);
	__range_valid(buffer);
	if( !( buffer && fmt) ) return false;
	register char *p;
	char *argString;
	long argValue;
	unsigned long argUValue;
	char numberBuffer[64]; //enough ? 10 is enough for 32bit unsigned int...
	char *pNumBuf;
	unsigned int tmp;
	*bTruncated = false;
	int len = 512;

	for (p=buffer ; *fmt ; ++fmt) {
		if(len < 3)goto truncate;
		//copy up to a '%'
		if (*fmt != '%') {
			*p++ = *fmt;
			--len;
			continue;
		}
		++fmt; //skip this '%'
		switch(*fmt){
			case 's': //string
				argString = kvi_va_arg(list,char *);
				if(!argString)continue;
				//check for space...
				while(*argString){
					*p++ = *argString++;
					if(--len < 3)goto truncate;
				}
				continue;
			case 'Q': // QString! (this should almost never happen)
			{
				QString * s = kvi_va_arg(list,QString *);
				QByteArray cs = KviQString::toUtf8(*s);
				const char * t = cs.data();
				if(!t)continue; // nothing to do
				while(*t)
				{
					*p++ = *t++;
					if(--len < 3)goto truncate;
				}
				continue;
			}
			case 'd': //signed integer
				argValue = kvi_va_arg(list,int);
				if(argValue < 0){ //negative integer
					*p++ = '-';
					if(--len < 3)goto truncate; //place just for CRLF
					argValue = -argValue; //need to have it positive
					if(argValue < 0)argValue = 0; // -0 (hack the exception)
				}
				//write the number in a temporary buffer
				pNumBuf = numberBuffer;
				do {
					tmp = argValue / 10;
					*pNumBuf++ = argValue - (tmp * 10) + '0';
				} while((argValue = tmp));
				//copy now....
				do {
					*p++ = *--pNumBuf;
					if(--len < 3)goto truncate;
				} while(pNumBuf != numberBuffer);
				continue;
			case 'u': //unsigned integer
				argUValue = kvi_va_arg(list,unsigned int); //many implementations place int here
				//write the number in a temporary buffer
				pNumBuf = numberBuffer;
				do {
					tmp = argUValue / 10;
					*pNumBuf++ = argUValue - (tmp * 10) + '0';
				} while((argUValue = tmp));
				//copy now....
				if(--len < 3)goto truncate; //no place for digits
				do {
					*p++ = *--pNumBuf;
					if(--len < 3)goto truncate;
				} while(pNumBuf != numberBuffer);
				continue;
			case 'c': //char
				*p++ = (char)kvi_va_arg(list,int);
				--len;
				continue;
			default: //a normal percent
				*p++ = '%';  //write it
				if(--len < 3)goto truncate; //not enough space for next char
				if(*fmt){        //this if is just in case that we have a % at the end of the string.
					*p++ = *fmt; //and write this char
					--len;
				}
				continue;
		}
	}
	//succesfull finish
	__range_valid(len >= 2);
	*p++ = '\r';
	*p   = '\n';
	return ((p-buffer)+1);
truncate:
	__range_valid(len >= 2);
	*bTruncated = true;
	*p++ = '\r';
	*p   = '\n';
	return ((p-buffer)+1);
}

#ifndef COMPILE_ix86_ASM

bool kvi_strEqualCS(const char *str1,const char *str2)
{
	__range_valid(str1);
	__range_valid(str2);
	if( !( str1 && str2 ) ) return false;
	register unsigned char *s1 = (unsigned char *)str1;
	register unsigned char *s2 = (unsigned char *)str2;
	while(*s1)if(*s1++ != *s2++)return false;
	return (*s1 == *s2);
}

bool kvi_strEqualCSN(const char *str1,const char *str2,int len)
{
	__range_valid(str1);
	__range_valid(str2);
	__range_valid(len >= 0);
	if( !( str1 && str2 && (len >= 0) ) ) return false;
	register unsigned char *s1 = (unsigned char *)str1;
	register unsigned char *s2 = (unsigned char *)str2;
	while(len-- && *s1)if(*s1++ != *s2++)return false;
	return (len < 0);
}

#endif

bool kvi_strEqualCIN(const char *str1,const char *str2,int len)
{
	__range_valid(str1);
	__range_valid(str2);
	__range_valid(len >= 0);
	if( !( str1 && str2 && (len >= 0) ) ) return false;
	register unsigned char *s1 = (unsigned char *)str1;
	register unsigned char *s2 = (unsigned char *)str2;
	while(len-- && *s1)if(tolower(*s1++) != tolower(*s2++))return false;
	return (len < 0);
}

bool kvi_strEqualCI(const char *str1,const char *str2)
{
	__range_valid(str1);
	__range_valid(str2);
	if( !( str1 && str2) ) return false;
	register unsigned char *s1 = (unsigned char *)str1;
	register unsigned char *s2 = (unsigned char *)str2;
	while(*s1)if(tolower(*s1++) != tolower(*s2++))return false;
	return (*s1 == *s2);
}

//note that greater here means that come AFTER in the alphabetic order
// return < 0 ---> str1 < str2
// return = 0 ---> str1 = str2
// return > 0 ---> str1 > str2
int kvi_strcmpCI(const char *str1,const char *str2)
{
	//abcd abce
	__range_valid(str1);
	__range_valid(str2);
	if( !( str1 && str2) ) return false;
	register unsigned char *s1 = (unsigned char *)str1;
	register unsigned char *s2 = (unsigned char *)str2;
	int diff;
	unsigned char rightchar;
	while(!(diff=(rightchar=tolower(*s1++)) - tolower(*s2++)))if(!rightchar)break;
    return diff; //diff is nonzero or end of both was reached (it is positive if *s2 > *s1
}

//
////note that greater here means that come AFTER in the alphabetic order
//// return < 0 ---> str1 < str2
//// return = 0 ---> str1 = str2
//// return > 0 ---> str1 > str2
//int kvi_strcmpCIN(const char *str1,const char *str2,int len)
//{
//	//abcd abce
//	__range_valid(str1);
//	__range_valid(str2);
//	register unsigned char *s1 = (unsigned char *)str1;
//	register unsigned char *s2 = (unsigned char *)str2;
//	int diff;
//	unsigned char rightchar;
//	while(len--)
//	{
//		if(!(diff=(rightchar=tolower(*s1++)) - tolower(*s2++)))break;
//		if(!rightchar)break;
//	}
//    return diff; //diff is nonzero or end of both was reached (it is positive if *s2 > *s1
//}

int kvi_strcmpCS(const char *str1,const char *str2)
{
	//abcd abce
	__range_valid(str1);
	__range_valid(str2);
	if( !( str1 && str2) ) return false;
	register unsigned char *s1 = (unsigned char *)str1;
	register unsigned char *s2 = (unsigned char *)str2;
	int diff;
	while(!(diff=(*s1)-(*s2++)))if(!*s1++)break;
    return diff; //diff is nonzero or end of both was reached (it is positive if *s2 > *s1
}

int kvi_strMatchRevCS(const char *str1, const char *str2, int index)
{
	__range_valid(str1);
	__range_valid(str2);
	if( !( str1 && str2) ) return false;
	register char *s1=(char *)str1;
	register char *s2=(char *)str2;

	int curlen=(int)strlen(str1);
	int diff;

	if (index<0 || index >= curlen) index = curlen-1;

	s1+=index;
	while (*s2) s2++;
	s2--;

	// now start comparing
	while (1){
		/* in this case, we have str1 = "lo" and str2 = "hello" */
		if (s1<str1 && !(s2<str2)) return 256;
		if (s2<str2) return 0;
		if ((diff=(*s1)-(*s2))) return diff;
		s1--;
		s2--;
	}
}

KviStr::KviStr()
{
	m_ptr = (char *)kvi_malloc(1);
	*m_ptr = '\0';
	m_len = 0;
}

KviStr::KviStr(const char *str)
{
	//Deep copy constructor
	if(str){
		//Deep copy
		m_len = (int)strlen(str);
		m_ptr = (char *)kvi_malloc(m_len+1);
		kvi_fastmove(m_ptr,str,m_len+1);
	} else {
		m_ptr = (char *)kvi_malloc(1);
		*m_ptr = '\0';
		m_len = 0;
	}
}

KviStr::KviStr(const QByteArray &str)
{
	//Deep copy constructor
	if(str.data())
	{
		//Deep copy
		m_len = str.length();
		m_ptr = (char *)kvi_malloc(m_len+1);
		kvi_fastmove(m_ptr,str,m_len+1);
	} else {
		m_ptr = (char *)kvi_malloc(1);
		*m_ptr = '\0';
		m_len = 0;
	}
}


KviStr::KviStr(const char *str,int len)
{
	__range_valid(str);
	//__range_valid(len <= ((int)strlen(str))); <-- we trust the user here (and a strlen() call may run AFTER len if data is not null terminated)
	__range_valid(len >= 0);
	m_len = len;
	m_ptr = (char *)kvi_malloc(m_len+1);
	kvi_fastmove(m_ptr,str,m_len);
	*(m_ptr+m_len) = '\0';
}

KviStr::KviStr(const char *bg,const char *end)
{
	__range_valid(bg);
	__range_valid(end);
	__range_valid(bg <= end);
	m_len = end-bg;
	m_ptr = (char *)kvi_malloc(m_len +1);
	kvi_fastmove(m_ptr,bg,m_len);
	*(m_ptr + m_len)='\0';
}

KviStr::KviStr(KviFormatConstructorTag,const char *fmt,...)
{
	m_ptr=(char *)kvi_malloc(256);
	//First try
	kvi_va_list list;
	kvi_va_start(list,fmt);
	//print...with max 256 chars
	m_len=kvi_vsnprintf(m_ptr,256,fmt,list);
	kvi_va_end(list);

	//check if we failed
	if(m_len < 0){
		//yes , failed....
		int dummy=256;
		do{ //we failed , so retry with 256 more chars
			dummy+=256;
			//realloc
			m_ptr=(char *)kvi_realloc(m_ptr,dummy);
			//print...
			kvi_va_start(list,fmt);
			m_len=kvi_vsnprintf(m_ptr,dummy,fmt,list);
			kvi_va_end(list);
		} while(m_len < 0);
	}
	//done...
	//now m_len is the length of the written string not including the terminator...
	//perfect! :)
	m_ptr=(char *)kvi_realloc(m_ptr,m_len+1);
}

KviStr::KviStr(const KviStr &str)
{
	__range_valid(str.m_ptr);
	m_len = str.m_len;
	m_ptr = (char *)kvi_malloc(m_len+1);
	kvi_fastmove(m_ptr,str.m_ptr,m_len+1);
}

KviStr::KviStr(const QString &str)
{
	QByteArray sz = KviQString::toUtf8(str);
	if(sz.length() > 0)
	{
		m_len = sz.length();
		m_ptr = (char *)kvi_malloc(m_len+1);
		kvi_fastmove(m_ptr,sz.data(),m_len+1);
	} else {
		m_ptr = (char *)kvi_malloc(1);
		*m_ptr = '\0';
		m_len = 0;
	}
}

KviStr::KviStr(char c,int fillLen)
{
	__range_valid(fillLen >= 0);
	m_len = fillLen;
	m_ptr = (char *)kvi_malloc(m_len+1);
	register char *p=m_ptr;
	while(fillLen--)*p++=c;
	*p='\0';
}


KviStr::KviStr(const kvi_wchar_t * unicode)
{
	if(!unicode)
	{
		m_len = 0;
		m_ptr = (char *)kvi_malloc(1);
		*m_ptr = 0;
	} else {
		m_len = kvi_wstrlen(unicode);
		m_ptr = (char *)kvi_malloc(m_len + 1);
		register char * p = m_ptr;
		while(*unicode)*p++ = *unicode++;
		*p = 0;
	}
}

KviStr::KviStr(const kvi_wchar_t * unicode,int len)
{
	m_len = len;
	m_ptr = (char *)kvi_malloc(m_len + 1);
	register char * p = m_ptr;
	char * end = p + len;
	while(p != end)
	{
		*p++ = *unicode++;
	}
	*p = 0;
}




KviStr::~KviStr()
{
	kvi_free(m_ptr);
}

void KviStr::setLength(int iLen)
{
	__range_valid(iLen >= 0);
	m_len = iLen;
	m_ptr = (char *)kvi_realloc(m_ptr,m_len+1);
	*(m_ptr + m_len) = '\0';
}

KviStr & KviStr::operator=(const KviStr &str)
{
	__range_valid(str.m_ptr);
	__range_valid(str.m_ptr != m_ptr);
	m_len = str.m_len;
	m_ptr = (char *)kvi_realloc(m_ptr,m_len+1);
	kvi_fastmove(m_ptr,str.m_ptr,m_len+1);
	return (*this);
}

KviStr & KviStr::operator=(const QByteArray &str)
{
	m_len = str.length();
	m_ptr = (char *)kvi_realloc(m_ptr,m_len+1);
	if(str.data())kvi_fastmove(m_ptr,str.data(),m_len+1);
	else *m_ptr = 0;
	return (*this);
}

KviStr & KviStr::operator=(const char *str)
{
	//__range_valid(str);
	if(str){
		m_len = (int)strlen(str);
		m_ptr = (char *)kvi_realloc(m_ptr,m_len+1);
		kvi_memmove(m_ptr,str,m_len+1);
	} else {
		m_ptr = (char *)kvi_realloc(m_ptr,1);
		*m_ptr = '\0';
		m_len = 0;
	}
	return (*this);
}

void KviStr::clear()
{
	m_ptr = (char *)kvi_realloc(m_ptr,1);
	*m_ptr = '\0';
	m_len = 0;
}


bool KviStr::hasNonWhiteSpaceData() const
{
	const char * aux = m_ptr;
	while(*aux)
	{
		if(((*aux) != ' ') && ((*aux) != '\t'))return true;
		aux++;
	}
	return false;
}

static char hexdigits[16] = { '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f' };

void KviStr::bufferToHex(const char *buffer,int len)
{
	__range_valid(buffer);
	m_len = (len * 2);
	m_ptr = (char *)kvi_realloc(m_ptr,m_len + 1);
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
	*(m_ptr+m_len) = '\0';
}



static char get_decimal_from_hex_digit_char(char dgt)
{
	if((dgt >= '0') && (dgt <= '9'))return (dgt - '0');
	if((dgt >= 'A') && (dgt <= 'F'))return (10 + (dgt - 'A'));
	if((dgt >= 'a') && (dgt <= 'f'))return (10 + (dgt - 'a'));
	return -1;
}

// This is just error-correcting...it treats non hex stuff as zeros
/*
static inline char get_decimal_from_hex_digit_char(char dgt)
{
	char c = pedantic_get_decimal_from_hex_digit(dgt);
	if(c == -1)return 0;
	return c;
}

int KviStr::hexToBuffer(char ** buffer,bool bNullToNewlines)
{
	int len;
	if(m_len % 2)len = (m_len / 2) + 1;
	else len = (m_len / 2);
	*buffer = (char *)kvi_malloc(len);

	char * ptr = *buffer;

	char * aux = m_ptr;
	while(*aux)
	{
		*ptr = get_decimal_from_hex_digit_char(*aux) * 16;
		aux++;
		if(*aux)
		{
			*ptr += get_decimal_from_hex_digit_char(*aux);
			aux++;
		}
		if(bNullToNewlines)if(!(*ptr))*ptr = '\n';
		ptr++;
	}
	return len;
}
*/

int KviStr::hexToBuffer(char ** buffer,bool bNullToNewlines)
{
	*buffer = 0;
	if((m_len == 0) || (m_len & 1))return -1; // this is an error
	int len = (m_len / 2);
	if(len < 1)return -1;
	*buffer = (char *)kvi_malloc(len);

	char * ptr = *buffer;
	char * aux = m_ptr;

	char aux2;

	while(*aux)
	{
		*ptr = get_decimal_from_hex_digit_char(*aux) * 16;
		if(*ptr == -1)
		{
			kvi_free(*buffer);
			*buffer = 0;
			return -1;
		}
		aux++;
		aux2 = get_decimal_from_hex_digit_char(*aux);
		if(aux2 == -1)
		{
			kvi_free(*buffer);
			*buffer = 0;
			return -1;
		}
		*ptr += aux2;
		aux++;
		if(bNullToNewlines)if(!(*ptr))*ptr = '\n';
		ptr++;
	}
	return len;
}

static const char * base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


void KviStr::bufferToBase64(const char * buffer,int len)
{
	m_len = (len / 3) << 2;
	if(len % 3)m_len += 4;

	m_ptr = (char *)kvi_realloc(m_ptr,m_len + 1);

	unsigned char aux1,aux2,aux3;
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
	if((base64 >= 'A') && (base64 <= 'Z'))return (base64 - 'A');
	if((base64 >= 'a') && (base64 <= 'z'))return ((base64 - 'a') + 26);
	if((base64 >= '0') && (base64 <= '9'))return ((base64 - '0') + 52);
	if(base64 == '+')return 62;
	if(base64 == '/')return 63;
	if(base64 == '=')return 64;
	return 65;
}


int KviStr::base64ToBuffer(char ** buffer,bool)
{
	*buffer = 0;
	if((m_len == 0) || (m_len & 3))return -1; // this is an error
	int len = (m_len >> 2) * 3;
	*buffer = (char *)kvi_malloc(len);

	char * auxBuf = *buffer;

	unsigned char aux1,aux2,aux3,aux4;
	char * aux_ptr = m_ptr;

	int newLen = len;

	while(*aux_ptr)
	{
		if(newLen != len)
		{
			// ops... there was a padding and we still have chars after it
			// this is an error
			kvi_free(*buffer);
			*buffer = 0;
			return -1;
		}
		aux1 = get_base64_idx(*aux_ptr++);
		aux2 = get_base64_idx(*aux_ptr++);
		aux3 = get_base64_idx(*aux_ptr++);
		aux4 = get_base64_idx(*aux_ptr++);
		if((aux3 > 64) || (aux4 > 64))
		{
			// error
			kvi_free(*buffer);
			*buffer = 0;
			return -1;
		}
		if((aux1 | aux2) > 63)
		{
			// again error...impossible padding
			kvi_free(*buffer);
			*buffer = 0;
			return -1;
		}
		if(aux4 == 64)
		{
			if(aux3 == 64)
			{
				// Double padding, only one digit here
				*auxBuf++ = (char)((aux1 << 2) | (aux2 >> 4));
				newLen -= 2;
			} else {
				// Single padding, two digits here
				*auxBuf++ = (char)((aux1 << 2) | (aux2 >> 4)); // >> 4 is a shr , not a ror! :)
				*auxBuf++ = (char)((aux2 << 4) | (aux3 >> 2));
				newLen -= 1;
			}
		} else {
			if(aux3 == 64)
			{
				// error... impossible padding
				kvi_free(*buffer);
				*buffer = 0;
				return -1;
			} else {
				// Ok , no padding, three digits here
				*auxBuf++ = (char)((aux1 << 2) | (aux2 >> 4));
				*auxBuf++ = (char)((aux2 << 4) | (aux3 >> 2));
				*auxBuf++ = (char)((aux3 << 6) | aux4);
			}
		}
	}

	if(newLen != len)*buffer = (char *)kvi_realloc(*buffer,newLen);
	return newLen;
}

KviStr & KviStr::setStr(const char *str,int len)
{
	if(!str)
	{
		clear();
		return *this;
	}
	int alen = (int)strlen(str);
	if((len < 0) || (len > alen))m_len = alen;
	else m_len = len;
	m_ptr = (char *)kvi_realloc(m_ptr,m_len+1);
	kvi_memmove(m_ptr,str,m_len);
	*(m_ptr+m_len) = '\0';
	return (*this);
}

KviStr & KviStr::operator=(const QString &str)
{
	QByteArray sz = KviQString::toUtf8(str);
	if(sz.length() > 0){
		m_len = sz.length();
		m_ptr = (char *)kvi_realloc(m_ptr,m_len+1);
		kvi_fastmove(m_ptr,sz.data(),m_len+1);
	} else {
		m_ptr = (char *)kvi_realloc(m_ptr,1);
		*m_ptr = '\0';
		m_len = 0;
	}
	return (*this);
}

KviStr & KviStr::operator=(char c)
{
	m_len = 1;
	m_ptr = (char *)kvi_realloc(m_ptr,2);
	*m_ptr = c;
	*(m_ptr+1)='\0';
	return (*this);
}

void KviStr::append(char c)
{
	m_ptr = (char *)kvi_realloc(m_ptr,m_len+2);
	*(m_ptr+m_len)=c;
	m_len++;
	*(m_ptr+m_len)='\0';
}

void KviStr::append(const KviStr &str)
{
	__range_valid(str.m_ptr);
	m_ptr = (char *)kvi_realloc(m_ptr,m_len+str.m_len+1);
	kvi_fastmove((m_ptr+m_len),str.m_ptr,str.m_len+1);
	m_len += str.m_len;
}

void KviStr::append(const char *str)
{
	if(!str)return;
	int len = (int)strlen(str);
	m_ptr = (char *)kvi_realloc(m_ptr,m_len+len+1);
	kvi_fastmove((m_ptr+m_len),str,len+1);
	m_len += len;
}

void KviStr::append(const QString &str)
{
	QByteArray sz = KviQString::toUtf8(str);
	if(sz.length() < 1)return;
	m_ptr = (char *)kvi_realloc(m_ptr,m_len+sz.length()+1);
	kvi_fastmove((m_ptr+m_len),sz.data(),sz.length()+1);
	m_len += sz.length();
}

void KviStr::append(const char *str,int len)
{
	__range_valid(str);
//	__range_valid(len <= ((int)strlen(str)));
	__range_valid(len >= 0);
	m_ptr = (char *)kvi_realloc(m_ptr,m_len+len+1);
	kvi_fastmove((m_ptr+m_len),str,len);
	m_len += len;
	*(m_ptr + m_len)='\0';
}

void KviStr::append(KviFormatConstructorTag ,const char *fmt,...)
{
	int auxLen;
	m_ptr=(char *)kvi_realloc(m_ptr,m_len + 256);
	//First try
	kvi_va_list list;
	kvi_va_start(list,fmt);
	//print...with max 256 chars
	auxLen =kvi_vsnprintf(m_ptr + m_len,256,fmt,list);
	kvi_va_end(list);

	//check if we failed
	if(auxLen < 0){
		//yes , failed....
		int dummy=256;
		do{ //we failed , so retry with 256 more chars
			dummy+=256;
			//realloc
			m_ptr=(char *)kvi_realloc(m_ptr,m_len + dummy);
			//print...
			kvi_va_start(list,fmt);
			auxLen=kvi_vsnprintf(m_ptr + m_len,dummy,fmt,list);
			kvi_va_end(list);
		} while(auxLen < 0);
	}
	m_len += auxLen;
	//done...
	//now m_len is the length of the written string not including the terminator...
	//perfect! :)
	m_ptr=(char *)kvi_realloc(m_ptr,m_len+1);
}

void KviStr::extractFromString(const char *begin,const char *end)
{
	__range_valid(begin);
	__range_valid(end);
	__range_valid(end >= begin);
	m_len = end-begin;
	m_ptr = (char *)kvi_realloc(m_ptr,m_len+1);
	kvi_fastmove(m_ptr,begin,m_len);
	*(m_ptr + m_len)='\0';
}

void KviStr::prepend(const KviStr &str)
{
	__range_valid(str.m_ptr);
	__range_valid(str.m_ptr != m_ptr);
	m_ptr = (char *)kvi_realloc(m_ptr,m_len+str.m_len+1);
	kvi_memmove((m_ptr+str.m_len),m_ptr,m_len+1); //move self
	kvi_fastmove(m_ptr,str.m_ptr,str.m_len);
	m_len += str.m_len;
}

void KviStr::prepend(const char *str)
{
	if(!str)return;
	int len = (int)strlen(str);
	m_ptr = (char *)kvi_realloc(m_ptr,m_len+len+1);
	kvi_memmove((m_ptr+len),m_ptr,m_len+1); //move self
	kvi_fastmove(m_ptr,str,len);
	m_len += len;
}

void KviStr::prepend(const char *str,int len)
{
	__range_valid(str);
	__range_valid(len <= ((int)strlen(str)));
	__range_valid(len >= 0);
	m_ptr = (char *)kvi_realloc(m_ptr,m_len+len+1);
	kvi_memmove((m_ptr+len),m_ptr,m_len+1); //move self
	kvi_fastmove(m_ptr,str,len);
	m_len += len;
}

unsigned char iso88591_toUpper_map[256]=
{
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

void KviStr::toUpperISO88591()
{
	register char *p=m_ptr;
	while(*p)
	{
		*p=(char)iso88591_toUpper_map[(unsigned char)*p];
		p++;
	}
}

void KviStr::toUpper()
{
	register char *p=m_ptr;
	while(*p)
	{
		*p=toupper(*p);
		p++;
	}
}

unsigned char iso88591_toLower_map[256]=
{
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

void KviStr::toLowerISO88591()
{
	register char *p=m_ptr;
	while(*p)
	{
		*p=(char)iso88591_toLower_map[(unsigned char)*p];
		p++;
	}
}


void KviStr::toLower()
{
	register char *p=m_ptr;
	while(*p)
	{
		*p=tolower(*p);
		p++;
	}
}

KviStr KviStr::upper() const
{
	KviStr tmp(*this);
	tmp.toUpper();
	return tmp;
}

KviStr KviStr::upperISO88591() const
{
	KviStr tmp(*this);
	tmp.toUpperISO88591();
	return tmp;
}

KviStr KviStr::lower() const
{
	KviStr tmp(*this);
	tmp.toLower();
	return tmp;
}

KviStr KviStr::lowerISO88591() const
{
	KviStr tmp(*this);
	tmp.toLowerISO88591();
	return tmp;
}

KviStr KviStr::left(int maxLen) const
{
	if(maxLen <= 0)
	{
		KviStr empty;
		return empty;
	}
	if(maxLen > m_len)maxLen=m_len;
	KviStr str(m_ptr,maxLen);
	return str;
}

KviStr KviStr::right(int maxLen) const
{
	if(maxLen <= 0)
	{
		KviStr empty;
		return empty;
	}
	if(maxLen > m_len)maxLen=m_len;
	KviStr str((m_ptr+(m_len-maxLen)),maxLen);
	return str;
}

KviStr KviStr::middle(int idx,int maxLen) const
{
	__range_valid(maxLen >= 0);
	__range_valid(idx >= 0);
	if((maxLen <= 0) || (idx < 0)){ //max len negative...invalid params
		KviStr ret;
		return ret;
	}
	if((maxLen + idx) <= m_len){ //valid params
		KviStr str(m_ptr+idx,maxLen);
		return str;
	}
	if(idx < m_len){ //string shorter than requested
		KviStr str(m_ptr+idx);
		return str;
	}
	// idx out of bounds
	KviStr ret;
	return ret;
}

KviStr ** KviStr::splitToArray(char sep,int max,int * realCount) const
{
	KviStr ** strings = (KviStr **)kvi_malloc(sizeof(KviStr *));
	int number = 0;
	char * ptr = m_ptr;
	char * last = ptr;
	while((max > 0) && *ptr)
	{
		strings = (KviStr **)kvi_realloc((void *)strings,sizeof(KviStr *) * (number + 2));
		if(max > 1)
		{
			while(*ptr && (*ptr != sep))ptr++;
			strings[number] = new KviStr(last,ptr - last);
		} else {
			strings[number] = new KviStr(ptr);
		}
		number++;
		max--;
		if(*ptr)
		{
			ptr++;
			last = ptr;
		}
	}
	if(realCount)*realCount = number;
	strings[number] = 0;
	return strings;
}
/*
	WORKING BUT UNUSED

KviStr ** KviStr::splitToArray(const char * sep,int max,int * realCount) const
{
	KviStr ** strings = (KviStr **)kvi_malloc(sizeof(KviStr *));
	KviStr tmp = *this;
	int idx = tmp.findFirstIdx(sep);
	int number = 0;
	int seplen = kvi_strLen(sep);


	while(idx != -1)
	{
		strings = (KviStr **)kvi_realloc(sizeof(KviStr *) * (number + 2));
		strings[number] = new KviStr(tmp.ptr(),idx);
		tmp.cutLeft(idx + seplen);
		number++;
		idx = tmp.findFirstIdx(sep);
	}

	if(tmp.hasData())
	{
		strings = (KviStr **)kvi_realloc(sizeof(KviStr *) * (number + 2));
		strings[number] = new KviStr(tmp);
		number++;
	}

	if(realCount)*realCount = number;
	strings[number] = 0;
	return strings;
}
*/
void KviStr::freeArray(KviStr ** strings)
{
	if(!strings)return;
	KviStr ** aux = strings;
	while(*aux)
	{
		delete (*aux); // delete (KviStr *)
		aux++;
	}
	kvi_free(strings);
}

void KviStr::freeBuffer(char * buffer)
{
	if(!buffer)return;
	kvi_free(buffer);
}

void KviStr::joinFromArray(KviStr ** strings,const char * sep,bool bLastSep)
{
	setLen(0);
	if(!strings)return;

	while(*strings)
	{
		append(*(*strings));
		strings++;
		if(*strings)
		{
			if(sep)append(sep);
		} else {
			if(sep && bLastSep)append(sep);
		}
	}
}

KviStr & KviStr::insert(int idx,const char *data)
{
	__range_valid(data);
	if(idx <= m_len){
		int len = (int)strlen(data);
		m_ptr = (char *)kvi_realloc(m_ptr,m_len+len+1);
		kvi_memmove(m_ptr+idx+len,m_ptr+idx,(m_len - idx)+1);
		kvi_fastmove(m_ptr+idx,data,len);
		m_len+=len;
	}
	return (*this);
}

KviStr & KviStr::insert(int idx,char c)
{
	if(idx <= m_len){
		m_ptr = (char *)kvi_realloc(m_ptr,m_len+2);
		kvi_memmove(m_ptr+idx+1,m_ptr+idx,(m_len - idx)+1);
		m_len++;
		*(m_ptr + idx) = c;
	}
	return (*this);
}

// FIXME: #warning "Double check the following two functions !!!"

KviStr & KviStr::hexEncodeWithTable(const unsigned char table[256])
{
	char * aux = m_ptr;
	char * begin = m_ptr;

	char * n  = 0;
	int curSize = 0;

	while(*aux)
	{
		if(table[*((unsigned char *)aux)] || (*aux == '%'))
		{
			int len = aux - begin;
			n = (char *)kvi_realloc(n,curSize + len + 3);
			kvi_memmove(n + curSize,begin,len);
			curSize += len;

			n[curSize] = '%';
			curSize++;
			n[curSize] = hexdigits[(unsigned int)(((unsigned char)(*aux)) / 16)];
			curSize++;
			n[curSize] = hexdigits[(unsigned int)(((unsigned char)(*aux)) % 16)];
			curSize++;

			aux++;
			begin = aux;

		} else aux++;
	}

	int len = aux - begin;
	n = (char *)kvi_realloc(n,curSize + len + 1);
	kvi_memmove(n + curSize,begin,len);
	curSize += len;

	n[curSize] = '\0';

	kvi_free((void *)m_ptr);
	m_ptr = n;
	m_len = curSize;

	return (*this);
}

KviStr & KviStr::hexEncodeWhiteSpace()
{
	static unsigned char ascii_jump_table[256]=
	{
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
	};

	return hexEncodeWithTable(ascii_jump_table);
}

KviStr & KviStr::hexDecode(const char * pFrom)
{
	// WARNING: pFrom can be also m_ptr here!
	const char * aux = pFrom;
	const char * begin = pFrom;

	char * n  = 0;
	int curSize = 0;

	while(*aux)
	{
		if(*aux == '%')
		{
			// move last block
			int len = aux - begin;
			n = (char *)kvi_realloc(n,curSize + len + 1);
			kvi_memmove(n + curSize,begin,len);
			curSize += len;

			// get the hex code
			aux++;

			char theChar = get_decimal_from_hex_digit_char(*aux);
			if(theChar < 0)
			{
				n[curSize] = '%'; // wrong code...just a '%'
				curSize++;
			} else {
				aux++;
				char theChar2 = get_decimal_from_hex_digit_char(*aux);
				if(theChar2 < 0)
				{
					// wrong code...just a '%' and step back
					n[curSize] = '%';
					curSize++;
					aux--;
				} else {
					n[curSize] = (theChar * 16) + theChar2;
					curSize++;
					aux++;
				}
			}

			begin = aux;

		} else aux++;
	}

	int len = aux - begin;
	n = (char *)kvi_realloc(n,curSize + len + 2);
	kvi_memmove(n + curSize,begin,len);
	curSize += len;
	n[curSize] = '\0';

	kvi_free((void *)m_ptr);
	m_ptr = n;
	m_len = curSize;

	return (*this);
}

KviStr & KviStr::replaceAll(const char c,const char *str)
{
	int idx = findFirstIdx(c);
	KviStr tmp;
	while(idx >= 0){
		if(idx > 0)tmp += left(idx);
		cutLeft(idx+1);
		tmp.append(str);
		idx = findFirstIdx(c);
	}
	tmp.append(*this);
	// Now copy
	m_len = tmp.m_len;
	m_ptr = (char *)kvi_realloc(m_ptr,m_len+1);
	kvi_fastmove(m_ptr,tmp.m_ptr,m_len+1);
	return (*this);
}

KviStr & KviStr::replaceAll(const char *toFind,const char *str,bool bCaseS)
{
	int len = (int)strlen(toFind);
	int idx = findFirstIdx(toFind,bCaseS);
	KviStr tmp;
	while(idx >= 0)
	{
		if(idx > 0)tmp += left(idx);
		cutLeft(idx+len);
		tmp.append(str);
		idx = findFirstIdx(toFind,bCaseS);
	}
	tmp.append(*this);
	// Now copy
	m_len = tmp.m_len;
	m_ptr = (char *)kvi_realloc(m_ptr,m_len+1);
	kvi_fastmove(m_ptr,tmp.m_ptr,m_len+1);
	return (*this);
}

KviStr & KviStr::transliterate(const char * szToFind,const char * szReplacement)
{
	while(*szToFind && *szReplacement)
	{
		char * p = m_ptr;
		while(*p)
		{
			if(*p == *szToFind)*p = *szReplacement;
			++p;
		}
		++szToFind;
		++szReplacement;
	}
	return (*this);
}


int KviStr::occurences(char c,bool caseS) const
{
	register char *p = m_ptr;
	int cnt=0;
	if(caseS){
		while(*p){
			if(*p == c)cnt++;
			p++;
		}
	} else {
		char b=tolower(c);
		while(*p){
			if(tolower(*p) == b)cnt++;
			p++;
		}
	}
	return cnt;
}

int KviStr::occurences(const char *str,bool caseS) const
{
	__range_valid(str);
	register char *p = m_ptr;
	int cnt=0;
	int len = (int)strlen(str);
	if(caseS){
		while(*p){
			if(*p == *str){
				if(kvi_strEqualCSN(p,str,len))cnt++;
			}
			p++;
		}
	} else {
		while(*p){
			char c = tolower(*str);
			if(tolower(*p) == c){
				if(kvi_strEqualCIN(p,str,len))cnt++;
			}
			p++;
		}
	}
	return cnt;
}

bool KviStr::contains(char c,bool caseS) const
{
	register char *p = m_ptr;
	if(caseS)
	{
		while(*p)
		{
			if(*p == c)return true;
			p++;
		}
	} else {
		char b=tolower(c);
		while(*p)
		{
			if(tolower(*p) == b)return true;
			p++;
		}
	}
	return false;
}

bool KviStr::contains(const char *str,bool caseS) const
{
	__range_valid(str);
	register char *p = m_ptr;
	int len = (int)strlen(str);
	if(caseS)
	{
		while(*p)
		{
			if(*p == *str)
			{
				if(kvi_strEqualCSN(p,str,len))return true;
			}
			p++;
		}
	} else {
		while(*p)
		{
			char c = tolower(*str);
			if(tolower(*p) == c)
			{
				if(kvi_strEqualCIN(p,str,len))return true;
			}
			p++;
		}
	}
	return false;
}


KviStr & KviStr::setNum(long num)
{
	char numberBuffer[30];
	bool bNegative = false;
	long tmp;
	register char *p;
	register char *pNumBuf = numberBuffer;

	// somebody can explain me why 	-(-2147483648) = -2147483648 ? (2^31)
	// it is like signed char x = 128 ---> 10000000 that is signed -0 (!?)
	// mmmmh...or it is assumed to be -128 (a number rappresentation exception)
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

	if(num < 0){ //negative integer
		bNegative = true;
		num = -num; //need to have it positive
		if(num < 0){ // 2^31 exception
			// We need to avoid absurd responses like ".(./),." :)
			num = 0; // we get a negative zero here...it is still an exception
		}
	}

	//write the number in a temporary buffer (at least '0')
	do {
		tmp = num / 10;
		*pNumBuf++ = num - (tmp * 10) + '0';
	} while((num = tmp));

	//copy now....
	m_len = pNumBuf - numberBuffer; //length of the number string
	if(bNegative){
		m_len++;
		m_ptr = (char *)kvi_realloc(m_ptr,m_len+1);
		p=m_ptr;
		*p++='-';
	} else {
		m_ptr = (char *)kvi_realloc(m_ptr,m_len+1);
		p=m_ptr;
	}
	do { *p++ = *--pNumBuf; } while(pNumBuf != numberBuffer);
	*(m_ptr + m_len)='\0';
	return (*this);
}

KviStr & KviStr::setNum(unsigned long num)
{
	char numberBuffer[30];
	unsigned long tmp;
	register char *p;
	register char *pNumBuf = numberBuffer;

	//write the number in a temporary buffer (at least '0')
	do {
		tmp = num / 10;
		*pNumBuf++ = num - (tmp * 10) + '0';
	} while((num = tmp));

	//copy now....
	m_len = pNumBuf - numberBuffer; //length of the number string
	m_ptr = (char *)kvi_realloc(m_ptr,m_len+1);
	p=m_ptr;
	do { *p++ = *--pNumBuf; } while(pNumBuf != numberBuffer);
	*(m_ptr + m_len)='\0';
	return (*this);
}

long long KviStr::toLongLong(bool *bOk) const
{
	long long result = 0;
	if(bOk)*bOk = false;
	register char *p=m_ptr;
	bool bNeg = false;
	while(isspace(*p))p++; //skip spaces
	if(*p == '-'){
		bNeg = true;
		p++;
	} else {
		if(*p == '+')p++;
	}
	if(isdigit(*p)){                      //point to something interesting ?
		do{
			result = (result * 10) + (*p - '0');
			p++;
		} while(isdigit(*p));
		if(bNeg)result = -result;
		while(isspace(*p))p++;        //skip trailing spaces
		if(*p)return 0;               //if this is not the end...die.
		if(bOk)*bOk = true;
		return result;
	}
	return 0;
}

unsigned long long KviStr::toULongLong(bool *bOk) const
{
	unsigned long long result = 0;
	if(bOk)*bOk = false;
	register char *p=m_ptr;
	while(isspace(*p))p++; //skip spaces
	if(isdigit(*p)){                      //point to something interesting ?
		do{
			result = (result * 10) + (*p - '0');
			p++;
		} while(isdigit(*p));
		while(isspace(*p))p++;        //skip trailing spaces
		if(*p)return 0;               //if this is not the end...die.
		if(bOk)*bOk = true;
		return result;
	}
	return 0;
}

long KviStr::toLong(bool *bOk) const
{
	long result = 0;
	if(bOk)*bOk = false;
	register char *p=m_ptr;
	bool bNeg = false;
	while(isspace(*p))p++; //skip spaces
	if(*p == '-'){
		bNeg = true;
		p++;
	} else {
		if(*p == '+')p++;
	}
	if(isdigit(*p)){                      //point to something interesting ?
		do{
			result = (result * 10) + (*p - '0');
			p++;
		} while(isdigit(*p));
		if(bNeg)result = -result;
		while(isspace(*p))p++;        //skip trailing spaces
		if(*p)return 0;               //if this is not the end...die.
		if(bOk)*bOk = true;
		return result;
	}
	return 0;
}

unsigned long KviStr::toULong(bool *bOk) const
{
	unsigned long result = 0;
	if(bOk)*bOk = false;
	register char *p=m_ptr;
	while(isspace(*p))p++; //skip spaces
	if(isdigit(*p)){                      //point to something interesting ?
		do{
			result = (result * 10) + (*p - '0');
			p++;
		} while(isdigit(*p));
		while(isspace(*p))p++;        //skip trailing spaces
		if(*p)return 0;               //if this is not the end...die.
		if(bOk)*bOk = true;
		return result;
	}
	return 0;
}

long KviStr::toLongExt(bool *bOk,int base)
{
	if(m_len == 0){
		if(bOk)*bOk = false;
		return 0;
	}
	char * endptr;
	long result = strtol(m_ptr,&endptr,base);
	if(*endptr){
		// must be whitespaces , otherwise there is trailing garbage inside
		while(isspace(*endptr) && (*endptr))endptr++;
		if(*endptr){
			// still not at the end
			// trailing garbage not allowed
			if(bOk)*bOk = false;
			return result;
		}
	}
	if(bOk)*bOk = true;
	return result;
}

//
//working code , but unused in kvirc
//
//unsigned long KviStr::toULongExt(bool *bOk = 0,int base = 0)
//{
//	if(m_len == 0){
//		if(bOk)*bOk = false;
//		return 0;
//	}
//	char * endptr;
//	unsigned long result = strtoul(m_ptr,&endptr,base);
//	if(*endptr != '\0'){
//		if(bOk)*bOk = false;
//	}
//	return result;
//}

KviStr & KviStr::cutLeft(int len)
{
	__range_valid(len >= 0);
	if(len <= m_len){
		m_len -= len;
		kvi_memmove(m_ptr,m_ptr+len,m_len+1);
		m_ptr = (char *)kvi_realloc(m_ptr,m_len+1);
	} else {
		m_ptr = (char *)kvi_realloc(m_ptr,1);
		*m_ptr = '\0';
		m_len = 0;
	}
	return (*this);
}

KviStr & KviStr::cutRight(int len)
{
	__range_valid(len >= 0);
	if(len <= m_len){
		m_len -= len;
		m_ptr = (char *)kvi_realloc(m_ptr,m_len+1);
		*(m_ptr +m_len)='\0';
	} else {
		m_ptr = (char *)kvi_realloc(m_ptr,1);
		*m_ptr = '\0';
		m_len = 0;
	}
	return (*this);
}

KviStr & KviStr::cut(int idx,int len)
{
	__range_valid(idx >= 0);
	__range_valid(len >= 0);
	if(idx < m_len){
		// idx = 3 len = 3 m_len = 10
		// 0123456789
		// abcdefghij
		//    ^  ^
		//   p1  p2
		char * p1 = m_ptr+idx;
		if(len + idx > m_len)len = m_len - idx;
		char * p2 = p1+len;
		kvi_memmove(p1,p2,(m_len - (len+idx)) +1);
		m_len -= len;
		m_ptr = (char *)kvi_realloc(m_ptr,m_len+1);
	}
	return (*this);
}

KviStr & KviStr::cutToFirst(char c,bool bIncluded)
{
	int idx = findFirstIdx(c);
	if(idx != -1)cutLeft(bIncluded ? idx + 1 : idx);
	return (*this);
}

KviStr KviStr::leftToFirst(char c,bool bIncluded) const
{
	int idx = findFirstIdx(c);
	if(idx == -1)return KviStr(*this);
	return KviStr(m_ptr,bIncluded ? idx + 1 : idx);
}


KviStr KviStr::leftToLast(char c,bool bIncluded) const
{
	int idx = findLastIdx(c);
	return KviStr(m_ptr,bIncluded ? idx + 1 : idx);
}

KviStr & KviStr::cutFromFirst(char c,bool bIncluded)
{
	int idx = findFirstIdx(c);
	if(idx != -1)cutRight(bIncluded ? (m_len - idx) : (m_len - (idx + 1)));
	return (*this);
}

KviStr & KviStr::cutToLast(char c,bool bIncluded)
{
	int idx = findLastIdx(c);
	if(idx != -1)cutLeft(bIncluded ? idx + 1 : idx);
	return (*this);
}

KviStr & KviStr::cutFromLast(char c,bool bIncluded)
{
	int idx = findLastIdx(c);
	if(idx != -1)cutRight(bIncluded ? (m_len - idx) : (m_len - (idx + 1)));
	return (*this);
}

KviStr & KviStr::cutToFirst(const char *c,bool bIncluded)
{
	int len = (int)strlen(c);
	int idx = findFirstIdx(c);
	if(idx != -1)cutLeft(bIncluded ? idx + len : idx);
	return (*this);
}

KviStr & KviStr::cutFromFirst(const char *c,bool bIncluded)
{
	int len = (int)strlen(c);
	int idx = findFirstIdx(c);
	if(idx != -1)cutRight(bIncluded ? (m_len - idx) : (m_len - (idx + len)));
	return (*this);
}

KviStr & KviStr::cutToLast(const char *c,bool bIncluded)
{
	int len = (int)strlen(c);
	int idx = findLastIdx(c);
	if(idx != -1)cutLeft(bIncluded ? idx + len : idx);
	return (*this);
}

KviStr & KviStr::cutFromLast(const char *c,bool bIncluded)
{
	int len = (int)strlen(c);
	int idx = findLastIdx(c);
	if(idx != -1)cutRight(bIncluded ? (m_len - idx) : (m_len - (idx + len)));
	return (*this);
}

KviStr & KviStr::setLen(int len)
{
	__range_valid(len >= 0);
	m_ptr = (char *)kvi_realloc(m_ptr,len+1);
	*(m_ptr+len)='\0';
	m_len = len;
	return (*this);
}

KviStr & KviStr::stripLeftWhiteSpace()
{
	register char *p=m_ptr;
	while(isspace(*p))p++;
	m_len -= (p-m_ptr);
	kvi_memmove(m_ptr,p,m_len+1);
	m_ptr = (char *)kvi_realloc(m_ptr,m_len+1);
	return (*this);
}

KviStr & KviStr::stripLeft(char c)
{
	__range_valid(c != '\0');
	register char *p=m_ptr;
	while(*p == c)p++;
	m_len -= (p-m_ptr);
	kvi_memmove(m_ptr,p,m_len+1);
	m_ptr = (char *)kvi_realloc(m_ptr,m_len+1);
	return (*this);
}

bool KviStr::getToken(KviStr & str,char sep)
{
	__range_valid(str.m_ptr);
	__range_valid(str.m_ptr != m_ptr);
	register char *p=m_ptr;
	//skip to the end
	while(*p && (*p != sep))p++;
	//0123456789
	//abcd xyz
	//^   ^
	str.m_len = p-m_ptr;
	str.m_ptr = (char *)kvi_realloc(str.m_ptr,str.m_len+1);
	kvi_fastmove(str.m_ptr,m_ptr,str.m_len);
	*(str.m_ptr + str.m_len)='\0';
	while(*p && (*p == sep))p++;
	cutLeft(p-m_ptr);
	return (m_len != 0);
}

bool KviStr::getLine(KviStr &str)
{
	__range_valid(str.m_ptr);
	__range_valid(str.m_ptr != m_ptr);
	if(m_len == 0)return false;
	register char *p=m_ptr;
	//skip to the end
	while(*p && (*p != '\n'))p++;
	//0123456789
	//abcd xyz
	//^   ^
	str.m_len = p-m_ptr;
	str.m_ptr = (char *)kvi_realloc(str.m_ptr,str.m_len+1);
	kvi_fastmove(str.m_ptr,m_ptr,str.m_len);
	*(str.m_ptr + str.m_len)='\0';
	p++;
	cutLeft(p-m_ptr);
	return true;
}

KviStr KviStr::getToken(char sep)
{
	register char *p=m_ptr;
	while(*p && (*p != sep))p++;
	KviStr ret(m_ptr,p);
	while(*p && (*p == sep))p++;
	cutLeft(p-m_ptr);
	return ret;
}

KviStr & KviStr::sprintf(const char *fmt,...)
{
	m_ptr=(char *)kvi_realloc(m_ptr,256);
	//First try
	kvi_va_list list;
	kvi_va_start(list,fmt);
	//print...with max 256 chars
	m_len=kvi_vsnprintf(m_ptr,256,fmt,list);
	kvi_va_end(list);

	//check if we failed
	if(m_len < 0){
		//yes , failed....
		int dummy=256;
		do{ //we failed , so retry with 256 more chars
			dummy+=256;
			//realloc
			m_ptr=(char *)kvi_realloc(m_ptr,dummy);
			//print...
			kvi_va_start(list,fmt);
			m_len=kvi_vsnprintf(m_ptr,dummy,fmt,list);
			kvi_va_end(list);
		} while(m_len < 0);
	}
	//done...
	//now m_len is the length of the written string not including the terminator...
	//perfect! :)
	m_ptr=(char *)kvi_realloc(m_ptr,m_len+1);
	return (*this);
}

int KviStr::find(const char *str,int idx,bool caseS) const
{
	if(idx >= m_len)return -1;
	register char *p=m_ptr + idx;
	int len = (int)strlen(str);
	if(caseS){
		for(;;){
			while(*p && (*p != *str))p++;
			if(*p){
				if(kvi_strEqualCSN(str,p,len))return (p-m_ptr);
				else p++;
			} else return -1;
		}
	} else {
		for(;;){
			char tmp = toupper(*str);
			while(*p && (toupper(*p) != tmp))p++;
			if(*p){
				if(kvi_strEqualCIN(str,p,len))return (p-m_ptr);
				else p++;
			} else return -1;
		}
	}
}

int KviStr::find(char c,int idx) const
{
	if(idx >= m_len)return -1;
	register char *p=m_ptr + idx;
	while(*p && (*p != c))p++;
	return (*p ? p-m_ptr : -1);
}


int KviStr::findRev(const char *str,int idx,bool caseS) const
{
	if((m_len + idx) < 0)return -1;
	register char *p=m_ptr + m_len + idx;
	int len = (int)strlen(str);
	if(caseS)
	{
		for(;;)
		{
			while((p >= m_ptr) && (*p != *str))p--;
			if(p >= m_ptr){
				if(kvi_strEqualCSN(str,p,len))return (p-m_ptr);
				else p--;
			} else return -1;
		}
	} else {
		for(;;){
			char tmp = toupper(*str);
			while((p >= m_ptr) && (toupper(*p) != tmp))p--;
			if(p >= m_ptr){
				if(kvi_strEqualCIN(str,p,len))return (p-m_ptr);
				else p--;
			} else return -1;
		}
	}
}

int KviStr::findFirstIdx(char c) const
{
	register char *p=m_ptr;
	while(*p && (*p != c))p++;
	return (*p ? p-m_ptr : -1);
}

int KviStr::findFirstIdx(const char *str,bool caseS) const
{
	// This function can't be used to search inside
	// multibyte encoded strings... convert your
	// code to QString and use QString::findRev().
	// We must throw away KviStr at all in this case...

	// return QString(m_ptr).find(QString(str),0,caseS);;

	// Both this KviStr and the const char * str are assumed
	// to be in the proper (and same) encoding.
	// If KviStr is in encoding A then QString(m_ptr) might
	// or not be decoded correctly.
	// Also if KviStr is in UTF-8 (for example), then
	// a position in QString() does not map to the position in the char array
	// since a single UNICODE char may use one or more bytes...

	__range_valid(str);
	register char *p=m_ptr;
	int len = (int)strlen(str);
	if(caseS){
		for(;;){
			while(*p && (*p != *str))p++;
			if(*p){
				if(kvi_strEqualCSN(str,p,len))return (p-m_ptr);
				else p++;
			} else return -1;
		}
	} else {
		// this will NOT work for strings that aren't in the current system encoding :(
		for(;;){
			char tmp = toupper(*str);
			while(*p && (toupper(*p) != tmp))p++;
			if(*p){
				if(kvi_strEqualCIN(str,p,len))return (p-m_ptr);
				else p++;
			} else return -1;
		}
	}
}

int KviStr::findLastIdx(char c) const
{
	//Empty string ?
	if(m_len < 1)return -1;
	//p points to the last character in the string
	register char *p=((m_ptr+m_len)-1);
	//go back until we find a match or we run to the first char in the string.
	while((*p != c) && (p > m_ptr))p--;
	//if *p == c --> matched , else we are at the beginning of the string.
	return ((*p == c)? p-m_ptr : -1);
}

int KviStr::findLastIdx(const char *str,bool caseS) const
{
	// This function can't be used to search inside
	// multibyte encoded strings... convert your
	// code to QString and use QString::findRev().
	// We must throw away KviStr at all in this case...

	// return QString(m_ptr).findRev(QString(str),-1,caseS);

	__range_valid(str);
	//Calc the len of the searched string
	int len = (int)strlen(str);
	//Too long ?
	if(m_len < len)return -1;
	//p points to the last character in the string
	register char *p=((m_ptr+m_len)-1);
	if(caseS){
		for(;;){
			//go back until we find a character that mathes or we run to the first char.
			while((*p != *str) && (p > m_ptr))p--;
			if(*p == *str){
				//maybe occurence....
				if(kvi_strEqualCSN(str,p,len))return (p-m_ptr);
				else {
					//Nope...continue if there is more data to check...
					if(p == m_ptr)return -1;
					p--;
				}
			} else return -1; //Beginning of the string
		}
	} else {
		// case insensitive
		for(;;){
			//go back until we find a character that mathes or we run to the first char.
			char tmp = toupper(*str);
			while((toupper(*p) != tmp) && (p > m_ptr))p--;
			if(toupper(*p) == tmp){
				//maybe occurence....
				if(kvi_strEqualCIN(str,p,len))return (p-m_ptr);
				else {
					//Nope...continue if there is more data to check...
					if(p == m_ptr)return -1;
					p--;
				}
			} else return -1; //Beginning of the string
		}
	}
}

KviStr & KviStr::trimmed()
{
	// 0123456789
	//    abcd   0
	// ^        ^
	// left   right
	register char *left=m_ptr;
	register char *right=m_ptr+m_len-1;
	// skip initial spaces
	while(isspace(*left))left++;
	if(*left){
		// valid string , left points to first non-space
		while((right >= left) && isspace(*right))right--;
		// 0123456789
		//    abcd   0
		//    ^  ^
		// left   right
		m_len = (right - left)+1;
		kvi_memmove(m_ptr,left,m_len);
		m_ptr = (char *)kvi_realloc(m_ptr,m_len+1);
		*(m_ptr+m_len)='\0';
	} else {
		m_ptr = (char *)kvi_realloc(m_ptr,1);
		*m_ptr = '\0';
		m_len = 0;
	}
	return (*this);
}

KviStr & KviStr::stripRightWhiteSpace()
{
	if(*m_ptr)
	{
		register char *right=m_ptr+m_len-1;
		const char *start=right;
		//isspace accepts 0..255 values in MSVC
		while((right >= m_ptr) && ((unsigned)(*right +1) <= 256) && isspace( *right ))right--;
		if(right != start)
		{
			m_len = (right - m_ptr) + 1;
			m_ptr = (char *)kvi_realloc(m_ptr,m_len+1);
			*(m_ptr+m_len)='\0';
		}
	}
	return (*this);
}

KviStr & KviStr::stripRight(char c)
{
	if(*m_ptr)
	{
		register char *right=m_ptr+m_len-1;
		const char *start=right;
		while((right >= m_ptr) && (*right == c))right--;
		if(right != start)
		{
			m_len = (right - m_ptr) + 1;
			m_ptr = (char *)kvi_realloc(m_ptr,m_len+1);
			*(m_ptr+m_len)='\0';
		}
	}
	return (*this);
}

KviStr & KviStr::stripSpace()
{
	// 0123456789
	//    abcd   0
	// ^        ^
	// left   right
	register char *left=m_ptr;
	register char *right=m_ptr+m_len-1;
	// skip initial spaces
	while((*left == ' ') || (*left == '\t'))left++;
	if(*left){
		// valid string , left points to first non-space
		while((right >= left) && ((*right == ' ') || (*right == '\t')))right--;
		// 0123456789
		//    abcd   0
		//    ^  ^
		// left   right
		m_len = (right - left)+1;
		kvi_memmove(m_ptr,left,m_len);
		m_ptr = (char *)kvi_realloc(m_ptr,m_len+1);
		*(m_ptr+m_len)='\0';
	} else {
		m_ptr = (char *)kvi_realloc(m_ptr,1);
		*m_ptr = '\0';
		m_len = 0;
	}
	return (*this);
}

bool KviStr::isNum() const
{
	register char *p=m_ptr;
	while(isspace(*p))p++;
	if(*p=='-')p++;
	if(!isdigit(*p))return false;
	while(isdigit(*p))p++;
	while(isspace(*p))p++;
	return (*p=='\0');
}

bool KviStr::isUnsignedNum() const
{
	register char *p=m_ptr;
	while(isspace(*p))p++;
	if(!isdigit(*p))return false;
	while(isdigit(*p))p++;
	while(isspace(*p))p++;
	return (*p=='\0');
}

static KviStr g_szApplicationWideEmptyString;

KviStr & KviStr::emptyString()
{
	return g_szApplicationWideEmptyString;
}


bool KviStr::ext_contains(register const char * data,const char * item,bool caseS)
{
	if(item && data)
	{
		int len = (int)strlen(item);
		char c = tolower(*item);
		if(caseS)
		{
			while(*data)
			{
				while(*data && (tolower(*data) != c))data++;
				if(*data)
				{
					if(kvi_strEqualCSN(item,data,len))return true;
					else data++;
				}
			}
		} else {
			while(*data)
			{
				while(*data && (tolower(*data) != c))data++;
				if(*data)
				{
					if(kvi_strEqualCIN(item,data,len))return true;
					else data++;
				}
			}
		}
	}
	return false;
}


//void KviStr::pointerToBitString(const void * ptr)
//{
//	m_len = (sizeof(void *) * 8);
//	m_ptr = kvi_realloc(m_ptr,m_len + 1);
//	for(int i=0;i < m_len;i++)
//	{
//		m_ptr[i] = (ptr & 1) ? '1' : '0';
//		ptr >> 1;
//	}
//	m_ptr[i] = '\0';
//}
//
//void * KviStr::bitStringToPointer()
//{
//	if(m_len != (sizeof(void *) * 8))return 0;
//	const char * aux = m_ptr;
//	void * ptr = 0;
//	for(int i=m_len - 1;i >= 0;i--)
//	{
//		if(m_ptr[i] == '1')ptr &= 1;
//		else if(m_ptr[i] !='0')return 0;
//		ptr << 1;
//	}
//	return ptr;
//}




//	static char ascii_jump_table[256]=
//	{
//		//	000 001 002 003 004 005 006 007   008 009 010 011 012 013 014 015
//		//	NUL SOH STX ETX EOT ENQ ACK BEL   BS  HT  LF  VT  FF  CR  SO  SI
//			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
//		//	016 017 018 019 020 021 022 023   024 025 026 027 028 029 030 031
//		//	DLE DC1 DC2 DC3 DC4 NAK SYN ETB   CAN EM  SUB ESC FS  GS  RS  US
//			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
//		//	032 033 034 035 036 037 038 039   040 041 042 043 044 045 046 047
//		//	    !   "   #   $   %   &   '     (   )   *   +   ,   -   .   /
//			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
//		//	048 049 050 051 052 053 054 055   056 057 058 059 060 061 062 063
//		//	0   1   2   3   4   5   6   7     8   9   :   ;   <   =   >   ?
//			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
//		//	064 065 066 067 068 069 070 071   072 073 074 075 076 077 078 079
//		//	@   A   B   C   D   E   F   G     H   I   J   K   L   M   N   O
//			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
//		//	080 081 082 083 084 085 086 087   088 089 090 091 092 093 094 095
//		//	P   Q   R   S   T   U   V   W     X   Y   Z   [   \   ]   ^   _
//			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
//		//	096 097 098 099 100 101 102 103   104 105 106 107 108 109 110 111
//		//	`   a   b   c   d   e   f   g     h   i   j   k   l   m   n   o
//			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
//		//	112 113 114 115 116 117 118 119   120 121 122 123 124 125 126 127
//		//	p   q   r   s   t   u   v   w     x   y   z   {   |   }   ~   
//			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
//		//	128 129 130 131 132 133 134 135   136 137 138 139 140 141 142 143
//		//
//			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
//		//	144 145 146 147 148 149 150 151   152 153 154 155 156 157 158 159
//		//
//			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
//		//	160 161 162 163 164 165 166 167   168 169 170 171 172 173 174 175
//		//
//			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
//		//	176 177 178 179 180 181 182 183   184 185 186 187 188 189 190 191
//		//
//			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
//		//	192 193 194 195 196 197 198 199   200 201 202 203 204 205 206 207
//		//	�  �  �  �  �  �  �  �    �  �  �  �  �  �  �  �
//			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
//		//	208 209 210 211 212 213 214 215   216 217 218 219 220 221 222 223
//		//	�  �  �  �  �  �  �  �    �  �  �  �  �  �  �  �
//			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
//		//	224 225 226 227 228 229 230 231   232 233 234 235 236 237 238 239
//		//	�  �  �  �  �  �  �  �    �  �  �  �  �  �  �  �
//			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
//		//	240 241 242 243 244 245 246 247   248 249 250 251 252 253 254 255
//		//	�  �  �  �  �  �  �  �
//			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0
//	};
