#ifndef _KVI_STRINGCONVERSION_H_
#define _KVI_STRINGCONVERSION_H_

//=============================================================================
//
//   File : kvi_stringconversion.h
//   Creation date : Thu Oct 20 2000 13:27:12 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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
#include "kvi_string.h"
#include "kvi_pixmap.h"
#include "kvi_msgtype.h"

#include <QRect>
#include <QColor>
#include <QFont>
#include <QStringList>
#include <QString>


namespace KviStringConversion
{
	extern KVILIB_API void init(const QString& szGlobalDir,const QString& szLocalDir);

	extern KVILIB_API void encodePath(QString& buffer);
	extern KVILIB_API void decodePath(QString& buffer);

	extern KVILIB_API void encodePath(QStringList& buffer);
	extern KVILIB_API void decodePath(QStringList& buffer);

	// bool <->
	extern KVILIB_API void toString(const bool bValue,QString &buffer);
	extern KVILIB_API bool fromString(const QString &szValue,bool &buffer);

	// int <-> QString
	extern KVILIB_API void toString(const int iValue,QString &buffer);
	extern KVILIB_API bool fromString(const QString &szValue,int &buffer);

	// uint <-> QString
	extern KVILIB_API void toString(const unsigned int uValue,QString &buffer);
	extern KVILIB_API bool fromString(const QString &szValue,unsigned int &buffer);

	// QRect <-> QString
	extern KVILIB_API void toString(const QRect &rValue,QString &buffer);
	extern KVILIB_API bool fromString(const QString &szValue,QRect &buffer);

	// QString <-> QString (Null conversion)
	extern KVILIB_API void toString(const QString &szValue,QString &buffer);
	extern KVILIB_API bool fromString(const QString &szValue,QString &buffer);

	// KviPixmap <-> QString
	extern KVILIB_API void toString(const KviPixmap &pValue,QString &buffer);
	extern KVILIB_API bool fromString(const QString &szValue,KviPixmap &buffer);

	// QFont <-> QString
	extern KVILIB_API void toString(const QFont &fValue,QString &buffer);
	extern KVILIB_API bool fromString(const QString &szValue,QFont &buffer);

	// KviMsgType <-> QString
	extern KVILIB_API void toString(const KviMsgType &mValue,QString &buffer);
	extern KVILIB_API bool fromString(const QString &szValue,KviMsgType &buffer);

	// QColor <-> QString
	extern KVILIB_API void toString(const QColor &cValue,QString &buffer);
	extern KVILIB_API bool fromString(const QString &szValue,QColor &buffer);

	// QStringList <-> QString
	extern KVILIB_API void toString(const QStringList &sValue,QString &buffer);
	extern KVILIB_API bool fromString(const QString &szValue,QStringList &buffer);
}

#endif //!_KVI_STRINGCONVERSION_H_
