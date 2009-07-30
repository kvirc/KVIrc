//=============================================================================
//
//   File : kvircmodule.cpp
//   Creation date : Wed Nov 19 19:11:29 2008 GMT by Elvio Basello
//
//   This file is part of the KVirc irc client distribution
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#include "kvi_settings.h"

#ifdef COMPILE_PYTHON_SUPPORT

#include <Python.h>

#define KVIRC_MODULE

#include "kvircmodule.h"
#include "kvi_window.h"
#include "kvi_userinput.h"
#include "kvi_kvs_runtimecontext.h"
#include "kvi_kvs_script.h"


static KviKvsRunTimeContext * g_pCurrentKvsContext = 0;
static KviStr g_szLastReturnValue("");

/*
static int PyKVIrc_Echo(const char * pcCmd)
{
	return system(pcCmd);
}
*/

static PyObject * PyKVIrc_echo(PyObject * pSelf, PyObject * pArgs)
{
	const char * pcText;
	int iColorSet;
	const char * pcWinId;

	if(!PyArg_ParseTuple(pArgs,"s|is",&pcText,&iColorSet,&pcWinId))
		return 0;

	if(g_pCurrentKvsContext && pcText)
	{
		KviWindow * pWnd;
		if(pcWinId)
		{
			pWnd = g_pApp->findWindow(pcWinId);
			if(!pWnd)
				pWnd = g_pCurrentKvsContext->window();
		} else {
			pWnd = g_pCurrentKvsContext->window();
		}
		pWnd->outputNoFmt(iColorSet,QString::fromUtf8(pcText));
	}

	//sts = PyKVIrc_System(command);
	//return Py_BuildValue("i", sts);
	return Py_BuildValue("i",1);
}

static PyObject * PyKVIrc_say(PyObject * pSelf, PyObject * pArgs)
{
	const char * pcText;
	const char * pcWinId;

	if(!PyArg_ParseTuple(pArgs,"s|s",&pcText,&pcWinId))
		return 0;

	if(g_pCurrentKvsContext && pcText)
	{
		KviWindow * pWnd;
		if(pcWinId)
		{
			pWnd = g_pApp->findWindow(pcWinId);
			if(!pWnd)
				pWnd = g_pCurrentKvsContext->window();
		} else {
			pWnd = g_pCurrentKvsContext->window();
		}
		QString szTmp = QString::fromUtf8(pcText);
		KviUserInput::parse(szTmp,pWnd);
	}

	return Py_BuildValue("i",1);
}

static PyObject * PyKVIrc_warning(PyObject * pSelf, PyObject * pArgs)
{
}

static PyObject * PyKVIrc_getLocal(PyObject * pSelf, PyObject * pArgs)
{
}

static PyObject * PyKVIrc_setLocal(PyObject * pSelf, PyObject * pArgs)
{
}

static PyObject * PyKVIrc_getGlobal(PyObject * pSelf, PyObject * pArgs)
{
}

static PyObject * PyKVIrc_setGlobal(PyObject * pSelf, PyObject * pArgs)
{
}

static PyObject * PyKVIrc_eval(PyObject * pSelf, PyObject * pArgs)
{
	const char * pcCode;
	char * pcRetVal;

	if(!PyArg_ParseTuple(pArgs,"s",&pcCode))
		return 0;

	if(g_pCurrentKvsContext && pcCode)
	{
		KviKvsVariant ret;
		if(KviKvsScript::run(QString::fromUtf8(pcCode),g_pCurrentKvsContext->window(),0,&ret))
		{
			QString szTmp;
			ret.asString(szTmp);
			g_szLastReturnValue = szTmp;
		} else {
			g_szLastReturnValue = "";
		}
		pcRetVal = g_szLastReturnValue.ptr();
	} else {
		pcRetVal = "";
	}

	return Py_BuildValue("s",pcRetVal);
}

static PyObject * PyKVIrc_internalWarning(PyObject * pSelf, PyObject * pArgs)
{
}

static PyMethodDef KVIrcMethods[] = {
	{ "KVIrc::echo", PyKVIrc_echo, METH_VARARGS,
		"Outputs a text to a KVIrc window" },
	{ "KVIrc::say", PyKVIrc_say, METH_VARARGS,
		"Types text in a window" },
	{ "KVIrc::warning", PyKVIrc_warning, METH_VARARGS,
		"Prints a warning message" },
	{ "KVIrc::getLocal", PyKVIrc_getLocal, METH_VARARGS,
		"Gets a local variable" },
	{ "KVIrc::setLocal", PyKVIrc_setLocal, METH_VARARGS,
		"Sets a local variable" },
	{ "KVIrc::getGlobal", PyKVIrc_getGlobal, METH_VARARGS,
		"Gets a global variable" },
	{ "KVIrc::setGlobal", PyKVIrc_setGlobal, METH_VARARGS,
		"Sets a global variable" },
	{ "KVIrc::eval", PyKVIrc_eval, METH_VARARGS,
		"Change the behaviour of a set of commands" },
	{ "KVIrc::internalWarning", PyKVIrc_internalWarning, METH_VARARGS,
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

	// Create a CObject containing the API pointer array's address
	pC_API_Object = PyCObject_FromVoidPtr((void *)PyKVIrc_API,NULL);

	if(pC_API_Object)
		PyModule_AddObject(pModule,"_C_API",pC_API_Object);
}

#endif
