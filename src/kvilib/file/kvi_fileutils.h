#ifndef _KVI_FILEUTILS_H_
#define _KVI_FILEUTILS_H_

//
//   File : kvi_fileutils.h
//   Creation date : Fri Dec 25 1998 18:27:04 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2000 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"
#include "kvi_string.h"

#include <qfile.h>
#include <qstringlist.h>

#include <time.h>


#ifdef COMPILE_ON_WINDOWS
	#define KVI_PATH_SEPARATOR "\\"
	#define KVI_PATH_SEPARATOR_CHAR '\\'
#else
	#define KVI_PATH_SEPARATOR "/"
	#define KVI_PATH_SEPARATOR_CHAR '/'
#endif

// #warning "Add kvi_trashFile(const char * path) ? - is it needed in the whole app"
// #warning "or should it be availible only for dirbrowser module ?"
namespace KviFileUtils
{
	//extern KVILIB_API bool readLine(QFile * f,QString &szBuffer,bool bClearBuffer = true);
	//extern KVILIB_API bool loadFileStripCR(const QString &szPath,QString &szBuffer);

	// loads the file at szPath to szBuffer eventually converting from utf8
	extern KVILIB_API bool loadFile(const QString &szPath,QString &szBuffer,bool bUtf8 = true);
	extern KVILIB_API bool loadFile(const char* szPath,QString &szBuffer,bool bUtf8 = true);
	// adjusts the file path to the current platform
	extern KVILIB_API void adjustFilePath(QString &szPath);
	// returns true if szPath points to an existing directory
	extern KVILIB_API bool directoryExists(const QString &szPath);
	extern KVILIB_API bool directoryExists(const char* path);
	// returns true if szPath points to an existing file
	extern KVILIB_API bool fileExists(const QString &szPath);
	extern KVILIB_API bool fileExists(const char* szPath);
	// removes a file
	extern KVILIB_API bool removeFile(const QString &szPath);
	extern KVILIB_API bool removeFile(const char* path);
	// removes a dir (must be empty)
	extern KVILIB_API bool removeDir(const QString &szPath);
	extern KVILIB_API bool removeDir(const char* path);
	// writes a complete file (utf8 version)
	extern KVILIB_API bool writeFile(const QString &szPath,const QString &szData,bool bAppend = false);
	extern KVILIB_API bool writeFile(const char* path,const QString &szData,bool bAppend = false);
	// writes a complete file (local 8 bit version)
	extern KVILIB_API bool writeFileLocal8Bit(const QString &szPath,const QString &szData,bool bAppend = false);
	extern KVILIB_API bool writeFileLocal8Bit(const char* path,const QString &szData,bool bAppend = false);
	// reads a complete file and puts it in the string szBuffer, if the file is smaller than uMaxSize bytes
	extern KVILIB_API bool readFile(const QString &szPath,QString &szBuffer,unsigned int uMaxSize = 65535);
	extern KVILIB_API bool readFile(const char* path,QString &szBuffer,unsigned int uMaxSize = 65535);
	// extracts the filename from a complete path (strips leading path)
	extern KVILIB_API QString extractFileName(const QString &szFileNameWithPath);
	// cp -f
	extern KVILIB_API bool copyFile(const QString &szSrc,const QString &szDst);
	extern KVILIB_API bool copyFile(const char* src,const char* dst);
	// mv
	extern KVILIB_API bool renameFile(const QString &szSrc,const QString &szDst);
	extern KVILIB_API bool renameFile(const char* src,const char* dst);
	// mkdir
	extern KVILIB_API bool makeDir(const QString &szPath);
	extern KVILIB_API bool makeDir(const char* path);
	// reads a text line, returns false if EOF is reached
	extern KVILIB_API bool readLine(QFile * f,QString &szBuffer,bool bUtf8 = true);
	extern KVILIB_API bool isReadable(const QString &szFname);
	extern KVILIB_API bool isAbsolutePath(const QString &szPath);
};

/*
Important:

REMEMBER: ONLY QString's doesn't break localization!
char* filenames breaks it
instead of QFile(char*) we MUST use QString szString=char* ; QFile(szString);
*/

// Returns true if the path begins with '/'
KVILIB_API extern bool kvi_isAbsolutePath(const char *path);
// Simplifies the path from double separators , and eventually converts the separators
KVILIB_API extern void kvi_adjustFilePath(KviStr & path);
KVILIB_API extern void kvi_adjustFilePath(QString & path);
// Translates ANY string into a valid filename (with no path!)
// There is NO way to come back to the original string
// the algo is one-way only
KVILIB_API extern void kvi_encodeFileName(KviStr & path);
KVILIB_API extern void kvi_encodeFileName(QString & path);
// Returns true if the file exists and IS a file
//DEPRECATED: breaks intarnationalization!
//KVILIB_API extern bool kvi_fileExists(const char *path);

// kvi_fileExists & isReadable
//DEPRECATED: breaks intarnationalization!
//KVILIB_API extern bool kvi_fileIsReadable(const char *path);

// Returns true if the file exists and IS a directory
//DEPRECATED: breaks intarnationalization!
//KVILIB_API extern bool kvi_directoryExists(const char *path);

// Makes a directory (and eventually all the leading path)
//KVILIB_API extern bool kvi_makeDir(const char *path);

// Loads a file to a buffer (false if the file does not exist)
//DEPRECATED: breaks intarnationalization!
//KVILIB_API extern bool kvi_loadFile(const char *path,KviStr &buffer);
// Writes a buffer to a new file

//DEPRECATED: breaks intarnationalization!
//KVILIB_API extern bool kvi_writeFile(const char *path,KviStr &buffer,bool bAppend = false);

// cp :)
//KVILIB_API extern bool kvi_copyFile(const char *src,const char *dst);

// mv :)
//KVILIB_API extern bool kvi_renameFile(const char *src,const char *dst);

// Reads a single line from the file and returns false if EOF was encountered.
KVILIB_API extern bool kvi_readLine(QFile *f,KviStr &str);
// Same as above but strips whitespace too.
KVILIB_API extern bool kvi_readStrippedLine(QFile *f,KviStr &str);
// Reads stripped lines until a non empty one is found...
// An empty line may be returned if EOF is found before reading any significant data.
KVILIB_API extern bool kvi_readFirstNonEmptyStrippedLine(QFile *f,KviStr &str);
// Writes a data line to the file f and returns false if an error occurs
KVILIB_API extern bool kvi_writeLine(QFile *f,const char *line);
// Removes a file
KVILIB_API extern bool kvi_removeFile(const char * path);
// Removes a dir
//KVILIB_API extern bool kvi_removeDir(const char * path);

// File dialogs (DEAD)
//__kvi_extern QString kvi_askForDirectoryName(const char * basePath = 0);
//__kvi_extern QString kvi_askForOpenFileName(const char *basePath = 0,const char *filter = 0);
//__kvi_extern QStringList kvi_askForOpenFileNames(const char * basePath = 0,const char * filter = 0);
//__kvi_extern QString kvi_askForSaveFileName(const char *baseFileName = 0);

#endif //_KVI_FILEUTILS_H_INCLUDED_
