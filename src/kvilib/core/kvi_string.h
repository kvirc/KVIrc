#ifndef _KVI_STRING_H_
#define _KVI_STRING_H_
//=============================================================================
//
//   File : kvi_string.h
//   Creation date : Fri Mar 19 1999 03:06:26 by Szymon Stefanek
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

#include "kvi_settings.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifdef HAVE_STRINGS_H
	#include <strings.h> // useless ?
#endif

#include <QtGlobal>
#include <QString>
#include <QByteArray>

#include "kvi_inttypes.h"
#include "kvi_heapobject.h"
#include "kvi_stdarg.h"



//
//  sigh...
//  IRC is not UNICODE ...(yet) :(
//

#undef __KVI_EXTERN
#ifdef _KVI_STRING_CPP_
	#define __KVI_EXTERN
#else
	#define __KVI_EXTERN extern
#endif


__KVI_EXTERN KVILIB_API bool kvi_qstringEqualCI(const QString &s1,const QString &s2);


// Include inlined assembly implementations if required
#ifdef COMPILE_ix86_ASM
	#include "kvi_strasm.h"
#else
	// Returns true if the string str1 is equal to str2. case sensitive.
	__KVI_EXTERN KVILIB_API bool kvi_strEqualCS(const char *str1,const char *str2);
	// Returns true if the forst len characters of string str1 are equal to str2.
	// case sensitive.
	// Note that if str1 or str2 are shorter than len characters then are considered as NOT equal!
	__KVI_EXTERN KVILIB_API bool kvi_strEqualCSN(const char *str1,const char *str2,int len);
	// no such tricks in non-asm
	#define kvi_strEqualNoLocaleCI(str1,str2) kvi_strEqualCI(str1,str2)
	#define kvi_strEqualNoLocaleCIN(str1,str2,len) kvi_strEqualCIN(str1,str2,len)
	#define kvi_strLen(str) strlen(str)
#endif

// Returns true if the string str1 is equal to str2.
// case insensitive.
__KVI_EXTERN KVILIB_API bool kvi_strEqualCI(const char *str1,const char *str2);
// Returns true if the forst len characters of string str1 are equal to str2.
// case insensitive.
// Note that if str1 or str2 are shorter than len characters then are considered as NOT equal!
__KVI_EXTERN KVILIB_API bool kvi_strEqualCIN(const char *str1,const char *str2,int len);
// My own implementations of strcmp and strncasecmp
// Once I wrote it , I KNOW what they do : ALWAYS :)
// Note that greater here means that comes AFTER in the alphabetic order.
__KVI_EXTERN KVILIB_API int kvi_strcmpCI(const char *str1,const char *str2);
//__KVI_EXTERN KVILIB_API int kvi_strcmpCIN(const char *str1,const char *str2,int len);
__KVI_EXTERN KVILIB_API int kvi_strcmpCS(const char *str1,const char *str2);

// some wide char stuff
typedef kvi_u16_t kvi_wchar_t;
typedef kvi_u32_t kvi_wslen_t;

__KVI_EXTERN KVILIB_API kvi_wslen_t kvi_wstrlen(const kvi_wchar_t * str);
__KVI_EXTERN KVILIB_API int kvi_wvsnprintcf(kvi_wchar_t * buffer,kvi_wslen_t len,const char *fmt,kvi_va_list list);
__KVI_EXTERN KVILIB_API int kvi_wvsnprintf(kvi_wchar_t * buffer,kvi_wslen_t len,const kvi_wchar_t *fmt,kvi_va_list list);

//=============================================================================
//
// A simple string class.<br>
// -No data sharing.<br>
// -Not UNICODE.<br>
// -Has ALWAYS NON-NULL DATA.<br>
// -(Maybe)Unsafe :)<br>
// WARNING : Handle with care and use at own risk :)<br>
//
//=============================================================================

class KVILIB_API KviStr : public KviHeapObject
{
public:
	// No particular reason for these two names...
	// It is just because I like it :)

	enum KviFormatConstructorTag { Format , Sprintf };

	//=============================================================================
	// Constructors
	//=============================================================================

	// Empty string == "", len = 0, 1 byte allocated
	KviStr();

	// Deep copy of the NULL TERMINATED string (NULL str SAFE)
	KviStr(const char *str);

	// Copy len characters from string str (NOT NULL str SAFE, str MUST be at least len chars long)
	KviStr(const char *str,int len);

	// bg and end are pointers to a SINGLE string.<br>
	// A string is extracted starting from bg and ending at end (not included).<br>
	KviStr(const char *bg,const char *end);

	// Format constructor.<br>
	// tag is....yes....a dummy number used to resolve ambiguities.<br>
	// It is SAFE: will fail only if we run out of memory,<br>
	// but can handle only %s %d %u and %c.
	KviStr(KviFormatConstructorTag tag,const char *fmt,...);

	// Carbon copy :)...fast
	KviStr(const KviStr &str);

	// Compat with QT...<br>
	// WARNING : With QT2.x it WILL loose UNICODE data.<br>
	// Safe even if the QString is null.
	KviStr(const QString &str);

	KviStr(const QByteArray &str);

	// Fill sonstructor.
	// Creates a string long fillLen characters filled with character c.<br>
	KviStr(char c,int fillLen = 1);

	KviStr(const kvi_wchar_t * unicode);

	KviStr(const kvi_wchar_t * unicode,int len);

	// just free(m_ptr)
	~KviStr();
public:
	//yes...public..but think it as private...:)
	char *m_ptr; // pointer to allocated buffer , do not change this!
	int   m_len; // string data length not including the terminator

public:
	//=============================================================================
	// Basic const interface (read stuff)
	//=============================================================================

	// Internal data buffer
	char * ptr() const              { return m_ptr; };
	// Length: fast, cached
	int len() const                 { return m_len; };

	// I hate this operator...but sometimes it is really useful
	// especially in macros (kvi_options.cpp)
	operator const char * () const  { return m_ptr; };

	bool isEmpty() const            { return (m_len == 0); };
	bool hasData() const            { return (m_len != 0); };

	// this is better than string = "", it does not call strlen
	void clear();

	// forces the length of this string to be iLen (iLen does NOT include the trailing null : it is automatically added)
	void setLength(int iLen);

	// Returns true if there is something "readable" inside the string
	bool hasNonWhiteSpaceData() const;

	// Character at zero-based index : always safe!
	char & at(int idx) const        { return ((idx < m_len) ? m_ptr[idx] : m_ptr[m_len]);        };

	// character checks
	bool lastCharIs(char ch)  const { return (m_len > 0) ? (*(m_ptr + m_len - 1) == ch) : false; };
	bool firstCharIs(char ch) const { return (*m_ptr == ch);                                     };

	// upper and lower case copies
	KviStr upper() const;
	KviStr lower() const;
	KviStr upperISO88591() const;
	KviStr lowerISO88591() const;

	// left , right & co.
	// all parameters are safety-checked
	KviStr left(int maxLen) const;
	KviStr right(int maxLen) const ;
	KviStr middle(int idx,int maxLen) const;

	KviStr leftToFirst(char c,bool bIncluded = false) const;
	KviStr leftToLast(char c,bool bIncluded = false) const;
//	KviStr leftToFirst(const char * str); const;

	//=============================================================================
	// Non-const interface (write stuff)
	//=============================================================================

	// Null terminator is NOT included in len
	KviStr & setLen(int len);
	// str must not be 0, but len can be anything (it is checked)
	KviStr & setStr(const char *str,int len = -1);
	// Like the special constructor that gets the same args.
	void extractFromString(const char *begin,const char *end);


	// Safe sprintf. This one will never write past the end of the string
	// It can handle only %s %d %u and %c format flags.
	KviStr & sprintf(const char *fmt,...);

	// append functions
	void append(const KviStr &str);
	void append(const QString &str);
	void append(char c);
	void append(const char *str);                                     // str CAN be 0
	void append(const char *str,int len);                             // str CAN NOT be 0, and MUST be at least len chars long
	void append(KviFormatConstructorTag dummy,const char *fmt,...);

	// prepend stuff , same as above
	void prepend(const KviStr &str);
	void prepend(const char *str);                                    // str CAN be 0
	void prepend(const char *str,int len);                            // str CAN NOT be 0, and MUST be at least len chars long

	// if lastCharIs ch does nothing otherwise appends it
	void ensureLastCharIs(char ch)              { if(!lastCharIs(ch))append(ch);        };

	// Change THIS string to uppercase or lowercase
	void toUpperISO88591();
	void toUpper(); // this is LOCALE AWARE (in Turkish it maps i to Ý!)
	void toLowerISO88591();
	void toLower();

	// Assignment
	KviStr & operator=(const KviStr &str);       // deep copy
	KviStr & operator=(const char *str);         // str can be NULL here
	KviStr & operator=(char c);                  // 2 bytes allocated ,m_len = 1
	KviStr & operator=(const QString &str);
	KviStr & operator=(const QByteArray &str);

	// Append operators
	KviStr & operator+=(const KviStr &str)      { append(str); return (*this);          };
	KviStr & operator+=(const char *str)        { append(str); return (*this);          };
	KviStr & operator+=(char c)                 { append(c);   return (*this);          };
	KviStr & operator+=(const QString &str)     { append(str); return (*this);          };

	// Comparison
	bool equalsCI(const KviStr &other) const { if(m_len != other.m_len)return false; return kvi_strEqualCI(m_ptr,other.m_ptr); };
	bool equalsCS(const KviStr &other) const { if(m_len != other.m_len)return false; return kvi_strEqualCS(m_ptr,other.m_ptr); };
	bool equalsCI(const char * other) const { return kvi_strEqualCI(m_ptr,other); };
	bool equalsCS(const char * other) const { return kvi_strEqualCS(m_ptr,other); };
	bool equalsCIN(const char * other,int len) const { return kvi_strEqualCIN(m_ptr,other,len); };
	bool equalsCSN(const char * other,int len) const { return kvi_strEqualCSN(m_ptr,other,len); };

	//=============================================================================
	// HEX and Base64 stuff
	//=============================================================================

	// HEX transforms functions
	void bufferToHex(const char *buffer,int len);
	// Allocates the needed buffer and returns the allocated length,
	// returns -1 in case of error (and allocates nothing)
	// The string MUST contain only hex digits, and the digits MUST be in couples. (len % 2) must equal 0!
	// So this will fail also if there are leading or trailing spaces!
	int hexToBuffer(char ** buffer,bool bNullToNewlines = false);
	// BASE64 stuff
	void bufferToBase64(const char * buffer,int len);
	// same as hexToBuffer but obviously transforms base64 notation to binary data (len % 4) must equal 0!
	int base64ToBuffer(char ** buffer,bool bNullToNewlines = false);

	// frees a buffer allocated by hexToBuffer or base64ToBuffer
	static void freeBuffer(char * buffer);

	//=============================================================================
	// Splitters
	//=============================================================================

	// cut
	KviStr & cutLeft(int len);             // kills the first len characters
	KviStr & cutRight(int len);            // kills the last len characters
	KviStr & cut(int idx,int len);
	KviStr & cutToFirst(char c,bool bIncluded = true); // cuts the left part of the string up to the first character c or does nothing if the char c is not in the string
	KviStr & cutToLast(char c,bool bIncluded = true);
	KviStr & cutFromFirst(char c,bool bIncluded = true);
	KviStr & cutFromLast(char c,bool bIncluded = true);
	KviStr & cutToFirst(const char *c,bool bIncluded = true); // cuts the left part of the string up to the first character c or does nothing if the char c is not in the string
	KviStr & cutToLast(const char *c,bool bIncluded = true);
	KviStr & cutFromFirst(const char *c,bool bIncluded = true);
	KviStr & cutFromLast(const char *c,bool bIncluded = true);
	// & paste
	KviStr & insert(int idx,const char *data);
	KviStr & insert(int idx,char c);
	//Replaces all occurences of char c with the string str
	KviStr & replaceAll(const char c,const char *str);
	//same as above but with a string
	KviStr & replaceAll(const char *toFind,const char *str,bool bCaseS = true);

	KviStr & transliterate(const char * szToFind,const char * szReplacement);

	// Strips whitespace characters from beginning of this string.
	KviStr & stripLeftWhiteSpace();
	KviStr & stripRightWhiteSpace();
	// Stips inital and final WHITESPACE characters (see man isspace),<br>
	// and returns a reference to this string.
	KviStr & trimmed();

	// Strips spaces and tabs only
	KviStr & stripSpace();
	// Strips all occurences of the character c from the beginning of the string.<br>
	// Note that c can not be '\0' :)
	KviStr & stripLeft(char c);
	KviStr & stripRight(char c);

	//=============================================================================
	// Tokenize
	//=============================================================================

	// Extracts (copy to str and remove) a token from this string ,<br>
	// and returns true if there are more tokens to extract<br>
	// Does not strip initial separators!!<br>
	// str can NOT be this string.
	bool getToken(KviStr &str,char sep);
	// Does not strip initial separators!<br>
	// Can assign also to this string.
	KviStr getToken(char sep);
	// Extracts a line from the string.<br>
	// Returns false if there was no data to extract
	bool getLine(KviStr &str);

	// splits this string in a null-terminated array of strings
	// separated by sep.
	KviStr ** splitToArray(char sep,int max,int * realCount) const;
	//KviStr ** splitToArray(const char * sep,int max,int * realCount) const;
	static void freeArray(KviStr ** strings);
	// joins the array to this string
	// if sep is not 0 , it is inserted between the strings
	// if bLastSep is true and sep is non 0 , then sep is also appended at the end
	// of the buffer (after the last string)
	void joinFromArray(KviStr ** strings,const char * sep = 0,bool bLastSep = false);

	//=============================================================================
	// Utils
	//=============================================================================

	// encodes chars that have nonzero in the jumptable
	// into %HH equivalents
	KviStr & hexEncodeWithTable(const unsigned char table[256]);
	KviStr & hexEncodeWhiteSpace();
	KviStr & hexDecode(const char * pFrom);
	KviStr & hexDecode(){ return hexDecode(m_ptr); };

	//=============================================================================
	// Contains / occurence count
	//=============================================================================

	// Returns true if at least one occurence of str is found
	bool contains(const char *str,bool caseS=true) const;
	// Returns true if at least one occurence of character c is found in this string
	bool contains(char c,bool caseS=true) const;
	// Returns the number of occurences of string str in this string.<br>
	// Overlapped matches are counted.
	int occurences(const char *str,bool caseS=true) const;
	// Returns the number of occurences of character c in this string
	int occurences(char c,bool caseS=true) const;

	//=============================================================================
	// Find
	//=============================================================================

	// Finds the first occurence of the character c in this string,<br>
	// and returns its zero-based index or -1 if c can not be found.<br>
	// c can NOT be '\0' here.
	int findFirstIdx(char c) const;
	// Finds the first occurence of the sub-string str in this string,<br>
	// and returns its zero-based index or -1 if the sub-string can not be found.<br>
	// str can NOT be 0 here.
	int findFirstIdx(const char *str,bool caseS = true) const;
	// Finds the last occurence of the character c in this string,<br>
	// and returns its zero-based index or -1 if the character can not be found.
	int findLastIdx(char c) const;
	// Finds the last occurence of the sub-string str in this string,<br>
	// and returns its zero-based index or -1 if the sub-string can not be found.<br>
	// str can NOT be 0 here.
	int findLastIdx(const char *str,bool caseS = true) const;

	int find(char c,int startIdx) const;
	int find(const char * str,int startIdx,bool caseS = true) const;
	int findRev(const char * str,int startIdx,bool caseS = true) const;

	//=============================================================================
	// Numbers
	//=============================================================================

	// everything in base 10.... no overflow checks here
	long toLong(bool *bOk=0) const;
	unsigned long toULong(bool *bOk=0) const;
	long long toLongLong(bool *bOk=0) const;
	unsigned long long toULongLong(bool *bOk=0) const;
	char toChar(bool *bOk=0) const              { return (char)toLong(bOk);             };
	unsigned char toUChar(bool *bOk=0) const    { return (unsigned char)toULong(bOk);   };
	int toInt(bool *bOk=0) const                { return (int)toLong(bOk);              };
	unsigned int toUInt(bool *bOk=0) const      { return (unsigned int)toULong(bOk);    };
	short toShort(bool *bOk=0) const            { return (short)toLong(bOk);            };
	unsigned short toUShort(bool *bOk=0) const  { return (unsigned short)toLong(bOk);   };

	KviStr & setNum(long num);
	KviStr & setNum(unsigned long num);

	KviStr & setNum(int num)                    { return setNum((long)num);             };
	KviStr & setNum(unsigned int num)           { return setNum((unsigned long)num);    };
	KviStr & setNum(short num)                  { return setNum((long)num);             };
	KviStr & setNum(unsigned short num)         { return setNum((unsigned long)num);    };
	KviStr & setNum(char num)                   { return setNum((long)num);             };
	KviStr & setNum(unsigned char num)          { return setNum((unsigned long)num);    };

	// Retuns true if the string contains only digits and an optional '-' character
	// at the beginning.<be>
	// Space characters are allowed at the begginning and the end.<br>
	// There is no overflow check!
	bool isNum() const;
	bool isUnsignedNum() const;

	// special functions for multiple bases
	long toLongExt(bool *bOk = 0,int base = 0);
	// unsigned long toULongExt(bool *bOk = 0,int base = 0); //never used

	// returns an empty string...
	// this if often useful!
	static KviStr & emptyString();

	//=============================================================================
	// Dead interface
	//=============================================================================

	// Transform a pointer to a string with all 0 and 1
	// void pointerToBitString(const void * ptr);
	// Get a pointer from a string all of 0 and 1 : return 0 if invalid
	// void * bitStringToPointer();

	//=============================================================================
	// "External string" helper functions
	//=============================================================================

	// FIXME: Should it be KviStrExt::contains namespace ?
	static bool ext_contains(register const char * data,const char * item,bool caseS = true);
};

// FIXME: the functions below should end in the KviStr namespace ???


// Cool string parsing function.
// It will extract the first found token from the string aux_ptr , and return
// a pointer to the beginning of the next token , or end of the string.
// It skips the initial sep characters!
__KVI_EXTERN KVILIB_API const char * kvi_extractToken(KviStr &str,const char *aux_ptr,char sep =' ');
// Does not skip the beginning separators!
// Extracts data from the string up to the next separator character or the end of the string.
// and returns a pointer to that separator (or string end).
__KVI_EXTERN KVILIB_API const char * kvi_extractUpTo(KviStr &str,const char *aux_ptr,char sep=' ');
// Reduced vsnprintf...
// Handles %s,%c,%d,%u  (%% are TWO percents here and not one.)
// Returns -1 if the formatted string exceeded the buffer length.
// Otherwise returns the length of the formatted buffer...(not including '\0')
__KVI_EXTERN KVILIB_API int kvi_vsnprintf(char *buffer,int len,const char *fmt,kvi_va_list list);
// Reduced vsnprintf: special version for irc.
// Handles %s,%c,%d,%u  (%% are TWO percents here and not one.)
// Writes up to 510 characters and terminates the string with a CRLF
// Sets bTruncated if the requested format string was too large to fit in 512 bytes
// otherwise sets it to false; The buffer MUST be at least 512 bytes long.
// Always returns the length of the formatted buffer...(max 512 - min 2=CRLF)
__KVI_EXTERN KVILIB_API int kvi_irc_vsnprintf(char *buffer,const char *fmt,kvi_va_list list,bool *bTruncated);

// WILDCARD EXPRESSION MATCHING FUNCTIONS

// Returns true if the two regular expressions with wildcards matches
__KVI_EXTERN KVILIB_API bool kvi_matchWildExpr(register const char *m1,register const char *m2);
// Returns true if the two regular expressions with wildcards matches, case sensitive
//__KVI_EXTERN bool kvi_matchWildExprCS(register const char *m1,register const char *m2); // actually unused
// Same as kvi_matchWildExpr but with an additional char that acts as string terminator
// If there is a match this function returns true and puts the pointers where it stopped in r1 and r2
__KVI_EXTERN KVILIB_API bool kvi_matchWildExprWithTerminator(register const char *m1,register const char *m2,char terminator,
			const char ** r1,const char ** r2);

// Returns true if the wildcard expression exp matches the string str
__KVI_EXTERN KVILIB_API bool kvi_matchStringCI(register const char * exp,register const char * str);
#define kvi_matchString kvi_matchStringCI
__KVI_EXTERN KVILIB_API bool kvi_matchStringCS(register const char * exp,register const char * str);
__KVI_EXTERN KVILIB_API bool kvi_matchStringWithTerminator(register const char * exp,register const char * str,char terminator,const char ** r1,const char ** r2);

// This function works like a particular case of strncmp.
// It evaluates if str2 is the terminal part of str1.
// example: if str1 is "this is an experiment" and str2 is "xperiment"
// return 0.
// With the index parameter, the match start on str1 from the specified
// index. For example:
// if str1 is "this is an experiment" and str2 is "an" we have return !0
// but "this is an experiment"
//      012345678901234567890
// if we call kvi_strsubRevCS("this is an experiment","an", 9) we got a match.
__KVI_EXTERN KVILIB_API int kvi_strMatchRevCS(const char *str1, const char *str2, int index=-1);

// KviStr comparison non-member operators
__KVI_EXTERN KVILIB_API inline bool operator==(const KviStr &left,const KviStr &right)
{ return (left.m_len == right.m_len) ? kvi_strEqualCS(left.m_ptr,right.m_ptr) : false; }
__KVI_EXTERN KVILIB_API inline bool operator==(const KviStr &left,const char *right)
{ return kvi_strEqualCS(left.m_ptr,right); }
__KVI_EXTERN KVILIB_API inline bool operator==(const char *left,const KviStr &right)
{ return kvi_strEqualCS(left,right.m_ptr); }
__KVI_EXTERN KVILIB_API inline bool operator!=(const KviStr &left,const KviStr &right)
{ return !kvi_strEqualCS(left.m_ptr,right.m_ptr); }
__KVI_EXTERN KVILIB_API inline bool operator!=(const KviStr &left,const char *right)
{ return !kvi_strEqualCS(left.m_ptr,right); }
__KVI_EXTERN KVILIB_API inline bool operator!=(const char *left,const KviStr &right)
{ return !kvi_strEqualCS(left,right.m_ptr); }

__KVI_EXTERN KVILIB_API inline KviStr operator+(const KviStr &left,const KviStr &right)
{ KviStr ret(left); ret += right; return ret; }
__KVI_EXTERN KVILIB_API inline KviStr operator+(const KviStr &left,const char *right)
{ KviStr ret(left); ret += right; return ret; }
__KVI_EXTERN KVILIB_API inline KviStr operator+(const char *left,const KviStr &right)
{ KviStr ret(left); ret += right; return ret; }
__KVI_EXTERN KVILIB_API inline KviStr operator+(const KviStr &left,char right)
{ KviStr ret(left); ret += right; return ret; }
__KVI_EXTERN KVILIB_API inline KviStr operator+(char left,const KviStr &right)
{ KviStr ret(left); ret += right; return ret; }

inline int kvi_compare(const KviStr * p1,const KviStr * p2)
{
	return kvi_strcmpCI(p1->ptr(),p2->ptr());
}

#endif //_KVI_STRING_H_
