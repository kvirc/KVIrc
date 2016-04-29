#ifndef _CLASS_XMLREADER_H_
#define _CLASS_XMLREADER_H_
//=============================================================================
//
//   File : KvsObject_xmlreader.h
//   Created on Tue 27 Dec 2005 00:14:09 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2005-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "kvi_settings.h"
#include "KviQString.h"

#include "object_macros.h"

class KvsObject_xmlReader : public KviKvsObject
{
public:
	KVSO_DECLARE_OBJECT(KvsObject_xmlReader)
public:
	bool parse(KviKvsObjectFunctionCall * c);
	bool lastError(KviKvsObjectFunctionCall * c);

	void fatalError(const QString & szError);

protected:
	QString m_szLastError;
};

#endif //!_CLASS_XMLREADER_H_
