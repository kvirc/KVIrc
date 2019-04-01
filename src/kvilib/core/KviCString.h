#ifndef _KVI_STRING_H_
#define _KVI_STRING_H_
//=============================================================================
//
//   File : KviCString.h
//   Creation date : Fri Mar 19 1999 03:06:26 by Szymon Stefanek
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

#include "kvi_settings.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifdef HAVE_STRINGS_H
#include <strings.h> // useless ?
#endif

#include <QByteArray>
#include <QtGlobal>

#include "kvi_inttypes.h"
#include "kvi_stdarg.h"
#include "KviHeapObject.h"

//
//  IRC is not UNICODE ...(yet) :(
//

#undef __KVI_EXTERN
#ifdef _KVI_STRING_CPP_
#define __KVI_EXTERN
#else
#define __KVI_EXTERN extern
#endif

__KVI_EXTERN KVILIB_API bool kvi_qstringEqualCI(const QString & s1, const QString & s2);

// Include inlined assembly implementations if required

// Returns true if the string str1 is equal to str2. case sensitive.
__KVI_EXTERN KVILIB_API bool kvi_strEqualCS(const char * str1, const char * str2);
// Returns true if the forst len characters of string str1 are equal to str2.
// case sensitive.
// Note that if str1 or str2 are shorter than len characters then are considered as NOT equal!
__KVI_EXTERN KVILIB_API bool kvi_strEqualCSN(const char * str1, const char * str2, int len);
// no such tricks in non-asm
#define kvi_strEqualNoLocaleCI(str1, str2) kvi_strEqualCI(str1, str2)
#define kvi_strEqualNoLocaleCIN(str1, str2, len) kvi_strEqualCIN(str1, str2, len)
#define kvi_strLen(str) strlen(str)

// Returns true if the string str1 is equal to str2.
// case insensitive.
__KVI_EXTERN KVILIB_API bool kvi_strEqualCI(const char * str1, const char * str2);
// Returns true if the forst len characters of string str1 are equal to str2.
// case insensitive.
// Note that if str1 or str2 are shorter than len characters then are considered as NOT equal!
__KVI_EXTERN KVILIB_API bool kvi_strEqualCIN(const char * str1, const char * str2, int len);
// My own implementations of strcmp and strncasecmp
// Once I wrote it, I KNOW what they do : ALWAYS :)
// Note that greater here means that comes AFTER in the alphabetic order.
__KVI_EXTERN KVILIB_API int kvi_strcmpCI(const char * str1, const char * str2);
//__KVI_EXTERN KVILIB_API int kvi_strcmpCIN(const char *str1,const char *str2,int len);
__KVI_EXTERN KVILIB_API int kvi_strcmpCS(const char * str1, const char * str2);

// some wide char stuff
typedef kvi_u16_t kvi_wchar_t;
typedef kvi_u32_t kvi_wslen_t;

__KVI_EXTERN KVILIB_API kvi_wslen_t kvi_wstrlen(const kvi_wchar_t * str);
__KVI_EXTERN KVILIB_API int kvi_wvsnprintcf(kvi_wchar_t * buffer, kvi_wslen_t len, const char * fmt, kvi_va_list list);
__KVI_EXTERN KVILIB_API int kvi_wvsnprintf(kvi_wchar_t * buffer, kvi_wslen_t len, const kvi_wchar_t * fmt, kvi_va_list list);

//
// A simple string class.<br>
// -No data sharing.<br>
// -Not UNICODE.<br>
// -Has ALWAYS NON-NULL DATA.<br>
// -(Maybe)Unsafe :)<br>
// WARNING : Handle with care and use at own risk :)<br>
//

class KVILIB_API KviCString : public KviHeapObject
{
public:
	// No particular reason for these two names...
	// It is just because I like it :)

	enum KviFormatConstructorTag
	{
		Format,
		Sprintf
	};

	// Constructors
	// Empty string == "", len = 0, 1 byte allocated
	KviCString();

	// Deep copy of the NULL TERMINATED string (NULL str SAFE)
	KviCString(const char * str);

	// Copy len characters from string str (NOT NULL str SAFE, str MUST be at least len chars long)
	KviCString(const char * str, int len);

	// bg and end are pointers to a SINGLE string.<br>
	// A string is extracted starting from bg and ending at end (not included).<br>
	KviCString(const char * bg, const char * end);

	// Format constructor.<br>
	// tag is....yes....a dummy number used to resolve ambiguities.<br>
	// It is SAFE: will fail only if we run out of memory,<br>
	// but can handle only %s %d %u and %c.
	KviCString(KviFormatConstructorTag tag, const char * fmt, ...);

	// Carbon copy :)...fast
	KviCString(const KviCString & str);

	// Compat with QT...<br>
	// WARNING : With QT2.x it WILL loose UNICODE data.<br>
	// Safe even if the QString is null.
	KviCString(const QString & str);

	KviCString(const QByteArray & str);

	// Fill sonstructor.
	// Creates a string long fillLen characters filled with character c.<br>
	KviCString(char c, int fillLen = 1);

	KviCString(const kvi_wchar_t * unicode);

	KviCString(const kvi_wchar_t * unicode, int len);

	// just free(m_ptr)
	~KviCString();

public:
	//yes...public..but think it as private...:)
	char * m_ptr; // pointer to allocated buffer, do not change this!
	int m_len;    // string data length not including the terminator

public:

	// Basic const interface (read stuff)
	// Internal data buffer
	char * ptr() const { return m_ptr; };
	// Length: fast, cached
	int len() const { return m_len; };

	// I hate this operator...but sometimes it is really useful
	// especially in macros (KviOptions.cpp)
	operator const char *() const { return m_ptr; };

	bool isEmpty() const { return (m_len == 0); };
	bool hasData() const { return (m_len != 0); };

	// this is better than string = "", it does not call strlen
	void clear();

	// Returns true if there is something "readable" inside the string
	bool hasNonWhiteSpaceData() const;

	// Character at zero-based index : always safe!
	char & at(int idx) const { return ((idx < m_len) ? m_ptr[idx] : m_ptr[m_len]); };

	// character checks
	bool lastCharIs(char ch) const { return (m_len > 0) ? (*(m_ptr + m_len - 1) == ch) : false; };
	bool firstCharIs(char ch) const { return (*m_ptr == ch); };

	// upper and lower case copies
	KviCString upper() const;
	KviCString lower() const;
	KviCString upperISO88591() const;
	KviCString lowerISO88591() const;

	// left, right & co.
	// all parameters are safety-checked
	KviCString left(int maxLen) const;
	KviCString right(int maxLen) const;
	KviCString middle(int idx, int maxLen) const;

	KviCString leftToFirst(char c, bool bIncluded = false) const;
	KviCString leftToLast(char c, bool bIncluded = false) const;
	//	KviCString leftToFirst(const char * str); const;

	// Non-const interface (write stuff)
	// Null terminator is NOT included in len
	KviCString & setLen(int len);

	// str must not be 0, but len can be anything (it is checked)
	KviCString & setStr(const char * str, int len = -1);
	// Like the special constructor that gets the same args.
	void extractFromString(const char * begin, const char * end);

	// Safe sprintf. This one will never write past the end of the string
	// It can handle only %s %d %u and %c format flags.
	KviCString & sprintf(const char * fmt, ...);

	KviCString & vsprintf(const char * fmt, kvi_va_list list);

	// append functions
	void append(const KviCString & str);
	void append(const QString & str);
	void append(char c);
	void append(const char * str);          // str CAN be 0
	void append(const char * str, int len); // str CAN NOT be 0, and MUST be at least len chars long
	void append(KviFormatConstructorTag dummy, const char * fmt, ...);

	// prepend stuff, same as above
	void prepend(const KviCString & str);
	void prepend(const char * str);          // str CAN be 0
	void prepend(const char * str, int len); // str CAN NOT be 0, and MUST be at least len chars long

	// if lastCharIs ch does nothing otherwise appends it
	void ensureLastCharIs(char ch)
	{
		if(!lastCharIs(ch))
			append(ch);
	};

	// Change THIS string to uppercase or lowercase
	void toUpperISO88591();
	void toUpper(); // this is LOCALE AWARE (in Turkish it maps i to Ý!)
	void toLowerISO88591();
	void toLower();

	// Assignment
	KviCString & operator=(const KviCString & str); // deep copy
	KviCString & operator=(const char * str);       // str can be nullptr here
	KviCString & operator=(char c);                 // 2 bytes allocated,m_len = 1
	KviCString & operator=(const QString & str);
	KviCString & operator=(const QByteArray & str);

	// Append operators
	KviCString & operator+=(const KviCString & str)
	{
		append(str);
		return (*this);
	};
	KviCString & operator+=(const char * str)
	{
		append(str);
		return (*this);
	};
	KviCString & operator+=(char c)
	{
		append(c);
		return (*this);
	};
	KviCString & operator+=(const QString & str)
	{
		append(str);
		return (*this);
	};

	// Comparison
	bool equalsCI(const KviCString & other) const
	{
		if(m_len != other.m_len)
			return false;
		return kvi_strEqualCI(m_ptr, other.m_ptr);
	};
	bool equalsCS(const KviCString & other) const
	{
		if(m_len != other.m_len)
			return false;
		return kvi_strEqualCS(m_ptr, other.m_ptr);
	};
	bool equalsCI(const char * other) const { return kvi_strEqualCI(m_ptr, other); };
	bool equalsCS(const char * other) const { return kvi_strEqualCS(m_ptr, other); };
	bool equalsCIN(const char * other, int len) const { return kvi_strEqualCIN(m_ptr, other, len); };
	bool equalsCSN(const char * other, int len) const { return kvi_strEqualCSN(m_ptr, other, len); };

	// HEX and Base64 stuff
	// HEX transforms functions
	void bufferToHex(const char * buffer, int len);
	// Allocates the needed buffer and returns the allocated length,
	// returns -1 in case of error (and allocates nothing)
	// The string MUST contain only hex digits, and the digits MUST be in couples. (len % 2) must equal 0!
	// So this will fail also if there are leading or trailing spaces!
	int hexToBuffer(char ** buffer, bool bNullToNewlines = false);
	// BASE64 stuff
	void bufferToBase64(const char * buffer, int len);
	// same as hexToBuffer but obviously transforms base64 notation to binary data (len % 4) must equal 0!
	int base64ToBuffer(char ** buffer, bool bNullToNewlines = false);

	// frees a buffer allocated by hexToBuffer or base64ToBuffer
	static void freeBuffer(char * buffer);

	// Splitters
	// cut
	KviCString & cutLeft(int len);  // kills the first len characters
	KviCString & cutRight(int len); // kills the last len characters
	KviCString & cut(int idx, int len);
	KviCString & cutToFirst(char c, bool bIncluded = true); // cuts the left part of the string up to the first character c or does nothing if the char c is not in the string
	KviCString & cutToLast(char c, bool bIncluded = true);
	KviCString & cutFromFirst(char c, bool bIncluded = true);
	KviCString & cutFromLast(char c, bool bIncluded = true);
	KviCString & cutToFirst(const char * c, bool bIncluded = true); // cuts the left part of the string up to the first character c or does nothing if the char c is not in the string
	KviCString & cutToLast(const char * c, bool bIncluded = true);
	KviCString & cutFromFirst(const char * c, bool bIncluded = true);
	KviCString & cutFromLast(const char * c, bool bIncluded = true);
	// & paste
	KviCString & insert(int idx, const char * data);
	KviCString & insert(int idx, char c);
	//Replaces all occurrences of char c with the string str
	KviCString & replaceAll(const char c, const char * str);
	//same as above but with a string
	KviCString & replaceAll(const char * toFind, const char * str, bool bCaseS = true);

	KviCString & transliterate(const char * szToFind, const char * szReplacement);

	// Strips whitespace characters from beginning of this string.
	KviCString & stripLeftWhiteSpace();
	KviCString & stripRightWhiteSpace();
	// Strips initial and final WHITESPACE characters (see man isspace),<br>
	// and returns a reference to this string.
	KviCString & trim();

	// Strips spaces and tabs only
	KviCString & stripSpace();
	// Strips all occurrences of the character c from the beginning of the string.<br>
	// Note that c can not be '\0' :)
	KviCString & stripLeft(char c);
	KviCString & stripRight(char c);

	// either "truncate to" or "add padding up" to iLen characters.
	KviCString & padRight(int iLen, const char c = '\0');

	// Tokenize
	// Extracts (copy to str and remove) a token from this string,<br>
	// and returns true if there are more tokens to extract<br>
	// Does not strip initial separators!!<br>
	// str can NOT be this string.
	bool getToken(KviCString & str, char sep);
	// Does not strip initial separators!<br>
	// Can assign also to this string.
	KviCString getToken(char sep);
	// Extracts a line from the string.<br>
	// Returns false if there was no data to extract
	bool getLine(KviCString & str);

	// splits this string in a null-terminated array of strings
	// separated by sep.
	KviCString ** splitToArray(char sep, int max, int * realCount) const;
	//KviCString ** splitToArray(const char * sep,int max,int * realCount) const;
	static void freeArray(KviCString ** strings);
	// joins the array to this string
	// if sep is not 0, it is inserted between the strings
	// if bLastSep is true and sep is non 0, then sep is also appended at the end
	// of the buffer (after the last string)
	void joinFromArray(KviCString ** strings, const char * sep = nullptr, bool bLastSep = false);

	// Utils
	// encodes chars that have nonzero in the jumptable
	// into %HH equivalents
	KviCString & hexEncodeWithTable(const unsigned char table[256]);
	KviCString & hexEncodeWhiteSpace();
	KviCString & hexDecode(const char * pFrom);
	KviCString & hexDecode() { return hexDecode(m_ptr); };

	// Contains / occurrence count
	// Returns true if at least one occurrence of str is found
	bool contains(const char * str, bool caseS = true) const;
	// Returns true if at least one occurrence of character c is found in this string
	bool contains(char c, bool caseS = true) const;
	// Returns the number of occurrences of string str in this string.<br>
	// Overlapped matches are counted.
	int occurrences(const char * str, bool caseS = true) const;
	// Returns the number of occurrences of character c in this string
	int occurrences(char c, bool caseS = true) const;

	// Find
	// Finds the first occurrence of the character c in this string,<br>
	// and returns its zero-based index or -1 if c can not be found.<br>
	// c can NOT be '\0' here.
	int findFirstIdx(char c) const;
	// Finds the first occurrence of the sub-string str in this string,<br>
	// and returns its zero-based index or -1 if the sub-string can not be found.<br>
	// str can NOT be 0 here.
	int findFirstIdx(const char * str, bool caseS = true) const;
	// Finds the last occurrence of the character c in this string,<br>
	// and returns its zero-based index or -1 if the character can not be found.
	int findLastIdx(char c) const;
	// Finds the last occurrence of the sub-string str in this string,<br>
	// and returns its zero-based index or -1 if the sub-string can not be found.<br>
	// str can NOT be 0 here.
	int findLastIdx(const char * str, bool caseS = true) const;

	int find(char c, int startIdx) const;
	int find(const char * str, int startIdx, bool caseS = true) const;
	int findRev(const char * str, int startIdx, bool caseS = true) const;

	// Numbers
	// everything in base 10.... no overflow checks here
	long toLong(bool * bOk = nullptr) const;
	unsigned long toULong(bool * bOk = nullptr) const;
	long long toLongLong(bool * bOk = nullptr) const;
	unsigned long long toULongLong(bool * bOk = nullptr) const;
	char toChar(bool * bOk = nullptr) const { return (char)toLong(bOk); };
	unsigned char toUChar(bool * bOk = nullptr) const { return (unsigned char)toULong(bOk); };
	int toInt(bool * bOk = nullptr) const { return (int)toLong(bOk); };
	unsigned int toUInt(bool * bOk = nullptr) const { return (unsigned int)toULong(bOk); };
	short toShort(bool * bOk = nullptr) const { return (short)toLong(bOk); };
	unsigned short toUShort(bool * bOk = nullptr) const { return (unsigned short)toLong(bOk); };

	KviCString & setNum(long num);
	KviCString & setNum(unsigned long num);

	KviCString & setNum(int num) { return setNum((long)num); };
	KviCString & setNum(unsigned int num) { return setNum((unsigned long)num); };
	KviCString & setNum(short num) { return setNum((long)num); };
	KviCString & setNum(unsigned short num) { return setNum((unsigned long)num); };
	KviCString & setNum(char num) { return setNum((long)num); };
	KviCString & setNum(unsigned char num) { return setNum((unsigned long)num); };

	// Returns true if the string contains only digits and an optional '-' character
	// at the beginning.<be>
	// Space characters are allowed at the begginning and the end.<br>
	// There is no overflow check!
	bool isNum() const;
	bool isUnsignedNum() const;

	// special functions for multiple bases
	long toLongExt(bool * bOk = nullptr, int base = 0);
	// unsigned long toULongExt(bool *bOk = nullptr,int base = 0); //never used

	// returns an empty string...
	// this if often useful!
	static KviCString & emptyString();

	// Dead interface

	// Transform a pointer to a string with all 0 and 1
	// void pointerToBitString(const void * ptr);
	// Get a pointer from a string all of 0 and 1 : return nullptr if invalid
	// void * bitStringToPointer();

	// "External string" helper functions

	// FIXME: Should it be KviCStringExt::contains namespace ?
	static bool ext_contains(const char * data, const char * item, bool caseS = true);
};

// FIXME: the functions below should end in the KviCString namespace ???

// Cool string parsing function.
// It will extract the first found token from the string aux_ptr, and return
// a pointer to the beginning of the next token, or end of the string.
// It skips the initial sep characters!
__KVI_EXTERN KVILIB_API const char * kvi_extractToken(KviCString & str, const char * aux_ptr, char sep = ' ');
// Does not skip the beginning separators!
// Extracts data from the string up to the next separator character or the end of the string.
// and returns a pointer to that separator (or string end).
__KVI_EXTERN KVILIB_API const char * kvi_extractUpTo(KviCString & str, const char * aux_ptr, char sep = ' ');
// Reduced vsnprintf...
// Handles %s,%c,%d,%u  (%% are TWO percents here and not one.)
// Returns -1 if the formatted string exceeded the buffer length.
// Otherwise returns the length of the formatted buffer...(not including '\0')
__KVI_EXTERN KVILIB_API int kvi_vsnprintf(char * buffer, int len, const char * fmt, kvi_va_list list);
// Reduced vsnprintf: special version for irc.
// Handles %s,%c,%d,%u  (%% are TWO percents here and not one.)
// Writes up to 510 characters and terminates the string with a CRLF
// Sets bTruncated if the requested format string was too large to fit in 512 bytes
// otherwise sets it to false; The buffer MUST be at least 512 bytes long.
// Always returns the length of the formatted buffer...(max 512 - min 2=CRLF)
__KVI_EXTERN KVILIB_API int kvi_irc_vsnprintf(char * buffer, const char * fmt, kvi_va_list list, bool * bTruncated);

// WILDCARD EXPRESSION MATCHING FUNCTIONS

// Returns true if the two regular expressions with wildcards matches
__KVI_EXTERN KVILIB_API bool kvi_matchWildExpr(const char * m1, const char * m2);
// Returns true if the two regular expressions with wildcards matches, case sensitive
//__KVI_EXTERN bool kvi_matchWildExprCS(const char *m1, const char *m2); // actually unused
// Same as kvi_matchWildExpr but with an additional char that acts as string terminator
// If there is a match this function returns true and puts the pointers where it stopped in r1 and r2
__KVI_EXTERN KVILIB_API bool kvi_matchWildExprWithTerminator(const char * m1, const char * m2, char terminator,
    const char ** r1, const char ** r2);

// Returns true if the wildcard expression exp matches the string str
__KVI_EXTERN KVILIB_API bool kvi_matchStringCI(const char * exp, const char * str);
#define kvi_matchString kvi_matchStringCI
__KVI_EXTERN KVILIB_API bool kvi_matchStringCS(const char * exp, const char * str);
__KVI_EXTERN KVILIB_API bool kvi_matchStringWithTerminator(const char * exp, const char * str, char terminator, const char ** r1, const char ** r2);

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
__KVI_EXTERN KVILIB_API int kvi_strMatchRevCS(const char * str1, const char * str2, int index = -1);

// KviCString comparison non-member operators
__KVI_EXTERN inline bool operator==(const KviCString & left, const KviCString & right)
{
	return (left.m_len == right.m_len) ? kvi_strEqualCS(left.m_ptr, right.m_ptr) : false;
}
__KVI_EXTERN inline bool operator==(const KviCString & left, const char * right)
{
	return kvi_strEqualCS(left.m_ptr, right);
}
__KVI_EXTERN inline bool operator==(const char * left, const KviCString & right)
{
	return kvi_strEqualCS(left, right.m_ptr);
}
__KVI_EXTERN inline bool operator!=(const KviCString & left, const KviCString & right)
{
	return !kvi_strEqualCS(left.m_ptr, right.m_ptr);
}
__KVI_EXTERN inline bool operator!=(const KviCString & left, const char * right)
{
	return !kvi_strEqualCS(left.m_ptr, right);
}
__KVI_EXTERN inline bool operator!=(const char * left, const KviCString & right)
{
	return !kvi_strEqualCS(left, right.m_ptr);
}

__KVI_EXTERN inline KviCString operator+(const KviCString & left, const KviCString & right)
{
	KviCString ret(left);
	ret += right;
	return ret;
}
__KVI_EXTERN inline KviCString operator+(const KviCString & left, const char * right)
{
	KviCString ret(left);
	ret += right;
	return ret;
}
__KVI_EXTERN inline KviCString operator+(const char * left, const KviCString & right)
{
	KviCString ret(left);
	ret += right;
	return ret;
}
__KVI_EXTERN inline KviCString operator+(const KviCString & left, char right)
{
	KviCString ret(left);
	ret += right;
	return ret;
}
__KVI_EXTERN inline KviCString operator+(char left, const KviCString & right)
{
	KviCString ret(left);
	ret += right;
	return ret;
}

inline int kvi_compare(const KviCString * p1, const KviCString * p2)
{
	return kvi_strcmpCI(p1->ptr(), p2->ptr());
}

#endif //_KVI_STRING_H_
