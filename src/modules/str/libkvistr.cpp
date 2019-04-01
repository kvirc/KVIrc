//=============================================================================
//
//   File : libkvistr.cpp
//   Creation date : Thu Dec 27 2001 17:13:12 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright ©        2009 Kai Wasserbäch <debian@carbon-project.org>
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

//#warning: FIXME: Incomplete documentation ('seealso', 'example', etc)

#include "KviModule.h"
#include "KviLocale.h"
#include "KviControlCodes.h"
#include "KviQString.h"
#include "kvi_debug.h"
#include "kvi_settings.h"
#include "KviMemory.h"
#include "KviKvsArrayCast.h"
#include "KviOptions.h"

#include <QClipboard>
#include <QRegExp>

#ifdef COMPILE_SSL_SUPPORT
// The current implementation
#include "KviSSL.h"
#include <openssl/evp.h>
#include <openssl/pem.h>

#if OPENSSL_VERSION_NUMBER < 0x10100005L
#define EVP_MD_CTX_new EVP_MD_CTX_create
#define EVP_MD_CTX_free EVP_MD_CTX_destroy
#endif

#else
// The fallback we can always use, but with very limited set of
// functionality.
#include <QCryptographicHash>
#endif

/*
	@doc: str.section
	@type:
		function
	@title:
		$str.section
	@short:
		Returns a section of the string.
	@syntax:
		<string> $str.section(<string_to_split:string>,<separator:string>,<pos_from:int>,<pos_to:int>)
	@description:
		Returns a section of the string.
		The <string_to_split> is treated as a sequence of fields separated by <separator>.[br]
		The returned string consists of the fields from position start <pos_from> to position end <pos_to>.[br]
		Fields are numbered 0, 1, 2, etc., counting from the left, and -1, -2, etc., counting from right to left.[br]
	@examples:
		[example]
		%s = $str.section( "forename**middlename**surname**phone","**", 2, 2 );
		%s is "surname".
		[/example]
*/

static bool str_kvs_fnc_section(KviKvsModuleFunctionCall * c)
{
	QString szString, szSeparator;
	kvs_int_t iPosFrom, iPosTo;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("string_to_split", KVS_PT_STRING, 0, szString)
	KVSM_PARAMETER("separator", KVS_PT_NONEMPTYSTRING, 0, szSeparator)
	KVSM_PARAMETER("pos_from", KVS_PT_INT, 0, iPosFrom)
	KVSM_PARAMETER("pos_to", KVS_PT_INT, 0, iPosTo)
	KVSM_PARAMETERS_END(c)
	c->returnValue()->setString(szString.section(szSeparator, iPosFrom, iPosTo));
	return true;
}

/*
	@doc: str.fromClipboard
	@type:
		function
	@title:
		$str.fromClipboard
	@short:
		Returns a string from clipboard.
	@syntax:
		<string> $str.fromClipboard()
	@description:
		Returns clipboard's contents.
	@examples:
		[example]
		%s = $str.fromClipboard();
		[/example]
	@seealso:
		[cmd]str.toClipboard[/cmd]
*/

static bool str_kvs_fnc_fromclipboard(KviKvsModuleFunctionCall * c)
{
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETERS_END(c)
	QString szString = QApplication::clipboard()->text(QClipboard::Clipboard);
	c->returnValue()->setString(szString);
	return true;
}

/*
	@doc: str.toClipboard
	@type:
		command
	@title:
		str.toClipboard
	@keyterms:
		OS clipboard
	@short:
		Sets the OS clipboard contents
	@syntax:
		str.toClipboard <value:string>
	@description:
		Sets the OS clipboard contents
	@seealso:
		[fnc]$str.fromClipboard[/fnc]
*/

static bool str_kvs_cmd_toClipboard(KviKvsModuleCommandCall * c)
{
	QString szValue;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("value", KVS_PT_STRING, KVS_PF_OPTIONAL, szValue)
	KVSM_PARAMETERS_END(c)
	QApplication::clipboard()->setText(szValue, QClipboard::Clipboard);
	return true;
}

/*
	@doc: str.len
	@type:
		function
	@title:
		$str.len
	@short:
		Returns the length of the given string
	@syntax:
		<uint> $str.len(<data:string>)
	@description:
		Returns the length (that is, number of characters) of the given string.
		This function is internally aliased to [fnc]$str.length[/fnc]() too.
*/

/*
	@doc: str.length
	@type:
		function
	@title:
		$str.length
	@short:
		Returns the length of the given string
	@syntax:
		<uint> $str.length(<data:string>)
	@description:
		Returns the length (that is, number of characters) of the given string.
		This function is internally aliased to [fnc]$str.len[/fnc]() too.
*/
static bool str_kvs_fnc_len(KviKvsModuleFunctionCall * c)
{
	QString szString;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("data", KVS_PT_STRING, 0, szString)
	KVSM_PARAMETERS_END(c)
	c->returnValue()->setInteger(szString.length());
	return true;
}

/*
	@doc: str.lowcase
	@type:
		function
	@title:
		$str.lowcase
	@short:
		Returns the given string with all characters turned to toLower case
	@syntax:
		<string> $str.lowcase(<string_to_convert:string>)
	@description:
		Returns the <string_to_convert> with all characters turned to toLower case.
		Warning: this function uses ISO-8859-1 locale to make the case translation.
		If you want to use a locale aware translation mapping then please
		use localelowcase.
*/

static bool str_kvs_fnc_lowcase(KviKvsModuleFunctionCall * c)
{
	QString szString;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("string_to_convert", KVS_PT_STRING, 0, szString)
	KVSM_PARAMETERS_END(c)
	c->returnValue()->setString(KviQString::lowerISO88591(szString));
	return true;
}

/*
	@doc: str.upcase
	@type:
		function
	@title:
		$str.upcase
	@short:
		Returns the given string with all characters turned to upper case
	@syntax:
		<string> $str.upcase(<string_to_convert:string>)
	@description:
		Returns the given <string_to_convert> with all characters turned to upper case.
		Warning: this function uses ISO-8859-1 locale to make the case translation.
		If you want to use a locale aware translation mapping then please
		use $str.localeupcase.
*/

static bool str_kvs_fnc_upcase(KviKvsModuleFunctionCall * c)
{
	QString szString;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("string_to_convert", KVS_PT_STRING, 0, szString)
	KVSM_PARAMETERS_END(c)
	c->returnValue()->setString(KviQString::upperISO88591(szString));
	return true;
}

/*
	@doc: str.localelowcase
	@type:
		function
	@title:
		$str.localelowcase
	@short:
		Returns the given string with all characters turned to toLower case
	@syntax:
		<string> $str.localelowcase(<string_to_convert:string>)
	@description:
		Returns the given <string_to_convert> with all characters turned to toLower case.
		Warning: this function is locale aware and it may produce unexpected
		results in locales that contain strange exceptions (like Turkish which maps
		i to Y with an accent). For IRC interaction you might prefer using $str.lowcase
*/

static bool str_kvs_fnc_localelowcase(KviKvsModuleFunctionCall * c)
{
	QString szString;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("string_to_convert", KVS_PT_STRING, 0, szString)
	KVSM_PARAMETERS_END(c)
	c->returnValue()->setString(szString.toLower());
	return true;
}

/*
	@doc: str.localeupcase
	@type:
		function
	@title:
		$str.localeupcase
	@short:
		Returns the given string with all characters turned to toUpper case
	@syntax:
		<string> $str.localeupcase(<string_to_convert:string>)
	@description:
		Returns the given <string_to_convert> with all characters turned to toLower case.
		Warning: this function is locale aware and it may produce unexpected
		results in locales that contain strange exceptions (like Turkish which maps
		i to Y with an accent). For IRC interaction you might prefer using $str.upcase
*/

static bool str_kvs_fnc_localeupcase(KviKvsModuleFunctionCall * c)
{
	QString szString;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("string_to_convert", KVS_PT_STRING, 0, szString)
	KVSM_PARAMETERS_END(c)
	c->returnValue()->setString(szString.toUpper());
	return true;
}

/*
	@doc: str.isnumber
	@type:
		function
	@title:
		$str.isnumber
	@short:
		Returns [b]1[/b] if the given string represents a number
	@syntax:
		<bool> $str.isnumber(<givenstring:string>)
	@description:
		Returns [b]1[/b] if the given string represents a number, and [b]0[/b] otherwise.
*/

static bool str_kvs_fnc_isnumber(KviKvsModuleFunctionCall * c)
{
	KviKvsVariant * v;
	KviKvsNumber num;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("givenstring", KVS_PT_VARIANT, 0, v)
	KVSM_PARAMETERS_END(c)
	c->returnValue()->setBoolean(v->asNumber(num));
	return true;
}

/*
	@doc: str.isunsignednumber
	@type:
		function
	@title:
		$str.isunsignednumber
	@short:
		Returns [b]1[/b] if the given string represents an unsigned number
	@syntax:
		<bool> $str.isunsignednumber(<givenstring:string>)
	@description:
		Returns [b]1[/b] if the given string represents an unsigned number, and [b]0[/b] otherwise.
*/

static bool str_kvs_fnc_isunsignednumber(KviKvsModuleFunctionCall * c)
{
	KviKvsVariant * v;
	KviKvsNumber nNum;
	bool bRet;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("givenstring", KVS_PT_VARIANT, 0, v)
	KVSM_PARAMETERS_END(c)
	if(!v->asNumber(nNum))
		bRet = false;
	else
	{
		if(nNum.isInteger())
			bRet = nNum.integer() >= 0;
		else
			bRet = nNum.real() >= 0.0;
	}
	c->returnValue()->setBoolean(bRet);
	return true;
}

/*
	@doc: str.isempty
	@type:
		function
	@title:
		$str.isempty
	@short:
		Returns [b]1[/b] if the given string don't have any character.
	@syntax:
		<string> $str.isempty(<givenstring:string>)
	@description:
		Returns [b]1[/b] if the given string don't have any character (that is, is empty).
		This function is almost useless since it is equivalent to the
		comparison with an empty string...
*/

static bool str_kvs_fnc_isempty(KviKvsModuleFunctionCall * c)
{
	QString szString;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("datastring", KVS_PT_STRING, 0, szString)
	KVSM_PARAMETERS_END(c)
	c->returnValue()->setBoolean(szString.isEmpty());
	return true;
}

/*
	@doc: str.contains
	@type:
		function
	@title:
		$str.contains
	@short:
		Returns [b]1[/b] if the first parameter contains the second
	@syntax:
		<bool> $str.contains(<container:string>,<tofind:string>[,<case:bool>])
	@description:
		Returns [b]1[/b] if the first string parameter contains the second string parameter.
		If the third parameter is set to true, then the search is case sensitive.
	@seealso:
		[fnc]$str.match[/fnc]()
*/

static bool str_kvs_fnc_contains(KviKvsModuleFunctionCall * c)
{
	QString szString, szSubString;
	bool bCase;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("container", KVS_PT_STRING, 0, szString)
	KVSM_PARAMETER("tofind", KVS_PT_STRING, 0, szSubString)
	KVSM_PARAMETER("case", KVS_PT_BOOL, KVS_PF_OPTIONAL, bCase)
	KVSM_PARAMETERS_END(c)
	bool bIs = szString.contains(szSubString, bCase ? Qt::CaseSensitive : Qt::CaseInsensitive);
	c->returnValue()->setBoolean(bIs);
	return true;
}

/*
	@doc: str.equal
	@type:
		function
	@title:
		$str.equal
	@short:
		Returns [b]1[/b] if the two string parameters are equal
	@syntax:
		<bool> $str.equal(<fromcompare:string>,<tocompare:string>[,<case:bool>])
	@description:
		Returns [b]1[/b] if the two string parameters are equal.
		If the third parameter is set to true, then the search is case sensitive.
	@seealso:
		[fnc]$str.match[/fnc]()
*/

static bool str_kvs_fnc_equal(KviKvsModuleFunctionCall * c)
{
	QString szString, szString2;
	bool bCase;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("fromcompare", KVS_PT_STRING, 0, szString)
	KVSM_PARAMETER("tocompare", KVS_PT_STRING, 0, szString2)
	KVSM_PARAMETER("case", KVS_PT_BOOL, KVS_PF_OPTIONAL, bCase)
	KVSM_PARAMETERS_END(c)
	bool bIs = szString.compare(szString2, bCase ? Qt::CaseSensitive : Qt::CaseInsensitive) == 0;
	c->returnValue()->setBoolean(bIs);
	return true;
}

/*
	@doc: str.cmp
	@type:
		function
	@title:
		$str.cmp
	@short:
		Compare two strings alphabetically
	@syntax:
		<bool> $str.cmp(<fromcompare:string>,<tocompare:string>[,<case:bool>])
	@description:
		This function compares two strings alphabetically. If the first string is [b]greater[/b] than the second, it will return a positive number, a negative number is the second is greater and 0 if the two strings are equal.
		If the third parameter is set to true, then the search is case sensitive.
	@seealso:
		[fnc]$str.match[/fnc]()
*/

static bool str_kvs_fnc_cmp(KviKvsModuleFunctionCall * c)
{
	QString szString, szString2;
	bool bCase;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("fromcompare", KVS_PT_STRING, 0, szString)
	KVSM_PARAMETER("tocompare", KVS_PT_STRING, 0, szString2)
	KVSM_PARAMETER("case", KVS_PT_BOOL, KVS_PF_OPTIONAL, bCase)
	KVSM_PARAMETERS_END(c)
	int iCmp = szString.compare(szString2, bCase ? Qt::CaseSensitive : Qt::CaseInsensitive);
	c->returnValue()->setInteger(iCmp);
	return true;
}

/*
	@doc: str.find
	@type:
		function
	@title:
		$str.find
	@short:
		Find the index of the nth occurrence of a substring in a string
	@syntax:
		<int> $str.find(<findIn:string>,<tofind:string>[,<occurrence:int>])
	@description:
		This function search in the string given as the first parameter for the string
		given as his second parameter, and will return the index where the nth occurrence
		given as the third parameter is found or -1 if it's not located. It starts
		counting at 0. If occurrence is not specified then the first occurrence
		is searched.[br]
*/

static bool str_kvs_fnc_find(KviKvsModuleFunctionCall * c)
{
	QString szFindIn, szToFind;
	kvs_int_t iOccurrence;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("findIn", KVS_PT_STRING, 0, szFindIn)
	KVSM_PARAMETER("tofind", KVS_PT_STRING, 0, szToFind)
	KVSM_PARAMETER("occurrence", KVS_PT_INTEGER, KVS_PF_OPTIONAL, iOccurrence)
	KVSM_PARAMETERS_END(c)

	int iIdx = -1;
	if(iOccurrence >= 0 && !szFindIn.isEmpty() && !szToFind.isEmpty())
	{
		do
		{
			iIdx = szFindIn.indexOf(szToFind, iIdx + 1, Qt::CaseInsensitive);
		} while(--iOccurrence >= 0 && iIdx != -1);
	}

	c->returnValue()->setInteger(iIdx);
	return true;
}

/*
	@doc: str.findfirst
	@type:
		function
	@title:
		$str.findfirst
	@short:
		Find the index of a substring in a string
	@syntax:
		<int> $str.findfirst(<findIn:string>,<toFind:string>[,<case:bool>[,<from_index:integer>]])
	@description:
		This function search in the string given as the first parameter for the string given as his second parameter, and will return the index where is first located or -1 if it's not located. It starts counting at 0.
		If the third parameter is set to true, then the search is case sensitive.
*/

static bool str_kvs_fnc_findfirst(KviKvsModuleFunctionCall * c)
{
	QString szString, szString2;
	bool bCase;
	kvs_int_t iFromIndex;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("findIn", KVS_PT_STRING, 0, szString)
	KVSM_PARAMETER("toFind", KVS_PT_STRING, 0, szString2)
	KVSM_PARAMETER("case", KVS_PT_BOOL, KVS_PF_OPTIONAL, bCase)
	KVSM_PARAMETER("from_index", KVS_PT_INTEGER, KVS_PF_OPTIONAL, iFromIndex)
	KVSM_PARAMETERS_END(c)
	int iIdx = szString.indexOf(szString2, iFromIndex, bCase ? Qt::CaseSensitive : Qt::CaseInsensitive);
	c->returnValue()->setInteger(iIdx);
	return true;
}

/*
	@doc: str.findlast
	@type:
		function
	@title:
		$str.findlast
	@short:
		Find the last index of a substring in a string
	@syntax:
		<int> $str.findlast(<findIn:string>,<toFind:string>[,<case:bool>])
	@description:
		This function search in the string given as the first parameter for the string
		given as his second parameter, and will return the index where is last located or -1 if it's not located.
		If the third parameter is set to true, then the search is case sensitive.
*/

static bool str_kvs_fnc_findlast(KviKvsModuleFunctionCall * c)
{
	QString szString, szString2;
	bool bCase;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("findIn", KVS_PT_STRING, 0, szString)
	KVSM_PARAMETER("toFind", KVS_PT_STRING, 0, szString2)
	KVSM_PARAMETER("case", KVS_PT_BOOL, KVS_PF_OPTIONAL, bCase)
	KVSM_PARAMETERS_END(c)
	int iIdx = szString.lastIndexOf(szString2, -1, bCase ? Qt::CaseSensitive : Qt::CaseInsensitive);
	c->returnValue()->setInteger(iIdx);
	return true;
}

/*
	@doc: str.left
	@type:
		function
	@title:
		$str.left
	@short:
		Returns a substring starting from the left until the given index.
	@syntax:
		<string> $str.left(<data:string>,<index:int>)
	@description:
		This function returns a substring of the first string parameter which is the
		string starting from the left until the index specified in the second parameter.
*/

static bool str_kvs_fnc_left(KviKvsModuleFunctionCall * c)
{
	QString szString;
	kvs_int_t iIdx;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("data", KVS_PT_STRING, 0, szString)
	KVSM_PARAMETER("index", KVS_PT_INTEGER, 0, iIdx)
	KVSM_PARAMETERS_END(c)
	c->returnValue()->setString(szString.left(iIdx));
	return true;
}

/*
	@doc: str.right
	@type:
		function
	@title:
		$str.right
	@short:
		Returns a substring starting from the right until the given index.
	@syntax:
		<string> $str.right(<data:string>,<index:int>)
	@description:
		This function returns a substring of the first string parameter which is the
		string starting from the right until the index specified in the second parameter.
		The index start counting at the last character and increase until the first.
*/

static bool str_kvs_fnc_right(KviKvsModuleFunctionCall * c)
{
	QString szString;
	kvs_int_t iIdx;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("data", KVS_PT_STRING, 0, szString)
	KVSM_PARAMETER("index", KVS_PT_INTEGER, 0, iIdx)
	KVSM_PARAMETERS_END(c)
	c->returnValue()->setString(szString.right(iIdx));
	return true;
}

/*
	@doc: str.mid
	@type:
		function
	@title:
		$str.mid
	@short:
		Returns a substring starting from a given index.
	@syntax:
		<string> $str.mid(<data:string>,<startidx:int>[,<nchars:int>])
	@description:
		This function returns a substring of the first string parameter which is the
		string starting at the (numeric) index given with startidx and counting nchars
		forward. If <nchars> is not given or is less than 1, all the characters until
		the end of the string will be returned.
*/

static bool str_kvs_fnc_mid(KviKvsModuleFunctionCall * c)
{
	QString szString;
	kvs_int_t iIdx, iNchars;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("data", KVS_PT_STRING, 0, szString)
	KVSM_PARAMETER("startidx", KVS_PT_INTEGER, 0, iIdx)
	KVSM_PARAMETER("nchars", KVS_PT_INTEGER, KVS_PF_OPTIONAL, iNchars)
	KVSM_PARAMETERS_END(c)
	c->returnValue()->setString(szString.mid(iIdx, (iNchars > 0 ? iNchars : -1)));
	return true;
}

/*
	@doc: str.append
	@type:
		function
	@title:
		$str.append
	@short:
		Append one string to another.
	@syntax:
		<string> $str.append(<string:string>,<toappend:string>)
	@description:
		This function returns a string created appending the second string parameter
		to the end of the first string parameter.
*/

static bool str_kvs_fnc_append(KviKvsModuleFunctionCall * c)
{
	QString szString, szString2;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("string", KVS_PT_STRING, 0, szString)
	KVSM_PARAMETER("toappend", KVS_PT_STRING, 0, szString2)
	KVSM_PARAMETERS_END(c)
	c->returnValue()->setString(szString.append(szString2));
	return true;
}

/*
	@doc: str.prepend
	@type:
		function
	@title:
		$str.prepend
	@short:
		Prepend one string to another.
	@syntax:
		<string> $str.prepend(<string:string>,<toprepend:string>)
	@description:
		This function returns a string created prepending the second string parameter
		to the start of the first string parameter.
*/

static bool str_kvs_fnc_prepend(KviKvsModuleFunctionCall * c)
{
	QString szString, szString2;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("string", KVS_PT_STRING, 0, szString)
	KVSM_PARAMETER("toprepend", KVS_PT_STRING, 0, szString2)
	KVSM_PARAMETERS_END(c)
	c->returnValue()->setString(szString.prepend(szString2));
	return true;
}

/*
	@doc: str.insert
	@type:
		function
	@title:
		$str.insert
	@short:
		Inserts a substring in a string at a given index
	@syntax:
		<string> $str.insert(<string:string>,<substring:string>,<index:int>)
	@description:
		Inserts the substring given in the second parameter in the string given in the
		first parameter at the index given in the third parameter, then returns the
		resulting string.
*/

static bool str_kvs_fnc_insert(KviKvsModuleFunctionCall * c)
{
	QString szString, szString2;
	kvs_int_t iIdx;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("string", KVS_PT_STRING, 0, szString)
	KVSM_PARAMETER("substring", KVS_PT_STRING, 0, szString2)
	KVSM_PARAMETER("nchars", KVS_PT_INTEGER, 0, iIdx)
	KVSM_PARAMETERS_END(c)
	c->returnValue()->setString(szString.insert(iIdx, szString2));
	return true;
}

/*
	@doc: str.strip
	@type:
		function
	@title:
		$str.strip
	@short:
		Returns a whitespace stripped string
	@syntax:
		<string> $str.strip(<string:string>)
	@description:
		Returns a left and right whitespace stripped version of the string given as the
		first parameter.
*/

static bool str_kvs_fnc_strip(KviKvsModuleFunctionCall * c)
{
	QString szString;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("string", KVS_PT_STRING, 0, szString)
	KVSM_PARAMETERS_END(c)
	c->returnValue()->setString(szString.trimmed());
	return true;
}

/*
	@doc: str.stripleft
	@type:
		function
	@title:
		$str.stripleft
	@short:
		Returns a left whitespace stripped string
	@syntax:
		<string> $str.stripleft(<string:string>)
	@description:
		Returns a left whitespace stripped version of the string given as the
		first parameter.
*/

static bool str_kvs_fnc_stripleft(KviKvsModuleFunctionCall * c)
{
	QString szString;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("string", KVS_PT_STRING, 0, szString)
	KVSM_PARAMETERS_END(c)
	int idx = 0;
	while(idx < szString.length() && szString[idx].isSpace())
		++idx;
	if(idx > 0)
		szString.remove(0, idx);
	c->returnValue()->setString(szString);
	return true;
}

/*
	@doc: str.stripright
	@type:
		function
	@title:
		$str.stripright
	@short:
		Returns a right whitespace stripped string
	@syntax:
		<string> $str.stripright(<string:string>)
	@description:
		Returns a right whitespace stripped version of the string given as the
		first parameter.
*/

static bool str_kvs_fnc_stripright(KviKvsModuleFunctionCall * c)
{
	QString szString;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("string", KVS_PT_STRING, 0, szString)
	KVSM_PARAMETERS_END(c)
	int iIdx = szString.length();
	while(iIdx > 0 && szString[iIdx - 1].isSpace())
		--iIdx;
	if(iIdx < szString.length())
		szString.chop(szString.length() - iIdx);
	c->returnValue()->setString(szString);
	return true;
}

/*
	@doc: str.stripcolors
	@type:
		function
	@title:
		$str.stripcolors
	@short:
		Returns a mIRC color codes stripped string
	@syntax:
		<string> $str.stripcolors(<string:string>)
	@description:
		Removes all mIRC color codes from a string, including also bold, underline, reverse,
		icon, crypting and ctcp control codes.
*/

static bool str_kvs_fnc_stripcolors(KviKvsModuleFunctionCall * c)
{
	QString szString;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("string", KVS_PT_STRING, 0, szString)
	KVSM_PARAMETERS_END(c)
	c->returnValue()->setString(KviControlCodes::stripControlBytes(szString));
	return true;
}

/*
	@doc: str.replace
	@type:
		function
	@title:
		$str.replace
	@short:
		Replace substrings in a string
	@syntax:
		<string> $str.replace(<string:string>,<toreplace:string>,<replacewith:string>[,<case:bool>])
	@description:
		This function returns a string created replacing all occurrences of the second parameter
		('toreplace') in the string given as the first parameter ('string') with the string
		given as the third parameter ('replacewith').
		If the fourth parameter is set to true, then the string replacement is case sensitive.
	@examples:
		[example]
			echo $str.replace("I like big networks","big","neural")
		[/example]
*/

static bool str_kvs_fnc_replace(KviKvsModuleFunctionCall * c)
{
	QString szString, szNewStr, szToReplace;
	bool bCase;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("string", KVS_PT_STRING, 0, szString)
	KVSM_PARAMETER("toreplace", KVS_PT_STRING, 0, szToReplace)
	KVSM_PARAMETER("newstr", KVS_PT_STRING, 0, szNewStr)
	KVSM_PARAMETER("case", KVS_PT_BOOL, KVS_PF_OPTIONAL, bCase)
	KVSM_PARAMETERS_END(c)
	szString.replace(szToReplace, szNewStr, bCase ? Qt::CaseSensitive : Qt::CaseInsensitive);
	c->returnValue()->setString(szString);
	return true;
}

/*
	@doc: str.urlencode
	@type:
		function
	@title:
		$str.urlencode
	@short:
		Returns a browser formatted string
	@syntax:
		<string> $str.urlencode(<string:string>)
	@description:
		This function returns a string created replacing all occurrences in the parameter ('string') with their respective html entities.[br]
		The replacement is case insensitive.
*/

static bool str_kvs_fnc_urlencode(KviKvsModuleFunctionCall * c)
{
	QString szString;
	unsigned int idx = 0;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("string", KVS_PT_STRING, 0, szString)
	KVSM_PARAMETERS_END(c)

	const char * const toReplace[] = {
		" ", "#", "$", "/", ":",
		"<", "=", ">", "?", "@",
		"[", "\\", "]", "^", "`",
		"{", "|", "}", nullptr
	};

	const char * const newStr[] = {
		"%20", "%23", "%24", "%2F", "%3A",
		"%3C", "%3D", "%3E", "%3F", "%40",
		"%5B", "%5C", "%5D", "%5E", "%60",
		"%7B", "%7C", "%7D", nullptr
	};

	while(toReplace[idx])
	{
		szString.replace(toReplace[idx], newStr[idx], Qt::CaseInsensitive);
		idx++;
	}

	c->returnValue()->setString(szString);
	return true;
}

/*
	@doc: str.lefttofirst
	@type:
		function
	@title:
		$str.lefttofirst
	@short:
		Returns the left part of a string until a given substring
	@syntax:
		<string> $str.lefttofirst(<string:string>,<substring:string>[,<case:bool>])
	@description:
		This function returns the left part of the string given as the first parameter
		from the first character until the first occurrence of the string given as the second parameter.
		The returned string doesn't include the substring of the second parameter in the returned value.
		If the substring is not found, the original string is returned.
		If the third parameter is set to true, then the search is case sensitive; it defaults to false.
	@examples:
		[example]
			%test = "Hello! My nickname is Pragma, my name is Szymon"
			echo $str.lefttofirst(%test, my);	//  "Hello! "
			echo $str.lefttofirst(%test, my, true);	//  "Hello! My nickname is Pragma, "
			echo $str.lefttofirst(%test, invalid);	//  "Hello! My nickname is Pragma, my name is Szymon"
		[/example]
*/

static bool str_kvs_fnc_lefttofirst(KviKvsModuleFunctionCall * c)
{
	QString szString, szNewstr;
	bool bCase;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("string", KVS_PT_STRING, 0, szString)
	KVSM_PARAMETER("substring", KVS_PT_STRING, 0, szNewstr)
	KVSM_PARAMETER("case", KVS_PT_BOOL, KVS_PF_OPTIONAL, bCase)
	KVSM_PARAMETERS_END(c)
	int idx = szString.indexOf(szNewstr, 0, bCase ? Qt::CaseSensitive : Qt::CaseInsensitive);
	if(idx != -1)
		c->returnValue()->setString(szString.left(idx));
	else
		c->returnValue()->setString(szString);
	return true;
}

/*
	@doc: str.lefttolast
	@type:
		function
	@title:
		$str.lefttolast
	@short:
		Returns the left part of a string until the last occurrence of a given substring
	@syntax:
		<string> $str.lefttolast(<string:string>,<substring:string>[,<case:bool>])
	@description:
		This function returns the left part of the string given as the first parameter
		from the first character until the last occurrence of the string given as the second parameter.
		The returned string doesn't include the substring of the second parameter in the returned value.
		If the substring is not found, the original string is returned.
		If the third parameter is set to true, then the search is case sensitive; it defaults to false.
	@examples:
		[example]
			%test = "Hello! My nickname is Pragma, my name is Szymon"
			echo $str.lefttolast(%test, My);		//  "Hello! My nickname is Pragma, "
			echo $str.lefttolast(%test, My, true);	//  "Hello! "
			echo $str.lefttolast(%test, invalid);	//  "Hello! My nickname is Pragma, my name is Szymon"
		[/example]
*/

static bool str_kvs_fnc_lefttolast(KviKvsModuleFunctionCall * c)
{
	QString szString, szNewstr;
	bool bCase;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("string", KVS_PT_STRING, 0, szString)
	KVSM_PARAMETER("substring", KVS_PT_STRING, 0, szNewstr)
	KVSM_PARAMETER("case", KVS_PT_BOOL, KVS_PF_OPTIONAL, bCase)
	KVSM_PARAMETERS_END(c)
	int idx = szString.lastIndexOf(szNewstr, -1, bCase ? Qt::CaseSensitive : Qt::CaseInsensitive);
	if(idx != -1)
		c->returnValue()->setString(szString.left(idx));
	else
		c->returnValue()->setString(szString);
	return true;
}

/*
	@doc: str.rightfromfirst
	@type:
		function
	@title:
		$str.rightfromfirst
	@short:
		Returns the right part of a string from the first occurrence of a given substring
	@syntax:
		<string> $str.rightfromfirst(<string:string>,<substring:string>[,<case:bool>])
	@description:
		This function returns the right part of the string given as the first parameter
		from the position where the first occurrence of the string given as the second parameter
		is found to the end. It doesn't include the substring of the second parameter in the returned value.
		If the substring is not found, an empty string is returned.
		If the third parameter is set to true, then the search is case sensitive; it defaults to false.
	@examples:
		[example]
			%test = "Hello! My nickname is Pragma, my name is Szymon"
			echo $str.rightfromfirst(%test, my);		//  " nickname is Pragma, my name is Szymon"
			echo $str.rightfromfirst(%test, my, true);	//  " name is Szymon"
			echo $str.rightfromfirst(%test, invalid);	//  ""
		[/example]
*/

static bool str_kvs_fnc_rightfromfirst(KviKvsModuleFunctionCall * c)
{
	QString szString, szNewstr;
	bool bCase;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("string", KVS_PT_STRING, 0, szString)
	KVSM_PARAMETER("substring", KVS_PT_STRING, 0, szNewstr)
	KVSM_PARAMETER("case", KVS_PT_BOOL, KVS_PF_OPTIONAL, bCase)
	KVSM_PARAMETERS_END(c)
	int idx = szString.indexOf(szNewstr, 0, bCase ? Qt::CaseSensitive : Qt::CaseInsensitive);
	if(idx != -1)
		c->returnValue()->setString(szString.right(szString.length() - (idx + szNewstr.length())));
	else
		c->returnValue()->setString("");
	return true;
}

/*
	@doc: str.rightfromlast
	@type:
		function
	@title:
		$str.rightfromlast
	@short:
		Returns the right part of a string from the last occurrence of a given substring
	@syntax:
		<string> $str.rightfromlast(<string:string>,<substring:string>[,<case:bool>])
	@description:
		This function returns the right part of the string given as the first parameter
		from the position where the last occurrence of the string given as the second parameter
		is found. It doesn't include the substring of the second parameter in the returned value.
		If the substring is not found, an empty string is returned.
		If the third parameter is set to true, then the search is case sensitive; it defaults to false.
		@examples:
		[example]
			%test = "Hello! My nickname is Pragma, my name is Szymon"
			echo $str.rightfromlast(%test, My);		//  " name is Szymon"
			echo $str.rightfromlast(%test, My, true);	//  " nickname is Pragma, my name is Szymon"
			echo $str.rightfromlast(%test, invalid);	//  ""
		[/example]
*/

static bool str_kvs_fnc_rightfromlast(KviKvsModuleFunctionCall * c)
{
	QString szString, szNewstr;
	bool bCase;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("string", KVS_PT_STRING, 0, szString)
	KVSM_PARAMETER("substring", KVS_PT_STRING, 0, szNewstr)
	KVSM_PARAMETER("case", KVS_PT_BOOL, KVS_PF_OPTIONAL, bCase)
	KVSM_PARAMETERS_END(c)
	int idx = szString.lastIndexOf(szNewstr, -1, bCase ? Qt::CaseSensitive : Qt::CaseInsensitive);
	if(idx != -1)
		c->returnValue()->setString(szString.right(szString.length() - (idx + szNewstr.length())));
	else
		c->returnValue()->setString("");
	return true;
}

/*
	@doc: str.match
	@type:
		function
	@title:
		$str.match
	@short:
		Matches a fixed string against a wildcard expression
	@syntax:
		<bool> $str.match(<expression:string>,<string:string>[,<flags:string>[,<case:bool>]])
	@description:
		Returns [b]1[/b] if the fixed <string> matches the <expression>, and [b]0[/b] otherwise.[br]
		If <flags> contains the flag [b]r[/b] then <expression> is treated as a full
		regular expression otherwise it is treated as a simple wildcard expression containing
		the classic wildcards [b]*[/b] and [b]?[/b].[br]
		If <flags> contains the flag [b]e[/b] then only an exact match is considered (e.g. the full
		<string> is exactly matched by <expression>), otherwise partial matches are allowed too (e.g.
		<expression> is found inside <string>).[br]
		If the third parameter is set to true, then the match is case sensitive.[br]
	@examples:
		[example]
			%test = "Hello! My nickname is Pragma"
			[cmd]if[/cmd]($str.match(Pragma*,%test))[cmd]echo[/cmd] "Matches Pragma*"
			[cmd]if[/cmd]($str.match(*Pragma,%test))[cmd]echo[/cmd] "Matches *Pragma"
			[cmd]if[/cmd]($str.match(H*y*i?k*a,%test))[cmd]echo[/cmd] "Matches H*y*i?k*a"
			[cmd]if[/cmd]($str.match(H*y*i?K*a,%test))[cmd]echo[/cmd] "Matches H*y*i?K*a"
		[/example]
	@seealso:
		[fnc]$str.contains[/fnc](),[fnc]$str.equal[/fnc]()
*/

static bool str_kvs_fnc_match(KviKvsModuleFunctionCall * c)
{
	QString szWildcard, szString, szFlags;
	bool bCase;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("wildcard", KVS_PT_NONEMPTYSTRING, 0, szWildcard)
	KVSM_PARAMETER("string", KVS_PT_STRING, 0, szString)
	KVSM_PARAMETER("flags", KVS_PT_STRING, KVS_PF_OPTIONAL, szFlags)
	KVSM_PARAMETER("case", KVS_PT_BOOL, KVS_PF_OPTIONAL, bCase)
	KVSM_PARAMETERS_END(c)
	bool bRegExp = szFlags.contains('r', Qt::CaseInsensitive);
	bool bExact  = szFlags.contains('e', Qt::CaseInsensitive);
	bool bIs = KviQString::matchString(szWildcard, szString, bRegExp, bExact, bCase);
	c->returnValue()->setBoolean(bIs);
	return true;
}

/*
	@doc: str.word
	@type:
		function
	@title:
		$str.word
	@short:
		Returns the nth word in a string
	@syntax:
		<string> $str.word(<n:int>,<string:string>)
	@description:
		Returns the nth word inside the <string> (with n starting from 0!)[br]
		A word is a substring not containing spaces (ASCII chars 32, carriage returns, tabs etc.).[br]
		If the string contains less than n+1 words then an empty string is returned.[br]
		This function is faster than a call to [fnc]$str.split[/fnc]() and array indexing
		if you need a single word to be extracted from a complex string.[br]
		If you need to manage more than one word from the string then the [fnc]$str.split[/fnc]()
		method is more efficient.[br]
		This function is a special case of [fnc]$str.token[/fnc]() and it runs a bit faster.
	@examples:
		[example]
			%test = "this is  a  string   full of  words"
			[cmd]echo[/cmd] $str.word(4,%test)
		[/example]
	@seealso:
		[fnc]$str.token[/fnc]
*/

static bool str_kvs_fnc_word(KviKvsModuleFunctionCall * c)
{
	QString szString;
	kvs_int_t iOccurence;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("occurrence", KVS_PT_INT, 0, iOccurence)
	KVSM_PARAMETER("string", KVS_PT_STRING, 0, szString)
	KVSM_PARAMETERS_END(c)
	int idx = 0;
	int cnt = 0;
	int len = szString.length();
	while(idx < len)
	{
		QChar szTmp = szString[idx].unicode();
		while(szTmp.isSpace())
		{
			idx++;
			szTmp = szString[idx].unicode();
		}
		int begin = idx;
		while(idx < len && !szTmp.isSpace())
		{
			idx++;
			szTmp = szString[idx].unicode();
		}
		if(iOccurence == (kvs_int_t)cnt)
		{
			c->returnValue()->setString(szString.mid(begin, idx - begin));
			return true;
		}
		cnt++;
	}
	return true;
}

/*
	@doc: str.token
	@type:
		function
	@title:
		$str.token
	@short:
		Returns the nth token in a string
	@syntax:
		<string> $str.token(<n:int>,<separator:string>,<string:string>)
	@description:
		Returns the nth token inside the <string> (with n starting from 0!)[br]
		A token is a substring not containing the characters listed in <separators>.[br]
		If the string contains less than n+1 tokens then an empty string is returned.[br]
		[fnc]$str.word[/fnc]() is an optimized function dedicated to the special case
		in that <separators> is a string containing all the whitespace characters.[br]
		Please note that this function is SLOW. You might want to take a look at [fnc]$str.word[/fnc]()
		or even better to [fnc]$str.split[/fnc]().[br]
		This function is case sensitive: you need to specify both cases in the <separators> string
		if you want to do a case insensitive tokenization.[br]
	@examples:
		[example]
			%test = "21 hours 10 minutes 15 seconds"
			[cmd]echo[/cmd] $str.token(1," hoursmintecd",%test)
		[/example]
	@seealso:
		[fnc]$str.word[/fnc][br]
		[fnc]$str.split[/fnc][br]
*/

static bool str_kvs_fnc_token(KviKvsModuleFunctionCall * c)
{
	QString szString, sep;
	kvs_uint_t n;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("n", KVS_PT_UINT, 0, n)
	KVSM_PARAMETER("separator", KVS_PT_STRING, 0, sep)
	KVSM_PARAMETER("string", KVS_PT_STRING, 0, szString)
	KVSM_PARAMETERS_END(c)
	if(sep.isEmpty())
	{
		c->returnValue()->setString(szString);
		return true;
	}

	int idx = 0;
	unsigned int cnt = 0;
	int len = szString.length();
	while(idx < len)
	{
		QChar szTmp = szString[idx].unicode();
		while(sep.contains(szTmp))
		{
			idx++;
			szTmp = szString[idx].unicode();
		}
		int begin = idx;
		while(idx < len && !sep.contains(szTmp))
		{
			idx++;
			szTmp = szString[idx].unicode();
		}
		if(n == cnt)
		{
			c->returnValue()->setString(szString.mid(begin, idx - begin));
			return true;
		}
		cnt++;
	}
	return true;
}

/*
	@doc: str.charsum
	@type:
		function
	@title:
		$str.charsum
	@short:
		Returns the sum of the character codes of the string
	@syntax:
		<int> $str.charsum(<data:string>[,<bCaseInsensitive:boolean>])
	@description:
		Returns the sum of the character codes of the parameter <string>.
		The sum is suitable for implementing a simple hashing algorithm.[br]
		If <bCaseInsensitive> is specified and $true then the string
		will be converted to toLowercase first.
*/

static bool str_kvs_fnc_charsum(KviKvsModuleFunctionCall * c)
{
	QString szString;
	bool bCaseInsensitive;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("data", KVS_PT_STRING, 0, szString)
	KVSM_PARAMETER("bCaseInsensitive", KVS_PT_BOOL, KVS_PF_OPTIONAL, bCaseInsensitive)
	KVSM_PARAMETERS_END(c)
	unsigned int sum = 0;
	int idx = 0;
	int len = szString.length();
	if(bCaseInsensitive)
	{
		while(idx < len)
		{
			sum += szString[idx].toLower().unicode();
			idx++;
		}
	}
	else
	{
		while(idx < len)
		{
			sum += szString[idx].unicode();
			idx++;
		}
	}
	c->returnValue()->setInteger(sum);
	return true;
}

/*
	@doc: str.chop
	@type:
		function
	@title:
		$str.chop
	@short:
		Returns a string removing n characters from the end of it.
	@syntax:
		<string> $str.chop(<data:string>,<n:integer>)
	@description:
		Returns a string removing n characters from the end of it.
*/

static bool str_kvs_fnc_chop(KviKvsModuleFunctionCall * c)
{
	QString szString, sep;
	kvs_uint_t n;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("data", KVS_PT_NONEMPTYSTRING, 0, szString)
	KVSM_PARAMETER("n", KVS_PT_UINT, 0, n)
	KVSM_PARAMETERS_END(c)
	szString.chop(n);
	c->returnValue()->setString(szString);
	return true;
}

/*
	@doc: str.digest
	@type:
		function
	@title:
		$str.digest
	@short:
		Returns the sum of the character codes of the string
	@syntax:
		<string> $str.digest(<data:string>[,<algorithm:string>])
	@description:
		Calculates digest for given string using algorithm passed as 2nd argument.
		Currently supported: md5, md4, md2, sha1, mdc2, ripemd160, dss1
		Default is md5. Requires OpenSSL support, but offers a minimal set of hashes in any case.
*/

static bool str_kvs_fnc_digest(KviKvsModuleFunctionCall * c)
{
	QString szString, szType, szResult;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("data", KVS_PT_NONEMPTYSTRING, 0, szString)
	KVSM_PARAMETER("algorithm", KVS_PT_NONEMPTYSTRING, KVS_PF_OPTIONAL, szType)
	KVSM_PARAMETERS_END(c)

#if defined(COMPILE_SSL_SUPPORT)
	if(szType.isEmpty())
		szType = "md5";

	EVP_MD_CTX *mdctx;
	const EVP_MD * md;
	unsigned char md_value[EVP_MAX_MD_SIZE];
	unsigned int md_len, i;
	char buff[3];
	OpenSSL_add_all_digests();

	md = EVP_get_digestbyname(szType.toUtf8().data());
	if(!md)
	{
		c->warning(__tr2qs("%Q algorithm is not supported"), &szType);
		return true;
	}

	mdctx = EVP_MD_CTX_new();
	EVP_DigestInit_ex(mdctx, md, nullptr);
	EVP_DigestUpdate(mdctx, szString.toUtf8().data(), szString.toUtf8().length());
	EVP_DigestFinal_ex(mdctx, md_value, &md_len);
	EVP_MD_CTX_free(mdctx);

	for(i = 0; i < md_len; i++)
	{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		_snprintf(buff, 3, "%02x", md_value[i]);
#else
		snprintf(buff, 3, "%02x", md_value[i]);
#endif
		szResult.append(buff);
	}

	c->returnValue()->setString(szResult);
#else // fall back to QCryptographicHash
	QCryptographicHash::Algorithm qAlgo;
	if(szType.toLower() == "sha1")
	{
		qAlgo = QCryptographicHash::Sha1;
	}
	else if(szType.toLower() == "md4")
	{
		qAlgo = QCryptographicHash::Md4;
	}
	else if(szType.toLower() == "md5")
	{
		qAlgo = QCryptographicHash::Md5;
	}
	else
	{
		c->warning(__tr2qs("KVIrc is compiled without Crypto++ or SSL support. $str.digest supports only MD4, MD5 and SHA1."));
		return true;
	}

	c->returnValue()->setString(QString(QCryptographicHash::hash(szString.toLocal8Bit(), qAlgo).toHex()).toUpper());
#endif

	return true;
}

/*
	@doc: str.join
	@type:
		function
	@title:
		$str.join
	@short:
		Returns a string joined from several strings
	@syntax:
		<string> $str.join(<separator:string>,<data:array>[,<flags:string>])
	@description:
		Joins all the string in the <data> array by using
		the specified <separator> and returns the result.
		If <flags> contains the character [b][i]n[/i][/b] then empty strings in the <data>
		array are skipped.

*/

static bool str_kvs_fnc_join(KviKvsModuleFunctionCall * c)
{
	QString szSep;
	KviKvsArrayCast ac;
	QString szFlags;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("separator", KVS_PT_STRING, 0, szSep)
	KVSM_PARAMETER("data", KVS_PT_ARRAYCAST, 0, ac)
	KVSM_PARAMETER("flags", KVS_PT_STRING, KVS_PF_OPTIONAL, szFlags)
	KVSM_PARAMETERS_END(c)

	QString szRet;
	bool bSkipEmpty = szFlags.contains('n', Qt::CaseInsensitive);

	bool bFirst = true;

	if(KviKvsArray * a = ac.array())
	{
		kvs_uint_t uIdx = 0;
		kvs_uint_t uSize = a->size();
		while(uIdx < uSize)
		{
			KviKvsVariant * v = a->at(uIdx);
			if(v)
			{
				QString tmp;
				v->asString(tmp);
				if(bFirst)
				{
					szRet = tmp;
					bFirst = false;
				}
				else
				{
					szRet += szSep;
					szRet += tmp;
				}
			}
			else
			{
				if(!bSkipEmpty)
				{
					if(bFirst)
					{
						bFirst = false;
					}
					else
					{
						szRet += szSep;
					}
				}
			}
			uIdx++;
		}
	}

	c->returnValue()->setString(szRet);
	return true;
}

/*
	@doc: str.grep
	@type:
		function
	@title:
		$str.grep
	@short:
		Performs a search in an array of strings via regular expression matching
	@syntax:
		<array> $str.grep(<match:string>,<strings:array>[,<flags:string>,<offset:integer>])
	@description:
		Returns an array with the elements of <strings> which match the string <match>.
		<flags> can be any combination of the characters [b]s[/b], [b]w[/b] and [b]r[/b].[br]
		If the  flag [b]w[/b] is specified then <match> is assumed to be a wildcard regular
		expression (with [b]*[/b] and [b]?[/b] wildcards). If the flag [b]r[/b] is specified
		then <match> is assumed to be a standard regular expression. If none of
		[b]w[/b] and [b]r[/b] is specified then <match> is treated as a simple string to be
		searched in each element of the <strings> array. [b]r[/b] takes precedence over [b]w[/b].
		If the flag [b]s[/b] is specified the matches are case sensitive.[br]
		If the offset is specified attempts to find a match in from position offset in every array's item. [br]
		If offset is -1, the search starts at the last character; if -2, at the next to last character; etc. [br]
		Note that since almost any other variable type can be automatically cast
		to an array, you can also use this function on scalars or hashes.
	@examples:
		[example]
			[comment]# Find all the nicknames starting with the letter A or a[/comment]
			[cmd]echo[/cmd] $str.grep("^a",[fnc]$chan.users[/fnc],"r")
			[comment]# Find the current CPU speed (on UNIX like machines only)[/comment]
			[cmd]echo[/cmd] $str.grep("MHz",[fnc]$str.split[/fnc]([fnc]$lf[/fnc],[fnc]$file.read[/fnc]("/proc/cpuinfo")))
			[comment]# simply check if the specified string matches a regular expression[/comment]
			[comment]# (this in fact is a little tricky, but you will probably not notice it :D)[/comment]
			[cmd]if[/cmd]($str.grep("[st]+","test string","r"))[cmd]echo[/cmd] "Yeah, it matches!"
		[/example]
	@seealso:
		[fnc]$array[/fnc]
*/

static bool str_kvs_fnc_grep(KviKvsModuleFunctionCall * c)
{
	KviKvsArrayCast ac;
	QString szMatch, szFlags;
	kvs_int_t iOffset;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("match", KVS_PT_STRING, 0, szMatch)
	KVSM_PARAMETER("strings", KVS_PT_ARRAYCAST, 0, ac)
	KVSM_PARAMETER("flags", KVS_PT_STRING, KVS_PF_OPTIONAL, szFlags)
	KVSM_PARAMETER("offset", KVS_PT_INTEGER, KVS_PF_OPTIONAL, iOffset)
	KVSM_PARAMETERS_END(c)

	KviKvsArray * n = new KviKvsArray();
	c->returnValue()->setArray(n);

	KviKvsArray * a = ac.array();

	bool bCaseSensitive = szFlags.contains('s', Qt::CaseInsensitive);
	bool bRegexp = szFlags.contains('r', Qt::CaseInsensitive);
	bool bWild = szFlags.contains('w', Qt::CaseInsensitive);

	// FIXME: The sub pattern matching does not belong to grep.
	// FIXME: DO NOT DOCUMENT FLAGS p and x (they should be removed)
	// 2015.08.24: Left for compatibility: remove in some years :)
	bool bSubPatterns = szFlags.contains('p', Qt::CaseInsensitive);
	bool bExcludeCompleteMatch = szFlags.contains('x', Qt::CaseInsensitive);
	// 2015.08.24: End of "left for compatibility": remove in some years :)

	int idx = 0;
	int cnt = a->size();

	int i = 0;
	if(bRegexp || bWild)
	{
		QRegExp re(szMatch, bCaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive, bRegexp ? QRegExp::RegExp2 : QRegExp::Wildcard);
		while(idx < cnt)
		{
			KviKvsVariant * v = a->at(idx);
			if(v)
			{
				QString sz;
				v->asString(sz);
				int index = re.indexIn(sz, iOffset);
				if(index != -1)
				{
					// FIXME: The sub pattern matching does not belong to grep.
					// FIXME: DO NOT DOCUMENT FLAGS p and x (they should be removed)
					// 2015.08.24: Left for compatibility: remove in some years :)
					if(bSubPatterns)
					{
						int start = 0;
						if(cnt == 1)
						{
							start = 1;
							n->set(i, new KviKvsVariant((kvs_int_t)(index + re.matchedLength())));
							i++;
						}
						if(bExcludeCompleteMatch)
							start = 1;
						for(int j = start; j < re.capturedTexts().size(); ++j)
						{
							n->set(i, new KviKvsVariant(re.capturedTexts().at(j)));
							i++;
						}
					}
					else
					{
						// 2015.08.24: End of "left for compatibility": remove in some years :)
						n->set(i, new KviKvsVariant(sz));
						i++;
						// 2015.08.24: Left for compatibility: remove in some years :)
					}
					// 2015.08.24: End of "left for compatibility": remove in some years :)
				}
			}
			idx++;
		}
	}
	else
	{
		while(idx < cnt)
		{
			KviKvsVariant * v = a->at(idx);
			if(v)
			{
				QString sz;
				v->asString(sz);
				if(sz.contains(szMatch, bCaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive))
				{
					n->set(i, new KviKvsVariant(sz));
					i++;
				}
			}
			idx++;
		}
	}

	return true;
}

/*
	@doc: str.split
	@type:
		function
	@title:
		$str.split
	@short:
		Splits a string to an array
	@syntax:
		<array> $str.split(<separator:string>,<data:string>[,<flags:string>[,<maxfields:integer>]])
	@description:
		Splits the <data> string by <separator> and returns an array of substrings.[br]
		<flags> may be a combination of the characters [b]s[/b], [b]w[/b], [b]r[/b] and [b]n[/b].[br]
		If [b]s[/b] is specified, <separator> matching is case sensitive, otherwise it is case insensitive.[br]
		If [b]w[/b] is specified, <separator> is treated as a wildcard-type regular expression
		(with [b]*[/b] and [b]?[/b] wildcards).[br]
		If r is specified, <separator> is treated as a extended-type regular expression
		(with character classes, special escapes etc.).[br]
		If both [b]w[/b] and [b]r[/b] are specified, [b]w[/b] takes precedence.[br]
		If neither [b]w[/b] and [b]r[/b] are specified <separator> is treated as a simple string to be matched.[br]
		If [b]n[/b] is specified then any resulting empty fields are discarded.[br]
		If <maxfield> is specified, then at most <maxfields> items are returned in the array (i.e. the last
		item may be not completely split).
	@examples:
		[example]
			[comment]# Split the fields[/comment]
			%test[] = $str.split(!,"Field0!Field1!Field2!Field3!!Field5")
			echo %test[]
			%i = 0
			[cmd]while[/cmd](%i < %test[]#)
			{
				[cmd]echo[/cmd] "Field %i: %test[%i]"
				%i++;
			}
		[/example]
		Regexp splitting:
		[example]
			%Test[] = $str.split("[ ]*[0-9][0-9]*-","AllOfThem: 1-Balboy 2-Pragma 3-Iakkolo 4-Crocodile",r)
			echo %Test[]
			%Test[] = $str.split("Y*H","hihiYeaHhohohoyeahYepYEAHhi",sw)
			echo %Test[]
		[/example]
		If used outside of an array context, a comma-separated list of substrings is returned:
		[example]
			[cmd]echo[/cmd] $str.split("[ ]*","Condense spaces and change &nbsp; &nbsp; all &nbsp; &nbsp; &nbsp; it in commas",r)
		[/example]
*/

static bool str_kvs_fnc_split(KviKvsModuleFunctionCall * c)
{
	QString szSep, szStr, szFla;
	kvs_int_t iMaxItems;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("separator", KVS_PT_STRING, 0, szSep)
	KVSM_PARAMETER("string", KVS_PT_STRING, 0, szStr)
	KVSM_PARAMETER("flags", KVS_PT_STRING, KVS_PF_OPTIONAL, szFla)
	KVSM_PARAMETER("maxitems", KVS_PT_INTEGER, KVS_PF_OPTIONAL, iMaxItems)
	KVSM_PARAMETERS_END(c)

#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
	if(c->params()->count() < 4)
	    iMaxItems = -1;

	KviKvsArray * a = new KviKvsArray();
	c->returnValue()->setArray(a);

	if(iMaxItems == 0)
		return true;
	if(iMaxItems == 1)
	{
		a->append(new KviKvsVariant{szStr});
		return true;
	}

	bool bWild = szFla.contains('w', Qt::CaseInsensitive);
	bool bContainsR = szFla.contains('r', Qt::CaseInsensitive);

	QString::SplitBehavior splitBehavior = szFla.contains('n', Qt::CaseInsensitive) ? QString::SkipEmptyParts : QString::KeepEmptyParts;
	Qt::CaseSensitivity sensitivity = szFla.contains('s', Qt::CaseInsensitive) ? Qt::CaseSensitive : Qt::CaseInsensitive;

	QVector<QStringRef> list;
	if(bWild || bContainsR)
		list = szStr.splitRef(QRegExp{szSep, sensitivity, bWild ? QRegExp::Wildcard : QRegExp::RegExp2}, splitBehavior);
	else
		list = szStr.splitRef(szSep, splitBehavior, sensitivity);

	if(iMaxItems < 0 || iMaxItems >= list.size())
	{
		for(auto&& str : list)
			a->append(new KviKvsVariant{str.toString()});
	}
	else
	{
		for(int i{0}; i < iMaxItems - 1; ++i)
			a->append(new KviKvsVariant{list[i].toString()});
		a->append(new KviKvsVariant{szStr.mid(list[iMaxItems - 1].position())});
	}
#else //QT_VERSION
	if(c->params()->count() < 4)
		iMaxItems = -1;

	KviKvsArray * a = new KviKvsArray();
	c->returnValue()->setArray(a);

	if(szSep.isEmpty())
	{
		a->set(0, new KviKvsVariant(szStr));
		return true;
	}

	if(iMaxItems == 0)
		return true;

	bool bWild = szFla.contains('w', Qt::CaseInsensitive);
	bool bContainsR = szFla.contains('r', Qt::CaseInsensitive);
	bool bCaseSensitive = szFla.contains('s', Qt::CaseInsensitive);
	bool bNoEmpty = szFla.contains('n', Qt::CaseInsensitive);

	int id = 0;

	int iMatch = 0;
	int iStrLen = szStr.length();
	int iBegin = 0;

	if(bContainsR || bWild)
	{
		QRegExp re(szSep, bCaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive, bWild ? QRegExp::Wildcard : QRegExp::RegExp2);

		while((iMatch != -1) && (iMatch < iStrLen) && ((id < (iMaxItems - 1)) || (iMaxItems < 0)))
		{
			iMatch = re.indexIn(szStr, iBegin);
			if(iMatch != -1)
			{
				int len = re.matchedLength();
				if((len == 0) && (iBegin == iMatch))
					iMatch++; // safety measure for empty string matching

				QString tmp = szStr.mid(iBegin, iMatch - iBegin);
				if(bNoEmpty)
				{
					if(!tmp.isEmpty())
					{
						a->set(id, new KviKvsVariant(tmp));
						id++;
					}
				}
				else
				{
					a->set(id, new KviKvsVariant(tmp));
					id++;
				}

				iMatch += len;
				iBegin = iMatch;
			}
		}
	}
	else
	{
		while((iMatch != -1) && (iMatch < iStrLen) && ((id < (iMaxItems - 1)) || (iMaxItems < 0)))
		{
			iMatch = szStr.indexOf(szSep, iBegin, bCaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive);
			if(iMatch != -1)
			{
				QString tmp = szStr.mid(iBegin, iMatch - iBegin);
				if(bNoEmpty)
				{
					if(!tmp.isEmpty())
					{
						a->set(id, new KviKvsVariant(tmp));
						id++;
					}
				}
				else
				{
					a->set(id, new KviKvsVariant(tmp));
					id++;
				}

				iMatch += szSep.length();
				iBegin = iMatch;
			}
		}
	}

	if(iBegin < iStrLen)
	{
		QString tmpx = szStr.right(iStrLen - iBegin);
		if(bNoEmpty)
		{
			if(!tmpx.isEmpty())
				a->set(id, new KviKvsVariant(tmpx));
		}
		else
		{
			a->set(id, new KviKvsVariant(tmpx));
		}
	}
	else
	{
		if(!bNoEmpty)
			a->set(id, new KviKvsVariant(QString())); // empty string at the end
	}
#endif //QT_VERSION

	return true;
}

/*
	@doc: str.printf
	@type:
		function
	@title:
		$str.printf
	@short:
		Returns a formatted string in a C sprintf-like fashion.
	@syntax:
		<string> $str.printf(<format:string>[,<parameter:variant>[...]]);
	@description:
		This function acts like the C sprintf(): it returns
		a string formatted by following the specification in <format>
		and by using the following corresponding variadic parameters.
		Since the percent sign is used as variable prefix in KVIrc,
		this function uses the question mark [b]?[/b] instead.[br]
		The <format> string can contain the following escape sequences:
		[table]
		[tr][td][b]?s[/b][/td][td]The next parameter is evaluated as a string and substituted in place of ?s[/td][/tr]
		[tr][td][b]?d[/b][/td][td]The next parameter is evaluated as a signed integer and substituted in place of ?d[/td][/tr]
		[tr][td][b]?i[/b][/td][td]Same as ?d[/td][/tr]
		[tr][td][b]?u[/b][/td][td]The next parameter is evaluated as an unsigned signed integer and substituted in place of ?d[/td][/tr]
		[tr][td][b]?x[/b][/td][td]The next parameter is evaluated as an unsigned integer and its hexadecimal representation
			is substituted in place of ?x[/td][/tr]
		[tr][td][b]?h[/b][/td][td]Same as ?x[/td][/tr]
		[tr][td][b]?X[/b][/td][td]Same as ?x but toUppercase hexadecimal digits are used[/td][/tr]
		[tr][td][b]?H[/b][/td][td]Same as ?X[/td][/tr]
		[tr][td][b]??[/b][/td][td]A literal question mark[/td][/tr]
		[tr][td][b]?[.N]f[/b][/td][td]The next parameter is evaluated as a real floating point value
			and its representation substituted in place of ?f. The optional [.N] modifier,
			where N is an unsigned integer, represents the desired precision.[/td][/tr]
		[tr][td][b]?[.N]e[/b][/td][td]The next parameter is evaluated as a real floating point value
			and its scientific representation substituted in place of ?e. The optional [.N] modifier,
			where N is an unsigned integer, represents the desired precision.[/td][/tr]
		[tr][td][b]?[.N]E[/b][/td][td]Same as ?e but an toUppercase E is used as the exponent prefix[/td][/tr]
		[/table]
	@examples:
		[example]
			%val = $(1.0 / 3.0);
			$str.printf("1/3 with a precision of 3 digits is ?.3f, while in scientific notation it's ?e",%val,%val)
		[/example]
*/

static bool str_kvs_fnc_printf(KviKvsModuleFunctionCall * c)
{
	QString szFormat;
	KviKvsVariantList vArgs;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("format", KVS_PT_STRING, 0, szFormat)
	KVSM_PARAMETER("vArgs", KVS_PT_VARIANTLIST, 0, vArgs)
	KVSM_PARAMETERS_END(c)

	QString s;

#define MEMINCREMENT 32

	const QChar * fmt = szFormat.constData();

	if(fmt)
	{
		int reallen = 0;
		int allocsize = MEMINCREMENT;
		QChar * buffer = (QChar *)KviMemory::allocate(sizeof(QChar) * allocsize);

		//9999999999999999999999999999999\0
		char numberBuffer[1024];
		char * pNumBuf;
		kvs_uint_t tmp;
		kvs_int_t argValue;
		kvs_uint_t argUValue;
		kvs_real_t argRValue;

		QChar * p = buffer;

#define INCREMENT_MEM                                                               \
	{                                                                               \
		allocsize += MEMINCREMENT;                                                  \
		buffer = (QChar *)KviMemory::reallocate(buffer, sizeof(QChar) * allocsize); \
		p = buffer + reallen;                                                       \
	}

#define INCREMENT_MEM_BY(numchars)                                                  \
	{                                                                               \
		allocsize += numchars + MEMINCREMENT;                                       \
		buffer = (QChar *)KviMemory::reallocate(buffer, sizeof(QChar) * allocsize); \
		p = buffer + reallen;                                                       \
	}

		KviKvsVariant * pVar;

		pVar = vArgs.first();

		for(; fmt->unicode(); ++fmt)
		{
			if(reallen == allocsize)
				INCREMENT_MEM

			//copy up to a '?'
			if(fmt->unicode() != '?')
			{
				*p++ = *fmt;
				reallen++;
				continue;
			}

			++fmt; //skip this '?'
			switch(fmt->unicode())
			{
				case 's':
				{
					QString sz;
					if(pVar)
						pVar->asString(sz);
					if(sz.isEmpty())
						continue;
					int len = sz.length();
					if((allocsize - reallen) < len)
						INCREMENT_MEM_BY(len)
					const QChar * ch = sz.unicode();
					while(len--)
						*p++ = *ch++;
					reallen += sz.length();
					pVar = vArgs.next();
					continue;
				}
				case 'd': //signed integer
				{
					if(pVar)
					{
						if(!pVar->asInteger(argValue))
						{
							c->warning(__tr2qs("Invalid argument for ?d escape sequence, 0 assumed"));
							argValue = 0;
						}
					}
					else
					{
						c->warning(__tr2qs("Missing argument for ?d escape sequence, 0 assumed"));
						argValue = 0;
					}

					if(argValue < 0)
					{ //negative integer
						*p++ = '-';
						reallen++;
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
					if((allocsize - reallen) < (int)argUValue)
						INCREMENT_MEM_BY(argUValue)
					do
					{
						*p++ = QChar(*--pNumBuf);
					} while(pNumBuf != numberBuffer);
					reallen += argUValue;
					pVar = vArgs.next();
					continue;
				}
				case 'u': //unsigned integer
				{
					if(pVar)
					{
						if(!pVar->asInteger(argValue))
						{
							c->warning(__tr2qs("Invalid argument for ?u escape sequence, 0 assumed"));
							argValue = 0;
						}
					}
					else
					{
						c->warning(__tr2qs("Missing argument for ?u escape sequence, 0 assumed"));
						argValue = 0;
					}
					argUValue = (kvs_uint_t)argValue;
					//write the number in a temporary buffer
					pNumBuf = numberBuffer;
					do
					{
						tmp = argUValue / 10;
						*pNumBuf++ = argUValue - (tmp * 10) + '0';
					} while((argUValue = tmp));
					//copy now....
					argValue = pNumBuf - numberBuffer; //length of the number string
					if((allocsize - reallen) < argValue)
						INCREMENT_MEM_BY(argValue)
					do
					{
						*p++ = *--pNumBuf;
					} while(pNumBuf != numberBuffer);
					reallen += argValue;
					pVar = vArgs.next();
					continue;
				}
				case 'h':
				case 'x': // hexadecimal unsigned integer
				{
					static char hexsmalldigits[] = "0123456789abcdef";

					if(pVar)
					{
						if(!pVar->asInteger(argValue))
						{
							c->warning(__tr2qs("Invalid argument for ?x escape sequence, 0 assumed"));
							argValue = 0;
						}
					}
					else
					{
						c->warning(__tr2qs("Missing argument for ?x escape sequence, 0 assumed"));
						argValue = 0;
					}
					argUValue = (kvs_uint_t)argValue;

					//write the number in a temporary buffer
					pNumBuf = numberBuffer;
					do
					{
						tmp = argUValue / 16;
						*pNumBuf++ = hexsmalldigits[argUValue - (tmp * 16)];
					} while((argUValue = tmp));
					//copy now....
					argValue = pNumBuf - numberBuffer; //length of the number string
					if((allocsize - reallen) < argValue)
						INCREMENT_MEM_BY(argValue)
					do
					{
						*p++ = *--pNumBuf;
					} while(pNumBuf != numberBuffer);
					reallen += argValue;
					pVar = vArgs.next();
					continue;
				}
				case 'H':
				case 'X': // hexadecimal unsigned integer
				{
					static char hexbigdigits[] = "0123456789ABCDEF";
					if(pVar)
					{
						if(!pVar->asInteger(argValue))
						{
							c->warning(__tr2qs("Invalid argument for ?X escape sequence, 0 assumed"));
							argValue = 0;
						}
					}
					else
					{
						c->warning(__tr2qs("Missing argument for ?X escape sequence, 0 assumed"));
						argValue = 0;
					}
					argUValue = (kvs_uint_t)argValue;
					//write the number in a temporary buffer
					pNumBuf = numberBuffer;
					do
					{
						tmp = argUValue / 16;
						*pNumBuf++ = hexbigdigits[argUValue - (tmp * 16)];
					} while((argUValue = tmp));
					//copy now....
					argValue = pNumBuf - numberBuffer; //length of the number string
					if((allocsize - reallen) < argValue)
						INCREMENT_MEM_BY(argValue)
					do
					{
						*p++ = *--pNumBuf;
					} while(pNumBuf != numberBuffer);
					reallen += argValue;
					pVar = vArgs.next();
					continue;
				}
				case '?':
				{
					if(fmt->unicode())
					{
						if(reallen == allocsize)
							INCREMENT_MEM
						*p++ = *fmt;
						reallen++;
					}
					continue;
				}
				break;
				case 'e':
				case 'E':
				case 'F':
				case 'f':
				case '.':
				{
					// precision mark
					const QChar * save = fmt;
					char fmtbuffer[8];
					fmtbuffer[0] = '%';
					fmtbuffer[1] = '.';
					int idx = 2;

					if(fmt->unicode() == '.')
					{
						// handle the optional precision parameter
						fmt++;
						while((fmt->unicode() >= '0') && (fmt->unicode() <= '9') && (idx < 6))
						{
							fmtbuffer[idx] = fmt->unicode();
							fmt++;
							idx++;
						}
					}

					fmtbuffer[idx] = fmt->unicode();
					fmtbuffer[idx + 1] = 0;

					if(pVar)
					{
						if(!pVar->asReal(argRValue))
						{
							c->warning(__tr2qs("Invalid argument for a floating point escape sequence, 0.0 assumed"));
							argRValue = 0.0;
						}
					}
					else
					{
						c->warning(__tr2qs("Missing argument for a floating point escape sequence, 0.0 assumed"));
						argRValue = 0;
					}
					switch(fmt->unicode())
					{
						case 'e':
						case 'E':
						case 'F':
						case 'f':
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
							_snprintf(numberBuffer, sizeof(numberBuffer), fmtbuffer, argRValue);
#else
							::sprintf(numberBuffer, fmtbuffer, argRValue);
#endif
							//copy now....
							argValue = kvi_strLen(numberBuffer);
							if((allocsize - reallen) < argValue)
								INCREMENT_MEM_BY(argValue)
							pNumBuf = numberBuffer;
							while(*pNumBuf)
							{
								*p++ = *pNumBuf++;
							}
							reallen += argValue;
							break;
						default:
							// anything else is crap.. invalid format
							fmt = save;
							*p++ = '?'; //write it
							reallen++;
							if(fmt->unicode())
							{
								if(reallen == allocsize)
									INCREMENT_MEM
								*p++ = *fmt;
								reallen++;
							}
							break;
					}
					pVar = vArgs.next();
					continue;
				}
				break;
				default: //a normal ? followed by some char
				{
					*p++ = '?'; //write it
					reallen++;
					if(fmt->unicode())
					{
						if(reallen == allocsize)
							INCREMENT_MEM
						*p++ = *fmt;
						reallen++;
					}
					continue;
				}
			}
		}

		s.setUnicode(buffer, reallen);
		KviMemory::free(buffer);
	}

	c->returnValue()->setString(s);
	return true;
}

/*
	@doc: str.evpSign
	@type:
		function
	@title:
		$str.evpSign
	@short:
		Returns a signature for a message created using your certificate's private key
	@syntax:
		<string> $str.evpSign(<message:string>[,<certificate:string>[,<password:string>]])
	@description:
		This function returns a base64-encoded signature string created signing the
		provided <message> using the private key of the specified <certificate>.[br]
		If the <certificate> parameter is omitted, the private key specified in the
		kvirc options will be used.[br]
		If any error occurs, this function will return an empty string.
	@examples:
		[example]
			# Emulate a call with no parameters
			%message="test message";
			%pcert=$file.read($option(stringSSLPrivateKeyPath));
			%sign=$str.evpSign(%message,%pcert,$option(stringSSLPrivateKeyPass));
		[/example]
		[example]
			# Sign and verify the signature using the certificates from options
			%message="test message";
			%sign=$str.evpSign(%message);
			if($str.evpVerify(%message, %sign))
			{
				echo "signature is valid";
			} else {
				echo "signature is not valid";
			}
		[/example]
	@seealso:
		[fnc]$str.evpVerify[/fnc]
		[fnc]$certificate[/fnc]
		[fnc]$dcc.getSSLCertInfo[/fnc]
*/

static bool str_kvs_fnc_evpSign(KviKvsModuleFunctionCall * c)
{
	QByteArray szMessage;
	QByteArray szCert;
	QByteArray szPass;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("message", KVS_PT_NONEMPTYCSTRING, 0, szMessage)
	KVSM_PARAMETER("certificate", KVS_PT_NONEMPTYCSTRING, KVS_PF_OPTIONAL, szCert)
	KVSM_PARAMETER("password", KVS_PT_NONEMPTYCSTRING, KVS_PF_OPTIONAL, szPass)
	KVSM_PARAMETERS_END(c)

#if defined(COMPILE_SSL_SUPPORT)

	KviSSL::globalSSLInit();
	EVP_MD_CTX *md_ctx;
	EVP_PKEY * pKey = nullptr;
	unsigned int len = 0;
	unsigned char * sig = nullptr;
	int err;

	if(szCert.isEmpty())
	{
		//use default cert
		if(!KVI_OPTION_BOOL(KviOption_boolUseSSLPrivateKey))
		{
			c->warning(__tr2qs("No certificate specified and no private key certificate defined in KVIrc options."));
			c->returnValue()->setString("");
			return true;
		}

		FILE * f = fopen(KVI_OPTION_STRING(KviOption_stringSSLPrivateKeyPath).toUtf8().data(), "r");
		if(!f)
		{
			c->warning(__tr2qs("File I/O error while trying to use the private key file %s"), KVI_OPTION_STRING(KviOption_stringSSLPrivateKeyPath).toUtf8().data());
			c->returnValue()->setString("");
			return true;
		}

		szPass = KVI_OPTION_STRING(KviOption_stringSSLPrivateKeyPass).toUtf8();
		PEM_read_PrivateKey(f, &pKey, nullptr, szPass.data());

		fclose(f);

		if(!pKey)
		{
			c->warning(__tr2qs("Can't read private key while trying to use the default private key certificate %s"), KVI_OPTION_STRING(KviOption_stringSSLPrivateKeyPath).toUtf8().data());
			c->returnValue()->setString("");
			return true;
		}
	}
	else
	{
		// get from parameter (with optional password)
		BIO * in;
		in = BIO_new_mem_buf((unsigned char *)szCert.data(), szCert.size());
		PEM_read_bio_PrivateKey(in, &pKey, nullptr, szPass.data());
		BIO_free(in);

		if(!pKey)
		{
			c->warning(__tr2qs("Can't read private key while trying to use the provided certificate (wrong password?)"));
			c->returnValue()->setString("");
			return true;
		}
	}

	len = EVP_PKEY_size(pKey);
	sig = (unsigned char *)KviMemory::allocate(len * sizeof(char));

	md_ctx = EVP_MD_CTX_new();
	EVP_SignInit(md_ctx, EVP_sha1());
	EVP_SignUpdate(md_ctx, (unsigned char *)szMessage.data(), szMessage.length());
	err = EVP_SignFinal(md_ctx, sig, &len, pKey);
	EVP_MD_CTX_free(md_ctx);
	if(err)
	{
		QByteArray szSign((const char *)sig, len);
		OPENSSL_free(sig);
		EVP_PKEY_free(pKey);
		c->returnValue()->setString(szSign.toBase64().data());
		return true;
	}
	c->warning(__tr2qs("An error occurred while signing the message."));
	c->returnValue()->setString("");
	return true;

#else
	c->warning(__tr2qs("KVIrc is compiled without OpenSSL support."));
	c->returnValue()->setString("");
	return true;
#endif
}

/*
	@doc: str.evpVerify
	@type:
		function
	@title:
		$str.evpVerify
	@short:
		Verifies the signature for a message against a public key
	@syntax:
		<bool> $str.evpVerify(<message:string>,<signature:string>[,<certificate:string>[,<password:string>]])
	@description:
		This function verifies the signature for a message against a public key contained in a certificate.[br]
		The signature has to be base64-encoded, as the one returned by [fnc]$str.evpSign[/fnc].[br]
		If the <certificate> parameter is omitted, the public key certificate specified in the
		kvirc options will be used.[br]
		If any error occurs, this function will return false.
	@examples:
		[example]
			# Emulate a call with no certificate parameters
			%message="test message";
			%signature=$str.evpSign(%message);
			%cert=$file.read($option(stringSSLCertificatePath));
			$str.evpVerify(%message,%signature,%cert,$option(stringSSLCertificatePass));
		[/example]
		[example]
			# Sign and verify the signature using the certificates from options
			%message="test message";
			%sign=$str.evpSign(%message);
			if($str.evpVerify(%message, %sign))
			{
				echo "signature is valid";
			} else {
				echo "signature is not valid";
			}
		[/example]
	@seealso:
		[fnc]$str.evpSign[/fnc]
		[fnc]$certificate[/fnc]
		[fnc]$dcc.getSSLCertInfo[/fnc]
*/

static bool str_kvs_fnc_evpVerify(KviKvsModuleFunctionCall * c)
{
	QByteArray szMessage;
	QByteArray szCert;
	QByteArray szSign;
	QByteArray szSignB64;
	QByteArray szPass;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("message", KVS_PT_NONEMPTYCSTRING, 0, szMessage)
	KVSM_PARAMETER("signature", KVS_PT_NONEMPTYCSTRING, 0, szSignB64)
	KVSM_PARAMETER("certificate", KVS_PT_NONEMPTYCSTRING, KVS_PF_OPTIONAL, szCert)
	KVSM_PARAMETER("password", KVS_PT_NONEMPTYCSTRING, KVS_PF_OPTIONAL, szPass)
	KVSM_PARAMETERS_END(c)

#if defined(COMPILE_SSL_SUPPORT)

	KviSSL::globalSSLInit();
	szSign = QByteArray::fromBase64(szSignB64);
	const char * message = szMessage.data();

	EVP_MD_CTX* md_ctx;
	EVP_PKEY * pKey = nullptr;
	X509 * cert = nullptr;
	int err = -1;

	if(szCert.isEmpty())
	{
		//use default cert
		if(!KVI_OPTION_BOOL(KviOption_boolUseSSLCertificate))
		{
			c->warning(__tr2qs("No certificate specified and no public key certificate defined in KVIrc options."));
			c->returnValue()->setString("");
			return true;
		}

		FILE * f = fopen(KVI_OPTION_STRING(KviOption_stringSSLCertificatePath).toUtf8().data(), "r");
		if(!f)
		{
			c->warning(__tr2qs("File I/O error while trying to use the public key file %s"), KVI_OPTION_STRING(KviOption_stringSSLCertificatePath).toUtf8().data());
			c->returnValue()->setString("");
			return true;
		}

		szPass = KVI_OPTION_STRING(KviOption_stringSSLCertificatePass).toUtf8();
		PEM_read_X509(f, &cert, nullptr, szPass.data());

		fclose(f);

		if(cert)
		{
			pKey = (EVP_PKEY *)X509_get_pubkey(cert);
			X509_free(cert);
		}

		if(!pKey)
		{
			c->warning(__tr2qs("Can't read public key while trying to use the default public key certificate %s"), KVI_OPTION_STRING(KviOption_stringSSLCertificatePath).toUtf8().data());
			c->returnValue()->setString("");
			return true;
		}
	}
	else
	{
		// get from parameter (with optional password)
		BIO * in = BIO_new_mem_buf((unsigned char *)szCert.data(), szCert.size());
		PEM_read_bio_X509(in, &cert, nullptr, szPass.data());

		if(cert)
		{
			pKey = (EVP_PKEY *)X509_get_pubkey(cert);
			X509_free(cert);
		}
		else
		{
			pKey = PEM_read_bio_PUBKEY(in, nullptr, nullptr, szPass.data());
		}

		BIO_free(in);

		if(!pKey)
		{
			c->warning(__tr2qs("Can't read public key from the provided certificate."));
			c->returnValue()->setBoolean(false);
			return true;
		}
	}

	md_ctx = EVP_MD_CTX_new();
	EVP_VerifyInit(md_ctx, EVP_sha1());
	EVP_VerifyUpdate(md_ctx, message, strlen(message));
	err = EVP_VerifyFinal(md_ctx, (unsigned char *)szSign.data(), szSign.size(), pKey);
	EVP_MD_CTX_free(md_ctx);
	EVP_PKEY_free(pKey);
	switch(err)
	{
		case 0:
			c->returnValue()->setBoolean(false);
			return true;
		case 1:
			c->returnValue()->setBoolean(true);
			return true;
		default:
			c->warning(__tr2qs("An error occurred during signature verification."));
			c->returnValue()->setBoolean(false);
			return true;
	}
#else
	c->warning(__tr2qs("KVIrc is compiled without OpenSSL support."));
	c->returnValue()->setBoolean(false);
	return true;
#endif
}

///////////////////////////////////////////////////////////////////////////////
//  Module stuff
///////////////////////////////////////////////////////////////////////////////

static bool str_module_init(KviModule * m)
{
	KVSM_REGISTER_FUNCTION(m, "append", str_kvs_fnc_append);
	KVSM_REGISTER_FUNCTION(m, "charsum", str_kvs_fnc_charsum);
	KVSM_REGISTER_FUNCTION(m, "chop", str_kvs_fnc_chop);
	KVSM_REGISTER_FUNCTION(m, "cmp", str_kvs_fnc_cmp);
	KVSM_REGISTER_FUNCTION(m, "contains", str_kvs_fnc_contains);
	KVSM_REGISTER_FUNCTION(m, "digest", str_kvs_fnc_digest);
	KVSM_REGISTER_FUNCTION(m, "equal", str_kvs_fnc_equal);
	KVSM_REGISTER_FUNCTION(m, "find", str_kvs_fnc_find);
	KVSM_REGISTER_FUNCTION(m, "findfirst", str_kvs_fnc_findfirst);
	KVSM_REGISTER_FUNCTION(m, "findlast", str_kvs_fnc_findlast);
	KVSM_REGISTER_FUNCTION(m, "fromClipboard", str_kvs_fnc_fromclipboard);
	KVSM_REGISTER_FUNCTION(m, "grep", str_kvs_fnc_grep);
	KVSM_REGISTER_FUNCTION(m, "insert", str_kvs_fnc_insert);
	KVSM_REGISTER_FUNCTION(m, "isempty", str_kvs_fnc_isempty);
	KVSM_REGISTER_FUNCTION(m, "isnumber", str_kvs_fnc_isnumber);
	KVSM_REGISTER_FUNCTION(m, "isunsignednumber", str_kvs_fnc_isunsignednumber);
	KVSM_REGISTER_FUNCTION(m, "join", str_kvs_fnc_join);
	KVSM_REGISTER_FUNCTION(m, "left", str_kvs_fnc_left);
	KVSM_REGISTER_FUNCTION(m, "lefttofirst", str_kvs_fnc_lefttofirst);
	KVSM_REGISTER_FUNCTION(m, "lefttolast", str_kvs_fnc_lefttolast);
	KVSM_REGISTER_FUNCTION(m, "len", str_kvs_fnc_len);
	KVSM_REGISTER_FUNCTION(m, "length", str_kvs_fnc_len);
	KVSM_REGISTER_FUNCTION(m, "localelowcase", str_kvs_fnc_localelowcase);
	KVSM_REGISTER_FUNCTION(m, "localeupcase", str_kvs_fnc_localeupcase);
	KVSM_REGISTER_FUNCTION(m, "lowcase", str_kvs_fnc_lowcase);
	KVSM_REGISTER_FUNCTION(m, "match", str_kvs_fnc_match);
	KVSM_REGISTER_FUNCTION(m, "mid", str_kvs_fnc_mid);
	KVSM_REGISTER_FUNCTION(m, "prepend", str_kvs_fnc_prepend);
	KVSM_REGISTER_FUNCTION(m, "printf", str_kvs_fnc_printf);
	KVSM_REGISTER_FUNCTION(m, "replace", str_kvs_fnc_replace);
	KVSM_REGISTER_FUNCTION(m, "right", str_kvs_fnc_right);
	KVSM_REGISTER_FUNCTION(m, "rightfromfirst", str_kvs_fnc_rightfromfirst);
	KVSM_REGISTER_FUNCTION(m, "rightfromlast", str_kvs_fnc_rightfromlast);
	KVSM_REGISTER_FUNCTION(m, "section", str_kvs_fnc_section);
	KVSM_REGISTER_FUNCTION(m, "split", str_kvs_fnc_split);
	KVSM_REGISTER_FUNCTION(m, "strip", str_kvs_fnc_strip);
	KVSM_REGISTER_FUNCTION(m, "stripcolors", str_kvs_fnc_stripcolors);
	KVSM_REGISTER_FUNCTION(m, "stripleft", str_kvs_fnc_stripleft);
	KVSM_REGISTER_FUNCTION(m, "stripright", str_kvs_fnc_stripright);
	KVSM_REGISTER_FUNCTION(m, "token", str_kvs_fnc_token);
	KVSM_REGISTER_FUNCTION(m, "upcase", str_kvs_fnc_upcase);
	KVSM_REGISTER_FUNCTION(m, "urlencode", str_kvs_fnc_urlencode);
	KVSM_REGISTER_FUNCTION(m, "word", str_kvs_fnc_word);
	KVSM_REGISTER_FUNCTION(m, "evpSign", str_kvs_fnc_evpSign);
	KVSM_REGISTER_FUNCTION(m, "evpVerify", str_kvs_fnc_evpVerify);

	KVSM_REGISTER_SIMPLE_COMMAND(m, "toClipboard", str_kvs_cmd_toClipboard);
	return true;
}

static bool str_module_cleanup(KviModule *)
{
	return true;
}

KVIRC_MODULE(
    "Str",   // module name
    "4.0.0", // module version
    "Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)"
    "          (C) 2002 Juanjo Alvarez (juanjux at yahoo dot es)"
    "          (C) 2005 Tonino Imbesi (grifisx at barmes dot org)"
    "          (C) 2005 Alessandro Carbone (elfonol at gmail dot com)"
    "          (C) 2009 Kai Wasserbäch (debian at carbon-project dot org)"
    "          (C) 2010 Elvio Basello (hell at hellvis69 dot netsons dot org)", // author & (C)
    "Interface to the str system",
    str_module_init,
    0,
    0,
    str_module_cleanup,
    0)
