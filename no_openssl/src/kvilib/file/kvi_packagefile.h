#ifndef _KVI_PACKAGEFILE_H_
#define _KVI_PACKAGEFILE_H_
//=============================================================================
//
//   File : kvi_packagefile.h
//   Creation date : Tue 26 Dec 2006 05:33:33 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2006-2008 Szymon Stefanek <pragma at kvirc dot net>
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
* \file kvi_packagefile.h
* \author Szymon Stefanek
* \brief File packaging utilities
*
* \def KVI_PACKAGE_INFOFIELD_TYPE_STRING Defines the string type of the info field
* \def KVI_PACKAGE_INFOFIELD_TYPE_BINARYBUFFER Defines the binary type of the info field
* \def KVI_PACKAGE_DATAFIELD_TYPE_FILE Defines the file type of the data field
* \def KVI_PACKAGE_DATAFIELD_FLAG_FILE_DEFLATE Defines the type of file compression of the data field
*/

#include "kvi_settings.h"
#include "kvi_qstring.h"
#include "kvi_pointerhashtable.h"
#include "kvi_pointerlist.h"

#include <QObject>
#include <QByteArray>

class QProgressDialog;
class QLabel;
class QFileInfo;
class KviFile;

#define KVI_PACKAGE_INFOFIELD_TYPE_STRING 1
#define KVI_PACKAGE_INFOFIELD_TYPE_BINARYBUFFER 2
#define KVI_PACKAGE_DATAFIELD_TYPE_FILE 1
#define KVI_PACKAGE_DATAFIELD_FLAG_FILE_DEFLATE 1

/**
* \class KviPackageIOEngine
* \brief This class is the base class engine to work with KVIrc package files.
*/
class KVILIB_API KviPackageIOEngine
{
public:
	/**
	* \brief Creates the I/O engine object
	* \return KviPackageIOEngine
	*/
	KviPackageIOEngine();

	/**
	* \brief Destroys the object
	*/
	virtual ~KviPackageIOEngine();
protected:
	QString                                   m_szLastError;
	KviPointerHashTable<QString,QString>    * m_pStringInfoFields;
	KviPointerHashTable<QString,QByteArray> * m_pBinaryInfoFields;
	QProgressDialog                         * m_pProgressDialog;
	QLabel                                  * m_pProgressDialogLabel;
public:
	/**
	* \brief Returns the last error
	* \return const QString &
	*/
	const QString & lastError(){ return m_szLastError; };

	/**
	* \brief Sets the last error
	* \param szLastError The string error
	* \return void
	*/
	void setLastError(const QString & szLastError){ m_szLastError = szLastError; };

	/**
	* \brief Returns the string info fields
	* \return KviPointerHashTable<QString,QString> *
	*/
	KviPointerHashTable<QString,QString> * stringInfoFields(){ return m_pStringInfoFields; };

	/**
	* \brief Returns the binary info fields
	* \return KviPointerHashTable<QString,QByteArray> *
	*/
	KviPointerHashTable<QString,QByteArray> * binaryInfoFields(){ return m_pBinaryInfoFields; };
protected:
	/**
	* \brief Shows the progress dialog
	* \param szCaption The caption of the dialog window
	* \param iTotalSteps The total number of steps
	* \return void
	*/
	void showProgressDialog(const QString & szCaption, int iTotalSteps);

	/**
	* \brief Hides the progress dialog
	* \return void
	*/
	void hideProgressDialog();

	/**
	* \brief Updates the progress dialog
	* \param iProgress The value in steps of the progress bar
	* \param szLabel The label of the step
	* \return bool
	*/
	bool updateProgress(int iProgress, const QString & szLabel);

	/**
	* \brief Sets the last error as write error
	*
	* This is a shortcut to setLastError()
	* \return bool
	*/
	bool writeError();

	/**
	* \brief Sets the last error as read error
	*
	* This is a shortcut to setLastError()
	* \return bool
	*/
	bool readError();
};

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
protected:
	/**
	* \class DataField
	* \brief This class holds the data for the fields
	*/
	class DataField
	{
	public:
		kvi_u32_t m_uType;
		// output length of the field
		kvi_u32_t m_uWrittenFieldLength;
		// data fields for the File DataFieldType
		bool m_bFileAllowCompression;
		QString m_szFileLocalName;
		QString m_szFileTargetName;
	};

	KviPointerList<DataField> * m_pDataFields;
	int m_iCurrentProgress;
public:
	/**
	* \enum AddFileFlags
	*/
	enum AddFileFlags {
		NoCompression = 1, /**< If you don't want to compress the file */
		FollowSymLinks = 2 /**< If you want to follow the symlinks */
	};

	/**
	* \enum PackFlags
	*/
	enum PackFlags {
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
	bool packFile(KviFile * pFile, DataField * pDataField);

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
};

/**
* \class KviPackageReader
* \brief This class is used to read KVIrc package files.
*
* You simply instantiate it and then call unpack().
*/
class KVILIB_API KviPackageReader : public KviPackageIOEngine
{
public:
	/**
	* \brief Creates the package reader object
	* \return KviPackageReader
	*/
	KviPackageReader();

	/**
	* \brief Destroys the object
	*/
	virtual ~KviPackageReader();
public:
	/**
	* \enum UnpackFlags
	*/
	enum UnpackFlags {
		NoProgressDialog = 1 /**< If you want to avoid the progress dialog */
	};

	/**
	* \brief Read the header of the package
	*
	* This is a shortcut to readHeaderInternal()
	* \param szLocalFileName The source package
	* \return bool
	*/
	bool readHeader(const QString & szLocalFileName);

	/**
	* \brief Get the string info field contained in the package
	* \param szName The name of the info field
	* \param szBuffer The buffer where to store data
	* \return bool
	*/
	bool getStringInfoField(const QString & szName, QString & szBuffer);

	/**
	* \brief Unpack the KVIrc package file
	*
	* This is a shortcut to unpackInternal() hiding the progress dialog
	* \param szLocalFileName The source package
	* \param szUnpackPath The path where to unpack the package
	* \param uUnpackFlags The flags to unpack
	* \return bool
	*/
	bool unpack(const QString & szLocalFileName, const QString & szUnpackPath, kvi_u32_t uUnpackFlags = 0);
private:
	/**
	* \brief Unpack the KVIrc package file
	*
	* This is a shortcut to unpackFile() performing some checks
	* \param szLocalFileName The source package
	* \param szUnpackPath The path where to unpack the package
	* \param uUnpackFlags The flags to unpack
	* \return bool
	*/
	bool unpackInternal(const QString & szLocalFileName, const QString & szUnpackPath, kvi_u32_t uUnpackFlags = 0);

	/**
	* \brief Unpack the KVIrc package file
	*
	* This is the real unpack() function.
	* \param pFile The source file package
	* \param szUnpackPath The path where to unpack the package
	* \return bool
	*/
	bool unpackFile(KviFile * pFile, const QString & szUnpackPath);

	/**
	* \brief Read the header of the package
	*
	* This is the real readHeader() function
	* \param pFile The pointer to the real file
	* \param szLocalFileName The source package
	* \return bool
	*/
	bool readHeaderInternal(KviFile * pFile, const QString & szLocalFileName);
};

#endif //_KVI_PACKAGEFILE_H_
