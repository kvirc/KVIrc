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

#include <QFile>
#include <QStringList>

#include <time.h>


#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
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
	inline bool fileExists(const QString &szPath) {	return QFile::exists(szPath); };
	extern KVILIB_API bool fileExists(const char* szPath);
	// removes a file
	extern KVILIB_API bool removeFile(const QString &szPath);
	extern KVILIB_API bool removeFile(const char* path);
	// removes a dir (must be empty)
	extern KVILIB_API bool removeDir(const QString &szPath);
	extern KVILIB_API bool removeDir(const char* path);
	// removes a dir recursively
	extern KVILIB_API bool deleteDir(const QString &szPath);
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

	extern KVILIB_API QString extractFilePath(const QString &szFileNameWithPath);
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
	extern KVILIB_API bool readLines(QFile * f,QStringList &buffer,int iStartLine = 0, int iCount = -1, bool bUtf8 = true);
	extern KVILIB_API bool isReadable(const QString &szFname);
	extern KVILIB_API bool isAbsolutePath(const QString &szPath);
};

// ALL THIS STUFF BELOW SHOULD DIE: IF YOU SEE IT, REPLACE WITH THE FUNCTIONS IN THE NAMESPACE ABOVE

// Returns true if the path begins with '/'
KVILIB_API extern bool kvi_isAbsolutePath(const char *path);
// Translates ANY string into a valid filename (with no path!)
// There is NO way to come back to the original string
// the algo is one-way only
KVILIB_API extern void kvi_encodeFileName(KviStr & path);
KVILIB_API extern void kvi_encodeFileName(QString & path);

// Reads a single line from the file and returns false if EOF was encountered.
KVILIB_API extern bool kvi_readLine(QFile *f,KviStr &str);
// Removes a file

#endif //_KVI_FILEUTILS_H_INCLUDED_
