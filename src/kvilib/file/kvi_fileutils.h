#ifndef _KVI_FILEUTILS_H_
#define _KVI_FILEUTILS_H_
//=============================================================================
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
//=============================================================================

/**
* \file kvi_fileutils.h
* \author Szymon Stefanek
* \brief File utilities functions
*/

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
/**
* \namespace KviFileUtils
* \brief A namespace to handle file utilities functions
*/
namespace KviFileUtils
{
	//extern KVILIB_API bool readLine(QFile * f,QString &szBuffer,bool bClearBuffer = true);
	//extern KVILIB_API bool loadFileStripCR(const QString &szPath,QString &szBuffer);

	/**
	* \brief Loads the file at szPath to szBuffer eventually converting from UTF-8
	* \param szPath The path to the file to load
	* \param szBuffer The buffer where to load the file
	* \param bUtf8 Where to convert from UTF-8
	*/
	extern KVILIB_API bool loadFile(const QString & szPath, QString & szBuffer, bool bUtf8 = true);

	/**
	* \brief Adjusts the file path to the current platform
	* \param szPath The path to the file
	*/
	extern KVILIB_API void adjustFilePath(QString & szPath);

	/**
	* \brief Returns true if szPath points to an existing directory
	* \param szPath The path to the directory
	*/
	extern KVILIB_API bool directoryExists(const QString & szPath);

	/**
	* \brief Returns true if szPath points to an existing file
	* \param szPath The path to the file
	*/
	inline bool fileExists(const QString & szPath){ return QFile::exists(szPath); };

	/**
	/* \brief Removes a file
	* \param szPath The file to remove
	*/
	extern KVILIB_API bool removeFile(const QString & szPath);

	/**
	/* \brief Removes a file
	* \param path The file to remove
	*/
	extern KVILIB_API bool removeFile(const char * path);

	/**
	* \brief Removes a dir (must be empty)
	* \param szPath The directory to remove
	*/
	extern KVILIB_API bool removeDir(const QString & szPath);

	/**
	* \brief Removes a dir (must be empty)
	* \param path The directory to remove
	*/
	extern KVILIB_API bool removeDir(const char * path);

	/**
	* \brief Removes a dir recursively
	* \param szPath The path of the directory to remove
	*/
	extern KVILIB_API bool deleteDir(const QString & szPath);

	/**
	* \brief Writes a complete file (UTF-8 version)
	* \param szPath The path to the file
	* \param szData The data to write
	* \param bAppend If we want to append data or overwrite
	*/
	extern KVILIB_API bool writeFile(const QString & szPath, const QString & szData, bool bAppend = false);

	/**
	* \brief Writes a complete file (UTF-8 version)
	* \param path The path to the file
	* \param szData The data to write
	* \param bAppend If we want to append data or overwrite
	*/
	extern KVILIB_API bool writeFile(const char * path, const QString & szData, bool bAppend = false);

	/**
	* \brief Writes a complete file (local 8 bit version)
	* \param szPath The path to the file
	* \param szData The data to write
	* \param bAppend If we want to append data or overwrite
	*/
	extern KVILIB_API bool writeFileLocal8Bit(const QString & szPath, const QString & szData, bool bAppend = false);

	/**
	* \brief Writes a complete file (local 8 bit version)
	* \param path The path to the file
	* \param szData The data to write
	* \param bAppend If we want to append data or overwrite
	*/
	extern KVILIB_API bool writeFileLocal8Bit(const char * path, const QString & szData, bool bAppend = false);

	/**
	* \brief Reads a complete file and puts it in the string szBuffer
	* The file must be smaller than uMaxSize bytes
	* \param szPath The path to the file to read
	* \param szBuffer The buffer where to load the file
	* \param uMaxSize The maximum size of the file to read
	*/
	extern KVILIB_API bool readFile(const QString & szPath, QString & szBuffer, unsigned int uMaxSize = 65535);

	/**
	* \brief Reads a complete file and puts it in the string szBuffer
	* The file must be smaller than uMaxSize bytes
	* \param path The path to the file to read
	* \param szBuffer The buffer where to load the file
	* \param uMaxSize The maximum size of the file to read
	*/
	extern KVILIB_API bool readFile(const char * path, QString & szBuffer, unsigned int uMaxSize = 65535);

	/**
	* \brief Extracts the filename from a complete path (strips leading path)
	* \param szFileNameWithPath The complete path to the file
	*/
	extern KVILIB_API QString extractFileName(const QString & szFileNameWithPath);

	/**
	* \brief Extracts the filename from a complete path (strips leading path)
	* \param szFileNameWithPath The complete path
	*/
	extern KVILIB_API QString extractFilePath(const QString & szFileNameWithPath);

	/**
	* \brief Copy the file (cp -f)
	* \param szSrc The source file
	* \param szDst The destination file
	*/
	extern KVILIB_API bool copyFile(const QString & szSrc, const QString & szDst);

	/**
	* \brief Copy the file (cp -f)
	* \param src The source file
	* \param dst The destination file
	*/
	extern KVILIB_API bool copyFile(const char * src, const char * dst);

	/**
	* \brief Rename or move the file (mv)
	* \param szSrc The source file
	* \param szDst The destination file
	*/
	extern KVILIB_API bool renameFile(const QString & szSrc,const QString & szDst);

	/**
	* \brief Rename or move the file (mv)
	* \param src The source file
	* \param dst The destination file
	*/
	extern KVILIB_API bool renameFile(const char * src, const char * dst);

	/**
	* \brief Create a directory (mkdir)
	* \param szPath The path to the directory
	*/
	extern KVILIB_API bool makeDir(const QString & szPath);

	/**
	* \brief Create a directory (mkdir)
	* \param path The path to the directory
	*/
	extern KVILIB_API bool makeDir(const char * path);

	/**
	* \brief Reads a text line, returns false if EOF is reached
	* \param f The source file
	* \param szBuffer The buffer where to store the line read
	* \param bUtf8 If we want to convert from UTF-8
	*/
	extern KVILIB_API bool readLine(QFile * f, QString & szBuffer, bool bUtf8 = true);

	/**
	* \brief Reads text lines, returns false if EOF is reached
	* \param f The source file
	* \param buffer The buffer where to store the lines read
	* \param iStartLine The number of the first line to read
	* \param iCount The number of lines to read
	* \param bUtf8 If we want to convert from UTF-8
	*/
	extern KVILIB_API bool readLines(QFile * f, QStringList & buffer, int iStartLine = 0, int iCount = -1, bool bUtf8 = true);

	/**
	* \brief Returns true if the file is readable, false otherwise
	* \param szFname The source file
	*/
	extern KVILIB_API bool isReadable(const QString & szFname);

	/**
	* \brief Returns true if the path is absolute, false otherwise
	* \param szPath The path to check
	*/
	extern KVILIB_API bool isAbsolutePath(const QString & szPath);

	/**
	* \brief Translates ANY string into a valid filename (with no path!)
	* There is NO way to come back to the original string the algo is
	* one-way only
	* \param path
	*/
	KVILIB_API extern void encodeFileName(QString & path);
};

#endif //_KVI_FILEUTILS_H
