#ifndef _KVI_FILEUTILS_H_
#define _KVI_FILEUTILS_H_
//=============================================================================
//
//   File : KviFileUtils.h
//   Creation date : Fri Dec 25 1998 18:27:04 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 1998-2010 Szymon Stefanek (pragma at kvirc dot net)
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

/**
* \file KviFileUtils.h
* \author Szymon Stefanek
* \brief File utilities functions
*
* \def KVI_PATH_SEPARATOR Defines the path separator, UNIX or win32
* \def KVI_PATH_SEPARATOR_CHAR Defines the path separator char, UNIX or win32
*/

#include "kvi_settings.h"

#include <QFile>

class QByteArray;
class QString;
class QStringList;

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
#define KVI_PATH_SEPARATOR "\\"
#define KVI_PATH_SEPARATOR_CHAR '\\'
#else
#define KVI_PATH_SEPARATOR "/"
#define KVI_PATH_SEPARATOR_CHAR '/'
#endif

// #warning "Add kvi_trashFile(const char * path) ? - is it needed in the whole app"
// #warning "or should it be available only for dirbrowser module?"
/**
* \namespace KviFileUtils
* \brief A namespace to handle file utilities functions
*/
namespace KviFileUtils
{
	//KVILIB_API bool readLine(QFile * f,QString &szBuffer,bool bClearBuffer = true);
	//KVILIB_API bool loadFileStripCR(const QString &szPath,QString &szBuffer);

	/**
	* \brief Loads the file at szPath to szBuffer eventually converting from UTF-8
	* \param szPath The path to the file to load
	* \param szBuffer The buffer where to load the file
	* \param bUtf8 Where to convert from UTF-8
	* \return bool
	*/
	KVILIB_API bool loadFile(const QString & szPath, QString & szBuffer, bool bUtf8 = true);

	/**
	* \brief Adjusts the file path to the current platform
	* \param szPath The path to the file
	* \return void
	*/
	KVILIB_API void adjustFilePath(QString & szPath);

	/**
	* \brief Returns true if szPath points to an existing directory
	* \param szPath The path to the directory
	* \return bool
	*/
	KVILIB_API bool directoryExists(const QString & szPath);

	/**
	* \brief Returns true if szPath points to an existing file
	* \param szPath The path to the file
	* \return bool
	*/
	inline bool fileExists(const QString & szPath) { return QFile::exists(szPath); }

	/**
	* \brief Removes a file
	* \param szPath The file to remove
	* \return bool
	*/
	KVILIB_API bool removeFile(const QString & szPath);

	/**
	* \brief Removes a file
	* \param pcPath The file to remove
	* \return bool
	*/
	KVILIB_API bool removeFile(const char * pcPath);

	/**
	* \brief Removes a dir (must be empty)
	* \param szPath The directory to remove
	* \return bool
	*/
	KVILIB_API bool removeDir(const QString & szPath);

	/**
	* \brief Removes a dir (must be empty)
	* \param pcPath The directory to remove
	* \return bool
	*/
	KVILIB_API bool removeDir(const char * pcPath);

	/**
	* \brief Removes a dir recursively
	* \param szPath The path of the directory to remove
	* \return bool
	*/
	KVILIB_API bool deleteDir(const QString & szPath);

	/**
	* \brief Writes a complete file (UTF-8 version)
	* \param szPath The path to the file
	* \param oData The data to write
	* \param bAppend If we want to append data or overwrite
	* \return bool
	*/
	KVILIB_API bool writeFile(const QString & szPath, const QByteArray & oData, bool bAppend = false);

	/**
	* \brief Writes a complete file (UTF-8 version)
	* \param szPath The path to the file
	* \param szData The data to write
	* \param bAppend If we want to append data or overwrite
	* \return bool
	*/
	KVILIB_API bool writeFile(const QString & szPath, const QString & szData, bool bAppend = false);

	/**
	* \brief Writes a complete file (UTF-8 version)
	* \param pcPath The path to the file
	* \param szData The data to write
	* \param bAppend If we want to append data or overwrite
	* \return bool
	*/
	KVILIB_API bool writeFile(const char * pcPath, const QString & szData, bool bAppend = false);

	/**
	* \brief Writes a complete file (local 8 bit version)
	* \param szPath The path to the file
	* \param szData The data to write
	* \param bAppend If we want to append data or overwrite
	* \return bool
	*/
	KVILIB_API bool writeFileLocal8Bit(const QString & szPath, const QString & szData, bool bAppend = false);

	/**
	* \brief Writes a complete file (local 8 bit version)
	* \param pcPath The path to the file
	* \param szData The data to write
	* \param bAppend If we want to append data or overwrite
	* \return bool
	*/
	KVILIB_API bool writeFileLocal8Bit(const char * pcPath, const QString & szData, bool bAppend = false);

	/**
	* \brief Reads a complete file and puts it in the string szBuffer
	*
	* The file must be smaller than uMaxSize bytes
	* \param szPath The path to the file to read
	* \param szBuffer The buffer where to load the file
	* \param uMaxSize The maximum size of the file to read
	* \return bool
	*/
	KVILIB_API bool readFile(const QString & szPath, QString & szBuffer, unsigned int uMaxSize = 65535);

	/**
	* \brief Reads a complete file and puts it in the string szBuffer
	*
	* The file must be smaller than uMaxSize bytes
	* \param pcPath The path to the file to read
	* \param szBuffer The buffer where to load the file
	* \param uMaxSize The maximum size of the file to read
	* \return bool
	*/
	KVILIB_API bool readFile(const char * pcPath, QString & szBuffer, unsigned int uMaxSize = 65535);

	/**
	* \brief Extracts the filename from a complete path (strips leading path)
	*
	* Extracts the filename from a complete path (strips leading path).
	* If szFileNameWithPath ends with a trailing slash/backslash then if bAllowEmpty is set
	* to true then the name is considered to be empty, if bAllowEmpty is set to false
	* then the trailing slash/backslash is stripped.
	*
	* \param szFileNameWithPath The complete path to the file
	* \return QString
	*/
	KVILIB_API QString extractFileName(const QString & szFileNameWithPath, bool bAllowEmpty = true);

	/**
	* \brief Extracts the filename from a complete path (strips leading path)
	* \param szFileNameWithPath The complete path
	* \return QString
	*/
	KVILIB_API QString extractFilePath(const QString & szFileNameWithPath);

	/**
	* \brief Copy the file (cp -f)
	* \param szSrc The source file
	* \param szDst The destination file
	* \return bool
	*/
	KVILIB_API bool copyFile(const QString & szSrc, const QString & szDst);

	/**
	* \brief Copy the file (cp -f)
	* \param pcSrc The source file
	* \param pcDst The destination file
	* \return bool
	*/
	KVILIB_API bool copyFile(const char * pcSrc, const char * pcDst);

	/**
	* \brief Rename or move the file (mv)
	* \param szSrc The source file
	* \param szDst The destination file
	* \return bool
	*/
	KVILIB_API bool renameFile(const QString & szSrc, const QString & szDst);

	/**
	* \brief Rename or move the file (mv)
	* \param pcSrc The source file
	* \param pcDst The destination file
	* \return bool
	*/
	KVILIB_API bool renameFile(const char * pcSrc, const char * pcDst);

	/**
	* \brief Create a directory (mkdir)
	* \param szPath The path to the directory
	* \return bool
	*/
	KVILIB_API bool makeDir(const QString & szPath);

	/**
	* \brief Create a directory (mkdir)
	* \param pcPath The path to the directory
	* \return bool
	*/
	KVILIB_API bool makeDir(const char * pcPath);

	/**
	* \brief Reads a text line, returns false if EOF is reached
	* \param pFile The source file
	* \param szBuffer The buffer where to store the line read
	* \param bUtf8 If we want to convert from UTF-8
	* \return bool
	*/
	KVILIB_API bool readLine(QFile * pFile, QString & szBuffer, bool bUtf8 = true);

	/**
	* \brief Reads text lines, returns false if EOF is reached
	* \param pFile The source file
	* \param buffer The buffer where to store the lines read
	* \param iStartLine The number of the first line to read
	* \param iCount The number of lines to read
	* \param bUtf8 If we want to convert from UTF-8
	* \return bool
	*/
	KVILIB_API bool readLines(QFile * pFile, QStringList & buffer, int iStartLine = 0, int iCount = -1, bool bUtf8 = true);

	/**
	* \brief Returns true if the file is readable, false otherwise
	* \param szFname The source file
	* \return bool
	*/
	KVILIB_API bool isReadable(const QString & szFname);

	/**
	* \brief Returns true if the path is absolute, false otherwise
	* \param szPath The path to check
	* \return bool
	*/
	KVILIB_API bool isAbsolutePath(const QString & szPath);

	/**
	* \brief Translates ANY string into a valid filename (with no path!)
	*
	* There is NO way to come back to the original string the algo is
	* one-way only
	* \param szPath
	* \return void
	*/
	KVILIB_API void encodeFileName(QString & szPath);

	/**
	* \brief Removes any unusable character from a filename (with no path!)
	*
	* There is NO way to come back to the original string the algo is
	* one-way only
	* \param szPath
	* \return void
	*/
	KVILIB_API void cleanFileName(QString & szPath);

	/**
	* Build a recursive file listing
	*/
	KVILIB_API QStringList getFileListing(const QString & szPath);
}

#endif //_KVI_FILEUTILS_H
