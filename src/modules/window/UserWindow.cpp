//=============================================================================
//
//   File : UserWindow.cpp
//   Created on Mon 30 Aug 2004 15:00:52 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "UserWindow.h"
#include "KviInput.h"
#include "KviIrcView.h"
#include "KviMainWindow.h"
#include "KviConsoleWindow.h"

#include <algorithm>
#include <map>

extern std::vector<UserWindow *> g_pUserWindowList;

UserWindow::UserWindow(const char * pcName, QString & szIcon, KviConsoleWindow * pConsole, int iCreationFlags)
    : KviWindow(KviWindow::UserWindow, pcName, pConsole)
{
	g_pUserWindowList.push_back(this);

	m_szIcon = szIcon;

	m_szPlainTextCaption = pcName;
	fillSingleColorCaptionBuffers(m_szPlainTextCaption);

	m_pIrcView = new KviIrcView(this, this);

	if(iCreationFlags & HasInput)
		m_pInput = new KviInput(this, nullptr);
	else
		m_pInput = nullptr;

	if(context())
		context()->registerContextWindow(this);
}

UserWindow::~UserWindow()
{
	if(context())
		context()->unregisterContextWindow(this);
	g_pUserWindowList.erase(std::remove(g_pUserWindowList.begin(), g_pUserWindowList.end(), this), g_pUserWindowList.end());
}

QPixmap * UserWindow::myIconPtr()
{
	QPixmap * pPix = g_pIconManager->getImage(m_szIcon);
	if(!pPix)
		return g_pIconManager->getSmallIcon(KviIconManager::None);
	else
		return pPix;
}

void UserWindow::resizeEvent(QResizeEvent *)
{
	if(m_pInput)
	{
		int hght = m_pInput->heightHint();
		m_pIrcView->setGeometry(0, 0, width(), height() - hght);
		m_pInput->setGeometry(0, height() - hght, width(), hght);
	}
	else
	{
		m_pIrcView->setGeometry(0, 0, width(), height());
	}
}

void UserWindow::fillCaptionBuffers()
{
	// nothing here
}

void UserWindow::setWindowTitleStrings(const QString & szPlainText)
{
	m_szPlainTextCaption = szPlainText;
	updateCaption();
}
