#ifndef _KVI_FILE_H_
#define _KVI_FILE_H_

//=============================================================================
//
//   File : kvi_file.h
//   Creation date : Mon Dec 17 2001 00:05:04 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"
#include "kvi_heapobject.h"
#include "kvi_qstring.h"
#include "kvi_string.h"
#include "kvi_pointerlist.h"
#include "kvi_inttypes.h"
#include "kvi_qcstring.h"

#include <QFile>
#include <time.h>

#define kvi_file_offset_t qlonglong


class KVILIB_API KviFile : public QFile, public KviHeapObject
{
public:
	KviFile();
	KviFile(const QString &name);
	~KviFile();
public:
	// Wrappers portable across Qt 3.x and Qt 4.x
	bool openForReading();
	bool openForWriting(bool bAppend = false);


	// Missing functions in Qt 4.x
	qint64 writeBlock(const char * data,quint64 uLen){ return write(data,uLen); };
	qint64 readBlock(char * data,quint64 uLen){ return read(data,uLen); };

	// This stuff loads and saves LITTLE ENDIAN DATA!
	bool save(kvi_u64_t t);
	bool load(kvi_u64_t &t);

	bool save(kvi_i64_t t){ return save((kvi_u64_t)t); };
	bool load(kvi_i64_t &t){ return load((kvi_u64_t &)t); };

	bool save(kvi_u32_t t);
	bool load(kvi_u32_t &t);

	bool save(kvi_i32_t t){ return save((kvi_u32_t)t); };
	bool load(kvi_i32_t &t){ return load((kvi_u32_t &)t); };

	bool save(kvi_u16_t t);
	bool load(kvi_u16_t &t);

	bool save(kvi_i16_t t){ return save((kvi_u16_t)t); };
	bool load(kvi_i16_t &t){ return load((kvi_u16_t &)t); };

	bool save(kvi_u8_t t);
	bool load(kvi_u8_t &t);

	bool save(kvi_i8_t t){ return save((kvi_u8_t)t); };
	bool load(kvi_i8_t &t){ return load((kvi_u8_t &)t); };;

	bool save(const KviStr &szData);
	bool load(KviStr &szData);


	bool save(const QByteArray &bData);
	bool load(QByteArray &bData);

	bool save(const QString &szData);
	bool load(QString &szData);

	bool skipFirst(char t,unsigned int maxdist = 0xffffffff);
	bool skipFirst(const KviStr &t,unsigned int maxdist = 0xffffffff);

	bool save(KviPointerList<KviStr> * pData);
	bool load(KviPointerList<KviStr> * pData);
};


#endif //_KVI_FILE_H_
