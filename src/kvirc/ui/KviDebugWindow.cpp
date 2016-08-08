//=============================================================================
//
//   File : KviDebugWindow.cpp
//   Creation date : Sun Jul 18 2005 14:12:22 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviDebugWindow.h"
#include "KviConsoleWindow.h"
#include "KviIconManager.h"
#include "KviIrcView.h"
#include "KviInput.h"
#include "KviOptions.h"
#include "KviLocale.h"
#include "KviConfigurationFile.h"

#include "KviMainWindow.h"

#include <QList>
#include <QPixmap>

KviDebugWindow * KviDebugWindow::m_pInstance = nullptr;

KviDebugWindow::KviDebugWindow()
    : KviWindow(KviWindow::Debug, __tr2qs("Debug Messages"), nullptr)
{
	m_pInstance = this;
	m_pSplitter = new KviTalSplitter(Qt::Horizontal, this);
	m_pSplitter->setObjectName("main_splitter");
	m_pIrcView = new KviIrcView(m_pSplitter, this);
	m_pInput = new KviInput(this, nullptr);
	updateCaption();
}

KviDebugWindow::~KviDebugWindow()
{
	m_pInstance = nullptr;
}

KviDebugWindow * KviDebugWindow::getInstance()
{
	if(m_pInstance)
		return m_pInstance;
	m_pInstance = new KviDebugWindow();
	g_pMainWindow->addWindow(m_pInstance, !KVI_OPTION_BOOL(KviOption_boolShowMinimizedDebugWindow));
	//if(KVI_OPTION_BOOL(KviOption_boolShowMinimizedDebugWindow))
	//	m_pInstance->minimize();
	return m_pInstance;
}

void KviDebugWindow::getBaseLogFileName(QString & buffer)
{
	buffer = "debug";
}

void KviDebugWindow::saveProperties(KviConfigurationFile * cfg)
{
	KviWindow::saveProperties(cfg);
	cfg->writeEntry("Splitter", m_pSplitter->sizes());
}

void KviDebugWindow::loadProperties(KviConfigurationFile * cfg)
{
	int w = width();
	KviWindow::loadProperties(cfg);
	QList<int> def;
	def.append((w * 80) / 100);
	def.append((w * 20) / 100);
	m_pSplitter->setSizes(cfg->readIntListEntry("Splitter", def));
}

void KviDebugWindow::fillCaptionBuffers()
{
	m_szPlainTextCaption = windowName();
}

QPixmap * KviDebugWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KviIconManager::Bug);
}

void KviDebugWindow::resizeEvent(QResizeEvent *)
{
	int hght = m_pInput->heightHint();
	m_pSplitter->setGeometry(0, 0, width(), height() - hght);
	m_pInput->setGeometry(0, height() - hght, width(), hght);
}

QSize KviDebugWindow::sizeHint() const
{
	QSize ret(m_pSplitter->sizeHint().width(), m_pIrcView->sizeHint().height() + m_pInput->heightHint());
	return ret;
}
