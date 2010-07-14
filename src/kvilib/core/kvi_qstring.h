#ifndef _KVI_QSTRING_H_
#define _KVI_QSTRING_H_
//=============================================================================
//
//   File : kvi_qstring.h
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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

/**
* \file kvi_qstring.h
* \author Szymon Stefanek
* \brief Helper functions for the QString class
*/

#include "kvi_settings.h"
#include "kvi_inttypes.h"
#include "kvi_stdarg.h"

#include <QString>
#include <QByteArray>

/**
* \namespace KviQString
* \brief A namespace for QString helper functions
*
* This namespace contains several helper functions that are used when dealing
* with QString.
*/
namespace KviQString
{
	/**
	* \brief A global empty string (note that this is ALSO NULL under Qt 3.x)
	*/
	extern KVILIB_API const QString Empty;
	
	/**
	* \brief Returns a readable size in byte's multiples
	*
	* This will return strings like KiB, MiB, GiB, TiB and so on
	* \param size The size of the file
	* \return QString
	*/
	extern KVILIB_API QString makeSizeReadable(quint64 size);
	
	/**
	* \brief Compares two strings with case sensitive
	*
	* It returns true if the strings are equal, false otherwise
	* \param sz1 First string
	* \param sz2 Second string
	* \return bool
	*/
	extern KVILIB_API bool equalCS(const QString & sz1, const QString & sz2);
	
	/**
	* \brief Compares two strings with case sensitive
	*
	* It returns true if the strings are equal, false otherwise
	* \param sz1 First string
	* \param pc2 Second string
	* \return bool
	*/
	extern KVILIB_API bool equalCS(const QString & sz1, const char * pc2);
	
	/**
	* \brief Compares two strings with case sensitive
	*
	* It returns true if the strings are equal, false otherwise
	* \param pc1 First string
	* \param sz2 Second string
	* \return bool
	*/
	inline bool equalCS(const char * pc1, const QString & sz2)
		{ return equalCS(sz2,pc1); }
	
	/**
	* \brief Compares two strings with case insensitive
	*
	* It returns true if the strings are equal, false otherwise
	* \param sz1 First string
	* \param sz2 Second string
	* \return bool
	*/
	extern KVILIB_API bool equalCI(const QString & sz1, const QString & sz2);
	
	/**
	* \brief Compares two strings with case insensitive
	*
	* It returns true if the strings are equal, false otherwise
	* \param sz1 First string
	* \param pc2 Second string
	* \return bool
	*/
	extern KVILIB_API bool equalCI(const QString & sz1, const char * pc2);
	
	/**
	* \brief Compares two strings with case insensitive
	*
	* It returns true if the strings are equal, false otherwise.
	* The parameter pC2 is assumed to be null terminated here!
	* \param sz1 First string
	* \param pC2 Second string
	* \return bool
	*/
	extern KVILIB_API bool equalCI(const QString & sz1, const QChar * pC2);
	
	/**
	* \brief Compares two strings with case insensitive
	*
	* It returns true if the strings are equal, false otherwise.
	* \param pc1 First string
	* \param sz2 Second string
	* \return bool
	*/
	inline bool equalCI(const char * pc1, const QString & sz2)
		{ return equalCI(sz2,pc1); }
	
	/**
	* \brief Compares two strings with case insensitive
	*
	* It returns true if the strings are equal, false otherwise.
	* The parameter pC1 is assumed to be null terminated here!
	* \param pC1 First string
	* \param sz2 Second string
	* \return bool
	*/
	inline bool equalCI(const QChar * pC1, const QString & sz2)
		{ return equalCI(sz2,pC1); }
	
	/**
	* \brief Compares two strings with case sensitive up to N chars
	*
	* It returns true if the strings are equal, false otherwise
	* \param sz1 First string
	* \param sz2 Second string
	* \param uLen The length of the string to check
	* \return bool
	*/
	extern KVILIB_API bool equalCSN(const QString & sz1, const QString & sz2, unsigned int uLen);
	
	/**
	* \brief Compares two strings with case sensitive up to N chars
	*
	* It returns true if the strings are equal, false otherwise
	* \param sz1 First string
	* \param pc2 Second string
	* \param uLen The length of the string to check
	* \return bool
	*/
	extern KVILIB_API bool equalCSN(const QString & sz1, const char * pc2, unsigned int uLen);
	
	/**
	* \brief Compares two strings with case sensitive up to N chars
	*
	* It returns true if the strings are equal, false otherwise
	* \param pc1 First string
	* \param sz2 Second string
	* \param uLen The length of the string to check
	* \return bool
	*/
	inline bool equalCSN(const char * pc1, const QString & sz2, unsigned int uLen)
		{ return equalCSN(sz2,pc1,uLen); }
	
	/**
	* \brief Compares two strings with case insensitive up to N chars
	*
	* It returns true if the strings are equal, false otherwise
	* \param sz1 First string
	* \param sz2 Second string
	* \param uLen The length of the string to check
	* \return bool
	*/
	extern KVILIB_API bool equalCIN(const QString & sz1, const QString & sz2, unsigned int uLen);
	
	/**
	* \brief Compares two strings with case insensitive up to N chars
	*
	* It returns true if the strings are equal, false otherwise
	* \param sz1 First string
	* \param pc2 Second string
	* \param uLen The length of the string to check
	* \return bool
	*/
	extern KVILIB_API bool equalCIN(const QString & sz1, const char * pc2, unsigned int uLen);
	
	/**
	* \brief Compares two strings with case insensitive up to N chars
	*
	* It returns true if the strings are equal, false otherwise
	* pC2 is assumed to be null terminated here!
	* \param sz1 First string
	* \param pC2 Second string
	* \param uLen The length of the string to check
	* \return bool
	*/
	extern KVILIB_API bool equalCIN(const QString & sz1, const QChar * pC2, unsigned int uLen);
	
	/**
	* \brief Compares two strings with case insensitive up to N chars
	*
	* It returns true if the strings are equal, false otherwise
	* \param pc1 First string
	* \param sz2 Second string
	* \param uLen The length of the string to check
	* \return bool
	*/
	inline bool equalCIN(const char * pc1, const QString & sz2, unsigned int uLen)
		{ return equalCIN(sz2,pc1,uLen); }
	
	/**
	* \brief Compares two strings with case insensitive up to N chars
	*
	* It returns true if the strings are equal, false otherwise
	* pC1 is assumed to be null terminated here!
	* \param pC1 First string
	* \param sz2 Second string
	* \param uLen The length of the string to check
	* \return bool
	*/
	inline bool equalCIN(const QChar * pC1, const QString & sz2, unsigned int uLen)
		{ return equalCIN(sz2,pC1,uLen); }

	/**
	* \brief Compares two strings with case insensitive
	*
	* Note that greater here means that come AFTER in the alphabetic order
	* return < 0 ---> str1 < str2
	* return = 0 ---> str1 = str2
	* return > 0 ---> str1 > str2
	* \param sz1 The first string
	* \param sz2 The second string
	* \param bNonAlphaGreater Wheter to check if strings are not alphabetics
	* \return int
	*/
	extern KVILIB_API int cmpCI(const QString & sz1, const QString & sz2, bool bNonAlphaGreater = false);
	
	/**
	* \brief Compares two strings with case insensitive up to N chars
	*
	* Note that greater here means that come AFTER in the alphabetic order
	* return < 0 ---> str1 < str2
	* return = 0 ---> str1 = str2
	* return > 0 ---> str1 > str2
	* \param sz1 The first string
	* \param sz2 The second string
	* \param uLen The number of chars to check
	* \return int
	*/
	extern KVILIB_API int cmpCIN(const QString & sz1, const QString & sz2, unsigned int uLen);
	
	/**
	* \brief Compares two strings with case sensitive
	*
	* Note that greater here means that come AFTER in the alphabetic order
	* return < 0 ---> str1 < str2
	* return = 0 ---> str1 = str2
	* return > 0 ---> str1 > str2
	* \param sz1 The first string
	* \param sz2 The second string
	* \return int
	*/
	extern KVILIB_API int cmpCS(const QString & sz1, const QString & sz2);

	/**
	* \brief Resets the size of the string
	* \param szSrc The source string
	* \return void
	*/
	extern KVILIB_API void detach(QString & szSrc);

	/**
	* \brief Returns a pointer to the data stored in the string.
	*
	* This makes the string szSrc appear as a null terminated array.
	* It MAY RETURN 0 when the string is null!
	* \param szSrc The source string
	* \return QChar *
	*/
	extern KVILIB_API const QChar * nullTerminatedArray(const QString & szSrc);

	/**
	* \brief Returns true if the string ends with the given character
	* \param szSrc The source string
	* \param c The char to check
	* \return bool
	*/
	inline bool lastCharIs(QString & szSrc, const QChar & c)
		{ return szSrc.endsWith(c); }

	/**
	* \brief Ensures the last char of a string is the given char
	*
	* If the string does not end with the given char, it appends it
	* \param szSrc The source string
	* \param c The char to check
	* \return void
	*/
	extern KVILIB_API void ensureLastCharIs(QString & szSrc, const QChar & c);

	/**
	* \brief Matches two strings containging wildcards (* and ?)
	* \param szM1 The first string to match
	* \param szM2 The second string to match
	* \return bool
	*/
	extern KVILIB_API bool matchWildExpressions(const QString & szM1, const QString & szM2);
	
	/**
	* \brief Matches two string containging wildcards (* and ?) or regular expressions
	* \param szExp The regular expression to match
	* \param szStr The source string
	* \param bIsRegExp Wheter to use a wildcard or regexp matching
	* \param bExact Wheter to match the whole string
	* \param bCs Wheter to match with case sensitive
	* \return bool
	*/
	extern KVILIB_API bool matchString(const QString & szExp, const QString & szStr, bool bIsRegExp = false, bool bExact = false, bool bCs = false);

	/**
	* \brief Writes to the character string
	* \param szSrc The source string
	* \param szFmt The format string
	* \param list The list of format parameters
	* \return void
	*/
	extern KVILIB_API void vsprintf(QString & szSrc, const QString & szFmt, kvi_va_list list);
	
	/**
	* \brief Writes to the character string
	* \param szSrc The source string
	* \param szFmt The format string
	* \param ... The list of format parameters
	* \return QString
	*/
	extern KVILIB_API QString & sprintf(QString & szSrc, const QString & szFmt, ...);
	
	/**
	* \brief Trims all the whitespaces at the end of the given string
	* \param szSrc The source string
	* \return void
	*/
	extern KVILIB_API void stripRightWhiteSpace(QString & szSrc);
	
	/**
	* \brief Trims all c chars at the start of the given string
	* \param szSrc The source string
	* \param c The char to trim
	* \return void
	*/
	extern KVILIB_API void stripLeft(QString & szSrc, const QChar & c);
	
	/**
	* \brief Trims all c chars at the end of the given string
	* \param szSrc The source string
	* \param c The char to trim
	* \return void
	*/
	extern KVILIB_API void stripRight(QString & szSrc, const QChar & c);
	
	/**
	* \brief Appends a formatted string
	* \param szSrc The source string
	* \param szFmt The format string
	* \param ... Format string parameters
	* \return void
	*/
	extern KVILIB_API void appendFormatted(QString & szSrc, const QString & szFmt, ...);
	
	/**
	* \brief Appends the given number to the source string
	* \param szSrc The source string
	* \param dReal Number in double format
	* \return void
	*/
	extern KVILIB_API void appendNumber(QString & szSrc, double dReal);
	
	/**
	* \brief Appends the given number to the source string
	* \param szSrc The source string
	* \param iInteger Number in kvi_i64_t format
	* \return void
	*/
	extern KVILIB_API void appendNumber(QString & szSrc, kvi_i64_t iInteger);
	
	/**
	* \brief Appends the given number to the source string
	* \param szSrc The source string
	* \param iInteger Number in int format
	* \return void
	*/
	extern KVILIB_API void appendNumber(QString & szSrc, int iInteger);
	
	/**
	* \brief Appends the given number to the source string
	* \param szSrc The source string
	* \param uInteger Number in unsigned int format
	* \return void
	*/
	extern KVILIB_API void appendNumber(QString & szSrc, unsigned int uInteger);
	
	/**
	* \brief Appends the given number to the source string
	* \param szSrc The source string
	* \param uInteger Number in kvi_i64_t format
	* \return void
	*/
	extern KVILIB_API void appendNumber(QString & szSrc, kvi_u64_t uInteger);

	/**
	* \brief Cuts the string after the first occurrence of the given char
	*
	* If the char is not found, the string is returned as it is.
	* \param szSrc The source string
	* \param c The char to search for
	* \param bIncluded Wheter to include the given char
	* \return void
	*/
	extern KVILIB_API void cutFromFirst(QString & szSrc, const QChar & c, bool bIncluded = true);
	
	/**
	* \brief Cuts the string after the first occurrence of the given char
	*
	* If the char is not found, the string is returned as it is.
	* \param szSrc The source string
	* \param szFind The string to search for
	* \param bIncluded Wheter to include the given string
	* \return void
	*/
	extern KVILIB_API void cutFromFirst(QString & szSrc, const QString & szFind, bool bIncluded = true);
	
	/**
	* \brief Cuts the string after the last occurrence of the given char
	*
	* If the char is not found, the string is returned as it is.
	* \param szSrc The source string
	* \param c The char to search for
	* \param bIncluded Wheter to include the given char
	* \return void
	*/
	extern KVILIB_API void cutFromLast(QString & szSrc, const QChar & c, bool bIncluded = true);
	
	/**
	* \brief Cuts the string after the last occurrence of the given char
	*
	* If the char is not found, the string is returned as it is.
	* \param szSrc The source string
	* \param szFind The string to search for
	* \param bIncluded Wheter to include the given string
	* \return void
	*/
	extern KVILIB_API void cutFromLast(QString & szSrc, const QString & szFind, bool bIncluded = true);
	
	/**
	* \brief Cuts the string until the first occurrence of the given char is found
	*
	* If the char is not found, the string is returned as it is.
	* \param szSrc The source string
	* \param c The char to search for
	* \param bIncluded Wheter to include the given char in the cut
	* \param bClearIfNotFound Wheter to cut the whole string if the char is not found
	* \return void
	*/
	extern KVILIB_API void cutToFirst(QString & szSrc, const QChar & c, bool bIncluded = true, bool bClearIfNotFound = false);
	
	/**
	* \brief Cuts the string until the first occurrence of the given char is found
	*
	* If the char is not found, the string is returned as it is.
	* \param szSrc The source string
	* \param szFind The string to search for
	* \param bIncluded Wheter to include the given string in the cut
	* \param bClearIfNotFound Wheter to cut the whole string if the string is not found
	* \return void
	*/
	extern KVILIB_API void cutToFirst(QString & szSrc, const QString & szFind, bool bIncluded = true, bool bClearIfNotFound = false);
	
	/**
	* \brief Cuts the string until the last occurrence of the given char is found
	*
	* If the char is not found, the string is returned as it is.
	* \param szSrc The source string
	* \param c The char to search for
	* \param bIncluded Wheter to include the given char in the cut
	* \param bClearIfNotFound Wheter to cut the whole string if the char is not found
	* \return void
	*/
	extern KVILIB_API void cutToLast(QString & szSrc, const QChar & c, bool bIncluded = true, bool bClearIfNotFound = false);
	
	/**
	* \brief Cuts the string until the last occurrence of the given char is found
	*
	* If the char is not found, the string is returned as it is.
	* \param szSrc The source string
	* \param szFind The string to search for
	* \param bIncluded Wheter to include the given string in the cut
	* \param bClearIfNotFound Wheter to cut the whole string if the string is not found
	* \return void
	*/
	extern KVILIB_API void cutToLast(QString & szSrc, const QString & szFind, bool bIncluded = true, bool bClearIfNotFound = false);

	/**
	* \brief Returns the string up to the the first occurrence of the given char
	*
	* If the char is not found, the string is returned as it is.
	* \param szSrc The source string
	* \param c The char to search for
	* \param bIncluded Wheter to include the given char in the returned string
	* \param bReturnFullStringIfNotFound True if we want the function to return the whole string if the char isn't found and
	*        false if we want the function to return an empty string in this case.
	* \return void
	*/
	extern KVILIB_API QString leftToFirst(QString & szSrc, const QChar & c, bool bIncluded = true, bool bReturnFullStringIfNotFound = true);
	
	/**
	* \brief Returns the string up to the first occurrence of the given string
	*
	* If the char is not found, the string is returned as it is.
	* \param szSrc The source string
	* \param szFind The string to search for
	* \param bIncluded Wheter to include the given string in the returned string
	* \param bReturnFullStringIfNotFound True if we want the function to return the whole string if the string isn't found and
	*        false if we want the function to return an empty string in this case.
	* \return void
	*/
	extern KVILIB_API QString leftToFirst(QString & szSrc, const QString & szFind, bool bIncluded = true, bool bReturnFullStringIfNotFound = true);
	
	/**
	* \brief Returns the string up to the last occurrence of the given char
	*
	* If the char is not found, the string is returned as it is.
	* \param szSrc The source string
	* \param c The char to search for
	* \param bIncluded Wheter to include the given char in the returned string
	* \param bReturnFullStringIfNotFound True if we want the function to return the whole string if the char isn't found and
	*        false if we want the function to return an empty string in this case.
	* \return void
	*/
	extern KVILIB_API QString leftToLast(QString & szSrc, const QChar & c, bool bIncluded = true, bool bReturnFullStringIfNotFound = true);
	
	/**
	* \brief Returns the string up to the last occurrence of the given string
	*
	* If the char is not found, the string is returned as it is.
	* \param szSrc The source string
	* \param szFind The string to search for
	* \param bIncluded Wheter to include the given string in the returned string
	* \param bReturnFullStringIfNotFound True if we want the function to return the whole string if the string isn't found and
	*        false if we want the function to return an empty string in this case.
	* \return void
	*/
	extern KVILIB_API QString leftToLast(QString & szSrc, const QString & szFind, bool bIncluded = true, bool bReturnFullStringIfNotFound = true);

	/**
	* \brief Returns an ISO-8859-1 upper case string
	* \param szSrc The source string
	* \return QString
	*/
	extern KVILIB_API QString upperISO88591(const QString & szSrc);
	
	/**
	* \brief Returns an ISO-8859-1 lower case string
	* \param szSrc The source string
	* \return QString
	*/
	extern KVILIB_API QString lowerISO88591(const QString & szSrc);
	
	/**
	* \brief Returns a token from a string
	* \param szSrc The source string
	* \param sep The token to find
	* \return QString
	*/
	extern KVILIB_API QString getToken(QString & szSrc, const QChar & sep);

	/**
	* \brief Replaces a string with another
	* \param szSrc The source string
	* \param szToFind The string to replace
	* \param szReplacement The string to replace with
	* \return void
	*/
	extern KVILIB_API void transliterate(QString & szSrc, const QString & szToFind, const QString & szReplacement);

	/**
	* \brief Returns an hexadecimal converted string starting from a buffer
	* \param szRetBuffer The buffer to hold the hexadecimal string
	* \param pcBuffer The buffer containing the string to convert
	* \param uLen The max length of the original string
	*/
	extern KVILIB_API void bufferToHex(QString & szRetBuffer, const unsigned char * pcBuffer, unsigned int uLen);

	/**
	* \brief Returns the index position of the last occurrence of the character
	*
	* The search is made forward starting from index.
	* \param szSrc The source string
	* \param c The character to find
	* \param iIndex The index to start from
	* \param bCs Case sensitive search
	* \return int
	*/
	inline int find(const QString & szSrc, QChar c, int iIndex = 0, bool bCs = true)
	{
		return szSrc.indexOf(c,iIndex,bCs ? Qt::CaseSensitive : Qt::CaseInsensitive);
	}

	/**
	* \brief Returns the index position of the last occurrence of the character
	*
	* The search is made forward starting from index.
	* \param szSrc The source string
	* \param c The character to find
	* \param iIndex The index to start from
	* \param bCs Case sensitive search
	* \return int
	*/
	inline int find(const QString & szSrc, char c, int iIndex = 0, bool bCs = true)
	{
		return szSrc.indexOf(c,iIndex,bCs ? Qt::CaseSensitive : Qt::CaseInsensitive);
	}

	/**
	* \brief Returns the index position of the last occurrence of the string
	*
	* The search is made forward starting from index.
	* \param szSrc The source string
	* \param szStr The string to find
	* \param iIndex The index to start from
	* \param bCs Case sensitive search
	* \return int
	*/
	inline int find(const QString & szSrc, const QString & szStr, int iIndex = 0, bool bCs = true)
	{
		return szSrc.indexOf(szStr,iIndex,bCs ? Qt::CaseSensitive : Qt::CaseInsensitive);
	}

	/**
	* \brief Returns the index position of the last occurrence of the string
	*
	* The search is made forward starting from index.
	* \param szSrc The source string
	* \param pcStr The string to find
	* \param iIndex The index to start from
	* \param bCs Case sensitive search
	* \return int
	*/
	inline int find(const QString & szSrc, const char * pcStr, int iIndex = 0, bool bCs = true)
	{
		return szSrc.indexOf(QString(pcStr),iIndex,bCs ? Qt::CaseSensitive : Qt::CaseInsensitive);
	}

	/**
	* \brief Returns the index position of the last occurrence of the string
	*
	* The search is made forward starting from index.
	* \param szSrc The source string
	* \param rx The regexp to match
	* \param iIndex The index to start from
	* \return int
	*/
	inline int find(const QString & szSrc, const QRegExp & rx, int iIndex = 0)
	{
		return szSrc.indexOf(rx,iIndex);
	}

	/**
	* \brief Returns the index position of the last occurrence of the character
	*
	* The search is made backward.
	* If index is -1 the search starts at the last character.
	* \param szSrc The source string
	* \param c The character to find
	* \param iIndex The index to start from
	* \param bCs Case sensitive search
	* \return int
	*/
	inline int findRev(const QString & szSrc, QChar c, int iIndex = -1, bool bCs = true)
	{
		return szSrc.lastIndexOf(c,iIndex,bCs ? Qt::CaseSensitive : Qt::CaseInsensitive);
	}

	/**
	* \brief Returns the index position of the last occurrence of the character
	*
	* The search is made backward.
	* If index is -1 the search starts at the last character.
	* \param szSrc The source string
	* \param c The character to find
	* \param iIndex The index to start from
	* \param bCs Case sensitive search
	* \return int
	*/
	inline int findRev(const QString & szSrc, char c, int iIndex = -1, bool bCs = true)
	{
		return szSrc.lastIndexOf(c,iIndex,bCs ? Qt::CaseSensitive : Qt::CaseInsensitive);
	}

	/**
	* \brief Returns the index position of the last occurrence of the given string
	*
	* The search is made backward.
	* If index is -1 the search starts at the last character.
	* \param szSrc The source string
	* \param szStr The string to find
	* \param iIndex The index to start from
	* \param bCs Case sensitive search
	* \return int
	*/
	inline int findRev(const QString & szSrc, const QString & szStr, int iIndex = -1, bool bCs = true)
	{
		return szSrc.lastIndexOf(szStr,iIndex,bCs ? Qt::CaseSensitive : Qt::CaseInsensitive);
	}

	/**
	* \brief Returns the index position of the last occurrence of the given string
	*
	* The search is made backward.
	* If index is -1 the search starts at the last character.
	* \param szSrc The source string
	* \param pcStr The string to find
	* \param iIndex The index to start from
	* \param bCs Case sensitive search
	* \return int
	*/
	inline int findRev(const QString & szSrc, const char * pcStr, int iIndex = -1, bool bCs = true)
	{
		return szSrc.lastIndexOf(QString(pcStr),iIndex,bCs ? Qt::CaseSensitive : Qt::CaseInsensitive);
	}

	/**
	* \brief Returns the index position of the last match of the regexp
	*
	* The search is made backward.
	* If index is -1 the search starts at the last character.
	* \param szSrc The source string
	* \param rx The regexp to match
	* \param iIndex The index to start from
	* \return int
	*/
	inline int findRev(const QString & szSrc, const QRegExp & rx, int iIndex = -1)
	{
		return szSrc.lastIndexOf(rx,iIndex);
	}

	/**
	* \brief Return a whitespace-trimmed string
	*
	* Spaces are trimmed at start and end of the string
	* \param szSrc The source string
	* \return QString
	*/
	inline QString trimmed(const QString & szSrc)
	{
		return szSrc.trimmed();
	}

	/**
	* \brief Return a UTF-8 formatted string
	* \param szSrc The source string
	* \return QByteArray
	* \warning: DO NOT USE CONSTRUCTS LIKE char * c = KviQString::toUtf8(something).data();
	* They are dangerous since with many compilers the returned string
	* gets destroyed at the end of the instruction and the c pointer gets
	* thus invalidated.
	* Use
	* QByteArray tmp = KviQString::toUtf8(something);
	* char * c = tmp.data();
	* instead. Yes, I know that it sucks, but it's the only way to
	* transit to Qt 4.x more or less cleanly...
	*/
	inline QByteArray toUtf8(const QString & szSrc)
	{
		return szSrc.toUtf8();
	}

	/**
	* \brief Return the local 8-bit representation of the string
	* \param szSrc The source string
	* \return QByteArray
	*/
	inline QByteArray toLocal8Bit(const QString & szSrc)
	{
		return szSrc.toLocal8Bit();
	}

	/**
	* \brief Return the string converted to a long
	* \param szNumber The source number
	* \param bOk The conversion error handling
	* \return kvi_i64_t
	*/
	inline kvi_i64_t toI64(QString & szNumber, bool * bOk)
	{
#if SYSTEM_SIZE_OF_LONG_INT == 8
		return szNumber.toLong(bOk);
#else
		return szNumber.toLongLong(bOk);
#endif
	}

	/**
	* \brief Return the string converted to an unsigned long
	* \param szNumber The source number
	* \param bOk The conversion error handling
	* \return kvi_u64_t
	*/
	inline kvi_u64_t toU64(QString & szNumber, bool * bOk)
	{
#if SYSTEM_SIZE_OF_LONG_INT == 8
		return szNumber.toULong(bOk);
#else
		return szNumber.toULongLong(bOk);
#endif
	}
}

#endif //_KVI_QSTRING_H_
