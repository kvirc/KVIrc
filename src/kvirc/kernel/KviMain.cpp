//=============================================================================
//
//   File : KviMain.cpp
//   Creation date : Sun Jun 18 2000 12:38:45 by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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



#include "KviApplication.h"
#include "KviCString.h"
#include "kvi_settings.h"
#include "KviIrcUrl.h"
#include "kvi_defaults.h"
#include "kvi_sourcesdate.h"
#include "KviMessageBox.h"
#include "KviBuildInfo.h"
#ifdef COMPILE_DBUS_SUPPORT
	#ifndef COMPILE_KDE_SUPPORT // 'cause kde adds an interface itself
		#include "KviDbusAdaptor.h"
	#endif
#endif
#ifndef COMPILE_NO_IPC
	extern bool kvi_sendIpcMessage(const char * message); // KviIpcSentinel.cpp
#endif

#include <QtGlobal> //for qDebug()
#include <QMessageBox>

#ifdef COMPILE_KDE_SUPPORT
	#include <KCmdLineArgs>
	#include <KAboutData>
#endif

#define KVI_ARGS_RETCODE_OK 0
#define KVI_ARGS_RETCODE_ERROR 1
#define KVI_ARGS_RETCODE_STOP 2

typedef struct _ParseArgs
{
	int       argc;
	char **   argv;
	char *    configFile;
	bool      createFile;
	bool      bForceNewSession;
	bool      bShowPopup;
	bool      bShowSplashScreen;
	bool      bExecuteCommandAndClose;
	QString   szExecCommand;
	QString   szExecRemoteCommand;
} ParseArgs;

int parseArgs(ParseArgs * a)
{
	QString szServer;
	QString szPort;
	int idx;

	if(a->argc < 2)return KVI_ARGS_RETCODE_OK;

	for(idx = 1;idx < a->argc;idx++)
	{
		QString szMessage;
		char * p = a->argv[idx];

		if((kvi_strLen(p) > 3) && (*p == '-') && (*(p+1) == '-'))p++;

		if(kvi_strEqualCI("-v",p) || kvi_strEqualCI("-version",p))
		{
			KviQString::appendFormatted(szMessage,"KVIrc %s '%s'\n",KVI_VERSION,KVI_RELEASE_NAME);
			KviQString::appendFormatted(szMessage,"Sources date: %s\n",KVI_SOURCES_DATE);
			szMessage += "Build date: ";
			szMessage += KviBuildInfo::buildDate();
			szMessage += "\n";

			KviQString::appendFormatted(szMessage,"Home page: http://www.kvirc.net/\n");

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
			MessageBox(0,szMessage.toLocal8Bit().data(),"KVIrc",0);
#else
			qDebug("%s",szMessage.toLocal8Bit().data());
#endif

			return KVI_ARGS_RETCODE_STOP;
		}

		if(kvi_strEqualCI("-h",p) || kvi_strEqualCI("-help",p))
		{
			KviQString::appendFormatted(szMessage,"Usage:\n");
			KviQString::appendFormatted(szMessage,"  %s [options] [server [port]] [ircurl [ircurl [...]]]\n",a->argv[0]);
			KviQString::appendFormatted(szMessage," \n");
			KviQString::appendFormatted(szMessage,"Available options:\n");
			KviQString::appendFormatted(szMessage,"  -h, --help   : Print this help and exit\n");
			KviQString::appendFormatted(szMessage,"  -v, --version: Print version information and exit\n");
			KviQString::appendFormatted(szMessage,"  -c <file>    : Use <file> as config file instead of ~/%s\n",KVI_HOME_CONFIG_FILE_NAME);
			KviQString::appendFormatted(szMessage,"                 (defaults to $HOME/%s if <file> does not exist)\n",KVI_HOME_CONFIG_FILE_NAME);
			KviQString::appendFormatted(szMessage,"  -n <file>    : Use <file> as config file instead of $HOME/%s\n",KVI_HOME_CONFIG_FILE_NAME);
			KviQString::appendFormatted(szMessage,"                 (create <file> if it does not exist)\n");
#ifdef COMPILE_NO_IPC
			KviQString::appendFormatted(szMessage,"  -f           : Accepted but ignored (for compatibility)\n");
#else
			KviQString::appendFormatted(szMessage,"  -f           : Force a new KVIrc session, even if there is already\n");
			KviQString::appendFormatted(szMessage,"                 a running one.\n");
#endif
			KviQString::appendFormatted(szMessage,"  -e <commands>: If a KVIrc session is already running, execute\n");
			KviQString::appendFormatted(szMessage,"                 the <commands> in that session, otherwise start up\n");
			KviQString::appendFormatted(szMessage,"                 normally and execute <commands>\n");
			KviQString::appendFormatted(szMessage,"                 <commands> must be a single shell token.\n");
			KviQString::appendFormatted(szMessage,"                 You can eventually use this switch more than once\n");
			KviQString::appendFormatted(szMessage,"  -x <commands>: If a KVIrc session is already running, execute\n");
			KviQString::appendFormatted(szMessage,"                 the <commands> in that session, otherwise exit from application without doing anything/\n");
			KviQString::appendFormatted(szMessage,"                 <commands> must be a single shell token.\n");
			KviQString::appendFormatted(szMessage,"                 You can eventually use this switch more than once\n");
			KviQString::appendFormatted(szMessage,"  -r <commands>: If a KVIrc session is already running, execute the <commands>\n");
			KviQString::appendFormatted(szMessage,"                 in that session, otherwise start up normally (do not execute).\n");
			KviQString::appendFormatted(szMessage,"                 <commands> must be a single shell token.\n");
			KviQString::appendFormatted(szMessage,"                 You can eventually use this switch more than once\n");
			KviQString::appendFormatted(szMessage,"  -m           : If a KVIrc session is already running, show an informational\n");
			KviQString::appendFormatted(szMessage,"                 popup dialog instead of writing to the console\n");
			KviQString::appendFormatted(szMessage,"  --nosplash   : Do not show the splash screen at startup\n");
			KviQString::appendFormatted(szMessage,"  [server]     : Connect to this server after startup\n");
			KviQString::appendFormatted(szMessage,"  [port]       : Use this port for connection\n");
			KviQString::appendFormatted(szMessage,"  [ircurl]     : URL in the following form:\n");
			KviQString::appendFormatted(szMessage,"                 irc[6]://<server>[:<port>][/<channel>[?<pass>]]\n");

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
			MessageBox(0,szMessage.toLocal8Bit().data(),"KVIrc",0);
#else
			qDebug("%s",szMessage.toLocal8Bit().data());
#endif
			return KVI_ARGS_RETCODE_STOP;
		}

		if(kvi_strEqualCI("-c",p))
		{
			idx++;
			if(idx >= a->argc)
			{
				qDebug("Option -c requires a config file name");
				return KVI_ARGS_RETCODE_ERROR;
			}
			p = a->argv[idx];
			a->configFile = p;
			qDebug("Using file %s as config",p);
			continue;
		}

		if(kvi_strEqualCI("-e",p))
		{
			idx++;
			if(idx >= a->argc)
			{
				qDebug("Option -e requires a command");
				return KVI_ARGS_RETCODE_ERROR;
			}
			p = a->argv[idx];
			if(!a->szExecCommand.isEmpty())a->szExecCommand.append("\n");
			a->szExecCommand.append(p);
			continue;
		}

		if(kvi_strEqualCI("-x",p))
		{
			idx++;
			if(idx >= a->argc)
			{
				qDebug("Option -x requires a command");
				return KVI_ARGS_RETCODE_ERROR;
			}
			p = a->argv[idx];
			if(!a->szExecCommand.isEmpty())a->szExecCommand.append("\n");
			a->szExecCommand.append(p);
			a->bExecuteCommandAndClose=true;
			continue;
		}

		if(kvi_strEqualCI("-r",p))
		{
			idx++;
			if(idx >= a->argc)
			{
				qDebug("Option -r requires a command");
				return KVI_ARGS_RETCODE_ERROR;
			}
			p = a->argv[idx];
			if(!a->szExecRemoteCommand.isEmpty())
				a->szExecRemoteCommand.append("\n");
			a->szExecRemoteCommand.append(p);
			continue;
		}

		if(kvi_strEqualCI("-m",p))
		{
			a->bShowPopup = true;
			continue;
		}

		if(kvi_strEqualCI("-n",p))
		{
			idx++;
			if(idx >= a->argc)
			{
				qDebug("Option -n requires a config file name");
				return KVI_ARGS_RETCODE_ERROR;
			}
			p = a->argv[idx];
			a->configFile = p;
			a->createFile=true;
			qDebug("Using file %s as config",p);
			continue;
		}

		if(kvi_strEqualCI("-nosplash",p))
		{
			a->bShowSplashScreen = false;
			continue;
		}

		if(kvi_strEqualCI("-f",p))
		{
			a->bForceNewSession = true;
			continue;
		}

		if(kvi_strEqualCI("-session",p)||kvi_strEqualCI("-display",p)||kvi_strEqualCI("-name",p))
		{
			// Qt apps are supposed to handle the params to these switches, but we'll skip arg for now
			idx++;
			continue;
		}

		if(kvi_strEqualCI("-external",p))
		{
			idx++;
			if(idx >= a->argc)
			{
				qDebug("Option --external requires n irc:// url");
				return KVI_ARGS_RETCODE_ERROR;
			}
			p = a->argv[idx];
			if(kvi_strEqualCIN(p,"irc://",6) || kvi_strEqualCIN(p,"irc6://",7) || kvi_strEqualCIN(p,"ircs://",7) || kvi_strEqualCIN(p,"ircs6://",8))
			{
				a->szExecCommand = "openurl ";
				a->szExecCommand.append(QString::fromLocal8Bit(p).remove(QChar('$')).remove(QChar(';')).remove(QChar('%')));
				return KVI_ARGS_RETCODE_OK;
			}
			return KVI_ARGS_RETCODE_ERROR;
		}

		if(*p != '-')
		{
			// no dash
			if(kvi_strEqualCIN(p,"irc://",6) || kvi_strEqualCIN(p,"irc6://",7) || kvi_strEqualCIN(p,"ircs://",7) || kvi_strEqualCIN(p,"ircs6://",8))
			{
				if(!a->szExecCommand.isEmpty())
					a->szExecCommand.append('\n');

				a->szExecCommand = "openurl ";
				a->szExecCommand.append(QString::fromLocal8Bit(p).remove(QChar('$')).remove(QChar(';')).remove(QChar('%')));
			} else {
				QString szTmp = QString::fromLocal8Bit(p);
				bool bOk;
				szTmp.toUInt(&bOk);
				if(bOk)
				{
					szPort = szTmp;
				} else {
					QString ri = szTmp.right(4);
					if(KviQString::equalCI(ri,".kvs"))
					{
						if(!a->szExecCommand.isEmpty())
							a->szExecCommand.append('\n');
						a->szExecCommand.append("parse \"");
						szTmp.replace(QChar('$'), QString("\\$"));
						szTmp.replace(QChar('\\'), QString("\\\\"));
						a->szExecCommand.append(szTmp);
						a->szExecCommand.append('"');
					} else if(KviQString::equalCI(ri,".kvt"))
					{
						if(!a->szExecCommand.isEmpty())
							a->szExecCommand.append('\n');

						a->szExecCommand.append("theme.install \"");
						szTmp.replace(QChar('$'), QString("\\$"));
						szTmp.replace(QChar('\\'), QString("\\\\"));
						a->szExecCommand.append(szTmp);
						a->szExecCommand.append('"');
					} else if(KviQString::equalCI(ri,".kva"))
					{
						if(!a->szExecCommand.isEmpty())
							a->szExecCommand.append('\n');
						a->szExecCommand.append("addon.install \"");
						szTmp.replace(QChar('$'), QString("\\$"));
						szTmp.replace(QChar('\\'), QString("\\\\"));
						a->szExecCommand.append(szTmp);
						a->szExecCommand.append('"');
					} else {
						szServer = szTmp; // assume a plain server name
					}
				}
			}
		}
	}

	if(!szServer.isEmpty())
	{
		if(!a->szExecCommand.isEmpty())
			a->szExecCommand.append('\n');

		a->szExecCommand.append("server -u ");
		a->szExecCommand.append(szServer);
		if(!szPort.isEmpty())
		{
			a->szExecCommand.append(' ');
			a->szExecCommand.append(szPort);
		}
	}

	return KVI_ARGS_RETCODE_OK;
}

int main(int argc, char ** argv)
{
	ParseArgs a;
	a.argc = argc;
	a.argv = argv;
	a.configFile = 0;
	a.createFile = false;
	a.bForceNewSession = false;
	a.bShowPopup = false,
	a.bShowSplashScreen = true;
	a.bExecuteCommandAndClose = false;

	int iRetCode = parseArgs(&a);

	if(iRetCode != KVI_ARGS_RETCODE_OK)
	{
		return ((iRetCode == KVI_ARGS_RETCODE_ERROR) ? (-1) : 0);
	}

#ifdef COMPILE_KDE_SUPPORT
	KAboutData * pAboutData = new KAboutData( // FIXME: this is never deleted ? Should it be ?
			"kvirc", // internal program name
			"kvirc", // message catalogue name
			ki18n("KVIrc"), // user-visible program name
			KVI_VERSION, // program version
			ki18n("Visual IRC Client"), // description
			KAboutData::License_GPL, // license
			ki18n("(c) 1998-2010 The KVIrc Development Team"),
			ki18n("???"), // *some other text* ????
			"http://www.kvirc.net", // homepage
			"https://svn.kvirc.de/kvirc/" // bug address (FIXME: this would be an E-MAIL address...)
		);

	//fake argc/argv initialization: kde will use argv[0] as out appName in some dialogs
	// (eg: kdebase/workspace/kwin/killer/killer.cpp)
	KCmdLineArgs::init(1, &argv[0], pAboutData);
#endif

	KviApplication * pTheApp = new KviApplication(argc,argv);
#ifdef COMPILE_KDE_SUPPORT
	pTheApp->setAboutData(pAboutData);
#endif

#ifdef COMPILE_DBUS_SUPPORT
	#ifndef COMPILE_KDE_SUPPORT
		new KviDbusAdaptor(pTheApp); // FIXME: shouldn't this be deleted by someone ?
		QDBusConnection::sessionBus().registerObject("/MainApplication", pTheApp);
	#endif
#endif

	QString szRemoteCommand = a.szExecCommand;
	if(!a.szExecRemoteCommand.isEmpty())
	{
		if(!szRemoteCommand.isEmpty())
		{
			szRemoteCommand.append('\n');
		}
		szRemoteCommand.append(a.szExecRemoteCommand);
	}

	/*
		FIXME: There is a race condition in the IPC mechanism.
			If one starts two instances of kvirc one immediately after another
			then both instances may run through kvi_sendIpcMessage
			without finding the sentinel window and thus both may decide
			to start.
			A weak file locking mechanism should be used too...

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	QString szLock = convertSeparators(cleanDirPath(QDir::homePath() + "/.kvirc.lock"));
#else
	QString szLock = convertSeparators(cleanDirPath(QDir::homePath() + "/.kvirc.lock"));
#endif

	QFileInfo inf(szLock);
	bool bLocked = false;
	if(inf.exists())
	{
		iLocked = inf.lastModified().secsTo(QDateTime::currentDateTime());
	}
	*/

#ifndef COMPILE_NO_IPC
	if(!a.bForceNewSession)
	{
		// here we could use CreateMutex on win and semget() on linux
		// in order to get a shared semaphore to ensure instance unicity.

		if(kvi_sendIpcMessage(szRemoteCommand.toLocal8Bit().data()))
		{
			if(szRemoteCommand.isEmpty())
			{
				KviCString szTmp(KviCString::Format,"Another KVIrc session is already running on this display and with this user id.\nUse %s -f if you want to force a new session.",argv[0]);
				if(a.bShowPopup)
				{
					QMessageBox::information(0,"Session - KVIrc",szTmp.ptr(),QMessageBox::Ok);
				} else {
					qDebug("%s",szTmp.ptr());
				}
			}
			delete pTheApp;
			return 0;
		}

		if(a.bExecuteCommandAndClose)
		{
			delete pTheApp;
			return 0;
		}
	}
#endif

	pTheApp->m_bCreateConfig      = a.createFile;
	pTheApp->m_szConfigFile       = a.configFile;
	pTheApp->m_szExecAfterStartup = a.szExecCommand;
	pTheApp->m_bShowSplashScreen  = a.bShowSplashScreen;
	pTheApp->setup();

	// YEAH!
	int iRetVal = pTheApp->exec();
	// :)

	delete pTheApp;

	return iRetVal;
}
