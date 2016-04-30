//=============================================================================
//
//   File : KvsObject_process.cpp
//   Creation date : Fri Mar 18 21:30:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005-2008 Alessandro Carbone (elfonol at gmail dot com)
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

#include "KvsObject_process.h"
#include "KviError.h"
#include "kvi_debug.h"
#include "kvi_settings.h"
#include "KviLocale.h"
#include "QProcess"

/*
	@doc:	process
	@keyterms:
		process object class, process
	@title:
		process class
	@type:
		class
	@short:
		A class to manage process.
	@inherits:
		[class]object[/class]
	@description:
		The Process class  is used to start external programs and to communicate with them.[br]
		[b]WARNING! at last you have to close the process![/b]
	@functions:
		!fn: $addArgument(<process-name:string>)
		With this command you give the process name (or more arguments) for communication.
		See the next example.
		!fn: $startProcess()
		Tries to run the process.[br]
		[b]e.g.[/b]
		[example]
			%process=$new(process);[br]
			%process->$addArg("cmd.exe");[br]
			%process->$start();[br]
		[/example]
		!fn: <string> $readStdout()
		Reads the data that the process has written to standard output.
		!fn: <string> $readStderr()
		Reads the data that the process has written to standard error.
		[b]e.g.[/b]
		[example]
			class (test,object)
			{
				slotReadStdout()
				{
					%stdo = %Process->$readStdout()
					#%Aoutput->$append(%stdo);[comment]# coming soon in the new texteditor class[/comment]
					%Aoutput->$settext(%stdo);
				}
				slotReadStderr()
				{
					%stderr= %Process->$readStderr()
					#%Aoutput->$append(%stderr);[comment]# coming soon in the new texteditor class[/comment]
					%Aoutput->$settext(%stderr);
				}
			}

			%tt=$new(test)
			%A=$new(widget)
			%A->$setGeometry(100,100,400,300)
			%layoutA=$new(layout,%A)
			%Ainput=$new(lineedit,%A)
			#%Aoutput=$new(textedit,%A)[comment]# coming soon in the new texteditor class[/commnet]
			%Aoutput=$new(label,%A)
			%bclosekill=$new(button,%A)
			%bclosekill->$settext("&CloseKill ")
			%bkill=$new(button,%A)
			%bkill->$settext("&Kill ")
			%bterminate=$new(button,%A)
			%bterminate->$settext("&Ask to Terminate ")
			%layoutA->$addwidget(%Ainput,0,0)
			%layoutA->$addwidget(%Aoutput,1,0)
			%layoutA->$addwidget(%bclosekill,3,0)
			%layoutA->$addwidget(%bkill,4,0,)
			%layoutA->$addwidget(%bterminate,5,0)

			%Process=$new(process)
			%Process->$addArgument("cmd.exe")
			%Process->$startProcess();

			connect %Process readyReadStdout %tt slotReadStdout
			connect %Process readyReadStderr %tt slotReadStderr
			privateimpl(%Ainput,returnPressedEvent)
			{
				%command=%Ainput->$text() "\r\n"
				%Process->$writeToStdin(%command);
				%Ainput->$setText("");
			}

			privateimpl(%bclosekill,mousepressevent)
			{
				%Process->$closekill();
				delete %A;
			}
			privateimpl(%bkill,mousepressevent)
			{
				%Process->$kill();
				delete %A;
			}
			privateimpl(%bterminate,mousepressevent)
			{
				%Process->$tryTerminate();
				delete %A;
			}
			%A->$show();
		[/example]
		!fn: $writeToStdin(<command:string>)
		With this command you send a command to the process:
		!fn: $closekill()
		This tries to terminate the process the nice way.[br]
		If the process is still running after 5 seconds, it terminates the process the hard way.[br]
		(I think that this is the better way.)[br]
		[b]e.g.[/b]
		[pre]
			%Process->close_kill();
		[/pre]
		!fn: $kill()
		Kill the process the hard way (Bad Idea).
		!fn: $tryTerminate()
		Tries to terminate the process (It could end well, but...).
		!fn: $closeStdin()
		Close the standard input.
		!fn: <boolean> $isRunning()
		Return 1 if the process is running, else return 0.
		!fn: <boolean> $normalExit()
		Returns true if the process has exited normally; otherwise returns false.
		!fn: $readyReadStdoutEvent()
		This function is invoked by the process when there is new data.[br]
		The default implementation emits the [classfnc]$readyReadStdout[/classfnc]() signal.
		!fn: $readyReadStderrEvent()
		This function is invoked by the process when there are new error messages.[br]
		The default implementation emits the [classfnc]$readyReadStderr[/classfnc]() signal.
	@signals:
		!sg: $readyReadStdout()
		This signal is emitted by the default implementation of [classfnc]$readyReadStdoutEvent[/classfnc]().[br]
		If you reimplement that function you will have to emit the signal manually (if you still need it).
		!sg: $readyReadStderr()
		This signal is emitted by the default implementation of [classfnc]$readyReadStderrEvent[/classfnc]().
*/

KVSO_BEGIN_REGISTERCLASS(KvsObject_process, "process", "object")

KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_process, addArgument);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_process, startProcess);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_process, readStdout);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_process, readStderr);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_process, writeToStdin);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_process, closekill);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_process, kill);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_process, tryTerminate);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_process, closeStdin);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_process, isRunning);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_process, normalExit);

// Events
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_process, readyReadStdoutEvent);
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_process, readyReadStderrEvent);

KVSO_END_REGISTERCLASS(KvsObject_process)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_process, KviKvsObject)

m_pProcess = new QProcess();
connect(m_pProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(slotReadStdout()));
connect(m_pProcess, SIGNAL(readyReadStandardError()), this, SLOT(slotReadStderr()));

KVSO_END_CONSTRUCTOR(KvsObject_process)

KVSO_BEGIN_DESTRUCTOR(KvsObject_process)

delete m_pProcess;

KVSO_END_CONSTRUCTOR(KvsObject_process)

KVSO_CLASS_FUNCTION(process, addArgument)
{

	QString szArgument;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("argument", KVS_PT_NONEMPTYSTRING, 0, szArgument)
	KVSO_PARAMETERS_END(c)
	m_szArgs.append(szArgument);
	return true;
}

//->Start the process.
KVSO_CLASS_FUNCTION(process, startProcess)
{
	CHECK_INTERNAL_POINTER(m_pProcess)
	QString szcmd;

	szcmd = m_szArgs.takeFirst();
	m_pProcess->start(szcmd, m_szArgs);

	if(m_pProcess->state() == QProcess::NotRunning)
	{
		c->warning(__tr2qs_ctx("Process could not be started.", "objects"));
	}
	return true;
}
//-->Read the standard output.
KVSO_CLASS_FUNCTION(process, readStderr)
{
	CHECK_INTERNAL_POINTER(m_pProcess)
	QString ng_Process = m_pProcess->readAllStandardError();
	c->returnValue()->setString(ng_Process);
	return true;
}

//-->Read the standard error.
KVSO_CLASS_FUNCTION(process, readStdout)
{
	CHECK_INTERNAL_POINTER(m_pProcess)
	QString ng_Process = m_pProcess->readAllStandardOutput();
	c->returnValue()->setString(ng_Process);
	return true;
}
//-->Signals and slot to manage reading output and error from the process.
KVSO_CLASS_FUNCTION(process, readyReadStdoutEvent)
{
	emitSignal("readyReadStdout", c);
	return true;
}

KVSO_CLASS_FUNCTION(process, readyReadStderrEvent)
{
	emitSignal("readyReadStderr", c);
	return true;
}
KVSO_CLASS_FUNCTION(process, writeToStdin)
{
	CHECK_INTERNAL_POINTER(m_pProcess)
	QString szCommand;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("command", KVS_PT_STRING, 0, szCommand)
	KVSO_PARAMETERS_END(c)
	if(m_pProcess)
		m_pProcess->write(szCommand.toUtf8().data());
	return true;
}
//-->The 3 Closing process functions
KVSO_CLASS_FUNCTION(process, closekill)
{
	CHECK_INTERNAL_POINTER(m_pProcess)
	//I try to  to terminate the process the nice way....
	m_pProcess->terminate();
	//If the process is still running after 5 seconds, I'll terminate the process in the hard way.
	QTimer::singleShot(5000, m_pProcess, SLOT(kill()));
	return true;
}
KVSO_CLASS_FUNCTION(process, kill)
{
	CHECK_INTERNAL_POINTER(m_pProcess)
	m_pProcess->kill();
	return true;
}
KVSO_CLASS_FUNCTION(process, tryTerminate)
{
	CHECK_INTERNAL_POINTER(m_pProcess)
	m_pProcess->terminate();
	return true;
}
//-->Close the standard input.
KVSO_CLASS_FUNCTION(process, closeStdin)
{
	CHECK_INTERNAL_POINTER(m_pProcess)
	m_pProcess->closeReadChannel(QProcess::StandardOutput);
	return true;
}
//->Returns if the process still runnig
KVSO_CLASS_FUNCTION(process, isRunning)
{
	CHECK_INTERNAL_POINTER(m_pProcess)
	c->returnValue()->setBoolean(m_pProcess->state() == QProcess::Running);
	return true;
}
//->Returns if the process exited.
KVSO_CLASS_FUNCTION(process, normalExit)
{
	CHECK_INTERNAL_POINTER(m_pProcess)
	c->returnValue()->setBoolean(m_pProcess->state() == QProcess::NotRunning && m_pProcess->exitStatus() == QProcess::NormalExit);
	return true;
}

void KvsObject_process::slotReadStdout()
{
	KviKvsVariantList * params = nullptr;
	callFunction(this, "readyReadStdoutEvent", params);
}

void KvsObject_process::slotReadStderr()
{
	KviKvsVariantList * params = nullptr;
	callFunction(this, "readyReadStderrEvent", params);
}
