//=============================================================================
//
//   File : KviPackageIOEngine.cpp
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

#include "KviPackageIOEngine.h"
#include "KviLocale.h"

#include <QApplication>
#include <QByteArray>
#include <QLabel>
#include <QProgressDialog>

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

//
// Da Base Engine
//

KviPackageIOEngine::KviPackageIOEngine()
{
	m_pProgressDialog = nullptr;
	m_pStringInfoFields = new KviPointerHashTable<QString, QString>();
	m_pStringInfoFields->setAutoDelete(true);
	m_pBinaryInfoFields = new KviPointerHashTable<QString, QByteArray>();
	m_pBinaryInfoFields->setAutoDelete(true);
}

KviPackageIOEngine::~KviPackageIOEngine()
{
	if(m_pProgressDialog)
		delete m_pProgressDialog;
	delete m_pStringInfoFields;
	delete m_pBinaryInfoFields;
}

bool KviPackageIOEngine::updateProgress(int iProgress, const QString & szLabel)
{
	if(!m_pProgressDialog)
		return true;

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

void KviPackageIOEngine::showProgressDialog(const QString & szCaption, int iTotalSteps)
{
	m_pProgressDialog = new QProgressDialog(QString(""), __tr2qs("Cancel"), 0, iTotalSteps, nullptr);
	m_pProgressDialog->setModal(true);
	m_pProgressDialog->setWindowTitle(szCaption);

	m_pProgressDialogLabel = new QLabel(m_pProgressDialog);
	m_pProgressDialogLabel->setMaximumSize(500, 300);
	m_pProgressDialog->setLabel(m_pProgressDialogLabel);
}

void KviPackageIOEngine::hideProgressDialog()
{
	if(!m_pProgressDialog)
		return;
	delete m_pProgressDialog;
	m_pProgressDialog = nullptr;
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
