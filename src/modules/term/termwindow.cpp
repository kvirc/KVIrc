//=============================================================================
//
//   File : termwindow.cpp
//   Creation date : Thu Aug 31 2000 15:02:22 by Szymon Stefanek
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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include "termwindow.h"
#include "termwidget.h"

#ifdef COMPILE_KDE_SUPPORT
	#include "kvi_iconmanager.h"
	#include "kvi_options.h"
	#include "kvi_locale.h"
	#include "kvi_module.h"

	extern KviModule * g_pTermModule;
	extern KviPointerList<KviTermWindow> * g_pTermWindowList;
	extern KviPointerList<KviTermWidget> * g_pTermWidgetList;
	extern KviIconManager * g_pIconManager;

	KviTermWindow::KviTermWindow(KviFrame * lpFrm,const char * name)
	: KviWindow(KVI_WINDOW_TYPE_TERM,lpFrm,name)
	{
		g_pTermWindowList->append(this);
		m_pTermWidget = 0;
		m_pTermWidget = new KviTermWidget(this,lpFrm);
		// Ensure proper focusing
	//	setFocusHandler(m_pTermWidget->konsoleWidget(),this);
	}

	KviTermWindow::~KviTermWindow()
	{
		g_pTermWindowList->removeRef(this);
		if(g_pTermWindowList->isEmpty() && g_pTermWidgetList->isEmpty())g_pTermModule->unlock();
	}

	QPixmap * KviTermWindow::myIconPtr()
	{
		return g_pIconManager->getSmallIcon(KVI_SMALLICON_RAW);
	}

	void KviTermWindow::resizeEvent(QResizeEvent *)
	{
		if(m_pTermWidget)m_pTermWidget->setGeometry(0,0,width(),height());
	}

	QSize KviTermWindow::sizeHint() const
	{
		return m_pTermWidget ? m_pTermWidget->sizeHint() : KviWindow::sizeHint();
	}

	void KviTermWindow::fillCaptionBuffers()
	{
		m_szPlainTextCaption = __tr("Terminal");
	}


#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "termwindow.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES

#endif
