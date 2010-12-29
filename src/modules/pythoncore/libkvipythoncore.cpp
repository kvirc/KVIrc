//=============================================================================
//
//   File : libkvipythoncore.cpp
//   Creation date : Fri Nov 07 00:18:31 2008 GMT by Elvio Basello
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)
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


#include "kvi_settings.h"
#include "KviModule.h"
#include "KviLocale.h"
//#include "KviModuleManager.h"
//#include "KviFileUtils.h"
//#include "KviApplication.h"
//#include "KviOptions.h"
//#include "kvi_out.h"

#ifdef COMPILE_PYTHON_SUPPORT
	#include "kvircmodule.h"
	#include "pythoncoreinterface.h"

	#include <Python.h>

KviKvsRunTimeContext * g_pCurrentKvsContext = 0;
bool g_bExecuteQuiet = false;
QStringList g_lWarningList;
QString g_lError;
static KviCString g_szLastReturnValue("");

static PyThreadState * mainThreadState = NULL;

class KviPythonInterpreter
{
public:
	KviPythonInterpreter(const QString & szContextName);
	~KviPythonInterpreter();
protected:
	QString           m_szContextName;
	PyThreadState   * m_pThreadState;
public:
	bool init(); // if this fails then well.. :D
	void done();
	bool execute(const QString & szCode, QStringList & lArgs, QString & szRetVal, QString & szError, QStringList & lWarnings);
	const QString & contextName() const { return m_szContextName; };
};

KviPythonInterpreter::KviPythonInterpreter(const QString & szContextName)
{
	m_szContextName = szContextName;
	m_pThreadState = 0;
}

KviPythonInterpreter::~KviPythonInterpreter()
{
	done();
}

bool KviPythonInterpreter::init()
{
// get the global lock
	PyEval_AcquireLock();
	// get a reference to the PyInterpreterState
	PyInterpreterState * mainInterpreterState = mainThreadState->interp;
	// create a thread state object for this thread
	m_pThreadState = PyThreadState_New(mainInterpreterState);
	// swap in the current thread state
	PyThreadState_Swap(m_pThreadState);
	// and hook in the kvirc error handling routines
	QString szPreCode = QString( \
		"import kvirc\n" \
		"import sys\n" \
		"class kvirc_stderr_grabber:\n" \
		"\tdef write(self,s):\n" \
		"\t\tkvirc.error(s)\n" \
		"sys.stderr=kvirc_stderr_grabber()\n"
	);
	// evaluate that
	PyRun_SimpleString(szPreCode.toUtf8().data());
	// swap out our thread state for now
	PyThreadState_Swap(NULL);

	// free the lock
	PyEval_ReleaseLock();
	return true;
}

void KviPythonInterpreter::done()
{
	if(!m_pThreadState)return;
	// grab the lock
	PyEval_AcquireLock();
	// swap my thread state out of the interpreter
	PyThreadState_Swap(NULL);
	// clear out any cruft from thread state object
	PyThreadState_Clear(m_pThreadState);
	// delete my thread state object
	PyThreadState_Delete(m_pThreadState);
	// release the lock
	PyEval_ReleaseLock();
	m_pThreadState = 0;
}

bool KviPythonInterpreter::execute(
		const QString &szCode,
		QStringList &lArgs, //args
		QString &szRetVal,
		QString &szError,
		QStringList &) //lWarnings
{
	if(!m_pThreadState)
	{
		szError = __tr2qs_ctx("Internal error: python interpreter not initialized","python");
		return false;
	}

	int retVal;
	g_lError.clear();

	// grab the global interpreter lock
	PyEval_AcquireLock();
	// swap in my thread state
	PyThreadState_Swap(m_pThreadState);

	QString szVarCode = "aArgs = [";

	bool bFirst = true;
	foreach(QString szArg,lArgs)
	{
		if(!bFirst)
			szVarCode += ",";
		else
			bFirst = false;

		szVarCode += QString::fromAscii("\"%1\"").arg(szArg);
	}

	szVarCode += "]";

	PyRun_SimpleString(szVarCode.toUtf8().data());

	//clean "cr" from the python code (ticket #1028)
	QString szCleanCode = szCode;
	szCleanCode.replace(QRegExp("\r\n?"), "\n");
	// execute some python code
	retVal = PyRun_SimpleString(szCleanCode.toUtf8().data());

	szRetVal.setNum(retVal);

	if (PyErr_Occurred() || retVal)
	{
		szError = g_lError;
	}

	// clear the thread state
	PyThreadState_Swap(NULL);
	// release our hold on the global interpreter
	PyEval_ReleaseLock();

	if(retVal)
		return false;
	return true;
}

	static KviPointerHashTable<QString,KviPythonInterpreter> * g_pInterpreters = 0;

static KviPythonInterpreter * pythoncore_get_interpreter(const QString &szContextName)
{
	KviPythonInterpreter * i = g_pInterpreters->find(szContextName);
	if(i)return i;
	i = new KviPythonInterpreter(szContextName);
	if(!i->init())
	{
		delete i;
		return 0;
	}
	g_pInterpreters->replace(szContextName,i);
	return i;
}

static void pythoncore_destroy_interpreter(const QString &szContextName)
{
	KviPythonInterpreter * i = g_pInterpreters->find(szContextName);
	if(!i)return;
	g_pInterpreters->remove(szContextName);
	i->done();
	delete i;
}

static void pythoncore_destroy_all_interpreters()
{
	KviPointerHashTableIterator<QString,KviPythonInterpreter> it(*g_pInterpreters);

	while(it.current())
	{
		KviPythonInterpreter * i = it.current();
		i->done();
		delete i;
		++it;
	}
	g_pInterpreters->clear();
}

#endif // COMPILE_PYTHON_SUPPORT

static bool pythoncore_module_ctrl(KviModule *,const char * cmd,void * param)
{
#ifdef COMPILE_PYTHON_SUPPORT
	if(kvi_strEqualCS(cmd,KVI_PYTHONCORECTRLCOMMAND_EXECUTE))
	{
		KviPythonCoreCtrlCommand_execute * ex = (KviPythonCoreCtrlCommand_execute *)param;
		if(ex->uSize != sizeof(KviPythonCoreCtrlCommand_execute))return false;
		g_pCurrentKvsContext = ex->pKvsContext;
		g_bExecuteQuiet = ex->bQuiet;
		if(ex->szContext.isEmpty())
		{
			KviPythonInterpreter * m = new KviPythonInterpreter("temporary");
			if(!m->init())
			{
				delete m;
				return false;
			}
			ex->bExitOk = m->execute(ex->szCode,ex->lArgs,ex->szRetVal,ex->szError,ex->lWarnings);
			m->done();
			delete m;
		} else {
			KviPythonInterpreter * m = pythoncore_get_interpreter(ex->szContext);
			ex->bExitOk = m->execute(ex->szCode,ex->lArgs,ex->szRetVal,ex->szError,ex->lWarnings);
		}
		return true;
	}
	if(kvi_strEqualCS(cmd,KVI_PYTHONCORECTRLCOMMAND_DESTROY))
	{
		KviPythonCoreCtrlCommand_destroy * de = (KviPythonCoreCtrlCommand_destroy *)param;
		if(de->uSize != sizeof(KviPythonCoreCtrlCommand_destroy))return false;
		pythoncore_destroy_interpreter(de->szContext);
		return true;
	}
#endif // COMPILE_PYTHON_SUPPORT
	return false;
}

static bool pythoncore_module_init(KviModule *)
{
#ifdef COMPILE_PYTHON_SUPPORT

	// Initialize the Python interpreter
	Py_Initialize();
	PyEval_InitThreads();

	// save a pointer to the main PyThreadState object
	mainThreadState = PyThreadState_Get();
	// release the lock
	PyEval_ReleaseLock();

	// Initialize the Python module for KVIrc
	python_init();

	g_pInterpreters = new KviPointerHashTable<QString,KviPythonInterpreter>(17,false);
	g_pInterpreters->setAutoDelete(false);
	return true;

#endif // COMPILE_PYTHON_SUPPORT

	return false;
}

static bool pythoncore_module_cleanup(KviModule *)
{
#ifdef COMPILE_PYTHON_SUPPORT
	pythoncore_destroy_all_interpreters();
	delete g_pInterpreters;
	g_pInterpreters = 0;

	// shut down the interpreter
	PyEval_AcquireLock();
	PyThreadState_Swap(mainThreadState);
	PyEval_ReleaseLock();
	//FIXME currently python hangs forever on Py_Finalize()
	//under win32/mingw; this is a (bad) workaround
#ifndef COMPILE_ON_MINGW
	Py_Finalize();
#endif //COMPILE_ON_MINGW

#endif // COMPILE_PYTHON_SUPPORT

	return true;
}

static bool pythoncore_module_can_unload(KviModule *)
{
#ifdef COMPILE_PYTHON_SUPPORT
	return (g_pInterpreters->count() == 0);
#endif // COMPILE_PYTHON_SUPPORT
	return true;
}

KVIRC_MODULE(
	"PythonCore",                                                 // module name
	"4.0.0",                                                // module version
	"Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)\n" \
	"Copyright (C) 2009 Fabio Bas (ctrlaltca at libero dot it)",
	"Python Scripting Engine Core",
	pythoncore_module_init,
	pythoncore_module_can_unload,
	pythoncore_module_ctrl,
	pythoncore_module_cleanup,
	"python"
)
