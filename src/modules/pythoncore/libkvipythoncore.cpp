//=============================================================================
//
//   File : libkvipythoncore.cpp
//   Creation date : Fri Nov 07 00:18:31 2008 GMT by Elvio Basello
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)
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

#include <functional>
#include <memory>
#include <unordered_map>
#include <utility>

#include "kvi_settings.h"
#include "KviModule.h"
#include "KviLocale.h"

#ifdef COMPILE_PYTHON_SUPPORT
#include "pythoncoreinterface.h"

#include "pythonheaderwrapper.h"

KviKvsRunTimeContext * g_pCurrentKvsContext = nullptr;
bool g_bExecuteQuiet = false;
QStringList g_lWarningList;
QString g_lError;

static PyThreadState * mainThreadState = nullptr;

struct KviPythonLock
{
	KviPythonLock(PyThreadState * ts)
	{
		PyEval_RestoreThread(ts);
	}
	~KviPythonLock()
	{
		PyEval_SaveThread();
	}
};

struct KviPythonInterpreterDeleter
{
	void operator()(PyThreadState * ts) const
	{
		KviPythonLock lock{ ts };
		Py_EndInterpreter(ts);
		PyThreadState_Swap(mainThreadState);
	}
};

struct KviPythonInterpreter
{
	KviPythonInterpreter();
	bool execute(QString, QStringList &, QString &, QString &, QStringList &);
	std::unique_ptr<PyThreadState, KviPythonInterpreterDeleter> m_uptrThreadState;
};

struct KviCaseInsensitiveQStringHash
{
	std::size_t operator()(const QString & s) const
	{
		return static_cast<std::size_t>(qHash(s.toLower()));
	}
};

struct KviCaseInsensitiveQStringEqual
{
	bool operator()(const QString & s, const QString & t) const
	{
		return (s.toLower() == t.toLower());
	}
};

static std::unordered_map<QString, KviPythonInterpreter, KviCaseInsensitiveQStringHash, KviCaseInsensitiveQStringEqual> g_Interpreters;

KviPythonInterpreter::KviPythonInterpreter()
{
	KviPythonLock lock{ mainThreadState };
	m_uptrThreadState.reset(Py_NewInterpreter());

	// hook in the kvirc error handling routines
	QString szPreCode = QString(
	    "import kvirc\n"
	    "import sys\n"
	    "class kvirc_stderr_grabber:\n\tdef write(self,s):\n\t\tkvirc.error(s)\n"
	    "sys.stderr=kvirc_stderr_grabber()\n");

	PyRun_SimpleString(szPreCode.toUtf8().data());
}

bool KviPythonInterpreter::execute(QString szCode, QStringList & lArgs,
    QString & szRetVal, QString & szError, QStringList &)
{
	if(!m_uptrThreadState)
	{
		szError = __tr2qs_ctx("Internal error: Python interpreter not initialized", "python");
		return false;
	}

	g_lError.clear();

	KviPythonLock lock{ m_uptrThreadState.get() };

	QString szVarCode = "aArgs = [";

	bool bFirst = true;
	foreach(QString szArg, lArgs)
	{
		if(!bFirst)
			szVarCode += ",";
		else
			bFirst = false;

		szVarCode += QString::fromLatin1("\"%1\"").arg(szArg);
	}

	szVarCode += "]";

	PyRun_SimpleString(szVarCode.toUtf8().data());

	// clean "cr" from the python code (ticket #1028)
	szCode.replace(QRegExp("\r\n?"), "\n");

	int retVal = PyRun_SimpleString(szCode.toUtf8().data());

	szRetVal.setNum(retVal);

	if(PyErr_Occurred() || retVal)
		szError = g_lError;

	return !retVal;
}

static void pythoncore_destroy_interpreter(const QString & szContextName)
{
	const auto i = g_Interpreters.find(szContextName);

	if(i != g_Interpreters.end())
		g_Interpreters.erase(i);
}

#endif // COMPILE_PYTHON_SUPPORT

template <typename T>
T * castFromModParam(void * p)
{
	T * ex = static_cast<T *>(p);

	if(ex->uSize != sizeof(T))
		return nullptr;

	return ex;
}

static bool pythoncore_module_ctrl(KviModule *, const char * cmd, void * param)
{
#ifdef COMPILE_PYTHON_SUPPORT
	if(!strcmp(cmd, KVI_PYTHONCORECTRLCOMMAND_EXECUTE))
	{
		auto * pex = castFromModParam<KviPythonCoreCtrlCommand_execute>(param);

		if(!pex)
			return false;

		auto & ex = *pex;

		g_pCurrentKvsContext = ex.pKvsContext;
		g_bExecuteQuiet = ex.bQuiet;
		if(ex.szContext.isEmpty())
		{
			KviPythonInterpreter m;
			ex.bExitOk = m.execute(ex.szCode, ex.lArgs, ex.szRetVal, ex.szError, ex.lWarnings);
		}
		else
		{
			KviPythonInterpreter & m = g_Interpreters[ex.szContext];
			ex.bExitOk = m.execute(ex.szCode, ex.lArgs, ex.szRetVal, ex.szError, ex.lWarnings);
		}
		return true;
	}
	if(!strcmp(cmd, KVI_PYTHONCORECTRLCOMMAND_DESTROY))
	{
		auto * pde = castFromModParam<KviPythonCoreCtrlCommand_destroy>(param);

		if(!pde)
			return false;

		pythoncore_destroy_interpreter(pde->szContext);
		return true;
	}
#endif // COMPILE_PYTHON_SUPPORT
	return false;
}

static bool pythoncore_module_init(KviModule *)
{
#ifdef COMPILE_PYTHON_SUPPORT
	Py_Initialize();
	PyEval_InitThreads();

	mainThreadState = PyEval_SaveThread();

	if(!g_Interpreters.empty())
		qDebug("libkvipythoncore: init(): Called init twice??");

	return true;
#endif // COMPILE_PYTHON_SUPPORT
	return false;
}

static bool pythoncore_module_cleanup(KviModule *)
{
#ifdef COMPILE_PYTHON_SUPPORT
	g_Interpreters.clear();
	PyEval_RestoreThread(mainThreadState);
	Py_Finalize();
#endif // COMPILE_PYTHON_SUPPORT
	return true;
}

static bool pythoncore_module_can_unload(KviModule *)
{
#ifdef COMPILE_PYTHON_SUPPORT
	return g_Interpreters.empty();
#endif // COMPILE_PYTHON_SUPPORT
	return true;
}

KVIRC_MODULE(
    "PythonCore", // module name
    "4.0.0",      // module version
    "Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)\n"
    "Copyright (C) 2009 Fabio Bas (ctrlaltca at libero dot it)\n"
    "Copyright (C) 2016 Matt Ullman (staticfox at staticfox dot net)",
    "Python Scripting Engine Core",
    pythoncore_module_init,
    pythoncore_module_can_unload,
    pythoncore_module_ctrl,
    pythoncore_module_cleanup,
    "python")
