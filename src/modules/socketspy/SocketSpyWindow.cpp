//=============================================================================
//
//   File : SocketSpyWindow.cpp
//   Creation date : Thu Aug 31 2000 15:02:22 by Szymon Stefanek
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

#include "SocketSpyWindow.h"

#include "KviIconManager.h"
#include "KviIrcView.h"
#include "kvi_out.h"
#include "KviOptions.h"
#include "KviLocale.h"
#include "KviConsoleWindow.h"

#include <QString>
#include <unordered_set>

extern std::unordered_set<SocketSpyWindow *> g_pSocketSpyWindowList;

SocketSpyWindow::SocketSpyWindow(KviConsoleWindow * lpConsole)
    : KviWindow(KviWindow::SocketSpy, "socket_spy", lpConsole), KviIrcDataStreamMonitor(lpConsole->context())
{
	g_pSocketSpyWindowList.insert(this);
	m_pSplitter = new KviTalSplitter(Qt::Horizontal, this);
	setObjectName("spysocket_splitter");
	m_pIrcView = new KviIrcView(m_pSplitter, this);
	// Ensure proper focusing
	//setFocusHandler(m_pIrcView,this);
}

SocketSpyWindow::~SocketSpyWindow()
{
	g_pSocketSpyWindowList.erase(this);
}

void SocketSpyWindow::die()
{
	close();
}

QPixmap * SocketSpyWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KviIconManager::Spy);
}

void SocketSpyWindow::resizeEvent(QResizeEvent *)
{
	m_pSplitter->setGeometry(0, 0, width(), height());
}

QSize SocketSpyWindow::sizeHint() const
{
	return m_pIrcView->sizeHint();
}

void SocketSpyWindow::getBaseLogFileName(QString & buffer)
{
	buffer.sprintf("SOCKETSPY_%d", context()->id());
}

void SocketSpyWindow::fillCaptionBuffers()
{
	m_szPlainTextCaption = QString(__tr2qs("Socket Spy [IRC Context %1]")).arg(m_pConsole->context()->id());
}

bool SocketSpyWindow::incomingMessage(const char * message)
{
	outputNoFmt(KVI_OUT_SOCKETMESSAGE, console()->decodeText(message));
	return false;
}

bool SocketSpyWindow::outgoingMessage(const char * message)
{
	outputNoFmt(KVI_OUT_RAW, console()->decodeText(message));
	return false;
}

void SocketSpyWindow::connectionInitiated()
{
	output(KVI_OUT_SOCKETWARNING, __tr2qs("Socket open"));
}

void SocketSpyWindow::connectionTerminated()
{
	output(KVI_OUT_SOCKETWARNING, __tr2qs("Socket closed"));
}

void SocketSpyWindow::applyOptions()
{
	m_pIrcView->applyOptions();
	KviWindow::applyOptions();
}
