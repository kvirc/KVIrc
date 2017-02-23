//=============================================================================
//
//   File : KviPackageWriter.cpp
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

#include "KviPackageWriter.h"
#include "KviFile.h"
#include "KviFileUtils.h"
#include "KviLocale.h"
#include "KviPointerList.h"
#include "kvi_inttypes.h"

#include <QByteArray>
#include <QDir>
#include <QString>

#ifdef COMPILE_ZLIB_SUPPORT
#include <zlib.h>
#endif

//
// See KviPackageIOEngine.cpp for the description of the KVIrc package file
//

class KviPackageWriterDataField
{
public:
	kvi_u32_t m_uType;
	// output length of the field
	kvi_u32_t m_uWrittenFieldLength;
	// data fields for the File KviPackageWriterDataFieldType
	bool m_bFileAllowCompression;
	QString m_szFileLocalName;
	QString m_szFileTargetName;
};

class KviPackageWriterPrivate
{
public:
	KviPointerList<KviPackageWriterDataField> * pDataFields;
	int iCurrentProgress;
};

KviPackageWriter::KviPackageWriter()
    : KviPackageIOEngine()
{
	m_p = new KviPackageWriterPrivate();
	m_p->pDataFields = new KviPointerList<KviPackageWriterDataField>();
	m_p->pDataFields->setAutoDelete(true);
}

KviPackageWriter::~KviPackageWriter()
{
	delete m_p->pDataFields;
	delete m_p;
}

void KviPackageWriter::addInfoField(const QString & szName, const QString & szValue)
{
	stringInfoFields()->replace(szName, new QString(szValue));
}

void KviPackageWriter::addInfoField(const QString & szName, QByteArray * pValue)
{
	binaryInfoFields()->replace(szName, pValue);
}

bool KviPackageWriter::addFile(const QString & szLocalFileName, const QString & szTargetFileName, kvi_u32_t uAddFileFlags)
{
	QFileInfo fi(szLocalFileName);
	return addFileInternal(&fi, szLocalFileName, szTargetFileName, uAddFileFlags);
}

bool KviPackageWriter::addFileInternal(const QFileInfo * fi, const QString & szLocalFileName, const QString & szTargetFileName, kvi_u32_t uAddFileFlags)
{
	if(!(fi->isFile() && fi->isReadable()))
		return false;

	if(!(uAddFileFlags & FollowSymLinks))
	{
		if(fi->isSymLink())
			return true; // do NOT add a symlink
	}

	KviPackageWriterDataField * f = new KviPackageWriterDataField();
	f->m_uType = KVI_PACKAGE_DATAFIELD_TYPE_FILE;
	f->m_bFileAllowCompression = !(uAddFileFlags & NoCompression);
	f->m_szFileLocalName = szLocalFileName;
	f->m_szFileTargetName = szTargetFileName;
	m_p->pDataFields->append(f);

	return true;
}

bool KviPackageWriter::addDirectory(const QString & szLocalDirectoryName, const QString & szTargetDirectoryPrefix, kvi_u32_t uAddFileFlags)
{
	QDir d(szLocalDirectoryName);
	QDir::Filters iFlags;

	iFlags = QDir::Files | QDir::Readable;
	if(!(uAddFileFlags & FollowSymLinks))
		iFlags |= QDir::NoSymLinks;

	int j;
	QFileInfoList sl = d.entryInfoList(iFlags);
	for(j = 0; j < sl.size(); j++)
	{
		QString szSFileName = szLocalDirectoryName;
		KviQString::ensureLastCharIs(szSFileName, QChar(KVI_PATH_SEPARATOR_CHAR));
		QFileInfo slowCopy = sl.at(j);
		szSFileName += slowCopy.fileName();

		QString szDFileName = szTargetDirectoryPrefix;
		KviQString::ensureLastCharIs(szDFileName, QChar(KVI_PATH_SEPARATOR_CHAR));
		szDFileName += slowCopy.fileName();
		if(!addFileInternal(&slowCopy, szSFileName, szDFileName, uAddFileFlags))
			return false;
	}
	iFlags = QDir::Dirs | QDir::Readable;
	if(!(uAddFileFlags & FollowSymLinks))
		iFlags |= QDir::NoSymLinks;
	sl = d.entryInfoList(iFlags);

	for(j = 0; j < sl.size(); j++)
	{
		QString szDir = sl.at(j).fileName();
		if(!KviQString::equalCS(szDir, "..") && !KviQString::equalCS(szDir, "."))
		{
			QString szSDirName = szLocalDirectoryName;
			KviQString::ensureLastCharIs(szSDirName, QChar(KVI_PATH_SEPARATOR_CHAR));
			szSDirName += szDir;
			QString szDDirName = szTargetDirectoryPrefix;
			KviQString::ensureLastCharIs(szDDirName, QChar(KVI_PATH_SEPARATOR_CHAR));
			szDDirName += szDir;
			if(!addDirectory(szSDirName, szDDirName, uAddFileFlags))
				return false;
		}
	}
	return true;
}

#define BUFFER_SIZE 32768

bool KviPackageWriter::packFile(KviFile * pFile, KviPackageWriterDataField * pDataField)
{
	QString szProgressText = QString(__tr2qs("Packaging file %1")).arg(pDataField->m_szFileLocalName);
	if(!updateProgress(m_p->iCurrentProgress, szProgressText))
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
	kvi_u32_t uFlags = pDataField->m_bFileAllowCompression ? (uSize > 64 ? KVI_PACKAGE_DATAFIELD_FLAG_FILE_DEFLATE : 0)
	                                                       : 0;
#else
	kvi_u32_t uFlags = 0;
#endif

	if(!pFile->save(uFlags))
		return writeError();

	QByteArray szTargetFileName = pDataField->m_szFileTargetName.toUtf8();

	// Path
	if(!pFile->save(szTargetFileName))
		return writeError();

	kvi_file_offset_t savedSizeOffset = pFile->pos();

	// Size : will update it if compression is requested
	if(!pFile->save(uSize))
		return writeError();

	pDataField->m_uWrittenFieldLength = 4 + 4 + 4 + szTargetFileName.length(); // sizeof(flags + uncompressed size + path len + path)

// FilePayload
#ifdef COMPILE_ZLIB_SUPPORT
	if(uFlags & KVI_PACKAGE_DATAFIELD_FLAG_FILE_DEFLATE)
	{
		unsigned char ibuffer[BUFFER_SIZE];
		unsigned char obuffer[BUFFER_SIZE];

		kvi_i32_t iReaded = source.read((char *)ibuffer, BUFFER_SIZE);
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

		if(deflateInit(&zstr, 9) != Z_OK)
		{
			setLastError(__tr2qs("Compression library initialization error"));
			return false;
		}

		while(iReaded > 0)
		{
			zstr.next_out = obuffer;
			zstr.avail_out = BUFFER_SIZE;

			if(deflate(&zstr, Z_NO_FLUSH) != Z_OK)
			{
				setLastError(__tr2qs("Compression library error"));
				return false;
			}

			if(zstr.avail_out < BUFFER_SIZE)
			{
				int iCompressed = zstr.next_out - obuffer;
				pDataField->m_uWrittenFieldLength += iCompressed;
				if(pFile->write((char *)obuffer, iCompressed) != iCompressed)
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
						memmove(ibuffer, zstr.next_in, zstr.avail_in);
					}
				}
				iReaded = source.read((char *)(ibuffer + zstr.avail_in), iDataToRead);
				if(iReaded < 0)
				{
					deflateEnd(&zstr);
					return readError();
				}
				zstr.avail_in += iReaded;
				zstr.next_in = ibuffer;

				if((zstr.total_in % 2000000) == 0)
				{
					QString szTmp = QString(" (%1 of %2 bytes)").arg(zstr.total_in, uSize);
					QString szPrg = szProgressText + szTmp;
					if(!updateProgress(m_p->iCurrentProgress, szPrg))
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
			ret = deflate(&zstr, Z_FINISH);

			if((ret == Z_OK) || (ret == Z_STREAM_END))
			{
				if(zstr.avail_out < BUFFER_SIZE)
				{
					int iCompressed = zstr.next_out - obuffer;
					pDataField->m_uWrittenFieldLength += iCompressed;
					if(pFile->write((char *)obuffer, iCompressed) != iCompressed)
					{
						deflateEnd(&zstr);
						return writeError();
					}
				}
				else
				{
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
		if(!pFile->save(uSize))
			return writeError();

		if(ret != Z_STREAM_END)
		{
			setLastError(__tr2qs("Error while compressing a file stream"));
			return false;
		}

		pFile->seek(here);
	}
	else
	{
#endif
		unsigned char buffer[BUFFER_SIZE];
		int iTotalFileSize = 0;
		kvi_i32_t iReaded = source.read((char *)buffer, BUFFER_SIZE);
		if(iReaded < 0)
			return readError();
		while(iReaded > 0)
		{
			iTotalFileSize += iReaded;
			if((iTotalFileSize % 1000000) == 0)
			{
				QString szTmp = QString(" (%1 of %2 bytes)").arg(iTotalFileSize, uSize);
				QString szPrg = szProgressText + szTmp;
				if(!updateProgress(m_p->iCurrentProgress, szPrg))
					return false; // aborted
			}
			pDataField->m_uWrittenFieldLength += iReaded;
			if(pFile->write((char *)buffer, iReaded) != iReaded)
				return writeError();
			iReaded = source.read((char *)buffer, BUFFER_SIZE);
		}
#ifdef COMPILE_ZLIB_SUPPORT
	}
#endif
	source.close();

	return true;
}

bool KviPackageWriter::pack(const QString & szFileName, kvi_u32_t uPackFlags)
{
	m_p->iCurrentProgress = 0;
	if(!(uPackFlags & NoProgressDialog))
	{
		showProgressDialog(__tr2qs("Creating package..."), 100);
		updateProgress(m_p->iCurrentProgress, __tr2qs("Writing package header"));
	}

	bool bRet = packInternal(szFileName, uPackFlags);

	hideProgressDialog();
	return bRet;
}

bool KviPackageWriter::packInternal(const QString & szFileName, kvi_u32_t)
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
	if(f.write(magic, 4) != 4)
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
	kvi_u32_t uCount = stringInfoFields()->count() + binaryInfoFields()->count();
	if(!f.save(uCount))
		return writeError();

	m_p->iCurrentProgress = 5;
	if(!updateProgress(m_p->iCurrentProgress, __tr2qs("Writing informational fields")))
		return false; // aborted

	// InfoFields (string)
	KviPointerHashTableIterator<QString, QString> it(*stringInfoFields());
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
	KviPointerHashTableIterator<QString, QByteArray> it2(*binaryInfoFields());
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

	m_p->iCurrentProgress = 10;
	if(!updateProgress(m_p->iCurrentProgress, __tr2qs("Writing package data")))
		return false; // aborted

	// write PackageData
	int iIdx = 0;
	for(KviPackageWriterDataField * pDataField = m_p->pDataFields->first(); pDataField; pDataField = m_p->pDataFields->next())
	{
		kvi_u32_t uKviPackageWriterDataFieldType = pDataField->m_uType;
		if(!f.save(uKviPackageWriterDataFieldType))
			return writeError();

		kvi_file_offset_t savedLenOffset = f.pos();
		// here we will store the length of the field once it's written
		if(!f.save(uKviPackageWriterDataFieldType))
			return writeError();

		m_p->iCurrentProgress = 10 + ((90 * iIdx) / m_p->pDataFields->count());

		switch(pDataField->m_uType)
		{
			case KVI_PACKAGE_DATAFIELD_TYPE_FILE:
				if(!packFile(&f, pDataField))
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
