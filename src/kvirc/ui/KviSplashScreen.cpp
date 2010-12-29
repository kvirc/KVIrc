//=============================================================================
//
//   File : KviSplashScreen.cpp
//   Creation date : Wed Aug 8 2001 17:46:10 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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


#include "KviCString.h"
#include "KviApplication.h"
#include "kvi_defaults.h"
#include "KviSplashScreen.h"
#include "KviLocale.h"
#include "KviFileUtils.h"

#include <QSplashScreen>
#include <QDesktopWidget>
#include <QPixmap>
#include <QPainter>
#include <QLayout>

#include <stdio.h>


KviSplashScreen::KviSplashScreen()
: QSplashScreen(QPixmap(1,1),Qt::FramelessWindowHint | Qt::Window | Qt::WindowStaysOnTopHint | Qt::SplashScreen)
{
	QString szPix;
	QPixmap * pix = 0;

	// check for the current theme splash screen pointer
	QString szPointerFile;
	g_pApp->getLocalKvircDirectory(szPointerFile,KviApplication::Themes,"current-splash");
	if(KviFileUtils::fileExists(szPointerFile))
	{
		QString szBuf;
		KviFileUtils::readFile(szPointerFile,szBuf);
		if(!szBuf.isEmpty())
		{
			g_pApp->getLocalKvircDirectory(szPix,KviApplication::Themes,szBuf);
			KviQString::ensureLastCharIs(szPix,KVI_PATH_SEPARATOR_CHAR);
			szPix.append("kvi_splash.png");
			pix = new QPixmap(szPix);
			if(pix->isNull())
			{
				// no way..
				delete pix;
				pix = 0;
			} else {
				// else we might have a themed splash screen
// 				g_pApp->getLocalKvircDirectory(szPix,KviApplication::Themes,szBuf);
				KviQString::ensureLastCharIs(szPix,KVI_PATH_SEPARATOR_CHAR);
				szPix.append("kvi_splash_overlay.png");
				m_pOverlay = new QPixmap(szPix);
				if(m_pOverlay->isNull())
				{
					// no way..
					delete pix;
					pix = 0;
					delete m_pOverlay;
					m_pOverlay = 0;
				}
			}
		}
	}

	if(!pix)
	{
		if(g_pApp->findImage(szPix,"kvi_splash.png"))
		{
			pix = new QPixmap(szPix);
		} else {
			// dummy image
			pix = new QPixmap(300,200);
		}

		if(g_pApp->findImage(szPix,"kvi_splash_overlay.png"))
		{
			m_pOverlay = new QPixmap(szPix);
		} else {
			m_pOverlay = new QPixmap(300,20);
		}
	}

	setPixmap(*pix);
	m_pTimer = new QTimer(this);
	connect(m_pTimer,SIGNAL(timeout()),this,SLOT(suicide()));
	delete pix;

	setWindowOpacity(0);
	m_bIncreasing=true;
	m_rTransparency=0;
	m_pFadeTimer= new QTimer(this);
	connect(m_pFadeTimer,SIGNAL(timeout()),this,SLOT(fadeTimerShot()));
	m_pFadeTimer->start(6);
}

// We don't need messages on the splash: they just add work to the translators and nobody can read them anyway :D
//void KviSplashScreen::message(QString szMessage)
//{
	//QSplashScreen::message(szMessage, Qt::AlignRight | Qt::AlignBottom, Qt::white);
//}

KviSplashScreen::~KviSplashScreen()
{
	g_pSplashScreen = 0; // make sure it's true
	delete m_pTimer;
	delete m_pOverlay;
}

void KviSplashScreen::showEvent(QShowEvent *)
{
	QRect rect = g_pApp->desktop()->screenGeometry(g_pApp->desktop()->primaryScreen());
	move((rect.width() - width())/2,(rect.height() - height())/2);

	m_creationTime = QTime::currentTime();
}

void KviSplashScreen::hideEvent(QHideEvent *)
{
	suicide();
}

void KviSplashScreen::setProgress(int iProgress)
{
	QPixmap slowQt4Copy = pixmap();
	QPainter painter(&slowQt4Copy);
	QSize size = slowQt4Copy.size();
	int iWidth = (m_pOverlay->width() * iProgress) / 100;
	painter.drawPixmap(0,size.height() - m_pOverlay->height(),iWidth,m_pOverlay->height(),*m_pOverlay,0,0,iWidth,m_pOverlay->height());
	painter.end();
	setPixmap(slowQt4Copy);

	//raise();
	repaint();
	g_pApp->processEvents(); //damn...
}

void KviSplashScreen::die()
{
	m_bIncreasing = false;
	m_pFadeTimer->start(6);
}

void KviSplashScreen::suicide()
{
	if(!g_pSplashScreen)
		return; // already in suicide ?
	//g_pApp->collectGarbage(this);
	g_pSplashScreen = 0;
	deleteLater();
	//delete this;
}

void KviSplashScreen::fadeTimerShot()
{
	if(m_bIncreasing)
	{
		m_rTransparency += 0.05;
		setWindowOpacity(m_rTransparency);
		if(m_rTransparency>=1)
		{
			m_pFadeTimer->stop();
			m_bIncreasing = false;
		}
	} else {
		m_rTransparency -= 0.02;
		setWindowOpacity(m_rTransparency);
		if(m_rTransparency<=0)
		{
			m_pFadeTimer->stop();
			m_bIncreasing = true;
			suicide();
		}
	}
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "KviSplashScreen.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
