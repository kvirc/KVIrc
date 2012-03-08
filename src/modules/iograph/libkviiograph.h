#ifndef _LIBKVIIOGRAPH_H_
#define _LIBKVIIOGRAPH_H_
//=============================================================================
//
//   File : kvi_iograph.cpp
//   Creation date : Tue Oct 31 2000 00:14:12 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include <QQueue>

#include "KviMainWindow.h"
#include "kvi_inttypes.h"
#include "KviModuleExtension.h"
#include "KviWindow.h"

#define KVI_IOGRAPH_NUMBER_POINTS 60
#define KVI_IOGRAPH_HORIZ_SEGMENTS 10
#define KVI_IOGRAPH_VERT_SEGMENTS 10
#define IOGRAPH_MODULE_EXTENSION_NAME "IO graph extension"

class KviIOGraphWidget : public QWidget
{
	Q_OBJECT
public:
	KviIOGraphWidget(QWidget *parent);
	~KviIOGraphWidget() {};
protected:
	QQueue<unsigned int> m_sendRates;
	QQueue<unsigned int> m_recvRates;
	unsigned int m_maxRate;
	kvi_u64_t m_uLastSentBytes;
	kvi_u64_t m_uLastRecvBytes;
protected:
	virtual void timerEvent(QTimerEvent *e);
	virtual void paintEvent (QPaintEvent *e);
};

class KviIOGraphWindow : public KviWindow
{
	Q_OBJECT
public:
	KviIOGraphWindow(const char * name);
	~KviIOGraphWindow();
private:
	KviIOGraphWidget * m_pIOGraph;
	virtual void updatePseudoTransparency();
protected:
	virtual QPixmap * myIconPtr();
	virtual void fillCaptionBuffers();
	virtual void resizeEvent(QResizeEvent *e);
	virtual void die();
	virtual void moveEvent(QMoveEvent *);
	virtual void paintEvent (QPaintEvent *e);
};

#endif

