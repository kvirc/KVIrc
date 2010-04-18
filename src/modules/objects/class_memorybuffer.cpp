//=============================================================================
//
//   File : class_memorybuffer.cpp
//   Creation date : Fri Mar 18 21:30:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2005-2008 Alessandro Carbone (elfonol at gmail dot com)
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

#include "class_memorybuffer.h"

#include "kvi_error.h"
#include "kvi_debug.h"
#include "kvi_settings.h"
#include "kvi_locale.h"
#include "kvi_process.h"
#include "kvi_fileutils.h"
#include "kvi_file.h"

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

*/

KVSO_BEGIN_REGISTERCLASS(KviKvsObject_memorybuffer,"memorybuffer","object")
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_memorybuffer,loadFromFile);
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_memorybuffer,saveToFile);
KVSO_END_REGISTERCLASS(KviKvsObject_memorybuffer)


KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_memorybuffer,KviKvsObject)

	m_pBuffer = new QByteArray();

KVSO_END_CONSTRUCTOR(KviKvsObject_memorybuffer)

KVSO_BEGIN_DESTRUCTOR(KviKvsObject_memorybuffer)

	delete m_pBuffer;

KVSO_END_CONSTRUCTOR(KviKvsObject_memorybuffer)

KVSO_CLASS_FUNCTION(memorybuffer,loadFromFile)
{
	CHECK_INTERNAL_POINTER(m_pBuffer)
	QString szFileName;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("filename",KVS_PT_NONEMPTYSTRING,0,szFileName)
	KVSO_PARAMETERS_END(c)
	if(KviFileUtils::fileExists(szFileName))
	{

		KviFile f(szFileName);
		debug("loading file %s",szFileName.toUtf8().data());
		if (f.open(QIODevice::ReadOnly))
		{
			m_pBuffer->resize(f.size());
			f.read(m_pBuffer->data(),f.size());
			f.close();
		}
		else debug("Error in load file!");
	}
	else c->warning(__tr2qs_ctx("The file '%Q' does not exist","objects"),&szFileName);
	return true;
}
KVSO_CLASS_FUNCTION(memorybuffer,saveToFile)
{
	CHECK_INTERNAL_POINTER(m_pBuffer)
	QString szFileName;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("filename",KVS_PT_NONEMPTYSTRING,0,szFileName)
	KVSO_PARAMETERS_END(c)
	KviFile f(szFileName);
	if (f.open(QIODevice::WriteOnly))
	{
		f.write(m_pBuffer->data(),m_pBuffer->size());
		f.close();
	}
	else
	{
		c->warning(__tr2qs_ctx("Cannot open file '%Q' for writing","objects"),&szFileName);
	}
	return true;
}
/*
#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_class_memorybuffer.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES*/


