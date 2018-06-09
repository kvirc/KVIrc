//=============================================================================
//
//   File : KviSignalHandler.cpp
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

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "kvi_settings.h"

#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)

#include "KviSignalHandler.h"

#include <csignal>
#include <sys/signal.h>
#include <sys/socket.h>
#include <unistd.h>

#include <QApplication>
#include <QSocketNotifier>

// Adapted from http://doc.qt.io/qt-5/unix-signals.html

// In the KviSignalHandler constructor, use the socketpair(2) function
// to initialize the file descriptor pair, and then create the
// QSocketNotifier to monitor the read end of the pair. The
// activated() signal of each QSocketNotifier is connected to the
// appropriate slot function, which effectively converts the Unix
// signal to the QSocketNotifier::activated() signal.

KviSignalHandler::KviSignalHandler(QObject *parent)
		: QObject(parent)
{
	if (::socketpair(AF_UNIX, SOCK_STREAM, 0, fd))
		qFatal("Couldn't create socketpair");

	sn = new QSocketNotifier(fd[1], QSocketNotifier::Read, this);
	connect(sn, SIGNAL(activated(int)), this, SLOT(handleSignal()));
}

// Somewhere else in your startup code, you install your Unix signal
// handlers with sigaction(2).

bool kvi_signalHandlerSetup()
{
	new KviSignalHandler();

	struct sigaction sa;
	::memset(&sa,0,sizeof(sa));

	sa.sa_handler = KviSignalHandler::unixSignalHandler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags |= SA_RESTART;

	return sigaction(SIGTERM, &sa, nullptr) == 0 && sigaction(SIGINT, &sa, nullptr) == 0;
}

// In your Unix signal handlers, you write a byte to the write end of
// a socket pair and return. This will cause the corresponding
// QSocketNotifier to emit its activated() signal, which will in turn
// cause the appropriate Qt slot function to run.

void KviSignalHandler::unixSignalHandler(int)
{
	char a = 1;
	(void)::write(fd[0], &a, sizeof(a));
}

// In the slot functions connected to the QSocketNotifier::activated()
// signals, you read the byte. Now you are safely back in Qt with your
// signal, and you can do all the Qt stuff you weren'tr allowed to do
// in the Unix signal handler.

void KviSignalHandler::handleSignal()
{
	sn->setEnabled(false);
	char tmp;
	(void)::read(fd[1], &tmp, sizeof(tmp));

	// do Qt stuff
	QCoreApplication::quit();

	sn->setEnabled(true);
}

int KviSignalHandler::fd[2];

#endif // !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
