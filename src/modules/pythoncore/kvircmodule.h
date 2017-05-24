#ifndef Py_KVIRCMODULE_H
#define Py_KVIRCMODULE_H
//=============================================================================
//
//   File : kvircmodule.h
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

/**
* \file kvircmodule.h
* \author Elvio Basello
* \brief KVIrc module for Python
*
* This python module defines some functions to interact with KVIrc and run its
* builtin functions. It's useful when writing Python scripts inside KVIrc.
*/

#include "kvi_settings.h"

#ifdef COMPILE_PYTHON_SUPPORT

#include "pythonheaderwrapper.h"

// python included like to pollute the global namespace, let's #undef some beef
#undef isspace
#undef isupper
#undef islower
#undef isalpha
#undef isalnum
#undef toupper
#undef tolower

// Prototype
PyMODINIT_FUNC python_init();

#ifdef __cplusplus
extern "C" {
#endif
#ifndef KVIRC_MODULE
// This section is used in modules that use kvircmodule's API
static void ** PyKVIrc_API;

#define PyKVIrc_echo \
	(*(int (*)(const char * pcCmd))PyKVIrc_API[0])
#define PyKVIrc_say \
	(*(int (*)(const char * pcCmd))PyKVIrc_API[1])
#define PyKVIrc_warning \
	(*(int (*)(const char * pcCmd))PyKVIrc_API[2])
#define PyKVIrc_getLocal \
	(*(int (*)(const char * pcCmd))PyKVIrc_API[3])
#define PyKVIrc_setLocal \
	(*(int (*)(const char * pcCmd))PyKVIrc_API[4])
#define PyKVIrc_getGlobal \
	(*(int (*)(const char * pcCmd))PyKVIrc_API[5])
#define PyKVIrc_setGlobal \
	(*(int (*)(const char * pcCmd))PyKVIrc_API[6])
#define PyKVIrc_eval \
	(*(int (*)(const char * pcCmd))PyKVIrc_API[7])
#define PyKVIrc_internalWarning \
	(*(int (*)(const char * pcCmd))PyKVIrc_API[8])
#define PyKVIrc_error \
	(*(int (*)(const char * pcCmd))PyKVIrc_API[9])
/**
		* \brief Returns 0 on success, -1 otherwise
		*
		* In case on unsuccess, it sets the exception
		* \return int
		*/
inline static int import_kvirc()
{
	PyObject * pModule = PyImport_ImportModule("kvirc");
	if(pModule)
	{
		PyObject * pC_API_Object = PyObject_GetAttrString(pModule, "_C_API");

		if(!pC_API_Object)
			return -1;

		if(PyCObject_Check(pC_API_Object))
			PyKVIrc_API = (void **)PyCObject_AsVoidPtr(pC_API_Object);

		Py_DECREF(pC_API_Object);
	}
	return 0;
}
#endif // KVIRC_MODULE

#ifdef __cplusplus
}
#endif

#endif // COMPILE_PYTHON_SUPPORT

#endif // Py_KVIRCMODULE_H
