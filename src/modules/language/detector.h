#ifndef _DETECTOR_H_
#define _DETECTOR_H_
//=============================================================================
//
//   File : detector.h
//   Creation date : Sun May 11 1999 00:07:20 CET by Szymon Stefanek
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

#define DLE_NUM_BEST_MATCHES 4
#define DLE_STRICT_UTF8_CHECKING 1

struct LanguageAndEncodingMatch
{
	const char * szLanguage;
	const char * szEncoding;
	double dScore;
};

struct LanguageAndEncodingResult
{
	LanguageAndEncodingMatch match[DLE_NUM_BEST_MATCHES]; // the first best matches
	double dAccuracy;                                     // accuracy score: from 0 to 100
};

void detect_language_and_encoding(const char * data, LanguageAndEncodingResult * retBuffer, int iFlags);

#endif //_DETECTOR_H_
