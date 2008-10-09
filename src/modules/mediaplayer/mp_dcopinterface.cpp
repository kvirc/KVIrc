//=============================================================================
//
//   File : mp_dcopinterface.cpp
//   Creation date : Sun 27 Mar 2005 16:56:21 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005-2008 Szymon Stefanek <pragma at kvirc dot net>
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

#include "mp_dcopinterface.h"

#ifdef COMPILE_KDE3_SUPPORT

#include "dcopclient.h"

#include <QDataStream>
#include <QLinkedList>

#include "kvi_app.h"
#include "kvi_thread.h"

KviMediaPlayerDCOPInterface::KviMediaPlayerDCOPInterface(const KviQCString &szAppId)
: KviMediaPlayerInterface()
{
	m_szAppId = szAppId;
}

KviMediaPlayerDCOPInterface::~KviMediaPlayerDCOPInterface()
{
}

bool KviMediaPlayerDCOPInterface::ensureAppRunning(const QString &szApp)
{
	if(findRunningApp(m_szAppId))return true;
	return startApp(m_szAppId,400);
}


bool KviMediaPlayerDCOPInterface::simpleDCOPCall(const KviQCString &szObj,const KviQCString &szFunc)
{
	if(!ensureAppRunning(m_szAppId))return false;
	QByteArray data;
	return g_pApp->dcopClient()->send(m_szAppId,szObj,szFunc,data);
}

bool KviMediaPlayerDCOPInterface::boolDCOPCall(const KviQCString &szObj,const KviQCString &szFunc,bool bVal)
{
	if(!ensureAppRunning(m_szAppId))return false;
	QByteArray data;
	QDataStream arg(data, IO_WriteOnly);
	arg << bVal;
	return g_pApp->dcopClient()->send(m_szAppId,szObj,szFunc,data);
}

bool KviMediaPlayerDCOPInterface::intDCOPCall(const KviQCString &szObj,const KviQCString &szFunc,int iVal)
{
	if(!ensureAppRunning(m_szAppId))return false;
	QByteArray data;
	QDataStream arg(data, IO_WriteOnly);
	arg << iVal;
	return g_pApp->dcopClient()->send(m_szAppId,szObj,szFunc,data);
}

bool KviMediaPlayerDCOPInterface::floatDCOPCall(const KviQCString &szObj,const KviQCString &szFunc,float fVal)
{
	if(!ensureAppRunning(m_szAppId))return false;
	QByteArray data;
	QDataStream arg(data, IO_WriteOnly);
	arg << fVal;
	return g_pApp->dcopClient()->send(m_szAppId,szObj,szFunc,data);
}

bool KviMediaPlayerDCOPInterface::stringDCOPCall(const KviQCString &szObj,const KviQCString &szFunc,QString szVal)
{
	if(!ensureAppRunning(m_szAppId))return false;
	QByteArray data;
	QDataStream arg(data, IO_WriteOnly);
	arg << szVal;
	return g_pApp->dcopClient()->send(m_szAppId,szObj,szFunc,data);
}

bool KviMediaPlayerDCOPInterface::stringRetDCOPCall(const KviQCString &szObj,const KviQCString &szFunc,QString &ret)
{
	if(!ensureAppRunning(m_szAppId))return false;
	QString title;
	QByteArray data, replyData;
	KviQCString replyType;
	if(!g_pApp->dcopClient()->call(m_szAppId,szObj,szFunc,data,replyType,replyData))
		return false;
	QDataStream reply( replyData, IO_ReadOnly );
	if(replyType == "QString")
	{
		reply >> ret;
		return true;
	}
	return false;
}

bool KviMediaPlayerDCOPInterface::intRetDCOPCall(const KviQCString &szObj,const KviQCString &szFunc,int &ret)
{
	if(!ensureAppRunning(m_szAppId))return false;
	QString title;
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

bool KviMediaPlayerDCOPInterface::boolRetDCOPCall(const KviQCString &szObj,const KviQCString &szFunc,bool &ret)
{
	if(!ensureAppRunning(m_szAppId))return false;
	QString title;
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


bool KviMediaPlayerDCOPInterface::findRunningApp(const QString &szApp)
{
	QLinkedList<KviQCString> allApps = g_pApp->dcopClient()->registeredApplications();
	QLinkedList<KviQCString>::iterator iterator;
	KviQCString sz = szApp.toLocal8Bit();
	for(iterator = allApps.begin();iterator != allApps.end();iterator++)
	{
		if(*iterator == sz)return true;
	}
	return false;
}

int KviMediaPlayerDCOPInterface::detectApp(const QString &szApp,bool bStart,int iScoreWhenFound,int iScoreWhenStarted)
{
	// lookup the amarok interface
	if(!g_pApp->dcopClient())return 0;
	if(findRunningApp(szApp))return 95; // found a running amarok, no need to run further

	// no amarok app found
	if(bStart)
	{
		// try to start it
		if(!startApp(szApp,5000))return 10; // very low possibility
		return findRunningApp(szApp) ? 99 : 0; // try to find it again
	}

	return 30; // it still might be installed on the system but we're just unable to start it...
}


bool KviMediaPlayerDCOPInterface::startApp(const QString &szApp,int iWaitMSecs)
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


#endif //COMPILE_KDE3_SUPPORT
