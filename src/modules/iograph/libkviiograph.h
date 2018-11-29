#ifndef _LIBKVIIOGRAPH_H_
#define _LIBKVIIOGRAPH_H_
//=============================================================================
//
//   File : libkviiograph.h
//   Creation date : Tue Oct 31 2000 00:14:12 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_inttypes.h"
#include "KviMainWindow.h"
#include "KviModuleExtension.h"
#include "KviWindow.h"

#include <QQueue>

#define KVI_IOGRAPH_NUMBER_POINTS 60
#define KVI_IOGRAPH_HORIZ_SEGMENTS 10
#define KVI_IOGRAPH_VERT_SEGMENTS 10
#define IOGRAPH_MODULE_EXTENSION_NAME "IO graph extension"

class KviIOGraphWidget : public QWidget
{
	Q_OBJECT
public:
	KviIOGraphWidget(QWidget * parent);
	~KviIOGraphWidget(){};

protected:
	QQueue<unsigned int> m_sendRates;
	QQueue<unsigned int> m_recvRates;
	unsigned int m_maxRate;
	kvi_u64_t m_uLastSentBytes;
	kvi_u64_t m_uLastRecvBytes;

protected:
	void timerEvent(QTimerEvent * e) override;
	void paintEvent(QPaintEvent * e) override;
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
	QPixmap * myIconPtr() override;
	void fillCaptionBuffers() override;
	void resizeEvent(QResizeEvent * e) override;
	virtual void die();
	void moveEvent(QMoveEvent *) override;
	void paintEvent(QPaintEvent * e) override;
};

#endif
