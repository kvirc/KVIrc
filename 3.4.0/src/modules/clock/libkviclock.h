#ifndef _LIBKVICLOCK_H_
#define _LIBKVICLOCK_H_

//
//   File : kvi_clock.cpp
//   Creation date : Mon Aug 13 2001 16:33:12 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
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

#if 0

#include "kvi_irctoolbar.h"

class KviClockDisplay : public KviIrcToolBarGraphicalApplet
{
	Q_OBJECT
public:
	KviClockDisplay(KviIrcToolBarAppletContainer * pContainer);
	~KviClockDisplay();
protected:
	unsigned char m_uDigit[6];
protected:
	virtual void drawContents(QPainter * p);
	virtual QSize sizeHint() const;
	virtual void timerEvent(QTimerEvent *e);
};

#endif

#endif
