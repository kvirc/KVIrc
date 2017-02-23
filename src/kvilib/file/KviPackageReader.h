#ifndef _KviPackageReader_h_
#define _KviPackageReader_h_
//=============================================================================
//
//   File : KviPackageReader.h
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
* \file KviPackageReader.h
* \author Szymon Stefanek
* \brief File packaging utilities
*/

#include "kvi_settings.h"
#include "KviPackageIOEngine.h"

class KviFile;
class QString;

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
	enum UnpackFlags
	{
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

#endif //_KviPackageReader_h_
