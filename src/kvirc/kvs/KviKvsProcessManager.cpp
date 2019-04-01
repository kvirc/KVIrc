//=============================================================================
//
//   File : KviKvsProcessManager.cpp
//   Creation date : Wed 07 Apr 2004 03:03:52 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "KviKvsProcessManager.h"
#include "KviWindow.h"
#include "kvi_out.h"
#include "KviLocale.h"
#include "KviApplication.h"
#include "KviConsoleWindow.h"
#include "KviKvsScript.h"
//#include <QSysInfo>

KviKvsProcessAsyncOperation::KviKvsProcessAsyncOperation(KviKvsProcessDescriptorData * d)
    : KviKvsAsyncOperation(d->pWnd)
{
	m_pData = d;
	m_pProcess = nullptr;
	m_pExtendedRunTimeData = new KviKvsExtendedRunTimeData(new KviKvsHash(), true);
	m_pPingTimer = nullptr;
	m_pRunTimeTimer = nullptr;
	m_bDeletePending = false;
}

KviKvsProcessAsyncOperation::~KviKvsProcessAsyncOperation()
{
	if(m_pPingTimer)
		delete m_pPingTimer;
	if(m_pRunTimeTimer)
		delete m_pRunTimeTimer;
	if(m_pProcess)
	{
		QObject::disconnect(m_pProcess, nullptr, this, nullptr);
		m_pProcess->kill();
		delete m_pProcess;
	}
	delete m_pExtendedRunTimeData;
	if(m_pData->pCallback)
		delete m_pData->pCallback;
	if(m_pData->pMagic)
		delete m_pData->pMagic;
	delete m_pData;
}

bool KviKvsProcessAsyncOperation::start()
{
	QStringList args;
	QString szcmd;

	if(m_pData->iFlags & KVI_KVS_PROCESSDESCRIPTOR_NOSHELL)
	{
		args = m_pData->szCommandline.split(" ", QString::SkipEmptyParts);
	}
	else
	{
		QString szShell = m_pData->szShell;
		if(szShell.isEmpty())
		{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
			// [01:26:00] <PragmaOff> btw, what is qt_winunicode ?
			// [01:26:12] <kode54> Qt export specific to win32
			// [01:26:27] <kode54> bool which indicates whether system is Unicode (NT) or not
			// [01:26:58] <kode54> not sure if that's documented, but it is a public export
			//
			// [02:50:21] <kode54> if ( QApplication::winVersion() & Qt::WV_NT_based )
			// [02:50:41] <kode54> I see another implementation using that, maybe it is the official way of detecting that :[
			szShell = !(QSysInfo::WindowsVersion & QSysInfo::WV_DOS_based) ? "cmd.exe /c" : "command.com /c";
// Thnx kode54 :)
#else
			szShell = "sh -c";
#endif
		}
		args = szShell.split(" ", QString::SkipEmptyParts);
		args.append(m_pData->szCommandline);
	}

	m_pProcess = new QProcess(this);

	if(m_pData->iFlags & KVI_KVS_PROCESSDESCRIPTOR_TRIGGERSTDOUT)
	{
		connect(m_pProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(readStdout()));
	}
	else
	{
		m_pProcess->closeReadChannel(QProcess::StandardOutput);
	}

	if(m_pData->iFlags & KVI_KVS_PROCESSDESCRIPTOR_TRIGGERSTDERR)
	{
		connect(m_pProcess, SIGNAL(readyReadStandardError()), this, SLOT(readStderr()));
	}
	else
	{
		m_pProcess->closeReadChannel(QProcess::StandardError);
	}

	connect(m_pProcess, SIGNAL(finished(int)), this, SLOT(processExited(int)));

	if(m_pData->iFlags & KVI_KVS_PROCESSDESCRIPTOR_TRIGGERSTARTED)
	{
		connect(m_pProcess, SIGNAL(started()), this, SLOT(processStarted()));
	}

	szcmd = args.takeFirst();
	m_pProcess->start(szcmd, args);

	if(m_pProcess->state() == QProcess::NotRunning)
	{
		return false;
	}

	if(m_pData->iMaxRunTime > 0)
	{
		m_pRunTimeTimer = new QTimer(this);
		connect(m_pRunTimeTimer, SIGNAL(timeout()), this, SLOT(maxRunTimeExpired()));
		m_pRunTimeTimer->start(m_pData->iMaxRunTime);
	}

	if(m_pData->iPingTimeout > 0)
	{
		m_pPingTimer = new QTimer(this);
		connect(m_pPingTimer, SIGNAL(timeout()), this, SLOT(ping()));
		m_pPingTimer->start(m_pData->iPingTimeout);
	}

	return true;
}

void KviKvsProcessAsyncOperation::ping()
{
	if(trigger(EventPing, QString()))
	{
		triggerSelfDelete();
	}
}

void KviKvsProcessAsyncOperation::triggerSelfDelete()
{
	if(m_bDeletePending)
		return;
	m_bDeletePending = true;
	QTimer::singleShot(m_pData->iMaxRunTime, this, SLOT(selfDelete()));
}

void KviKvsProcessAsyncOperation::selfDelete()
{
	delete this;
}

void KviKvsProcessAsyncOperation::maxRunTimeExpired()
{
	trigger(EventTerminated, "0");
	triggerSelfDelete();
}

bool KviKvsProcessAsyncOperation::trigger(CallbackEvent e, const QString & szData)
{
	if(m_bDeletePending)
		return false;

	if(!g_pApp->windowExists(m_pData->pWnd))
	{
		if(m_pData->iFlags & KVI_KVS_PROCESSDESCRIPTOR_KILLIFNOWINDOW)
		{
			return true;
		}
		m_pData->pWnd = g_pApp->activeConsole();
	}

	if(m_pData->pCallback)
	{
		KviKvsVariantList params;
		params.setAutoDelete(true);

		switch(e)
		{
			case EventStdout:
				params.append(new KviKvsVariant(QString("stdout")));
				break;
			case EventStderr:
				params.append(new KviKvsVariant(QString("stderr")));
				break;
			case EventTerminated:
				params.append(new KviKvsVariant(QString("terminated")));
				break;
			case EventStarted:
				params.append(new KviKvsVariant(QString("started")));
				break;
			case EventPing:
				params.append(new KviKvsVariant(QString("ping")));
				break;
			default:
				qDebug("Oops! Unknown trigger() CallbackEvent parameter in QProcessDescriptor::trigger()");
				return false;
				break;
		}

		params.append(new KviKvsVariant(szData));
		if(m_pData->pMagic)
		{
			KviKvsVariant * pTmp = new KviKvsVariant();
			pTmp->copyFrom(m_pData->pMagic);
			params.append(pTmp);
		}

		KviKvsVariant retVal;
		int iRet = m_pData->pCallback->run(m_pData->pWnd, &params, &retVal, KviKvsScript::PreserveParams, m_pExtendedRunTimeData);
		if(!iRet)
		{
			m_pData->pWnd->output(KVI_OUT_PARSERERROR,
			    __tr2qs_ctx("Error triggered from process callback handler: killing process", "kvs"));
			return true;
		}

		if(!retVal.isNothing())
		{
			QString sz;
			retVal.asString(sz);
			m_pProcess->write(sz.toUtf8().data());
		}

		if(iRet & KviKvsScript::HaltEncountered)
		{
			// halt encountered: kill the process
			return true;
		}
	}

	return false;
}

void KviKvsProcessAsyncOperation::readStdout()
{
	if(m_bDeletePending)
		return;
	if(m_pData->iFlags & KVI_KVS_PROCESSDESCRIPTOR_OUTPUTBYBLOCKS)
	{
		QByteArray a = m_pProcess->readAllStandardOutput();
		if(a.size() > 0)
			m_szStdoutBuffer += QString(a);
	}
	else
	{
		m_pProcess->setReadChannel(QProcess::StandardOutput);
		QString l;
		bool bBreak = false;
		while((m_pProcess->canReadLine()) && (!bBreak))
		{
			l = m_pProcess->readLine();
			if(m_pData->iFlags & KVI_KVS_PROCESSDESCRIPTOR_TRIGGERSTDOUT)
			{
				if(trigger(EventStdout, l))
				{
					bBreak = true;
					triggerSelfDelete();
				}
			}
		}
	}
}

void KviKvsProcessAsyncOperation::readStderr()
{
	if(m_bDeletePending)
		return;
	if(m_pData->iFlags & KVI_KVS_PROCESSDESCRIPTOR_OUTPUTBYBLOCKS)
	{
		QByteArray a = m_pProcess->readAllStandardError();
		if(a.size() > 0)
			m_szStderrBuffer += QString(a);
	}
	else
	{
		m_pProcess->setReadChannel(QProcess::StandardError);
		QString l;
		bool bBreak = false;
		while((m_pProcess->canReadLine()) && (!bBreak))
		{
			l = m_pProcess->readLine();

			if(m_pData->iFlags & KVI_KVS_PROCESSDESCRIPTOR_TRIGGERSTDERR)
			{
				if(trigger(EventStderr, l))
				{
					bBreak = true;
					triggerSelfDelete();
				}
			}
		}
	}
}

void KviKvsProcessAsyncOperation::processStarted()
{
	if(m_bDeletePending)
		return;

	QString szPid;
	szPid.setNum((intptr_t)(m_pProcess->pid()));
	if(trigger(EventStarted, szPid))
	{
		triggerSelfDelete();
		return;
	}
}

void KviKvsProcessAsyncOperation::processExited(int exitCode)
{
	if(m_bDeletePending)
		return;

	readStdout(); // just to make sure
	readStderr(); // just to make sure

	if(m_pData->iFlags & KVI_KVS_PROCESSDESCRIPTOR_OUTPUTBYBLOCKS)
	{
		// trigger Stdout and Stderr once
		if(m_pData->iFlags & KVI_KVS_PROCESSDESCRIPTOR_TRIGGERSTDOUT)
		{
			if(trigger(EventStdout, m_szStdoutBuffer))
			{
				triggerSelfDelete();
				return;
			}
		}

		if(m_pData->iFlags & KVI_KVS_PROCESSDESCRIPTOR_TRIGGERSTDERR)
		{
			if(trigger(EventStdout, m_szStderrBuffer))
			{
				triggerSelfDelete();
				return;
			}
		}
	}

	if(m_pData->iFlags & KVI_KVS_PROCESSDESCRIPTOR_TRIGGERTERMINATED)
	{
		QString szRetVal;
		szRetVal.setNum(exitCode);
		trigger(EventTerminated, szRetVal);
	}

	triggerSelfDelete();
}

/*

KviKvsProcessManager::KviKvsProcessManager()
: QObject()
{
}

KviKvsProcessManager::~KviKvsProcessManager()
{
}

void KviKvsProcessManager::init()
{
	if(m_pInstance)return;
	m_pInstance = new QProcessManager();
}

void KviKvsProcessManager::done()
{
	if(!m_pInstance)return;
	delete m_pInstance;
	m_pInstance = nullptr;
}

bool KviKvsProcessManager::execute(KviKvsProcessAsyncOperationData * d)
{
	KviKvsProcessAsyncOperation * pd = new KviKvsProcessAsyncOperation(d,this);
	if(!pd->start())
	{
		//delete d; <-- delete by KviKvsProcessAsyncOperation
		delete pd;
		return false;
	}
	return true;
}
*/
