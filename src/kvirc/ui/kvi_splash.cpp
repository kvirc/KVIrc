//=============================================================================
//
//   File : kvi_splash.cpp
//   Creation date : Wed Aug 8 2001 17:46:10 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#define __KVIRC__
#include "kvi_string.h"
#include "kvi_app.h"
#include "kvi_defaults.h"
#include "kvi_splash.h"
#include "kvi_locale.h"
#include "kvi_fileutils.h"
#include <qsplashscreen.h> 

#ifdef COMPILE_USE_QT4
	#include <qdesktopwidget.h>
#endif

#include <qpixmap.h>
#include <qpainter.h>
#include <qlayout.h>

#include <stdio.h>


KviSplashScreen::KviSplashScreen()
: QSplashScreen(QPixmap(1,1),Qt::WStyle_NoBorder | Qt::WType_TopLevel | Qt::WStyle_Customize | Qt::WStyle_StaysOnTop | Qt::WStyle_Splash)
{
	QString szPix;
	QPixmap * pix = 0;

	// check for the current theme splash screen pointer
	QString szPointerFile;
	g_pApp->getLocalKvircDirectory(szPointerFile,KviApp::Themes,"current-splash");
	if(KviFileUtils::fileExists(szPointerFile))
	{
		QString szBuf;
		KviFileUtils::readFile(szPointerFile,szBuf);
		if(!szBuf.isEmpty())
		{
			g_pApp->getLocalKvircDirectory(szPix,KviApp::Themes,szBuf);
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
// 				g_pApp->getLocalKvircDirectory(szPix,KviApp::Themes,szBuf);
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

#ifdef COMPILE_USE_QT4
	setWindowOpacity(0);
	m_bIncreasing=true;
	m_rTransparency=0;
	m_pFadeTimer= new QTimer(this);
	connect(m_pFadeTimer,SIGNAL(timeout()),this,SLOT(fadeTimerShot()));
	m_pFadeTimer->start(6);
#endif
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


void KviSplashScreen::showEvent(QShowEvent *e)
{
	move((g_pApp->desktop()->width() - width())/2,
		(g_pApp->desktop()->height() - height())/2);
	m_creationTime = QTime::currentTime();
}

void KviSplashScreen::hideEvent(QHideEvent *e)
{
	suicide();
}

void KviSplashScreen::setProgress(int progress)
{
#ifdef COMPILE_USE_QT4
	QPixmap slowQt4Copy = pixmap();
	QPainter painter(&slowQt4Copy);
	QSize size = slowQt4Copy.size();
	int w = (m_pOverlay->width() * progress) / 100;
	painter.drawPixmap(0,size.height() - m_pOverlay->height(),w,m_pOverlay->height(),*m_pOverlay,0,0,w,m_pOverlay->height());
	painter.end();
	setPixmap(slowQt4Copy);
#else
	QPainter painter(pixmap());
	QSize size = pixmap()->size();
	painter.drawPixmap(0,size.height() - m_pOverlay->height(),*m_pOverlay,0,0,(m_pOverlay->width() * progress) / 100,m_pOverlay->height());
	painter.end();
#endif
	//raise();
	repaint();
	g_pApp->processEvents(); //damn...
}

#define KVI_SPLASH_SCREEN_MINIMUM_TIMEOUT_IN_MSECS 2000

void KviSplashScreen::die()
{
#ifdef COMPILE_USE_QT4
	m_bIncreasing = false;
	m_pFadeTimer->start(6);
#else
	QTime now = QTime::currentTime();
	int mSecs = m_creationTime.msecsTo(now);
	int mRemaining = KVI_SPLASH_SCREEN_MINIMUM_TIMEOUT_IN_MSECS - mSecs;
	if(mRemaining < 0)mRemaining = 0;
	m_pTimer->start(mRemaining,true);
#endif
}


void KviSplashScreen::suicide()
{
	if(!g_pSplashScreen)return; // already in suicide ?
	//g_pApp->collectGarbage(this);
	g_pSplashScreen = 0;
	deleteLater();
	//delete this;
}


void KviSplashScreen::fadeTimerShot()
{
#ifdef COMPILE_USE_QT4
	if(m_bIncreasing)
	{
		m_rTransparency+=0.05;
		setWindowOpacity(m_rTransparency);
		if(m_rTransparency>=1)
		{
			m_pFadeTimer->stop();
			m_bIncreasing=false;
		}


	} else {
		m_rTransparency-=0.02;
		setWindowOpacity(m_rTransparency);
		if(m_rTransparency<=0)
		{
			m_pFadeTimer->stop();
			m_bIncreasing=true;
			suicide();
		}
	}
#endif
}


#include "kvi_splash.moc"
