//=============================================================================
//
//   File : kvi_tal_scrollview.cpp
//   Creation date : Mon May 05 2008 11:25:08 by Alessandro Carbone
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2008 Alessandro Carbone (elfonol at gmail dot com)
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

#include "kvi_tal_scrollview.h"

int KviTalScrollView::childX(QWidget * child)
{
	return child->x();
}

int KviTalScrollView::childY(QWidget * child)
{
	return child->y();
}

void KviTalScrollView::moveChild(QWidget * child, int x, int y)
{
	child->move(x,y);
}

void KviTalScrollView::resizeContents(int w, int h)
{
	viewport()->resize(w,h);
}

void KviTalScrollView::addChild(QWidget * child, int x, int y)
{
	child->setParent(this);
	child->move(x,y);
}

int KviTalScrollView::visibleWidth()
{
	return width();
}

int KviTalScrollView::visibleHeight()
{
	return height();
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
	#include "kvi_tal_scrollview.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
