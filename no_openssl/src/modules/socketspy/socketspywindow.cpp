//=============================================================================
//
//   File : socketspywindow.cpp
//   Creation date : Thu Aug 31 2000 15:02:22 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the socketspys of the GNU General Public License
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

#include "socketspywindow.h"

#include "kvi_iconmanager.h"
#include "kvi_ircview.h"
#include "kvi_out.h"
#include "kvi_options.h"
#include "kvi_locale.h"
#include "kvi_console.h"
//#include "kvi_module.h"

extern KviPointerList<KviSocketSpyWindow> * g_pSocketSpyWindowList;

KviSocketSpyWindow::KviSocketSpyWindow(KviFrame * lpFrm,KviConsole * lpConsole)
: KviWindow(KVI_WINDOW_TYPE_SOCKETSPY,lpFrm,"socket_spy",lpConsole) , KviIrcDataStreamMonitor(lpConsole->context())
{
	g_pSocketSpyWindowList->append(this);
	m_pSplitter = new KviTalSplitter(Qt::Horizontal,this);
	setObjectName("spysocket_splitter");
	m_pIrcView = new KviIrcView(m_pSplitter,lpFrm,this);
	// Ensure proper focusing
	//setFocusHandler(m_pIrcView,this);
}

KviSocketSpyWindow::~KviSocketSpyWindow()
{
	g_pSocketSpyWindowList->removeRef(this);
}

void KviSocketSpyWindow::die()
{
	close();
}

QPixmap * KviSocketSpyWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KVI_SMALLICON_SPY);
}

void KviSocketSpyWindow::resizeEvent(QResizeEvent *)
{
	m_pSplitter->setGeometry(0,0,width(),height());
}

QSize KviSocketSpyWindow::sizeHint() const
{
	return m_pIrcView->sizeHint();
}

void KviSocketSpyWindow::getBaseLogFileName(QString &buffer)
{
	buffer.sprintf("SOCKETSPY_%d",context()->id());
}

void KviSocketSpyWindow::fillCaptionBuffers()
{
	KviQString::sprintf(m_szPlainTextCaption,__tr2qs("Socket Spy [IRC Context %u]"),m_pConsole->context()->id());
}

bool KviSocketSpyWindow::incomingMessage(const char * message)
{
	outputNoFmt(KVI_OUT_SOCKETMESSAGE,console()->decodeText(message));
	return false;
}

bool KviSocketSpyWindow::outgoingMessage(const char * message)
{
	outputNoFmt(KVI_OUT_RAW,console()->decodeText(message));
	return false;

}

void KviSocketSpyWindow::connectionInitiated()
{
	output(KVI_OUT_SOCKETWARNING,__tr2qs("Socket open"));
}

void KviSocketSpyWindow::connectionTerminated()
{
	output(KVI_OUT_SOCKETWARNING,__tr2qs("Socket closed"));
}

void KviSocketSpyWindow::applyOptions()
{
	m_pIrcView->applyOptions();
	KviWindow::applyOptions();
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_socketspywindow.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
