//=============================================================================
//
//   File : TermWindow.cpp
//   Creation date : Thu Aug 31 2000 15:02:22 by Szymon Stefanek
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

#include "TermWindow.h"
#include "TermWidget.h"

#ifdef COMPILE_KDE4_SUPPORT
#include "KviIconManager.h"
#include "KviOptions.h"
#include "KviLocale.h"
#include "KviModule.h"

#include <unordered_set>

extern KviModule * g_pTermModule;
extern std::unordered_set<TermWindow *> g_pTermWindowList;
extern std::unordered_set<TermWidget *> g_pTermWidgetList;
extern KviIconManager * g_pIconManager;

TermWindow::TermWindow(const char * name)
    : KviWindow(KviWindow::Terminal, name)
{
	g_pTermWindowList.insert(this);
	m_pTermWidget = nullptr;
	m_pTermWidget = new TermWidget(this);
}

TermWindow::~TermWindow()
{
	g_pTermWindowList.erase(this);
	if(g_pTermWindowList.empty() && g_pTermWidgetList.empty())
		g_pTermModule->unlock();
}

QPixmap * TermWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KviIconManager::Raw);
}

void TermWindow::resizeEvent(QResizeEvent *)
{
	if(m_pTermWidget)
		m_pTermWidget->setGeometry(0, 0, width(), height());
}

QSize TermWindow::sizeHint() const
{
	return m_pTermWidget ? m_pTermWidget->sizeHint() : KviWindow::sizeHint();
}

void TermWindow::fillCaptionBuffers()
{
	m_szPlainTextCaption = __tr("Terminal");
}

#endif
