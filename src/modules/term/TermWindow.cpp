//=============================================================================
//
//   File : TermWindow.cpp
//   Creation date : Thu Aug 31 2000 15:02:22 by Szymon Stefanek
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

#include "TermWindow.h"
#include "TermWidget.h"

#ifdef COMPILE_KDE_SUPPORT
	#include "KviIconManager.h"
	#include "KviOptions.h"
	#include "KviLocale.h"
	#include "KviModule.h"

	extern KviModule * g_pTermModule;
	extern KviPointerList<TermWindow> * g_pTermWindowList;
	extern KviPointerList<TermWidget> * g_pTermWidgetList;
	extern KviIconManager * g_pIconManager;

	TermWindow::TermWindow(const char * name)
	: KviWindow(KviWindow::Terminal,name)
	{
		g_pTermWindowList->append(this);
		m_pTermWidget = 0;
		m_pTermWidget = new TermWidget(this);
		// Ensure proper focusing
	//	setFocusHandler(m_pTermWidget->konsoleWidget(),this);
	}

	TermWindow::~TermWindow()
	{
		g_pTermWindowList->removeRef(this);
		if(g_pTermWindowList->isEmpty() && g_pTermWidgetList->isEmpty())g_pTermModule->unlock();
	}

	QPixmap * TermWindow::myIconPtr()
	{
		return g_pIconManager->getSmallIcon(KviIconManager::Raw);
	}

	void TermWindow::resizeEvent(QResizeEvent *)
	{
		if(m_pTermWidget)m_pTermWidget->setGeometry(0,0,width(),height());
	}

	QSize TermWindow::sizeHint() const
	{
		return m_pTermWidget ? m_pTermWidget->sizeHint() : KviWindow::sizeHint();
	}

	void TermWindow::fillCaptionBuffers()
	{
		m_szPlainTextCaption = __tr("Terminal");
	}


#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "TermWindow.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES

#endif
