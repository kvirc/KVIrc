#ifndef _KVI_FILE_H_
#define _KVI_FILE_H_
//=============================================================================
//
//   File : KviFile.h
//   Creation date : Mon Dec 17 2001 00:05:04 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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
* \file KviFile.h
* \author Szymon Stefanek
* \brief This file handles file operations performed by KVIrc
*/

#include "kvi_settings.h"
#include "KviHeapObject.h"
#include "KviPointerList.h"
#include "kvi_inttypes.h"

#include <QFile>

class KviCString;
class QByteArray;

#define kvi_file_offset_t qlonglong

/**
* \class KviFile
* \brief KVIrc File class
*/
class KVILIB_API KviFile : public QFile, public KviHeapObject
{
public:
	/**
	* \brief Constructs file object
	* \return KviFile
	*/
	KviFile();

	/**
	* \brief Constructs file object
	* \param szName The name of the file
	* \return KviFile
	*/
	KviFile(const QString & szName);

	/**
	* \brief Destroys file object
	*/
	~KviFile();

public:
	/**
	* \brief Saves a file
	* \param file The file to save
	* \warning This function saves LITTLE ENDIAN DATA!
	* \return bool
	*/
	bool save(kvi_u64_t file);

	/**
	* \brief Saves a file
	* \param file The file to save
	* \warning This function saves LITTLE ENDIAN DATA!
	* \return bool
	*/
	bool save(kvi_i64_t file) { return save((kvi_u64_t)file); };

	/**
	* \brief Saves a file
	* \param file The file to save
	* \warning This function saves LITTLE ENDIAN DATA!
	* \return bool
	*/
	bool save(kvi_u32_t file);

	/**
	* \brief Saves a file
	* \param file The file to save
	* \warning This function saves LITTLE ENDIAN DATA!
	* \return bool
	*/
	bool save(kvi_i32_t file) { return save((kvi_u32_t)file); };

	/**
	* \brief Saves a file
	* \param file The file to save
	* \warning This function saves LITTLE ENDIAN DATA!
	* \return bool
	*/
	bool save(kvi_u16_t file);

	/**
	* \brief Saves a file
	* \param file The file to save
	* \warning This function saves LITTLE ENDIAN DATA!
	* \return bool
	*/
	bool save(kvi_i16_t file) { return save((kvi_u16_t)file); };

	/**
	* \brief Saves a file
	* \param file The file to save
	* \warning This function saves LITTLE ENDIAN DATA!
	* \return bool
	*/
	bool save(kvi_u8_t file);

	/**
	* \brief Saves a file
	* \param file The file to save
	* \warning This function saves LITTLE ENDIAN DATA!
	* \return bool
	*/
	bool save(kvi_i8_t file) { return save((kvi_u8_t)file); };

	/**
	* \brief Saves a file
	* \param szData The file to save
	* \warning This function saves LITTLE ENDIAN DATA!
	* \return bool
	*/
	bool save(const KviCString & szData);

	/**
	* \brief Saves a file
	* \param data The file to save
	* \warning This function saves LITTLE ENDIAN DATA!
	* \return bool
	*/
	bool save(const QByteArray & data);

	/**
	* \brief Saves a file
	* \param szData The file to save
	* \warning This function saves LITTLE ENDIAN DATA!
	* \return bool
	*/
	bool save(const QString & szData);

	/**
	* \brief Saves a file to a list
	* \param pData The list where file to save is
	* \warning This function saves LITTLE ENDIAN DATA!
	* \return bool
	*/
	bool save(KviPointerList<KviCString> * pData);

	/**
	* \brief Loads a file
	* \param file The file to load
	* \warning This function loads LITTLE ENDIAN DATA!
	* \return bool
	*/
	bool load(kvi_u64_t & file);

	/**
	* \brief Loads a file
	* \param file The file to load
	* \warning This function loads LITTLE ENDIAN DATA!
	* \return bool
	*/
	bool load(kvi_i64_t & file) { return load((kvi_u64_t &)file); };

	/**
	* \brief Loads a file
	* \param file The file to load
	* \warning This function loads LITTLE ENDIAN DATA!
	* \return bool
	*/
	bool load(kvi_u32_t & file);

	/**
	* \brief Loads a file
	* \param file The file to load
	* \warning This function loads LITTLE ENDIAN DATA!
	* \return bool
	*/
	bool load(kvi_i32_t & file) { return load((kvi_u32_t &)file); };

	/**
	* \brief Loads a file
	* \param file The file to load
	* \warning This function loads LITTLE ENDIAN DATA!
	* \return bool
	*/
	bool load(kvi_u16_t & file);

	/**
	* \brief Loads a file
	* \param file The file to load
	* \warning This function loads LITTLE ENDIAN DATA!
	* \return bool
	*/
	bool load(kvi_i16_t & file) { return load((kvi_u16_t &)file); };

	/**
	* \brief Loads a file
	* \param file The file to load
	* \warning This function loads LITTLE ENDIAN DATA!
	* \return bool
	*/
	bool load(kvi_u8_t & file);

	/**
	* \brief Loads a file
	* \param file The file to load
	* \warning This function loads LITTLE ENDIAN DATA!
	* \return bool
	*/
	bool load(kvi_i8_t & file) { return load((kvi_u8_t &)file); };

	/**
	* \brief Loads a file
	* \param szData The file to load
	* \warning This function loads LITTLE ENDIAN DATA!
	* \return bool
	*/
	bool load(KviCString & szData);

	/**
	* \brief Loads a file
	* \param data The file to load
	* \warning This function loads LITTLE ENDIAN DATA!
	* \return bool
	*/
	bool load(QByteArray & data);

	/**
	* \brief Loads a file
	* \param szData The file to load
	* \warning This function loads LITTLE ENDIAN DATA!
	* \return bool
	*/
	bool load(QString & szData);

	/**
	* \brief Loads a file from a list
	* \param pData The list of files to load
	* \warning This function loads LITTLE ENDIAN DATA!
	* \return bool
	*/
	bool load(KviPointerList<KviCString> * pData);

	/**
	* \brief Skips the first occurrence of the given char for a given dimension
	* \param t The char to find and skip
	* \param uMaxDist The dimension of the search field
	* \return bool
	*/
	bool skipFirst(char t, unsigned int uMaxDist = 0xffffffff);

	/**
	* \brief Skips the first occurrence of the given string for a given dimension
	* \param szText The string to find and skip
	* \param uMaxDist The dimension of the search field
	* \return bool
	*/
	bool skipFirst(const KviCString & szText, unsigned int uMaxDist = 0xffffffff);
};

#endif //_KVI_FILE_H_
