//=============================================================================
//
//   File : kvi_tal_tooltip.cpp
//   Creation date : Mon Jan 22 2007 11:25:08 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2007 Szymon Stefanek (pragma at kvirc dot net)
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
#include "kvi_tal_tooltip.h"

KviTalToolTip::KviTalToolTip(QWidget * pParent)
#ifndef COMPILE_USE_QT4
: QToolTip(pParent)
#endif
{
	// FIXME: With QT4 will need a helper that intercepts
	// the widget QHelpEvent thingies...
	// Will also need a deleting parent I guess...
}

KviTalToolTip::~KviTalToolTip()
{
}

#ifdef COMPILE_USE_QT4
void KviTalToolTip::add(QWidget * widget,const QString & text)
{
	QToolTip::add(widget,text);
}


void KviTalToolTip::tip(const QRect & rect,const QString & text)
{
	QToolTip::showText(rect.topLeft(),text);
}
#endif

void KviTalToolTip::maybeTip(const QPoint & p)
{
	// does nothing here.. and in Qt 4.x will even fail to work
}


