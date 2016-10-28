//=============================================================================
//
//   File : KviPixmapUtils.cpp
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

#include "KviPixmapUtils.h"

namespace KviPixmapUtils
{
	void drawPixmapWithPainter(QPainter * p, QPixmap * pix, int flags, const QRect & paintRect, int iWidgetWidth, int iWidgetHeight, int dx, int dy)
	{
		if(!pix)
			return;
		if(!flags)
		{
			p->drawTiledPixmap(paintRect.left(), paintRect.top(), paintRect.width(), paintRect.height(), *pix, dx, dy);
			return;
		}

		int iPixWidth = pix->width();
		int iPixHeight = pix->height();
		int x = 0;
		int y = 0;

		if(!(flags & Qt::AlignHorizontal_Mask))
			x = -1;
		else if(flags & Qt::AlignRight)
			x = iWidgetWidth - iPixWidth;
		else if(flags & Qt::AlignHCenter)
			x = (iWidgetWidth - iPixWidth) / 2;

		if(!(flags & Qt::AlignVertical_Mask))
			y = -1;
		else if(flags & Qt::AlignBottom)
			y = iWidgetHeight - iPixHeight;
		else if(flags & Qt::AlignVCenter)
			y = (iWidgetHeight - iPixHeight) / 2;

		if(x == -1)
		{
			p->drawTiledPixmap(paintRect.left(), y, paintRect.width(), iPixHeight, *pix, dx, dy);
		}
		else if(y == -1)
		{
			p->drawTiledPixmap(x, paintRect.top(), iPixWidth, paintRect.height(), *pix, dx, dy);
		}
		else
		{
			p->drawPixmap(x, y, *pix);
		}
	}
}
