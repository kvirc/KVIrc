#ifndef _KVI_SIGNALHANDLER_H_
#define _KVI_SIGNALHANDLER_H_
//=============================================================================
//
//   File : KviSignalHandler.h
//   Creation date : Sat 2016-07-09 07:32:25 UTC by Vladimir Panteleev
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 1999-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"

#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)

#include <QObject>

class QSocketNotifier;

// Adapted from http://doc.qt.io/qt-5/unix-signals.html

class KviSignalHandler : public QObject
{
	Q_OBJECT

public:
	KviSignalHandler(QObject *parent = nullptr);

	static void unixSignalHandler(int unused);

public slots:
	// Qt signal handler
	void handleSignal();

private:
	static int fd[2];

	QSocketNotifier *sn;
};

bool KVILIB_API kvi_signalHandlerSetup();

#else // !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
static bool kvi_signalHandlerSetup() { return true; }
#endif // !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)

#endif //!_KVI_SIGNALHANDLER_H_
