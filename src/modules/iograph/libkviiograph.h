#ifndef _LIBKVIIOGRAPH_H_
#define _LIBKVIIOGRAPH_H_
//=============================================================================
//
//   File : kvi_iograph.cpp
//   Creation date : Tue Oct 31 2000 00:14:12 CEST by Szymon Stefanek
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#include <QSplitter>

#include "kvi_frame.h"
#include "kvi_inttypes.h"
#include "kvi_moduleextension.h"
#include "kvi_window.h"

#define KVI_IOGRAPH_NUMBER_POINTS 148
#define KVI_IOGRAPH_HORIZ_SEGMENTS 6
#define KVI_IOGRAPH_VERT_SEGMENTS 29
#define IOGRAPH_MODULE_EXTENSION_NAME "IO graph extension"

class KviIOGraphWidget : public QWidget
{
	Q_OBJECT
public:
	KviIOGraphWidget(QWidget *parent);
	~KviIOGraphWidget() {};
protected:
	unsigned int m_sendRates[KVI_IOGRAPH_NUMBER_POINTS];
	unsigned int m_recvRates[KVI_IOGRAPH_NUMBER_POINTS];
	int m_iNextPoint;
	kvi_u64_t m_uLastSentBytes;
	kvi_u64_t m_uLastRecvBytes;
protected:
	virtual void timerEvent(QTimerEvent *e);
	virtual void paintEvent (QPaintEvent *e);
};

class KviIOGraphWindow : public KviWindow , public KviModuleExtension
{
	Q_OBJECT
public:
	KviIOGraphWindow(KviModuleExtensionDescriptor * d,KviFrame * lpFrm,const char * name);
	~KviIOGraphWindow();
private:
	KviIOGraphWidget * m_pIOGraph;
protected:
	virtual QPixmap * myIconPtr();
	virtual void fillCaptionBuffers();
	virtual void resizeEvent(QResizeEvent *e);
	virtual void die();
};

#endif

