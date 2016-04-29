#ifndef _KVI_HEAPOBJECT_H_
#define _KVI_HEAPOBJECT_H_
//=============================================================================
//
//   File : KviHeapObject.h
//   Creation date : Wed 24 Mar 2004 04:45:17 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2010 Szymon Stefanek <pragma at kvirc dot net>
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
* \file KviHeapObject.h
* \author Szymon Stefanek
* \brief Heap Object
*/

#include "kvi_settings.h"

// See KviHeapObject.cpp for comments on this class

#ifdef COMPILE_ON_WINDOWS
/**
	* \class KviHeapObject
	*
	* On windows we need to override new and delete operators
	* to ensure that always the right new/delete pair is called for an
	* object instance
	* This bug jumps out because windows uses a local heap for each
	* executable module (exe or dll).
	* (this is a well known bug described in Q122675 of MSDN)
	*
	* on Linux it is not needed: there is a single global heap
	*
	* 05.02.2005 : scalar/vector deleting destructors in modules
	*
	* There are also other issues involving the MSVC compiler.
	* When the operator new is called on an object with a virtual
	* destructor the compiler generates a helper function
	* called "vector deleting destructor" that is used to both
	* free the object's memory and call the object's destructor.
	* (In fact there is also a "scalar deleting destructor" but
	* MSVC seems to call the vector version also for scalar deletes ?!?)
	* The problem arises when operator new is called in a module:
	* the helper function gets stuffed in one of the module's sections
	* and when the module is unloaded any attempt to delete
	* the object will simply jump into no man's land.
	*
	* An "unhandled exception" in a "call [%eax]" corresponding
	* to a delete <pointer> may be a symptom of this problem.
	*
	* I haven't been able to find a solution nicer than having
	* a static allocation function in each class that can be
	* created from inside a module and destroyed anywhere else
	* and has a virtual destructor.
	*/
class KVILIB_API KviHeapObject
{
public:
	/**
		* \brief Overloading of the new operator
		*
		* \param size_t size in bytes of the memory that has to be allocated
		* \return void *
		*/
	void * operator new(size_t uSize);

	/**
		* \brief Overloading of the delete operator
		*
		* \param pData pointer to the objet that needs to be freed
		* \return void
		*/
	void operator delete(void * pData);

	/**
		* \brief Overloading of the new[] operator
		*
		* \param size_t size in bytes of the memory that has to be allocated
		* \return void *
		*/
	void * operator new[](size_t uSize);

	/**
		* \brief Overloading of the delete[] operator
		*
		* \param pData pointer to the objet that needs to be freed
		* \return void
		*/
	void operator delete[](void * pData);

	/**
		* \brief Overloading of the new operator (debug version)
		*
		* \param size_t size in bytes of the memory that has to be allocated
		* \return void *
		*/
	void * operator new(size_t uSize, const char *, int);

	/**
		* \brief Overloading of the delete operator (debug version)
		*
		* \param pData pointer to the objet that needs to be freed
		* \return void
		*/
	void operator delete(void * pData, const char *, int);
};
#else  //COMPILE_ON_WINDOWS
class KVILIB_API KviHeapObject
{
	// on other platforms this crap is not necessary
};
#endif //COMPILE_ON_WINDOWS

#endif //_KVI_HEAPOBJECT_H_
