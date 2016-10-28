#ifndef _CLASS_FILE_H_
#define _CLASS_FILE_H_
//=============================================================================
//
//   File : KvsObject_file.h
//   Creation date : Fri Mar 18 21:30:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005-2009 Alessandro Carbone (elfonol at gmail dot com)
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

#include "object_macros.h"
#include "KvsObject_widget.h"
#include "KviFile.h"

class KvsObject_file : public KvsObject_widget
{
public:
	KVSO_DECLARE_OBJECT(KvsObject_file)
protected:
	KviFile * m_pFile;

public:
	KviFile * file() { return m_pFile; };
	bool setName(KviKvsObjectFunctionCall * c);
	bool name(KviKvsObjectFunctionCall * c);
	bool open(KviKvsObjectFunctionCall * c);
	bool isOpen(KviKvsObjectFunctionCall * c);
	bool close(KviKvsObjectFunctionCall * c);
	bool flush(KviKvsObjectFunctionCall * c);
	bool size(KviKvsObjectFunctionCall * c);
	bool resize(KviKvsObjectFunctionCall * c);

	bool atEnd(KviKvsObjectFunctionCall * c);
	// int QFile at() const
	bool where(KviKvsObjectFunctionCall * c);
	// void QFile at(int)
	bool seek(KviKvsObjectFunctionCall * c);
	bool putch(KviKvsObjectFunctionCall * c);
	bool getch(KviKvsObjectFunctionCall * c);
	bool readByte(KviKvsObjectFunctionCall * c);
	bool unGetch(KviKvsObjectFunctionCall * c);

	bool readBlock(KviKvsObjectFunctionCall * c);
	bool writeBlock(KviKvsObjectFunctionCall * c);

	bool readLine(KviKvsObjectFunctionCall * c);
	bool writeLine(KviKvsObjectFunctionCall * c);

	bool readHexBlock(KviKvsObjectFunctionCall * c);
	bool writeHexBlock(KviKvsObjectFunctionCall * c);

	bool write(KviKvsObjectFunctionCall * c);
	bool read(KviKvsObjectFunctionCall * c);
};

#endif // !_CLASS_FILE_H_
