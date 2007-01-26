#ifndef _KVI_DCOPHELPER_H_
#define _KVI_DCOPHELPER_H_
//=============================================================================
//
//   File : kvi_dcophelper.h
//   Created on Sat 20 Jan 2007 12:35:21 by Alexander Stillich
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005 Szymon Stefanek <pragma at kvirc dot net>
//   Copyright (C) 2007 Alexander Stillich <torque at pltn dot org>
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
#include "kvi_qstring.h"
#include "kvi_valuelist.h"

#ifdef COMPILE_KDE_SUPPORT

typedef KviValueList<KviQCString> KviQCStringList;

class KviDCOPHelper
{

public:

	// Constructs a DCOP helper object.
	// bStartApp: tries to start application when a dcop call is about to be made and the app is not already running
	// szAppID: application name as seen by DCOP
	KviDCOPHelper(bool bStartApp, const QCString &szAppId);
	~KviDCOPHelper();

protected:

	QCString 	m_szAppId;
	bool 	m_bStartApp;

protected:

	bool 	ensureAppRunning(const QString &szApp);
	bool 	findRunningApp(const QString &szApp);
	bool 	startApp(const QString &szApp,int iWaitMSecs = 0);
	int 		detectApp(const QString &szApp,bool bStart,int iScoreWhenFound,int iScoreWhenStarted);

	// naming convention: [return value] Ret [argument type(s)] DCOPCall

	bool 	voidRetVoidDCOPCall(const QCString &szObj,const QCString &szFunc);
	bool 	voidRetIntDCOPCall(const QCString &szObj,const QCString &szFunc,int iVal);
	bool 	voidRetIntBoolDCOPCall(const QCString &szObj,const QCString &szFunc,int iVal, bool bVal);
	bool 	voidRetIntIntIntDCOPCall(const QCString &szObj,const QCString &szFunc,int iVal1, int iVal2, int iVal3);
	bool 	voidRetBoolDCOPCall(const QCString &szObj,const QCString &szFunc,bool bVal);
	bool 	voidRetStringDCOPCall(const QCString &szObj,const QCString &szFunc,const QString &szVal);
	bool 	voidRetFloatDCOPCall(const QCString &szObj,const QCString &szFunc,float fVal);
	bool 	stringRetVoidDCOPCall(const QCString &szObj,const QCString &szFunc,QString &ret);
	bool 	intRetVoidDCOPCall(const QCString &szObj,const QCString &szFunc,int &ret);
	bool 	intRetIntDCOPCall(const QCString &szObj,const QCString &szFunc,int &ret, int iVal);
	bool 	boolRetVoidDCOPCall(const QCString &szObj,const QCString &szFunc,bool &ret);

	bool		qvalueListIntRetIntDCOPCall(const QCString &szObj,const QCString &szFunc,KviValueList<int> &ret, int iVal);
	bool		qcstringListRetIntDCOPCall(const QCString &szObj,const QCString &szFunc,KviQCStringList &ret, int iVal);
	bool		qcstringListRetVoidDCOPCall(const QCString &szObj,const QCString &szFunc,KviQCStringList &ret);
};

#endif //COMPILE_KDE_SUPPORT

#endif // _KVI_DCOPHELPER_H_
