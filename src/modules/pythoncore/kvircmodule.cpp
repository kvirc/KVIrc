//=============================================================================
//
//   File : kvircmodule.cpp
//   Creation date : Wed Nov 19 19:11:29 2008 GMT by Elvio Basello
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

#ifdef COMPILE_PYTHON_SUPPORT

#include <Python.h>

#define KVIRC_MODULE

#include "kvircmodule.h"
#include "KviApplication.h"
#include "KviWindow.h"
#include "KviUserInput.h"
#include "KviKvsRunTimeContext.h"
#include "KviKvsScript.h"

#include <QThread>

extern KviKvsRunTimeContext * g_pCurrentKvsContext;
extern bool g_bExecuteQuiet;
extern KviCString g_szLastReturnValue;
extern QStringList g_lWarningList;
extern QString g_lError;

static PyObject * PyKVIrc_echo(PyObject * pSelf, PyObject * pArgs)
{
	Q_UNUSED(pSelf);
	const char * pcText=0;
	int iColorSet=0;
	const char * pcWinId=0;
	KviWindow * pWnd=0;

	if(QThread::currentThread() != g_pApp->thread())
	{
		qDebug("[pythoncore][ERROR] kvirc module functions must be called from the main KVIrc thread");
		return 0; // Sorry, we're NOT thread safe
	}

	if(!PyArg_ParseTuple(pArgs,"s|is",&pcText,&iColorSet,&pcWinId))
		return 0;

	if(pcText)
	{
		if(pcWinId)
		{
			pWnd = g_pApp->findWindow(pcWinId);
		}
		if(!pWnd)
		{
			if(g_pCurrentKvsContext)
			{
				pWnd = g_pCurrentKvsContext->window();
			} else if(g_pActiveWindow) {
				pWnd = g_pActiveWindow;
			} else {
				pWnd = (KviWindow*) g_pApp->activeConsole();
			}
		}
		if(pWnd)
			pWnd->outputNoFmt(iColorSet,QString::fromUtf8(pcText));
	}

	return Py_BuildValue("i",1);
}

static PyObject * PyKVIrc_say(PyObject * pSelf, PyObject * pArgs)
{
	Q_UNUSED(pSelf);
		
	const char * pcText=0;
	const char * pcWinId=0;
	KviWindow * pWnd=0;

	if(QThread::currentThread() != g_pApp->thread())
	{
		qDebug("[pythoncore][ERROR] kvirc module functions must be called from the main KVIrc thread");
		return 0; // Sorry, we're NOT thread safe
	}

	if(!PyArg_ParseTuple(pArgs,"s|s",&pcText,&pcWinId))
		return 0;

	if(pcText)
	{
		if(pcWinId)
		{
			pWnd = g_pApp->findWindow(pcWinId);
		}
		if(!pWnd)
		{
			if(g_pCurrentKvsContext)
			{
				pWnd = g_pCurrentKvsContext->window();
			} else if(g_pActiveWindow) {
				pWnd = g_pActiveWindow;
			} else {
				pWnd = (KviWindow*) g_pApp->activeConsole();
			}
		}
		if(pWnd)
		{
			QString szText=QString::fromUtf8(pcText);
			KviUserInput::parse(szText,pWnd);
		}
	}

	return Py_BuildValue("i",1);
}

static PyObject * PyKVIrc_warning(PyObject * pSelf, PyObject * pArgs)
{
	Q_UNUSED(pSelf);
	const char * pcText=0;

	if(QThread::currentThread() != g_pApp->thread())
	{
		qDebug("[pythoncore][ERROR] kvirc module functions must be called from the main KVIrc thread");
		return 0; // Sorry, we're NOT thread safe
	}

	if(!PyArg_ParseTuple(pArgs,"s",&pcText))
		return 0;

	if(pcText && !g_bExecuteQuiet)
	{
		if(g_pCurrentKvsContext)
			g_pCurrentKvsContext->warning(pcText);
	}

	return Py_BuildValue("i",1);
}

static PyObject * PyKVIrc_getLocal(PyObject * pSelf, PyObject * pArgs)
{
	Q_UNUSED(pSelf);

	QString tmp;
	const char * szVarName=0;

	if(QThread::currentThread() != g_pApp->thread())
	{
		qDebug("[pythoncore][ERROR] kvirc module functions must be called from the main KVIrc thread");
		return 0; // Sorry, we're NOT thread safe
	}


	if(!PyArg_ParseTuple(pArgs,"s",&szVarName))
		return 0;
	
	if(g_pCurrentKvsContext)
	{
		KviKvsVariant * pVar = g_pCurrentKvsContext->localVariables()->find(szVarName);
		if(pVar)
		{
			pVar->asString(tmp);
			return Py_BuildValue("s",tmp.toUtf8().data());
		} else return Py_BuildValue("s","");
	}
	return 0;
}

static PyObject * PyKVIrc_setLocal(PyObject * pSelf, PyObject * pArgs)
{
	Q_UNUSED(pSelf);

	QString tmp;
	const char * szVarName=0;
	const char * szVarValue=0;

	if(QThread::currentThread() != g_pApp->thread())
	{
		qDebug("[pythoncore][ERROR] kvirc module functions must be called from the main KVIrc thread");
		return 0; // Sorry, we're NOT thread safe
	}

	if(!PyArg_ParseTuple(pArgs,"ss",&szVarName, &szVarValue))
		return 0;

	if(g_pCurrentKvsContext)
	{
		if(szVarValue && *szVarValue)
		{
			KviKvsVariant * pVar = g_pCurrentKvsContext->localVariables()->get(szVarName);
			pVar->setString(szVarValue);
		} else {
			g_pCurrentKvsContext->localVariables()->unset(szVarName);
		}
		return Py_BuildValue("i",1);
	}
	return 0;
}

static PyObject * PyKVIrc_getGlobal(PyObject * pSelf, PyObject * pArgs)
{
	Q_UNUSED(pSelf);

	QString tmp;
	const char * szVarName=0;

	if(QThread::currentThread() != g_pApp->thread())
	{
		qDebug("[pythoncore][ERROR] kvirc module functions must be called from the main KVIrc thread");
		return 0; // Sorry, we're NOT thread safe
	}

	if(!PyArg_ParseTuple(pArgs,"s",&szVarName))
		return 0;

	if(g_pCurrentKvsContext)
	{
		KviKvsVariant * pVar = g_pCurrentKvsContext->globalVariables()->find(szVarName);
		if(pVar)
		{
			pVar->asString(tmp);
			return Py_BuildValue("s",tmp.toUtf8().data());
		} else return Py_BuildValue("s","");
	}
	return 0;
}

static PyObject * PyKVIrc_setGlobal(PyObject * pSelf, PyObject * pArgs)
{
	Q_UNUSED(pSelf);

	QString tmp;
	const char * szVarName=0;
	const char * szVarValue=0;

	if(QThread::currentThread() != g_pApp->thread())
	{
		qDebug("[pythoncore][ERROR] kvirc module functions must be called from the main KVIrc thread");
		return 0; // Sorry, we're NOT thread safe
	}


	if(!PyArg_ParseTuple(pArgs,"ss",&szVarName, &szVarValue))
		return 0;

	if(g_pCurrentKvsContext)
	{
		if(szVarValue && *szVarValue)
		{
			KviKvsVariant * pVar = g_pCurrentKvsContext->globalVariables()->get(szVarName);
			pVar->setString(szVarValue);
		} else {
			g_pCurrentKvsContext->globalVariables()->unset(szVarName);
		}
		return Py_BuildValue("i",1);
	}
	return 0;
}

static PyObject * PyKVIrc_eval(PyObject * pSelf, PyObject * pArgs)
{
	Q_UNUSED(pSelf);
	const char * pcCode=0;
	char * pcRetVal=0;
	KviWindow * pWnd=0;

	if(QThread::currentThread() != g_pApp->thread())
	{
		qDebug("[pythoncore][ERROR] kvirc module functions must be called from the main KVIrc thread");
		return 0; // Sorry, we're NOT thread safe
	}

	if(!PyArg_ParseTuple(pArgs,"s",&pcCode))
		return 0;

	if(pcCode)
	{
		if(g_pCurrentKvsContext)
		{
			pWnd = g_pCurrentKvsContext->window();
		} else if(g_pActiveWindow) {
			pWnd = g_pActiveWindow;
		} else {
			pWnd = (KviWindow*) g_pApp->activeConsole();
		}
		if(pWnd)
		{
			KviKvsVariant ret;
			QString szRet;
			if(KviKvsScript::run(QString::fromUtf8(pcCode),pWnd,0,&ret))
			{
				ret.asString(szRet);
				pcRetVal = szRet.toUtf8().data();
			}
		}
	}

	return Py_BuildValue("s",pcRetVal);
}

static PyObject * PyKVIrc_internalWarning(PyObject * pSelf, PyObject * pArgs)
{
	Q_UNUSED(pSelf);
	const char * pcText=0;

	if(QThread::currentThread() != g_pApp->thread())
	{
		qDebug("[pythoncore][ERROR] kvirc module functions must be called from the main KVIrc thread");
		return 0; // Sorry, we're NOT thread safe
	}

	if(!PyArg_ParseTuple(pArgs,"s",&pcText))
		return 0;

	if(pcText && !g_bExecuteQuiet)
	{
		g_lWarningList.append(QString(pcText));
	}

	return Py_BuildValue("i",1);
}

static PyObject * PyKVIrc_error(PyObject * pSelf, PyObject * pArgs)
{
	Q_UNUSED(pSelf);
	const char * pcText=0;

	if(QThread::currentThread() != g_pApp->thread())
	{
		qDebug("[pythoncore][ERROR] kvirc module functions must be called from the main KVIrc thread");
		return 0; // Sorry, we're NOT thread safe
	}

	if(!PyArg_ParseTuple(pArgs,"s",&pcText))
		return 0;

	if(pcText)
	{
		g_lError.append(pcText);
	}

	return Py_BuildValue("i",1);
}

static PyMethodDef KVIrcMethods[] = {
	{ "echo", PyKVIrc_echo, METH_VARARGS,
		"Outputs a text to a KVIrc window" },
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
	{ NULL, NULL, 0, NULL }
};

PyMODINIT_FUNC python_init()
{
	static void * PyKVIrc_API[PyKVIrc_API_NUM];
	PyObject * pModule;
	PyObject * pC_API_Object;

	pModule = Py_InitModule("kvirc",KVIrcMethods);
	if(!pModule) return;

	// Initialize the C API pointer array
	PyKVIrc_API[0] = (void *)PyKVIrc_echo;
	PyKVIrc_API[1] = (void *)PyKVIrc_say;
	PyKVIrc_API[2] = (void *)PyKVIrc_warning;
	PyKVIrc_API[3] = (void *)PyKVIrc_getLocal;
	PyKVIrc_API[4] = (void *)PyKVIrc_setLocal;
	PyKVIrc_API[5] = (void *)PyKVIrc_getGlobal;
	PyKVIrc_API[6] = (void *)PyKVIrc_setGlobal;
	PyKVIrc_API[7] = (void *)PyKVIrc_eval;
	PyKVIrc_API[8] = (void *)PyKVIrc_internalWarning;
	PyKVIrc_API[9] = (void *)PyKVIrc_error;
	
	// Create a CObject containing the API pointer array's address
	pC_API_Object = PyCObject_FromVoidPtr((void *)PyKVIrc_API,NULL);

	if(pC_API_Object)
		PyModule_AddObject(pModule,"_C_API",pC_API_Object);
}

#endif
