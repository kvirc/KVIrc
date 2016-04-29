//=============================================================================
//
//   File : KvsObject_memoryBuffer.cpp
//   Creation date : Fri Mar 18 21:30:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005-2008 Alessandro Carbone (elfonol at gmail dot com)
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

#include "KvsObject_memoryBuffer.h"

#include "KviError.h"
#include "kvi_debug.h"
#include "kvi_settings.h"
#include "KviLocale.h"
#include "QProcess"
#include "KviFileUtils.h"
#include "KviFile.h"

#include <QFileInfo>

/*
	@doc:	memorybuffer
	@keyterms:
		object class
	@title:
		memorybuffer class
	@type:
		class
	@short:
		A class container to manage binary data.
	@inherits:
		[class]object[/class]
	@description:
		The memorybuffer class  is used to storage binary data[br]
	@functions:
		!fn: $loadFromFile(<file_name:string>)
		Load into memorybuffer the <file_name> file.
		!fn: $saveToFile(<file_name:string>)
		Save the memorybuffer to <file_name> file.
		!fn: $clear()
		Clear the memorybuffer.

*/

KVSO_BEGIN_REGISTERCLASS(KvsObject_memoryBuffer, "memorybuffer", "object")
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_memoryBuffer, loadFromFile);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_memoryBuffer, clear);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_memoryBuffer, saveToFile);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_memoryBuffer, size);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_memoryBuffer, readByteAt);
KVSO_END_REGISTERCLASS(KvsObject_memoryBuffer)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_memoryBuffer, KviKvsObject)

mIdx = 0;
m_pBuffer = new QByteArray();

KVSO_END_CONSTRUCTOR(KvsObject_memoryBuffer)

KVSO_BEGIN_DESTRUCTOR(KvsObject_memoryBuffer)

delete m_pBuffer;

KVSO_END_CONSTRUCTOR(KvsObject_memoryBuffer)

KVSO_CLASS_FUNCTION(memoryBuffer, clear)
{
	CHECK_INTERNAL_POINTER(m_pBuffer)
	m_pBuffer->clear();
	return true;
}

KVSO_CLASS_FUNCTION(memoryBuffer, loadFromFile)
{
	CHECK_INTERNAL_POINTER(m_pBuffer)
	QString szFileName;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("filename", KVS_PT_NONEMPTYSTRING, 0, szFileName)
	KVSO_PARAMETERS_END(c)
	if(KviFileUtils::fileExists(szFileName))
	{

		KviFile f(szFileName);
		qDebug("loading file %s", szFileName.toUtf8().data());
		if(f.open(QIODevice::ReadOnly))
		{
			m_pBuffer->resize(f.size());
			f.read(m_pBuffer->data(), f.size());
			f.close();
		}
		else
			qDebug("Error in loaded file!");
	}
	else
		c->warning(__tr2qs_ctx("The file '%Q' doesn't exist", "objects"), &szFileName);
	return true;
}
KVSO_CLASS_FUNCTION(memoryBuffer, size)
{
	CHECK_INTERNAL_POINTER(m_pBuffer)
	c->returnValue()->setInteger((kvs_int_t)m_pBuffer->size());
	return true;
}

KVSO_CLASS_FUNCTION(memoryBuffer, readByteAt)
{
	CHECK_INTERNAL_POINTER(m_pBuffer)
	kvs_int_t iIdx;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("index", KVS_PT_INT, 0, iIdx)
	KVSO_PARAMETERS_END(c)
	if(iIdx > m_pBuffer->size())
	{
		c->warning(__tr2qs_ctx("Index '%I' out of the buffer size", "objects"), &iIdx);
		return true;
	}
	unsigned char ch = m_pBuffer->at(iIdx);
	c->returnValue()->setInteger(ch);
	return true;
}
KVSO_CLASS_FUNCTION(memoryBuffer, saveToFile)
{
	CHECK_INTERNAL_POINTER(m_pBuffer)
	QString szFileName;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("filename", KVS_PT_NONEMPTYSTRING, 0, szFileName)
	KVSO_PARAMETERS_END(c)
	KviFile f(szFileName);
	if(f.open(QIODevice::WriteOnly))
	{
		f.write(m_pBuffer->data(), m_pBuffer->size());
		f.close();
	}
	else
	{
		c->warning(__tr2qs_ctx("Can't open file '%Q' for writing", "objects"), &szFileName);
	}
	return true;
}
