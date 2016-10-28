#ifndef _SOCKETSPYWINDOW_H_
#define _SOCKETSPYWINDOW_H_
//=============================================================================
//
//   File : SocketSpyWindow.h
//   Creation date : Sun Nov 26 2000 13:13:00 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the socketspys of the GNU General Public License
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

#include "KviWindow.h"
#include "KviIrcDataStreamMonitor.h"

class KviConsoleWindow;

class SocketSpyWindow final : public KviWindow, public KviIrcDataStreamMonitor
{
	Q_OBJECT
public:
	SocketSpyWindow(KviConsoleWindow * lpConsole);
	~SocketSpyWindow();

private:
	QPixmap * myIconPtr() override;
	void fillCaptionBuffers() override;
	void resizeEvent(QResizeEvent * e) override;
	void getBaseLogFileName(QString & buffer) override;
	void applyOptions() override;

public:
	QSize sizeHint() const override;
	bool incomingMessage(const char * message) override;
	//For proxy connections it might spit out binary data!
	bool outgoingMessage(const char * message) override;
	void connectionInitiated() override;
	void connectionTerminated() override;
	void die() override;
};

#endif //_KVI_SOCKETSPYWINDOW_H_
