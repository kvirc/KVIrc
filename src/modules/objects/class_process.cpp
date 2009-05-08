//=============================================================================
//
//   File : class_process.cpp
//   Creation date : Fri Mar 18 21:30:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2005-2008 Alessandro Carbone (elfonol at gmail dot com)
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

#include "class_process.h"
#include "kvi_error.h"
#include "kvi_debug.h"
#include "kvi_settings.h"
#include "kvi_locale.h"
#include "kvi_process.h"

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
		!WARNING AT LAST YOU HAVE TO CLOSE THE PROCESS!
	@functions:
		!fn: $addArg(<process-name:string>)
		With this command you give the process name (or more arguments) for comunication.
		Es: see the next example.
		!fn: $start()
		Tries to run the process.[br]
		Es: [br]
		%process=$new(process);[br]
		%process->$addArg("cmd.exe");[br]
		%process->$start();[br]
		!fn: <string> $readStdout()
		Reads the data that the process has written to standard output.
		!fn: <string> $readStderr()
		Reads the data that the process has written to standard error.
		Es:[br]
		-------------------Start:[br]
		class (test,object)[br]
		{[br]
			slotReadStdout()[br]
			{[br]
	 			%stdo = %Process->$readStdout()[br]
				#%Aoutput->$append(%stdo);// coming soon in the new texteditor class[br]
		 		%Aoutput->$settext(%stdo);[br]
	 		}[br]
			slotReadStderr()[br]
			{[br]
	 			%stderr= %Process->$readStderr()[br]
		 		#%Aoutput->$append(%stderr);// coming soon in the new texteditor class[br]
				%Aoutput->$settext(%stderr);[br]
	 		}[br]
		}[br]

		%tt=$new(test)[br]
		%A=$new(widget)[br]
		%A->$setGeometry(100,100,400,300)[br]
		%layoutA=$new(layout,%A)[br]
		%Ainput=$new(lineedit,%A)[br]
		#%Aoutput=$new(textedit,%A)// coming soon in the new texteditor class[br]
		%Aoutput=$new(label,%A)[br]
		%bclosekill=$new(button,%A)[br]
		%bclosekill->$settext("&CloseKill ")[br]
		%bkill=$new(button,%A)[br]
		%bkill->$settext("&Kill ")[br]
		%bterminate=$new(button,%A)[br]
		%bterminate->$settext("&Ask to Terminate ")[br]
		%layoutA->$addwidget(%Ainput,0,0)[br]
		%layoutA->$addwidget(%Aoutput,1,0)[br]
		%layoutA->$addwidget(%bclosekill,3,0)[br]
		%layoutA->$addwidget(%bkill,4,0,)[br]
		%layoutA->$addwidget(%bterminate,5,0)[br]

		%Process=$new(process)[br]
		%Process->$addArg("cmd.exe")[br]
		%Process->$start();[br]

		connect %Process readyReadStdout %tt slotReadStdout[br]
		connect %Process readyReadStderr %tt slotReadStderr[br]
		privateimpl(%Ainput,returnPressedEvent)[br]
		{
		%command=%Ainput->$text() "\r\n"[br]
		%Process->$writeToStdin(%command);[br]
		%Ainput->$setText("");[br]
		}[br]

		privateimpl(%bclosekill,mousepressevent)[br]
		{[br]
			%Process->$closekill();[br]
			delete %A;[br]
		}[br]
		privateimpl(%bkill,mousepressevent)[br]
		{[br]
			%Process->$kill();[br]
			delete %A;[br]
		}[br]
		privateimpl(%bterminate,mousepressevent)[br]
		{[br]
			%Process->$tryTerminate();[br]
			delete %A;[br]
		}[br]
		%A->$show();[br]
		--------------------End.[br]
		!fn: $writeToStdin(<command:string>)
		Whit this command you send a command to the process:
		!fn: $closekill()
		This tries to terminate the process the nice way.[br]
		If the process is still running after 5 seconds, it terminates the process the hard way.[br]
		(I think that this is the better way.)[br]
		Es:
		%Process->close_kill();
		!fn: $kill()
		Kill the process in hard way.(Bad Idea)
		!fn: $tryTerminate()
		Tries to terminate the process.(It could be well but...)
		!fn: $closeStdin()
		Close the standard Input.
		!fn: <boolean> $isRunning()
		Return 1 if the process is running, else return 0.
		!fn: <boolean> $normalExit()
		Returns TRUE if the process has exited normally; otherwise returns FALSE.
		!fn: $readyReadStdoutEvent()
		This function is invoched by the process when there are new datas.[br]
		The default implementation emits the [classfnc]$readyReadStdout[/classfnc]() signal.
		!fn: $readyReadStderrEvent()
		This function is invoched by the process when there are new error messages.[br]
		The default implementation emits the [classfnc]$readyReadStderr[/classfnc]() signal.

	@signals:
		!sg: $readyReadStdout()
		This signal is emitted by the default implementation of [classfnc]$readyReadStdoutEvent[/classfnc]().[br]
		If you reimplement that function you will have to emit the signal manually (if you still need it).
		!sg: $readyReadStderr()
		This signal is emitted by the default implementation of [classfnc]$readyReadStderrEvent[/classfnc]().
*/

KVSO_BEGIN_REGISTERCLASS(KviKvsObject_process,"process","object")

	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_process,addArgument);
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_process,startProcess);
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_process,readStdout);
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_process,readStderr);
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_process,writeToStdin);
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_process,closekill);
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_process,kill);
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_process,tryTerminate);
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_process,closeStdin);
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_process,isRunning);
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_process,normalExit);

  // Events
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_process,readyReadStdoutEvent);
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_process,readyReadStderrEvent);

  KVSO_END_REGISTERCLASS(KviKvsObject_process)


KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_process,KviKvsObject)

	m_pProcess = new KviProcess();
	connect(m_pProcess,SIGNAL(readyReadStandardOutput()),this,SLOT(slotReadStdout()));
	connect(m_pProcess,SIGNAL(readyReadStandardError()),this,SLOT(slotReadStderr()));

KVSO_END_CONSTRUCTOR(KviKvsObject_process)

KVSO_BEGIN_DESTRUCTOR(KviKvsObject_process)

	delete m_pProcess;

KVSO_END_CONSTRUCTOR(KviKvsObject_process)



KVSO_CLASS_FUNCTION(process,addArgument)
{

	QString szArgument;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("argument",KVS_PT_NONEMPTYSTRING,0,szArgument)
	KVSO_PARAMETERS_END(c)
	args.append(szArgument);
	return true;
}

//->Start the process.
KVSO_CLASS_FUNCTION(process,startProcess)
{
	CHECK_INTERNAL_POINTER(m_pProcess)
	QString szcmd;

	szcmd = args.takeFirst();
	m_pProcess->start(szcmd, args);

	if(m_pProcess->state()==KviProcess::NotRunning)
	{
		c->warning( __tr2qs_ctx("Process could not be started.","objects"));
	}
	return true;
}
//-->Read the standard output.
KVSO_CLASS_FUNCTION(process,readStderr)
{
	CHECK_INTERNAL_POINTER(m_pProcess)
	QString ng_Process = m_pProcess->readAllStandardError();
	c->returnValue()->setString(ng_Process);
	return true;
}

//-->Read the standard error.
KVSO_CLASS_FUNCTION(process,readStdout)
{
	CHECK_INTERNAL_POINTER(m_pProcess)
	QString ng_Process =m_pProcess->readAllStandardOutput();
	c->returnValue()->setString(ng_Process);
	return true;
}
//-->Signals and slot to manage reading output and error from the process.
KVSO_CLASS_FUNCTION(process,readyReadStdoutEvent)
{
	emitSignal("readyReadStdout",c);
	return true;
}

KVSO_CLASS_FUNCTION(process,readyReadStderrEvent)
{
	emitSignal("readyReadStderr",c);
	return true;
}
KVSO_CLASS_FUNCTION(process,writeToStdin)
{
	CHECK_INTERNAL_POINTER(m_pProcess)
	QString szCommand;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("command",KVS_PT_STRING,0,szCommand)
	KVSO_PARAMETERS_END(c)
	if (m_pProcess) m_pProcess->write(szCommand.toUtf8().data());
	return true;
}
//-->The 3 Closing process functions
KVSO_CLASS_FUNCTION(process,closekill)
{
	CHECK_INTERNAL_POINTER(m_pProcess)
	//I try to  to terminate the process the nice way....
	m_pProcess->terminate();
	//If the process is still running after 5 seconds, I'll terminate the process in the hard way.
	QTimer::singleShot( 5000, m_pProcess, SLOT( kill() ) );
	return true;
}
KVSO_CLASS_FUNCTION(process,kill)
{
	CHECK_INTERNAL_POINTER(m_pProcess)
	m_pProcess->kill();
	return true;
}
KVSO_CLASS_FUNCTION(process,tryTerminate)
{
	CHECK_INTERNAL_POINTER(m_pProcess)
	m_pProcess->terminate();
	return true;
}
//-->Close the standard input.
KVSO_CLASS_FUNCTION(process,closeStdin)
{
	CHECK_INTERNAL_POINTER(m_pProcess)
	m_pProcess->closeReadChannel(KviProcess::StandardOutput);
	return true;
}
//->Returns if the process still runnig
KVSO_CLASS_FUNCTION(process,isRunning)
{
	CHECK_INTERNAL_POINTER(m_pProcess)
	c->returnValue()->setBoolean(m_pProcess->state()==KviProcess::Running);
	return true;
}
//->Returns if the process exited.
KVSO_CLASS_FUNCTION(process,normalExit)
{
	CHECK_INTERNAL_POINTER(m_pProcess)
	c->returnValue()->setBoolean(m_pProcess->state()==KviProcess::NotRunning && m_pProcess->exitStatus()==KviProcess::NormalExit);
	return true;
}

void KviKvsObject_process::slotReadStdout()
{
	KviKvsVariantList *params=0;
	callFunction(this,"readyReadStdoutEvent",params);
}

void KviKvsObject_process::slotReadStderr()
{
	KviKvsVariantList *params=0;
	callFunction(this,"readyReadStderrEvent",params);
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_class_process.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES

