//=============================================================================
//
//   File : kvi_doublebuffer.cpp
//   Created on Fri 27 Jan 2006 18:59:54 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2006 Szymon Stefanek <pragma at kvirc dot net>
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#define __KVILIB__

#include "kvi_doublebuffer.h"

static QPixmap * g_pMemoryPixmap = 0;
static unsigned int g_uMaxRequestedWidth = 0;
static unsigned int g_uMaxRequestedHeight = 0;

KviDoubleBuffer::KviDoubleBuffer(unsigned int uWidth,unsigned int uHeight)
{
	if((g_pMemoryPixmap->width() < uWidth) || (g_pMemoryPixmap->height() < uHeight))
	{
		// The memory buffer is too small
		// There is either no such user requirement or it has grown by the meantime
		unsigned int uMaxW = uWidth > g_pMemoryPixmap->width() ? uWidth : g_pMemoryPixmap->width();
		unsigned int uMaxH = uHeight > g_pMemoryPixmap->height() ? uHeight : g_pMemoryPixmap->height();

		// QT4SUX: QPixmap::resize() is missing (it's a widely used function and assigning a new QPixmap() seems to be slower and not intuitive)
		*g_pMemoryPixmap = QPixmap(uMaxW,uMaxH);
	}

	if(uWidth > g_uMaxRequestedWidth)g_uMaxRequestedWidth = uWidth;
	if(uHeight > g_uMaxRequestedHeight)g_uMaxRequestedHeight = uHeight;
}

KviDoubleBuffer::~KviDoubleBuffer()
{
	// We never shrink here (it's time consuming)
}

QPixmap * KviDoubleBuffer::pixmap()
{
	return g_pMemoryPixmap;
}

void KviDoubleBuffer::init()
{
	if(g_pMemoryPixmap)return;
	g_pMemoryPixmap = new QPixmap();
}

void KviDoubleBuffer::done()
{
	if(!g_pMemoryPixmap)return;
	delete g_pMemoryPixmap;
	g_pMemoryPixmap = 0;
}

void KviDoubleBuffer::heartbeat()
{
	if(((g_uMaxRequestedHeight + 64) < g_pMemoryPixmap->height()) || ((g_uMaxRequestedWidth + 64) < g_pMemoryPixmap->width()))
	{
		// do shrink :)
		// QT4SUX: QPixmap::resize() is missing (it's a widely used function and assigning a new QPixmap() seems to be slower and not intuitive)
		*g_pMemoryPixmap = QPixmap(g_uMaxRequestedWidth,g_uMaxRequestedHeight);
	}
	g_uMaxRequestedHeight = 0;
	g_uMaxRequestedWidth = 0;
}
