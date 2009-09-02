//=============================================================================
//
//   File : libkvipythoncore.cpp
//   Creation date : Fri Nov 07 00:18:31 2008 GMT by Elvio Basello
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
#include "kvi_module.h"
#include "kvi_locale.h"
//#include "kvi_modulemanager.h"
//#include "kvi_fileutils.h"
//#include "kvi_app.h"
//#include "kvi_options.h"
//#include "kvi_out.h"

#ifdef COMPILE_PYTHON_SUPPORT
	#include "kvircmodule.h"
	#include "pythoncoreinterface.h"
	#include <Python.h>

/*
	class KviPythonInterpreter
	{
	public:
		KviPythonInterpreter(const QString & szContextName);
		~KviPythonInterpreter();
	protected:
		QString           m_szContextName;
		//PerlInterpreter * m_pInterpreter;
	public:
		bool init(); // if this fails then well.. :D
		void done();
		bool execute(const QString & szCode, QStringList & lArgs, QString & szRetVal, QString & szError, QStringList & lWarnings);
		const QString & contextName(){ return m_szContextName; };
	//protected:
	//	QString svToQString(SV * sv);
	};

	KviPythonInterpreter::KviPythonInterpreter(const QString & szContextName)
	{
		m_szContextName = szContextName;
		//m_pInterpreter = 0;
	}

	KviPythonInterpreter::~KviPythonInterpreter()
	{
		done();
	}
*/
#endif // COMPILE_PYTHON_SUPPORT

static bool pythoncore_module_init(KviModule *)
{
#ifdef COMPILE_PYTHON_SUPPORT

	// Initialize the Python interpreter
	debug("Initializing Python interpreter");
#ifdef COMPILE_ON_MAC
	PyMac_Initialize();
#else
	Py_Initialize();
#endif

	// Initialize the Python module for KVIrc
	python_init();

	//g_pInterpreters = new KviPointerHashTable<QString,KviPerlInterpreter>(17,false);
	//g_pInterpreters->setAutoDelete(false);
	return true;

#endif // COMPILE_PYTHON_SUPPORT

	return false;
}

static bool pythoncore_module_cleanup(KviModule *)
{
#ifdef COMPILE_PYTHON_SUPPORT
	//perlcore_destroy_all_interpreters();
	//delete g_pInterpreters;
	//g_pInterpreters = 0;

	// Free the memory used by the Python interpreter
	Py_Finalize();

	// Close the intepreter
	Py_Exit(0);
#endif // COMPILE_PYTHON_SUPPORT

	return true;
}

KVIRC_MODULE(
	"PythonCore",                                                 // module name
	"4.0.0",                                                // module version
	"Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org", // author & (C)
	"Python Scripting Engine Core",
	pythoncore_module_init,
	0,
	0,
	pythoncore_module_cleanup
)
