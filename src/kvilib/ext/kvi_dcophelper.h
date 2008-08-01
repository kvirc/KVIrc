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
#include "kvi_qcstring.h"
#include <QList>

#ifdef COMPILE_KDE3_SUPPORT

typedef QList<KviQCString> KviQCStringList;

class KVILIB_API KviDCOPHelper
{

public:

	// Constructs a DCOP helper object.
	// bStartApp: tries to start application when a dcop call is about to be made and the app is not already running
	// szAppID: application name as seen by DCOP
	KviDCOPHelper(bool bStartApp, const KviQCString &szAppId);
	~KviDCOPHelper();

protected:

	KviQCString 	m_szAppId;
	bool 	m_bStartApp;

protected:

	bool 	ensureAppRunning(const QString &szApp);
	bool 	findRunningApp(const QString &szApp);
	bool 	startApp(const QString &szApp,int iWaitMSecs = 0);
	int 		detectApp(const QString &szApp,bool bStart,int iScoreWhenFound,int iScoreWhenStarted);

	// naming convention: [return value] Ret [argument type(s)] DCOPCall

	bool 	voidRetVoidDCOPCall(const KviQCString &szObj,const KviQCString &szFunc);
	bool 	voidRetIntDCOPCall(const KviQCString &szObj,const KviQCString &szFunc,int iVal);
	bool 	voidRetIntBoolDCOPCall(const KviQCString &szObj,const KviQCString &szFunc,int iVal, bool bVal);
	bool 	voidRetIntIntIntDCOPCall(const KviQCString &szObj,const KviQCString &szFunc,int iVal1, int iVal2, int iVal3);
	bool 	voidRetBoolDCOPCall(const KviQCString &szObj,const KviQCString &szFunc,bool bVal);
	bool 	voidRetStringDCOPCall(const KviQCString &szObj,const KviQCString &szFunc,const QString &szVal);
	bool 	voidRetFloatDCOPCall(const KviQCString &szObj,const KviQCString &szFunc,float fVal);
	bool 	stringRetVoidDCOPCall(const KviQCString &szObj,const KviQCString &szFunc,QString &szRet);
	bool 	stringRetIntDCOPCall(const KviQCString &szObj,const KviQCString &szFunc,QString &szRet,int iVal);
	bool 	intRetVoidDCOPCall(const KviQCString &szObj,const KviQCString &szFunc,int &ret);
	bool 	intRetIntDCOPCall(const KviQCString &szObj,const KviQCString &szFunc,int &ret, int iVal);
	bool 	boolRetVoidDCOPCall(const KviQCString &szObj,const KviQCString &szFunc,bool &ret);

	bool		qvalueListIntRetIntDCOPCall(const KviQCString &szObj,const KviQCString &szFunc,QList<int> &ret, int iVal);
	bool		qcstringListRetIntDCOPCall(const KviQCString &szObj,const KviQCString &szFunc,KviQCStringList &ret, int iVal);
	bool		qcstringListRetVoidDCOPCall(const KviQCString &szObj,const KviQCString &szFunc,KviQCStringList &ret);
};

#endif //COMPILE_KDE_SUPPORT

#endif // _KVI_DCOPHELPER_H_
