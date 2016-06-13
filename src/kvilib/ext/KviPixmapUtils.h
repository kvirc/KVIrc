#ifndef _KVIPIXMAPUTILS_H_
#define _KVIPIXMAPUTILS_H_
//=============================================================================
//
//   File : KviPixmapUtils.h
//   Creation date : Wed Dec 29 2010 01:10:05 CEST by Elvio Basello
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2010 Elvio Basello (hellvis69 at gmail dot com)
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

// this file was originally part of KviPixmap.h

#include "kvi_settings.h"

#include <QPainter>

namespace KviPixmapUtils
{
	extern KVILIB_API void drawPixmapWithPainter(QPainter * p, QPixmap * pix, int flags, const QRect & paintRect, int iWidgetWidth, int iWidgetHeight, int dx, int dy);
	inline void drawPixmapWithPainter(QPainter * p, QPixmap * pix, int flags, const QRect & paintRect, int iWidgetWidth, int iWidgetHeight) { KviPixmapUtils::drawPixmapWithPainter(p, pix, flags, paintRect, iWidgetWidth, iWidgetHeight, paintRect.left(), paintRect.top()); }
}

#endif // _KVIPIXMAPUTILS_H_
