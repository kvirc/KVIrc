#ifndef _MP_DCOPINTERFACE_H_
#define _MP_DCOPINTERFACE_H_
//=============================================================================
//
//   File : mp_dcopinterface.h
//   Created on Sun 27 Mar 2005 16:56:21 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005 Szymon Stefanek <pragma at kvirc dot net>
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

#include "kvi_settings.h"

#ifdef COMPILE_KDE_SUPPORT

#include "mp_interface.h"

class KviMediaPlayerDCOPInterface : public KviMediaPlayerInterface
{
public:
	KviMediaPlayerDCOPInterface(const QCString &szAppId);
	~KviMediaPlayerDCOPInterface();
protected:
	QCString m_szAppId;
protected:
	bool ensureAppRunning(const QString &szApp);
	bool findRunningApp(const QString &szApp);
	bool startApp(const QString &szApp,int iWaitMSecs = 0);
	int detectApp(const QString &szApp,bool bStart,int iScoreWhenFound,int iScoreWhenStarted);
	bool simpleDCOPCall(const QCString &szObj,const QCString &szFunc);
	bool intDCOPCall(const QCString &szObj,const QCString &szFunc,int iVal);
	bool boolDCOPCall(const QCString &szObj,const QCString &szFunc,bool bVal);
	bool stringDCOPCall(const QCString &szObj,const QCString &szFunc,QString szVal);
	bool floatDCOPCall(const QCString &szObj,const QCString &szFunc,float fVal);
	bool stringRetDCOPCall(const QCString &szObj,const QCString &szFunc,QString &ret);
	bool intRetDCOPCall(const QCString &szObj,const QCString &szFunc,int &ret);
	bool boolRetDCOPCall(const QCString &szObj,const QCString &szFunc,bool &ret);
};

#endif //COMPILE_KDE_SUPPORT

#endif //!_MP_DCOPINTERFACE_H_
