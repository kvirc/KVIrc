//=============================================================================
//
//   File : kvi_dcophelper.cpp
//   Creation date : Sat 20 Jan 2005 12:35:21 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005 Szymon Stefanek <pragma at kvirc dot net>
//   Copyright (C) 2007-2008 Alexander Stillich <torque at pltn dot org>
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

#include "kvi_dcophelper.h"

#ifdef COMPILE_KDE3_SUPPORT

#include "dcopclient.h"

#include <QDataStream>
#include <QList>

// must be included this way, since kvilib is built
// before kvirc and symlinks to headers aren't set yet
#include "../../kvirc/kernel/kvi_app.h"
#include "kvi_thread.h"

KviDCOPHelper::KviDCOPHelper(bool bStartApp, const KviQCString &szAppId)
{
	m_szAppId = szAppId;
}

KviDCOPHelper::~KviDCOPHelper()
{
}

bool KviDCOPHelper::ensureAppRunning(const QString &szApp)
{
	if (findRunningApp(m_szAppId))
		return true;

	if (m_bStartApp)
		return startApp(m_szAppId,400);

	return false;
}


bool KviDCOPHelper::voidRetVoidDCOPCall(const KviQCString &szObj,const KviQCString &szFunc)
{
	if(!ensureAppRunning(m_szAppId))return false;
	QByteArray data;
	return g_pApp->dcopClient()->send(m_szAppId,szObj,szFunc,data);
}

bool KviDCOPHelper::voidRetBoolDCOPCall(const KviQCString &szObj,const KviQCString &szFunc,bool bVal)
{
	if(!ensureAppRunning(m_szAppId))return false;
	QByteArray data;
	QDataStream arg(data, IO_WriteOnly);
	arg << bVal;
	return g_pApp->dcopClient()->send(m_szAppId,szObj,szFunc,data);
}

bool KviDCOPHelper::voidRetIntDCOPCall(const KviQCString &szObj,const KviQCString &szFunc,int iVal)
{
	if(!ensureAppRunning(m_szAppId))return false;
	QByteArray data;
	QDataStream arg(data, IO_WriteOnly);
	arg << iVal;
	return g_pApp->dcopClient()->send(m_szAppId,szObj,szFunc,data);
}

bool KviDCOPHelper::voidRetIntBoolDCOPCall(const KviQCString &szObj,const KviQCString &szFunc,int iVal, bool bVal)
{
	if(!ensureAppRunning(m_szAppId))return false;
	QByteArray data;
	QDataStream arg(data, IO_WriteOnly);
	arg << iVal;
	arg << bVal;
	return g_pApp->dcopClient()->send(m_szAppId,szObj,szFunc,data);
}

bool KviDCOPHelper::voidRetIntIntIntDCOPCall(const KviQCString &szObj,const KviQCString &szFunc,int iVal1, int iVal2, int iVal3)
{
	if(!ensureAppRunning(m_szAppId))return false;
	QByteArray data;
	QDataStream arg(data, IO_WriteOnly);
	arg << iVal1;
	arg << iVal2;
	arg << iVal3;
	return g_pApp->dcopClient()->send(m_szAppId,szObj,szFunc,data);
}

bool KviDCOPHelper::voidRetFloatDCOPCall(const KviQCString &szObj,const KviQCString &szFunc,float fVal)
{
	if(!ensureAppRunning(m_szAppId))return false;
	QByteArray data;
	QDataStream arg(data, IO_WriteOnly);
	arg << fVal;
	return g_pApp->dcopClient()->send(m_szAppId,szObj,szFunc,data);
}

bool KviDCOPHelper::voidRetStringDCOPCall(const KviQCString &szObj,const KviQCString &szFunc,const QString &szVal)
{
	if(!ensureAppRunning(m_szAppId))return false;
	QByteArray data;
	QDataStream arg(data, IO_WriteOnly);
	arg << szVal;
	return g_pApp->dcopClient()->send(m_szAppId,szObj,szFunc,data);
}

bool KviDCOPHelper::stringRetVoidDCOPCall(const KviQCString &szObj,const KviQCString &szFunc,QString &szRet)
{
	if(!ensureAppRunning(m_szAppId))return false;
	QByteArray data, replyData;
	KviQCString replyType;
	if(!g_pApp->dcopClient()->call(m_szAppId,szObj,szFunc,data,replyType,replyData))
		return false;
	QDataStream reply( replyData, IO_ReadOnly );
	if(replyType == "QString")
	{
		reply >> szRet;
		return true;
	}
	return false;
}

bool KviDCOPHelper::stringRetIntDCOPCall(const KviQCString &szObj,const KviQCString &szFunc,QString &szRet,int iVal)
{
	if(!ensureAppRunning(m_szAppId))return false;
	QByteArray data, replyData;
	KviQCString replyType;

	QDataStream arg(data, IO_WriteOnly);
	arg << iVal;

	if(!g_pApp->dcopClient()->call(m_szAppId,szObj,szFunc,data,replyType,replyData))
		return false;

	QDataStream reply( replyData, IO_ReadOnly );
	if(replyType == "QString")
	{
		reply >> szRet;
		return true;
	}
	return false;
}

bool KviDCOPHelper::intRetVoidDCOPCall(const KviQCString &szObj,const KviQCString &szFunc,int &ret)
{
	if(!ensureAppRunning(m_szAppId))return false;
	QByteArray data, replyData;
	KviQCString replyType;
	if(!g_pApp->dcopClient()->call(m_szAppId,szObj,szFunc,data,replyType,replyData))
		return false;
	QDataStream reply( replyData, IO_ReadOnly );
	if(replyType == "int")
	{
		reply >> ret;
		return true;
	}
	return false;
}

bool KviDCOPHelper::intRetIntDCOPCall(const KviQCString &szObj,const KviQCString &szFunc,int &ret, int iVal)
{
	if(!ensureAppRunning(m_szAppId))return false;
	QByteArray data, replyData;
	KviQCString replyType;

	QDataStream arg(data, IO_WriteOnly);
	arg << iVal;

	if(!g_pApp->dcopClient()->call(m_szAppId,szObj,szFunc,data,replyType,replyData))
		return false;

	QDataStream reply( replyData, IO_ReadOnly );
	if(replyType == "int")
	{
		reply >> ret;
		return true;
	}
	return false;
}

bool KviDCOPHelper::boolRetVoidDCOPCall(const KviQCString &szObj,const KviQCString &szFunc,bool &ret)
{
	if(!ensureAppRunning(m_szAppId))return false;
	QByteArray data, replyData;
	KviQCString replyType;
	if(!g_pApp->dcopClient()->call(m_szAppId,szObj,szFunc,data,replyType,replyData))
		return false;
	QDataStream reply( replyData, IO_ReadOnly );
	if(replyType == "bool")
	{
		reply >> ret;
		return true;
	}
	return false;
}

bool KviDCOPHelper::qvalueListIntRetIntDCOPCall(const KviQCString &szObj,const KviQCString &szFunc,QList<int> &ret, int iVal)
{
	if(!ensureAppRunning(m_szAppId))
		return false;

	QByteArray data, replyData;
	KviQCString replyType;
	QDataStream arg(data, IO_WriteOnly);

	arg << iVal;


	if(!g_pApp->dcopClient()->call(m_szAppId,szObj,szFunc,data,replyType,replyData))
		return false;

	if(replyType != "QList<int>")
		return false;

	QDataStream replyStream(replyData, IO_ReadOnly);
	replyStream >> ret;

	return true;
}

bool KviDCOPHelper::qcstringListRetVoidDCOPCall(const KviQCString &szObj,const KviQCString &szFunc,KviQCStringList &ret)
{
	QByteArray data, replyData;
	KviQCString replyType;

	if (!g_pApp->dcopClient()->call(m_szAppId,szObj,szFunc,data,replyType,replyData))
		return false;

	if (replyType != "KviQCStringList")
		return false;

	QDataStream replyStream(replyData, IO_ReadOnly);
	replyStream >> ret;

	return true;
}

bool KviDCOPHelper::qcstringListRetIntDCOPCall(const KviQCString &szObj,const KviQCString &szFunc,KviQCStringList &ret, int iVal)
{
	QByteArray data, replyData;
	KviQCString replyType;

	QDataStream arg(data, IO_WriteOnly);
	arg << iVal;

	if (!g_pApp->dcopClient()->call(m_szAppId,szObj,szFunc,data,replyType,replyData))
		return false;

	if (replyType != "KviQCStringList")
		return false;

	QDataStream replyStream(replyData, IO_ReadOnly);
	replyStream >> ret;

	return true;
}

bool KviDCOPHelper::findRunningApp(const QString &szApp)
{
	QList<KviQCString> allApps = g_pApp->dcopClient() ->registeredApplications();
	QList<KviQCString>::iterator iterator;
	KviQCString sz = szApp.toLocal8Bit();
	for (iterator = allApps.begin();iterator != allApps.end();iterator++)
	{
		if(*iterator == sz)
			return true;
	}
	return false;
}

int KviDCOPHelper::detectApp(const QString &szApp,bool bStart,int iScoreWhenFound,int iScoreWhenStarted)
{
	// dcop available
	if(!g_pApp->dcopClient())
		return 0;

	if(findRunningApp(szApp))
		return 95; // found a running app, no need to run further

	// no app found running
	if(bStart)
	{
		// try to start it
		if(!startApp(szApp,5000))
			return 10; // very low possibility
		return findRunningApp(szApp) ? 99 : 0; // try to find it again
	}

	return 30; // it still might be installed on the system but we're just unable to start it...
}


bool KviDCOPHelper::startApp(const QString &szApp,int iWaitMSecs)
{
	// we could use KApplication::startServiceByDesktopName here
	// but we want to be able to wait a defined amount of time
	QStringList tmp;
	QByteArray data, replyData;
	KviQCString replyType;
	QDataStream arg(data, IO_WriteOnly);
	arg << szApp << tmp;
	if(!g_pApp->dcopClient()->call(
			"klauncher",
			"klauncher",
			"start_service_by_desktop_name(QString,QStringList)",
			data,
			replyType,
			replyData))
	{
		return false;
	} else {
		QDataStream reply(replyData, IO_ReadOnly);
		if(replyType != "serviceResult")return false;
		int result;
		KviQCString dcopName;
		QString error;
		reply >> result >> dcopName >> error;
		if(result != 0)return false;
	}
	// ok , we seem to have started it.. but it might take some seconds
	// for the app to get registered
	// we wait up to five seconds
	if(iWaitMSecs > 0)
	{
		int i = 0;
		while(i < iWaitMSecs)
		{
			if(findRunningApp(szApp))return true;
			KviThread::msleep(100);
			i += 100;
		}
		return findRunningApp(szApp);
	}
	return true;
}


#endif //COMPILE_KDE_SUPPORT
