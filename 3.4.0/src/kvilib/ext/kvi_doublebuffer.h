#ifndef _KVI_DOUBLEBUFFER_H_
#define _KVI_DOUBLEBUFFER_H_
//=============================================================================
//
//   File : kvi_doublebuffer.h
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

#include "kvi_settings.h"

#include <qpixmap.h>

//
// This class is basically a huge shared memory pixmap meant to be used in double-buffer
// painting operations. The memory buffer is resized on the fly so you will always obtain
// a pixmap that is at least of the specified size. The problem is that this is a time
// consuming operation (not good in a paint event). We solve it by keeping the buffer
// with the greatest requested size in the last N minutes.
//
// The keyword in all this thingie is "memory is cheap, processing time is not".
// We know in advance that KVIrc needs a huge double buffer anyway...
// So we basically grow instantly but we are really lazy at shrinking.
//

class KVILIB_API KviDoubleBuffer
{
public:
	KviDoubleBuffer(unsigned int uWidth,unsigned int uHeight);
	~KviDoubleBuffer();
public:
	// This returns a pointer to the memory buffer. The buffer is at least
	// of the size declared in the constructor.
	QPixmap * pixmap();

	// The stuff below is internal (used only by KviApp)

	// to be called at application initialisation and cleanup
	static void init();
	static void done();
	// this has to be called at sensible intervals (like 2 minutes)
	static void heartbeat();
};

#endif //!_KVI_DOUBLEBUFFER_H_
