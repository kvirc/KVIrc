#ifndef _KVI_SPLASH_H_
#define _KVI_SPLASH_H_
//=============================================================================
//
//   File : KviSplashScreen.h
//   Creation date : Wed Aug 8 2001 17:45:12 CEST by Szymon Stefanek
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

/**
* \file KviSplashScreen.h
* \author Szymon Stefanek
* \brief Splash screen
*
* \def KVI_SPLASH_SCREEN_MINIMUM_TIMEOUT_IN_MSECS The minimum timeout in msec
* \def KVI_SPLASH_SET_PROGRESS Updates the splash screen progress bar
*/

#include "kvi_settings.h"

#include <QFrame>
#include <QProgressBar>
#include <QLabel>
#include <QTimer>
#include <QDateTime>
#include <QSplashScreen>
#include <QPixmap>

#define KVI_SPLASH_SCREEN_MINIMUM_TIMEOUT_IN_MSECS 2000

#define KVI_SPLASH_SET_PROGRESS(__val) \
	if(g_pSplashScreen) \
		g_pSplashScreen->setProgress(__val);

//#define KVI_SPLASH_SET_TEXT(__txt) if(g_pSplashScreen){ g_pSplashScreen->message(__txt); qDebug(__txt.latin1()); }

/**
* \class KviSplashScreen
* \brief Splash screen class
*/
class KVIRC_API KviSplashScreen : public QSplashScreen
{
	Q_OBJECT
public:
	/**
	* \brief Constructs the splash screen object
	* \return KviSplashScreen
	*/
	KviSplashScreen();

	/**
	* \brief Destroys the splash screen object
	*/
	virtual ~KviSplashScreen();
private:
	QTimer  * m_pTimer;
	QTime     m_creationTime;
	QPixmap * m_pOverlay;
	bool      m_bIncreasing;
	qreal     m_rTransparency;
	QTimer  * m_pFadeTimer;
public:
	/**
	* \brief Sets the progress in steps
	* \param iProgress The progress step
	* \return void
	*/
	void setProgress(int iProgress);

	/**
	* \brief Kills the splash screen
	* \return void
	*/
	void die();

	//void message(QString);
protected slots:
	/**
	* \brief Called when we want to kill the splash screen
	* \return void
	*/
	void suicide();

	/**
	* \brief Sets the fading effect
	* \return void
	*/
	void fadeTimerShot();
protected:
	virtual void showEvent(QShowEvent * e);
	virtual void hideEvent(QHideEvent * e);
};

extern KVIRC_API KviSplashScreen * g_pSplashScreen;

#endif //_KVI_SPLASH_H_
