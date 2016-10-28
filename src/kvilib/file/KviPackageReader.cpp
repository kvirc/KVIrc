//=============================================================================
//
//   File : KviPackageReader.cpp
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

#include "KviPackageReader.h"
#include "KviFile.h"
#include "KviFileUtils.h"
#include "KviLocale.h"
#include "kvi_inttypes.h"

#include <QString>

#ifdef COMPILE_ZLIB_SUPPORT
#include <zlib.h>
#endif

//
// See KviPackageIOEngine.cpp for the description of the KVIrc package file
//

KviPackageReader::KviPackageReader()
    : KviPackageIOEngine()
{
}

KviPackageReader::~KviPackageReader()
    = default;

bool KviPackageReader::readHeaderInternal(KviFile * pFile, const QString &)
{
	// read the PackageHeader
	// Magic
	char magic[4];

	if(pFile->read(magic, 4) != 4)
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

	stringInfoFields()->clear();
	binaryInfoFields()->clear();

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
				if(!pFile->load(szValue))
					return readError();
				stringInfoFields()->replace(szKey, new QString(szValue));
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
				binaryInfoFields()->replace(szKey, pbValue);
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

bool KviPackageReader::readHeader(const QString & szLocalFileName)
{
	KviFile f(szLocalFileName);
	if(!f.open(QFile::ReadOnly))
	{
		setLastError(__tr2qs("Can't open file for reading"));
		return false;
	}

	return readHeaderInternal(&f, szLocalFileName);
}

#define BUFFER_SIZE 32768

bool KviPackageReader::unpackFile(KviFile * pFile, const QString & szUnpackPath)
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
	KviQString::ensureLastCharIs(szFileName, QChar(KVI_PATH_SEPARATOR_CHAR));

	szFileName += szPath;

	// no attacks please :)
	szFileName.replace(QString("..\\"), QString(""));
	szFileName.replace(QString("../"), QString(""));

	KviFileUtils::adjustFilePath(szFileName);

	int idx = szFileName.lastIndexOf(QChar(KVI_PATH_SEPARATOR_CHAR));
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

	QString szProgressText = QString(__tr2qs("Unpacking file %1")).arg(szFileName);
	if(!updateProgress(pFile->pos(), szProgressText))
		return false; // aborted

	// Size
	kvi_u32_t uSize;
	if(!pFile->load(uSize))
		return readError();

// FilePayload
#ifdef COMPILE_ZLIB_SUPPORT
	if(uFlags & KVI_PACKAGE_DATAFIELD_FLAG_FILE_DEFLATE)
	{
		//qDebug ("loading compressed data");
		int iRemainingSize = uSize;
		unsigned char ibuffer[BUFFER_SIZE];
		unsigned char obuffer[BUFFER_SIZE];

		int iToRead = iRemainingSize;
		if(iToRead > BUFFER_SIZE)
			iToRead = BUFFER_SIZE;
		int iReaded = pFile->read((char *)ibuffer, iToRead);
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

			if(inflate(&zstr, Z_NO_FLUSH) != Z_OK)
			{
				setLastError(__tr2qs("Compression library error"));
				return false;
			}

			if(zstr.avail_out < BUFFER_SIZE)
			{
				int iDecompressed = zstr.next_out - obuffer;
				if(dest.write((char *)obuffer, iDecompressed) != iDecompressed)
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
						memmove(ibuffer, zstr.next_in, zstr.avail_in);
					}
				}

				if(iDataToRead > iRemainingSize)
					iDataToRead = iRemainingSize;

				iReaded = pFile->read((char *)(ibuffer + zstr.avail_in), iDataToRead);
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
					QString szTmp = QString(" (%1 of %2 bytes)").arg(zstr.total_in, uSize);
					QString szPrg = szProgressText + szTmp;
					if(!updateProgress(pFile->pos(), szPrg))
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
			ret = inflate(&zstr, Z_FINISH);

			if((ret == Z_OK) || (ret == Z_STREAM_END) || (ret == Z_BUF_ERROR))
			{
				if(zstr.avail_out < BUFFER_SIZE)
				{
					int iDecompressed = zstr.next_out - obuffer;
					if(dest.write((char *)obuffer, iDecompressed) != iDecompressed)
					{
						inflateEnd(&zstr);
						return writeError();
					}
				}
				else
				{
					//THIS HAPPENS FOR ZERO SIZE FILES
					qDebug("Hum... internal, rEWq (ret = %d) (avail_out = %d)", ret, zstr.avail_out);

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
	}
	else
	{
#endif
		//qDebug("Load uncompressed data");
		unsigned char buffer[BUFFER_SIZE];
		int iTotalFileSize = 0;
		int iRemainingData = uSize;
		int iToRead = iRemainingData;
		if(iToRead > BUFFER_SIZE)
			iToRead = BUFFER_SIZE;
		int iReaded = 1;
		//qDebug("iReaded %i and iToRead %i and index %i",iReaded,iToRead,pFile->pos());
		while((iReaded > 0) && (iToRead > 0))
		{
			iReaded = pFile->read((char *)buffer, iToRead);
			if(iReaded > 0)
			{
				iTotalFileSize += iReaded;
				iRemainingData -= iReaded;

				if((iTotalFileSize % 3000000) == 0)
				{
					QString szTmp = QString(" (%1 of %2 bytes)").arg(iTotalFileSize).arg(uSize);
					QString szPrg = szProgressText + szTmp;
					if(!updateProgress(pFile->pos(), szPrg))
						return false; // aborted
				}
				//qDebug("write file with size %i and name %s",iReaded,dest.fileName().toUtf8().data());
				if(dest.write((char *)buffer, iReaded) != iReaded)
					return writeError();
			}
			//qDebug("Remains %i", iRemainingData);
			iToRead = iRemainingData;
			if(iToRead > BUFFER_SIZE)
				iToRead = BUFFER_SIZE;
		}
//qDebug("finish to read %i and index %i",iReaded,pFile->pos());
#ifdef COMPILE_ZLIB_SUPPORT
	}
#endif
	dest.close();

	return true;
}

bool KviPackageReader::getStringInfoField(const QString & szName, QString & szBuffer)
{
	QString * pVal = stringInfoFields()->find(szName);
	if(!pVal)
		return false;
	szBuffer = *pVal;
	return true;
}

bool KviPackageReader::unpack(const QString & szLocalFileName, const QString & szUnpackPath, kvi_u32_t uUnpackFlags)
{
	bool bRet = unpackInternal(szLocalFileName, szUnpackPath, uUnpackFlags);
	hideProgressDialog();
	return bRet;
}

bool KviPackageReader::unpackInternal(const QString & szLocalFileName, const QString & szUnpackPath, kvi_u32_t uUnpackFlags)
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
		showProgressDialog(__tr2qs("Reading package..."), size);
		updateProgress(0, __tr2qs("Reading package header"));
	}

	if(!readHeaderInternal(&f, szLocalFileName))
		return false;

	if(!updateProgress(f.pos(), __tr2qs("Reading package data")))
		return false; // aborted

	while(!f.atEnd())
	{
		// DataFieldType
		kvi_u32_t uDataFieldType;
		if(!f.load(uDataFieldType))
			return readError();
		// DataFieldLen
		kvi_u32_t uDataFieldLen;
		if(!f.load(uDataFieldLen))
			return readError();

		switch(uDataFieldType)
		{
			case KVI_PACKAGE_DATAFIELD_TYPE_FILE:
				if(!unpackFile(&f, szUnpackPath))
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
