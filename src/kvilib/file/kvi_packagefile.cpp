//=============================================================================
//
//   File : kvi_packagefile.cpp
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

#include "kvi_packagefile.h"

#include "kvi_file.h"
#include "kvi_fileutils.h"
#include "kvi_locale.h"
#include "kvi_inttypes.h"

#include <QProgressDialog>
#include <QLabel>
#include <QDir>

#ifdef COMPILE_ZLIB_SUPPORT
	#include <zlib.h>
#endif

//
// A KVIrc Package File is basically a simple zip file with some additional meta-data.
// The package file has the following format
//

// Field               Type       Bytes           Description
//-------------------------------------------------------------------------------
// Package:
//   PackageHeader
//   PackageInfo
//   PackageData

// PackageHeader:
//   Magic             Bytes      4               'KVPF': Signature for the Kvirc Package File
//   Version           uint32     4               0x00000001:  Version of this package file
//   Flags             uint32     4               0x00000000:  Flags, in version 1 is reserved and must be zero
//

// PackageInfo:
//   InfoFieldCount    uint32     4               Number of package info fields
//   InfoField         InfoField  Variable        A list of informational name-value pairs
//   InfoField         InfoField  Variable        A list of informational name-value pairs
//   InfoField         InfoField  Variable        A list of informational name-value pairs
//   ....              ....       ....

// PackageData:
//   DataField         DataField  Variable        A list of data fields with format defined below
//   DataField         DataField  Variable        A list of data fields with format defined below
//   DataField         DataField  Variable        A list of data fields with format defined below
//   ....              ....       ....

// InfoField:
//   Name              UniString  Variable        The "name" element of the info field
//   ValueType         uint32     4               The type of the following ValueData field
//   ValueData         ValueData  Variable

// ValueData for ValueType 1 (string field)
//   Value             UniString  Variable        The value element of type string of the the info field

// ValueData for ValueType 2 (binary buffer field)
//   BufferLen         uint32     4               The length of the binary buffer
//   BufferData        Bytes      Variable        The data for the binary buffer


// UniString:
//   StringLen         uint32     4               The length of the string data in BYTES (null terminator NOT included)
//   StringData        Bytes      StringLen       An utf8 encoded string (do NOT write the NULL terminator)

// Bytes:
//   Byte              uint8      1               A byte
//   Byte              uint8      1               A byte
//   ....              ....       ....

// DataField:
//   FieldType         uint32     4               The type of the field, see below for defined values
//   FieldLen          uint32     4               FieldData length in bytes (useful for skipping a field if unsupported)
//   FieldData         Variable   FieldLen        The data of the field, see below for defined values

// FieldData for FieldType 1 (file field)
//   Flags             uint32     4               Bitmask. Bits: 1=FileIsDeflated
//   Path              UniString  Variable        A relative path expressed as utf8 string. \ AND / are considered to be separators
//   Size              uint32     4               Size of the following file data
//   FilePayload       Bytes      Variable

// Everything is stored in LITTLE ENDIAN byte order.

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Da Base Engine

KviPackageIOEngine::KviPackageIOEngine()
{
	m_pProgressDialog = 0;
	m_pStringInfoFields = new KviPointerHashTable<QString,QString>();
	m_pStringInfoFields->setAutoDelete(true);
	m_pBinaryInfoFields = new KviPointerHashTable<QString,QByteArray>();
	m_pBinaryInfoFields->setAutoDelete(true);
}

KviPackageIOEngine::~KviPackageIOEngine()
{
	if(m_pProgressDialog)delete m_pProgressDialog;
	delete m_pStringInfoFields;
	delete m_pBinaryInfoFields;
}

bool KviPackageIOEngine::updateProgress(int iProgress,const QString &szLabel)
{
	if(!m_pProgressDialog)return true;

	m_pProgressDialog->setValue(iProgress);
	m_pProgressDialogLabel->setText(szLabel);
	qApp->processEvents();
	if(m_pProgressDialog->wasCanceled())
	{
		setLastError(__tr2qs("Operation cancelled"));
		return false;
	}
	return true;
}

void KviPackageIOEngine::showProgressDialog(const QString &szCaption,int iTotalSteps)
{
	m_pProgressDialog = new QProgressDialog(QString(""),__tr2qs("Cancel"),0,iTotalSteps,0);
	m_pProgressDialog->setModal(true);
	m_pProgressDialog->setWindowTitle(szCaption);

	m_pProgressDialogLabel = new QLabel(m_pProgressDialog);
	m_pProgressDialogLabel->setMaximumSize(500,300);
	m_pProgressDialog->setLabel(m_pProgressDialogLabel);
}

void KviPackageIOEngine::hideProgressDialog()
{
	if(!m_pProgressDialog)return;
	delete m_pProgressDialog;
	m_pProgressDialog = 0;
}

bool KviPackageIOEngine::writeError()
{
	setLastError(__tr2qs("File write error"));
	return false;
}

bool KviPackageIOEngine::readError()
{
	setLastError(__tr2qs("File read error"));
	return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Da Writer


KviPackageWriter::KviPackageWriter()
: KviPackageIOEngine()
{
	m_pDataFields = new KviPointerList<DataField>();
	m_pDataFields->setAutoDelete(true);
}

KviPackageWriter::~KviPackageWriter()
{
	delete m_pDataFields;
}

void KviPackageWriter::addInfoField(const QString &szName,const QString &szValue)
{
	m_pStringInfoFields->replace(szName,new QString(szValue));
}

void KviPackageWriter::addInfoField(const QString &szName,QByteArray * pValue)
{
	m_pBinaryInfoFields->replace(szName,pValue);
}

bool KviPackageWriter::addFile(const QString &szLocalFileName,const QString &szTargetFileName,kvi_u32_t uAddFileFlags)
{
	QFileInfo fi(szLocalFileName);
	return addFileInternal(&fi,szLocalFileName,szTargetFileName,uAddFileFlags);
}

bool KviPackageWriter::addFileInternal(const QFileInfo * fi,const QString &szLocalFileName,const QString &szTargetFileName,kvi_u32_t uAddFileFlags)
{
	if(!(fi->isFile() && fi->isReadable()))
		return false;

	if(!(uAddFileFlags & FollowSymLinks))
	{
		if(fi->isSymLink())
			return true; // do NOT add a symlink
	}

	DataField * f = new DataField();
	f->m_uType = KVI_PACKAGE_DATAFIELD_TYPE_FILE;
	f->m_bFileAllowCompression = !(uAddFileFlags & NoCompression);
	f->m_szFileLocalName = szLocalFileName;
	f->m_szFileTargetName = szTargetFileName;
	m_pDataFields->append(f);

	return true;
}

bool KviPackageWriter::addDirectory(const QString &szLocalDirectoryName,const QString &szTargetDirectoryPrefix,kvi_u32_t uAddFileFlags)
{
	QDir d(szLocalDirectoryName);
	QDir::Filters iFlags;

	iFlags = QDir::Files | QDir::Readable;
	if(!(uAddFileFlags & FollowSymLinks))
		iFlags |= QDir::NoSymLinks;

	int j;
	QFileInfoList sl = d.entryInfoList(iFlags);
	for(j=0;j<sl.size();j++)
	{
		QString szSFileName = szLocalDirectoryName;
		KviQString::ensureLastCharIs(szSFileName,QChar(KVI_PATH_SEPARATOR_CHAR));
		QFileInfo slowCopy = sl.at(j);
		szSFileName += slowCopy.fileName();

		QString szDFileName = szTargetDirectoryPrefix;
		KviQString::ensureLastCharIs(szDFileName,QChar(KVI_PATH_SEPARATOR_CHAR));
		szDFileName += slowCopy.fileName();
		if(!addFileInternal(&slowCopy,szSFileName,szDFileName,uAddFileFlags))
			return false;
	}
	iFlags = QDir::Dirs | QDir::Readable;
	if(!(uAddFileFlags & FollowSymLinks))
		iFlags |= QDir::NoSymLinks;
	sl = d.entryInfoList(iFlags);

	for(j=0;j<sl.size();j++)
	{
		QString szDir = sl.at(j).fileName();
		if(!KviQString::equalCS(szDir,"..") && !KviQString::equalCS(szDir,"."))
		{
			QString szSDirName = szLocalDirectoryName;
			KviQString::ensureLastCharIs(szSDirName,QChar(KVI_PATH_SEPARATOR_CHAR));
			szSDirName += szDir;
			QString szDDirName = szTargetDirectoryPrefix;
			KviQString::ensureLastCharIs(szDDirName,QChar(KVI_PATH_SEPARATOR_CHAR));
			szDDirName += szDir;
			if(!addDirectory(szSDirName,szDDirName,uAddFileFlags))
				return false;
		}
	}
	return true;
}

#define BUFFER_SIZE 32768

bool KviPackageWriter::packFile(KviFile * pFile,DataField * pDataField)
{
	QString szProgressText;
	KviQString::sprintf(szProgressText,__tr2qs("Packaging file %Q"),&(pDataField->m_szFileLocalName));
	if(!updateProgress(m_iCurrentProgress,szProgressText))
		return false; // aborted


	KviFile source(pDataField->m_szFileLocalName);
	if(!source.open(QFile::ReadOnly))
	{
		setLastError(__tr2qs("Failed to open a source file for reading"));
		return false;
	}

	kvi_u32_t uSize = source.size();

	// Flags
#ifdef COMPILE_ZLIB_SUPPORT
	kvi_u32_t uFlags = pDataField->m_bFileAllowCompression ?
			(uSize > 64 ? KVI_PACKAGE_DATAFIELD_FLAG_FILE_DEFLATE : 0)
			: 0;
#else
	kvi_u32_t uFlags = 0;
#endif

	if(!pFile->save(uFlags))return writeError();

	QByteArray szTargetFileName = KviQString::toUtf8(pDataField->m_szFileTargetName);

	// Path
	if(!pFile->save(szTargetFileName))return writeError();

	kvi_file_offset_t savedSizeOffset = pFile->pos();

	// Size : will update it if compression is requested
	if(!pFile->save(uSize))return writeError();

	pDataField->m_uWrittenFieldLength = 4 + 4 + 4 + szTargetFileName.length(); // sizeof(flags + uncompressed size + path len + path)

	// FilePayload
#ifdef COMPILE_ZLIB_SUPPORT
	if(uFlags & KVI_PACKAGE_DATAFIELD_FLAG_FILE_DEFLATE)
	{
		unsigned char ibuffer[BUFFER_SIZE];
		unsigned char obuffer[BUFFER_SIZE];

		kvi_i32_t iReaded = source.read((char *)ibuffer,BUFFER_SIZE);
		if(iReaded < 0)
			return readError();

		z_stream zstr;
		zstr.zalloc = Z_NULL;
		zstr.zfree = Z_NULL;
		zstr.opaque = Z_NULL;
		zstr.next_in = ibuffer;
		zstr.avail_in = iReaded;
		zstr.next_out = obuffer;
		zstr.avail_out = BUFFER_SIZE;

		if(deflateInit(&zstr,9) != Z_OK)
		{
			setLastError(__tr2qs("Compression library initialization error"));
			return false;
		}

		while(iReaded > 0)
		{
			zstr.next_out = obuffer;
			zstr.avail_out = BUFFER_SIZE;

			if(deflate(&zstr,Z_NO_FLUSH) != Z_OK)
			{
				setLastError(__tr2qs("Compression library error"));
				return false;
			}

			if(zstr.avail_out < BUFFER_SIZE)
			{
				int iCompressed = zstr.next_out - obuffer;
				pDataField->m_uWrittenFieldLength += iCompressed;
				if(pFile->write((char *)obuffer,iCompressed) != iCompressed)
				{
					deflateEnd(&zstr);
					return writeError();
				}
			}

			if(zstr.avail_in < BUFFER_SIZE)
			{
				int iDataToRead = BUFFER_SIZE - zstr.avail_in;
				if(iDataToRead < BUFFER_SIZE)
				{
					if(ibuffer != zstr.next_in)
					{
						// hum, there is still some data in the buffer to be readed
						// and it is not at the beginning...move it to the beginning of ibuffer
						memmove(ibuffer,zstr.next_in,zstr.avail_in);
					}
				}
				iReaded = source.read((char *)(ibuffer + zstr.avail_in),iDataToRead);
				if(iReaded < 0)
				{
					deflateEnd(&zstr);
					return readError();
				}
				zstr.avail_in += iReaded;
				zstr.next_in = ibuffer;

				if((zstr.total_in % 2000000) == 0)
				{
					QString szTmp;
					KviQString::sprintf(szTmp,QString(" (%d of %d bytes)"),zstr.total_in,uSize);
					QString szPrg = szProgressText + szTmp;
					if(!updateProgress(m_iCurrentProgress,szPrg))
						return false; // aborted
				}
			}
		}

		// flush pending output
		zstr.next_out = obuffer;
		zstr.avail_out = BUFFER_SIZE;

		int ret;
		do
		{
			ret = deflate(&zstr,Z_FINISH);

			if((ret == Z_OK) || (ret == Z_STREAM_END))
			{
				if(zstr.avail_out < BUFFER_SIZE)
				{
					int iCompressed = zstr.next_out - obuffer;
					pDataField->m_uWrittenFieldLength += iCompressed;
					if(pFile->write((char *)obuffer,iCompressed) != iCompressed)
					{
						deflateEnd(&zstr);
						return writeError();
					}
				} else {
					deflateEnd(&zstr);
					setLastError(__tr2qs("Compression library internal error"));
					return false;
				}

				zstr.next_out = obuffer;
				zstr.avail_out = BUFFER_SIZE;
			}

		} while(ret == Z_OK);

		// store the compressed data size
		kvi_file_offset_t here = pFile->pos();
		pFile->seek(savedSizeOffset);
		uSize = zstr.total_out;

		deflateEnd(&zstr);
		if(!pFile->save(uSize))return writeError();

		if(ret != Z_STREAM_END)
		{
			setLastError(__tr2qs("Error while compressing a file stream"));
			return false;
		}

		pFile->seek(here);
	} else {
#endif
		unsigned char buffer[BUFFER_SIZE];
		int iTotalFileSize = 0;
		kvi_i32_t iReaded = source.read((char *)buffer,BUFFER_SIZE);
		if(iReaded < 0)
			return readError();
		while(iReaded > 0)
		{
			iTotalFileSize += iReaded;
			if((iTotalFileSize % 1000000) == 0)
			{
				QString szTmp;
				KviQString::sprintf(szTmp,QString(" (%d of %d bytes)"),iTotalFileSize,uSize);
				QString szPrg = szProgressText + szTmp;
				if(!updateProgress(m_iCurrentProgress,szPrg))
					return false; // aborted
			}
			pDataField->m_uWrittenFieldLength += iReaded;
			if(pFile->write((char *)buffer,iReaded) != iReaded)
				return writeError();
			iReaded = source.read((char *)buffer,BUFFER_SIZE);
		}
#ifdef COMPILE_ZLIB_SUPPORT
	}
#endif
	source.close();

	return true;
}

bool KviPackageWriter::pack(const QString &szFileName,kvi_u32_t uPackFlags)
{
	m_iCurrentProgress = 0;
	if(!(uPackFlags & NoProgressDialog))
	{
		showProgressDialog(__tr2qs("Creating package..."),100);
		updateProgress(m_iCurrentProgress,__tr2qs("Writing package header"));
	}

	bool bRet = packInternal(szFileName,uPackFlags);

	hideProgressDialog();
	return bRet;
}

bool KviPackageWriter::packInternal(const QString &szFileName,kvi_u32_t)
{

	KviFile f(szFileName);
	if(!f.open(QFile::WriteOnly | QFile::Truncate))
	{
		setLastError(__tr2qs("Can't open file for writing"));
		return false;
	}

	// write the PackageHeader

	// Magic
	char magic[4];
	magic[0] = 'K';
	magic[1] = 'V';
	magic[2] = 'P';
	magic[3] = 'F';
	if(f.write(magic,4) != 4)
		return writeError();

	// Version
	kvi_u32_t uVersion = 0x1;
	if(!f.save(uVersion))
		return writeError();

	// Flags
	kvi_u32_t uFlags = 0x0;
	if(!f.save(uFlags))
		return writeError();

	// write PackageInfo

	// InfoFieldCount
	kvi_u32_t uCount = m_pStringInfoFields->count() + m_pBinaryInfoFields->count();
	if(!f.save(uCount))
		return writeError();

	m_iCurrentProgress = 5;
	if(!updateProgress(m_iCurrentProgress,__tr2qs("Writing informational fields")))
		return false; // aborted

	// InfoFields (string)
	KviPointerHashTableIterator<QString,QString> it(*m_pStringInfoFields);
	while(QString * s = it.current())
	{
		if(!f.save(it.currentKey()))
			return writeError();
		kvi_u32_t uType = KVI_PACKAGE_INFOFIELD_TYPE_STRING;
		if(!f.save(uType))
			return writeError();
		if(!f.save(*s))
			return writeError();
		++it;
	}

	// InfoFields (binary)
	KviPointerHashTableIterator<QString,QByteArray> it2(*m_pBinaryInfoFields);
	while(QByteArray * b = it2.current())
	{
		if(!f.save(it2.currentKey()))
			return writeError();
		kvi_u32_t uType = KVI_PACKAGE_INFOFIELD_TYPE_BINARYBUFFER;
		if(!f.save(uType))
			return writeError();
		if(!f.save(*b))
			return writeError();
		++it2;
	}

	m_iCurrentProgress = 10;
	if(!updateProgress(m_iCurrentProgress,__tr2qs("Writing package data")))
		return false; // aborted

	// write PackageData
	int iIdx = 0;
	for(DataField * pDataField = m_pDataFields->first();pDataField;pDataField = m_pDataFields->next())
	{
		kvi_u32_t uDataFieldType = pDataField->m_uType;
		if(!f.save(uDataFieldType))return writeError();

		kvi_file_offset_t savedLenOffset = f.pos();
		// here we will store the length of the field once it's written
		if(!f.save(uDataFieldType))return writeError();

		m_iCurrentProgress = 10 + ((90 * iIdx) / m_pDataFields->count());

		switch(pDataField->m_uType)
		{
			case KVI_PACKAGE_DATAFIELD_TYPE_FILE:
				if(!packFile(&f,pDataField))
					return false;
			break;
			default:
				setLastError(__tr2qs("Internal error"));
				return false;
			break;
		}

		kvi_file_offset_t savedEndOffset = f.pos();
		f.seek(savedLenOffset);
		if(!f.save(pDataField->m_uWrittenFieldLength))
			return writeError();

		f.seek(savedEndOffset);
		iIdx++;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Da Reader

KviPackageReader::KviPackageReader()
: KviPackageIOEngine()
{
}

KviPackageReader::~KviPackageReader()
{
}

bool KviPackageReader::readHeaderInternal(KviFile * pFile,const QString &)
{
	// read the PackageHeader

	// Magic
	char magic[4];

	if(pFile->read(magic,4) != 4)
		return readError();

	if((magic[0] != 'K') || (magic[1] != 'V') || (magic[2] != 'P') || (magic[3] != 'F'))
	{
		setLastError(__tr2qs("The file specified is not a valid KVIrc package"));
		return false;
	}

	// Version
	kvi_u32_t uVersion;
	if(!pFile->load(uVersion))
		return readError();
	if(uVersion != 0x1)
	{
		setLastError(__tr2qs("The package has an invalid version number, it might have been created by a newer KVIrc"));
		return false;
	}

	// Flags
	kvi_u32_t uFlags;
	if(!pFile->load(uFlags))
		return readError();
	// we ignore them at the moment

	// read PackageInfo

	// InfoFieldCount
	kvi_u32_t uCount;
	if(!pFile->load(uCount))
		return writeError();

	m_pStringInfoFields->clear();
	m_pBinaryInfoFields->clear();

	kvi_u32_t uIdx = 0;
	while(uIdx < uCount)
	{
		QString szKey;
		if(!pFile->load(szKey))
			return readError();
		kvi_u32_t uFieldType;
		if(!pFile->load(uFieldType))
			return readError();
		switch(uFieldType)
		{
			case KVI_PACKAGE_INFOFIELD_TYPE_STRING:
			{
				QString szValue;
				if(!pFile->load(szValue))return readError();
				m_pStringInfoFields->replace(szKey,new QString(szValue));
			}
			break;
			case KVI_PACKAGE_INFOFIELD_TYPE_BINARYBUFFER:
			{
				QByteArray * pbValue = new QByteArray();
				if(!pFile->load(*pbValue))
				{
					delete pbValue;
					return readError();
				}
				m_pBinaryInfoFields->replace(szKey,pbValue);
			}
			break;
			default:
				setLastError(__tr2qs("Invalid info field: the package is probably corrupt"));
			break;
		}
		uIdx++;
	}

	return true;
}

bool KviPackageReader::readHeader(const QString &szLocalFileName)
{
	KviFile f(szLocalFileName);
	if(!f.open(QFile::ReadOnly))
	{
		setLastError(__tr2qs("Can't open file for reading"));
		return false;
	}

	return readHeaderInternal(&f,szLocalFileName);
}

bool KviPackageReader::unpackFile(KviFile * pFile,const QString &szUnpackPath)
{
	// Flags
	kvi_u32_t uFlags;
	if(!pFile->load(uFlags))
		return readError();

#ifndef COMPILE_ZLIB_SUPPORT
	if(uFlags & KVI_PACKAGE_DATAFIELD_FLAG_FILE_DEFLATE)
	{
		setLastError(__tr2qs("The package contains compressed data but this executable does not support compression"));
		return false;
	}
#endif

	// Path
	QString szPath;
	if(!pFile->load(szPath))
		return readError();

	QString szFileName = szUnpackPath;
	KviQString::ensureLastCharIs(szFileName,QChar(KVI_PATH_SEPARATOR_CHAR));

	szFileName += szPath;

	// no attacks please :)
	szFileName.replace(QString("..\\"),QString(""));
	szFileName.replace(QString("..//"),QString(""));

	KviFileUtils::adjustFilePath(szFileName);

	int idx = KviQString::findRev(szFileName,QChar(KVI_PATH_SEPARATOR_CHAR));
	if(idx != -1)
	{
		QString szPrefixPath = szFileName.left(idx);
		if(!KviFileUtils::makeDir(szPrefixPath))
		{
			setLastError(__tr2qs("Failed to create the target directory"));
			return false;
		}
	}

	KviFile dest(szFileName);
	if(!dest.open(QFile::WriteOnly | QFile::Truncate))
	{
		setLastError(__tr2qs("Failed to open a source file for reading"));
		return false;
	}

	QString szProgressText;
	KviQString::sprintf(szProgressText,__tr2qs("Unpacking file %Q"),&szFileName);
	if(!updateProgress(pFile->pos(),szProgressText))
		return false; // aborted

	// Size
	kvi_u32_t uSize;
	if(!pFile->load(uSize))return readError();


	// FilePayload
#ifdef COMPILE_ZLIB_SUPPORT
	if(uFlags & KVI_PACKAGE_DATAFIELD_FLAG_FILE_DEFLATE)
	{
		int iRemainingSize = uSize;
		unsigned char ibuffer[BUFFER_SIZE];
		unsigned char obuffer[BUFFER_SIZE];

		int iToRead = iRemainingSize;
		if(iToRead > BUFFER_SIZE)iToRead = BUFFER_SIZE;
		int iReaded = pFile->read((char *)ibuffer,iToRead);
		iRemainingSize -= iReaded;

		z_stream zstr;
		zstr.zalloc = Z_NULL;
		zstr.zfree = Z_NULL;
		zstr.opaque = Z_NULL;
		zstr.next_in = ibuffer;
		zstr.avail_in = iReaded;
		zstr.next_out = obuffer;
		zstr.avail_out = BUFFER_SIZE;

		if(inflateInit(&zstr) != Z_OK)
		{
			setLastError(__tr2qs("Compression library initialization error"));
			return false;
		}

		while((iReaded > 0) && (iRemainingSize > 0))
		{
			zstr.next_out = obuffer;
			zstr.avail_out = BUFFER_SIZE;

			if(inflate(&zstr,Z_NO_FLUSH) != Z_OK)
			{
				setLastError(__tr2qs("Compression library error"));
				return false;
			}

			if(zstr.avail_out < BUFFER_SIZE)
			{
				int iDecompressed = zstr.next_out - obuffer;
				if(dest.write((char *)obuffer,iDecompressed) != iDecompressed)
				{
					inflateEnd(&zstr);
					return writeError();
				}
			}

			if(zstr.avail_in < BUFFER_SIZE)
			{
				int iDataToRead = BUFFER_SIZE - zstr.avail_in;
				if(iDataToRead < BUFFER_SIZE)
				{
					if(ibuffer != zstr.next_in)
					{
						// hum, there is still some data in the buffer to be readed
						// and it is not at the beginning...move it to the beginning of ibuffer
						memmove(ibuffer,zstr.next_in,zstr.avail_in);
					}
				}

				if(iDataToRead > iRemainingSize)
					iDataToRead = iRemainingSize;

				iReaded = pFile->read((char *)(ibuffer + zstr.avail_in),iDataToRead);
				if(iReaded < 0)
				{
					inflateEnd(&zstr);
					return readError();
				}

				iRemainingSize -= iReaded;
				zstr.avail_in += iReaded;
				zstr.next_in = ibuffer;

				if((zstr.total_in % 2000000) == 0)
				{
					QString szTmp;
					KviQString::sprintf(szTmp,QString(" (%d of %d bytes)"),zstr.total_in,uSize);
					QString szPrg = szProgressText + szTmp;
					if(!updateProgress(pFile->pos(),szPrg))
						return false; // aborted
				}
			}
		}

		// flush pending output
		zstr.next_out = obuffer;
		zstr.avail_out = BUFFER_SIZE;

		int ret;

		do {
			ret = inflate(&zstr,Z_FINISH);

			if((ret == Z_OK) || (ret == Z_STREAM_END) || (ret == Z_BUF_ERROR))
			{
				if(zstr.avail_out < BUFFER_SIZE)
				{
					int iDecompressed = zstr.next_out - obuffer;
					if(dest.write((char *)obuffer,iDecompressed) != iDecompressed)
					{
						inflateEnd(&zstr);
						return writeError();
					}
				} else {
					//THIS HAPPENS FOR ZERO SIZE FILES
					debug("hum.... internal, rEWq (ret = %d) (avail_out = %d)",ret,zstr.avail_out);

					inflateEnd(&zstr);
					setLastError(__tr2qs("Compression library internal error"));
					return false;
				}
				zstr.next_out = obuffer;
				zstr.avail_out = BUFFER_SIZE;
			}

		} while((ret == Z_OK) || (ret == Z_BUF_ERROR));

		inflateEnd(&zstr);

		if(ret != Z_STREAM_END)
		{
			setLastError(__tr2qs("Error in compressed file stream"));
			return false;
		}

	} else {
#endif
		unsigned char buffer[BUFFER_SIZE];
		int iTotalFileSize = 0;
		int iRemainingData = uSize;
		int iToRead = iRemainingData;
		if(iToRead > BUFFER_SIZE)iToRead = BUFFER_SIZE;
		int iReaded = 1;

		while((iReaded > 0) && (iToRead > 0))
		{
			iReaded = pFile->read((char *)buffer,iToRead);
			if(iReaded > 0)
			{
				iTotalFileSize += iReaded;
				iRemainingData -= iReaded;

				if((iTotalFileSize % 3000000) == 0)
				{
					QString szTmp;
					KviQString::sprintf(szTmp,QString(" (%d of %d bytes)"),iTotalFileSize,uSize);
					QString szPrg = szProgressText + szTmp;
					if(!updateProgress(pFile->pos(),szPrg))
						return false; // aborted
				}

				if(dest.write((char *)buffer,iReaded) != iReaded)
					return writeError();
			}

			int iToRead = iRemainingData;
			if(iToRead > BUFFER_SIZE)iToRead = BUFFER_SIZE;
		}
#ifdef COMPILE_ZLIB_SUPPORT
	}
#endif
	dest.close();

	return true;
}

bool KviPackageReader::getStringInfoField(const QString &szName,QString &szBuffer)
{
	QString * pVal = m_pStringInfoFields->find(szName);
	if(!pVal)
		return false;
	szBuffer = *pVal;
	return true;
}

bool KviPackageReader::unpack(const QString &szLocalFileName,const QString &szUnpackPath,kvi_u32_t uUnpackFlags)
{
	bool bRet = unpackInternal(szLocalFileName,szUnpackPath,uUnpackFlags);
	hideProgressDialog();
	return bRet;
}

bool KviPackageReader::unpackInternal(const QString &szLocalFileName,const QString &szUnpackPath,kvi_u32_t uUnpackFlags)
{

	KviFile f(szLocalFileName);
	if(!f.open(QFile::ReadOnly))
	{
		setLastError(__tr2qs("Can't open file for reading"));
		return false;
	}

	kvi_file_offset_t size = f.size();

	if(!(uUnpackFlags & NoProgressDialog))
	{
		showProgressDialog(__tr2qs("Reading package..."),size);
		updateProgress(0,__tr2qs("Reading package header"));
	}

	if(!readHeaderInternal(&f,szLocalFileName))
		return false;

	if(!updateProgress(f.pos(),__tr2qs("Reading package data")))
		return false; // aborted

	while(!f.atEnd())
	{
		// DataFieldType
		kvi_u32_t uDataFieldType;
		if(!f.load(uDataFieldType))return readError();
		// DataFieldLen
		kvi_u32_t uDataFieldLen;
		if(!f.load(uDataFieldLen))return readError();

		switch(uDataFieldType)
		{
			case KVI_PACKAGE_DATAFIELD_TYPE_FILE:
				if(!unpackFile(&f,szUnpackPath))
					return false;
			break;
			default:
				setLastError(__tr2qs("Invalid data field: the package is probably corrupt"));
				return false;
			break;
		}
	}

	return true;
}
