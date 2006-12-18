#ifndef _LIBKVIIOGRAPH_H_
#define _LIBKVIIOGRAPH_H_

//
//   File : kvi_iograph.cpp
//   Creation date : Tue Oct 31 2000 00:14:12 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2000 Szymon Stefanek (pragma at kvirc dot net)
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


#define KVI_IOGRAPH_NUMBER_POINTS 148

class KviIOGraphDisplay : public KviIrcContextGraphicalApplet
{
	Q_OBJECT
public:
	KviIOGraphDisplay(KviIrcContextController * pController,bool sentGraph,bool recvGraph);
	~KviIOGraphDisplay();
protected:
	unsigned int m_sendRates[KVI_IOGRAPH_NUMBER_POINTS];
	unsigned int m_recvRates[KVI_IOGRAPH_NUMBER_POINTS];
	int m_iNextPoint;
	unsigned int m_uLastSentBytes;
	unsigned int m_uLastRecvBytes;
	bool m_bShowSentGraph;
	bool m_bShowRecvGraph;
protected:
	virtual void drawContents(QPainter *p);
	virtual QSize sizeHint() const;
	virtual void timerEvent(QTimerEvent *e);
};

#endif

#endif
