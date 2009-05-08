#ifndef _KVI_SPLASH_H_
#define _KVI_SPLASH_H_
//=============================================================================
//
//   File : kvi_splash.h
//   Creation date : Wed Aug 8 2001 17:45:12 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2005 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include "kvi_settings.h"


#include <qframe.h>

#include <qprogressbar.h>
#include <qlabel.h>
#include <qtimer.h>
#include <qdatetime.h>
#include <qsplashscreen.h> 
#include <qpixmap.h>

class KVIRC_API KviSplashScreen : public QSplashScreen
{
	Q_OBJECT
public:
	KviSplashScreen();
	virtual ~KviSplashScreen();
private:
	QTimer         * m_pTimer;
	QTime            m_creationTime;
	QPixmap        * m_pOverlay;
#ifdef COMPILE_USE_QT4
	bool             m_bIncreasing;
	qreal            m_rTransparency;
	QTimer         * m_pFadeTimer;
#endif
protected:
	virtual void showEvent(QShowEvent * e);
	virtual void hideEvent(QHideEvent * e);
public:
	void setProgress(int progress);
	void die();
	//void message(QString);
protected slots:
	void suicide();
	void fadeTimerShot();
};

extern KVIRC_API KviSplashScreen * g_pSplashScreen;

#define KVI_SPLASH_SET_PROGRESS(__val) if(g_pSplashScreen)g_pSplashScreen->setProgress(__val);
//#define KVI_SPLASH_SET_TEXT(__txt) if(g_pSplashScreen){ g_pSplashScreen->message(__txt); debug(__txt.latin1()); }


#endif //_KVI_SPLASH_H_
