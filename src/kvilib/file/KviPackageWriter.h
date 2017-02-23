#ifndef _KviPackageWriter_h_
#define _KviPackageWriter_h_
//=============================================================================
//
//   File : KviPackageWriter.h
//   Creation date : Tue 26 Dec 2006 05:33:33 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2006-2010 Szymon Stefanek <pragma at kvirc dot net>
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
* \file KviPackageWriter.h
* \author Szymon Stefanek
* \brief File packaging utilities
*/

#include "kvi_settings.h"
#include "KviPackageIOEngine.h"

class KviFile;
class KviPackageWriterPrivate;
class KviPackageWriterDataField;
class QByteArray;
class QFileInfo;
class QString;

/**
* \class KviPackageWriter
* \brief This class is used to create KVIrc package files.
*
* You simply instantiate it, add some info fields, add some files and then call
* pack().
*/
class KVILIB_API KviPackageWriter : public KviPackageIOEngine
{
public:
	/**
	* \brief Creates the package writer object
	* \return KviPackageWriter
	*/
	KviPackageWriter();

	/**
	* \brief Destroys the object
	*/
	virtual ~KviPackageWriter();

private:
	/**
	* Hidden private data
	*/
	KviPackageWriterPrivate * m_p;

public:
	/**
	* \enum AddFileFlags
	*/
	enum AddFileFlags
	{
		NoCompression = 1, /**< If you don't want to compress the file */
		FollowSymLinks = 2 /**< If you want to follow the symlinks */
	};

	/**
	* \enum PackFlags
	*/
	enum PackFlags
	{
		NoProgressDialog = 1 /**< If you want to avoid the progress dialog */
	};

	/**
	* \brief Adds a file to the package.
	*
	* The file must be specified as absolute local path and as target path
	* relative to the KVIrc local directory.
	* This is a shortcut to addFileInternal()
	* ... more ?
	* \param szLocalFileName The source file
	* \param szTargetFileName The destination file
	* \param uAddFileFlags The flags of the file
	* \return bool
	*/
	bool addFile(const QString & szLocalFileName, const QString & szTargetFileName, kvi_u32_t uAddFileFlags = 0);

	/**
	* \brief Adds a directory to the package.
	* \param szLocalDirectoryName The source directory
	* \param szTargetDirectoryPrefix The destination directory
	* \param uAddFileFlags The flags of the directory
	* \return bool
	*/
	bool addDirectory(const QString & szLocalDirectoryName, const QString & szTargetDirectoryPrefix, kvi_u32_t uAddFileFlags = 0);

	/**
	* \brief Adds an info field as a name=value pair
	* \param szName The name of the info field
	* \param szValue The value of the info field
	* \return void
	*/
	void addInfoField(const QString & szName, const QString & szValue);

	/**
	* \brief Adds an info field as a name=value pair
	* \param szName The name of the info field
	* \param pArray The value of the info field
	* \return void
	*/
	void addInfoField(const QString & szName, QByteArray * pArray);

	/**
	* \brief Attempts to pack everything and store it as the specified file.
	*
	* This is a shortcut to packInternal() hiding the progress dialog
	* There is no mandatory extension but you *should* use
	* KVI_FILEEXTENSION_THEMEPACKAGE for themes and
	* KVI_FILEEXTENSION_ADDONPACKAGE for addons. See kvi_fileextension.h
	* \param szFileName The file name of the package
	* \param uPackFlags The flags of the package
	* \return bool
	*/
	bool pack(const QString & szFileName, kvi_u32_t uPackFlags = 0);

private:
	/**
	* \brief Attempts to pack everything and store it as the specified file.
	*
	* This is a shortcut to packFile() performing some checks
	* \param szFileName The file name of the package
	* \param uPackFlags The flags of the package
	* \return bool
	*/
	bool packInternal(const QString & szFileName, kvi_u32_t uPackFlags = 0);

	/**
	* \brief Attempts to pack everything and store it as the specified file.
	*
	* This is the real pack() function
	* \param pFile The source file package
	* \param pDataField The data field for the package
	* \return bool
	*/
	bool packFile(KviFile * pFile, KviPackageWriterDataField * pDataField);

	/**
	* \brief Adds a file to the package.
	*
	* This is the real addFile() function
	* \param fi THe information of the file
	* \param szLocalFileName The source file
	* \param szTargetFileName The destination file
	* \param uAddFileFlags The flags of the file
	* \return bool
	*/
	bool addFileInternal(const QFileInfo * fi, const QString & szLocalFileName, const QString & szTargetFileName, kvi_u32_t uAddFileFlags = 0);

}; // class KviPackageWriter

#endif //_KviPackageWriter_h_
