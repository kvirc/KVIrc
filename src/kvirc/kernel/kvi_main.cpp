//=============================================================================
//
//   File : kvi_main.cpp
//   Creation date : Sun Jun 18 2000 12:38:45 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2000 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_app.h"
#include "kvi_string.h"
#include "kvi_settings.h"
#include "kvi_ircurl.h"
#include "kvi_defaults.h"
#include "kvi_sourcesdate.h"
#include "kvi_msgbox.h"

#ifndef COMPILE_NO_IPC
	extern bool kvi_sendIpcMessage(const char * message); // kvi_ipc.cpp
#endif

#include <qglobal.h> //for debug()

#include <QMessageBox>

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
	KviStr    szExecCommand;
	KviStr    szExecRemoteCommand;
} ParseArgs;

int parseArgs(ParseArgs * a)
{
	KviStr szServer;
	KviStr szPort;
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
			KviQString::appendFormatted(szMessage,"Build date: %s\n",KVI_BUILD_DATE);
			KviQString::appendFormatted(szMessage,"Home page: http://www.kvirc.net/\n");

#ifdef COMPILE_ON_WINDOWS
			MessageBox(0,szMessage.local8Bit().data(),"KVIrc",0);
#else
			debug(szMessage);
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
			KviQString::appendFormatted(szMessage,"                 popup dialog instead of writing to the console");
			KviQString::appendFormatted(szMessage,"  --nosplash   : Do not show the splash screen at startup\n");
			KviQString::appendFormatted(szMessage,"  [server]     : Connect to this server after startup\n");
			KviQString::appendFormatted(szMessage,"  [port]       : Use this port for connection\n");
			KviQString::appendFormatted(szMessage,"  [ircurl]     : URL in the following form:\n");
			KviQString::appendFormatted(szMessage,"                 irc[6]://<server>[:<port>][/<channel>[?<pass>]]\n");

#ifdef COMPILE_ON_WINDOWS
			MessageBox(0,szMessage.local8Bit().data(),"KVIrc",0);
#else
			debug(szMessage);
#endif
			return KVI_ARGS_RETCODE_STOP;
		}

		if(kvi_strEqualCI("-c",p))
		{
			idx++;
			if(idx >= a->argc)
			{
				debug("Option -c requires a config file name");
				return KVI_ARGS_RETCODE_ERROR;
			}
			p = a->argv[idx];
			a->configFile = p;
			debug("Using file %s as config",p);
			continue;
		}

		if(kvi_strEqualCI("-e",p))
		{
			idx++;
			if(idx >= a->argc)
			{
				debug("Option -e requires a command");
				return KVI_ARGS_RETCODE_ERROR;
			}
			p = a->argv[idx];
			if(a->szExecCommand.hasData())a->szExecCommand.append("\n");
			a->szExecCommand.append(p);
			continue;
		}
		
		if(kvi_strEqualCI("-x",p))
		{
			idx++;
			if(idx >= a->argc)
			{
				debug("Option -x requires a command");
				return KVI_ARGS_RETCODE_ERROR;
			}
			p = a->argv[idx];
			if(a->szExecCommand.hasData())a->szExecCommand.append("\n");
			a->szExecCommand.append(p);
			a->bExecuteCommandAndClose=true;
			continue;
		}
		
		if(kvi_strEqualCI("-r",p))
		{
			idx++;
			if(idx >= a->argc)
			{
				debug("Option -r requires a command");
				return KVI_ARGS_RETCODE_ERROR;
			}
			p = a->argv[idx];
			if(a->szExecRemoteCommand.hasData())a->szExecRemoteCommand.append("\n");
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
				debug("Option -n requires a config file name");
				return KVI_ARGS_RETCODE_ERROR;
			}
			p = a->argv[idx];
			a->configFile = p;
			a->createFile=true;
			debug("Using file %s as config",p);
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
		
		if(kvi_strEqualCI("-session",p)||kvi_strEqualCI("-display",p))
		{
			// Qt apps are supposed to handle the params to these switches, but we'll skip arg for now
			idx++;
			continue;
		}

		if(*p != '-')
		{
			// no dash
			if(kvi_strEqualCIN(p,"irc://",6) || kvi_strEqualCIN(p,"irc6://",7) || kvi_strEqualCIN(p,"ircs://",7) || kvi_strEqualCIN(p,"ircs6://",8))
			{
				KviStr tmp = QString::fromLocal8Bit(p);
				if(a->szExecCommand.hasData())a->szExecCommand.append('\n');
				a->szExecCommand.append("openurl ");
				tmp.replaceAll("$",""); // the urls can't contain $ signs
				tmp.replaceAll(";",""); // the urls can't contain ; signs
				a->szExecCommand.append(tmp);
			} else {
				QString tmp = QString::fromLocal8Bit(p);
				bool bOk;
				tmp.toUInt(&bOk);
				if(bOk)szPort = tmp;
				else {
					QString ri = tmp.right(4);
					if(KviQString::equalCI(ri,".kvs"))
					{
						if(a->szExecCommand.hasData())a->szExecCommand.append('\n');
						a->szExecCommand.append("parse \"");
						tmp.replace('$',"\\$");
						tmp.replace('\\',"\\\\");
						a->szExecCommand.append(tmp);
						a->szExecCommand.append('"');
					} else if(KviQString::equalCI(ri,".kvt"))
					{
						if(a->szExecCommand.hasData())a->szExecCommand.append('\n');
						a->szExecCommand.append("theme.install \"");
						tmp.replace('$',"\\$");
						tmp.replace('\\',"\\\\");
						a->szExecCommand.append(tmp);
						a->szExecCommand.append('"');
					} else
						szServer = tmp; // assume a plain server name
				}
			}
		}
	}

	if(szServer.hasData())
	{
		if(a->szExecCommand.hasData())a->szExecCommand.append('\n');
		a->szExecCommand.append("server -u ");
		a->szExecCommand.append(szServer);
		if(szPort.hasData())
		{
			a->szExecCommand.append(' ');
			a->szExecCommand.append(szPort);
		}
	}

	return KVI_ARGS_RETCODE_OK;
}

int main(int argc,char ** argv)
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

	int retCode = parseArgs(&a);

	if(retCode != KVI_ARGS_RETCODE_OK)return ((retCode == KVI_ARGS_RETCODE_ERROR) ? (-1) : 0);

	// Need to have the X socket open before IPC startup
	KviApp * theApp = new KviApp(argc,argv);

	KviStr szRemoteCommand = a.szExecCommand;
	if(a.szExecRemoteCommand.hasData())
	{
		if(szRemoteCommand.hasData())szRemoteCommand.append('\n');
		szRemoteCommand.append(a.szExecRemoteCommand);
	}

	/*
		FIXME: There is a race condition in the IPC mechanism.
			If one starts two instances of kvirc one immediately after another
			then both instances may run through kvi_sendIpcMessage 
			without finding the sentinel window and thus both may decide
			to start.
			A weak file locking mechanism should be used too...
		
#ifdef COMPILE_ON_WINDOWS
	QString szLock = convertSeparators(cleanDirPath(QDir::homeDirPath() + "/.kvirc.lock"));
#else
	QString szLock = convertSeparators(cleanDirPath(QDir::homeDirPath() + "/.kvirc.lock"));
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
	
		if(kvi_sendIpcMessage(szRemoteCommand.ptr()))
		{
			if(szRemoteCommand.isEmpty())
			{
				KviStr tmp(KviStr::Format,"Another KVIrc session is already running on this display and with this user id.\nUse %s -f if you want to force a new session.",argv[0]);
				if(a.bShowPopup)
					QMessageBox::information(0,"Session - KVIrc",tmp.ptr(),QMessageBox::Ok);
				else
					debug(tmp.ptr());
			}
			delete theApp;
			return 0;
		} else if(a.bExecuteCommandAndClose) {
			delete theApp;
			return 0;
		}
	}
#endif

	theApp->m_bCreateConfig      = a.createFile;
	theApp->m_szConfigFile       = a.configFile;
	theApp->m_szExecAfterStartup = a.szExecCommand;
	theApp->m_bShowSplashScreen  = a.bShowSplashScreen;
	theApp->setup();

	// YEAH!
	int retVal = theApp->exec();
	// :)

	delete theApp;
	theApp = 0;
	return retVal;
}
