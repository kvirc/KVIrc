//=============================================================================
//
//   File : kvircmodule.cpp
//   Creation date : Wed Nov 19 19:11:29 2008 GMT by Elvio Basello
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

#include "kvi_settings.h"

#ifdef COMPILE_PYTHON_SUPPORT

#include "pythonheaderwrapper.h"

#define KVIRC_MODULE

#include "kvircmodule.h"
#include "KviApplication.h"
#include "KviWindow.h"
#include "KviUserInput.h"
#include "KviKvsRunTimeContext.h"
#include "KviKvsScript.h"

#include <QThread>

// For compatibility with Python 3
#if PY_MAJOR_VERSION >= 3
#define KVI_PyMODINIT_RETVAL(module) module
#else
#define KVI_PyMODINIT_RETVAL(module) \
	;                                \
	(void)(module)
struct PyModuleDef
{
	char m_base;
	const char *m_name, *m_doc;
	int m_size;
	PyMethodDef * m_methods;
};
static const char PyModuleDef_HEAD_INIT = 0;
static PyObject * PyModule_Create(const struct PyModuleDef * def)
{
	return Py_InitModule3(def->m_name, def->m_methods, def->m_doc);
}
#endif

extern KviKvsRunTimeContext * g_pCurrentKvsContext;
extern bool g_bExecuteQuiet;
extern KviCString g_szLastReturnValue;
extern QStringList g_lWarningList;
extern QString g_lError;

static PyObject * PyKVIrc_echo(PyObject * /* pSelf */, PyObject * pArgs)
{
	const char *pcText = nullptr, *pcWinId = nullptr;
	KviWindow * pWnd = nullptr;
	int iColorSet = 0;

	if(QThread::currentThread() != g_pApp->thread())
	{
		qDebug("[pythoncore][ERROR] KVIrc module functions must be called from the main KVIrc thread");
		return nullptr; // Sorry, we're NOT thread safe
	}

	if(!PyArg_ParseTuple(pArgs, "s|is", &pcText, &iColorSet, &pcWinId))
		return nullptr;

	if(pcText)
	{
		if(pcWinId)
			pWnd = g_pApp->findWindow(pcWinId);

		if(!pWnd)
		{
			if(g_pCurrentKvsContext)
				pWnd = g_pCurrentKvsContext->window();
			else if(g_pActiveWindow)
				pWnd = g_pActiveWindow;
			else
				pWnd = (KviWindow *)g_pApp->activeConsole();
		}

		if(pWnd)
			pWnd->outputNoFmt(iColorSet, QString::fromUtf8(pcText));
	}

	return Py_BuildValue("i", 1);
}

static PyObject * PyKVIrc_say(PyObject * /* pSelf */, PyObject * pArgs)
{
	const char *pcText = nullptr, *pcWinId = nullptr;
	KviWindow * pWnd = nullptr;

	if(QThread::currentThread() != g_pApp->thread())
	{
		qDebug("[pythoncore][ERROR] KVIrc module functions must be called from the main KVIrc thread");
		return nullptr; // Sorry, we're NOT thread safe
	}

	if(!PyArg_ParseTuple(pArgs, "s|s", &pcText, &pcWinId))
		return nullptr;

	if(pcText)
	{
		if(pcWinId)
			pWnd = g_pApp->findWindow(pcWinId);

		if(!pWnd)
		{
			if(g_pCurrentKvsContext)
				pWnd = g_pCurrentKvsContext->window();
			else if(g_pActiveWindow)
				pWnd = g_pActiveWindow;
			else
				pWnd = (KviWindow *)g_pApp->activeConsole();
		}
		if(pWnd)
		{
			QString szText = QString::fromUtf8(pcText);
			KviUserInput::parse(szText, pWnd);
		}
	}

	return Py_BuildValue("i", 1);
}

static PyObject * PyKVIrc_warning(PyObject * /* pSelf */, PyObject * pArgs)
{
	const char * pcText = nullptr;

	if(QThread::currentThread() != g_pApp->thread())
	{
		qDebug("[pythoncore][ERROR] KVIrc module functions must be called from the main KVIrc thread");
		return nullptr; // Sorry, we're NOT thread safe
	}

	if(!PyArg_ParseTuple(pArgs, "s", &pcText))
		return nullptr;

	if(pcText && !g_bExecuteQuiet)
	{
		if(g_pCurrentKvsContext)
			g_pCurrentKvsContext->warning(pcText);
	}

	return Py_BuildValue("i", 1);
}

static PyObject * PyKVIrc_getLocal(PyObject * /* pSelf */, PyObject * pArgs)
{
	const char * szVarName = nullptr;
	QString tmp;

	if(QThread::currentThread() != g_pApp->thread())
	{
		qDebug("[pythoncore][ERROR] KVIrc module functions must be called from the main KVIrc thread");
		return nullptr; // Sorry, we're NOT thread safe
	}

	if(!PyArg_ParseTuple(pArgs, "s", &szVarName))
		return nullptr;

	if(g_pCurrentKvsContext)
	{
		KviKvsVariant * pVar = g_pCurrentKvsContext->localVariables()->find(szVarName);
		if(pVar)
		{
			pVar->asString(tmp);
			return Py_BuildValue("s", tmp.toUtf8().data());
		}
		else
		{
			return Py_BuildValue("s", "");
		}
	}
	return nullptr;
}

static PyObject * PyKVIrc_setLocal(PyObject * /* pSelf */, PyObject * pArgs)
{
	const char *szVarName = nullptr, *szVarValue = nullptr;
	QString tmp;

	if(QThread::currentThread() != g_pApp->thread())
	{
		qDebug("[pythoncore][ERROR] KVIrc module functions must be called from the main KVIrc thread");
		return nullptr; // Sorry, we're NOT thread safe
	}

	if(!PyArg_ParseTuple(pArgs, "ss", &szVarName, &szVarValue))
		return nullptr;

	if(g_pCurrentKvsContext)
	{
		if(szVarValue && *szVarValue)
		{
			KviKvsVariant * pVar = g_pCurrentKvsContext->localVariables()->get(szVarName);
			pVar->setString(szVarValue);
		}
		else
		{
			g_pCurrentKvsContext->localVariables()->unset(szVarName);
		}
		return Py_BuildValue("i", 1);
	}
	return nullptr;
}

static PyObject * PyKVIrc_getGlobal(PyObject * /* pSelf */, PyObject * pArgs)
{
	const char * szVarName = nullptr;
	QString tmp;

	if(QThread::currentThread() != g_pApp->thread())
	{
		qDebug("[pythoncore][ERROR] KVIrc module functions must be called from the main KVIrc thread");
		return nullptr; // Sorry, we're NOT thread safe
	}

	if(!PyArg_ParseTuple(pArgs, "s", &szVarName))
		return nullptr;

	if(g_pCurrentKvsContext)
	{
		KviKvsVariant * pVar = g_pCurrentKvsContext->globalVariables()->find(szVarName);
		if(pVar)
		{
			pVar->asString(tmp);
			return Py_BuildValue("s", tmp.toUtf8().data());
		}
		else
		{
			return Py_BuildValue("s", "");
		}
	}
	return nullptr;
}

static PyObject * PyKVIrc_setGlobal(PyObject * /* pSelf */, PyObject * pArgs)
{
	const char *szVarName = nullptr, *szVarValue = nullptr;
	QString tmp;

	if(QThread::currentThread() != g_pApp->thread())
	{
		qDebug("[pythoncore][ERROR] KVIrc module functions must be called from the main KVIrc thread");
		return nullptr; // Sorry, we're NOT thread safe
	}

	if(!PyArg_ParseTuple(pArgs, "ss", &szVarName, &szVarValue))
		return nullptr;

	if(g_pCurrentKvsContext)
	{
		if(szVarValue && *szVarValue)
		{
			KviKvsVariant * pVar = g_pCurrentKvsContext->globalVariables()->get(szVarName);
			pVar->setString(szVarValue);
		}
		else
		{
			g_pCurrentKvsContext->globalVariables()->unset(szVarName);
		}
		return Py_BuildValue("i", 1);
	}
	return nullptr;
}

static PyObject * PyKVIrc_eval(PyObject * /* pSelf */, PyObject * pArgs)
{
	const char * pcCode = nullptr;
	KviWindow * pWnd = nullptr;
	char * pcRetVal = nullptr;

	if(QThread::currentThread() != g_pApp->thread())
	{
		qDebug("[pythoncore][ERROR] KVIrc module functions must be called from the main KVIrc thread");
		return nullptr; // Sorry, we're NOT thread safe
	}

	if(!PyArg_ParseTuple(pArgs, "s", &pcCode))
		return nullptr;

	if(pcCode)
	{
		if(g_pCurrentKvsContext)
			pWnd = g_pCurrentKvsContext->window();
		else if(g_pActiveWindow)
			pWnd = g_pActiveWindow;
		else
			pWnd = (KviWindow *)g_pApp->activeConsole();

		if(pWnd)
		{
			KviKvsVariant ret;
			QString szRet;
			if(KviKvsScript::run(QString::fromUtf8(pcCode), pWnd, nullptr, &ret))
			{
				ret.asString(szRet);
				pcRetVal = szRet.toUtf8().data();
			}
		}
	}

	return Py_BuildValue("s", pcRetVal);
}

static PyObject * PyKVIrc_internalWarning(PyObject * /* pSelf */, PyObject * pArgs)
{
	const char * pcText = nullptr;

	if(QThread::currentThread() != g_pApp->thread())
	{
		qDebug("[pythoncore][ERROR] KVIrc module functions must be called from the main KVIrc thread");
		return nullptr; // Sorry, we're NOT thread safe
	}

	if(!PyArg_ParseTuple(pArgs, "s", &pcText))
		return nullptr;

	if(pcText && !g_bExecuteQuiet)
		g_lWarningList.append(QString(pcText));

	return Py_BuildValue("i", 1);
}

static PyObject * PyKVIrc_error(PyObject * /* pSelf */, PyObject * pArgs)
{
	const char * pcText = nullptr;

	if(QThread::currentThread() != g_pApp->thread())
	{
		qDebug("[pythoncore][ERROR] KVIrc module functions must be called from the main KVIrc thread");
		return nullptr; // Sorry, we're NOT thread safe
	}

	if(!PyArg_ParseTuple(pArgs, "s", &pcText))
		return nullptr;

	if(pcText)
		g_lError.append(pcText);

	return Py_BuildValue("i", 1);
}

static PyMethodDef KVIrcMethods[] = {
	{ "echo", PyKVIrc_echo, METH_VARARGS,
	    "Outputs text to a KVIrc window" },
	{ "say", PyKVIrc_say, METH_VARARGS,
	    "Types text in a window" },
	{ "warning", PyKVIrc_warning, METH_VARARGS,
	    "Prints a warning message" },
	{ "getLocal", PyKVIrc_getLocal, METH_VARARGS,
	    "Gets a local variable" },
	{ "setLocal", PyKVIrc_setLocal, METH_VARARGS,
	    "Sets a local variable" },
	{ "getGlobal", PyKVIrc_getGlobal, METH_VARARGS,
	    "Gets a global variable" },
	{ "setGlobal", PyKVIrc_setGlobal, METH_VARARGS,
	    "Sets a global variable" },
	{ "eval", PyKVIrc_eval, METH_VARARGS,
	    "Change the behaviour of a set of commands" },
	{ "internalWarning", PyKVIrc_internalWarning, METH_VARARGS,
	    "" },
	{ "error", PyKVIrc_error, METH_VARARGS,
	    "" },
	{ nullptr, nullptr, 0, nullptr }
};

PyMODINIT_FUNC python_init()
{
	static PyCFunction PyKVIrc_API[] = {
		PyKVIrc_echo,
		PyKVIrc_say,
		PyKVIrc_warning,
		PyKVIrc_getLocal,
		PyKVIrc_setLocal,
		PyKVIrc_getGlobal,
		PyKVIrc_setGlobal,
		PyKVIrc_eval,
		PyKVIrc_internalWarning,
		PyKVIrc_error,
	};
	static struct PyModuleDef def = {
		PyModuleDef_HEAD_INIT,
		"kvirc",
		"KVIrc module",
		-1,
		KVIrcMethods
	};
	PyObject * pModule = PyModule_Create(&def);
	if(!pModule)
	{
		KVI_ASSERT("Python: Unable to initialize KVIrc import module");
	}
	else
	{
		// Create a CObject containing the API pointer array's address
		PyObject * pC_API_Object = PyCObject_FromVoidPtr(PyKVIrc_API, nullptr);
		if(pC_API_Object)
			PyModule_AddObject(pModule, "_C_API", pC_API_Object);
	}
	return KVI_PyMODINIT_RETVAL(pModule);
}

static struct KviPythonModuleInitializer
{
	KviPythonModuleInitializer()
	{
		PyImport_AppendInittab("kvirc", python_init);
	}
} initializer;

#endif
