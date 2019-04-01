#ifndef _KviPackageIOEngine_h_
#define _KviPackageIOEngine_h_
//=============================================================================
//
//   File : KviPackageIOEngine.h
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
* \file KviPackageIOEngine.h
* \author Szymon Stefanek
* \brief File packaging utilities
*/

#include "kvi_settings.h"

#include "KviPointerHashTable.h"

#include <QString>

class QByteArray;
class QProgressDialog;
class QLabel;

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

private:
	QString m_szLastError;
	KviPointerHashTable<QString, QString> * m_pStringInfoFields;
	KviPointerHashTable<QString, QByteArray> * m_pBinaryInfoFields;
	QProgressDialog * m_pProgressDialog;
	QLabel * m_pProgressDialogLabel = nullptr;

public:
	/**
	* \brief Returns the last error
	* \return const QString &
	*/
	const QString & lastError() { return m_szLastError; };

	/**
	* \brief Returns the string info fields
	* \return KviPointerHashTable<QString,QString> *
	*/
	KviPointerHashTable<QString, QString> * stringInfoFields() { return m_pStringInfoFields; };

	/**
	* \brief Returns the binary info fields
	* \return KviPointerHashTable<QString,QByteArray> *
	*/
	KviPointerHashTable<QString, QByteArray> * binaryInfoFields() { return m_pBinaryInfoFields; };
protected:
	/**
	* \brief Sets the last error
	* \param szLastError The string error
	* \return void
	*/
	void setLastError(const QString & szLastError) { m_szLastError = szLastError; };

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

#endif //_KviPackageIOEngine_h_
