#ifndef _KVI_PACKAGEFILE_H_
#define _KVI_PACKAGEFILE_H_
//=============================================================================
//
//   File : kvi_packagefile.h
//   Created on Tue 26 Dec 2006 05:33:33 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2006 Szymon Stefanek <pragma at kvirc dot net>
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

#include "kvi_settings.h"
#include "kvi_qstring.h"
#include "kvi_pointerhashtable.h"
#include "kvi_qcstring.h" // QByteArray anyway
#include <QObject>
#include "kvi_pointerlist.h"

class KviFile;
class QProgressDialog;
class QLabel;
class QFileInfo;

//
// This class is used for creating KVIrc package files.
// You simply instantiate it, add some info fields, add some files and then call pack().
//

class KVILIB_API KviPackageIOEngine
{
public:
	KviPackageIOEngine();
	virtual ~KviPackageIOEngine();
protected:
	QString m_szLastError;
	KviPointerHashTable<QString,QString> * m_pStringInfoFields;
	KviPointerHashTable<QString,QByteArray> * m_pBinaryInfoFields;
	QProgressDialog * m_pProgressDialog;
	QLabel * m_pProgressDialogLabel;
public:
	const QString & lastError(){ return m_szLastError; };
	void setLastError(const QString &szLastError){ m_szLastError = szLastError; };
	KviPointerHashTable<QString,QString> * stringInfoFields(){ return m_pStringInfoFields; };
	KviPointerHashTable<QString,QByteArray> * binaryInfoFields(){ return m_pBinaryInfoFields; };
protected:
	void showProgressDialog(const QString &szCaption,int iTotalSteps);
	void hideProgressDialog();
	bool updateProgress(int iProgress,const QString &szLabel);
	bool writeError();
	bool readError();
};

#define KVI_PACKAGE_INFOFIELD_TYPE_STRING 1
#define KVI_PACKAGE_INFOFIELD_TYPE_BINARYBUFFER 2

#define KVI_PACKAGE_DATAFIELD_TYPE_FILE 1

#define KVI_PACKAGE_DATAFIELD_FLAG_FILE_DEFLATE 1

class KVILIB_API KviPackageWriter : public KviPackageIOEngine
{
public:
	KviPackageWriter();
	virtual ~KviPackageWriter();
protected:

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
	// Adds a file to the package. The file must be specified as absolute local
	// path and as target path relative to the KVIrc local directory.
	// ... more ?
	enum AddFileFlags {
		NoCompression = 1,
		FollowSymLinks = 2
	};
	bool addFile(const QString &szLocalFileName,const QString &szTargetFileName,kvi_u32_t uAddFileFlags = 0);
	bool addDirectory(const QString &szLocalDirectoryName,const QString &szTargetDirectoryPrefix,kvi_u32_t uAddFileFlags = 0);
	// Adds an info field as a name=value pair
	void addInfoField(const QString &szName,const QString &szValue);
	void addInfoField(const QString &szName,QByteArray * pArray);
	// Attempts to pack everything and store it as the specified file.
	// There is no mandatory extension but you *should* use KVI_FILEEXTENSION_THEMEPACKAGE for themes
	// and KVI_FILEEXTENSION_ADDONPACKAGE for addons. See kvi_fileextension.h
	enum PackFlags {
		NoProgressDialog = 1
	};
	bool pack(const QString &szFileName,kvi_u32_t uPackFlags = 0);
private:
	bool packInternal(const QString &szFileName,kvi_u32_t uPackFlags = 0);
	bool packFile(KviFile * pFile,DataField * pDataField);
	bool addFileInternal(const QFileInfo * fi,const QString &szLocalFileName,const QString &szTargetFileName,kvi_u32_t uAddFileFlags = 0);
};

class KVILIB_API KviPackageReader : public KviPackageIOEngine
{
public:
	KviPackageReader();
	virtual ~KviPackageReader();
public:
	bool readHeader(const QString &szLocalFileName);
	enum UnpackFlags {
		NoProgressDialog = 1
	};
	bool getStringInfoField(const QString &szName,QString &szBuffer);
	bool unpack(const QString &szLocalFileName,const QString &szUnpackPath,kvi_u32_t uUnpackFlags = 0);
private:
	bool unpackInternal(const QString &szLocalFileName,const QString &szUnpackPath,kvi_u32_t uUnpackFlags = 0);
	bool unpackFile(KviFile * pFile,const QString &szUnpackPath);
	bool readHeaderInternal(KviFile * pFile,const QString &szLocalFileName);
};



#endif //!_KVI_PACKAGEFILE_H_
